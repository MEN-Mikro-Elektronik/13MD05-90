/****************************************************************************
 ************                                                    ************
 ************    H W B U G                                       ************
 ************                                                    ************
 ****************************************************************************
 *
 *      $Author: rt $
 *        $Date: 2010/09/24 17:02:30 $
 *    $Revision: 1.5 $
 *
 *  Description: Simple Debugger for Hardware Development Purpose
 *
 *
 *     Required: uti.l (under OS-9)
 *     Switches: none
 *
 *-------------------------------[ Functions ]-------------------------------
 *
 *  void add_command( command, func, helpline, repeat )
 *  clearline( n )
 *  char *get_line(prompt)
 *  help_screen()
 *  quit()
 *  what()
 *  main(argc,argv)
 *  int make_hex( argp, hexval )
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1993-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
 /*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/hwbug.c,v 1.5 2010/09/24 17:02:30 rt Exp $";

#include "hwbug.h"
#include <MEN/uti.h>

#define MAX_LINELENGTH 	100		/* length of command line */
#define MAX_ARGS 		10		/* max. number of args in cmd line */
#define CTRL(c) ((c)-'@')

extern int errno;
char *m_prompt = "HwBug> ";
int exit_hwbug = 0;
int control_c = 0;

u_int8 G_iomapped = 0;

static char
	*h_h  = "                            Display help",
	*h_q  = "                            Quit",
	*h_c  = "[B|W|L|N|#] <adr>           Change Memory \n\
                              (B)yte (W)ord (L)ong (N)oRead (#)Increm.",
	*h_d  = "[B|W|L] [<adr> <cnt>]       Display Memory \n\
                              (B)yte (W)ord (L)ong",
	*h_f  = "[B|W|L]  <adr> <cnt> <val>  Fill Memory with fixed Values\n\
                              (B)yte (W)ord (L)ong\n\
 F[B|W|L]P <adr> <cnt>        Fill Memory with linear Pattern\n\
                              (B)yte (W)ord (L)ong",
	*h_i = "                             perform io_mapped accesses\n\
	                           when supported",
	*h_m = "                             perform mem_mapped accesses";

/*----------------------------------------------------------------------
 * command descriptor structure
 */
typedef struct _cmd_desc{
	struct  _cmd_desc *next;
	char 	*command;			/* command name */
	char 	*helpline;			/* help line */
	int		repeat;				/* repeat if empty line entered? */
	int		(*func)();			/* function to call */
} cmd_desc;

static cmd_desc *cmd_head = 0L;	/* command list header */

/*----------------------------------------------------------------------
 * add_command - add a command to the command list
 */
//void add_command(char *command, int (*func)(), char *helpline, int repeat )
void add_command(char *command, void *func, char *helpline, int repeat )
{
/* 	;           /\* command name *\/ */
/* 	;          /\* function to call *\/ */
/* 	char    *helpline;          /\* help line *\/ */
/* int     repeat;             /\* repeat if empty line entered? *\/ */

	/* extern char *malloc(); */
	cmd_desc *cmd;

	if( !(cmd = (cmd_desc *)malloc(sizeof(*cmd))))
		return;

	cmd->command 	= command;
	cmd->func 		= func;
	cmd->helpline	= helpline;
	cmd->repeat		= repeat;
	cmd->next		= 0L;

	if( cmd_head == 0L )
		cmd_head = cmd;
	else {
		cmd_desc *c = cmd_head;

		while( c->next )
			c = c->next;
		c->next = cmd;
	}
}

/*----------------------------------------------------------------------
 * clearline - clear line with backspace
 */
void clearline( int n )
{
	while( n-- ) printf("\b \b");
}

/*----------------------------------------------------------------------
 * get_line - get and edit input line
 */
char *get_line(prompt)
char *prompt;
{
	extern char *hist_next(), *hist_prev();

	static char linebuf[MAX_LINELENGTH+1];
	char *cursor;
	char *hist_cmd;
	int c;
	int esc_seq = 0;

	printf("%s", prompt );
	FLUSH;

	cursor = linebuf;
	do {
		*cursor = '\0';
		c = os_get_char();

		if( c == -1 ){
/*			printf("\n***Read I/O error %d\n", errno ); */
			cursor = linebuf;
			continue;
		}
		
		if( c==27 || c=='[' ) {		/* ESC sequence */
			if( c==27 ) esc_seq = 1;
			if( c=='[' && esc_seq == 1 ) esc_seq=2;
		}
		else {
			if( esc_seq == 2 ) {
				if( c=='A' ) c = CTRL('W');			/* up=prev */
				else if( c=='B' ) c = CTRL('Z');	/* down=next */
				else c = 0;							/* ignore */
			}
			if( c==127 ) c=CTRL('H');				/* backspace */
			if( c=='~' ) c=CTRL('X');				/* del=clear line */
			
			switch( c ){
			case CTRL('W'):					/* previous command from hist. */
			case CTRL('Z'):					/* next command from hist */
				if( c == CTRL('W') ) {
					hist_cmd = hist_prev();
					if( !hist_cmd ) break;
				}
				else {
					hist_cmd = hist_next();
				}
	
				clearline( cursor-linebuf );
				if( hist_cmd ){
					strcpy( linebuf, hist_cmd );
					cursor = linebuf+strlen(linebuf);
	
					printf( "%s", linebuf );
				}
				else {
					*linebuf = '\0';
					cursor = linebuf;
				}
				break;
	
			case CTRL('H'):					/* backspace */
				if( cursor != linebuf ){
					*cursor-- = '\0';
	
					printf(	"\b \b" );
				}
				break;
			case CTRL('X'):					/* clear line */
				clearline( cursor-linebuf );
				*linebuf = '\0';
				cursor = linebuf;
				break;
	
			default:
				if( cursor != &linebuf[MAX_LINELENGTH] ){
					if( c >= 0x20 ){
						c = toupper(c);
						*cursor++ = c;
						printf("%c", c );
					}
				}
				break;
			}
			esc_seq = 0;
		}
		FLUSH;
	} while ( c != NL );

	hist_save(linebuf);
	return linebuf;
}

int help_screen()
{
	cmd_desc *c = cmd_head;

	while( c ){
		printf("%2s%s\n", c->command, c->helpline);
		c = c->next;
	}
	printf("\n");
	printf("Line Editing Features:\n");
	printf(" ^W = up history       ^Z = down history\n");
	printf(" ^H = delete char      ^X = kill line\n");
	printf("\n");
	printf("Version:\n");
	printf(" %s\n", &RCSid[9]);
	
	return 0;
}

int quit()
{
	exit_hwbug = 1;

	return 0;
}

void what()
{
	printf("what? (Press 'H' for help-screen)\n");
}

int access_io_mapped()
{
	int32 error = 0;
	if( (error = os_init_io_mapped()) < 0)
		printf("IO-Mapped access not supported or initialization failed\n");
	
	return error;
}

int access_mem_mapped()
{
	G_iomapped = 0;
	
	return 0;
}

int main(int argc, char **argv)
{
	char *line;
	cmd_desc *c = NULL, *last_cmd = 0L;
    /* include ident string, suppress warning */
    char a = a = RCSid[0];
    int cmd_line_mode = 1;
    int error = 0;

	/* extern int change_data(), fill_data(), display_data(); */

	char *c_argv_alloc[MAX_ARGS+1];
	char **c_argv = c_argv_alloc;
	int c_argc;

	os_usage(argc,argv);
	
	/* check parameters */
	if( argc > 1 ){
		cmd_line_mode = 0;
	}

	if( cmd_line_mode ) {
		printf(",------------------------------,\n");
		printf("|   HwBug - Hardware Debugger  |\n");
		printf("|      (c) MEN 1993-2010       |\n");
		printf("|    by K.Popp, R.Seeberger    |\n");
		printf("|                              |\n");
		printf("|   Press 'H' for help-screen  |\n");
		printf(" ------------------------------'\n\n");
	}

	os_init();

	hist_init();
	
	add_command( "H", help_screen, h_h, 0 );
	add_command( "C", change_data, h_c, 0 );
	add_command( "F", fill_data, h_f, 0 );
	add_command( "D", display_data, h_d, 1 );
	add_command( "Q", quit, h_q, 0 );
	add_command( "I", access_io_mapped, h_i, 0 );
	add_command( "M", access_mem_mapped, h_m, 0 );

	while(!exit_hwbug){
		control_c = 0;
		
		if( cmd_line_mode ) {
			/*------------------+
			| get command line  |
			+------------------*/
			line = get_line(m_prompt);
	
			/*----------------------------------+
			| split command line into arguments |
			+----------------------------------*/
			c_argc = line_args( line, c_argv, MAX_ARGS, ", \t", "\n" );
		}
		else {
			/*-------------------------------+
			| command line passed from shell |
			+-------------------------------*/
			char *ptr;
			
			c_argc = argc -1;
			c_argv = &argv[1];
			ptr = c_argv[0];
			do {
				*ptr = toupper(*ptr);	/* command has to be upper case */
			} while (*(++ptr));
			
		}

		printf("\n");

		/*--- repeat command if required ---*/
		if( c_argc == 0 ){
			if( last_cmd && last_cmd->repeat){
				(*c->func)( c_argc, c_argv );
			}
			goto next;
		}
		/*------------------+
		| detect command    |
		+------------------*/
		c = cmd_head;
		while(c){
			if( !strncmp( c->command, c_argv[0], strlen(c->command))){
				/*--- found command ---*/
				last_cmd = c;
				if( (*c->func)( c_argc, c_argv ) != 0 )
					what();
				break;
			}
			c = c->next;
		}
		if(!c) {
			what();
			error++;
		}
next:
		if(!cmd_line_mode) {
			break;
		}
		c_argc = 0;
		/* free_args( c_argv ); */
	}
	
	os_exit();
	
	return cmd_line_mode ? 0 : error;
}


int make_hex( argp, hexval )
char *argp;
u_int32 *hexval;
{
	if( sscanf( argp, "%lx", hexval ) != 1 )
		return -1;
	return 0;
}

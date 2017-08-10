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
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: hwbug.c,v $
 * Revision 1.5  2010/09/24 17:02:30  rt
 * R: 1) Option to pass commands from shell added (e.g. for scripts).
 *    2) Line editing features (history...) improved.
 * M: 1) main() changed.
 *    2) get_line() changed.
 *
 * Revision 1.4  2009/02/27 19:43:31  rt
 * R:1. Don't say "what?" every time.
 * M:1. Added return value to help_screen(), quit(), access_io_mapped()...
 *
 * Revision 1.3  2008/09/15 13:15:22  GLeonhardt
 * Avoid warnings
 *
 * Revision 1.2  2004/11/30 17:28:36  cs
 * Added support for IO-mapped access (for LINUX only)
 *
 * Revision 1.1  2003/05/26 10:06:00  ub
 * Initial Revision
 *
 * Revision 1.9  1997/01/22 10:02:17  kp
 * V1.7: Bug fix in display.c if no arguments given
 * Bug fix in os9.c (stackframe size for 68040)
 *
 * Revision 1.8  1996/12/20 16:07:02  see
 * fill byte/word/long commands added
 * fill pattern added
 * version 1.6
 *
 * Revision 1.7  1996/08/15 13:23:33  see
 * dump byte/word/long commands added
 * version 1.5
 *
 * Revision 1.6  95/06/12  17:20:15  17:20:15  see (Roland Seeberger)
 * bug fixed, call os_init before the add_command's to accept the DI
 * logo changed
 *
 * Revision 1.5  95/06/09  12:15:49  12:15:49  see (Roland Seeberger)
 * call os_usage() before printing logo
 * version 1.4
 *
 * Revision 1.4  93/08/11  08:19:38  kp
 * Version 1.3: bug fix in display.c (chars 0x7f and 0xff)
 *              bug fix in os9.c (^C Handling in running programs)
 *
 * Revision 1.3  93/05/26  14:49:49  kp
 * Version 1.2: bug fix in os_init (SSM was already assumed to be installed)
 *
 * Revision 1.2  93/05/10  10:47:38  kp
 * Version 1.1: support for MC683xx.
 *
 * Revision 1.1  93/05/04  11:26:27  kp
 * Initial revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1993 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

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
void clearline( n )
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

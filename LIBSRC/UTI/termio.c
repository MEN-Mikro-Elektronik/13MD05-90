/*****************************************************************************

Project...........: io.lib
Filename..........: termio.c 
Author............: kp
Creation Date.....: 15.10.90

Description.......: 

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |          | First Installation                           		|        *
* 02 | 10.06.91 | added cursor-keys/semigraphic						| kp	 *
*	 |			| changed from unbuffered to buffered I/O			|		 *
* 03 | 10.07.95	| cursor_on/off added                               | see    *
* 04 | 04.04.97	| clear_to_eol added                                | see    *
* 05 | 07.04.97	| get_screen_size added                             | see    *
******************************************************************************/

#include <stdio.h>

#ifdef OS9
# include <sgstat.h>
# include <termcap.h>
#else
extern char PC_, *BC, *UP, *tgetstr(), *tgoto();
extern short ospeed;
extern int tgetnum(), tgetflag(), tgetent();
extern /*void*/ tputs();
#endif



#define TCAPSLEN 400

extern char *getenv();
extern int errno;

short term_lines, term_colms;

static char tcapbuf[TCAPSLEN];
typedef char *char_p;

char PC_;			/* pad character 							*/
short ospeed;		/* terminal speed							*/
char 		*BC,	/* backspace character string 				*/
			*BL,	/* bell										*/
 			*UP;	/* up cursor								*/
			
char 		*CL,	/* clear screen								*/
			*CM,	/* cursor motion							*/
			*CE,	/* clear to EOL								*/
			*CD,	/* clear to End of display					*/
			*SO,	/* standout begin							*/
			*SE,	/* standout end								*/
			*HO,	/* cursor home								*/
			*CR,	/* carriage return							*/
			*NL;	/* new line									*/

char_p		GA,		/* semi-graphic characters					*/
			GB,
			GC,
			GD,
			GI,
			GJ,
			GK,
			GL,
			GM,
			GN,
			GO,
			
			GS,		/* graphic charset on 						*/
			GX;		/* graphic charset off						*/
			
char_p		KU,		/* cursor key up							*/
			KD,		/* down										*/		
			KL,		/* left										*/		
			KR;		/* right									*/		

char_p		VI,		/* cursor off								*/
			VE;		/* cursor on								*/
			
static int standoutflag;
static int cursorflag=1;

init_screen()
{
	static char tcbuf[1024];
	char *empty_str = "";
	char *ptr, *temp;
	char *term_type;

	errno = 1;
	
	if((term_type = getenv("TERM")) == NULL)
		panic("Environment variable TERM not defined!",empty_str);
	
	if(tgetent(tcbuf, term_type) <= 0)
		panic("Unknown terminal type",term_type);
		
	ptr	= tcapbuf;
	
	if(temp = tgetstr("PC", &ptr)) 
		PC_ = *temp;

	CR = tgetstr("cr", &ptr);
	NL = tgetstr("nl", &ptr);
	CL = tgetstr("cl", &ptr);
	CM = tgetstr("cm", &ptr);
	CE = tgetstr("ce", &ptr);
	CD = tgetstr("cd", &ptr);
	UP = tgetstr("up", &ptr);
	SE = tgetstr("se", &ptr);
	SO = tgetstr("so", &ptr);

	BL = tgetstr("bl", &ptr);
	BC = tgetstr("bc", &ptr);

	GA = tgetstr("ga", &ptr);
	GB = tgetstr("gb", &ptr);
	GC = tgetstr("gc", &ptr);
	GD = tgetstr("gd", &ptr);

	GS = tgetstr("gs", &ptr);
	GX = tgetstr("gx", &ptr);

	GI = tgetstr("gi", &ptr);
	GJ = tgetstr("gj", &ptr);
	GK = tgetstr("gk", &ptr);

	GL = tgetstr("gl", &ptr);
	GM = tgetstr("gm", &ptr);
	GN = tgetstr("gn", &ptr);
	GO = tgetstr("go", &ptr);

	KU = tgetstr("ku", &ptr);
	KD = tgetstr("kd", &ptr);
	KL = tgetstr("kl", &ptr);
	KR = tgetstr("kr", &ptr);

	VI = tgetstr("vi", &ptr);
	VE = tgetstr("ve", &ptr);

	/*HO = tgetstr("ho", &ptr);*/
	term_lines = tgetnum("li");
	term_colms = tgetnum("co");
	
	if(term_lines < 1 || term_colms < 1)
		panic("No lines or colomns in termcap entry!", empty_str);
		
	if(!(CE && CL && CM && UP))
		panic("Incomplete termcap entry!", empty_str);
		
	if(ptr >= &tcapbuf[TCAPSLEN])
		panic("Terminal description too big!", empty_str);
#ifdef OS9
	nopause();
#endif
	errno = 0;
}

/*
 * clear_screen(): cursor home; clear screen
 */
clear_screen()
{
	putpad(HO);
	putpad(CL);
}

/*
 * clear_eol(): clear to end of line
 */
clear_eol(x, y)
{
	move_cursor(x, y);
	putpad(CE);
}	

clear_to_eol()
{
	putpad(CE);
}	

/*
 * cursor_on/off()
 */
cursor_on()
{
	putpad(VE);	cursorflag = 1;
}	

cursor_off()
{
	putpad(VI);	cursorflag = 0;
}	

p_cursor(val)
int	val;
{
	int	rval = cursorflag;
	
	if(val)	cursor_on();
	else	cursor_off();

	return(rval);
}

/*
 * move_cursor(x, y)
 */
move_cursor(x, y)
int x, y;
{
	putpad(tgoto(CM, x, y));
}

/*
 * standout(onflg)
 */		
standout(onflg)
{
	if(SO && onflg){
		putpad(SO);
		standoutflag = 1;
	}
	if(SE && !onflg){
		putpad(SE);
		standoutflag = 0;
	}
}		 

/*
 * exit_screen()
 */
exit_screen()
{
	if(standoutflag) standout(0);
#ifdef OS9
	restpause();
#endif
	move_cursor(0, term_lines-1);
}	 

tputc(c)
char c;
{
	/*return write(1, &c, 1);*/
	putchar(c);
}

touts(s)
char *s;
{
	/*write(1, s, strlen(s));*/
	printf("%s", s);
}

putpad(s)
char *s;
{
	tputs(s, 1, tputc);
}

/*
 * get_screen_x/ysize(): get the screen size
 *
 */
int get_screen_xsize()
{
	return(term_colms);
}	 

int get_screen_ysize()
{
	return(term_lines);
}	 

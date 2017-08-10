/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: getstr.c
 *      Project: UTI library
 *
 *       Author: kp
 *        $Date: 2008/09/15 12:49:04 $
 *    $Revision: 1.3 $
 *
 *  Description: ask for a string from terminal 
 *                      
 *                      
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: getstr.c,v $
 * Revision 1.3  2008/09/15 12:49:04  GLeonhardt
 * Avoid warnings
 *
 * Revision 1.2  1997/12/19 14:55:04  kp
 * Added new header
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: getstr.c,v 1.3 2008/09/15 12:49:04 GLeonhardt Exp $";

#include <stdio.h>
#include <string.h>

/* returns buf - or zero on error) */
char *getstr(txt,buf,def,maxchar,help)
char *txt;					 /* text to put out 							*/
char *buf;					 /* destination buffer							*/
char *def;					 /* default, is returned when 'CR' typed		*/
int maxchar;				 /* maximum # of chars allowed					*/
char *help;					 /* helptext (printed when '?' is hit)			*/
{
	int c,n;

	do {
		printf("%s <%s>: ",txt,def);
		n = 0;
		while((c = getchar()) != '\n' && c != EOF && n <= maxchar && c != '?')
			buf[n++] = c;

		if(c == EOF) return NULL; 
		
/*		if(c == '?'){
			printf("%s\n",help);
			while(getchar() != '\n'); 
			continue;
		}
*/
		if(n > maxchar){
			putchar(0x07);				/* BEEP */
			while(getchar() != '\n'); 	/* skip bad line */
			continue;
		}
		buf[n] = '\0';
		if (n==0)
			strncpy(buf,def,maxchar);	/* return default */
		break;
	} while(1);

	return buf;
}
			

/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: ask.c
 *      Project: UTI library
 *
 *       Author: 
 *        $Date: 2008/09/15 12:48:39 $
 *    $Revision: 1.6 $
 *
 *  Description: print a string to console and ask for user input
 *					(does not wait for <CR> under OS-9).
 *					If only <CR> has been hit, defaultkey is returned
 *                      
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: ask.c,v $
 * Revision 1.6  2008/09/15 12:48:39  GLeonhardt
 * Avoid warnings
 *
 * Revision 1.5  1999/01/20 14:54:11  kp
 * updated for LINUX
 *
 * Revision 1.4  1998/01/08 08:51:29  popp
 * Updated for Visual C 4.0
 *
 * Revision 1.3  1997/12/19 14:53:16  kp
 * fixed bad ifdefs
 *
 * Revision 1.2  1997/12/19 14:01:35  kp
 * UNIX/OS9 merge
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: ask.c,v 1.6 2008/09/15 12:48:39 GLeonhardt Exp $";

#include <stdio.h>


char ask(txt,keys,defaultkey,help)
char *txt,*keys,defaultkey,*help;
{
	char *p,c;
#if defined (HPUX) || defined (HPRT) || defined(LINUX)
	char buf[80];
#endif

	do {
		p = keys;
		printf("%s (",txt);
		while(*p){
			putchar(*p++);
			if(*p) putchar('/');
		}	
		printf(") <%c> ",defaultkey);
		fflush(stdout);
#ifdef OS9
		if(read(0,&c,1) <= 0) c = defaultkey;
#endif
#if defined (HPUX) || defined (HPRT) || defined(LINUX)
		if(gets(buf) == 0 ) 
		    c = defaultkey;
		else
			c = *buf;
#endif		
#if defined(WIN32)
		while ((c = getche())==0)     /* while illegal code */
			if (!c)                    /* extended code ? */
				c = getch();            /* get code */
#endif		
		
		if(c == '\n') {
			c = defaultkey;
			break;
		}
/*		if(c == '?') {
			printf("\n%s\n",help);		
			continue;
		}
*/
		for(p=keys; *p; p++)
			if(c == *p)
				break;
		if(*p == 0){
			putchar('\007');
			putchar('\n');
		}
	} while(*p == 0);
		
	putchar('\n');
	return c;
}			


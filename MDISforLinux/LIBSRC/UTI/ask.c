/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: ask.c
 *      Project: UTI library
 *
 *       Author: 
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
 *---------------------------------------------------------------------------
 * Copyright (c) 1997-2019, MEN Mikro Elektronik GmbH
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
		if(fgets(buf, sizeof(buf), stdin) == 0 )
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


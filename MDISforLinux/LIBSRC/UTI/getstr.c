/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: getstr.c
 *      Project: UTI library
 *
 *       Author: kp
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
			

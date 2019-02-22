/*****************************************************************************
* 
* Project...........: IO-Library for general purposes
* Filename..........: GETINT.C
* Author............: schn
* Creation Date.....: 23.02.90
* 
* Description.......: ask for integer value from terminal 
* 
*---------------------------------------------------------------------------
* Copyright (c) 2019, MEN Mikro Elektronik GmbH
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
/*
#include <modes.h>
#include <errno.h>
#include <module.h>
*/
/*----------------------------------------------------------------------------*/
int getint(txt,def,min,max,help) /* ask for an integer value 					*/ 
char *txt;					 /* text to put out 							*/
int def;					 /* default, is returned when 'CR' typed		*/
int min,max;				 /* minimum and maximum allowed					*/
char *help;					 /* helptext (printed when '?' is hit)			*/
{
	int c,sign;
	int n;
	int nochmal;

	do{	
		nochmal = 0;
		n = def;
		printf("%s <%d>: ",txt,def);	
		while ((c = getchar()) == ' ' || c == '\t');
		if (c != '\n'){ 				/* 'CR': return default */
			sign = 1;
			if (c == '+' || c == '-'){		/* sign */
				sign = (c=='+') ? 1 : -1;
				c = getchar();
			}
			for ( n=0; c>='0' && c<='9'; c=getchar())
				n = 10 * n + c - '0';
			n *= sign;
			if ( c != '\n' ){
/*				if(c == '?') printf("%s\n",help); 
				else */ putchar(0x07);			/* BEEP */
				nochmal = 1;
				while((c = getchar()) != '\n');
			}
			if ( n<min || n>max ){
				putchar(0x07);				/* BEEP */
				nochmal = 1;
			}
		}
	}while( nochmal );
	return(n);
}


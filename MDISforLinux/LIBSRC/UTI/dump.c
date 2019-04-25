/*****************************************************************************
* Project...........: IO-Library for general purposes
* Filename..........: DUMP.C
* Author............: schn
* Creation Date.....: 23.02.90
* 
* Description.......: subroutine to display a hexdump
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

void dump(txt,buf,n)
/* display hexdump, similar to DUMP utility, but a subroutine */  
char *txt;		/* text for information or comment displayed in first line */
char *buf;		/* start dumping from this address */ 
int n;			/* dump n bytes */
{
	int i, m;
	register char *k;
	
	printf("%s\n",txt);
	for(k=buf;k<buf+n;k+=16){	
		printf("%04x: ",k-buf);
		if( k+16 <= buf+n)
			m = 16;
		else
			m = n%16;	

		for(i=0;i< m;i++){
			printf("%02x ",*(k+i)&0xff);
		}
		for(; i<16; i++)
			printf("   ");

		for(i=0;i < m;i++){
			if ( *(k+i)>=32 && *(k+i)<=127 ) printf("%c",*(k+i));
			else printf(".");
		}
		printf("\n");
	}
}

/*****************************************************************************
Project...........: IO-Library for general purposes
Filename..........: DUMP.C
Author............: schn
Creation Date.....: 23.02.90

Description.......: subroutine to display a hexdump

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |          | First Installation                           		| schn   *
* 02 | 05.04.90 | add this header, put it into io.l					| schn	 *
* 03 | 30.05.90 | print address at each line						| K.P.   *
* 04 | 12.04.91 | print only the exact number of bytes				| K.P.   *
******************************************************************************/

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

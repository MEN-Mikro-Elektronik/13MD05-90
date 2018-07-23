/*****************************************************************************
Project...........: IO-Library for general purposes
Filename..........: FDUMP.C
Author............: k.p.
Creation Date.....: 26.09.90

Description.......: subroutine to display a hexdump to FILE

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 | 26.09.90 | First Installation                           		| K.P.   *
* 02 | 12.04.91 | print only the exact number of bytes				| K.P.   *
******************************************************************************/
#include <stdio.h>

void fdump(fp, txt, buf, n)
/* display hexdump, similar to DUMP utility, but a subroutine */  
FILE *fp;		/* ptr to file stream */
char *txt;		/* text for information or comment displayed in first line */
char *buf;		/* start dumping from this address */ 
int n;			/* dump n bytes */
{
	int i, m;
	register char *k;
	
	fprintf(fp,"%s\n",txt);
	for(k=buf;k<buf+n;k+=16){	
		fprintf(fp,"%04x: ",k-buf);
		if( k+16 <= buf+n)
			m = 16;
		else
			m = n%16;	

		for(i=0;i< m;i++){
			fprintf(fp,"%02x ",*(k+i)&0xff);
		}
		for(; i<16; i++)
			fprintf(fp,"   ");

		for(i=0;i < m;i++){
			if ( *(k+i)>=32 && *(k+i)<=127 ) fprintf(fp,"%c",*(k+i));
			else fprintf(fp,".");
		}
		fprintf(fp,"\n");
	}
}

/*****************************************************************************

Project...........: IO-Library for general purposes
Filename..........: GETINT.C
Author............: schn
Creation Date.....: 23.02.90

Description.......: ask for integer value from terminal 

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 | 23.02.90 | First Installation                           		| schn   *
* 02 | 05.04.90 | add this header, put it into IO.L
* 03 | 02.07.90 | add helptext										| K.P.   *
******************************************************************************/
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


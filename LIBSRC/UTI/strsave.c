/*****************************************************************************

Project...........: uti.lib - utility library
Filename..........: strsave.c
Author............: k.p.
Creation Date.....: 26.09.90

Description.......: save a string in memory

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 | 26.09.90 | First Installation                           		| K.P.   *
******************************************************************************/
#include <string.h>
#include <stdlib.h>
/*
 * char *strsave(char *str)
 * save a string in memory
 */
char *strsave(str)
char *str;
{
	char *dest;
	
	if( (dest = malloc(strlen(str)+1)) != NULL )
		strcpy(dest,str);
	return dest;
}			


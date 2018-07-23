/*****************************************************************************

Project...........: uti.l
Filename..........: str_to_upper.c
Author............: K.P.
Creation Date.....: 30.12.90

Description.......: convert a string to upper case

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |29.05.91 | First Installation                           		| K.P.   *
******************************************************************************/
#include <ctype.h>

char *str_to_upper(dest, source)
char *dest, *source;	/* dest and source may be the same */
{
	register char *dp = dest;
	
	while(*source)
		*dp++ = toupper(*source++);

	*dp = '\0';	/* terminate string */		

	return dest;
}

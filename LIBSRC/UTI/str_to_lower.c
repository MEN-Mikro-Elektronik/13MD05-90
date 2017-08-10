/*****************************************************************************

Project...........: uti.l
Filename..........: str_to_lower.c
Author............: K.P.
Creation Date.....: 30.12.90

Description.......: convert a string to lower case

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |03.12.90 | First Installation                           		| K.P.   *
* 02 |29.05.91 | string is now terminated correctly					| K.P.   *
*    |		   | return value is now usuable						| 		 *
******************************************************************************/
#include <ctype.h>

char *str_to_lower(dest, source)
char *dest, *source;	/* dest and source may be the same */
{
	register char *dp = dest;

	while(*source)
		*dp++ = tolower(*source++);

	*dp = '\0';	/* terminate string */		

	return dest;
}

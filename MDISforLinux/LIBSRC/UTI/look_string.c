/*****************************************************************************

Project...........: uti.l
Filename..........: look_string.c
Author............: K.P.
Creation Date.....: 03.12.90

Description.......: test if a string is in string array

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |03.12.90 | First Installation                           		| K.P.   *
* 02 |08.08.91 | changed return value; now returning array index if | K.P.   *
*    |         | found, else -1										|		 *
******************************************************************************/
#include <ctype.h>

int look_string(string, array, caseflg)
char *string, **array;
int caseflg;	/* if != 0, don't be case sensitive */
{
	register int arridx = 0;
	register char *s, *t;

	while(*array){
		for(s=string, t=*array; *t && *s; s++, t++){
			if(caseflg){
				if(tolower(*s) != tolower(*t))
					break;
			}
			else {
				if(*s != *t)
					break;
			}
		}
		if(*s == *t) return arridx; /* found it */							
		array++;
		arridx++;
	}
	return -1; /* not found */
}

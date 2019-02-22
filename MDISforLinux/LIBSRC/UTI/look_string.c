/*****************************************************************************
* 
* Project...........: uti.l
* Filename..........: look_string.c
* Author............: K.P.
* Creation Date.....: 03.12.90
* 
* Description.......: test if a string is in string array
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

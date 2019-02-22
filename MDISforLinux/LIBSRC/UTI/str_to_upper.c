/*****************************************************************************
* 
* Project...........: uti.l
* Filename..........: str_to_upper.c
* Author............: K.P.
* Creation Date.....: 30.12.90
* 
* Description.......: convert a string to upper case
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

char *str_to_upper(dest, source)
char *dest, *source;	/* dest and source may be the same */
{
	register char *dp = dest;
	
	while(*source)
		*dp++ = toupper(*source++);

	*dp = '\0';	/* terminate string */		

	return dest;
}

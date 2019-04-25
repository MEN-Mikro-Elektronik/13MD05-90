/*****************************************************************************
* 
* Project...........: uti.lib - utility library
* Filename..........: strsave.c
* Author............: k.p.
* Creation Date.....: 26.09.90
* 
* Description.......: save a string in memory
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


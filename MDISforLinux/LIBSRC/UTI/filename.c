/*****************************************************************************
* 
* Project...........: uti.l
* Filename..........: filename.c
* Author............: K.P.
* Creation Date.....: 03.12.90
* 
* Description.......: get filenamepart of a path
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


/*
 * filename() get the filename-part of path
 */
char *filename(path)
char *path;
{
	char *fn = path, *p = path;
	
	while(*p) {
		if(*p == '\\' || *p == '/')
			fn = p+1;
		p++;
	}
	return fn;
}			


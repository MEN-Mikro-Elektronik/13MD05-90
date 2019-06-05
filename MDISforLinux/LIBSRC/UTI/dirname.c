/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: dirname
 *      Project: uti
 *
 *      Author: GLeonhardt 
 *
 *  Description: gets the directory from a whole path
 *                      
 *               buf is needed to hold the directory name, dirname always 
 *				 returns <buf>
 *
 *     Required:  
 *     Switches:  
 *
 *  
 *---------------------------------------------------------------------------
 * Copyright (c) 1996-2019, MEN Mikro Elektronik GmbH
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

char *dirname(path,buf)
char *path, *buf;
{
	char *p;

	strcpy( buf, path );
	if( (p = strrchr(buf,'\\')) != NULL ){
		/*--- path includes a directory name ---*/
		*p = '\0';
	}
	else if( (p = strrchr(buf,'/')) != NULL ){
		/*--- path includes a directory name ---*/
		*p = '\0';
	}
	else {
		/*--- no directory included, return "." ---*/
		strcpy( buf, ".");
	}
	
	return buf;
}			


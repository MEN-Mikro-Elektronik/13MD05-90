/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: dirname
 *      Project: uti
 *
 *      $Author: GLeonhardt $
 *        $Date: 2008/09/15 12:56:56 $
 *    $Revision: 1.4 $
 *
 *  Description: gets the directory from a whole path
 *                      
 *               buf is needed to hold the directory name, dirname always 
 *				 returns <buf>
 *
 *     Required:  
 *     Switches:  
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *---------------------------------------------------------------------------
 * (c) Copyright 1996 by MEN mikro elektronik GmbH, Nuernberg, Germany 
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
 
static char *RCSid="$Header: /dd2/CVSR/COM/LIBSRC/UTI/dirname.c,v 1.4 2008/09/15 12:56:56 GLeonhardt Exp $";

#include <string.h>

char *dirname(path,buf)
char *path, *buf;
{
	char *p;
    /* include ident string, suppress warning */
    char a = a = RCSid[0];

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


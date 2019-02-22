/*****************************************************************************
* 
* Project...........: uti.l
* Filename..........: make_path.c
* Author............: K.P.
* Creation Date.....: 03.12.90
* 
* Description.......: concatenate filename to pathname
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

#if !defined NULL
#define NULL 0L
#endif

#if defined (WIN32)
# define PSEP '\\'
#endif

#if defined (HPRT) || defined (HPUX) || defined (OS9) || defined(LINUX) || defined(OS9000)
# define PSEP '/'
#endif

char *make_path(buf, path, file, size)
char *buf;	/* result buffer */
char *path;	/* the pathname */
char *file;	/* the filename */
int  size;	/* sizeof buf 	*/
{
	if((strlen(path) + strlen(file) + 1) >= size)
	    return NULL;

	if (buf != path)             			/* buf not equal ? */
		strcpy(buf,path);                       /* copy path */

	if(*(buf+strlen(buf)-1) == '.')		/* remove '.' */
		*(buf+strlen(buf)-1) = 0;

	if(*(buf+strlen(buf)-1) != PSEP) {      /* add '\' */
		*(buf+strlen(buf)+1) = 0;
		*(buf+strlen(buf))   = PSEP;
	}

	strcat(buf,file);                       /* add file */
	return(buf);
}



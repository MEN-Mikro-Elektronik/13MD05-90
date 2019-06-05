/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: getshorterrmsg.c
 *      Project: 
 *
 *       Author: 
 *
 *  Description: fill buffer with error message
 *                      
 *                      
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *---------------------------------------------------------------------------
 * Copyright (c) 1997-2019, MEN Mikro Elektronik GmbH
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
#include <stdio.h>
#include <errno.h>

char *getshorterrmsg( error, buffer )
int error;			/* error number to search for							*/
char *buffer;		/* the buffer that gets the message						*/
					/* buffer must be at least 81 chars						*/
{

#if defined(HPRT) || defined(HPUX)
	char *s;
	s = ERRMSG(error);
	sprintf(buffer, "%d - %s",error,(s ? s : "(unknown)"));
#endif

#ifdef OS9
	extern char *index();
	char *p;
	int fd;			/* file descriptor for error message file 				*/
	int cmp_lo, cmp_hi;

	if( (fd = open("/dd/SYS/errmsg.short", 1)) == -1)
		return NULL;	/* cannot open	*/		
		
	while( readln( fd, buffer, 80 ) > 0 )
		if( sscanf( buffer, "%3d:%3d", &cmp_hi, &cmp_lo ) == 2 )
			if( (cmp_hi == error >> 8) && (cmp_lo == error & 0xff)){
				if( p = index( buffer, '\n'))
					*p = '\0';
				close(fd);
				return buffer;
			}		
	/* error number was not found: simply print error number */
	sprintf( buffer, "%03d:%03d", error >> 8, error & 0xff );
#endif

#if defined(WIN32) || defined(LINUX)
    char *result;

    result = strerror(error);
	strcpy(buffer,result);
#endif

#if !defined(HPRT) && !defined(HPUX) && !defined(OS9) && !defined(WIN32) && !defined(LINUX)
	sprintf(buffer, "%d", error);
#endif
	return buffer;
}

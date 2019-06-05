/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: panic.c
 *      Project: UTI library
 *
 *       Author: kp
 *
 *  Description: printout error message and leave the program
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

#include <stdio.h>
#include <stdlib.h>
#ifdef LINUX
 #include <errno.h>
#endif /*LINUX*/

void panic(str1, str2)
char *str1, *str2;
{
#ifndef LINUX
	extern int errno;
#endif /*LINUX*/
#ifdef OS9
	extern char *_prgname();

	fprintf(stderr, "%s: %s %s\n", _prgname(), str1, str2);
#endif
#if defined(HPUX) || defined(HPRT) || defined(WIN32) || defined(LINUX)
	fprintf(stderr, "%s %s\n", str1, str2);
	perror("");
#endif

	exit(errno);
}

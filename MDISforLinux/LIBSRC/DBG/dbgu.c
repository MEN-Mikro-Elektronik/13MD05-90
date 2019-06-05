/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: dbgu.c
 *      Project: DBG library for user space
 *
 *       Author: christian.schuster@men.de
 *
 *  Description: Debug functions for Linux
 *
 *     Required: -
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2005-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
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

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_err.h>
#include <stdarg.h>			// for va_list
#include <stdio.h>			// for vsprintf

#ifdef WINNT
#include <windows.h>
#endif
/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef void DBG_HANDLE;

/* dummy definition for dbg.h */
#define DBG_MYLEVEL     null
/* say: don't declare DBG_HANDLE */
#define _NO_DBG_HANDLE
/* DBG_HANDLE needs to be declared BEFORE including dbg.h */
#include <MEN/dbg.h>

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
/* none */


/****************************** DBG_Init ************************************
 *
 *  Description:  Initializes the debug output, allocates and returns debug
 *                handle.
 *				  Does nothing on Linux	
 *---------------------------------------------------------------------------
 *  Input......:  name    name of debug output data module or NULL
 *				  dbgP    pointer to variable where debug handle will stored
 *
 *  Output.....:  *dbgP   debug handle
 *                return  success (0) or error code
 *  Globals....:  -
 ****************************************************************************/
int32 DBG_Init( char *name, DBG_HANDLE **dbgP )
{
	*dbgP = (DBG_HANDLE *)1;
    return(0);
}/*DBG_Init*/

/****************************** DBG_Exit ************************************
 *
 *  Description:  Terminate the debug output
 *
 *---------------------------------------------------------------------------
 *  Input......:  dbgP    pointer to variable where debug handle is stored
 *  Output.....:  return  0 
 *  Globals....:  -
 ****************************************************************************/
int32  DBG_Exit( DBG_HANDLE **dbgP )
{
    *dbgP = NULL;
    return( 0 );
}/*DBG_Exit*/


/*************************** DBG_Write **************************************
 *
 *  Description:  Print a debug message
 *
 *---------------------------------------------------------------------------
 *  Input......:  dbg		debug handle
 *                *fmt		format string
 *                ...		variable argument list
 *  Output.....:  return	success (0) or error code
 *  Globals....:  -
 ****************************************************************************/
int32 DBG_Write(
    DBG_HANDLE	*dbg,
    char		*fmt,
	... 
)
{
#ifdef WINNT
	u_int32		error;
#endif
	va_list		argptr;

#ifdef WINNT
	error = GetLastError();
#endif

	va_start( argptr, fmt );

	vprintf( fmt, argptr );

	va_end( argptr );
	
#ifdef WINNT
	SetLastError(error);
#endif

	return 0;
}

/*************************** DBG_Memdump ************************************
 *
 *  Description:  Print a formatted hex dump
 *
 *                Trailing blanks in the description string define the
 *                indention of all following debug lines.
 *
 *---------------------------------------------------------------------------
 *  Input......:  txt     description string or NULL (first line)
 *                buf     memory to dump
 *                len     nr of bytes
 *                fmt     dump format [bytes]
 *                        1=byte, 2=word, 4=long, 8=64bit 
 *  Output.....:  return  0
 *  Globals....:  -
 ****************************************************************************/
int32  DBG_Memdump
(
   DBG_HANDLE *dbg,
   char    *txt,
   void    *buf,
   u_int32 len,
   u_int32 fmt
)
{
#ifdef WINNT
	u_int32		error;
#endif
    long i,line=1, blanks=0, n;
    char  *k=txt, *k0, *kmax=(char*)buf+len;

#ifdef WINNT
	error = GetLastError();
#endif
    
    if (txt) {
		while(*k++ == ' ')			/* count trailing blanks */
			blanks++;

        printf("%s (%ld bytes)\n",txt,len);
    }

    for (k=k0=(char*)buf; k0<kmax; k0+=16,line++) {
		for (n=0; n<blanks; n++)	/* write trailing blanks */
			printf(" ");
   
        printf("%08x+%04x: ",(long)buf, (short)(k-(char*)buf));

        switch(fmt) {                                      /* dump hex: */
           case 8 : for (k=k0,i=0; i<16; i+=8, k+=8)       /* 64-bit aligned */
                       if (k<kmax)  printf("%08x%08x ",
											  *(long*)k,*(long*)k+4);
                       else         printf("                 ");
                    break;

           case 4 : for (k=k0,i=0; i<16; i+=4, k+=4)       /* long aligned */
                       if (k<kmax)  printf("%08x ",*(long*)k);
                       else         printf("         ");
                    break;

           case 2 : for (k=k0,i=0; i<16; i+=2, k+=2)       /* word aligned */
                       if (k<kmax)  printf("%04x ",*(short*)k & 0xffff);
                       else         printf("     ");
                    break;

           default: for (k=k0,i=0; i<16; i++, k++)         /* byte aligned */
                       if (k<kmax)  printf("%02x ",*k & 0xff);
                       else         printf("   ");
        }

        for (k=k0,i=0; i<16 && k<kmax; i++, k++)           /* dump ascii's: */
            if ( *k>=32 && *k<=127 )
               printf("%c", *k);
            else
               printf(".");

        printf("\n");
    }

#ifdef WINNT
	SetLastError(error);
#endif
	return(0);
}/*DBG_Memdump*/


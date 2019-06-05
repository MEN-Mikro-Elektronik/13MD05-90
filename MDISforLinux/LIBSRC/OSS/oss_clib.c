/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_clib.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Substitutes for C library calls
 *
 *    \switches  -
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
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

#include "oss_intern.h"

/**********************************************************************/
/** Copy memory block.
 * \copydoc oss_specification.c::OSS_MemCopy()
 */
void OSS_MemCopy(
   OSS_HANDLE *oss,
   u_int32    size,
   char       *src,
   char       *dest
)
{
	DBGWRT_1((DBH,"OSS_MemCopy src=0x%p dst=0x%p size=0x%lx\n",
			  src, dest, size ));
	memcpy( dest, src, size );
}/*OSS_MemCopy*/

/**********************************************************************/
/** Fill data block with byte value.
 * \copydoc oss_specification.c::OSS_MemFill()
 */
void OSS_MemFill(
    OSS_HANDLE *oss,
    u_int32    size,
    char       *adr,
    int8       value
)
{
    while (size--)
        *adr++ = (char)value;
}/*OSS_MemFill*/

/**********************************************************************/
/** Get the length of a string.
 * \copydoc oss_specification.c::OSS_StrLen()
 */
u_int32 OSS_StrLen(
    OSS_HANDLE  *oss,
    char        *string
)
{
	return strlen(string);
}/*OSS_StrLen*/

/**********************************************************************/
/** Compare two strings.
 * \copydoc oss_specification.c::OSS_StrCmp()
 */
int32 OSS_StrCmp(
    OSS_HANDLE  *oss,
    char        *str1,
    char        *str2
)
{
	return strcmp(str1,str2);
}/*OSS_StrCmp*/

/**********************************************************************/
/** Compare a limited number of characters of two strings.
 *
 * \copydoc oss_specification.c::OSS_StrNcmp()
 */
int32 OSS_StrNcmp(
    OSS_HANDLE  *oss,
    char        *str1,
    char        *str2,
    u_int32     nbrOfBytes
)
{
	return strncmp( str1, str2, nbrOfBytes );
}/*OSS_StrNcmp*/

/**********************************************************************/
/** Break string into tokens.
 *
 * \copydoc oss_specification.c::OSS_StrTok()
 */
char* OSS_StrTok(
    OSS_HANDLE  *oss,
    char        *string,
    char        *separator,
    char        **lastP
)
{
    DBGCMD( static const char functionName[] = "OSS_StrTok"; )
    char  *startCmpP;
    char  *helpP;
    char  *retP;
    int32 strLen1 = 0;
    int32 strLen2 = 0;
    int32 i;

    DBGWRT_3((DBH,"%s()\n", functionName));

    if( *lastP == NULL )
    {
        if( string == NULL )
            return( NULL );
        startCmpP = string;
    }
    else
    {
        startCmpP = *lastP;
    }/*if*/

    /* store the pointer to return */
    retP = startCmpP;
    *lastP = startCmpP;

    /* determine the string len */
    helpP = startCmpP;
    while( *helpP != 0 )
    {
        strLen1++;
        helpP++;
    }/*while*/

    /* determine the tok len */
    helpP = separator;
    while( *helpP != 0 )
    {
        strLen2++;
        helpP++;
    }/*while*/

    if( strLen1 < strLen2 )
    {
       return( NULL );
    }
    else
    {
        /* look for tok */
        for( i=0; i < (strLen1-strLen2); i++ )
        {
            if( !OSS_StrNcmp(oss, startCmpP, separator, strLen2 ) )
            {
                /* found */
                *startCmpP = 0;                /* insert 0 */
                *lastP = startCmpP + strLen2;  /* return nest searchP */
                return( retP );
            }/*if*/
            startCmpP++;
        }/*for*/

        return( NULL ); /* no more tokens */
    }/*if*/

}/*OSS_StrTok*/

/**********************************************************************/
/** Print to string using variable number of arguments
 *
 * \copydoc oss_specification.c::OSS_Sprintf()
 * \sa OSS_Vsprintf
 */
int32 OSS_Sprintf(
    OSS_HANDLE  *oss,
    char *str,
    const char *fmt,
    ...
)
{
    va_list argptr;
    int32   lengs;

    va_start(argptr,fmt);
    lengs = OSS_Vsprintf(oss,str,fmt,argptr);
    va_end(argptr);

    return(lengs);
}/*OSS_Sprintf*/

/**********************************************************************/
/** Print to string using \em va_list
 *
 * \copydoc oss_specification.c::OSS_Vsprintf()
 * \sa OSS_Sprintf
 */
int32 OSS_Vsprintf(
    OSS_HANDLE  *oss,
    char *str,
    const char *fmt,
    va_list arg
)
{
    return( vsprintf(str,fmt,arg) );
}/*OSS_Vsprintf*/

/**********************************************************************/
/** Copy string.
 *
 * \copydoc oss_specification.c::OSS_StrCpy()
 */
void OSS_StrCpy(
    OSS_HANDLE  *oss,
    char        *from,
    char        *to
)
{
	strcpy( to, from );
}/*OSS_StrCpy*/


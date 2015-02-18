/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  errstr.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/06/06 09:29:43 $
 *    $Revision: 1.2 $
 *
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  Create error string
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 *
 * $Log: errstr.c,v $
 * Revision 1.2  2003/06/06 09:29:43  kp
 * Changed headers for doxygen
 *
 * Revision 1.1  2001/01/19 14:39:41  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: errstr.c,v 1.2 2003/06/06 09:29:43 kp Exp $";

#include "uos_int.h"
#include <MEN/usr_ers.h>   /* UOS error string table   */

/**********************************************************************/
/** Convert UOS error number to static string
 *
 * \copydoc usr_oss_specification.c::UOS_ErrString()
 * \sa UOS_ErrnoGet, UOS_ErrStringTs
 */
char *UOS_ErrString(int32 errCode)
{
    static char errMsg[128];
	return UOS_ErrStringTs( errCode, errMsg );
}

/**********************************************************************/
/** Convert UOS error number to string
 *
 * \copydoc usr_oss_specification.c::UOS_ErrStringTs()
 * \sa UOS_ErrnoGet, UOS_ErrString
 */
char *UOS_ErrStringTs(int32 errCode, char *strBuf )
{
    char        *errString = NULL;
    u_int32     n;

	/* search in table */
	for (n=0; n < UOS_NBR_OF_ERR; n++) {
		if (errCode == UOS_ErrStrTable[n].errCode) {   /* found ? */
			errString = UOS_ErrStrTable[n].errString;
			break;
		}
	}

	sprintf(strBuf,"ERROR (UOS) 0x%04lx:  %s", errCode,
			errString ? errString : "unknown error");

    return(strBuf);
}



/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  m_errstr.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/06/06 09:09:41 $
 *    $Revision: 1.1 $
 *
 *  	 \brief  MDIS error string functions for MDIS_API
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 1997-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <MEN/men_typs.h>   /* MEN type definitions      */
#include <MEN/mdis_api.h>   /* MDIS api                  */
#include <MEN/mdis_err.h>   /* MDIS error codes          */
#include <MEN/mdis_ers.h>   /* MDIS error string table   */
#include <MEN/mdis_mk.h>

/**********************************************************************/
/** Convert MDIS error code to static string
 *
 * \copydoc mdis_api_specification.c::M_errstring()
 * \sa M_errstringTs
 */
char* M_errstring(int32 errCode)
{
    static char errMsg[128];

	return M_errstringTs( errCode, errMsg );
}
/**********************************************************************/
/** Convert MDIS error code to string
 *
 * \copydoc mdis_api_specification.c::M_errstringTs()
 * \sa M_errstring
 */
char* M_errstringTs(int32 errCode, char *strBuf)
{
    char        *errString = NULL;
    u_int32     n;

    /*----------------------+
    |  MDIS error code      |
    +----------------------*/
	if (errCode==ERR_SUCCESS || IN_RANGE(errCode,ERR_OS,ERR_END)) {
		/* search in MDIS msg table */
		for (n=0; n < MDIS_NBR_OF_ERR; n++) {
			if (errCode == errStrTable[n].errCode) {   /* found ? */
				errString = errStrTable[n].errString;
				break;
			}
		}

		if (errString)
			sprintf(strBuf,"ERROR (MDIS) 0x%04lx:  %s",errCode,errString);
		else
			sprintf(strBuf,"ERROR (MDIS) 0x%04lx:  unknown MDIS error",errCode);
    }
    /*----------------------+
    |  else: system error   |
    +----------------------*/
	else {
		sprintf(strBuf,"ERROR (LINUX) #%ld:  %s",
				errCode, strerror( errCode ) );
	}

    return(strBuf);
}




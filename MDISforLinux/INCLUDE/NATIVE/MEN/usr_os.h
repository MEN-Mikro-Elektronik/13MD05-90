/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: usr_os.h
 *
 *       Author: ts
 *        $Date: 2007/05/11 13:57:47 $
 *    $Revision: 1.2 $
 *
 *  Description: NiosII specific data types and defines for using the UOS lib
 *
 *     Switches: UOS_FOR_RTAI - set by RTAI "stdio.h"
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#ifndef _USR_OS_H_
#define _USR_OS_H_

#ifndef UOS_FOR_RTAI
# include <stdio.h>
# include <signal.h>
//# include <endian.h>
#endif

#ifdef __cplusplus
   extern "C" {
#endif

/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
/* error code offset */
#define ERR_UOS				0x2000

#define NO_SHARED_MEM
#define NO_CALLBACK

#ifndef UOS_FOR_RTAI
 /* some predefined signal codes */
# define UOS_SIG_USR1		(SIGUSR1)
# define UOS_SIG_USR2		(SIGUSR2)

#endif /* !UOS_FOR_RTAI */

#ifdef UOS_FOR_RTAI
# define UOS_SIG_USR1		1
# define UOS_SIG_USR2	    2


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
# ifdef _RTAI_MBX_H
extern int32 UOS_RtaiSigInit( MBX **mbxP );
# endif
extern int32 UOS_RtaiSigExit( void );
extern int UOS_RtaiInit(void);
extern void UOS_RtaiExit(void);

#endif /* UOS_FOR_RTAI */

#ifdef __cplusplus
   }
#endif
#endif /*_USR_OS_H_*/





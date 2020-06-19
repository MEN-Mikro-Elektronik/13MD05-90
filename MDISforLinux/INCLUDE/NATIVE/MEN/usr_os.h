/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: usr_os.h
 *
 *       Author: ts
 *
 *  Description: NiosII specific data types and defines for using the UOS lib
 *
 *     Switches: UOS_FOR_RTAI - set by RTAI "stdio.h"
 *
 *---------------------------------------------------------------------------
 * Copyright 2000-2019, MEN Mikro Elektronik GmbH
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
/*
 * We use linux real-time signals, because multiple instances of real-time signals
 * can be queued.
 * Warning: Becasue SIGRTMIN is not a fix value and defined as function call in
 * user space, it isn't usable in switch/case constructs. However, as workaround we
 * use here a fix value. On most linux distros SIGRTMIN is 34 or 35. 
 */
#define SIGRTMIN_FIX	(36)
#define UOS_SIG_USR1	(SIGRTMIN_FIX+1)
#define UOS_SIG_USR2	(SIGRTMIN_FIX+2)
#define UOS_SIG_USR3	(SIGRTMIN_FIX+3)
#define UOS_SIG_USR4	(SIGRTMIN_FIX+4)

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





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
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: usr_os.h,v $
 * Revision 1.2  2007/05/11 13:57:47  ts
 * use SIGUSR1/2 instead SIGRTMIN for UOS_SIG_USR1/2
 *
 * Revision 1.1  2007/02/06 13:10:35  aw
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

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





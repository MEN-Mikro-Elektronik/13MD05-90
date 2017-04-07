/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  oss_intern.h
 *
 *      \author  kp
 *        $Date: 2012/08/07 19:06:28 $
 *    $Revision: 1.15 $
 *
 *	   \project  MDIS4Linux
 *       \brief  oss internal defines
 *    \switches  DBG
 *               MAC_USERSPACE
 *               OXX_IO_ACCESS_EN
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_intern.h,v $
 * Revision 1.15  2012/08/07 19:06:28  ts
 * R: include file asm/io.h unnecessary
 * M: removed include
 *
 * Revision 1.14  2012/04/23 13:49:56  ts
 * R: spinlock declaration changed in kernel 3.x
 * M: include spinlock_types.h, use DEFINE_SPINLOCK if defined
 *
 * Revision 1.13  2007/03/28 16:06:58  ts
 * renamed defines with naming VME4L.. to be generic
 *
 * Revision 1.12  2007/02/06 17:04:45  ts
 * cosmetics
 *
 * Revision 1.10  2006/11/14 12:15:34  ts
 * added version.h include for KERNEL_VERSION
 *
 * Revision 1.9  2006/09/26 10:17:21  ts
 * adapted for either classic RTAI or Xenomai usage
 *
 * Revision 1.8  2005/07/08 11:32:54  cs
 * added support for userspace
 * Copyright line changed
 *
 * Revision 1.7  2004/10/27 14:34:11  kp
 * adapted to RTAI 3.0
 *
 * Revision 1.6  2004/07/16 15:00:34  kp
 * adapted to Redhat9 kernel
 *
 * Revision 1.5  2004/06/09 09:24:56  kp
 * Linux 2.6 adaptions
 *
 * Revision 1.4  2003/04/11 16:13:19  kp
 * use MDIS_RTAI_SUPPORT switch
 *
 * Revision 1.3  2003/02/21 11:25:07  kp
 * added rtMode flag to OSS_HANDLE.
 * added OSS_RtaiXXX prototypes
 *
 * Revision 1.2  2002/05/31 15:10:45  kp
 * include "slab.h"
 *
 * Revision 1.1  2001/01/19 14:39:08  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#ifndef _OSS_INTERN_H
#  define _OSS_INTERN_H

#  ifdef __cplusplus
      extern "C" {
#  endif

#ifndef MAC_USERSPACE
#define __NO_VERSION__
# include <linux/module.h>
# include <linux/version.h>
# include <linux/slab.h>
# include <linux/delay.h>
# include <linux/timer.h>
# include <linux/spinlock_types.h>
# include <linux/sched.h>
# include <linux/ioport.h>
# include <linux/pci.h>
# include <linux/signal.h>
#else /* Userspace OSS... */
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <stdarg.h>
# include <errno.h>
# include <time.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <sys/stat.h>
# ifdef OSS_USR_IO_MAPPED_ACC_EN
#  include <sys/io.h>
# endif /* OSS_USR_IO_MAPPED_ACC_EN */
#endif /* !MAC_USERSPACE */

#include <MEN/men_typs.h>
#include <MEN/mdis_err.h>
#include <MEN/dbg.h>        /* debug module */


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
#ifndef MAC_USERSPACE
typedef struct {
	char instName[40];			/* name of OSS instance */	
	int32 dbgLevel;				/* debug level */
	DBG_HANDLE *dbh;			/* debug handle */
} OSS_HANDLE;
#endif /* !MAC_USERSPACE */

#include <MEN/oss.h>

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define DBG_MYLEVEL         oss->dbgLevel
#define DBH 				oss->dbh

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,0,0)
# define TASK_SIGPENDING 	pending.signal.sig
#else
# define TASK_SIGPENDING 	signal.sig
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20)
# define TASK_LOCK_SIGNALS(t,flags)	    spin_lock_irqsave(&(t)->sighand->siglock, flags);
# define TASK_UNLOCK_SIGNALS(t,flags)	spin_unlock_irqrestore(&(t)->sighand->siglock, flags);
# define RECALC_SIGPENDING() 			recalc_sigpending()
#else
# define TASK_LOCK_SIGNALS(t,flags)	    spin_lock_irqsave(&(t)->sigmask_lock, flags);
# define TASK_UNLOCK_SIGNALS(t,flags)	spin_unlock_irqrestore(&(t)->sigmask_lock, flags);
# define RECALC_SIGPENDING() 			recalc_sigpending(current)
#endif

#ifdef MAC_USERSPACE
#define OSS_MAX_USEC	1000000		/* max mikrodelay */
#endif /* MAC_USERSPACE */
/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
extern sigset_t OSS_allRtSigs;
extern sigset_t OSS_allSigs;
extern int oss_pci_slot_devnbrs[16];

#ifdef MAC_USERSPACE
extern int OSS_Memdev;
extern OSS_DL_LIST OSS_VME_addrWinList;
#endif /* MAC_USERSPACE */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

#  ifdef __cplusplus
      }
#  endif

#endif/*_OSS_INTERN_H*/

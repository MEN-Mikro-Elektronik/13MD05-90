/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  oss_intern.h
 *
 *      \author  kp
 *
 *	   \project  MDISforLinux
 *       \brief  oss internal defines
 *    \switches  DBG
 *               MAC_USERSPACE
 *               OXX_IO_ACCESS_EN
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
# include <linux/sched/signal.h>
#endif
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

#ifndef MAC_USERSPACE
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20)
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
#endif /* !MAC_USERSPACE */

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

/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: oss_os.h
 *
 *       Author: kp
 *        $Date: 2014/07/17 17:49:54 $
 *    $Revision: 1.7 $
 *
 *  Description: Linux specific data types and defines
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_os.h,v $
 * Revision 1.7  2014/07/17 17:49:54  ts
 * R: compiler warning distinctive pointertype lacks conversion info
 * M: changed type OSS_IRQ_STATE to unsigned long, is also either 32- or 64bit
 *
 * Revision 1.6  2014/07/14 18:29:33  ts
 * R: on new 3.x kernels include of kernel.h breaks compile
 * M: removed include, not necessary anymore
 *
 * Revision 1.5  2010/12/06 10:48:24  CRuff
 * R: new prototypes OSS_SPINL_* defined in oss.h
 * M: added dummy typedef OSS_SPINL_HANDLE for non-kernel builds
 *
 * Revision 1.4  2010/12/06 10:20:49  CRuff
 * R: new prototypes OSS_SPINL_* defined in oss.h
 * M: added typedef OSS_SPINL_HANDLE
 *
 * Revision 1.3  2009/09/23 09:05:20  CRuff
 * R: type conversion error for OSS_IRQ_STATE on 64bit systems
 * M: use variable data type U_INT32_OR_64 to match the expected "unsigned long"
 *
 * Revision 1.2  2007/02/16 15:01:54  ts
 * corrected wrong previous checking
 *
 * Revision 2.6  2004/10/27 14:34:07  kp
 * adapted to RTAI 3.0
 *
 * Revision 2.5  2004/06/09 09:24:43  kp
 * adaptions to Linux 2.6
 * MDIS4/2004 compliance
 * default debug level changed
 *
 * Revision 2.3  2003/10/07 11:47:34  kp
 * removed ENDIAN switches (now controlled by Makefile)
 *
 * Revision 2.2  2003/06/06 09:19:51  kp
 * added OSS_ALARM_STATE
 *
 * Revision 2.1  2003/04/11 16:13:04  kp
 * changed CONFIG_RTHAL switch to MDIS_RTAI_SUPPORT
 *
 * Revision 2.0  2003/02/21 11:24:58  kp
 * reworked for RTAI integration
 *
 * Revision 1.1  2001/01/19 14:39:01  kp
 * Initial Revision
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
#ifndef _OSS_OS_H
#define _OSS_OS_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <linux/types.h>
#include <stdarg.h> /* for va_list */

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define NO_CALLBACK		/* flag for oss.h not to include callback protos */
#define NO_SHARED_MEM	/* flag for oss.h not to include shared mem protos */

#ifdef __KERNEL__
#include <linux/timer.h>
#include <linux/spinlock.h>

#define OSS_HAS_UNASSIGN_RESOURCES /* flag for oss.h  */

#define OSS_IRQ_EXCLUSIVE   0
#define OSS_IRQ_SHARED      1

#define OSS_LINUX_OS_HDL  	NULL
#define OSS_DBG_DEFAULT     0xc0008000

/*
 * rename all OSS functions to prevent name space conflicts
 */
#define OSS_Init		men_OSS_Init
#define OSS_Exit		men_OSS_Exit
#define OSS_Ident		men_OSS_Ident
#define OSS_MemGet		men_OSS_MemGet
#define OSS_MemFree		men_OSS_MemFree
#define OSS_MemChk		men_OSS_MemChk
#define OSS_MemCopy		men_OSS_MemCopy
#define OSS_MemFill		men_OSS_MemFill
#define OSS_MemChk		men_OSS_MemChk
#define OSS_StrCpy		men_OSS_StrCpy
#define OSS_StrLen		men_OSS_StrLen
#define OSS_StrCmp		men_OSS_StrCmp
#define OSS_StrNcmp		men_OSS_StrNcmp
#define OSS_StrTok		men_OSS_StrTok
#define OSS_IrqMask		men_OSS_IrqMask
#define OSS_IrqMaskR		men_OSS_IrqMaskR
#define OSS_IrqRestore		men_OSS_IrqRestore
#define OSS_IrqUnMask		men_OSS_IrqUnMask
#define OSS_IrqLevelToVector	men_OSS_IrqLevelToVector
#define OSS_SigCreate		men_OSS_SigCreate
#define OSS_SigSend		men_OSS_SigSend
#define OSS_SigRemove		men_OSS_SigRemove
#define OSS_SigInfo		men_OSS_SigInfo
#define OSS_SemCreate		men_OSS_SemCreate
#define OSS_SemRemove		men_OSS_SemRemove
#define OSS_SemWait		men_OSS_SemWait
#define OSS_SemSignal		men_OSS_SemSignal
#define OSS_DbgLevelSet		men_OSS_DbgLevelSet
#define OSS_DbgLevelGet		men_OSS_DbgLevelGet
#define OSS_Delay		men_OSS_Delay
#define OSS_TickRateGet		men_OSS_TickRateGet
#define OSS_TickGet		men_OSS_TickGet
#define OSS_GetPid		men_OSS_GetPid
#define OSS_MikroDelayInit	men_OSS_MikroDelayInit
#define OSS_MikroDelay		men_OSS_MikroDelay
#define OSS_BusToPhysAddr	men_OSS_BusToPhysAddr
#define OSS_PciGetConfig	men_OSS_PciGetConfig
#define OSS_PciSetConfig	men_OSS_PciSetConfig
#define OSS_PciSlotToPciDevice	men_OSS_PciSlotToPciDevice
#define OSS_IsaGetConfig	men_OSS_IsaGetConfig
#define OSS_UnAssignResources	men_OSS_UnAssignResources
#define OSS_AssignResources	men_OSS_AssignResources
#define OSS_MapPhysToVirtAddr   men_OSS_MapPhysToVirtAddr
#define OSS_UnMapVirtAddr	men_OSS_UnMapVirtAddr
#define OSS_Sprintf		men_OSS_Sprintf
#define OSS_Vsprintf		men_OSS_Vsprintf
#define OSS_AlarmCreate		men_OSS_AlarmCreate
#define OSS_AlarmRemove		men_OSS_AlarmRemove
#define OSS_AlarmSet		men_OSS_AlarmSet
#define OSS_AlarmClear		men_OSS_AlarmClear
#define OSS_AlarmMask		men_OSS_AlarmMask
#define OSS_AlarmRestore	men_OSS_AlarmRestore
#define	OSS_Swap16		men_OSS_Swap16
#define	OSS_Swap32		men_OSS_Swap32
#define	OSS_DL_NewList		men_OSS_DL_NewList
#define	OSS_DL_Remove		men_OSS_DL_Remove
#define	OSS_DL_RemHead		men_OSS_DL_RemHead
#define	OSS_DL_AddTail		men_OSS_DL_AddTail
#define	OSS_irqLock		men_OSS_irqLock

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
typedef spinlock_t OSS_SPINL_HANDLE;

typedef void OSS_IRQ_HANDLE;
extern spinlock_t OSS_irqLock;

#define OSS_HAS_IRQMASKR
typedef unsigned long OSS_IRQ_STATE;

#define OSS_HAS_ALARMMASK
typedef OSS_IRQ_STATE OSS_ALARM_STATE;

#ifndef _OSS_INTERN_H
typedef void OSS_HANDLE;
#endif

/*--- SEMAPHORE HANDLE ---*/
#ifndef _OSS_SEM_C
typedef void OSS_SEM_HANDLE;
#else  /* _OSS_SEM_C defined */

#define OSS_SEM_HANDLE OSS_LIN_SEM_HANDLE

typedef struct OSS_LIN_SEM_HANDLE
{
	int32 value;				/* semaphore's value */
	int32 semType;				/* OSS_SEM_BIN or OSS_SEM_COUNT */
	spinlock_t lock;			/* lock for sempahore modifications */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20)
	wait_queue_head_t wq;		/* wait queue for waiting processes */
#else
	struct wait_queue *wq;		/* wait queue for waiting processes */
#endif
} OSS_LIN_SEM_HANDLE;

#endif /* _OSS_SEM_C */

/*--- SIGNAL HANDLE ---*/
#ifndef _OSS_SIG_C
typedef void OSS_SIG_HANDLE;
#else  /* _OSS_SIG_C defined */

#define OSS_SIG_HANDLE OSS_LIN_SIG_HANDLE

typedef struct OSS_LIN_SIG_HANDLE
{
	pid_t pid;						/* process id */
	int sig;						/* signal number */
	struct task_struct *sig_task;	/* task information of calling task */
} OSS_LIN_SIG_HANDLE;

#endif /* _OSS_SIG_C */

/*--- ALARM HANDLE ---*/
#ifndef _OSS_ALARM_C
typedef void OSS_ALARM_HANDLE;
#else /* _OSS_ALARM_C defined */

#define OSS_ALARM_HANDLE OSS_LIN_ALARM_HANDLE

typedef struct OSS_LIN_ALARM_HANDLE
{
    void (*funct)(void *arg);	/* alarm routine to be called */
	void *arg;					/* arg to pass to function */
	int active;					/* timer routine started */
	int cyclic;					/* flags cyclic alarm */
	struct timer_list tmr;		/* the linux kernel timer struct */
	unsigned long interval;		/* timeout in jiffies */
	OSS_HANDLE *oss;
} OSS_LIN_ALARM_HANDLE;

#endif /* _OSS_ALARM_C */

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
extern int32  OSS_Init( char *instName, OSS_HANDLE **ossP );
extern int32  OSS_Exit( OSS_HANDLE **ossP );


#else /* NOT KERNEL */
typedef void OSS_HANDLE;
typedef void OSS_ALARM_HANDLE;
typedef void OSS_SIG_HANDLE;
typedef void OSS_SEM_HANDLE;
typedef void OSS_IRQ_HANDLE;
typedef void OSS_SPINL_HANDLE;

#endif /* __KERNEL__*/

#ifdef __cplusplus
   }
#endif
#endif /*_OSS_OS_H*/







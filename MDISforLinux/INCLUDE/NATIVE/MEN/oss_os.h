/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: oss_os.h
 *
 *       Author: kp
 *
 *  Description: Linux specific data types and defines
 *
 *     Switches: -
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
#ifndef _OSS_OS_H
#define _OSS_OS_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <linux/types.h>

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define NO_CALLBACK		/* flag for oss.h not to include callback protos */
#define NO_SHARED_MEM	/* flag for oss.h not to include shared mem protos */

#ifdef __KERNEL__
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,15,0)
#include <linux/stdarg.h> /* for va_list */
#else
// RHEL specific changes
#if defined(RHEL_RELEASE_CODE) && defined(RHEL_RELEASE_VERSION)
#if RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(9,1)
#include <linux/stdarg.h> /* for va_list */
#endif // RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(9,1)
#else
#include <stdarg.h> /* for va_list */
#endif // defined(RHEL_RELEASE_CODE) && defined(RHEL_RELEASE_VERSION)
#endif // LINUX_VERSION_CODE > KERNEL_VERSION(5,15,0)

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
#include <stdarg.h>

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







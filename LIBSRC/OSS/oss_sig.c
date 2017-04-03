/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_sig.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2010/04/14 10:06:41 $
 *    $Revision: 1.11 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Signal routines
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_sig.c,v $
 * Revision 1.11  2010/04/14 10:06:41  CRuff
 * R: find_task_by_vpid is deprecated
 * M: replace find_task_by_vpid by find_vpid/pid_task form kernel 2.6.26
 *
 * Revision 1.10  2009/09/23 09:21:44  CRuff
 * R: 1. OSS_SigSend causes problems when called in interrupt handler
 * M: 1a) get signal receiver task information when signal is created
 *        (OSS_SigCreate) and remember it in OSS_SIG_HANDLE
 *    1b) Mask interrupts when task information is written to OSS_SIG_HANDLE
 *
 * Revision 1.9  2009/09/08 16:44:10  CRuff
 * R: compiler warning
 * M: moved variable declaration to beginning of method
 *
 * Revision 1.8  2009/08/04 17:16:22  CRuff
 * R: OSS_SigSend causes application crash for kernels >= 2.6.26
 * M: get task struct of calling task and send signal there instead of using
 *    IRQ context information
 *
 * Revision 1.7  2009/07/27 17:12:17  CRuff
 * R: OSS_SigSend: wrong return value in error case
 * M: return ERR_OSS_SIG_SEND in error case
 *
 * Revision 1.6  2009/07/22 14:21:32  CRuff
 * R: SigSend did not work when called from interrupt context for kernel >2.6.26
 * M: get task information for calling task and hand it over to send_sig (instead
 *    of interrupt task context)
 *
 * Revision 1.5  2009/05/18 15:15:18  ts
 * R: build failed under ElinOS 2.6.27
 * M: replaced obsolete function kill_proc() with send_sig()
 *
 * Revision 1.4  2005/07/07 17:17:35  cs
 * Copyright line changed
 *
 * Revision 1.3  2003/04/11 16:13:34  kp
 * Comments changed to Doxygen
 *
 * Revision 1.2  2003/02/21 11:25:15  kp
 * added RTAI dispatching functions
 *
 * Revision 1.1  2001/01/19 14:39:16  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#define _OSS_SIG_C
#include "oss_intern.h"

struct OSS_LIN_SIG_HANDLE;

/************* LINUX Implementation ******************/

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*! \page linsigusage

  \section linsigusagesect Linux notes to OSS signals

  This implementation is straight forward. It uses Linux native
  signals, typically realtime signals.

  See \ref osssigusagesect for more info.
*/

/**********************************************************************/
/** Create signal handle.
 *
 * \copydoc oss_specification.c::OSS_SigCreate()
 *
 * See \ref linsigusagesect for more info.
 *
 *
 * \sa OSS_SigRemove, OSS_SigSend
 */
int32 OSS_SigCreate(
    OSS_HANDLE       *oss,
    int32            signal,
    OSS_SIG_HANDLE   **sigP
)
{
	OSS_SIG_HANDLE *sig;
	OSS_IRQ_STATE  irqState;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	struct pid *pidStruct;
#endif

    DBGWRT_1((DBH,"OSS - OSS_SigCreate: sig=0x%ld pid=%ld\n",
			  (long)signal,(long)current->pid));
	*sigP = NULL;

	/* allocate memory for handle */
	sig = (OSS_SIG_HANDLE *)kmalloc( sizeof(OSS_SIG_HANDLE), GFP_KERNEL );
	if( sig == NULL )
		return( ERR_OSS_MEM_ALLOC );
		
	irqState = OSS_IrqMaskR( oss, NULL );
	
	sig->pid = current->pid;
	sig->sig = signal;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
 pidStruct = find_vpid(sig->pid);
 sig->sig_task = pid_task(pidStruct, PIDTYPE_PID);
#endif

	OSS_IrqRestore( oss, NULL, irqState );

	*sigP = sig;
	return 0;
}/*OSS_SigCreate*/

/**********************************************************************/
/** Destroy semaphore handle.
 *
 * \copydoc oss_specification.c::OSS_SigRemove()
 *
 * See \ref linsigusagesect for more info.
 *
 *
 * \sa OSS_SigCreate, OSS_SigSend
 */
int32 OSS_SigRemove(
    OSS_HANDLE     *oss,
    OSS_SIG_HANDLE **sigP)
{
    OSS_SIG_HANDLE *sig = *sigP;

    DBGWRT_1((DBH,"OSS - OSS_SigRemove: sig=%ld pid=%ld\n",
			  (long)sig->sig,(long)sig->pid));

	if (sig->pid != current->pid) {		/* invalid process ? */
		DBGWRT_ERR((DBH," *** OSS_SigRemove: can't remove signal: "
					"wrong own pid\n"));
        return(ERR_OSS_SIG_CLR);
	}

	/* clear pointer before deallocating the handle to     */
	/* prevent interferences if interrupted by OSS_SigSend */
	*sigP = NULL;
	kfree( sig );
    return 0;
}

/**********************************************************************/
/** Send signal to a process
 *
 * \copydoc oss_specification.c::OSS_SigSend()
 *
 * See \ref linsigusagesect for more info.
 *
 *
 * \sa OSS_SigCreate, OSS_SigRemove
 */
int32 OSS_SigSend(
    OSS_HANDLE *oss,
    OSS_SIG_HANDLE* sig)
{

    DBGWRT_1((DBH,"OSS - OSS_SigSend: sig=%ld pid=%ld\n",
			  (long)sig->sig,(long)sig->pid));	

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	if (kill_proc( sig->pid, sig->sig, 1 )) {	
#else
	if ( send_sig(sig->sig, sig->sig_task, 1) ) {
#endif
		DBGWRT_ERR((DBH,"*** OSS_SigSend: error sending signal to pid %ld\n",
					(long)sig->pid ));
		return ERR_OSS_SIG_SEND;
	}
	return 0;
}
	
/**********************************************************************/
/** Get info about signal
 *
 * \copydoc oss_specification.c::OSS_SigInfo()
 */
int32 OSS_SigInfo(
    OSS_HANDLE     *oss,
    OSS_SIG_HANDLE *sig,
    int32          *signalP,
    int32          *pidP)
{
	*signalP = sig->sig;
	*pidP    = sig->pid;

	return 0;
}



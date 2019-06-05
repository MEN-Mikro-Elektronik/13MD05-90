/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_sig.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Signal routines
 *
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
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



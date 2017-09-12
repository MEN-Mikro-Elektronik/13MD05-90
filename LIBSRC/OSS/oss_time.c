/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_time.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/09/08 16:44:13 $
 *    $Revision: 1.9 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Time related routines of the OSS module
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_time.c,v $
 * Revision 1.9  2009/09/08 16:44:13  CRuff
 * R: compiler warning
 * M: changed variable initialization
 *
 * Revision 1.8  2009/07/28 12:03:52  CRuff
 * R: OSS_Delay causes too short delay if tickrollover occurs and delay is
 *    interrupted by signal
 * M: keep delay independent from tickrollover by not using system tick
 *    differences
 *
 * Revision 1.7  2007/12/04 16:27:22  ts
 * removed warning about possible use of uninitialized signal_t members
 *
 * Revision 1.6  2005/07/07 17:17:45  cs
 * Copyright line changed
 *
 * Revision 1.5  2004/06/09 09:25:06  kp
 * use macros for spin locks and signal mask accesses to be more independent
 * of kernel version
 *
 * Revision 1.4  2003/04/11 16:13:40  kp
 * Comments changed to Doxygen
 *
 * Revision 1.3  2003/02/21 11:25:19  kp
 * added RTAI dispatching functions
 *
 * Revision 1.2  2001/08/16 09:59:42  kp
 * Bug fix: OSS_Delay returned always 0
 *
 * Revision 1.1  2001/01/19 14:39:19  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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

#include "oss_intern.h"

/************* LINUX Implementation ******************/

void HandleSig( OSS_HANDLE *oss, sigset_t *oldBlocked )
{
	unsigned long flags;

	TASK_LOCK_SIGNALS( current, flags );
	*oldBlocked = current->blocked;

	if( signal_pending(current)){

		DBGWRT_2((DBH,"OSS_Delay interrupted by signal\n"));
		sigorsets( &current->blocked, &current->blocked, &OSS_allSigs );
		RECALC_SIGPENDING();		

	}
	else
		DBGWRT_ERR((DBH,"*** OSS_Delay returned too early without "
					"sigpending\n"));

	TASK_UNLOCK_SIGNALS( current, flags );
}

/**********************************************************************/
/** Let process sleep for specified time.
 *
 * \copydoc oss_specification.c::OSS_Delay()
 *
 * \linux \linrtai For RTAI implementation, see OSS_RtaiDelay().
 * \linux If a signal occurs during sleep, all signals are masked
 *        until sleep has finished.
 * \sa OSS_MikroDelay
 */
int32 OSS_Delay( OSS_HANDLE *oss, int32 msec )
{
	long ticks, i;
	long delta;
	sigset_t oldBlocked;
	int oldBlockedValid = 0;
	unsigned long flags;
	
	DBGWRT_1((DBH,"OSS_Delay %d ms\n", msec ));

	/* keep gcc from complaining about using possibly uninitialized values*/
	for (i = 0; i < _NSIG_WORDS; i++)
		oldBlocked.sig[i]=0;

	/*--- round time, and correct for timer inaccuracy ---*/
	ticks = (msec * HZ) / 1000;
	ticks++;

	if( (msec * HZ) % 1000 )
		ticks++;

	delta = ticks; /* initialize work counter with calculated wait time */
	
	while ( delta > 0 ) {
		DBGWRT_2((DBH, " %d ticks\n", delta ));
		set_current_state(TASK_INTERRUPTIBLE);
		delta = schedule_timeout( delta );

		// is wait time completed?
		if (delta > 0) {
			// wait time left; block signals
			HandleSig( oss, &oldBlocked );
			oldBlockedValid = TRUE;
		}
	}
	if( oldBlockedValid ){
		TASK_LOCK_SIGNALS( current, flags );
		current->blocked = oldBlocked;
		RECALC_SIGPENDING();		
		TASK_UNLOCK_SIGNALS( current, flags );
	}
	return (ticks * 1000) / HZ;
}/*OSS_Delay*/

/**********************************************************************/
/** Initialisation routine for OSS_MikroDelay().
 *
 * \copydoc oss_specification.c::OSS_MikroDelayInit()
 *
 * \linux For Linux and RTAI, this is a no-op routine.
 * \sa OSS_MikroDelay
 */
int32 OSS_MikroDelayInit( OSS_HANDLE *oss )
{
	DBGWRT_1((DBH,"OSS_MikroDelayInit\n"));
    return 0;					/* nothing to do */
}/*OSS_MikroDelayInit*/

/**********************************************************************/
/** Delay execution of a process by using a busy-loop.
 *
 * \copydoc oss_specification.c::OSS_MikroDelay()
 *
 * \linux \linrtai For RTAI implementation, see OSS_RtaiMikroDelay().
 * \linux \em udelay() is used.
 */
int32 OSS_MikroDelay( OSS_HANDLE *oss, u_int32 usec )
{
	DBGWRT_1((DBH,"OSS_MikroDelay us=%ld\n", usec));
	udelay(usec);
    return(0);
}/*OSS_MikroDelay*/

/**********************************************************************/
/** Get the tick rate.
 *
 * \copydoc oss_specification.c::OSS_TickRateGet()
 *
 * \linux \linrtai For RTAI implementation, see OSS_RtaiTickRateGet().
 * \linux returns kernel macro \em HZ.
 * \sa OSS_TickGet
 */
int32 OSS_TickRateGet( OSS_HANDLE *oss )
{
    return HZ;
}/*OSS_TickRateGet*/

/**********************************************************************/
/** Get the current system tick.
 *
 * \copydoc oss_specification.c::OSS_TickGet()
 *
 * \linux \linrtai For RTAI implementation, see OSS_RtaiTickGet().
 * \linux uses \em jiffies.
 * \sa OSS_TickRateGet
 */
u_int32 OSS_TickGet(OSS_HANDLE *oss)
{
	return jiffies;
}/*OSS_TickGet*/




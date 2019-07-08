/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_sem.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Semaphore routines
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

#define _OSS_SEM_C
#include "oss_intern.h"


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*! \page linsemusage

  \section linsemusagesect Linux notes to OSS semaphores

  This implementation uses Linux waitqueues for semaphores.
  There may be a better way to implement OSS semaphores using kernel
  semaphores, however I didn't find enough docs on these.

  Semaphore waits can be aborted by deadly signals under Linux.

  See \ref osssemusagesect for more info.
*/

/**********************************************************************/
/** Create a semaphore.
 *
 * \copydoc oss_specification.c::OSS_SemCreate()
 *
 * See \ref linsemusagesect for more info.
 *
 * \sa OSS_SemRemove, OSS_SemWait, OSS_SemSignal
 */
int32 OSS_SemCreate(
    OSS_HANDLE 	   *oss,
    int32          semType,
    int32          initVal,
    OSS_SEM_HANDLE **semP)
{
    DBGCMD( static const char functionName[] = "OSS_SemCreate()"; )
    OSS_SEM_HANDLE *semHandle;

    DBGWRT_1((DBH,"%s()\n", functionName));

	*semP = NULL;

	/* allocate memory for semaphore */
	semHandle = kmalloc( sizeof(OSS_SEM_HANDLE), GFP_KERNEL );
	if( semHandle == NULL )
		return( ERR_OSS_MEM_ALLOC );

	semHandle->value 	= initVal;
	semHandle->semType	= semType;

	init_waitqueue_head( &semHandle->wq );
	spin_lock_init( &semHandle->lock );

	*semP = semHandle;
    return( 0 );
}/*OSS_SemCreate*/

/**********************************************************************/
/** Destroy semaphore handle.
 *
 * \copydoc oss_specification.c::OSS_SemRemove()
 *
 * See \ref linsemusagesect for more info.
 *
 *
 * \sa OSS_SemCreate, OSS_SemWait, OSS_SemSignal
 */
int32 OSS_SemRemove(
    OSS_HANDLE *oss,
    OSS_SEM_HANDLE** semHandleP)
{
    DBGCMD( static const char functionName[] = "OSS_SemRemove"; )
    OSS_SEM_HANDLE *semHandle;

    DBGWRT_1((DBH,"%s() OSS_SEM_HANDLE = 0x%p\n", functionName, *semHandleP ));

	semHandle   = *semHandleP;
	*semHandleP = NULL;

	if( semHandle )
		kfree( semHandle );
    return( 0 );
}/*OSS_SemRemove*/

static int32 HandleSig( OSS_HANDLE *oss, sigset_t *oldBlocked )
{
	unsigned long flags;
	u_int32 sigMask;
	/*
	 * check if there is a signal <= 31 pending.
	 * If so, the signal is assumed to be deadly and OSS_SemWait
	 * is aborted.
	 * All other signals are ignored, and OSS_SemWait continues to wait.
	 * But in order to allow the process to sleep again, we must block
	 * these signals. The original signal mask is saved here and restored when
	 * OSS_SemWait exits
	 *
	 */

	/* ok. the following uses internas of sigsets, but fastest way to do it */
	TASK_LOCK_SIGNALS( current, flags );

	/* SIGUSR1/2 are no deadly signals and must not abort OSS_SemWait! */
	sigMask = 0x7fffffff & ~( (1<<(SIGUSR1-1)) | (1<<(SIGUSR2-1)) );

	if( (current->TASK_SIGPENDING[0] & sigMask) &
		~(current->blocked.sig[0])){

		DBGWRT_ERR((DBH,"*** OSS_SemWait killed by deadly signal mask=0x%x\n",
					current->TASK_SIGPENDING[0]));
		TASK_UNLOCK_SIGNALS( current, flags );
		return 1;
	}

	/* non-deadly signal pending, block all signals 31..__SIGRTMAX (63) */
	DBGWRT_2((DBH,"OSS_SemWait interrupted by non-deadly sig\n"));
	*oldBlocked = current->blocked;
	sigorsets( &current->blocked, &current->blocked, &OSS_allRtSigs );
	RECALC_SIGPENDING();

	TASK_UNLOCK_SIGNALS( current, flags );

	return 0;
}

/**********************************************************************/
/** Wait for semaphore.
 *
 * \copydoc oss_specification.c::OSS_SemWait()
 *
 * See \ref linsemusagesect for more info.
 *
 * \linux
 * - all signals < \c SIGRTMIN are assumed to be deadly.
 * - LL drivers trying to ignore \b all signals while waiting for a
 * signal will cause a busy loop under linux, because the scheduler
 * awakes a process always immediately when a non masked signal is
 * pending.
 *
 * \sa OSS_SemCreate, OSS_SemRemove, OSS_SemSignal
 */
int32 OSS_SemWait(
    OSS_HANDLE      *oss,
    OSS_SEM_HANDLE  *sem,
    int32           msec)
{
	unsigned long flags;
	u_int32 ticks=0, i=0;
	sigset_t oldBlocked;
	int oldBlockedValid=FALSE, sigGot=FALSE;
	int32 error=0;
    DBGCMD( static const char functionName[] = "OSS_SemWait"; )

    DBGWRT_1((DBH,"%s sem = 0x%p\n", functionName, sem ));

	/* keep gcc from complaining about using possibly uninitialized values*/
	for (i = 0; i < _NSIG_WORDS; i++)
		oldBlocked.sig[i]=0;

	/*--- try to claim semaphore ---*/

	spin_lock_irqsave( &sem->lock, flags );

	/* sem available? */
	if( sem->value > 0 ){
		sem->value--;			/* ok, got the semaphore */
		DBGWRT_2((DBH, " got sem immediately\n"));
		spin_unlock_irqrestore( &sem->lock, flags );
		return 0;
	}

	/* didn't get the semaphore, now block until semaphore is released */
	
	/* wait until timeout? */
	if( msec > 0 ){
		/*--- round time, and correct for timer inaccuracy ---*/
		ticks = (msec * HZ) / 1000;
		ticks++;

		if( (msec * HZ) % 1000 )
			ticks++;
	}
	/* no wait */
	else if( msec == OSS_SEM_NOWAIT ){
		DBGWRT_2((DBH, " sem not avail\n"));
		spin_unlock_irqrestore( &sem->lock, flags );
		return ERR_OSS_TIMEOUT;	
	}

	/* sem->lock is locked here */
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,13,0)
		wait_queue_t __wait;
#else
		wait_queue_entry_t __wait;
#endif
		init_waitqueue_entry(&__wait, current);	

		add_wait_queue( &sem->wq, &__wait);
		
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);

			/* sem available? */
			if( sem->value > 0 ){
				sem->value--;			/* ok, got the semaphore */
				sigGot = TRUE;				
				spin_unlock_irqrestore( &sem->lock, flags );
				DBGWRT_2((DBH, " got sem\n"));
				break;
			}
			
			/* sem NOT available */	
			spin_unlock_irqrestore( &sem->lock, flags );

			if (!signal_pending(current)) {
				if( msec != OSS_SEM_WAITFOREVER ){
					ticks = schedule_timeout( ticks );
					if (!ticks)		/* time elapsed */
						break;
				}
				else {
					/* wait forever */
					schedule();
				}
			}
			else {
				/* signal pending */
				if( HandleSig( oss, &oldBlocked )){
					/* the signal was deadly, */
					error = ERR_OSS_SIG_OCCURED;
					break;
				}
				else {
					/* non deadly signal */
					oldBlockedValid = TRUE;
				}
			}
			spin_lock_irqsave( &sem->lock, flags );
		} /* for */
		
		set_current_state(TASK_RUNNING);
		remove_wait_queue( &sem->wq, &__wait);
	}

	if( sigGot == FALSE ){		
		/* sem->lock is unlocked here */
		spin_lock_irqsave( &sem->lock, flags );
		
		/* sem available? */
		if( sem->value > 0 ){
			sem->value--;			/* ok, got the semaphore */
			DBGWRT_2((DBH, " got sem\n"));
			error = 0;
		}
		spin_unlock_irqrestore( &sem->lock, flags );

		if( error == 0 && ticks == 0 && msec != OSS_SEM_WAITFOREVER){
			DBGWRT_ERR((DBH,"*** %s timeout waiting for sem\n", functionName ));
			error = ERR_OSS_TIMEOUT;
		}
	}
		
	if( oldBlockedValid ){
		TASK_LOCK_SIGNALS( current, flags );
		/* restore org. process signal mask */
		current->blocked = oldBlocked;
		RECALC_SIGPENDING();
		TASK_UNLOCK_SIGNALS( current, flags );
	}


    return error;
}/*OSS_SemWait*/



/**********************************************************************/
/** Signal semaphore.
 *
 * \copydoc oss_specification.c::OSS_SemSignal()
 *
 * See \ref linsemusagesect for more info.
 *
 * \sa OSS_SemCreate, OSS_SemRemove, OSS_SemWait
 */
int32 OSS_SemSignal(
    OSS_HANDLE *oss,
    OSS_SEM_HANDLE* sem)
{
	unsigned long flags;
    DBGCMD( static const char functionName[] = "OSS_SemSignal"; )

    DBGWRT_1((DBH,"%s() sem = 0x%p\n", functionName, sem ));

	spin_lock_irqsave( &sem->lock, flags );

	if( sem->semType == OSS_SEM_BIN )
		sem->value = 1;
	else
		sem->value++;

	spin_unlock_irqrestore( &sem->lock, flags );

	wake_up_interruptible( &sem->wq ); /* wake up any waiting processes */

    return(0);
}/*OSS_SemSignal*/



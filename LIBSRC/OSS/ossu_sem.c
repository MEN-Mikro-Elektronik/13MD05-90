/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_sem.c
 *
 *      \author  christian.schuster@men.de
 *        $Date: 2005/07/08 11:32:59 $
 *    $Revision: 2.1 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Semaphore routines for user space
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ossu_sem.c,v $
 * Revision 2.1  2005/07/08 11:32:59  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#define _OSS_SEM_C
#include "oss_intern.h"

/*! \page linsemusage

  \section linusrsemusagesect Linux notes to OSS semaphores

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
 * \linux \linrtai For RTAI implementation, see OSS_RtaiSemCreate().
 *
 * \sa OSS_SemRemove, OSS_SemWait, OSS_SemSignal
 */
int32 OSS_SemCreate(
    OSS_HANDLE 	   *oss,
    int32          semType,
    int32          initVal,
    OSS_SEM_HANDLE **semP)
{
    DBGWRT_1((DBH,"OSS_SemCreate() -- NOP\n"));

    return( 0 );
}/*OSS_SemCreate*/

/**********************************************************************/
/** Destroy semaphore handle.
 *
 * \copydoc oss_specification.c::OSS_SemRemove()
 *
 * See \ref linsemusagesect for more info.
 *
 * \linux \linrtai For RTAI implementation, see OSS_RtaiSemRemove().
 *
 * \sa OSS_SemCreate, OSS_SemWait, OSS_SemSignal
 */
int32 OSS_SemRemove(
    OSS_HANDLE *oss,
    OSS_SEM_HANDLE** semHandleP)
{
    DBGWRT_1((DBH,"OSS_SemRemove() -- NOP\n"));

    return( 0 );
}/*OSS_SemRemove*/

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
 * - \linrtai For RTAI implementation, see OSS_RtaiSemWait().
 *
 * \sa OSS_SemCreate, OSS_SemRemove, OSS_SemSignal
 */
int32 OSS_SemWait(
    OSS_HANDLE      *oss,
    OSS_SEM_HANDLE  *sem,
    int32           msec)
{
    DBGWRT_1((DBH,"OSS_SemWait -- NOP\n" ));

    return( 0 );
}/*OSS_SemWait*/



/**********************************************************************/
/** Signal semaphore.
 *
 * \copydoc oss_specification.c::OSS_SemSignal()
 *
 * See \ref linsemusagesect for more info.
 *
 * \linux \linrtai For RTAI implementation, see OSS_RtaiSemSignal().
 *
 * \sa OSS_SemCreate, OSS_SemRemove, OSS_SemWait
 */
int32 OSS_SemSignal(
    OSS_HANDLE *oss,
    OSS_SEM_HANDLE* sem)
{
    DBGWRT_1((DBH,"OSS_SemSignal() -- NOP\n"));

    return( 0 );
}/*OSS_SemSignal*/

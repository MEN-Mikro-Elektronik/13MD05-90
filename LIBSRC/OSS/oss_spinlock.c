/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_spinlock.c
 *
 *      \author  christine.ruff@men.de
 *        $Date: 2012/10/11 13:58:05 $
 *    $Revision: 2.4 $
 *
 *	   \project  MDIS4Linux (OSS lib)
 *  	 \brief  Spinlock functions
 *
 *    \switches  DBG - enable debugging
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_spinlock.c,v $
 * Revision 2.4  2012/10/11 13:58:05  ts
 * R: 1a. spinlock usage in bb_chameleon.c is unnecessary under linux
 *    1b. spin_lock function caused CPU lock up under kernels 3.x
 * M: 1. replaced functions to return always ERR_SUCCESS so bb_chameleon is
 *       unchanged
 *
 * Revision 2.3  2012/04/25 18:07:24  ts
 * R: kernel oops when board handler finally closed
 * M: removed OSS_MemFree because OSS_MemAlloc was removed in OSS_SpinLockCreate
 *
 * Revision 2.2  2012/04/23 13:53:02  ts
 * R: update for kernels 3.x, spinlock declaration changed
 * M: init spinlock with DEFINE_SPINLOCK if defined
 *
 * Revision 2.1  2011/04/04 17:32:38  CRuff
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2011 MEN Mikro Elektronik GmbH. All rights reserved.
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

/**********************************************************************/
/** Create a spin lock.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockCreate()
 *
 *  \windows IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockRemove
 */
int32 OSS_SpinLockCreate(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    **spinlP )
{
	OSS_SPINL_HANDLE hdlP;
	*spinlP = &hdlP;
    return ERR_SUCCESS;
}

/**********************************************************************/
/** Destroy spin lock handle.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockRemove()
 *
 *  \windows Do nothing\n
 *  IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockCreate
 */
int32 OSS_SpinLockRemove(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    **spinlP )
{
    DBGWRT_1((DBH,  "<%s> :OSS_SpinLockRemove: entered\n", oss->instName ));
	*spinlP = NULL;
    return ERR_SUCCESS;
}

/**********************************************************************/
/** Acquire spin lock.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockAcquire()
 *
 *  \windows IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockRelease
 */
int32 OSS_SpinLockAcquire(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    *spinl )
{
	DBGWRT_1((DBH,  "<%s> :OSS_SpinLockAcquire: entered\n",
	                oss->instName ));
	return ERR_SUCCESS;
}

/**********************************************************************/
/** Release spin lock.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockRelease()
 *
 *  \windows IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockAcquire
 */
int32 OSS_SpinLockRelease(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    *spinl )
{
	DBGWRT_1((DBH,  "<%s> :OSS_SpinLockRelease: entered\n",
				oss->instName ));
    return ERR_SUCCESS;
}

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_task.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2005/07/07 17:17:41 $
 *    $Revision: 1.4 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Task routines
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_task.c,v $
 * Revision 1.4  2005/07/07 17:17:41  cs
 * Copyright line changed
 *
 * Revision 1.3  2003/04/11 16:13:38  kp
 * Comments changed to Doxygen
 *
 * Revision 1.2  2003/02/21 11:25:17  kp
 * added RTAI dispatching functions
 *
 * Revision 1.1  2001/01/19 14:39:18  kp
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

/**********************************************************************/
/** Get current task id.
 *
 * \copydoc oss_specification.c::OSS_GetPid()
 *
 * \linux \linrtai For RTAI implementation, see OSS_RtaiGetPid().
 */
u_int32 OSS_GetPid(OSS_HANDLE *oss)
{
	pid_t pid;
	pid = current->pid;
	DBGWRT_1((DBH,"OSS_GetPid = %d\n", pid));

	return(pid);
}/*OSS_GetPid*/

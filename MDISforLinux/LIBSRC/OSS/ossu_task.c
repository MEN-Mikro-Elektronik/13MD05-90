/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_task.c
 *
 *      \author  christian.schuster@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Task routines for user_space
 *
 *    \switches
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2005-2019, MEN Mikro Elektronik GmbH
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
 */
u_int32 OSS_GetPid(OSS_HANDLE *oss)
{
	pid_t pid = getpid();

	DBGWRT_1((DBH,"OSS_USR_GetPid = %d\n", (int)pid));

	return(pid);
}/*OSS_GetPid*/

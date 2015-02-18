/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_task.c
 *
 *      \author  christian.schuster@men.de
 *        $Date: 2005/07/08 11:33:00 $
 *    $Revision: 2.1 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Task routines for user_space
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ossu_task.c,v $
 * Revision 2.1  2005/07/08 11:33:00  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
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

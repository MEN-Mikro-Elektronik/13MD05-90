/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_resource.c
 *
 *      \author  christian.schuster@men.de
 *        $Date: 2005/07/08 11:32:58 $
 *    $Revision: 2.1 $
 *
 *	   \project  MDISforLinux
 *  	 \brief  Resource related routines of the OSS module for user space
 *
 *    \switches  -
 */
/*
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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
/** Assign memory, I/O, interrupt resources to a driver
 *
 * \copydoc oss_specification.c::OSS_AssignResources()
 *
 * \sa OSS_UnAssignResources
 */
int32 OSS_AssignResources(
    OSS_HANDLE      *oss,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
    OSS_RESOURCES   res[]
)
{
	DBGWRT_1((DBH,"OSS_AssignResources: entered NOOP\n"));
	return 0;

}



 /************************* OSS_UnAssignResources *****************************
 *
 *  Description:  UnAssign the resources e.g. memory space, interrupts... .
 *
 *				  Undo function to OSS_AssignResources
 *---------------------------------------------------------------------------
 *  Input......:  oss   OSS handle
 *                busType OSS_BUSTYPE_NONE | VME | PCI | ISA
 *                busNbr  e.g. pci bus nbr. 5
 *                resNbr  number of resources
 *                res[]   resource field
 *
 *  Output.....:  0 | error code
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
int32 OSS_UnAssignResources(
    OSS_HANDLE      *oss,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
    OSS_RESOURCES   res[]
)
{
	DBGWRT_1((DBH, "OSS_UnAssignResources: entered NOOP\n"));
	return 0;
}

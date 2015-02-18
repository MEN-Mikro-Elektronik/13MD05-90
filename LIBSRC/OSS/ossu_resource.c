/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_resource.c
 *
 *      \author  christian.schuster@men.de
 *        $Date: 2005/07/08 11:32:58 $
 *    $Revision: 2.1 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Resource related routines of the OSS module for user space
 *
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ossu_resource.c,v $
 * Revision 2.1  2005/07/08 11:32:58  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
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

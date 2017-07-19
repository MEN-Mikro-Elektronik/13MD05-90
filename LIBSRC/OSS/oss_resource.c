/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_resource.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2005/07/07 17:17:28 $
 *    $Revision: 1.4 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Resource related routines of the OSS module
 *
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_resource.c,v $
 * Revision 1.4  2005/07/07 17:17:28  cs
 * Copyright line changed
 *
 * Revision 1.3  2003/04/11 16:13:29  kp
 * Comments changed to Doxygen
 *
 * Revision 1.2  2001/08/16 09:59:40  kp
 * added debug-msg in OSS_UnAssignResources
 *
 * Revision 1.1  2001/01/19 14:39:14  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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
/** Assign memory, I/O, interrupt resources to a driver
 *
 * \copydoc oss_specification.c::OSS_AssignResources()
 *
 * \linux IRQs cannot be registered using this function. This function
 * is used only from BBIS drivers.  MDIS kernel uses direct Linux calls.
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
	int i;	
	int32 error = ERR_SUCCESS;
	OSS_RESOURCES *r = res;

	DBGWRT_1((DBH,"OSS_AssignResources\n"));
	for( i=0; i<resNbr; i++, r++ ){	

		switch( r->type ){

		case OSS_RES_MEM:
			DBGWRT_2((DBH," MEM: phys=0x%p size=0x%x\n", r->u.mem.physAddr, r->u.mem.size ));
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
			if( check_mem_region( (unsigned long)r->u.mem.physAddr, r->u.mem.size ))
			{
				error = ERR_OSS_BUSY_RESOURCE;
				break;
			}
#endif
			request_mem_region( (unsigned long)r->u.mem.physAddr,
								r->u.mem.size, oss->instName );
			break;
			
		case OSS_RES_IO:
			DBGWRT_2((DBH," IO: phys=0x%p size=0x%x\n",
					  r->u.mem.physAddr, r->u.mem.size ));
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
			if( check_region( (unsigned long)r->u.mem.physAddr, r->u.mem.size )){
				error = ERR_OSS_BUSY_RESOURCE;
				break;
			}
#endif
			request_region( (unsigned long)r->u.mem.physAddr, r->u.mem.size, oss->instName );
			break;
			
		case OSS_RES_IRQ:
			/*
			 * Note: IRQ not claimed here.
			 */
			DBGWRT_2((DBH," IRQ: level=%d level=%d\n",
					  r->u.irq.level, r->u.irq.vector ));
			break;
		default:
			error = ERR_OSS_UNK_RESOURCE;
			break;
		}
		if( error ) {
			DBGWRT_ERR((DBH,"*** OSS_AssignResources failed\n"));
			break;
		}
	}
    return error;
}



 /************************* OSS_UnAssignResources *****************************
 *
 *  Description:  UnAssign the resources e.g. memory space, interrupts... .
 *
 *				  Undo function to OSS_AssignResources
 *				  Linux Note: IRQs cannot be released using this function
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
	int i;	
	OSS_RESOURCES *r = res;

	DBGWRT_1((DBH,"OSS_UnAssignResources\n"));
	for( i=0; i<resNbr; i++, r++ ){	

		switch( r->type ){

		case OSS_RES_MEM:
			DBGWRT_2((DBH," MEM: phys=0x%p size=0x%x\n",
					  r->u.mem.physAddr, r->u.mem.size ));
			release_mem_region( (unsigned long)r->u.mem.physAddr,
								r->u.mem.size);
			break;
			
		case OSS_RES_IO:
			DBGWRT_2((DBH," IO: phys=0x%p size=0x%x\n",
					  r->u.mem.physAddr, r->u.mem.size ));
			release_region( (unsigned long)r->u.mem.physAddr,
								   r->u.mem.size);
			break;
			
		case OSS_RES_IRQ:
			/*
			 * Note: IRQ not released here.
			 */
			DBGWRT_2((DBH," IRQ: level=%d level=%d\n",
					  r->u.irq.level, r->u.irq.vector ));
			break;
		default:
			DBGWRT_ERR((DBH,"*** OSS_UnAssignResources: illegal resource type"
						" %d\n", r->type ));
			break;
		}
	}
    return ERR_SUCCESS;
}



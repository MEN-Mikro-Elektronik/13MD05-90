/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_map.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2005/07/07 17:17:22 $
 *    $Revision: 1.4 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Memory mapping related routines of the OSS module
 *
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_map.c,v $
 * Revision 1.4  2005/07/07 17:17:22  cs
 * Copyright line changed
 *
 * Revision 1.3  2004/06/09 09:25:01  kp
 * added support for UML (for testing)
 *
 * Revision 1.2  2003/04/11 16:13:25  kp
 * Comments changed to Doxygen
 *
 * Revision 1.1  2001/01/19 14:39:12  kp
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
/** Map physical address space to virtual address space
 * \copydoc oss_specification.c::OSS_MapPhysToVirtAddr()
 * \sa OSS_UnMapVirtAddr()
 */
int32 OSS_MapPhysToVirtAddr(
    OSS_HANDLE *oss,
    void       *physAddr,
    u_int32    size,
	int32	   addrSpace,
    int32      busType,
    int32      busNbr,
    void       **virtAddrP
)
{
	int32 error = ERR_SUCCESS;

    DBGWRT_1((DBH,"OSS - OSS_MapPhysToVirtAddr: phys=0x%08x\n",physAddr));

#ifndef CONFIG_USERMODE
	if( addrSpace == OSS_ADDRSPACE_MEM ) {
		*virtAddrP = ioremap_nocache( (unsigned long)physAddr, size );
		if( *virtAddrP == NULL )
			error = ERR_OSS_MAP_FAILED;
	}
	else {
		/* for I/O no similar mapping exists under linux */
		*virtAddrP = physAddr;	
	}
	DBGWRT_2((DBH," virtAddr=0x%p error=0x%x\n", *virtAddrP, error ));
#else
	/*
	 * UML has no I/O or mem mapped I/O, return some arbitrary value
	 * just to satisfy BBIS/MDIS driver (ISA with MT)
	 */
	error = 0;
	*virtAddrP = (void *)0xcafebabe;	
#endif
	return(error);
}

/**********************************************************************/
/** Unmap virtual address space
 * \copydoc oss_specification.c::OSS_UnMapVirtAddr()
 * \sa OSS_MapVirtAddr()
 */
int32 OSS_UnMapVirtAddr(
    OSS_HANDLE *oss,
    void       **virtAddrP,
    u_int32    size,
	int32	   addrSpace
)
{
    DBGWRT_1((DBH,"OSS - OSS_UnMapVirtAddr: virt=0x%08x\n",*virtAddrP));
#ifndef CONFIG_USERMODE
	if( addrSpace == OSS_ADDRSPACE_MEM ) {
		iounmap( *virtAddrP );
	}
#endif
	*virtAddrP = NULL;
	return(ERR_SUCCESS);		
}


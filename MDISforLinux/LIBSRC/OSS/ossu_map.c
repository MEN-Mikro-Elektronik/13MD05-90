/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_map.c
 *
 *      \author  christian.schuster@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Memory mapping related routines of the OSS module, user space
 *
 *    \switches  OSS_USR_IO_MAPPED_ACC_EN
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

    DBGWRT_1((DBH,"OSS_USR - OSS_MapPhysToVirtAddr: phys=0x%08x addrSpace=%s\n",
		physAddr,
		(addrSpace==OSS_ADDRSPACE_MEM) ? "Memory mapped" :
		(addrSpace==OSS_ADDRSPACE_IO)  ? "I/O mapped"    : "unknown"));

	if( busType == OSS_BUSTYPE_PCI ||
		busType == OSS_BUSTYPE_NONE ){
		if( addrSpace == OSS_ADDRSPACE_MEM ) {
			int32 pagesize = sysconf(_SC_PAGESIZE);
			int32 map_size = size;
			long map_start = (long)physAddr & ~(pagesize-1);

			if((long)physAddr & (pagesize-1)) {
				map_size = size + ((long)physAddr - map_start);
			}

			if(map_size >= pagesize) {
				if( map_size%pagesize )
					map_size += pagesize;
				map_size = pagesize * (map_size/pagesize);
			} else
				map_size = pagesize;

			DBGWRT_3((DBH,"OSS_USR - pagesize = 0x%08x; map_size = 0x%08x\n",
					 pagesize, map_size));

			/* mmap offset parameter must be a multiple of the page size */
			*virtAddrP = mmap( NULL, map_size, PROT_READ|PROT_WRITE,
								MAP_SHARED, OSS_Memdev, map_start );

			if( *virtAddrP == MAP_FAILED ){
				error = ERR_OSS_MAP_FAILED;
				DBGWRT_ERR((DBH,"OSS_USR - ERROR: Couldn't map physical memory\n"));
			}
			*virtAddrP = (void*)((U_INT32_OR_64)*virtAddrP | ((long)physAddr & (pagesize-1)));

		}
#ifdef OSS_USR_IO_MAPPED_ACC_EN
		else if( addrSpace == OSS_ADDRSPACE_IO &&
		 		 busType == OSS_BUSTYPE_PCI      ) {
			/* for I/O no similar mapping exists under linux */
			*virtAddrP = physAddr;
			if( iopl(3) != 0) {
				DBGWRT_ERR((DBH,"OSS_USR - ERROR:"
								"iopl failed, root privileges needed\n"
								"                 IO-mapped memory access"
								"will not work\n"));
			}
		}
#endif
		else {
			DBGWRT_ERR((DBH,"OSS_USR - ERROR: unknown addrSpace\n"));
			error = ERR_OSS_MAP_FAILED;
		}
	}
	else if ( busType == OSS_BUSTYPE_VME ){
		DBGWRT_1((DBH,"OSS_USR - OSS_MapPhysToVirtAddr: OSS_BUSTYPE_VME: use MapVmeAddr()\n"));
		*virtAddrP = physAddr;
	}
	else
		error = ERR_OSS_UNK_BUSTYPE;

	DBGWRT_2((DBH," virtAddr=0x%p error=0x%x\n", *virtAddrP, error ));
	return(error);
}

/**********************************************************************/
/** Unmap virtual address space
 * \copydoc oss_specification.c::OSS_UnMapVirtAddr()
 *
 * NOTE: This function is not to be called for spaces allocated with
 *       OSS_MapVmeAddr() since we can not see if it is such an address
 *       here.
 *
 * \sa MapPhysToVirtAddr()
 */
int32 OSS_UnMapVirtAddr(
    OSS_HANDLE *oss,
    void       **virtAddrP,
    u_int32    size,
	int32	   addrSpace
)
{
    DBGWRT_1((DBH,"OSS - OSS_UnMapVirtAddr: virt=0x%08x\n",*virtAddrP));
	if( addrSpace == OSS_ADDRSPACE_MEM ) {
		int32 pagesize = sysconf(_SC_PAGESIZE);
		int32 unmap_size = size;
		U_INT32_OR_64 unmap_start = (U_INT32_OR_64)*virtAddrP & ~(pagesize-1);

		if((U_INT32_OR_64)*virtAddrP & (pagesize-1)) {
			unmap_size = size + ((U_INT32_OR_64)*virtAddrP - unmap_start);
		}

		if(unmap_size >= pagesize) {
			if( unmap_size%pagesize )
				unmap_size += pagesize;
			unmap_size = pagesize * (unmap_size/pagesize);
		} else
			unmap_size = pagesize;
		munmap( (void*)unmap_start, unmap_size );
	}
	*virtAddrP = NULL;
	return(ERR_SUCCESS);
}

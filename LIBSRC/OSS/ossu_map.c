/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_map.c
 *
 *      \author  christian.schuster@men.de
 *        $Date: 2014/01/20 17:43:26 $
 *    $Revision: 2.3 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Memory mapping related routines of the OSS module, user space
 *
 *    \switches  OSS_USR_IO_MAPPED_ACC_EN
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ossu_map.c,v $
 * Revision 2.3  2014/01/20 17:43:26  awerner
 * R: fpga_load segmentation fault in 64bit Linux
 * M: Added support for 64bit in ossu_map.c (Cast U_INT32_OR_64)
 *
 * Revision 2.2  2007/07/13 17:49:16  cs
 * added support for OSS_BUSTYPE_NONE (handle like OSS_BUSTYPE_PCI)
 *
 * Revision 2.1  2005/07/08 11:32:56  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
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
			u_int32 map_start = (u_int32)physAddr & ~(pagesize-1);

			if((u_int32)physAddr & (pagesize-1)) {
				map_size = size + ((u_int32)physAddr - map_start);
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
			*virtAddrP = (void*)((U_INT32_OR_64)*virtAddrP | ((u_int32)physAddr & (pagesize-1)));

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

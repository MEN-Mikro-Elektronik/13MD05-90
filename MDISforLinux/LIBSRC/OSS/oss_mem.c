/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_mem.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Memory handling functions of the OSS module
 *
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
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
/** Allocates general memory block.	
 *
 * \copydoc oss_specification.c::OSS_MemGet()
 *
 * \linux Puts calling process to sleep if no free pages available (i.e.
 *	should never return NULL).
 *
 * \sa OSS_MemFree
 */
void* OSS_MemGet(
    OSS_HANDLE  *oss,
    u_int32     size,
    u_int32     *gotsizeP
)
{

	void *mem = kmalloc( size, GFP_KERNEL );
	if( mem != NULL )
		*gotsizeP = size;
	else
		*gotsizeP = 0;

	DBGWRT_1((DBH,"OSS_MemGet (Lin): size=0x%lx allocated addr=0x%p\n",
			  size, mem ));
	return mem;
}/*OSS_MemGet*/

/**********************************************************************/
/** Return memory block.
 *
 * \copydoc oss_specification.c::OSS_MemFree()
 *
 * \sa OSS_MemGet
 */
int32 OSS_MemFree(
    OSS_HANDLE *oss,
    void       *addr,
    u_int32    size
)
{
	DBGWRT_1((DBH,"OSS_MemFree (Lin): addr=0x%p size=0x%lx\n", addr, size));
	kfree(addr);
    return(0);
}/*OSS_MemFree*/


/**********************************************************************/
/** Check if memory block accessible by caller
 *
 * \copydoc oss_specification.c::OSS_MemChk()
 *
 * \linux This function is a no-op. When used in MDIS context, all user
 * buffers are copied first from user space and are therefore always
 * valid kernel addresses.
 */
int32 OSS_MemChk(
    OSS_HANDLE *oss,
    void       *addr,
    u_int32    size,
    int32      mode)
{
	return 0;
}



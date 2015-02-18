/***********************  I n c l u d e  -  F i l e  ***********************/
/**
 *         \file cha_ut.h
 *
 *       \author dieter.pfeuffer@men.de
 *        $Date: 2008/03/31 11:31:48 $
 *    $Revision: 1.1 $
 *
 *        \brief Internal header file for chameleon library UT
 *
 *     Switches: _LITTLE_ENDIAN_ / _BIG_ENDIAN_
 *               _DOXYGEN_ - to build proper doxygen documentation
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: cha_ut.h,v $
 * Revision 1.1  2008/03/31 11:31:48  DPfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _CHA_INT_H
#define _CHA_INT_H

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_err.h>

/*--------------------------------------*/
/*	DEFINES			            		*/
/*--------------------------------------*/
#define _OSS_H_		/* disable oss.h */
#define _MACCESS_H	/* disable maccess.h */
typedef volatile void* MACCESS;

#define OSS_HANDLE void*
#define CHAMELEONV2_EXPORT_CHAMELEONV0

#define OSS_DL_NODE     UOS_DL_NODE
#define OSS_DL_LIST     UOS_DL_LIST
#define OSS_DL_NewList  UOS_DL_NewList
#define OSS_DL_AddTail  UOS_DL_AddTail
#define OSS_DL_Remove   UOS_DL_Remove
#define OSS_DL_RemHead  UOS_DL_RemHead

#define OSS_PCI_VENDOR_ID   1
#define OSS_PCI_DEVICE_ID   2
#define OSS_PCI_ADDR_0      13
#define OSS_PCI_ADDR_1      14
#define OSS_PCI_ADDR_2      15
#define OSS_PCI_ADDR_3      16
#define OSS_PCI_ADDR_4      17
#define OSS_PCI_ADDR_5      18

#define OSS_ADDRSPACE_MEM         0
#define OSS_ADDRSPACE_IO          1
#define OSS_BUSTYPE_NONE        0   /* device is local */
#define OSS_BUSTYPE_PCI         2

/* swap macros */
#   ifdef MAC_BYTESWAP
#       define RSWAP16(a)   OSS_Swap16(a)
#       define RSWAP32(a)   OSS_Swap32(a)
#   else
#       define RSWAP16(a)   (a)
#       define RSWAP32(a)   (a)
#   endif

#   define MREAD_D16(ma,offs) \
        RSWAP16(*(volatile u_int16*)((u_int8*)(ma)+(offs)))

#   define MREAD_D32(ma,offs) \
        RSWAP32(*(volatile u_int32*)((u_int8*)(ma)+(offs)))

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
u_int32 *G_chaTblBus0Bar0P;

/* OSS_MemGet(), OSS_MemFree() supervision: */
int32	G_allocBytes;	/* number of allocated bytes */
int32	G_allocCalls;	/* number of outstanding free calls */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
void OSS_MemFill(
		OSS_HANDLE	*osHdl,
		u_int32		size,
		char		*adr,
		int8		value);

void* OSS_MemGet(
		OSS_HANDLE	*osHdl,
		u_int32		size,
		u_int32		*gotsize);

int32 OSS_MemFree(
		OSS_HANDLE	*osHdl,
		void		*addr,
		u_int32		size);

int32 OSS_PciGetConfig(
		OSS_HANDLE	*osHdl,
		int32       bus,
		int32       dev,
		int32       func,
		int32       which,
		int32		*valueP);

int32 OSS_BusToPhysAddr(
		OSS_HANDLE	*osHdl,
		int32       busType,
		void		**physicalAddrP,
		...);

int32 OSS_MapPhysToVirtAddr(
		OSS_HANDLE	*osHdl,
		void		*physAddr,
		u_int32		size,
		int32       addrSpace,
		int32		busType,
		int32		bus,
		void		**virtAddrP);

int32 OSS_UnMapVirtAddr(
		OSS_HANDLE	*osHdl,
		void		**virtAddr,
		u_int32		size,
		int32       addrSpace);

int32 DBG_Init(
    char		*name,
    DBG_HANDLE	**dbgP);

int32  DBG_Exit(
    DBG_HANDLE	**dbgP);

int32 DBG_Write(
    DBG_HANDLE	*dbg,
    char		*fmt,
	... );


#ifdef __cplusplus
	}
#endif

#endif	/* _CHA_INT_H */




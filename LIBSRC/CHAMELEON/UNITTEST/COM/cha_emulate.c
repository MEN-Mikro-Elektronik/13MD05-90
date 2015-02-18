/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file cha_emulate.c
 *       \author dieter.pfeuffer@men.de
 *        $Date: 2008/03/31 11:31:47 $
 *    $Revision: 1.1 $
 *
 *        \brief Basic Unit Test for Chameleon library - emulations
 *
 *     Required: -
 *     \switches (none)
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: cha_emulate.c,v $
 * Revision 1.1  2008/03/31 11:31:47  DPfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "cha_ut.h"

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
void OSS_MemFill(
		OSS_HANDLE	*osHdl,
		u_int32		size,
		char		*adr,
		int8		value)
{
	char *p=adr;

	while( p < adr+size )
		*p++ = value;
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
void* OSS_MemGet(
		OSS_HANDLE	*osHdl,
		u_int32		size,
		u_int32		*gotsize)
{
	void *p;
	
	if( (p = malloc( size )) )
		*gotsize = size;

	if( p ){
		G_allocBytes += size;
		G_allocCalls++;
	}

	return p;
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32 OSS_MemFree(
		OSS_HANDLE	*osHdl,
		void		*addr,
		u_int32		size)
{
	free( addr );

	G_allocBytes -= size;
	G_allocCalls--;

	return(0);
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32 OSS_PciGetConfig(
		OSS_HANDLE	*osHdl,
		int32       bus,
		int32       dev,
		int32       func,
		int32       which,
		int32		*valueP)
{
		
	/*
	 * virtual PCI device:
	 */
	if( bus==1 && dev==10 && func==0 ){

		switch( which ){
		case OSS_PCI_VENDOR_ID:
			*valueP = 0x1172;
			break;

		case OSS_PCI_DEVICE_ID:
			*valueP = 0x4d45;
			break;

		case OSS_PCI_ADDR_0:
			*valueP = (u_int32)G_chaTblBus0Bar0P;
			break;
		case OSS_PCI_ADDR_1:
			*valueP = 0x11000000;
			break;
		case OSS_PCI_ADDR_2:
			*valueP = 0x22000000;
			break;
		case OSS_PCI_ADDR_3:
			*valueP = 0x33000000;
			break;
		case OSS_PCI_ADDR_4:
			*valueP = 0x00000010;
			break;

		default:
			*valueP = 0xffffffff;
			return -1;
		}
	}
	return 0;
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32 OSS_BusToPhysAddr(
		OSS_HANDLE	*osHdl,
		int32       busType,
		void		**physicalAddrP,
		...)
{
	int32		status = ERR_SUCCESS;
	va_list		argptr;
    int32		value;

	if( busType != OSS_BUSTYPE_PCI )
		return ERR_OSS_UNK_BUSTYPE;

	va_start(argptr,physicalAddrP);

	{		
		int32 bus		= va_arg( argptr, int32 );
		int32 dev		= va_arg( argptr, int32 );
		int32 func		= va_arg( argptr, int32 );
		int32 pciAddrNr	= va_arg( argptr, int32 );

		int32 which;
		u_int32 ourDev;

		switch( pciAddrNr )
		{
			case 0:	which = OSS_PCI_ADDR_0;	break;
			case 1:	which = OSS_PCI_ADDR_1;	break;
			case 2:	which = OSS_PCI_ADDR_2;	break;
			case 3:	which = OSS_PCI_ADDR_3;	break;
			case 4:	which = OSS_PCI_ADDR_4;	break;
			case 5:	which = OSS_PCI_ADDR_5;	break;
			default:
				status = ERR_OSS_PCI_ILL_ADDRNBR;
				goto CLEANUP;
		}

		/* get base address from PCI Config Space */
		status = OSS_PciGetConfig( osHdl, bus, dev, func, which, &value );

		if( status || (value == 0xffffffff) ){
			status = ERR_OSS_PCI;
			goto CLEANUP;
		}

		/* mask out lower bits */
		if( value & 0x1 )
			/* PCI I/O space - bit 1..0 */
			*physicalAddrP = (void*)(value & ~0x3);
		else
			/* PCI MEM space - bit 3..0 */
			*physicalAddrP = (void*)(value & ~0xf);
	}

CLEANUP:

	va_end( argptr );
	return status;
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32 OSS_MapPhysToVirtAddr(
		OSS_HANDLE	*osHdl,
		void		*physAddr,
		u_int32		size,
		int32       addrSpace,
		int32		busType,
		int32		bus,
		void		**virtAddrP)
{
	*virtAddrP = physAddr;
	return 0;
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32 OSS_UnMapVirtAddr(
		OSS_HANDLE	*osHdl,
		void		**virtAddr,
		u_int32		size,
		int32       addrSpace)
{
	return 0;
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32 DBG_Init(
    char		*name,
    DBG_HANDLE	**dbgP)
{
	return(0);
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32  DBG_Exit(
    DBG_HANDLE	**dbgP)
{
	return(0);
}

/* * * * * * * * * * * * * * * emulated funtion * * * * * * * * * * * * * * */
int32 DBG_Write(
    DBG_HANDLE	*dbg,
    char		*fmt,
	...
)
{
	char		linebuf[2000];
	char		*str = linebuf;
	va_list		argptr=NULL;

	va_start(argptr,fmt);

	vsprintf( str, fmt, argptr );

	printf(("%s",linebuf));
	
	va_end(argptr);

	return(0);
}

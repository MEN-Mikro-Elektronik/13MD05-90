/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_bustoaddr.c
 *
 *      \author  christian.schuster@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Bus Address translation routines and PCI access routines
 *               for user space
 *
 *    \switches  OSS_CONFIG_VME
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
#include <unistd.h> /* for sysconfig(_SC_PAGESIZE) */
#include "../pciutils/lib/pci.h"

#undef CONFIG_PCI
/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define OSS_PAGE_SIZE 			(sysconf(_SC_PAGESIZE))

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
#ifdef OSS_CONFIG_VME
struct _OSS_VME_SPC
{
	int spaceEnum;
	char* spaceStr;
} OSS_Vme4LSpaceMap[] =
{
	{VME4L_SPC_A16_D16,		"a16_d16"		},	/**< Short non priviledged (D16) */
	{VME4L_SPC_A24_D64_BLT,	"a24_d64_blt"	},	/**< Standard non priviledged (D64-BLT), n/a */
	{VME4L_SPC_A16_D32,		"a16_d32"		},	/**< Short non priviledged (D32) */
	/* VME4L_SPC_A16_D32_BLT removed */
	{VME4L_SPC_A24_D16,		"a24_d16"		},	/**< Standard non priviledged (D16) */
	{VME4L_SPC_A24_D16_BLT, "a24_d16_blt"	},	/**< Standard non priviledged (D16-BLT) */
	{VME4L_SPC_A24_D32,		"a24_d32"		},	/**< Standard non priviledged (D32) */
	{VME4L_SPC_A24_D32_BLT,	"a24_d32_blt"	},	/**< Standard non priviledged (D32-BLT) */
	{VME4L_SPC_A32_D32,		"a32_d32"		},	/**< Extended non priviledged (D32) */
	{VME4L_SPC_A32_D32_BLT,	"a32_d32_blt"	},	/**< Extended non priviledged (D32-BLT) */
	{VME4L_SPC_A32_D64_BLT,	"a32_d64_blt"	},	/**< Extended non priviledged (D64-BLT) */


	{VME4L_SPC_SLV0,		"slv0"			},	/**< slave window #0 */
	{VME4L_SPC_SLV1,		"slv1"			},	/**< slave window #1 */
	{VME4L_SPC_SLV2,		"slv2"			},	/**< slave window #2 */
	{VME4L_SPC_SLV3,		"slv3"			},	/**< slave window #3 */
	{VME4L_SPC_SLV4,		"slv4"			},	/**< slave window #4 */
	{VME4L_SPC_SLV5,		"slv5"			},	/**< slave window #5 */

	{VME4L_SPC_INVALID,		"spc_invalid"	},	/**< invalid space */
};
#define OSS_NBR_OF_VME4L_SPC_ENT sizeof(OSS_Vme4LSpaceMap)/sizeof(struct _OSS_VME_SPC)
#endif /* OSS_CONFIG_VME */

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifdef OSS_CONFIG_PCI
static int32 PciGetReg(
	OSS_HANDLE *oss,
	u_int32 which,
	int16 *idxP,
	int16 *accessP );
#endif

/**********************************************************************/
/** Convert bus address to CPU physical address.
 *
 * \copydoc oss_specification.c::OSS_BusToPhysAddr()
 *
 * \linux The functionality of this routine depends on the linux
 * kernel configuration:
 * - \c CONFIG_PCI: Must be enabled to support PCI.
 * - \c CONFIG_MEN_VME_KERNELIF: Must be enabled to support VME. This
 *      requires a VME kernel mode interface compatible to MEN's specification.
 *
 * \sa OSS_MapPhysToVirtAddr(), OSS_AssignResources()
 */
int32 OSS_BusToPhysAddr
(
    OSS_HANDLE *oss,
    int32       busType,
    void       **physicalAddrP,
    ...
)
{
	int32 error = ERR_SUCCESS;
    va_list argptr;

    va_start( argptr, physicalAddrP );

	DBGWRT_1((DBH,  "<%s> :OSS_BusToPhysAddr: entered:\n", oss->devName));

    *physicalAddrP = NULL;

    switch( busType )
    {
	case OSS_BUSTYPE_NONE:
		*physicalAddrP = va_arg( argptr, void* );
		break;

#ifdef OSS_CONFIG_PCI
	case OSS_BUSTYPE_PCI:
	{
		int32    busNbr      = va_arg( argptr, u_int32 );
		int32    pciDevNbr   = va_arg( argptr, u_int32 );
		int32    pciFunction = va_arg( argptr, u_int32 );
		int32    addrNbr     = va_arg( argptr, u_int32 );
		struct pci_dev *pciDev = NULL;

		oss->pciFilter.bus  = busNbr;
		oss->pciFilter.slot = pciDevNbr;
		oss->pciFilter.func = pciFunction;

		for(pciDev=oss->pciAcc->devices; pciDev; pciDev=pciDev->next)
			if (pci_filter_match( &oss->pciFilter, pciDev))
				break;

		if( !pciDev ){
			error = ERR_OSS_PCI_NO_DEVINSLOT;
			break;
		}

		*physicalAddrP = (void *)pciDev->base_addr[addrNbr];
		break;
	}
#endif /*CONFIG_PCI*/

	case OSS_BUSTYPE_VME:
	{
		DBGWRT_ERR( ( DBH, "*** OSS_BusToPhysAddr: OSS_BUSTYPE_VME not supported (deprecated)\n"
							"    use OSS_MapVmeAddr()/OSS_UnMapVmeAddr()\n"));
		error = ERR_OSS_UNK_BUSTYPE;
		break;
	}
	default:
		DBGWRT_ERR( ( DBH, "*** OSS_BusToPhysAddr: bustype %d not supported\n",
					  busType ));
		error = ERR_OSS_UNK_BUSTYPE;
		break;
    }/*switch*/

    va_end( argptr );
	DBGWRT_2((DBH, " physAddr=0x%p error=0x%lx\n", *physicalAddrP, error ));
    return( error );
}/*OSS_BusToPhysAddr*/

#ifdef OSS_CONFIG_VME
/************************** OSS_MapVmeAddr **********************************
 *
 *  Description:  Map VMEbus address space into CPU physical address space.
 *
 * The function maps the VMEbus address space starting at address
 * vmeBusAddr with the size of size bytes into CPU local address
 * space and stores the corresponding local address into locAddrP.
 *
 * The used VMEbus address space (VMEbus address modifier) must be
 * specified using the addrMode parameter.
 * The maximum possible data access size on the VMEbus that should be used
 * for the pointer access with the returned local address must be set to
 * VME_DM_16 or VME_DM_32 through the maxDataMode parameter.
 *
 * The VMEbus address remains mapped until OSS_UnMapVmeAddr() is called
 * with the addrWinHdlP gotten from OSS_MapVmeAddr().
 *
 * If none VMEbus address windows is available for the specified parameters
 * the function returns with error code #ERR_OSS_NO_ADDR_WINDOW.
 *
 * The function's behavior can be altered using the flags parameter
 * (e.g. to specify a HW Swapping Mode or to enable Posted Write Access).
 *
 *---------------------------------------------------------------------------
 *  Input......:  oss			os handle
 *                vmeBusAddr	VME bus address relative to VME space.
 *                              Must be aligned to maxDataMode.
 *                addrMode		VME_AM_X address mode
 *                maxDataMode	maximum VMEbus data access size (VME_DM_16 or VME_DM_32)
 *                size			number of bytes to map. Should be aligned to maxDataMode.
 *                flags			bitwise OR of any of the following:
 *                                Hardware Swapping Modes
 *                                  VME_F_HWSWAP_MODE1 to use HW swapping mode #1
 *                                Posted Write Access
 *                                  VME_F_POSTWR use posted write instead of delayed write
 *                locAddrP		pointer to variable that receives the local address
 *                addrWinHdlP	pointer to variable that receives the address-window handle
 *
 *  Output.....:  *locAddrP		local address to access the mapped VMEbus address space
 *                *addrWinHdlP	address-window handle
 *				  return		error code
 *  Globals....:  -
 ****************************************************************************/
 extern int32 OSS_MapVmeAddr(
	OSS_HANDLE	*oss,
	u_int64 	vmeBusAddr,
	u_int32		addrMode,
	u_int32		maxDataMode,
	u_int32		size,
	u_int32		flags,
	void		**locAddrP,
	void		**addrWinHdlP )
{
	int found=FALSE, addrWindowGot = 0;
	OSS_VME_ADDRWIN_NODE *awN = NULL;
	VME4L_SPACE vmeSpace = VME4L_SPC_INVALID;
	int error = 0;
	char vmeSpcStr[15];
	u_int32 ii;
	vmeaddr_t lVmeBusAddr = vmeBusAddr;

	DBGWRT_1((DBH,  "<%s> :OSS_MapVmeAddr: entered:\n", oss->devName));
	DBGWRT_3((DBH,  "       BusAddr 0x%08x; addrMode 0x%02x dataMode 0x%02x\n"
					"       size:   0x%08x; flags 0x%08x\n",
					(u_int32)lVmeBusAddr, addrMode, maxDataMode, size, flags));

	*addrWinHdlP = NULL;

/*	 raising IRQL to DISPATCH_LEVEL
	 AcquireSpinLock();
*/
	/* scan the list of address windows in use and check
	 * if the requested address space matches to one window */
    for( awN=(OSS_VME_ADDRWIN_NODE *)OSS_VME_addrWinList.head;
         awN->n.next;
         awN = (OSS_VME_ADDRWIN_NODE *)awN->n.next ){

		/* check window type */
		if( OSS_VME_WinT_master != awN->winType )
			continue;

		/* check address mode */
		if( addrMode != awN->addrMode )
			continue;

		/* check max data mode */
		if( maxDataMode > awN->maxDataMode )
			continue;

		/* check address range */
		if( (  lVmeBusAddr < awN->vmeAddr ) ||	// addr-start too low?
			( (lVmeBusAddr + size) >			// addr-end too high?
			(awN->vmeAddr + awN->size)   ) )
			continue;

		/* check hw swapping */
		if( (flags & OSS_VME_HW_SWAP1) !=
			(awN->flags & OSS_VME_HW_SWAP1) )
			continue;

		/* check access flags */
		if( (flags & OSS_VME_ACC_MASK) !=
			(awN->flags & OSS_VME_ACC_MASK) )
			continue;

		/* if we are here, then the requested
		 * address space matches to the current window */
		found = TRUE;
		DBGWRT_3((DBH,  "OSS_MapVmeAddr: matching address window found:"
						" vmeAddr=0x%016I64x, size=0x%08x\n",
						awN->vmeAddr, awN->size));
		break;
	}

	/* we require a new window if no matching window was found */
	if( !found ){
		/* allocate a VMEK_ADDRWIN_NODE */
		DBGWRT_3((DBH,  "OSS_MapVmeAddr: OSS_VME_ADDRWIN_NODE (size=0x%x)\n",
						sizeof(OSS_VME_ADDRWIN_NODE)));
		awN = (OSS_VME_ADDRWIN_NODE *)malloc( sizeof(OSS_VME_ADDRWIN_NODE) );
		if( NULL == awN ){
			error = ERR_OSS_MEM_ALLOC;
			goto CLEANUP;
		}

		// zero mem
		OSS_MemFill(oss, sizeof(OSS_VME_ADDRWIN_NODE), (char*)awN, 0);

		// init node
		awN->winType	 = OSS_VME_WinT_master;
		awN->vmeAddr	 = lVmeBusAddr;
		awN->addrMode	 = addrMode;
		awN->maxDataMode = maxDataMode;
		awN->size        = size;
		awN->flags       = flags;

		switch ( addrMode & 0x00f0 ) /* check address size */
		{
			case ( OSS_VME_AM_16UD & 0x00f0 ):
				strcpy(vmeSpcStr, "a16_");
				break;
			case ( OSS_VME_AM_24UD & 0x00f0 ):
				strcpy(vmeSpcStr, "a24_");
				break;
			case ( OSS_VME_AM_32UD & 0x00f0 ):
				strcpy(vmeSpcStr, "a32_");
				break;
			default:
				error = ERR_OSS_ILL_PARAM;
				goto CLEANUP;
		}
		switch ( maxDataMode ) /* check data mode */
		{
			case ( OSS_VME_DM_8 ):
				strcat(vmeSpcStr, "d16");
				break;
			case ( OSS_VME_DM_16 ):
				strcat(vmeSpcStr, "d16");
				break;
			case ( OSS_VME_DM_32 ):
				strcat(vmeSpcStr, "d32");
				break;
			case ( OSS_VME_DM_64 ):
				strcat(vmeSpcStr, "d64");
				break;
			default:
				error = ERR_OSS_VME_ILL_SPACE;
				goto CLEANUP;
		}
		switch ( addrMode & 0x000f ) /* check address mode (BLT?) */
		{
/*			case ( OSS_VME_AM_24UB & 0x0f ): duplicate case */
			case ( OSS_VME_AM_32UB & 0x0f ):
			case ( OSS_VME_AM_32UB64  & 0x0f ):
				strcat(vmeSpcStr, "_blt");
			default:
				break;
		}

		for (ii=0; ii < OSS_NBR_OF_VME4L_SPC_ENT; ii++) {
			/* does VME space match? */
			if ( !strcmp( vmeSpcStr, OSS_Vme4LSpaceMap[ii].spaceStr ) ) {
				vmeSpace = OSS_Vme4LSpaceMap[ii].spaceEnum;
				break;
			}
		}

		DBGWRT_3((DBH, "OSS_MapVmeAddr: vmeSpcStr=\"%s\" vmeSpace=%d\n",
						vmeSpcStr, vmeSpace));

		awN->fd = VME4L_Open( vmeSpace );
		if( !awN->fd ) {
			DBGWRT_ERR((DBH, "\n*** OSS_MapVmeAddr ERROR opening file for"
							 "VME Space \"%s\"\n",
							 VME4L_SpaceName(vmeSpace)));
			error = ERR_OSS_VME_ILL_SPACE;
			goto CLEANUP;
		}

		if( (flags & OSS_VME_HW_SWAP1) &&
			(VME4L_SwapModeSet( awN->fd, VME4L_HW_SWAP1  ) != 0) ){
			DBGWRT_ERR((DBH, "\n*** OSS_MapVmeAddr ERROR setting swapping mode\n"));
			error = ERR_OSS_VME;
			goto CLEANUP;
		}

		if( VME4L_Map(  awN->fd,
						(vmeaddr_t)(lVmeBusAddr & ~(OSS_PAGE_SIZE-1)),
						awN->size,
						(void**)&awN->locAddr ) ) {
			DBGWRT_ERR((DBH, "\n*** OSS_MapVmeAddr ERROR mapping VME address"
							 "0x%08x\n",
							 (u_int32)(lVmeBusAddr & ~(OSS_PAGE_SIZE-1))));
			error = ERR_OSS_VME_ILL_SPACE;
			goto CLEANUP;
		}

		awN->locAddr |= (u_int32)(awN->vmeAddr & (OSS_PAGE_SIZE-1));

		addrWindowGot = TRUE;

		/* add node to list */
		OSS_DL_AddTail( &OSS_VME_addrWinList, &awN->n );
	}

	/* one more usage */
	awN->useNbr++;

	*locAddrP = (void*)awN->locAddr;
	/* return the VMEbus address window */
	*addrWinHdlP=awN;

CLEANUP:

	/* error? */
	if(error){

		if( addrWindowGot && awN->locAddr )
			VME4L_UnMap( awN->fd, (void*)awN->locAddr, awN->size);

		if( addrWindowGot )
			VME4L_Close( awN->fd );

		if( awN )
			free( awN );
	}

	DBGWRT_1((DBH, "OSS_MapVmeAddr exit: vmeBusAddr=0x%08x vmeMappedAddr=0x%08x\n",
				(u_int32)awN->vmeAddr,
				awN->locAddr));

	return error;
}

/************************** OSS_UnMapVmeAddr ********************************
 *
 *  Description:  Unmap VMEbus address space
 *
 * The function unmaps an OSS_MapVmeAddr() mapped VMEbus address space,
 * specified by the corresponding addrWinHdl .
 *
 *---------------------------------------------------------------------------
 *  Input......:  oss			os handle
 *                addrWinHdl	Address-window handle
 *
 *  Output.....:  return		error code
 *  Globals....:  -
 ****************************************************************************/
extern int32 OSS_UnMapVmeAddr(
	OSS_HANDLE	*oss,
	void		*addrWinHdl )
{
	OSS_VME_ADDRWIN_NODE *awN = (OSS_VME_ADDRWIN_NODE*)addrWinHdl;
	DBGWRT_1((DBH,"<%s> : OSS_UnMapVmeAddr: entered\n", oss->devName));

	/* one less usage */
	awN->useNbr--;

	/* no longer in use? */
	if( !awN->useNbr ){

		/* unmap VMEbus space */
		VME4L_UnMap( awN->fd, (void*)awN->locAddr, awN->size);
		VME4L_Close( awN->fd );

		// remove it from the list
		OSS_DL_Remove( &awN->n );
		// free mem
		free( awN );
	}

	return 0;
}
#endif /* OSS_CONFIG_VME */
/**********************************************************************/
/** Read a register from PCI configuration space.
 *
 * \copydoc oss_specification.c::OSS_PciGetConfig()
 *
 * \linux interrupt line and BAR[0..5] is not read from config space.
 * Instead, the linux internal copy is returned
 *
 * \sa OSS_PciSetConfig, OSS_BusToPhysAddr
 */
int32 OSS_PciGetConfig(
    OSS_HANDLE *oss,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       *valueP
)
{
    int32   retCode = 0;
#ifdef OSS_CONFIG_PCI
	unsigned int	value32;
	unsigned short	value16;
	unsigned char	value8;
	struct pci_dev *pciDev;
	int16 idx, access;

    DBGWRT_2((DBH,"OSS_PciGetConfig bus %lx dev %lx func %lx which %lx\n",
			  busNbr, pciDevNbr, pciFunction, which));

    *valueP = 0;

	/*--- determine config reg offset and access ---*/
	if( (retCode = PciGetReg( oss, which, &idx, &access )))
		goto GETCFG_END;


	oss->pciFilter.bus  = busNbr;
	oss->pciFilter.slot = pciDevNbr;
	oss->pciFilter.func = pciFunction;

	for(pciDev=oss->pciAcc->devices; pciDev; pciDev=pciDev->next)
		if (pci_filter_match( &oss->pciFilter, pciDev))
			break;

	if( !pciDev ){
		/* non-existant device */
		*valueP = 0xffffffff & ((1L<<(access*8))-1);
		return 0;
	}

	/*
	 * handle special regs. These may be different in the devices
	 * header and linux internal representation (think of ACPI)
	 */
	switch( which ){
	case OSS_PCI_INTERRUPT_LINE:
		*valueP  = pciDev->irq;
		break;
	case OSS_PCI_ADDR_0:
		*valueP = pciDev->base_addr[0];
		break;
	case OSS_PCI_ADDR_1:
		*valueP = pciDev->base_addr[1];
		break;
	case OSS_PCI_ADDR_2:
		*valueP = pciDev->base_addr[2];
		break;
	case OSS_PCI_ADDR_3:
		*valueP = pciDev->base_addr[3];
		break;
	case OSS_PCI_ADDR_4:
		*valueP = pciDev->base_addr[4];
		break;
	case OSS_PCI_ADDR_5:
		*valueP = pciDev->base_addr[5];
		break;

	default:
		/*
		 * for the other registers, directly access PCI config space
		 */

		switch( access )
		{
        case 4:
			if( (value32 = pci_read_long(pciDev, idx)) == 0xFF )
				retCode = 1;
			*valueP = (u_int32)value32;
			break;

        case 2:
			if( (value16 = pci_read_word(pciDev, idx)) == 0xFF )
				retCode = 1;
			*valueP = (u_int32)value16;
			break;

	    case 1:
			if( (value8 = pci_read_byte(pciDev, idx)) == 0xFF )
				retCode = 1;
			*valueP = (u_int32)value8;
			break;
		}/*switch*/

		if( retCode )
		{
			DBGWRT_ERR(( DBH, "*** OSS_PciGetConfig: error 0x%x reading cfg "
						 "space: reg 0x%04x\n", retCode, idx ));
			retCode = ERR_OSS_PCI;
			*valueP = 0;
		}/*if*/
	}
	DBGWRT_2((DBH, "  value=0x%08x\n", *valueP));

#else /* OSS_CONFIG_PCI */
	retCode = ERR_OSS_UNK_BUSTYPE;
#endif /* OSS_CONFIG_PCI */

GETCFG_END:
    return( retCode );
}/*OSS_PciGetConfig*/

/**********************************************************************/
/** Write to a register in PCI configuration space.
 *
 * \copydoc oss_specification.c::OSS_PciSetConfig()
 *
 * \sa OSS_PciGetConfig
 */
int32 OSS_PciSetConfig(
    OSS_HANDLE *oss,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       value
)
{
    int32   retCode = 0;
#ifdef OSS_CONFIG_PCI
	int16 idx, access;
	struct pci_dev *pciDev;

    DBGWRT_1((DBH,"OSS_PciSetConfig bus %lx dev %lx func %lx which %lx\n",
			  busNbr, pciDevNbr, pciFunction, which));

	oss->pciFilter.bus  = busNbr;
	oss->pciFilter.slot = pciDevNbr;
	oss->pciFilter.func = pciFunction;

	for(pciDev=oss->pciAcc->devices; pciDev; pciDev=pciDev->next)
		if (pci_filter_match( &oss->pciFilter, pciDev))
			break;

	if( !pciDev ){
		/* non-existant device */
		DBGWRT_ERR(( DBH, "*** OSS_PciSetConfig: non-existant device\n"));
		retCode = 1;
		goto SETCFG_END;
	}

	/*--- determine config reg offset and access ---*/

	if( (retCode = PciGetReg( oss, which, &idx, &access )))
		goto SETCFG_END;

    switch( access )
    {
        case 4:
			pci_write_long( pciDev, idx, (int)value);
			break;

        case 2:
			pci_write_word( pciDev, idx, (short)value );
			break;

	    case 1:
			pci_write_byte( pciDev, idx, (char)value);
			break;
    }/*switch*/

 SETCFG_END:
    if( retCode )
    {
        DBGWRT_ERR(( DBH, "*** OSS_PciSetConfig: error 0x%x writing cfg "
					 "space: reg 0x%04x\n", retCode, idx ));
        retCode = ERR_OSS_PCI;
    }/*if*/

#else  /* OSS_CONFIG_PCI */
	retCode = ERR_OSS_UNK_BUSTYPE;
#endif  /* OSS_CONFIG_PCI */

    return( retCode );
}/*OSS_PciSetConfig*/

/**********************************************************************/
/** Convert mechanical slot number to PCI device number
 *
 * \copydoc oss_specification.c::OSS_PciSlotToPciDevice()
 *
 * \linux The conversion is done by using the global array
 * #oss_pci_slot_devnbrs that can be modified by the user at load time
 * of the OSS module.
 *
 * \linux The PCI bus number is currently ignored
 */
int32 OSS_PciSlotToPciDevice(
    OSS_HANDLE *oss,
    int32      busNbr,
    int32      mechSlot,
    int32      *pciDevNbrP
)
{

    DBGWRT_1((DBH,"OSS_PciSlotToPciDevice mechSlot=%d,  entered NOP\n", mechSlot));

    return( 0 );
}/*OSS_PciSlotToPciDevice*/

#ifdef OSS_CONFIG_PCI
/********************************* PciGetReg ********************************
 *
 *  Description: Convert <which> parameter of OSS_PciGet/SetConfig
 *
 *			   	 Convert to register index and access size
 *---------------------------------------------------------------------------
 *  Input......: oss			OSS handle
 *				 which			parameter as passed to OSS_PciGet/SetConfig
 *  Output.....: return         success (0) or error code
 *				 *idxP			register index
 *				 *accessP		access size (1,2,4)
 *  Globals....: -
 ****************************************************************************/
static int32 PciGetReg(
	OSS_HANDLE *oss,
	u_int32 which,
	int16 *idxP,
	int16 *accessP )
{
	const struct {
		int16 idx;			/* PCI configuration space byte index */
		int16 access;		/* access mode byte/word/long */
	} regTbl[] = {
		{ 0, 0 },				/* - */
		{ 0x00, 2 },			/* OSS_PCI_VENDOR_ID */
		{ 0x02, 2 },			/* OSS_PCI_DEVICE_ID */
		{ 0x04, 2 },			/* OSS_PCI_COMMAND */
		{ 0x06, 2 },			/* OSS_PCI_STATUS */
		{ 0x08, 1 },			/* OSS_PCI_REVISION_ID */
		{ 0x09, 1 },			/* OSS_PCI_CLASS */
		{ 0x0a, 1 },			/* OSS_PCI_SUB_CLASS */
		{ 0x0b, 1 },			/* OSS_PCI_PROG_IF */
		{ 0x0c, 1 },			/* OSS_PCI_CACHE_LINE_SIZE */
		{ 0x0d, 1 },			/* OSS_PCI_PCI_LATENCY_TIMER */
		{ 0x0e, 1 },			/* OSS_PCI_HEADER_TYPE */
		{ 0x0f, 1 },			/* OSS_PCI_BIST */
		{ 0x10, 4 },			/* OSS_PCI_ADDR_0 */
		{ 0x14, 4 },			/* OSS_PCI_ADDR_1 */
		{ 0x18, 4 },			/* OSS_PCI_ADDR_2 */
		{ 0x1c, 4 },			/* OSS_PCI_ADDR_3 */
		{ 0x20, 4 },			/* OSS_PCI_ADDR_4 */
		{ 0x24, 4 },			/* OSS_PCI_ADDR_5 */
		{ 0x28, 4 },			/* OSS_PCI_CIS */
		{ 0x2c, 2 },			/* OSS_PCI_SUBSYS_VENDOR_ID */
		{ 0x2e, 2 },			/* OSS_PCI_SUBSYS_ID */
		{ 0x30, 4 },			/* OSS_PCI_EXPROM_ADDR */
		{ 0x3d, 1 },			/* OSS_PCI_INTERRUPT_PIN */
		{ 0x3c, 1 }				/* OSS_PCI_INTERRUPT_LINE */
	};

	switch( which & OSS_PCI_ACCESS ){
	case 0x00000000:

		/*--- standard PCI regs ---*/
		if( which < OSS_PCI_VENDOR_ID || which > OSS_PCI_INTERRUPT_LINE ){
			DBGWRT_ERR((DBH," *** OSS:PciGetReg: bad which parameter 0x%x\n",
						which));
			return ERR_OSS_PCI_UNK_REG;
		}
		*idxP 		= regTbl[which].idx;
		*accessP 	= regTbl[which].access;
		break;

	case OSS_PCI_ACCESS_8:
		/*--- byte access to any PCI config reg ---*/
		*idxP 		= which & 0xff;
		*accessP	= 1;
		break;

	case OSS_PCI_ACCESS_16:
		/*--- word access to any PCI config reg ---*/
		*idxP 		= which & 0xff;
		*accessP	= 2;
		break;

	case OSS_PCI_ACCESS_32:
		/*--- long access to any PCI config reg ---*/
		*idxP 		= which & 0xff;
		*accessP	= 4;
		break;

	default:
		DBGWRT_ERR((DBH," *** OSS:PciGetReg: bad which parameter 0x%x\n",
					which));
		return ERR_OSS_PCI_UNK_REG;
	}
	return 0;
}

#endif /* OSS_CONFIG_PCI */


/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_bustoaddr.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Bus Address translation routines and PCI access routines
 *
 *    \switches  -
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

#ifdef CONFIG_MEN_VME_KERNELIF
# include <MEN/men_vme_kernelif.h>
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifdef CONFIG_PCI
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

    *physicalAddrP = NULL;

    switch( busType )
    {
	case OSS_BUSTYPE_NONE:
		*physicalAddrP = va_arg( argptr, void* );
		break;

#ifdef CONFIG_PCI
	case OSS_BUSTYPE_PCI:
	{
		int32 	 merged_bus	 = va_arg( argptr, u_int32 );
		int32    pciDevNbr   = va_arg( argptr, u_int32 );
		int32    pciFunction = va_arg( argptr, u_int32 );
		int32    addrNbr     = va_arg( argptr, u_int32 );
		u_int32  busNbr      = OSS_BUS_NBR( merged_bus );
		u_int32  pciDomain   = OSS_DOMAIN_NBR( merged_bus );
		unsigned int devfn = PCI_DEVFN( pciDevNbr, pciFunction );
		struct pci_dev *dev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
		struct pci_bus *bus;

		if( (bus = pci_find_bus( pciDomain, busNbr ) ) == NULL ) {
			error = ERR_OSS_PCI_BUS_NOTFOUND;
			break;
		}
		if( (dev = pci_get_slot( bus, devfn )) == NULL ){
#else
		if (pciDomain != 0) {
			DBGWRT_ERR((DBH,"*** OSS_BusToPhysAddr: pci domains not supported in "
						"kernel versions < 2.6.0!\n"));
			error = ERR_OSS_PCI_BUS_NOTFOUND;
			goto BUSTOPHYS_END;
		}

		if( (dev = pci_find_slot( busNbr, devfn )) == NULL ){
#endif
			error = ERR_OSS_PCI_NO_DEVINSLOT;
			break;
		}

		*physicalAddrP = (void *)(U_INT32_OR_64)pci_resource_start( dev, addrNbr );
		break;
	}
#endif /*CONFIG_PCI*/

#ifdef CONFIG_MEN_VME_KERNELIF
	case OSS_BUSTYPE_VME:
	{
		void *vmeadrs 		= va_arg( argptr, void *);
		u_int32	vmespace 	= va_arg( argptr, u_int32 );
		/*u_int32 size		= va_arg( argptr, u_int32 );*/
		int space=0;

		switch(vmespace & 0xff){
		case OSS_VME_A16:	space |= VME_A16_SPACE; break;
		case OSS_VME_A24:	space |= VME_A24_SPACE; break;
		case OSS_VME_A32:	space |= VME_A32_SPACE; break;
		default:
			error = ERR_OSS_VME_ILL_SPACE;
			break;
		}

		switch(vmespace & 0xff00){
		case OSS_VME_D16:	space |= VME_D16_ACCESS; break;
		case OSS_VME_D32:	space |= VME_D32_ACCESS; break;
		default:
			error = ERR_OSS_VME_ILL_SPACE;
			break;
		}

		if( error ) break;

		error = vme_bus_to_phys( space, (uintptr_t)vmeadrs, physicalAddrP );

		if( error ){
			DBGWRT_ERR((DBH,"*** OSS_BusToPhysAddr: error %d in "
						"vme_bus_to_phys\n", error));
			error = ERR_OSS_VME_ILL_SPACE;
		}
		break;
	}
#endif /* CONFIG_MEN_VME_KERNELIF */

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
    int32       mergedBusNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       *valueP
)
{
    int32   retCode = 0;
#ifdef CONFIG_PCI
	unsigned int	value32;
	unsigned short	value16;
	unsigned char	value8;
	unsigned int devfn = PCI_DEVFN( pciDevNbr, pciFunction );
	struct pci_dev *dev;
	int16 idx, access;
	u_int32	busNbr = OSS_BUS_NBR( mergedBusNbr );
	u_int32 pciDomain = OSS_DOMAIN_NBR( mergedBusNbr );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	struct pci_bus *bus = NULL;
#endif

    DBGWRT_1((DBH,"OSS_PciGetConfig domain %lx bus %lx dev %lx func %lx which %lx\n",
			  pciDomain, busNbr, pciDevNbr, pciFunction, which));

    *valueP = 0;

	/*--- determine config reg offset and access ---*/
	if( (retCode = PciGetReg( oss, which, &idx, &access )))
		goto GETCFG_END;
		
	/* pci_find_slot is deprecated and removed from kernel version 2.6.31 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)

    /* look for bus in the given pci domain */                     	
	if( (bus = pci_find_bus( pciDomain, busNbr ) ) == NULL ) {
			return ERR_OSS_PCI_BUS_NOTFOUND;
	}

	if( (dev = pci_get_slot( bus, devfn )) == NULL ) {

#else
	if (pciDomain != 0) {
		DBGWRT_ERR((DBH,"*** OSS_PciGetConfig: pci domains not supported in "
					"kernel versions < 2.6.0!\n"));
		return ERR_OSS_PCI_BUS_NOTFOUND;
	}
	if( (dev = pci_find_slot( busNbr, devfn )) == NULL ){
#endif
		/* non-existant device */
		*valueP = 0xffffffff & ((1L<<(access*8))-1);
		return 0;
	}

	/*
	 * Kernels since 2.6.16 require explicit call to pci_enable_device(),
	 * or kernel parameter "pci=routeirq" has to be passed. Otherwise the
	 * PCI Interrupt wont work!
	 */

	if ((retCode = pci_enable_device(dev))) {
		DBGWRT_ERR(( DBH, "*** Linux error 0x%x during pci_enable_device!\n",
					 retCode));
		return retCode;
	}

	/*
	 * handle special regs. These may be different in the devices
	 * header and linux internal representation (think of ACPI)
	 */

	if (OSS_PCI_INTERRUPT_LINE == which) {
		*valueP = dev->irq;
	} else {
		switch( access )
		{
	    case 4:
			retCode = pci_read_config_dword( dev, idx, &value32 );
			*valueP = (u_int32)value32;
			break;
	
	    case 2:
			retCode = pci_read_config_word( dev, idx, &value16 );
			*valueP = (u_int32)value16;
			break;
	
	    case 1:
			retCode = pci_read_config_byte( dev, idx, &value8 );
			*valueP = (u_int32)value8;
			break;
		}/*switch*/
	}

	if( retCode )
	{
		DBGWRT_ERR(( DBH, "*** OSS_PciGetConfig: error 0x%x reading cfg "
					 "space: reg 0x%04x\n", retCode, idx ));
		retCode = ERR_OSS_PCI;
		*valueP = 0;
	}/*if*/

	DBGWRT_2((DBH, "  value=0x%08x\n", *valueP));

#else
	retCode = ERR_OSS_UNK_BUSTYPE;
#endif

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
    int32       mergedBusNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       value
)
{
    int32   retCode = 0;
#ifdef CONFIG_PCI
	int16 idx, access;
	unsigned int devfn = PCI_DEVFN( pciDevNbr, pciFunction );
	struct pci_dev *dev;
	u_int32	busNbr = OSS_BUS_NBR( mergedBusNbr );
	u_int32 pciDomain = OSS_DOMAIN_NBR( mergedBusNbr );
	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	struct pci_bus *bus;
#endif

    DBGWRT_1((DBH,"OSS_PciSetConfig domain %lx bus %lx dev %lx func %lx which %lx\n",
			  pciDomain, busNbr, pciDevNbr, pciFunction, which));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)

	// look for bus in the given pci domain
	if( (bus = pci_find_bus( pciDomain, busNbr ) ) == NULL ) {
			return ERR_OSS_PCI_BUS_NOTFOUND;
	}
	if( (dev = pci_get_slot( bus, devfn )) == NULL ){
#else
	if (pciDomain != 0) {
		DBGWRT_ERR((DBH,"*** OSS_PciSetConfig: pci domains not supported in "
					"kernel versions < 2.6.0!\n"));
		return ERR_OSS_PCI_BUS_NOTFOUND;
	}

	if( (dev = pci_find_slot( busNbr, devfn )) == NULL ){
#endif
		DBGWRT_ERR(( DBH, "*** OSS_PciSetConfig: non-existant device\n"));
		/* non-existant device */
		retCode = 1;
		goto SETCFG_END;
	}

	/*--- determine config reg offset and access ---*/

	if( (retCode = PciGetReg( oss, which, &idx, &access )))
		goto SETCFG_END;

    switch( access )
    {
        case 4:
			pci_write_config_dword( dev, idx, (int)value);
			break;

        case 2:
			pci_write_config_word( dev, idx, (short)value );
			break;

	    case 1:
			pci_write_config_byte( dev, idx, (char)value);
			break;	
    }/*switch*/

 SETCFG_END:
    if( retCode )
    {
        DBGWRT_ERR(( DBH, "*** OSS_PciSetConfig: error 0x%x writing cfg "
					 "space: reg 0x%04x\n", retCode, idx ));
        retCode = ERR_OSS_PCI;
    }/*if*/

#else
	retCode = ERR_OSS_UNK_BUSTYPE;
#endif

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
    int32 retCode=0;

    DBGWRT_1((DBH,"OSS_PciSlotToPciDevice mechSlot=%d\n", mechSlot));

    *pciDevNbrP = -1;

    if( mechSlot < 1 || mechSlot > 17) {
		retCode = ERR_OSS_ILL_PARAM;
        DBGWRT_ERR((DBH, "*** OSS_PciSlotToPciDevice: Bad mechSlot %d\n",
					mechSlot ));
        goto END;
	}/*if*/

	if( oss_pci_slot_devnbrs[mechSlot-1] == -1 ){
        DBGWRT_ERR((DBH, "*** OSS_PciSlotToPciDevice: MechSlot %d not "
					"available\n", mechSlot ));
		retCode = ERR_OSS_ILL_PARAM;
	}
	else {
		*pciDevNbrP = oss_pci_slot_devnbrs[mechSlot-1];
		DBGWRT_2((DBH," pciDevNbr=0x%lx\n", *pciDevNbrP ));
	}
END:
    return( retCode );
}/*OSS_PciSlotToPciDevice*/

#ifdef CONFIG_PCI
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

#endif /* CONFIG_PCI */





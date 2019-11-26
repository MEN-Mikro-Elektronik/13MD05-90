/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: open.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *
 *  Description: Open routine for MDIS kernel module
 *
 *     Required: -
 *     Switches: DBG
 *
 *---------------------------------------------------------------------------
 * Copyright 2000-2019, MEN Mikro Elektronik GmbH
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

#include <linux/pci.h>
#include "mk_intern.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--------------------------------------+
|   TYPEDEFS                            |
+--------------------------------------*/
/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

int32 G_irqHdl = 0; /* OSS_IRQ_HANDLE */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
int32 MapAddressSpaces(MK_DEV *dev, DESC_HANDLE *devDescHdl);
int32 GetDeviceParams(MK_DEV *dev);
int32 CheckPciDev( MK_DEV *dev );
int32 PciGetAddrReg(
	MK_DEV *dev,
	u_int32 pciBaseReg,
	void **addrP,
	u_int32 *sizeP,
	u_int32 *spaceP);
static void strtolower( char *s );
static int32 BrdIrqEnable( MK_DEV *dev, int enable );

/******************************** MDIS_InitialOpen ***************************
 *
 *  Description: Open first path to device
 *               - set up parameters
 *               - set up hardware
 *			
 *---------------------------------------------------------------------------
 *  Input......: devName	device name
 *				 devDescHdl	device descriptor handle
 *				 devDesc	device descriptor specifier
 *				 brdName	board device name
 *				 brdDesc	board descriptor specifier
 *				 persist    if non-zero, don't delete device on close
 *  Output.....: return  	success (0) or error code
 *				 *devP		device handle
 *  Globals....: -
 ****************************************************************************/
int32 MDIS_InitialOpen(
	char *devName,
	DESC_HANDLE *devDescHdl,
	DESC_SPEC *devDesc,
	char *brdName,
	DESC_SPEC *brdDesc,
	int persist,
	MK_DEV **devP)
{
	DBGCMD(const char fname[] = "MK:InitialOpen: "; )
    int32 error;     /* error holder */
	u_int32 size=0;
	u_int32 dbg_mylvl=0;
	int32 value;
	MK_DEV *dev;
	char hwName[MK_MAX_DRVNAME+1];
	char drvName[MK_MAX_DRVNAME+1];
	MK_DRV *drv;
	U_INT32_OR_64 hlpNrChan;

    DBGWRT_1((DBH,"MK - InitialOpen: dev=%s brd=%s\n", devName, brdName));

	*devP = NULL;

	/* allocate the device structure */
	dev = kmalloc( sizeof(*dev), GFP_KERNEL );
	if( dev == NULL )
		return ENOMEM;

	memset( (char *)dev, 0, sizeof(*dev));
	strcpy( dev->devName, devName );
	strcpy( dev->brdName, brdName );
	dev->persist = persist;
	
	/* init OSS for device */
	if(( error = OSS_Init( dev->devName, &dev->osh )))
		goto errexit;

	/*------------------------------+
	|  get/set debug levels         |
	+------------------------------*/
	if ((error = DESC_GetUInt32(devDescHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_OSS")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto errexit;

	OSS_DbgLevelSet(dev->osh, value);

	if ((error = DESC_GetUInt32(devDescHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_DESC")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto errexit;

	DESC_DbgLevelSet(devDescHdl, value);


	if ((error = DESC_GetUInt32(devDescHdl, OSS_DBG_DEFAULT, &dbg_mylvl, "DEBUG_LEVEL_MK")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto errexit;

	DBG_MYLEVEL = (int)dbg_mylvl;
	
	/*------------------------------+
	|  get device slot              |
	+------------------------------*/
	if ((error = DESC_GetUInt32(devDescHdl, 0, &dev->devSlot, "DEVICE_SLOT")))
		goto errexit;

	/*------------------------------+
	|  get hardware name            |
	+------------------------------*/
	size = sizeof(hwName) - strlen(MK_DRV_PREFIX);

	if ((error = DESC_GetString(devDescHdl, "", hwName, &size, "HW_TYPE")))
		goto errexit;

	/*------------------------------+
	|  Find the LL driver           |
	+------------------------------*/
	/*
	 * Since some old descriptors for M-modules <M100 have leading
	 * zeroes in hw name (e.g. M008), but ll drivers have the name
	 * men_ll_m8, remove the leading zeroes in that case.
	 */
	{		
		int i,j;

		strcpy( drvName, MK_DRV_PREFIX );
		i=7; j=0;

		drvName[i++] = hwName[j++];
	
		if( hwName[0] == 'm' || hwName[0] == 'M'){
			while( hwName[j] == '0' ) /* skip leading zeroes */
				j++;
		}
		/*--- copy rest of string ---*/
		strcpy( &drvName[i], &hwName[j] );
	}

	strtolower( drvName );

	DBGWRT_2((DBH, " info: device %s driver %s slot %d\n",
			  devName, drvName, dev->devSlot ));

	/*
	 * first, let's check if the LL driver is already loaded.
	 * If not, try to load it automatically through kerneld
	 */

	if( (drv = MDIS_FindDrvByName( drvName )) == NULL ) {
		DBGWRT_2((DBH," try to load %s trough kerneld\n", drvName ));
		
		MK_UNLOCK;
		request_module( drvName );
		MK_LOCK(error);

		if( error ) {
			error = EINTR;
			goto errexit;
		}
	}
	/*
	 * Now LL driver-module should be present, or HW_TYPE in system.dsc may
	 * have been wrong
	 */
	if( (drv = MDIS_FindDrvByName( drvName )) == NULL ){
		DBGWRT_ERR((DBH,"*** %s: driver module %s not found\n",
					fname, drvName ));
		error = ERR_MK_NO_LLDRV;
		/* like errexit but no 'FinalClose' call - nothing to close yet */
		goto moderr;
	}

	dev->drv = drv;				/* save driver link */

	/* increment module's use count */
	if( !try_module_get( drv->module ) ){
		DBGWRT_ERR((DBH, "*** %s: try_module failed\n", fname ));
		error = ERR_MK_NO_LLDRV;
		goto errexit;
	}

	/*--- get driver entry points ---*/
	drv->getEntry( &dev->llJumpTbl );

	/*------------------------------+
	|  prepare board handler        |
	+------------------------------*/
	/* open BBIS driver */
	if( (error = bbis_open( brdName,
							brdDesc,
							&dev->brd,
							&dev->brdJumpTbl ))){
		DBGWRT_ERR((DBH,"*** %s: %s can't open BBIS dev %s\n",
					fname, devName, brdName ));
		error = -error;
		goto errexit;
	}

	/*------------------------------+
	|  get fixed parameters         |
	+------------------------------*/
	if ((error = GetDeviceParams(dev)))				/* device params */
		goto errexit;

	if ((error = MDIS_GetBoardParams(dev)))	/* board params */
		goto errexit;

	if ((error = MDIS_GetPciBoardParams(dev, devDescHdl)))	/* PCI params */
		goto errexit;


	/*--- for PCI devices, check if device present and vend/devId match ---*/
	if( dev->devBusType == OSS_BUSTYPE_PCI ){
		
		if( (error = CheckPciDev(dev)))
			goto errexit;
	}

	/*------------------------------+
	|  map address spaces           |
	+------------------------------*/
	if ((error = MapAddressSpaces(dev, devDescHdl)))
		goto errexit;

	/*-----------------------------------+
	|  install interrupt                 |
	+-----------------------------------*/
	if (dev->irqUse || (dev->irqInfo & BBIS_IRQ_EXPIRQ)) {

		/* determine interrupt parameters */
		if( (error = MDIS_GetIrqParams( dev )))
			goto errexit;

		if( (error = MDIS_InstallSysirq(dev)))
			goto errexit;
	}

	if (dev->irqUse) {
		if ((error = DESC_GetUInt32(devDescHdl, 0, &dev->irqEnableKey,
									"IRQ_ENABLE")) &&
			error != ERR_DESC_KEY_NOTFOUND)
			goto errexit;
	}

	/*----------------------------------------+
	 | tell BBIS the irqHandle - if supported |
	 +----------------------------------------*/
	if( dev->brdJumpTbl.setIrqHandle != NULL )	{
		error = dev->brdJumpTbl.setIrqHandle( dev->brd,
											  (OSS_IRQ_HANDLE*)&G_irqHdl );
		if( error ) {
			DBGWRT_ERR(( DBH, "*** open.c: BB: setIrqHandle failed! \n" ));
			goto errexit;
		}/*if*/
	}

	/*------------------------------+
	|  init board slot              |
	+------------------------------*/
	if( (error = MDIS_SetMiface( dev )))
		goto errexit;

	/* create device semaphore */
	if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN, 1, &dev->semDev )))
		goto errexit;


	/*------------------------------+
	|  init device                  |
	+------------------------------*/
	if ((error = dev->llJumpTbl.init(devDesc,
					 dev->osh,
					 dev->ma,
					 dev->semDev,
					 NULL, /* irq handle */
					 &dev->ll ))) {
		dev->ll = NULL;
		DBGWRT_ERR((DBH," *** open: can't init device error=0x%lx\n", error));
	}
	else
		dev->initialized = TRUE;

	if( dev->exceptionOccurred ){
		printk( KERN_WARNING "*** MDIS: %d BBIS exceptions on %s / %s during "
				"device init. Check for correct module slot\n",
				dev->exceptionOccurred, dev->brdName, dev->devName );
		error = ERR_BUSERR;
		goto errexit;
	}
	if( error )
		goto errexit;
		
	/*------------------------------+
	|  get nr of device channels    |
	+------------------------------*/
	if ((error = dev->llJumpTbl.getStat(dev->ll,
										M_LL_CH_NUMBER,
										0,
										&hlpNrChan))) {
		DBGWRT_ERR((DBH," *** open: can't get M_LL_CH_NUMBER\n"));
		goto errexit;
	}
	dev->devNrChan = (u_int32)hlpNrChan;
	
	DBGWRT_2((DBH," device channels: %d channels\n",dev->devNrChan));

	/*------------------------------+
	|  prepare process locking      |
	+------------------------------*/
	if ((error = MDIS_InitLockMode(dev)))
		goto errexit;

	/*------------------------------+
	|  get irq enable               |
	|  enable interrupt             |
	+------------------------------*/
	if (dev->irqUse) {
		MK_UNLOCK;
		MDIS_EnableIrq(dev, dev->irqEnableKey);
		MK_LOCK(error);
	}

	OSS_DL_AddTail( &G_devList, &dev->node ); /* add to device list */

	*devP = dev;
    return ERR_SUCCESS;

 errexit:	
	MDIS_FinalClose( dev );
 moderr:
	DBGWRT_ERR((DBH,"*** %s %s failed error=0x%lx\n", fname, devName, error ));
	return error;
}

/******************************* MapAddressSpaces ****************************
 *
 *  Description: Map all address spaces required from device
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *				 devDescHdl descriptor handle
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MapAddressSpaces(MK_DEV *dev, DESC_HANDLE *devDescHdl)
{
    int32 error;     /* error holder */
	u_int32 n, subDevOff;

	/* for all addr spaces */
	for (n=0; n<dev->spaceCnt; n++)
	{	
		DBGWRT_2((DBH," space[%d]:\n",n));

		/* by default, use addrSpaceType from BBIS */
		dev->space[n].type = dev->addrSpaceType;

		/*------------------------------------------+
        |  get addr space [n] required from device  |
        |  (device addr space)                      |
        +------------------------------------------*/
		if ((error = dev->llJumpTbl.info(LL_INFO_ADDRSPACE,n,
										 &dev->space[n].addrMode,
										 &dev->space[n].dataMode,
										 &dev->space[n].reqSize ))) {
			DBGWRT_ERR((DBH," *** MapAddressSpaces: can't get "
						"LL_INFO_ADDRSPACE\n"));
			return(error);
		}

		DBGWRT_2((DBH,"  required : addrMode=0x%04x dataMode=0x%04x size=%d\n",
				  dev->space[n].addrMode, dev->space[n].dataMode,
				  dev->space[n].reqSize));

		if( dev->space[n].addrMode == MDIS_MA_PCICFG ){
			/*------------------------------------------------+
			|  LL driver requests access to PCI config space  |
			+------------------------------------------------*/
			if( dev->devBusType != OSS_BUSTYPE_PCI ){
			    DBGWRT_ERR((DBH," *** MapAddressSpaces: MDIS_MA_PCICFG allowed"
							"on PCI devices only!\n"));

				return ERR_BBIS_ILL_ADDRMODE;
			}

			dev->space[n].virtAddr = (void *) MAC_PCI2MAHDL2( dev->pciDomainNbr,
															  dev->pciBusNbr,
															  dev->pciDevNbr,
															  dev->pciFuncNbr);	 				
				
			goto mapped;		/* we're done */
		}
			
		if( dev->devBusType != OSS_BUSTYPE_PCI ){
			/*------------------------------------------------+
			|  NON PCI device, ask BBIS for address and size  |
			+------------------------------------------------*/

			/*------------------------------------------+
			|  get addr space [n] available on board    |
			|  (bus addr space)                         |
			+------------------------------------------*/
			if ((error = dev->brdJumpTbl.getMAddr(dev->brd,
												  dev->devSlot,
												  dev->space[n].addrMode,
												  dev->space[n].dataMode,
												  &dev->space[n].physAddr,
												  &dev->space[n].availSize))) {
				DBGWRT_ERR((DBH," *** MapAddressSpaces: can't getMAddr "
							"space[%d]\n",n));
				return(error);
			}
			
			if( dev->space[n].addrMode & MDIS_MA_FLAG_NOMAP ){
				/*------------------------------------------------+
				|  LL driver requests not to map                  |
				+------------------------------------------------*/
				dev->space[n].virtAddr = dev->space[n].physAddr;
			
				DBGWRT_2((DBH,"  MDIS_MA_FLAG_NOMAP set in addrMode (available: addr=0x%08x size=%d)\n",
					  dev->space[n].physAddr, dev->space[n].availSize));
				
				goto mapped;		/* we're done */
			}		
			
		}
		else {
			/*----------------------------------------------+
			|  PCI device, query the PCI base address regs  |
			+----------------------------------------------*/
			u_int32 pciBaseReg;
			
			/*--- check if PCI_BASEREG_ASSIGN specified in desc ---*/
			if ((error = DESC_GetUInt32(devDescHdl, n, &pciBaseReg,
										"PCI_BASEREG_ASSIGN_%d", n)) &&
				error != ERR_DESC_KEY_NOTFOUND){

				DBGWRT_ERR((DBH," *** MapAddressSpaces: error "
							"getting PCI_BASEREG_ASSIGN_%d\n", n));
				return(error);
			}
			
			DBGWRT_2((DBH,"  Using PCI base reg %d for addrspace %d\n",
					  pciBaseReg, n ));

			
			/*--- get size and address of PCI base reg ---*/
			if( (error = PciGetAddrReg( dev, pciBaseReg,
										&dev->space[n].physAddr,
										&dev->space[n].availSize,
										&dev->space[n].type)))
				return error;

		}
		DBGWRT_2((DBH,"  available: addr=0x%08x size=%d space=%d\n",
				  dev->space[n].physAddr, dev->space[n].availSize,
				  dev->space[n].type));

		/*------------------------------------------+
		|  Check if descriptor SUBDEVICE_OFFSET_x   |
		|  is present, add offset if so             |
		+------------------------------------------*/
		subDevOff = 0;
		if ((error = DESC_GetUInt32(devDescHdl, 0, &subDevOff,
									"SUBDEVICE_OFFSET_%d", n)) &&
			error != ERR_DESC_KEY_NOTFOUND){

			DBGWRT_ERR((DBH," *** MapAddressSpaces: error "
						"getting SUBDEVICE_OFFSET\n"));
			return(error);
		}
			
		if( subDevOff ){
			DBGWRT_2((DBH,"  subdevoff=0x%x\n", subDevOff ));
			dev->space[n].physAddr =
				(void *)((char *)dev->space[n].physAddr + subDevOff);
		}
		if( n==0 )
			dev->subDevOffset = subDevOff;

		/*------------------------------------------+
        |  check size                               |
        +------------------------------------------*/
		if (dev->space[n].availSize < dev->space[n].reqSize + subDevOff) {
			DBGWRT_ERR((DBH," *** MapAddressSpaces: size conflict "
						"space[%d]:",n));
			DBGWRT_ERR((DBH," required=%d <==> available=%d\n",
						dev->space[n].reqSize, dev->space[n].availSize));
			return(ERR_MK_ILL_MSIZE);
		}

		/*------------------------------------------+
        |  claim memory or I/O region				|
        +------------------------------------------*/
		if( dev->space[n].type == OSS_ADDRSPACE_MEM )
		{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
			if( check_mem_region( (unsigned long) dev->space[n].physAddr, dev->space[n].reqSize ))
			{
				DBGWRT_ERR((DBH," *** MapAddressSpaces: can't request mem space[%d] addr 0x%p size 0x%x\n",n,dev->space[n].physAddr,
							dev->space[n].reqSize));
				error = ERR_OSS_BUSY_RESOURCE;
				return error;
			}
#endif
			request_mem_region( (unsigned long) dev->space[n].physAddr,	dev->space[n].reqSize, dev->devName);
		}
		else {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
			if( check_region( (unsigned long) dev->space[n].physAddr,
							  dev->space[n].reqSize )){
				DBGWRT_ERR((DBH," *** MapAddressSpaces: can't request I/O "
							"space[%d] addr 0x%p size 0x%x\n",n,dev->space[n].physAddr,
							dev->space[n].reqSize));
				error = ERR_OSS_BUSY_RESOURCE;
				return error;
			}
#endif
			request_region( (unsigned long) dev->space[n].physAddr, dev->space[n].reqSize, dev->devName );
		}
		dev->space[n].flags |= MK_REQUESTED;

		/*------------------------------------------+
        |  map addr space                           |
        +------------------------------------------*/
		if ((error = OSS_MapPhysToVirtAddr(dev->osh,
										   dev->space[n].physAddr,
										   dev->space[n].reqSize,
										   dev->space[n].type,
										   dev->devBusType,
										   0,
										   &dev->space[n].virtAddr
										   ))) {
			DBGWRT_ERR((DBH," *** MapAddressSpaces: can't map addr "
						"space[%d]\n",n));
			return(error);
		}
		dev->space[n].flags |= MK_MAPPED;
mapped:
		dev->ma[n] = (MACCESS)dev->space[n].virtAddr;
		DBGWRT_2((DBH,"  mapped to: addr=0x%08x\n",dev->space[n].virtAddr));
	}

	return(ERR_SUCCESS);
}

/******************************* GetDeviceParams ***************************
 *
 *  Description: Get fixed device parameters via Info() function
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 GetDeviceParams(MK_DEV *dev)
{
    int32 error;     /* error holder */

	/* nr of addr spaces */
	if ((error = dev->llJumpTbl.info(LL_INFO_ADDRSPACE_COUNT,
									 &dev->spaceCnt))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: can't get "
					"LL_INFO_ADDRSPACE_COUNT\n"));
		return(error);
	}

	DBGWRT_2((DBH," device addr spaces: %d addr spaces\n",dev->spaceCnt));

	if (dev->spaceCnt > MDIS_MAX_MSPACE) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: too many address spaces "
					"required\n"));
		return(ERR_MK);
	}

	/* device interrupt */
	if ((error = dev->llJumpTbl.info(LL_INFO_IRQ, &dev->irqUse))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: can't get LL_INFO_IRQ\n"));
		return(error);
	}

	DBGWRT_2((DBH," device interrupt: %s\n",dev->irqUse ?
			  "required":"not required"));

    /* device characteristic */
	if ((error = dev->llJumpTbl.info(LL_INFO_HW_CHARACTER,
									&dev->devAddrMode,
									&dev->devDataMode))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: can't get "
					"LL_INFO_HW_CHARACTER\n"));
		return(error);
	}

	DBGWRT_2((DBH," device characteristic: AddrMode=0x%04x, DataMode=0x%04x\n",
			  dev->devAddrMode, dev->devDataMode));

    /* process locking mode */
	if ((error = dev->llJumpTbl.info(LL_INFO_LOCKMODE, &dev->lockMode))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: warning: can't get "
					"LL_INFO_LOCKMODE\n"));
		dev->lockMode = LL_LOCK_CALL;
	}

	DBGWRT_2((DBH," process locking: lockMode=%d\n",dev->lockMode));

	return(ERR_SUCCESS);
}

/******************************* MDIS_GetBoardParams ************************
 *
 *  Description: Get fixed board parameters via BrdInfo() function
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_GetBoardParams(MK_DEV *dev)
{
    int32 error;     /* error holder */

	/* bus system type */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_BUSTYPE,
										 &dev->busType))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_BUSTYPE\n"));
		return(error);
	}
	
	/*
	 * If BBIS_BRDINFO_DEVBUSTYPE not supported by BBIS handler, use
	 * OSS_BUSTYPE_MMODULE
	 */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_DEVBUSTYPE,
										dev->devSlot, &dev->devBusType))) {
		DBGWRT_ERR((DBH," WARNING: GetBoardParams: can't get "
					"BBIS_BRDINFO_DEVBUSTYPE\n"));
		dev->devBusType = OSS_BUSTYPE_MMODULE;
	}
			
	/*determine BBIS address space type */

	/* try newer BBIS_CFGINFO_ADDRSPACE (e.g. required for chameleon BBIS) */
	error = dev->brdJumpTbl.cfgInfo(dev->brd,
								     BBIS_CFGINFO_ADDRSPACE,
									 dev->devSlot,
									 &dev->addrSpaceType );
	if ( error == ERR_SUCCESS ){								
		DBGWRT_2((DBH," using BBIS_CFGINFO_ADDRSPACE: slot=%d, "
					  "mapping=%s\n", dev->devSlot,
					  dev->addrSpaceType == OSS_ADDRSPACE_MEM ? "mem" : "io" ));
					  	
	} else if ( error == ERR_BBIS_UNK_CODE ) {
		// use older BBIS_BRDINFO_ADDRSPACE
		if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_ADDRSPACE,
										dev->devSlot, &dev->addrSpaceType))) {
			DBGWRT_ERR((DBH," WARNING: GetBoardParams: can't get "
						"BBIS_BRDINFO_ADDRSPACE\n"));
			/*If BBIS_BRDINFO_ADDRSPACE not supported by BBIS handler,
			  use OSS_ADDRSPACE_MEM*/			
			dev->addrSpaceType = OSS_ADDRSPACE_MEM;
		}
	} else {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_CFGINFO_ADDRSPACE\n"));
		dev->addrSpaceType = OSS_ADDRSPACE_MEM;
	}
			
	DBGWRT_2((DBH," board bustype=%d, dev bustype=%d addrSpaceType=%d\n",
			  dev->busType,dev->devBusType,dev->addrSpaceType));

	/* irq info */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_INTERRUPTS, dev->devSlot,
										&dev->irqInfo))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_INTERRUPTS\n"));
		return(error);
	}

	DBGWRT_2((DBH," board irq: info=%d\n",dev->irqInfo));

	/* function info */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_FUNCTION,
										BBIS_FUNC_IRQSRVINIT,
										&dev->irqSrvInitFunc))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_FUNCTION BBIS_FUNC_IRQSRVINIT\n"));
		return(error);
	}
	
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_FUNCTION,
										BBIS_FUNC_IRQSRVEXIT,
										&dev->irqSrvExitFunc))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_FUNCTION BBIS_FUNC_IRQSRVEXIT\n"));
		return(error);
	}

	/*-------------------------------------+
	|  Request parameters for PCI device   |
	+-------------------------------------*/
	if( dev->busType == OSS_BUSTYPE_PCI ||
		dev->devBusType == OSS_BUSTYPE_PCI) {

		/*--- get PCI bus number from board  ---*/
		if ((error = dev->brdJumpTbl.cfgInfo(dev->brd,
											BBIS_CFGINFO_BUSNBR,
											&dev->pciBusNbr,	
											dev->devSlot))){
			DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_CFGINFO_BUSNBR\n"));
			return(error);
		}
		
		/*--- get PCI domain number from board  ---*/
		if ((error = dev->brdJumpTbl.cfgInfo(dev->brd,
											BBIS_CFGINFO_PCI_DOMAIN,
											&dev->pciDomainNbr,	
											dev->devSlot))){
			DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_CFGINFO_PCI_DOMAIN\n"));
			return(error);
		}
	}
	return(ERR_SUCCESS);
}
		
/******************************* MDIS_GetPciBoardParams *********************
 *
 *  Description: Get PCI board parameters from descriptor and BBIS
 *			     Does nothing for non-PCI devices
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *				 devDescHdl descriptor handle
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_GetPciBoardParams(MK_DEV *dev, DESC_HANDLE *devDescHdl)
{
	int32 error;

	if( dev->devBusType == OSS_BUSTYPE_PCI ){
		
		/*--- get PCI device number from board ---*/
		if ((error = dev->brdJumpTbl.cfgInfo(dev->brd,
											BBIS_CFGINFO_PCI_DEVNBR,
											dev->devSlot,
											&dev->pciDevNbr))){
			DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_CFGINFO_BUSNBR\n"));
			return(error);
		}

		/*--- get PCI function number from descriptor ---*/
		if ((error = DESC_GetUInt32(devDescHdl, 0, &dev->pciFuncNbr,
									"PCI_FUNCTION")) &&
			error != ERR_DESC_KEY_NOTFOUND){

			DBGWRT_ERR((DBH," *** GetBoardParams: error "
						"getting PCI_FUNCTION\n"));
			return(error);
		}

		DBGWRT_2((DBH," PCI domain=%d bus=%d dev=%d\n",
					dev->pciDomainNbr, dev->pciBusNbr, dev->pciDevNbr));
		
		/*--- get PCI Vendor/Device ID from descriptor (required) ---*/
		if ( (error = DESC_GetUInt32(devDescHdl, 0, &dev->pciVendorId,
									 "PCI_VENDOR_ID")) ||
			 (error = DESC_GetUInt32(devDescHdl, 0, &dev->pciDeviceId,
									 "PCI_DEVICE_ID"))){

			DBGWRT_ERR((DBH," *** GetBoardParams: error "
						"getting PCI_VENDOR/DEVICE_ID\n"));
			return(error);
		}
		/*--- get PCI SubVendor/SubSys ID from descriptor (optional) ---*/
		if ( ((error = DESC_GetUInt32(devDescHdl, 0xffffffff,
									  &dev->pciSubSysVendorId,
									 "PCI_SUBSYS_VENDOR_ID")) &&
			  error != ERR_DESC_KEY_NOTFOUND ) ||

			 ((error = DESC_GetUInt32(devDescHdl, 0xffffffff,
									  &dev->pciSubSysId,
									 "PCI_SUBSYS_ID")) &&
			  error != ERR_DESC_KEY_NOTFOUND )){
			DBGWRT_ERR((DBH," *** GetBoardParams: error "
						"getting PCI_SUBSYS_VENDOR/SUBSYS_ID\n"));
			return(error);
		}

		DBGWRT_2((DBH," PCI desc params: func=%d vendID=0x%4x devID=0x%4x "
				  "subvendID=0x%4x subdevID=0x%4x\n",
				  dev->pciFuncNbr, dev->pciVendorId, dev->pciDeviceId,
				  dev->pciSubSysVendorId, dev->pciSubSysId ));
	}
			
	return(ERR_SUCCESS);
}

/******************************* MDIS_GetIrqParams ***************************
 *
 *  Description: Determine interrupt parameters of device
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_GetIrqParams(MK_DEV *dev)
{
    int32 error;     /* error holder */

	/* device irq available on board slot ? */
    if ((dev->irqInfo & BBIS_IRQ_DEVIRQ) == 0) {
		DBGWRT_ERR((DBH," *** GetIrqParams: board supports no "
					"device irq\n"));
		return(ERR_MK_NO_IRQ);
	}

	if( dev->devBusType != OSS_BUSTYPE_PCI ){
		/*----------------------------+
		|  Non PCI devices, ask BBIS  |
		+----------------------------*/

		/* get irq params for board slot */
		if ((error = dev->brdJumpTbl.cfgInfo(dev->brd,
											 BBIS_CFGINFO_IRQ,
											 dev->devSlot,
											 &dev->irqVector,
											 &dev->irqLevel,
											 &dev->irqMode ))) {
			DBGWRT_ERR((DBH," *** GetIrqParams: can't get "
						"BBIS_CFGINFO_IRQ\n"));
			return(error);
		}

#ifdef CONFIG_MENEM3FAM
		/*
		 * on EM03 Family we have every FPGA Unit at one EXT IRQ Pin.
		 * An open_pic/CPU specific offset must be added to irq number,
		 * see include/asm/irq.h. Its done here cause Common chameleon BBIS
		 * shouldnt need to deal with OS/CPU Specifics.
		 */
		dev->irqVector += MPC85xx_IRQ_EXT0;
		dev->irqLevel  += MPC85xx_IRQ_EXT0;
#endif

		DBGWRT_2((DBH,"Slot %d interrupt: vector=%d, level=%d, mode=0x%04x\n",
				  dev->devSlot, dev->irqVector, dev->irqLevel, dev->irqMode));

	}
	else
	{
	    int32 intLine;

		/*-----------------------------------------+
		|  PCI devices, get IRQ from config space  |
		+-----------------------------------------*/
		if( (error = OSS_PciGetConfig( dev->osh,
									   OSS_MERGE_BUS_DOMAIN( dev->pciBusNbr, dev->pciDomainNbr ),
									   dev->pciDevNbr,
									   dev->pciFuncNbr,
									   OSS_PCI_INTERRUPT_LINE,
									   &intLine ))){
			
			DBGWRT_ERR((DBH," *** GetIrqParams: Error accessing "
						"cfg space\n"));
			return error;
		}

		if( intLine == 0xff ){
			DBGWRT_ERR((DBH," *** GetIrqParams: PCI dev supports no "
						"device irq\n"));
			return(ERR_MK_NO_IRQ);
		}
		
		dev->irqMode = BBIS_IRQ_SHARED;
		dev->irqLevel = intLine;

		/*--- convert to Linux vector number ---*/
		if( (error = OSS_IrqLevelToVector( dev->osh, OSS_BUSTYPE_PCI,
										   intLine,
										   (int32*)&dev->irqVector ))){
			
			DBGWRT_ERR((DBH," *** GetIrqParams: Can't convert level to "
						"vector\n"));
			return error;
		}
		
		DBGWRT_2((DBH," slot %d interrupt: vector=%d, level=%d\n",
				  dev->devSlot, dev->irqVector, dev->irqLevel));
	}
	return ERR_SUCCESS;
}
		
/******************************* MDIS_InstallSysirq *************************
 *
 *  Description: Install the system interrupt
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_InstallSysirq(MK_DEV *dev)
{
	int32 error;
	/* install irq in system */

	DBGWRT_2((DBH," install system interrupt: vect=%d level %d\n", dev->irqVector, dev->irqLevel));

#ifdef CONFIG_MEN_VME_KERNELIF
	/* VME interrupt installation, only through special interface */	
	if( dev->busType == OSS_BUSTYPE_VME ){
			if((error = vme_request_irq( dev->irqVector, (void (*)( int, void *, struct pt_regs *))MDIS_IRQFUNC,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
					IRQF_SHARED,
#else
					SA_SHIRQ,
#endif
					dev->devName,
					dev )) < 0 ){
				DBGWRT_ERR((DBH," *** InstallSysirq: can't install VME "
							"interrupt (linux error=%d)\n", -error ));
				return(ERR_MK_IRQ_INSTALL);
			}
	}
	else
#endif /* CONFIG_MEN_VME_KERNELIF */
	{

		if((error = request_irq( dev->irqVector, MDIS_IRQFUNC,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
								 IRQF_SHARED,
#else
								 SA_SHIRQ,
#endif
								 dev->devName,
								 dev )) < 0 ){
			DBGWRT_ERR((DBH," *** InstallSysirq: can't install interrupt (linux error=%d)\n", -error ));
			return(ERR_MK_IRQ_INSTALL);
		}
	}
	dev->irqInstalled = 1;

	return(ERR_SUCCESS);
}

/******************************* MDIS_InitLockMode ***************************
 *
 *  Description: Init process lock mode
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_InitLockMode(MK_DEV *dev)
{
	u_int32 n;
	int32 error=0;

	switch(dev->lockMode) {
		case LL_LOCK_NONE:
			DBGWRT_2((DBH," prepare lockMode LL_LOCK_NONE\n"));
			/* nothing to do */
			break;

		case LL_LOCK_CALL:
			DBGWRT_2((DBH," prepare lockMode LL_LOCK_CALL\n"));

			/* create sem's */
			if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
										&dev->semRead )))
				break;
			if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
										&dev->semWrite )))
				break;
			if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
										&dev->semBlkRead)))
				break;
			if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
										&dev->semBlkWrite)))
				break;
			if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
										&dev->semSetStat )))
				break;
			if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
										&dev->semGetStat )))
				break;
			break;

		case LL_LOCK_CHAN:
			DBGWRT_2((DBH," prepare lockMode LL_LOCK_CHAN\n"));

			/* alloc space for array of sem ptr's */
			dev->semChanP = OSS_MemGet(
				dev->osh,
				sizeof(OSS_SEM_HANDLE *) * dev->devNrChan,
				&dev->semChanAlloc );

			if( dev->semChanP == NULL )
				return(ERR_OSS_MEM_ALLOC);
				
			memset( dev->semChanP, 0, sizeof(OSS_SEM_HANDLE *) *
					dev->devNrChan);

			/* init sems */
			for (n=0; n<dev->devNrChan; n++)
				if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
											&dev->semChanP[n] )))
					break;

			break;
		default:
			DBGWRT_ERR((DBH," *** open: unknown lockMode=%d\n",dev->lockMode));
			return(ERR_MK);
	}

	return(error);
}


/******************************* MDIS_EnableIrq *****************************
 *
 *  Description: Enable/disable interrupt on board+device
 *			     System interrupt is masked while disabling
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev		 device structure
 *				 enable  0=disable, 1=enable
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_EnableIrq(MK_DEV *dev, u_int32 enable)
{
    int32 error = ERR_SUCCESS;     /* error holder */
    u_int32 ret1=0, ret2=0;

	DBGWRT_1((DBH,"MDIS_EnableIrq %sableIrq %d\n",
			  enable ? "en":"dis", dev->irqLevel ));

	MK_LOCK( error );
	if( error )
		return -EINTR;
	
	/*---------------------+
    |  enable interrupt    |
    +---------------------*/
	if (enable) {

		dev->irqEnable = 1;

#ifdef CONFIG_MEN_VME_KERNELIF
		/* Enable VME interrupt level (level is not disabled on exit) */
		if( dev->busType == OSS_BUSTYPE_VME ){
			vme_ilevel_control( dev->irqLevel, 1 );
		}
#endif /* CONFIG_MEN_VME_KERNELIF */

		/* enable board slot's irq */
		ret1 = BrdIrqEnable( dev, 1 );

		/* enable on device */
		if( dev->llJumpTbl.setStat )
			ret2 = dev->llJumpTbl.setStat(dev->ll, M_MK_IRQ_ENABLE, 0, 1);

		if (ret1 && ret2) {		/* none of the calls was successful ? */
			dev->irqEnable = 0;
			error = ERR_MK_IRQ_ENABLE;
		}
	}
	/*---------------------+
    |  disable interrupt   |
    +---------------------*/
	else {

		dev->irqEnable = 0;

		/* disable on device */
		if( dev->ll && dev->llJumpTbl.setStat )
			ret2 = dev->llJumpTbl.setStat(dev->ll, M_MK_IRQ_ENABLE, 0, 0);

		/* disable board slot's irq */
		ret1 = BrdIrqEnable( dev, 0 );

		if (ret1 && ret2) {		/* none of the calls was successful ? */
			error = ERR_MK_IRQ_ENABLE;
		}
	}
	MK_UNLOCK;

#ifdef DBG
	if (ret1) {
		DBGWRT_ERR((DBH," *** EnableIrq: warning: can't %sable irq on board\n",
					enable ? "en":"dis"));
	}
	if (ret2) {
		DBGWRT_ERR((DBH," *** EnableIrq: warning: can't %sable "
					"irq on device\n",
					enable ? "en":"dis"));
	}
#endif

	return(error);
}

/********************************* CheckPciDev ******************************
 *
 *  Description: Check if PCI device present and vendor/devId matches
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev		 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....: -
 ****************************************************************************/
int32 CheckPciDev( MK_DEV *dev )
{
	u_int32 vendorId, deviceId, subSysVendorId, subSysId;
	int32 error;
	u_int32 mergedBusNbr = OSS_MERGE_BUS_DOMAIN( dev->pciBusNbr, dev->pciDomainNbr );

	DBGWRT_1((DBH, "MK - CheckPciDev\n"));

	if( (error = OSS_PciGetConfig( dev->osh, mergedBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_VENDOR_ID,
								   (int32 *)&vendorId )) ||
		(error = OSS_PciGetConfig( dev->osh, mergedBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_DEVICE_ID,
								   (int32 *)&deviceId )) ||
		(error = OSS_PciGetConfig( dev->osh, mergedBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_SUBSYS_VENDOR_ID,
								   (int32 *)&subSysVendorId )) ||
		(error = OSS_PciGetConfig( dev->osh, mergedBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_SUBSYS_ID,
								   (int32 *)&subSysId ))){
		DBGWRT_ERR((DBH," *** CheckPciDev: Error 0x%x reading cfg space\n",
					error ));
		return error;
	}

	DBGWRT_2((DBH, " PCI params from device: vendID=0x%04x devID=0x%04x "
			  "subSysVendID=0x%04x subSysID=0x%04x\n",
			  vendorId, deviceId, subSysVendorId, subSysId));

	/*--- check if device present ---*/
	if( vendorId == 0xffff && deviceId == 0xffff ){
		DBGWRT_ERR(( DBH, " *** CheckPciDev: PCI device not present!\n"));
		return ERR_OSS_PCI_NO_DEVINSLOT; /* due to lack of better error code */
	}

	/*--- check if device matches vendor/device ID ---*/
	if( (vendorId != dev->pciVendorId) || (deviceId != dev->pciDeviceId)){
		DBGWRT_ERR(( DBH, " *** CheckPciDev: wrong vend/devId 0x%x/0x%x!\n",
					 vendorId, deviceId));
		return ERR_OSS_PCI_ILL_DEV; /* due to lack of better error code */
	}
		
	/*--- check if device matches subsystem vendor/ID ---*/
	if( (dev->pciSubSysVendorId != 0xffffffff &&
		 (dev->pciSubSysVendorId != subSysVendorId)) ||
		(dev->pciSubSysId != 0xffffffff &&
		 (dev->pciSubSysId != subSysId))){

		DBGWRT_ERR(( DBH, " *** CheckPciDev: wrong subsys vend/Id "
					 "0x%x/0x%x!\n",
					 subSysVendorId, subSysId));
		return ERR_OSS_PCI_ILL_DEV; /* due to lack of better error code */
	}
		
	return ERR_SUCCESS;			
}

	
/********************************* PciGetAddrReg ***************************
 *
 *  Description: Determine if PCI address register present and determine size
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev			device structure
 *				 pciBaseReg	base address register index (0..5)
 *  Output.....: return  	success (0) or error code
 *				 *addrP		physical address
 *				 *sizeP		size of region
 *				 *spaceP	space of region (OSS_ADDRSPACE_MEM or IO)
 *  Globals....: -
 ****************************************************************************/
int32 PciGetAddrReg(
	MK_DEV *dev,
	u_int32 pciBaseReg,
	void **addrP,
	u_int32 *sizeP,
	u_int32 *spaceP)
{
	unsigned int devfn = PCI_DEVFN( dev->pciDevNbr, dev->pciFuncNbr );
	struct pci_dev *pdev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	struct pci_bus *pbus;
	
	if( (pbus = pci_find_bus( dev->pciDomainNbr, dev->pciBusNbr ) ) == NULL ) {
			return ERR_OSS_PCI_ILL_ADDRNBR;
	}
	if( (pdev = pci_get_slot( pbus, devfn )) == NULL ){
#else
	if( (pdev = pci_find_slot( dev->pciBusNbr, devfn )) == NULL ){
#endif
		/* non-existant device */
		DBGWRT_ERR((DBH," *** PciGetAddrReg: Device non existant!\n",
					pciBaseReg ));
		return ERR_OSS_PCI_ILL_ADDRNBR;
	}

	*sizeP = pci_resource_len( pdev, pciBaseReg );

	if( *sizeP == 0 ){
		DBGWRT_ERR((DBH," *** PciGetAddrReg: Address reg %d non existant!\n",
					pciBaseReg ));
		return ERR_OSS_PCI_ILL_ADDRNBR;
	}
	
	*addrP = (void *)(U_INT32_OR_64)pci_resource_start( pdev, pciBaseReg );
	
	if( pci_resource_flags( pdev, pciBaseReg ) & IORESOURCE_IO )
		*spaceP = OSS_ADDRSPACE_IO;
	else
		*spaceP = OSS_ADDRSPACE_MEM;
	

	DBGWRT_3((DBH, "   pci addr reg %d = 0x%08x, size=0x%x type=%s\n",
			  pciBaseReg, *addrP, *sizeP, *spaceP == OSS_ADDRSPACE_IO ?
			  "I/O" : "MEM" ));

	return 0;
}

static void strtolower( char *s )
{
	while( *s ){
		if( *s >= 'A' && *s <= 'Z' )
			*s += 'a' - 'A';
		s++;
	}
}

/********************************* SetMiface ********************************
 *
 *  Description: set up the device slot with required address/data mode.
 *			
 *	Checks wether the slot is already enabled by another device with a
 *  different subdevice offset / pciFunc. Initialize device slot if not
 *  already done		
 *---------------------------------------------------------------------------
 *  Input......: dev			device structure
 *  Output.....: return  	success (0) or error code
 *  Globals....: -
 ****************************************************************************/
int32 MDIS_SetMiface( MK_DEV *dev )
{
	MK_DEV *dev2;
	int32 error;

	/* check if another device has already initialized the interface */
	for( dev2=(MK_DEV *)G_devList.head;
		 dev2->node.next;
		 dev2 = (MK_DEV *)dev2->node.next ){

		if( strcmp(dev2->brdName, dev->brdName) == 0 &&
			dev2->devSlot == dev->devSlot &&
			dev->devSlot < BBIS_SLOTS_ONBOARDDEVICE_START){
			
			/*
			 * Check if a device with the same offset/pciFunc is already
			 * initialized. This would be an error
			 */
			if( dev2->subDevOffset == dev->subDevOffset &&
				dev2->pciFuncNbr == dev->pciFuncNbr ){
					DBGWRT_ERR((DBH,"*** MK: Device slot already in use: "
								" %s/%d/%d/%x\n",
								dev2->brdName, dev2->devSlot,
								dev2->pciFuncNbr,
								dev2->subDevOffset));
					return ERR_LL_DEV_BUSY;	/* better error code? */
			}

			/*
			 * the device slot is already initialized. Check if it was
			 * initialized with the same parameters
			 */
			if( (dev->devAddrMode != dev2->devAddrMode) ||
				(dev->devDataMode != dev2->devDataMode)) {
					DBGWRT_ERR((DBH,"*** MK: Device slot initialized with "
								" different parameters by %s/%d\n",
								dev2->brdName, dev2->devSlot));
					return ERR_BBIS_ILL_DATAMODE;
			}

			/* otherwise we're done */
			return 0;
		}
	}

	/* device slot hasn't been initialized yet. do it now. */
	if ((error = dev->brdJumpTbl.setMIface(dev->brd,
										   dev->devSlot,
										   dev->devAddrMode,
										   dev->devDataMode))) {
		DBGWRT_ERR((DBH,"*** MK: can't init board slot\n"));
	}
	return error;
}

/********************************* BrdIrqEnable ******************************
 *
 *  Description: if <enable>=1 enable board interrupt if irq has not been
 *				 enabled on that slot
 *			   	 if <enable>=0 disable board interrupt if irq has not been
 *				 enabled on that slot by another device
 *---------------------------------------------------------------------------
 *  Input......: dev			device structure
 *				 enable			0=disable 1=enable
 *  Output.....: return  	success (0) or error code
 *  Globals....: -
 ****************************************************************************/
static int32 BrdIrqEnable( MK_DEV *dev, int enable )
{
	MK_DEV *dev2;
	int32 error=0;

	/* check if another device has already enabled irq for that slot */
	for( dev2=(MK_DEV *)G_devList.head;
		 dev2->node.next;
		 dev2 = (MK_DEV *)dev2->node.next ){

		if( dev2 == dev ) continue;	/* ignore my own device */

		if( strcmp(dev2->brdName, dev->brdName) == 0 &&
			dev2->devSlot == dev->devSlot ){
			
			if( dev2->irqEnable ){				
				return 0;		/* irq already/still enabled */
			}
		}
	}

	/* device slot's irq hasn't been enabled. enable/disable it now. */
	if( dev->brdJumpTbl.irqEnable )
		error = dev->brdJumpTbl.irqEnable(dev->brd, dev->devSlot, enable);

	return error;
}


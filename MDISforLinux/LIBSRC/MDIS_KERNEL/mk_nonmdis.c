/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  mk_nonmdis.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/04/11 16:26:26 $
 *    $Revision: 2.2 $
 *
 *  	 \brief  MDIS kernel interface for non-MDIS drivers
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mk_nonmdis.c,v $
 * Revision 2.2  2003/04/11 16:26:26  kp
 * changed switch CONFIG_RTHAL -> MDIS_RTAI_SUPPORT
 *
 * Revision 2.1  2003/02/21 13:34:45  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#include <mk_intern.h>


/**********************************************************************/
/** Prepare MDIS/BBIS for use with specified external device
 *
 * If the device \a devName is not yet known, more or less the same
 * actions are performed as by MDIS_InitialOpen.
 *
 * If the device is already open, just its useCount is incremented
 *
 * Assumptions/Restrictions:
 * - not suitable for PCI devices (but for PCI carrier boards)
 * - only one address space can be requested
 * - no sub device offsets
 * - it is assumed that the calling device needs an interrupt
 *
 * mdis_open_external_dev() is typically called from a drivers
 * init_module() function. For example, a hyothetical M99 driver
 * could call it as shown below:
 *
 * \code
 *  #define MAC_MEM_MAPPED
 *  #define LINUX
 *
 *  #include <MEN/men_typs.h>
 *  #include <MEN/maccess.h>
 *  #include <MEN/mk_nonmdisif.h>
 *  #include <MEN/mdis_com.h>
 *  #include <MEN/ll_defs.h>
 *
 *	if( (ret = mdis_open_external_dev( 0, "m99_1", "c203_1", 0,
 *									   MDIS_MA08, MDIS_MD16,
 *									   0x100, (void *)&G_ma,
 *									   NULL, &G_dev )) < 0)
 *		return ret;
 * \endcode
 *
 *
 * \param  devName 	name of calling device. This name is used to register
 *					it within MDIS, e.g. "m99_1"
 * \param  brdName	device name of carrier board, e.g. "D201_1"
 * \param  slotNo	slot number on carrier board [0..n]
 * \param  addrMode	for M-Modules, specify the M-module address mode to be
 *					used. Specify only one address mode MDIS_MAxx!
 * \param  dataMode	for M-Modules, specify the M-module data mode to be
 *					used. Specify only one data mode MDIS_MDxx!
 * \param  addrSpaceSize size of address space required by calling device
 *					in bytes.
 * \param  mappedAddrP pointer to variable where virtual mapped address
 *					of devices hardware regs will be stored
 * \param  ossHandleP pointer to variable where the OSS handle for that
 *					device will be stored (can be passed as NULL)
 * \param  devP		pointer to variable where the MDIS internal device handle
 *					will be stored. This must be passed to all further
 *					mdis_xxx_external_xxx() functions.
 *
 * \return 0 on success, or negative linux error number
 *
 * \sa mdis_close_external_dev, mdis_install_external_irq
 */
int mdis_open_external_dev(
	char *devName,
	char *brdName,
	int slotNo,
	int addrMode,
	int dataMode,
	int addrSpaceSize,
	void **mappedAddrP,
	void **ossHandleP,
	void **devP )
{
	const u_int8 dummyBrdDesc[4] = { 0 };
	DBGCMD(const char fname[] = "mdis_open_external_dev: "; )
	MK_DEV *dev;
	int32 error;

	DBGWRT_1((DBH,"%sdev=%s brd=%s\n", fname, devName, brdName));

	MK_LOCK(error);
	if( error )
		return -EINTR;

	*devP = NULL;
	*mappedAddrP = NULL;
	if( ossHandleP ) *ossHandleP = NULL;

	if( (dev = MDIS_FindDevByName( devName )) != NULL )
		goto goodexit;

	/* allocate the device structure */
	dev = kmalloc( sizeof(*dev), GFP_KERNEL );
	if( dev == NULL ){
		error = -ENOMEM;
		goto errexit;
	}

	memset( (char *)dev, 0, sizeof(*dev));
	strcpy( dev->devName, devName );
	strcpy( dev->brdName, brdName );
	dev->externalDev 	= TRUE;
	dev->useCount 		= 0;
	dev->devSlot 		= slotNo;
	dev->devAddrMode 	= addrMode;
	dev->devDataMode	= dataMode;
	dev->spaceCnt		= 1;
	dev->irqUse			= TRUE;

	dev->space[0].addrMode = addrMode;
	dev->space[0].dataMode = dataMode;
	dev->space[0].reqSize  = addrSpaceSize;
	
	/* init OSS for device */
	if( (error = OSS_Init( dev->devName, &dev->osh ))){
		error = -ENODEV;
		goto errexit;
	}

	/*
	 * open BBIS driver
	 * by specifying a dummy board descriptor, we make sure that open
	 * fails if the BBIS device is not yet initialized
	 */
	if( (error = bbis_open( brdName, (void *)dummyBrdDesc,
							&dev->brd, &dev->brdJumpTbl ))){
		DBGWRT_ERR((DBH,"*** %s%s can't open BBIS dev %s\n",
					fname, devName, brdName ));
		error = -ENODEV;
		goto errexit;
	}

	/* get board parameters */
	if ((error = MDIS_GetBoardParams(dev))){	/* board params */
		error = -EINVAL;
		goto errexit;
	}

	/* map address space */
	if ((error = dev->brdJumpTbl.getMAddr(dev->brd,
										  dev->devSlot,
										  dev->space[0].addrMode,
										  dev->space[0].dataMode,
										  &dev->space[0].physAddr,
										  &dev->space[0].availSize))){ 	
		DBGWRT_ERR((DBH," *** %s: can't getMAddr space[0]\n",fname));
		error = -EINVAL;
		goto errexit;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
	/* claim memory region */
	if( check_mem_region( (unsigned long) dev->space[0].physAddr, dev->space[0].reqSize ))
	{
		DBGWRT_ERR((DBH," *** %scan't request mem space[0] addr 0x%p\n",fname,dev->space[0].physAddr));
		error = -EBUSY;
		goto errexit;
	}
#endif
	request_mem_region( (unsigned long) dev->space[0].physAddr, dev->space[0].reqSize,dev->devName );
	dev->space[0].flags |= MK_REQUESTED;

	/* map address space */
	if ((error = OSS_MapPhysToVirtAddr(dev->osh,
									   dev->space[0].physAddr,
									   dev->space[0].reqSize,
									   dev->space[0].type,
									   dev->devBusType,
									   0,
									   &dev->space[0].virtAddr))) {
		DBGWRT_ERR((DBH," *** %scan't map addr space[0]\n",fname));
		error = -EBUSY;
		return(error);
	}
	dev->space[0].flags |= MK_MAPPED;
	dev->ma[0] = (MACCESS)dev->space[0].virtAddr;
	DBGWRT_2((DBH,"  mapped to: addr=0x%08x\n",dev->space[0].virtAddr));

	if (dev->irqUse || (dev->irqInfo & BBIS_IRQ_EXPIRQ)) {

		/* determine interrupt parameters */
		if( (error = MDIS_GetIrqParams( dev )))
			goto errexit;
	}

	/* prepare board slot */
	if( (error = MDIS_SetMiface( dev ))){
		error = -EINVAL;
		goto errexit;
	}

	OSS_DL_AddTail( &G_devList, &dev->node ); /* add to device list */
	dev->initialized = TRUE;

 goodexit:
	MK_UNLOCK;
	dev->useCount++;
	*devP = dev;
	if( ossHandleP )
		*ossHandleP = dev->osh;
	*mappedAddrP = (void *)dev->ma[0];
	return 0;

 errexit:
	MDIS_FinalClose( dev );
	MK_UNLOCK;
	DBGWRT_ERR((DBH,"*** %s %s failed error=%d\n", fname, devName, error ));

	return error;	
}

/**********************************************************************/
/** Deinitialize external device
 *
 * Decrements the devices useCount and - if its zero - deinitializes
 * all resources
 *
 * \param  _dev		device handle returned by #mdis_open_external_dev
 *
 * \return negative linux error number
 */
int mdis_close_external_dev( void *_dev )
{
	MK_DEV *dev = (MK_DEV *)_dev;
	DBGCMD(const char fname[] = "mdis_close_external_dev: "; )
	int32 error;

	if( dev == NULL )
		return -EINVAL;

	MK_LOCK(error);
	if( error )
		return -EINTR;

	DBGWRT_1((DBH,"%sdev=%s\n", fname, dev->devName ));

	mdis_remove_external_irq( _dev );

	if( --dev->useCount == 0 ){
		MDIS_FinalClose( dev );
	}

	MK_UNLOCK;
	return 0;
}

/**********************************************************************/
/** Install interrupt handler and enable interrupt for external device
 *
 * Installs \a handler as the interrupt handler for the external device.
 * \a handler is called in the context of the global MDIS_IrqHandler()
 * routine.
 *
 * \a handler gets passed one argument: The value of \a data passed to
 * mdis_install_external_irq().
 *
 * \a handler only needs to deal with the devices hardware regs, not
 * with the carrier board registers (this is done by MDIS).
 *
 * \a handler shall return the same return codes as a MDIS LL driver,
 * allthough the return value is currently ignored...
 *
 * \param  _dev		device handle returned by #mdis_open_external_dev
 * \param  handler	interrupt service routine
 * \param  data		passed unmodified to \a handler
 *
 * \return negative linux error number
 */
int mdis_install_external_irq(
	void *_dev,
	int (*handler)(void *data),
	void *data )
{
	MK_DEV *dev = (MK_DEV *)_dev;
	DBGCMD(const char fname[] = "mdis_install_external_irq: "; )
	int32 error;

	DBGWRT_1((DBH,"%s\n", fname ));

	dev->llJumpTbl.irq = (int32 (*)(LL_HANDLE*))handler;
	dev->ll			   = data;

	/*
	 * install normal interrupt
	 */
	if( (error = MDIS_InstallSysirq( dev )))
		return -EINVAL;

	/* enable irq on carrier board */
	if( (error = MDIS_EnableIrq( dev, TRUE )) )
		return -EINVAL;

	return 0;
}


/**********************************************************************/
/** Disable IRQ and remove interrupt handler
 *
 * The actions of this routine are also done by mdis_close_external_dev()
 *
 * \param  _dev		device handle returned by #mdis_open_external_dev
 *
 * \return negative linux error number
 */
int mdis_remove_external_irq( void *_dev )
{
	MK_DEV *dev = (MK_DEV *)_dev;
	DBGCMD(const char fname[] = "mdis_remove_external_irq: "; )
	int32 error;

	DBGWRT_1((DBH,"%s\n", fname ));

	MDIS_RemoveSysirq( dev );

	dev->llJumpTbl.irq = NULL;
	dev->ll			   = NULL;

	return 0;
}

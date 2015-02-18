/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: close.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2011/04/14 17:58:05 $
 *    $Revision: 2.6 $
 *
 *  Description: Close routine for MDIS kernel module
 *
 *     Required: -
 *     Switches: DBG
 *
 * ??? Exception IRQ disable
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: close.c,v $
 * Revision 2.6  2011/04/14 17:58:05  CRuff
 * R: 1. make en-/disabling of Board Interrupt and LL Interrupt to be an atomic
 *       operation
 *    2. really disable board interrupts during MDIS close
 * M: 1. call MK_UNLOCK before disabling interrupts (will be locked inside)
 *    2. set initialized flag of device to false after diabling the interrupts
 *       to avoid calling ll irq handler after the board hdl is already closed
 *
 * Revision 2.5  2007/06/27 11:58:38  ts
 * assert that dev->drv is valid
 *
 * Revision 2.4  2005/03/21 10:00:25  ts
 * module.h included, unresolved "module_put can occur without
 *
 * Revision 2.3  2005/01/19 11:31:09  ts
 * removed unnecessary module.h include
 *
 * Revision 2.2  2004/06/09 11:12:28  kp
 * added module_put to LL driver module (Linux 2.6)
 *
 * Revision 2.1  2003/04/11 16:26:17  kp
 * added use of MK_RTAI_SET_DEVLISTLOCK_FLAG
 *
 * Revision 2.0  2003/02/21 13:34:31  kp
 * added support for RTAI and non-mdis drivers
 *
 * Revision 1.3  2003/02/21 10:11:32  kp
 * fixed problem when mapping PCI I/O spaces
 *
 * Revision 1.2  2001/08/16 10:15:48  kp
 * support VME devices
 *
 * Revision 1.1  2001/01/19 14:58:30  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <linux/module.h>
#include "mk_intern.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int32 ClrMiface( MK_DEV *dev );

/******************************** MDIS_FinalClose ****************************
 *
 *  Description: Close last path to device
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev	 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....: -
 ****************************************************************************/
int32 MDIS_FinalClose( MK_DEV *dev )
{
    int32 error,n;     /* error holder */

    DBGWRT_1((DBH,"MK - FinalClose:\n"));
    DBGWRT_2((DBH,"MK - FinalClose: device=%s\n", dev->devName));

	if( dev == NULL ) return ERR_SUCCESS;

	/*------------------------------+
	|  de-init device               |
	+------------------------------*/
	/* disable irqs (if installed), ignore error */
	if (dev->irqUse) {
		MK_UNLOCK;
		MDIS_EnableIrq( dev, FALSE );
		MK_LOCK(error);
	}
	dev->initialized = FALSE;

	/* de-init device */
#ifdef DBG
	if( dev->ll ) DBGWRT_2((DBH," deinit dev\n"));
#endif
	if (dev->ll && (error = dev->llJumpTbl.exit(&dev->ll ))) {
		DBGWRT_ERR((DBH," *** FinalClose: device exit failed\n"));
	}


	/* decr. module use count, assert that dev->drv is valid! */
	if (dev->drv)
		module_put( dev->drv->module );

	/*------------------------------+
	|  Terminate process locking    |
	+------------------------------*/
	MDIS_TermLockMode( dev );

	/*------------------------------+
	|  cleanup interrupt            |
	+------------------------------*/
	MDIS_RemoveSysirq( dev );

	/*------------------------------+
	|  cleanup board slot           |
	+------------------------------*/
	if( dev->brd ){
		DBGWRT_2((DBH," deinit brd\n"));
		ClrMiface( dev );
	}

	if( dev->semDev ) OSS_SemRemove( dev->osh, &dev->semDev );

	/*------------------------------+
	|  unmap address spaces         |
	+------------------------------*/

	for (n=0; n<dev->spaceCnt; n++) {	/* for all addr spaces */
		if( dev->space[n].flags & MK_MAPPED ){
			if( (error = OSS_UnMapVirtAddr(dev->osh,
										  &dev->space[n].virtAddr,
										  dev->space[n].reqSize,
										  dev->space[n].type))) {
				DBGWRT_ERR((DBH," *** FinalClose: can't unmap addr "
							"space[%d]\n",n));
			}
		}
		if( dev->space[n].flags & MK_REQUESTED ){
			if( dev->space[n].type == OSS_ADDRSPACE_MEM ){
				release_mem_region(
					(unsigned long)dev->space[n].physAddr,
					dev->space[n].reqSize);
			}
			else {
				release_region(
					(unsigned long)dev->space[n].physAddr,
					dev->space[n].reqSize);
			}
		}
	}

	/*------------------------------+
	|  close BBIS driver            |
	+------------------------------*/
	DBGWRT_2((DBH," close board\n"));
	bbis_close( dev->brdName );

	/*-----------------------------------------------+
	|  Deregister device in global MDIS device list  |
	+-----------------------------------------------*/
	if( dev->node.next ){
		MK_RTAI_SET_DEVLISTLOCK_FLAG( TRUE ); /* sync with RTAI MDIS */
		OSS_DL_Remove( &dev->node );
		MK_RTAI_SET_DEVLISTLOCK_FLAG( FALSE );
	}	
	if( dev->osh )
		OSS_Exit( &dev->osh );

	kfree( dev );

    return(ERR_SUCCESS);
}

/******************************* MDIS_RemoveSysirq ***********************
 *
 *  Description: Remove & disable the system interrupt
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev 	 device structure
 *  Output.....: return  success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_RemoveSysirq(MK_DEV *dev)
{
	DBGWRT_2((DBH," irq installed: %d\n", dev->irqInstalled ));

	/* remove system irq */
	if (dev->irqInstalled) {
		DBGWRT_2((DBH," remove system interrupt: vect=%d\n",
				  dev->irqVector));

#ifdef CONFIG_MEN_VME_KERNELIF
		if( dev->busType == OSS_BUSTYPE_VME )
			vme_free_irq( dev->irqVector, dev );
		else
#endif
			free_irq( dev->irqVector, dev );
		dev->irqInstalled = 0;
	}
	return ERR_SUCCESS;
}

/********************************* MDIS_TermLockMode *************************
 *
 *  Description: Delete locking semaphores
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev			device structure
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
void MDIS_TermLockMode( MK_DEV *dev )
{
	int32 n;

	switch(dev->lockMode) {
	case LL_LOCK_NONE:
		break;

	case LL_LOCK_CALL:
		if( dev->semRead ) 		OSS_SemRemove( dev->osh, &dev->semRead );
		if( dev->semWrite ) 	OSS_SemRemove( dev->osh, &dev->semWrite );
		if( dev->semBlkRead ) 	OSS_SemRemove( dev->osh, &dev->semBlkRead );
		if( dev->semBlkWrite ) 	OSS_SemRemove( dev->osh, &dev->semBlkWrite );
		if( dev->semSetStat ) 	OSS_SemRemove( dev->osh, &dev->semSetStat );
		if( dev->semGetStat ) 	OSS_SemRemove( dev->osh, &dev->semGetStat );
		break;
	case LL_LOCK_CHAN:
		if( dev->semChanP ){
			for( n=0; n<dev->devNrChan; n++ ){
				if( dev->semChanP[n] )
					OSS_SemRemove( dev->osh, &dev->semChanP[n]);
			}
			OSS_MemFree( dev->osh, dev->semChanP, dev->semChanAlloc );
		}
		break;
	}
}

/********************************* ClrMiface ********************************
 *
 *  Description: clears the device slot if this was the last device on slot
 *			
 *---------------------------------------------------------------------------
 *  Input......: dev			device structure
 *  Output.....: return  	success (0) or error code
 *  Globals....: -
 ****************************************************************************/
static int32 ClrMiface( MK_DEV *dev )
{
	MK_DEV *dev2;
	int32 error;

	/* check if another device is using the interface */
	for( dev2=(MK_DEV *)G_devList.head;
		 dev2->node.next;
		 dev2 = (MK_DEV *)dev2->node.next ){

		if( strcmp(dev2->brdName, dev->brdName) == 0 &&
			dev2->devSlot == dev->devSlot ){
			
			/* yes, another device is using the interface */
			return 0;
		}
	}

	if( (error = dev->brdJumpTbl.clrMIface(dev->brd, dev->devSlot))) {
		DBGWRT_ERR((DBH,"*** MK: can't deinit board slot\n"));
	}
	return error;
}

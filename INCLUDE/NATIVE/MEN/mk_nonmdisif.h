/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  mk_nonmdisif.h
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/02/21 13:34:29 $
 *    $Revision: 2.1 $
 *
 *  	 \brief  Export declaration to be included by non-MDIS drivers
 *				 using the MDIS configuration & BBIS support
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mk_nonmdisif.h,v $
 * Revision 2.1  2003/02/21 13:34:29  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _MK_NONMDISIF_H
#define _MK_NONMDISIF_H

int mdis_open_external_dev(
	char *devName,
	char *brdName,
	int slotNo,
	int addrMode,
	int dataMode,
	int addrSpaceSize,
	void **mappedAddrP,
	void **ossHandleP,
	void **devP );

int mdis_close_external_dev( void *_dev );

int mdis_install_external_irq(
	void *_dev,
	int (*handler)(void *data),
	void *data );

int mdis_remove_external_irq( void *_dev );

#endif /* _MK_NONMDISIF_H */

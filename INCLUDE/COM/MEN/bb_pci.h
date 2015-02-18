/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_pci.h
 *
 *       Author: kp
 *        $Date: 1999/08/09 15:25:53 $
 *    $Revision: 2.1 $
 *
 *  Description: Header file for PCI BBIS driver
 *               - PCI specific status codes
 *               - PCI function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_pci.h,v $
 * Revision 2.1  1999/08/09 15:25:53  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BB_PCI_H_
#define _BB_PCI_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*---------------------------------------------------------------------------+
|    DEFINES                                                                 |
+---------------------------------------------------------------------------*/
/*-- PCI specific status codes ( M_BRD_OF / M_BRD_BLK_OF + 0x00...0xff ) --*/
/*											 S,G: S=setstat, G=getstat code */

/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void PCI_GetEntry( BBIS_ENTRY* bbisP );
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */

#ifdef __cplusplus
	}
#endif

#endif /* _BB_PCI_H_ */


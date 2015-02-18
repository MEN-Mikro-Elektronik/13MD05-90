/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_a21.h
 *
 *       Author: kp
 *        $Date: 2001/04/11 11:05:54 $
 *    $Revision: 2.1 $
 *
 *  Description: Header file for A21 BBIS driver
 *               - A21 specific status codes
 *               - A21 function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_a21.h,v $
 * Revision 2.1  2001/04/11 11:05:54  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BB_A21_H_
#define _BB_A21_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*---------------------------------------------------------------------------+
|    DEFINES                                                                 |
+---------------------------------------------------------------------------*/
/*-- A21 specific status codes ( M_BRD_OF / M_BRD_BLK_OF + 0x00...0xff ) --*/
/*											 S,G: S=setstat, G=getstat code */

/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void A21_GetEntry( BBIS_ENTRY* bbisP );
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */

#ifdef __cplusplus
	}
#endif

#endif /* _BB_A21_H_ */


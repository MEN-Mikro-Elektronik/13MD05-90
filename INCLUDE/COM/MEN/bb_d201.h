/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_d201.h
 *
 *       Author: ds
 *        $Date: 1999/12/08 10:59:35 $
 *    $Revision: 1.5 $
 *
 *  Description: Header file for D201 BBIS driver
 *               - D201 specific status codes
 *               - D201 function prototypes
 *
 *     Switches:  _ONE_NAMESPACE_PER_DRIVER_
 *				  _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_d201.h,v $
 * Revision 1.5  1999/12/08 10:59:35  Gromann
 * removed switch _LL_DRV_
 *
 * Revision 1.4  1999/09/08 09:30:28  Schmidt
 * all new written, old history entries 1.2..1.3 removed
 *
 * Revision 1.1  1998/02/23 17:39:09  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BB_D201_H_
#define _BB_D201_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*---------------------------------------------------------------------------+
|    DEFINES                                                                 |
+---------------------------------------------------------------------------*/
/*-- D201 specific status codes ( M_BRD_OF / M_BRD_BLK_OF + 0x00...0xff ) --*/
/*											 S,G: S=setstat, G=getstat code */
#define D201_BLK_EEPROM_DATA M_BRD_BLK_OF+0x01	/* G:  read eeprom data     */	

/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void D201_GetEntry( BBIS_ENTRY* bbisP );
	extern void C203_GetEntry( BBIS_ENTRY* bbisP );
	extern void C204_GetEntry( BBIS_ENTRY* bbisP );
	extern void F201_GetEntry( BBIS_ENTRY* bbisP );
	extern void F202_GetEntry( BBIS_ENTRY* bbisP );
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */

#ifdef __cplusplus
	}
#endif


#endif /* _BB_D201_H_ */




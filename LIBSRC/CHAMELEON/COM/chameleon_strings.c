/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  chameleon_strings.c
 *
 *      \author  kp
 *        $Date: 2008/03/31 11:31:36 $
 *    $Revision: 2.21 $
 *
 *        \brief  String routines for chameleon library
 *
 *     Switches: -
 */
 /*-------------------------------[ History ]---------------------------------
 *
 * $Log: chameleon_strings.c,v $
 * Revision 2.21  2008/03/31 11:31:36  DPfeuffer
 * modified to run chameleon unittest
 *
 * Revision 2.20  2007/07/04 16:06:55  cs
 * moved G_ChamTbl[] to chameleon.h
 *
 * Revision 2.19  2007/04/13 08:38:01  ts
 * Added 16Z057 (modified Z025, different IRQ handling)
 *
 * Revision 2.18  2007/02/14 17:09:33  rla
 * added 16Z087 table entry
 *
 * Revision 2.17  2007/02/08 10:02:43  aw
 * + CHAMELEON_16Z084_IDEEPROM, + CHAMELEON_16Z085_MS09N
 *
 * Revision 2.16  2006/08/02 10:53:21  DPfeuffer
 * 16Z082_TACHO renamed to 16z082_IMPULSE
 *
 * Revision 2.15  2006/08/01 15:31:58  ts
 * + CHAMELEON_16Z082_TACHO
 *
 * Revision 2.14  2006/07/11 10:38:30  rla
 * Added Z056 SPI entry
 *
 * Revision 2.13  2006/05/12 10:02:03  RLange
 * Added Z125_UART string
 *
 * Revision 2.12  2006/04/27 11:23:13  cs
 * added 16Z074, 16Z078 - 16Z081
 *
 * Revision 2.11  2006/02/02 17:19:50  cs
 * fixed:
 * - changed devId of  CHAMELEON_16Z023_IDENHS from 23 to 123
 *
 * Revision 2.10  2005/12/20 09:44:52  cs
 * 16Z052_SRAM -> 16Z052_GIRQ (16Z052_SRAM was never used and is replaced)
 * + OWB, QDEC, SPEED, QSPI, ETH strings
 *
 * Revision 2.9  2005/08/31 14:52:56  kp
 * added IP cores 065..069
 * 16Z053_IDE -> 16Z053_IDETGT
 *
 * Revision 2.8  2005/07/11 10:33:25  UFranke
 * changed
 *  -16Z063 from NAND to NAND_RAW
 *  -16Z053 from NAND to IDE
 *
 * Revision 2.7  2005/06/23 16:12:05  kp
 * Copyright line changed (sbo)
 *
 * Revision 2.6  2005/05/04 08:59:42  kp
 * cosmetic
 *
 * Revision 2.5  2005/04/29 14:45:20  dpfeuffer
 * - ChameleonHwName() removed
 * - CHAM_DevIdToModCode() added
 * - CHAM_ModCodeToDevId() added
 *
 * Revision 2.4  2005/03/16 16:51:04  kp
 * + PWM, DMA, NAND strings
 *
 * Revision 2.3  2005/02/10 09:59:56  kp
 * + CHAMELEON_16Z023_IDENHS
 *
 * Revision 2.2  2005/02/02 16:36:18  kp
 * fixed bad name for CHAMELEON_16Z031_SPI
 *
 * Revision 2.1  2005/01/21 16:30:18  ts
 * corrected table entry for 16Z044_DISP
 *
 * Revision 2.0  2005/01/21 13:45:18  kp
 * changed ChameleonModName table to use official codenames
 *
 * Revision 1.6  2004/12/21 12:21:10  CSchuster
 * added CHAMELEON_ERIVAN
 *
 * Revision 1.5  2004/11/29 10:50:09  kp
 * cosmetics
 *
 * Revision 1.4  2004/11/04 11:55:01  kp
 * added defs for new chameleon units
 *
 * Revision 1.3  2004/05/24 11:31:50  dpfeuffer
 * ChameleonHwName() implemented
 *
 * Revision 1.2  2004/03/09 10:41:57  kp
 * update chameleon module name table and searching
 *
 * Revision 1.1  2003/03/14 15:16:25  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifdef CHAM_UNITTEST
	#include "../UNITTEST/COM/cha_ut.h"
#endif

#define _CHAMELEON_STRINGS_C
/*--------------------------------------*/
/*    INCLUDES                          */
/*--------------------------------------*/
#include <MEN/men_typs.h>
#include <MEN/chameleon.h>

/**
 * \defgroup _CHAM_CODE_FUNC Chameleon Code Functions
 *  @{
 */

/**********************************************************************/
/** convert module-code to unit name
 *
 *	\param modCode		Chameleon-V0 module-code
 *
 *  \return const string with module name or "?" if unknown
 */
const char *ChameleonModName( u_int16 modCode )
{
	int i;

	for( i=0; G_ChamTbl[i].name; i++ ){
		if( G_ChamTbl[i].modCode == modCode )
			return G_ChamTbl[i].name;
	}
	return "?";
}

/**********************************************************************/
/** convert device-Id to unit name
 *
 *  \param devId		Chameleon-V2 device-Id
 *
 *  \return const string with module name or "?" if unknown
 */
const char *CHAM_DevIdToName( u_int16 devId )
{
	int i;

	for( i=0; G_ChamTbl[i].name; i++ ){
		if( G_ChamTbl[i].devId == devId )
			return G_ChamTbl[i].name;
	}
	return "?";
}

/**********************************************************************/
/** convert module-code to device-Id
 *
 *	\param modCode	Chameleon-V0 module-code
 *
 *  \return			Chameleon-V2 device-Id or 0xffff if no match
 */
u_int16 CHAM_ModCodeToDevId( u_int16 modCode )
{
	int i;

	for( i=0; G_ChamTbl[i].name; i++ ){
		if( G_ChamTbl[i].modCode == modCode )
			return G_ChamTbl[i].devId;
	}
	return 0xffff;	/* unknown */
}

/**********************************************************************/
/** convert device-Id to module-code
 *
 *	\param devId	Chameleon-V2 device-Id
 *
 *  \return			Chameleon-V0 module-code or 0xffff if no match
 */
u_int16 CHAM_DevIdToModCode( u_int16 devId )
{
	int i;

	for( i=0; G_ChamTbl[i].name; i++ ){
		if( G_ChamTbl[i].devId == devId )
			return G_ChamTbl[i].modCode;
	}
	return 0xffff;	/* unknown */
}

/*! @} */



/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_d201e.h
 *
 *       Author: ds
 *        $Date: 2009/08/06 09:04:19 $
 *    $Revision: 1.5 $
 *
 *  Description: reading words from the d201 EEPROM (NMC93CS46)
 *
 *     Switches: -
 *
 *	NOTE:	The most part of this code is an extract from 'PCI.C',
 *			a source file from the PLXmon Programm (see the header below).
 *	************************************************************************
 *
 *	PCI.C
 *
 *	User application level code for showing and programming PCI devices.
 *	Emphasis on PLX devices.
 *	Designed as a component of PLX's general-purpose monitor program.
 *
 *	Copyright PLX Technology, 1996
 *
 *	Changes
 *	960521	Ryan	Genesis - PCI BIOS and PCI device interface
 *
 *	*************************************************************************
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_d201e.h,v $
 * Revision 1.5  2009/08/06 09:04:19  CRuff
 * R: make 64bit compatible
 * M: change signature of EepromReadBuf; use type U_INT32_OR_64 for address
 *
 * Revision 1.4  2000/03/09 12:18:59  kp
 * D201_GLOBNAME macro now used to prefix global symbols
 *
 * Revision 1.3  2000/03/01 14:56:43  kp
 * EepromReadBuf was a global symbol. Renamed this variant specific
 * Removed all global (static variables). Sustituted with defines
 * Made PLD data const array
 *
 * Revision 1.2  1998/08/04 16:46:30  Schmidt
 * cosmetics
 *
 * Revision 1.1  1998/02/23 17:39:01  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _D201EEPROM_H_
#define _D201EEPROM_H_

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef D201_VARIANT
# define D201_VARIANT D201
#endif

#ifndef D201_GLOBNAME
#define _D201_GLOBNAME(var,name) var##_##name
# ifndef _ONE_NAMESPACE_PER_DRIVER_
#  define D201_GLOBNAME(var,name) _D201_GLOBNAME(var,name)
# else
#  define D201_GLOBNAME(var,name) _D201_GLOBNAME(D201,name)
# endif
#endif

#define EepromReadBuf	D201_GLOBNAME(D201_VARIANT,EepromReadBuf)

/* EEPROM constant */
#define EE_ADDR_MASK	0x003f		/* for combining addresses with instructions	*/
#define EE_CMD_LEN		9			/* bits in instructions							*/
#define EE_READ			0x0180		/* 01 1000 0000 read instruction				*/
#define EE_WRITE		0x0140		/* 01 0100 0000 write instruction				*/
#define EE_WREN			0x0130		/* 01 0011 0000 write enable instruction		*/
#define EE_WDS			0x0100		/* 01 0000 0000 write disable instruction		*/
#define EE_PREN			0x0130		/* 01 0011 0000 protect enable instruction		*/
#define EE_PRCLEAR		0x0177		/* 01 1111 1111 clear protect register instr	*/

/* EEPROM Functions */
extern u_int32	EepromReadBuf(U_INT32_OR_64 MiscCtrlReg, u_int16 *dest, u_int16 offset, u_int16 wcount);

extern char* D201e_Ident( void );

#ifdef __cplusplus
	}
#endif

#endif	/* _D201EEPROM_H_ */


/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_d201e.c
 *      Project: d201 board handler
 *
 *       Author: ds
 *        $Date: 2009/08/05 17:33:31 $
 *    $Revision: 1.7 $
 *
 *  Description: reading words from the d201 EEPROM (NMC93CS46)
 *
 *     Required: -
 *     Switches: see bb_d201.c
 *
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
 *---------------------------[ Public Functions ]----------------------------
 *
 *  EepromReadBuf(MiscCtrlReg, *dest, location, wcount)
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_d201e.c,v $
 * Revision 1.7  2009/08/05 17:33:31  CRuff
 * R: make 64bit compatible
 * M: change function signatures; use U_INT32_OR_64 for address parameters
 *
 * Revision 1.6  2008/09/26 13:54:39  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 1.5  2000/06/21 16:12:54  Schmidt
 * include <MEN/dbg.h> added
 *
 * Revision 1.4  2000/03/09 12:18:53  kp
 * cosmetics
 *
 * Revision 1.3  2000/02/24 16:43:04  Franke
 * big endian bug removed
 *
 * Revision 1.2  1998/09/21 11:32:52  Schmidt
 * IdentString : removed
 *
 * Revision 1.1  1998/02/23 17:38:58  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include <MEN/maccess.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/pci9050.h>	/* PCI9050 defines				*/
#include "bb_d201e.h"

/* '(UINT) EpromConReg = 0x50' changed to '(DWORD) MiscCtrlReg' */

/* defines for data types */
#define WORD	u_int16 /* unsigned short */
#define BOOL	u_int32 /* unsigned long */
#define UINT	u_int32 /* unsigned long */
#define DWORD	u_int32 /* unsigned long */

static WORD EepromReadWord(U_INT32_OR_64 MiscCtrlReg, UINT EpromAddr);
static BOOL EepromSendCmd(UINT cmd, U_INT32_OR_64 MiscCtrlReg);
static BOOL EepromClock(U_INT32_OR_64 MiscCtrlReg);
static DWORD ReadRtrDword(U_INT32_OR_64 reg);
static void WriteRtrDword(U_INT32_OR_64 reg, DWORD val);

extern BOOL
EepromReadBuf(U_INT32_OR_64 MiscCtrlReg, u_int16 *dest, u_int16 offset, u_int16 wcount)
{	/* Read words from EEPROM into buffer*/
	u_int16 i;

	for (i = 0; i < wcount; i++)
		dest[i] = EepromReadWord(MiscCtrlReg, i+offset);
	return 0;	/*ds all was ok */
}

static WORD
EepromReadWord(U_INT32_OR_64 MiscCtrlReg, UINT EpromAddr)
{	/* Return a word from National Semiconductor's NMC93S06/NMC93S46 EEPROM	*/
	WORD acc = 0x0;
	UINT i;

	EepromSendCmd(EE_READ | (EpromAddr & EE_ADDR_MASK), MiscCtrlReg);

	for (i = 0; i < 16; i++)
	{	/* Get word from EEPROM - one bit at a time */
		acc <<= 1;
		EepromClock(MiscCtrlReg);
		if (ReadRtrDword(MiscCtrlReg) & MISC_RX_EPR)
			acc |= 0x0001;
	}
#ifdef _LITTLE_ENDIAN_
	return acc;
#else
	#ifdef _BIG_ENDIAN_
	return( (WORD)(OSS_SWAP16(acc)) );
	#else
#		error "_LITTLE_ENDIAN_ or _BIG_ENDIAN_ must be defined"
	#endif
#endif
}

static BOOL
EepromSendCmd(UINT cmd, U_INT32_OR_64 MiscCtrlReg)
{	/* Send a command to National Semiconductor's NMC93S06/NMC93S46 EEPROM */
	UINT i;
	DWORD regval;
	
	regval = ReadRtrDword(MiscCtrlReg);
	regval |=  MISC_CS_EPR;	/* Chip select TRUE	*/
	regval &= ~(MISC_TX_EPR|MISC_CLK_EPR);	/* Clear instruction and clock bits */
	/* Toggle EEPROM's chip select (to get it out of Shift Register Mode) */
	WriteRtrDword(MiscCtrlReg, regval & ~MISC_CS_EPR); /* Chip select FALSE */
	WriteRtrDword(MiscCtrlReg, regval);

	for (i = 0; i < EE_CMD_LEN; i++)
	{	/* Send instruction - one bit at a time */
		regval &= ~MISC_TX_EPR;	/* Pre-clear the instruction bit */
		/* Set up one instruction bit */
		regval |= (cmd & (0x01 << (EE_CMD_LEN-1)))? MISC_TX_EPR: 0x00000000;
		WriteRtrDword (MiscCtrlReg, regval);
		EepromClock(MiscCtrlReg);
		cmd <<= 1;	/* Align next instruction bit */
	}
	return TRUE;
}

static BOOL
EepromClock(U_INT32_OR_64 MiscCtrlReg)
{	/* Send clocking sequence to Nat Sem's NMC93S06/NMC93S46 EEPROM */
	DWORD regval;

	regval = ReadRtrDword(MiscCtrlReg);
	WriteRtrDword(MiscCtrlReg, regval & ~MISC_CLK_EPR);	/* Clock low */
	WriteRtrDword(MiscCtrlReg, regval | MISC_CLK_EPR);	/* Clock high */
	return TRUE;
}

static DWORD
ReadRtrDword(U_INT32_OR_64 reg)	/*ds change WORD to DWORD*/
{	/* Port read a DWORD from the current device's runtime regs */
	/*ds return devReadRtrDword(CurrDev(), reg); */
	return ( MREAD_D32(reg,0) );	
}

static void
WriteRtrDword(U_INT32_OR_64 reg, DWORD val) /*ds change WORD to DWORD */
{	/* Write a DWORD to the current device's runtime regs */
	/* Caviat: we don't (dynamically) know where the device's reg base is*/
	/*ds devWriteRtrDword(CurrDev(), reg, val); */
	MWRITE_D32(reg,0,val);
	return;
}




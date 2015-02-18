/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: d201.h
 *
 *       Author: ds
 *        $Date: 2001/11/14 16:51:49 $
 *    $Revision: 1.11 $
 *
 *  Description: D201 BBIS - Common defines and prototypes
 *
 *     Switches: D201_VARIANT
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: d201.h,v $
 * Revision 1.11  2001/11/14 16:51:49  Schmidt
 * GetEntry function renamed to __D201_GetEntry for D201D302 BBIS
 *
 * Revision 1.10  2000/06/13 09:49:52  kp
 * Corrected offsets to 16 bit regs for BIG_ENDIAN and BYTESWAP
 * (only relevant if M-module is in D32 mode)
 *
 * Revision 1.9  2000/03/09 12:18:57  kp
 * D201_GLOBNAME macro now used to prefix global symbols
 * Added offset to 16 bit regs in swapped mode
 *
 * Revision 1.8  2000/03/01 14:56:47  kp
 * EepromReadBuf was a global symbol. Renamed this variant specific
 * Removed all global (static variables). Sustituted with defines
 * Made PLD data const array
 *
 * Revision 1.7  1999/09/08 09:30:12  Schmidt
 * all new written, old history entries 1.2..1.6 removed
 *
 * Revision 1.1  1998/02/23 17:39:03  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef D201_H_
#define D201_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*-----------------------------------------+
|  DEFINES (COMMON)                        |
+-----------------------------------------*/

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

/* variant specific function names */
#define Ident 			D201_GLOBNAME(D201_VARIANT,Ident)
#define PldIdent		D201_GLOBNAME(D201_VARIANT,PldIdent)
#define ttf_D201etc		D201_GLOBNAME(D201_VARIANT,ttf_D201etc)
#define ttf_F201etc		D201_GLOBNAME(D201_VARIANT,ttf_F201etc)
#define __D201_GetEntry	D201_GLOBNAME(D201_VARIANT,GetEntry)


#define D201_RES_NBR (BRD_SLOT_NBR+1)		/* number of resources			*/

/* PCI Config Space */
#define D201_PCI_VEN_ID			0x10b5		/* Vendor Id					*/
#define D201_PCI_DEV_ID			0x9050		/* Device Id					*/
#define D201_PCI_SUBVEN_ID		0x10b5		/* Subsystem Vendor Id			*/
#define D201_PCI_SUBDEV_ID		0x9050		/* Subsystem Device Id			*/
#define D201_PCI_RR_MEM			0			/* Base Addr RReg (MEM)			*/
#define D201_PCI_RR_IO			1			/* Base Addr RReg (IO)			*/
#define D201_PCI_M_MEM(nr)		(nr+2)		/* Base Addr Module 0..3 (MEM)	*/

/* M-Module address spaces */
#define D201_M_A24				0x00		/* A24 location					*/
#define D201_M_A24_SIZE         0x00fffe00	/* A24 size						*/
#define D201_M_A08				0x00fffe00	/* A08 location					*/
#define D201_M_A08_NOA24		0x00		/* A08 location (no A24)		*/
#define D201_M_A08_SIZE         0x100		/* A08 size						*/
#define D201_M_CTRL				0x00ffff00	/* ctrl reg location            */
#define D201_M_CTRL_NOA24		0x100		/* ctrl reg location (no A24)	*/
#define D201_M_CTRL_SIZE        0x100		/* ctrl reg size                */

/* PLD loader interface */
#define D201_PLD_DATA			MISC_TX_EPR
#define D201_PLD_DCLK			MISC_CLK_EPR
#define D201_PLD_CONFIG			(MISC_IO2_DATA | MISC_IO3_DATA | MISC_CS_EPR)
#define D201_PLD_STATUS			MISC_IO0_DATA
#define D201_PLD_CFGDONE		MISC_IO1_DATA

/* EEPROM specifics */
#define D201_EE_ID				0x64        /* D201 EEPROM ID location		*/
#define D201_EE_ID_LENGTH		28          /* 28 bytes long				*/
#define D201_EE_VEN_OFFSET		2           /* offset to vendor ID			*/
#define D201_EE_VEN_LENGTH		8           /* length of vendor ID			*/
#define D201_EE_VEN_ID			"MEN GmbH"  /* Vendor Id (8 char)			*/
#define D201_EE_PROD_OFFSET		10          /* offset to product ID			*/
#define D201_EE_PROD_LENGTH		6           /* length of product ID 6 of 9	*/

/*
 * M-Module ctrl reg (locations and bit definitions)
 */
/* --------------------- Registers ------------------ */
# if (defined(_BIG_ENDIAN_) && !defined(MAC_BYTESWAP)) || (defined(_LITTLE_ENDIAN_) && defined(MAC_BYTESWAP))
# define D201_16BITREGS_OFF	0x02 	/* address correct word */
#else
# define D201_16BITREGS_OFF	0x00
#endif

#define D201_M_MODE			(0x00+D201_16BITREGS_OFF)
#define D201_M_INT			(0x04+D201_16BITREGS_OFF)
#define D201_M_TRIGA_DIV	(0x80+D201_16BITREGS_OFF)
#define D201_M_TRIGB_DIV	(0x84+D201_16BITREGS_OFF)
#define D201_M_TRIGA_CTRL	(0x88+D201_16BITREGS_OFF)
#define D201_M_TRIGB_CTRL	(0x8c+D201_16BITREGS_OFF)
#define D201_M_GEOADDR		(0x94+D201_16BITREGS_OFF)

/* ------------------- Mode Control ----------------- */
#define D201_M_MODE_ABITS	0x03    /* addr mode bits */
#define D201_M_MODE_A08		0x00    /* A08			  */
#define D201_M_MODE_A24		0x02    /* A24			  */

#define D201_M_MODE_DBITS	0x0C    /* data mode bits */
#define D201_M_MODE_D16		0x04    /* data mode D16  */
#define D201_M_MODE_D32		0x08    /* data mode D32  */

/* ------------ Interrupt Control/Status ------------ */
#define D201_M_INT_ENABLE	0x10    /* Int_en		  */
#define D201_M_INT_TIMEOUT	0x20    /* Timeout_flag   */
#define D201_M_INT_BUSERR	0x40    /* Bus_error_flag */
#define D201_M_INT_INTFLAG	0x80    /* Int_flag       */

/* ---------------- Geographic Address -------------- */
#define D201_M_GEOADDR_MASK 0x1F	/* mask			  */

/*-----------------------------------------+
|  PROTOTYPES (COMMON)                     |
+-----------------------------------------*/
/* externals */
extern char*  Ident(void);			/* BBIS ident */
extern char*  PldIdent(void);		/* ttf data ident */
extern const u_int8 ttf_D201etc[];		/* D201 ttf data	   */
extern const u_int8 ttf_F201etc[];		/* F201 ttf data	   */


#ifdef __cplusplus
    }
#endif

#endif /* D201_H_ */





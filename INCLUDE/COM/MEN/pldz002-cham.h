/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  pldz002.h
 *
 *      \author  dieter.schmidt@men.de
 *        $Date: 2009/02/06 21:14:36 $
 *    $Revision: 2.15 $
 *
 *  	 \brief  MEN A15/A500 PCI2VME bridge PLD register defines (PLD Z002)
 *               16/32 bit registers are described in little endian format
 *
 *     Switches: PLDZ002_REV - specifies the used PLD revision (e.g. 17)
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: pldz002.h,v $
 * Revision 2.15  2009/02/06 21:14:36  cs
 * R: a bit was added to PLDZ002_MSTR register to control bus request mode
 * M: add define to support additional bit in PLDZ002_MSTR register
 *
 * Revision 2.14  2007/04/19 14:18:15  DPfeuffer
 * PLDZ002_MAILBOX(mbx) macro fixed
 *
 * Revision 2.13  2005/11/30 12:52:18  UFranke
 * added
 *  + SLV32 defines
 *
 * Revision 2.12  2005/11/25 14:58:12  UFranke
 * added
 *  + SLV32 defines
 *
 * Revision 2.11  2005/10/20 11:52:58  UFranke
 * added defines
 *  + PLDZ002_SLV24_SIZE_XXXX
 *
 * Revision 2.10  2005/07/05 12:28:41  kp
 * cosmetics
 *
 * Revision 2.9  2004/10/27 09:02:35  kp
 * added macros to allow to access registers of old and new (>= rev 17) PLD
 * from the same code
 *
 * Revision 2.8  2004/08/12 12:18:14  dpfeuffer
 * register defines changed for PLDZ002 Rev. 11 (use PLDZ002_REV switch)
 *
 * Revision 2.7  2004/07/16 15:10:43  kp
 * added new regs of PLDZ002 Rev. 10
 *
 * Revision 2.6  2004/02/05 11:25:37  UFranke
 * removed
 *  - double address definition mailbox control/status
 *
 * Revision 2.5  2003/12/23 13:38:37  dpfeuffer
 * PLDZ002_DMABD_XXX_OFF added
 *
 * Revision 2.4  2003/12/18 15:58:25  UFranke
 * added
 *  + Location Monitor and Mailbox bit definitions
 *
 * Revision 2.3  2003/12/18 09:21:24  UFranke
 * added
 *   + slave 16/32 bit definitions
 *
 * Revision 2.2  2003/12/10 15:57:09  kp
 * fixed some defs for current version of PLDZ002 (now Rev.7)
 *
 * Revision 2.1  2003/10/20 16:30:15  kp
 * alpha state
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _PLDZ002_H
#define _PLDZ002_H

#  ifdef __cplusplus
      extern "C" {
#  endif

#ifndef PLDZ002_REV
#	define PLDZ002_REV 7
#endif

/*--------------------------------------+
|   DEFINES & CONSTS                    |
+--------------------------------------*/

/*--- address spaces within BAR0 (swapped access) and BAR2 ---*/
#define PLDZ002_A24D16_SPACE	0x04000000	/**< A24D16 (standard) space - offset		*/
#define PLDZ002_A24Dxx_SIZE		0x01000000	/**< A24Dxx (standard) space - size (16MB)	*/

#define PLDZ002_A16D16_SPACE	0x00010000	/**< A16D16 (short) space - offset			*/
#define PLDZ002_A16D32_SPACE	0x00020000	/**< A16D32 (short) space - offset			*/
#define PLDZ002_A16Dxx_SIZE		0x00010000	/**< A16Dxx (short) space - size (64kB)		*/

#define PLDZ002_LRAM_SPACE		0x01400000	/**< Local RAM - offset						*/
#define PLDZ002_LRAM_SIZE		0x00100000	/**< Local RAM - size (1MB)					*/

#define PLDZ002_CTRL_SPACE		0x00000000	/**< Bridge Ctrl-Reg - offset				*/
#define PLDZ002_CTRL_SIZE		0x00000050	/**< Bridge Ctrl-Reg - size (80Bytes)		*/

#define PLDZ002_DMA_MAX_BDS		0x10 		/**< max number of BDs  */
#define PLDZ002_DMABD_SIZE 		0x10

/* PLD-Rev >= 17: DMA Buffer Descriptors and Mailbox Data Registers local RAM */
#define PLDZ002_RV11_SRAMREGS 	0xff800		/**< start of "regs" in SRAM  */

/* these two offsets are relative to SRAMREGS! */
#define PLDZ002_DMABD_OFF_RV11(bd) \
		(0x100+(PLDZ002_DMABD_SIZE*(bd)))	/**< DMA Buffer Descriptor (r/w) */
#define PLDZ002_MAILBOX_RV11(mbx)	\
		(0x0+(0x4*(mbx)))					/**< Mailbox Data Register 0..3 (r/w)*/

#define PLDZ002_IACK_SPACE		0x00400000	/**< IACK space - offset					*/
#define PLDZ002_IACK_SIZE		0x00000010	/**< IACK space - size (16 Bytes)			*/
#define PLDZ002_IACK_ID(lev)	((lev<<1)+0x01)	/**< interrupt id for level 			*/

/*--- address spaces within BAR1 (swapped access) and BAR3 ---*/
#define PLDZ002_A32D32_SPACE	0x00000000	/**< A32D32 (long) space - offset			*/
#define PLDZ002_A32D32_SIZE		0x20000000	/**< A32D32 (long) space - size (512MB)		*/

		  /*--- address spaces within BAR0 ---*/
#define PLDZ002_A24D32_SPACE	0x05000000	/**< A24D32 (standard) space - offset			*/

/*
 * PLD-Rev < 17: offsets to PLDZ002_CTRL_IRAM_SPACE and belonging bit defs
 * PLD-Rev >= 17: offsets to PLDZ002_CTRL_SPACE and belonging bit defs
 *		          (for DMABD and MAILBOX: offsets to PLDZ002_LRAM_SPACE)
 */
/* CTRL 0x00 */
#define PLDZ002_INTR			0x0000		/**< Interrupt Control Register (r/w)		*/
#define PLDZ002_INTR_IL_MASK	0x07		/**<	IL0..2 mask	*/
#define PLDZ002_INTR_INTEN		0x0008		/**<	INTEN bit	*/

/* CTRL 0x04 */
#define PLDZ002_INTID			0x0004		/**< VME Interrupt STATUS/ID Register (r/w) */

/* CTRL 0x08 */
#define PLDZ002_ISTAT			0x0008		/**< Interrupt Status Register (r)			*/
#define PLDZ002_ISTAT_ACFST		0x1			/**<	ACFST bit				*/

/* CTRL 0x0C */
#define PLDZ002_IMASK			0x000c		/**< Interrupt Mask Register (r/w)			*/
#define PLDZ002_IMASK_ACFEN		0x01		/**<	ACFEN bit	*/
#define PLDZ002_IMASK_IEN(lev) \
		  ((1<<(lev))&0xfe)					/**<	set IEN1..7 bit	*/

/* CTRL 0x10 */
#define PLDZ002_MSTR			0x0010		/**< Master Control Register (r/w)			*/
#define PLDZ002_MSTR_RMW		0x01		/**<	RMW bit		*/
#define PLDZ002_MSTR_REQ		0x02		/**<	REQ bit		*/
#define PLDZ002_MSTR_BERR		0x04		/**<	BERR bit	*/
#define PLDZ002_MSTR_IBERREN	0x08		/**<	IBERREN bit	*/
#define PLDZ002_MSTR_POSTWR		0x10		/**<	POSTWR bit	*/
#define PLDZ002_MSTR_AONLY		0x20		/**<	AONLY bit	*/
#define PLDZ002_MSTR_VFAIR		0x40		/**<	VFAIR bit	*/

/* CTRL 0x11 */
#define PLDZ002_AMOD			0x0011		/**< Address modifier Register (A21) (r/w)*/
/* mask to wipe AM modifier flags prior to setting */
#define VME4l_SPC_A16_AM_MASK	0x03		/**<	A16 AM (MSTR[8:9]  = AMOD[0:1])	*/	
#define VME4l_SPC_A24_AM_MASK	0x0c		/**<	A24 AM (MSTR[10:11]= AMOD[2:3])	*/
#define VME4l_SPC_A32_AM_MASK	0x30		/**<	A32 AM (MSTR[12:13]= AMOD[4:5])	*/


/* CTRL 0x14 */
#define PLDZ002_SLV24			0x0014		/**< Slave Control Register A24 (r/w)		*/
#define PLDZ002_SLV24_SLEN24			0x0010		/**< A24 slave enable */
#define PLDZ002_SLV24_BASE_MASK_UPPER	0x000f	/**< A24 base upper mask */
#define PLDZ002_SLV24_BASE_MASK_LOWER	0x0f00	/**< A24 base lower mask */
#define PLDZ002_SLV24_SIZE_MASK	0xf000		/**< A24 size mask */
#define PLDZ002_SLV24_SIZE_1MB	0x0000		/**< A24 size 1MB */
#define PLDZ002_SLV24_SIZE_512K	0x8000		/**< A24 size 512 kB */
#define PLDZ002_SLV24_SIZE_256K	0xC000		/**< A24 size 256 kB */
#define PLDZ002_SLV24_SIZE_128K	0xE000		/**< A24 size 128 kB */
#define PLDZ002_SLV24_SIZE_64K	0xF000		/**< A24 size 64  kB */

#define PLDZ002_SLV24_BASE_MASK_UPPER_SHIFT	20	/**< A24 base upper mask shift */
#define PLDZ002_SLV24_BASE_MASK_LOWER_SHIFT	8	/**< A24 base lower mask shift */


/* CTRL 0x18 */
#define PLDZ002_SYSCTL			0x0018		/**< System Controller Register (r/w)		*/
#define PLDZ002_SYSCTL_SYSCON	0x01		/**<	SYSCON bit	*/
#define PLDZ002_SYSCTL_SYSRES	0x02		/**<	SYSRES bit	*/
#define PLDZ002_SYSCTL_ATO		0x04		/**<	ATO bit		*/

/* CTRL 0x1C */
#define PLDZ002_LONGADD			0x001c		/**< Upper 3 Adress Bits for A32 (r/w)		*/
#define PLDZ002_LONGADD_ADR(adr) \
			(((adr)>>29)&0x07)				/**<	ADR31..29	*/

/* CTRL 0x20 */
#define PLDZ002_MAIL_IRQ_CTRL	0x0020		/**< Mailbox Interrupt Enable Register (r/w)*/

/* CTRL 0x24 */
#define PLDZ002_MAIL_IRQ_STAT	0x0024		/**< Mailbox Interrupt Status Register (r/w)*/
#define PLDZ002_MAIL_IRQ_RD		0x01		/**< read  bit box 0 shift left 2/4/6 for box 1/2/3 */
#define PLDZ002_MAIL_IRQ_WR		0x02		/**< write bit box 0 shift left 2/4/6 for box 1/2/3 */

/* CTRL 0x28 */
#define PLDZ002_PCI_OFFSET		0x0028		/**< busmaster slave window PCI offset (r/w)*/

/* CTRL 0x2C */
#define PLDZ002_DMASTA			0x002C		/**< DMA Status Register (r/w)*/
#define PLDZ002_DMASTA_EN		0x01		/**<	DMA_EN bit	*/
#define PLDZ002_DMASTA_IEN		0x02		/**<	DMA_IEN bit	*/
#define PLDZ002_DMASTA_IRQ		0x04		/**<	DMA_IRQ bit	*/
#define PLDZ002_DMASTA_ERR		0x08		/**<	DMA_ERR bit	*/
#define PLDZ002_DMASTA_ACT_BD(dmasta) \
		  (((dmasta)&0xf0)>>4)				/**<	returns number of active buffer descriptor */

/* PLD-Rev <  17: DMA Buffer Descriptors and Mailbox Data Registers in bridge internal RAM */
#define PLDZ002_DMABD_OFF_RV9(bd) \
		(0x900+(PLDZ002_DMABD_SIZE*(bd)))	/**< DMA Buffer Descriptor (r/w) */
#define PLDZ002_MAILBOX_RV9(mbx)	\
		(0x0800+(0x4*(mbx)))				/**< Mailbox Data Register 0..3 (r/w)*/

typedef struct {
	u_int32 destAdr;		/**< DMA_DEST_ADR		*/
	u_int32	srcAdr;			/**< DMA_SOUR_ADR		*/
	u_int32 len;		    /**< DMA_SIZE  */
	u_int32 flags;		    /**< DMA modifiers  */
} PLDZ002_DMABD;

/* offsets within PLDZ002_DMABD */
#define PLDZ002_DMABD_DESTADR_OFF	0x00
#define PLDZ002_DMABD_SRCADR_OFF	0x04
#define PLDZ002_DMABD_LEN_OFF		0x08
#define PLDZ002_DMABD_FLAGS_OFF		0x0c

/* macros to fill fields of PLDZ002_DMABD.flags */
#define PLDZ002_DMABD_SRC(x)	((x)<<16)		/* source location (x is one of PLDZ002_DMABD_DIR_xxx) */
#define PLDZ002_DMABD_DST(x)	((x)<<12)		/* dest location (x is one of PLDZ002_DMABD_DIR_xxx) */
#define PLDZ002_DMABD_AM_A24D16	(0x0<<4)
#define PLDZ002_DMABD_AM_A24D32	(0x4<<4)
#define PLDZ002_DMABD_AM_A32D32	(0x6<<4)
#define PLDZ002_DMABD_AM_A32D64	(0xe<<4)

#define PLDZ002_DMABD_SWAP		0x10
#define PLDZ002_DMABD_NOINC_SRC	0x04
#define PLDZ002_DMABD_NOINC_DST	0x02
#define PLDZ002_DMABD_END		0x01

/* use these flags to feed PLDZ002_DMABD_SRC() or PLDZ002_DMABD_DST() */
#define PLDZ002_DMABD_DIR_SRAM	0x01		/**<	SRAM address space	*/
#define PLDZ002_DMABD_DIR_VME	0x02		/**<	VME address space	*/
#define PLDZ002_DMABD_DIR_PCI	0x04		/**<	PCI address space	*/

/* CTRL 0x30 */
#define PLDZ002_SLV16			0x0030		/**< Slave Control Register A16 (r/w)		*/
/* CTRL 0x34 */
#define PLDZ002_SLV32			0x0034		/**< Slave Control Register A32 (r/w)		*/
#define PLDZ002_SLVxx_EN		        0x00000010	/**< slave enable */
#define PLDZ002_SLV32_BASE_MASK_UPPER	0x0000000F	/**< A32 slave base upper mask */
#define PLDZ002_SLV32_BASE_MASK_LOWER	0x0000FF00	/**< A32 slave base lower mask */
#define PLDZ002_SLV32_SIZE_MASK			0x00FF0000  /**< A32 slave size mask */
#define PLDZ002_SLV32_SIZE_256MB		0x00000000  /**< A32 slave size 256 MB */
#define PLDZ002_SLV32_SIZE_128MB		0x00800000  /**< A32 slave size 128 MB */
#define PLDZ002_SLV32_SIZE_64MB			0x00C00000  /**< A32 slave size 64 MB */
#define PLDZ002_SLV32_SIZE_32MB			0x00E00000  /**< A32 slave size 32 MB */
#define PLDZ002_SLV32_SIZE_16MB			0x00F00000  /**< A32 slave size 16 MB */
#define PLDZ002_SLV32_SIZE_8MB			0x00F80000  /**< A32 slave size 8 MB */
#define PLDZ002_SLV32_SIZE_4MB			0x00FC0000  /**< A32 slave size 4 MB */
#define PLDZ002_SLV32_SIZE_2MB			0x00FE0000  /**< A32 slave size 2 MB */
#define PLDZ002_SLV32_SIZE_1MB			0x00FF0000  /**< A32 slave size 1 MB */

#define PLDZ002_SLV32_BASE_MASK_UPPER_SHIFT	28	/**< A32 base upper mask shift */
#define PLDZ002_SLV32_BASE_MASK_LOWER_SHIFT	12	/**< A32 base lower mask shift */


/* CTRL 0x38 */
#define PLDZ002_LM_STAT_CTRL_0		0x0038	/**< location monitor 0 status/ctrl  (r/w)		*/
/* CTRL 0x3C */
#define PLDZ002_LM_STAT_CTRL_1		0x003C	/**< location monitor 1 status/ctrl  (r/w)		*/

#define PLDZ002_LM_STAT_CTRL_ADDR_MSK	0xC0 /**< address 3 and 4 */
#define PLDZ002_LM_STAT_CTRL_WR			0x20 /**< trigger at write */
#define PLDZ002_LM_STAT_CTRL_RD			0x10 /**< trigger at read */
#define PLDZ002_LM_STAT_CTRL_IRQ		0x08 /**< IRQ status */
#define PLDZ002_LM_STAT_CTRL_A16_MODE  	0x04 /**< 10 - A16 */
#define PLDZ002_LM_STAT_CTRL_A24_MODE  	0x06 /**< 11 - A24 */
#define PLDZ002_LM_STAT_CTRL_A32_MODE  	0x00 /**< 00 - A32 */
#define PLDZ002_LM_STAT_CTRL_IRQ_EN		0x01 /**< IRQ enable */

/* CTRL 0x40 */
#define PLDZ002_LM_ADDR_0		0x0040		/**< location monitor 0 addr (r/w)		*/
/* CTRL 0x44 */
#define PLDZ002_LM_ADDR_1		0x0044		/**< location monitor 1 addr (r/w)		*/
/* LOC_AM_0 = 00: LOCADR_0 [31:10]
 * LOC_AM_0 = 10: LOCADR_0 [15:10]
 * LOC_AM_0 = 11: LOCADR_0 [23:10]
 */

/* CTRL 0x48 */
#define PLDZ002_SLV24_PCI		0x0048		/**< Slave Control Register A24 to PCI (r/w)		*/
/* CTRL 0x4c */
#define PLDZ002_SLV32_PCI		0x004c		/**< Slave Control Register A32 to PCI (r/w)		*/

#if PLDZ002_REV < 17
/* offset relative to internal RAM! */
# define PLDZ002_DMABD_OFF(bd)	PLDZ002_DMABD_OFF_RV9(bd)
# define PLDZ002_MAILBOX(mbx) 	PLDZ002_MAILBOX_RV9(mbx)
#else
/* offset relative to SRAM start! */
# define PLDZ002_DMABD_OFF(bd)	(PLDZ002_RV11_SRAMREGS + PLDZ002_DMABD_OFF_RV11(bd))
# define PLDZ002_MAILBOX(mbx) 	(PLDZ002_RV11_SRAMREGS + PLDZ002_MAILBOX_RV11(mbx))
#endif

#  ifdef __cplusplus
       }
#  endif

#endif /* _PLDZ002_H */








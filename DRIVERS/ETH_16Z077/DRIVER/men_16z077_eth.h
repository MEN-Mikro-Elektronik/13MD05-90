/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  men_16z077_eth.h
 *
 *      \author  thomas.schnuerer@men.de
 *        $Date: 2013/07/10 18:34:24 $
 *    $Revision: 1.16 $
 *
 *        \brief Declarations for Z077/087 Ethernet IP cores
 *
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: men_16z077_eth.h,v $
 * Revision 1.16  2013/07/10 18:34:24  ts
 * R: PACKLEN setting made problem in customer application because register
 *    value is interpreted without FCS
 * M: decreased min/max value by 4 (FCS added by IP core)
 *
 * Revision 1.15  2012/11/06 20:18:27  ts
 * R: MAC addrs for x86 boards were not generated according to ident EEprom
 * M: added defines to check on x86 CPUs (F11S, P51x PMCs) for ident EEPROMs
 *
 * Revision 1.14  2012/10/01 11:46:46  ts
 * R: Board specific MAC addr offsets for certain CPUs were not considered yet
 * M: added defines for MAC offsets on F11S and F218
 *
 * Revision 1.13  2012/09/20 18:34:43  ts
 * R: mdiobus functions cause problems with MEN pseudo Phy inside F218 FPGA
 * M: made registration on mdio bus dependent on switch CONFIG_MENEP05
 *
 * Revision 1.12  2012/09/07 18:04:26  ts
 * R: support for IEEE 802.1Q VLAN tagging requested by customer
 * M: 1. added defines for VLAN processing depending on kernel version
 *    2. updated driver documentation
 *
 * Revision 1.11  2012/03/28 18:06:53  ts
 * R: support for phy driver platform was requested
 * M: added functionality for phy_connect and other phy platform functions
 *
 * Revision 1.10  2011/02/23 14:52:54  rt
 * R: 1) Some bit descriptions missing.
 *    2) Cosmetics.
 *    3) Z077_WEIGHT wrong.
 * M: 1) Added REG_COREREV_* defines.
 *    2) Removed duplicated defines.
 *    3) Changed to 0x40.
 *
 * Revision 1.9  2010/12/17 14:29:43  ts
 * R: IP core was enhanced with 4 error storing Registers
 * M: added defines for CORE_CNT0-3 and Rev. Register
 *
 * Revision 1.8  2010/06/04 12:40:18  ts
 * R: MM1 needs MAC address read out from EEPROM
 * M: added defines for SMB bit bang routines
 *
 * Revision 1.6  2009/09/17 15:08:31  ts
 * R: Cosmetics, removed many unnecessary defines left over from early development
 *
 * Revision 1.5  2009/06/09 14:42:32  ts
 * R: new register 0x70 for Rx counter introduced in Z87
 * M: added register RXBDSTAT
 *
 * Revision 1.4  2008/07/14 11:14:05  aw
 * R: Z087 bdBase was not aligned
 * M: use define Z077_BDALIGN
 *
 * Revision 1.3  2008/03/12 10:18:21  aw
 * supports NIOS_II in combination with Z087
 *
 * Revision 1.2  2008/02/14 13:30:04  ts
 * added description of bit OETH_MODER_BRO to reject broadcasts if bit=0
 *
 * Revision 1.1  2007/11/16 15:55:18  ts
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2012 by MEN mikro elektronik GmbH, Nuremberg, Germany
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 ****************************************************************************/

#ifndef _MEN_Z77_ETH_
#define _MEN_Z77_ETH_

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>

/* for EP05 support of mdio_bus facility was requested */
#if defined(CONFIG_MENEP05)
#  include <linux/phy.h>
#  include <linux/mii.h>
#endif

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#define Z77_USE_VLAN_TAGGING
#endif

#define MEN_Z77_DRV_NAME  "MEN ETH: "

/**
 * @defgroup _Z077MACS board specific MAC offsets 
 * These defines specify CPU specific MAC base addresses. After the MEN OUI
 * 00:c0:3a the MACs contain a CPU specific byte and the serial number of the
 * card.
 */
/*@{*/
#define Z77_MAC_OFFS_F11S		0x9ec000	/*!< F11S FPGA MAC: 00 c0 3a 9e cX XX */
#define Z77_MAC_OFFS_F218		0x027000	/*!< F218 FPGA MAC: 00 c0 3a 02 7X XX */
#define Z77_MAC_OFFS_P511		0x9cc000	/*!< P511 FPGA MAC: 00 c0 3a 0c cX XX */
#define Z77_MAC_P511_IF2OFF		0x1000		/*!< P511 offset 2. IF */
#define Z77_MAC_OFFS_P513		0xae2000	/*!< P513 FPGA MAC: 00 c0 3a ae 2X XX */
#define Z77_MAC_P513_IF2OFF		0x1000		/*!< P513 offset 2. IF */
#define MEN_F11S_SERNUM_MAX		16383
#define MEN_F11S_SERNUM_BASE 	0xc000
#define MEN_F11S				11
#define MEN_F218				218
#define MEN_P511				511
#define MEN_P513				513
/*@}*/

/**
 * @defgroup _Z077BASES Important basic defines for following Macros
 * These defines are used in all further Macros.
 * The base Address (IP core physical start) is retrieved from 
 * the found FPGA IP core 16Z077/Z087 through the men_chameleon driver.
 */
/*@{*/
#define Z077_BASE ((void*)dev->base_addr) /*!< baseaddrof IP core */
#define Z077_BDBASE			(np->bdBase)  /*!< BD base address in RAM (Z087) */
#define Z077_BD_OFFS		(np->bdOff )  /*!< BD start (phys+0x400 or RAM) */
#define TBD_OFF				(np->tbdOff ) /*!< TxBD offset to IP core start */
#define RBD_OFF  			(np->rbdOff ) /*!< RxBD offset to IP core start */
#define Z77_ETHBUF_SIZE		0x1000		  /*!< size of 1 BD, PAGESZ aligned */
#define Z77_CFGREG_SIZE		0x800		  /*!< all from MODER to last Rx BD */
#define MY_TX_TIMEOUT  		(5*HZ)  	  /*!< 5 seconds TX wait timeout */
#define Z077_REG_BASE		(0)	 		  /*!< Register base offset */
#define Z77_PACKLEN_DEFAULT 0x003c05fc	  /*!< packet length defaults (without FCS) */
/*@}*/

/**
 * @defgroup _Z077_REGISTERS 16Z077/087 Registers
 *
 *@{*/
#define Z077_REG_MODER		(Z077_REG_BASE+0x00) /*!< Mode control */
#define Z077_REG_INT_SRC	(Z077_REG_BASE+0x04) /*!< IRQ source */
#define Z077_REG_INT_MASK	(Z077_REG_BASE+0x08) /*!< IRQ Masking 	*/
#define Z077_REG_IPGT		(Z077_REG_BASE+0x0c) /*!< InterPacket Gap */
#define Z077_REG_IPGR1		(Z077_REG_BASE+0x10) /*!< No InterPacket Gap	*/
#define Z077_REG_IPGR2		(Z077_REG_BASE+0x14) /*!< No InterPacket Gap2	*/
#define Z077_REG_PACKLEN	(Z077_REG_BASE+0x18) /*!< Packet Len (min/max)*/
#define Z077_REG_COLLCONF	(Z077_REG_BASE+0x1c) /*!< Collision/Retry Conf. */
#define Z077_REG_TX_BDNUM	(Z077_REG_BASE+0x20) /*!< Number of TX BDs */
#define Z077_REG_CTRLMODER	(Z077_REG_BASE+0x24) /*!< Control Module Mode */
#define Z077_REG_MIIMODER	(Z077_REG_BASE+0x28) /*!< MII Mode Register	*/
#define Z077_REG_MIICMD		(Z077_REG_BASE+0x2c) /*!< MII command	*/
#define Z077_REG_MIIADR		(Z077_REG_BASE+0x30) /*!< MII Address 	*/
#define Z077_REG_MIITX_DATA (Z077_REG_BASE+0x34) /*!< MII Transmit Data	*/
#define Z077_REG_MIIRX_DATA (Z077_REG_BASE+0x38) /*!< MII Receive Data	*/
#define Z077_REG_MIISTATUS  (Z077_REG_BASE+0x3c) /*!< MII Status 		*/
#define Z077_REG_MAC_ADDR0 	(Z077_REG_BASE+0x40) /*!< MAC Addr[0-3]	*/
#define Z077_REG_MAC_ADDR1  (Z077_REG_BASE+0x44) /*!< MAC Addr[4-5]	*/
#define Z077_REG_HASH_ADDR0 (Z077_REG_BASE+0x48) /*!< Hash Reg 0 */
#define Z077_REG_HASH_ADDR1 (Z077_REG_BASE+0x4c) /*!< Hash Reg 1	*/
#define Z077_REG_TXCTRL    	(Z077_REG_BASE+0x50) /*!< TX control  */
/*@}*/

/**
 * @defgroup _Z087_NEWREGS 16Z087 (MEN) specific Registers
 */
/*@{*/
#define Z077_REG_GLOBALRST 	(Z077_REG_BASE+0x54) /*!< global MAC core reset   */
#define Z077_REG_BDSTART 	(Z077_REG_BASE+0x5c) /*!< Startaddr of TxRx BDs	  */
#define Z077_REG_RXEMPTY0 	(Z077_REG_BASE+0x60) /*!< RxBD[31:0] empty Flag	  */
#define Z077_REG_RXEMPTY1 	(Z077_REG_BASE+0x64) /*!< RxBD[63:32]empty Flag	  */
#define Z077_REG_TXEMPTY0 	(Z077_REG_BASE+0x68) /*!< TxBD[31:0] empty Flag	  */
#define Z077_REG_TXEMPTY1 	(Z077_REG_BASE+0x6c) /*!< TxBD[63:32]empty Flag	  */
#define Z077_REG_RXBDSTAT 	(Z077_REG_BASE+0x70) /*!< RxBD count and status	  */
#define Z077_REG_SMBCTRL 	(Z077_REG_BASE+0x74) /*!< SMBus bitbang interface */

#define Z077_REG_COREREV 	(Z077_REG_BASE+0x78) /*!< core revision register */
#define Z077_REG_RXERRCNT1	(Z077_REG_BASE+0x7c) /*!< Symbol/CRC errors count */
#define Z077_REG_RXERRCNT2	(Z077_REG_BASE+0x80) /*!< # of rcvd Frames/busy Err count */
#define Z077_REG_TXERRCNT1	(Z077_REG_BASE+0x84) /*!< FIFO underrun/Late coll. count */
#define Z077_REG_TXERRCNT2	(Z077_REG_BASE+0x88) /*!< # of sent frames/TxBD full count */

#define SMB_REG_SCL			0x1
#define SMB_REG_SDA			0x2
/*@}*/

/**
 * @defgroup _MACRO1 Macros acting on the Rx/Tx Descriptors
 * \verbatim
   Macros to manipulate Rx- and Tx BD Entries

   These provide HW-Independent ways to set/get Address entries and Flag
   settings. Detailled Explanation for Macro construction below:

   original ETH OpenCore:  BDs start at Z077_BD_OFFS (0x400) after ChamBase.

   Z077_BDBASE  = ChamBase + 0x400
   Z077_BD_OFFS = 0x400
   +--------------+
   |              |
   |--------------|- Rx BD Start
   |              |
   |--------------|- TX Bd  Start (= Z077_BDBASE + Z077_BD_OFFS )
   |XXXXXXXXXXXXXX|  Ctrl Regs
   +--------------+- Chameleon Base Address

   New Core: BDs reside in RAM, Address must be 10bit-aligned

   Z077_BDBASE  = pDrvCtrl->bdBase;
   Z077_BD_OFFS = 0

   +--------------+
   |              |
   |--------------|- Tx BD Start
   |              |
   +--------------+- Rx Bd  Start (= Z077_BDBASE + Z077_BD_OFFS )

   \endverbatim
 */
/*@{*/
#define Z077_SET_TBD_ADDR(n,adr) Z77WRITE_D32(Z077_BDBASE,  Z077_BD_OFFS+(((n)+TBD_OFF) * Z077_BDSIZE) + Z077_BD_OFFS_ADR, (adr))
/*!< set Tx BD Address for Tx BD nr. (n) */

#define Z077_SET_RBD_ADDR(n,adr) Z77WRITE_D32(Z077_BDBASE,  Z077_BD_OFFS+( ((n)+RBD_OFF) * Z077_BDSIZE)+ Z077_BD_OFFS_ADR, adr)
/*!< set Rx BD Address for Rx BD nr. (n) */

#define Z077_GET_RBD_ADDR(n) 	 Z77READ_D32(Z077_BDBASE, Z077_BD_OFFS + ( ((n)+RBD_OFF) * Z077_BDSIZE) + Z077_BD_OFFS_ADR)
/*!< get Rx BD Address for Rx BD nr. (n) */

#define Z077_GET_TBD_ADDR(n)  	 Z77READ_D32(Z077_BDBASE, Z077_BD_OFFS + ( ((n)+TBD_OFF) * Z077_BDSIZE)+Z077_BD_OFFS_ADR)
/*!< get Tx BD Address of ETH Frame of Tx BD nr. (n) */

#define Z077_SET_TBD_FLAG(n,f)   Z77WRITE_D16(Z077_BDBASE, Z077_BD_OFFS+( ((n)+TBD_OFF)*Z077_BDSIZE), \
											  Z77READ_D16( Z077_BDBASE, Z077_BD_OFFS+( ((n)+TBD_OFF)*Z077_BDSIZE))| (f) )
/*!< set flag f of Tx BD nr. (n) */
#define Z077_SET_RBD_FLAG(n,f)   Z77WRITE_D16(Z077_BDBASE, Z077_BD_OFFS + ((n)+RBD_OFF) * Z077_BDSIZE,\
											  Z77READ_D16( Z077_BDBASE, Z077_BD_OFFS + ((n)+RBD_OFF) * Z077_BDSIZE)| (f) )
/*!< set flag f of Rx BD nr. (n) */
#define Z077_CLR_TBD_FLAG(n,f)   Z77WRITE_D16(Z077_BDBASE, Z077_BD_OFFS + (((n)+TBD_OFF) * Z077_BDSIZE),\
											  Z77READ_D16( Z077_BDBASE, Z077_BD_OFFS + (((n)+TBD_OFF) * Z077_BDSIZE)) &	~(f) )
/*!< clear flag f of Tx BD nr. (n) */

#define Z077_CLR_RBD_FLAG(n,f)   Z77WRITE_D16(Z077_BDBASE, Z077_BD_OFFS + ((n)+RBD_OFF) * Z077_BDSIZE,\
											  Z77READ_D16( Z077_BDBASE, Z077_BD_OFFS + ((n)+RBD_OFF) * Z077_BDSIZE) & ~(f) )
/*!< clear flag f of Rx BD nr. (n) */

#define Z077_GET_RBD_FLAG(n,f)  (Z77READ_D16(Z077_BDBASE, Z077_BD_OFFS+( ((n)+RBD_OFF)*Z077_BDSIZE)) & (f) )
/*!< get flag f of Rx BD nr. (n) */

#define Z077_GET_TBD_FLAG(n,f)  (Z77READ_D16(Z077_BDBASE, Z077_BD_OFFS+( ((n)+TBD_OFF)*Z077_BDSIZE)) & (f) )
/*!< get TBD Flag f of Tx BD nr. (n) */

#define Z077_SET_TBD_LEN(n,l) Z77WRITE_D16(Z077_BDBASE, Z077_BD_OFFS + Z077_BD_OFFS_LEN+( ((n)+TBD_OFF) * Z077_BDSIZE), (l));
/*!< set ETH Frame length for Tx BD nr. (n) */

#define Z077_GET_TBD_LEN(n) Z77READ_D16(Z077_BDBASE,    Z077_BD_OFFS + Z077_BD_OFFS_LEN+( ((n)+TBD_OFF) * Z077_BDSIZE))
/*!< Get ETH Frame length for Tx BD nr. (n) */

#define Z077_GET_RBD_LEN(n)  Z77READ_D16(Z077_BDBASE,   Z077_BD_OFFS+( ((n)+RBD_OFF) * Z077_BDSIZE) + Z077_BD_OFFS_LEN )
/*!< retrieve the length stored in Rx BD nr. (n) */

#define Z077_CLR_RBD_LEN(n)  Z77WRITE_D16(Z077_BDBASE,  Z077_BD_OFFS+( ((n)+RBD_OFF) * Z077_BDSIZE)+Z077_BD_OFFS_LEN, 0 );
/*!< clear a Rx BDs len entry after packet is processed */

#define Z077_GET_RBD_ADDR(n) Z77READ_D32(Z077_BDBASE, Z077_BD_OFFS + ( ((n)+RBD_OFF) * Z077_BDSIZE) + Z077_BD_OFFS_ADR)
/*!< get the address of ETH Frame of Rx BD (n) */

#define Z077_RBD_EMPTY(n) Z077_GET_RBD_FLAG((n), OETH_RX_BD_EMPTY)
/*!< return non-zero if the Rx BD (n) is empty */
/*@}*/

/**
 * @defgroup _Z077_REGBITS Register Bit definitions
 */
/*@{*/
#define OETH_TOTAL_BD       128   	/*!< total number of Buffer descriptors */
#define OETH_MAXBUF_LEN     0x610  	/*!< reserved length of an Eth frame */
/*@}*/


/**
 * @defgroup _ETHT1 message levels for ethtool usage
 */
/*@{*/
#define ETHT_MESSAGE_OFF	0 	/*!< all Messages off */
#define ETHT_MESSAGE_LVL1	1 	/*!< only main debug messages */
#define ETHT_MESSAGE_LVL2	2 	/*!< more verbose messages */
#define ETHT_MESSAGE_LVL3	3 	/*!< maximum verbosity including interrupts */
#define Z77_MAX_MSGLVL          ETHT_MESSAGE_LVL3    /* to be set to highest LVL */        
/*@}*/


/**
 * @defgroup _TXDB Tx Buffer Descriptor Bits
 */
/*@{*/
#define OETH_TX_BD_READY     0x8000   /*!< Tx BD Ready */
#define OETH_TX_BD_IRQ       0x4000   /*!< Tx BD IRQ Enable	*/
#define OETH_TX_BD_WRAP      0x2000   /*!< Tx BD Wrap (last BD)	*/
#define OETH_TX_BD_PAD       0x1000   /*!< Tx BD Pad Enable */
#define OETH_TX_BD_CRC       0x0800   /*!< Tx BD CRC Enable */
#define OETH_TX_BD_UNDERRUN  0x0100   /*!< Tx BD Underrun Status */
#define OETH_TX_BD_RETRY     0x00F0   /*!< Tx BD Retry Status */
#define OETH_TX_BD_RETLIM    0x0008   /*!< Tx BD Retransm Limit Stat */
#define OETH_TX_BD_LATECOL   0x0004   /*!< Tx BD Late Collision Stat */
#define OETH_TX_BD_DEFER     0x0002   /*!< Tx BD Defer Status */
#define OETH_TX_BD_CARRIER   0x0001   /*!< Tx BD Carrier Sense Lost Status */
/*@}*/


/**
 * @defgroup _RXDB Rx Buffer Descriptor Bits
 */
/*@{*/
#define OETH_RX_BD_EMPTY     0x8000	/*!< Rx BD Empty */
#define OETH_RX_BD_IRQ       0x4000	/*!< Rx BD IRQ Enable */
#define OETH_RX_BD_WRAP      0x2000	/*!< Rx BD Wrap (last BD) */
#define OETH_RX_BD_MISS      0x0080	/*!< Rx BD Miss Status 	*/
#define OETH_RX_BD_OVERRUN   0x0040	/*!< Rx BD Overrun Status */
#define OETH_RX_BD_INVSYMB   0x0020	/*!< Rx BD Invalid Symbol Status */
#define OETH_RX_BD_DRIBBLE   0x0010	/*!< Rx BD Dribble Nibble Status */
#define OETH_RX_BD_TOOLONG   0x0008	/*!< Rx BD Too Long Status */
#define OETH_RX_BD_SHORT     0x0004	/*!< Rx BD Too Short Frame Status */
#define OETH_RX_BD_CRCERR    0x0002	/*!< Rx BD CRC Error Status */
#define OETH_RX_BD_LATECOL   0x0001	/*!< Rx BD Late Collision Status */
/*@}*/


/**
 * @defgroup _RXST Rx status Register bits
 */
/*@{*/
#define REG_RXSTAT_REV		(1<<6)	/*!< Z87 Revision bit */
#define REG_RXSTAT_RXEMPTY	(1<<7)	/*!< RXEMPTY bit of current watched BD */
/*@}*/

/**
 * @defgroup _COREREV Core Revision Bits
 */
/*@{*/
#define REG_COREREV_REVISION	(1<<0)	/*!< 16z087- IP core revision */
#define REG_COREREV_IRQNEWEN	(1<<8)	/*!< Enable new interrupt behavior */
/*@}*/

/**
 * @defgroup _MODER MODER Register bits descripton
 */
/*@{*/
#define OETH_MODER_RXEN     (1<<0) 	/*!< Receive Enable 					*/
#define OETH_MODER_TXEN     (1<<1) 	/*!< Transmit Enable					*/
#define OETH_MODER_NOPRE    (1<<2) 	/*!< No Preamb.   		  	REV01:NA	*/
#define OETH_MODER_BRO      (1<<3) 	/*!< 0: Reject Broadcast				*/
#define OETH_MODER_IAM      (1<<4) 	/*!< Use Individual Hash				*/
#define OETH_MODER_PRO      (1<<5) 	/*!< Promisc Mode 						*/
#define OETH_MODER_IFG      (1<<6) 	/*!< Min. IFG not required  REV01:NA 	*/
#define OETH_MODER_LOOPBCK  (1<<7) 	/*!< Loop Back 		  	  	REV01:NA	*/
#define OETH_MODER_NOBCKOF  (1<<8) 	/*!< No Backoff 		  	REV01:NA	*/
#define OETH_MODER_EXDFREN  (1<<9) 	/*!< Excess Defer 		  	REV01:NA	*/
#define OETH_MODER_FULLD    (1<<10) /*!< Full Duplex 						*/
#define OETH_MODER_RST      (1<<11) /*!< Reset MAC 		  	  	REV01:NA 	*/
#define OETH_MODER_DLYCRCEN (1<<12) /*!< Delayed CRC Enable  	REV01:NA 	*/
#define OETH_MODER_CRCEN    (1<<13) /*!< CRC Enable 						*/
#define OETH_MODER_HUGEN    (1<<14) /*!< Huge Enable 		  	REV01:NA	*/
#define OETH_MODER_PAD      (1<<15) /*!< Pad Enable 						*/
#define OETH_MODER_RECSMALL (1<<16) /*!< Receive Small 	  	  	REV01:NA	*/
#define OETH_MODER_RX_IDLE  (1<<17) /*!< Rx idle after reset  	REV01:NA	*/
#define OETH_MODER_TX_IDLE  (1<<18) /*!< Rx idle after reset  	REV01:NA	*/
#define OETH_MODER_HD_AVAL   (1<<19) /*!< Half duplex available              */
#define OETH_MODER_NIOSSHIFT (1<<27) /*!< Ethernetframe shifted at 2 Bytes	 */
#define OETH_MODER_COMPLI    (5<<29) /*!< activate compliance mode  */

/*@}*/


/**
 * @defgroup _INTS Interrupt Source Register bits descripton
 */
/*@{*/
#define OETH_INT_TXB      	0x0001 /*!< Transmit Buffer IRQ	*/
#define OETH_INT_TXE      	0x0002 /*!< Transmit Error IRQ 	*/
#define OETH_INT_RXF      	0x0004 /*!< Receive Frame IRQ 	*/
#define OETH_INT_RXE      	0x0008 /*!< Receive Error IRQ 	*/
#define OETH_INT_BUSY     	0x0010 /*!< Busy IRQ */
#define OETH_INT_TXC      	0x0020 /*!< Tx Control Frame IRQ */
#define OETH_INT_RXC      	0x0040 /*!< Rx Control Frame IRQ */
/*@}*/

/**
 * @defgroup _INTM Interrupt Mask Register bits descripton
 */
/*@{*/
#define OETH_INT_MASK_TXB       0x0001 /*!< Transmit Buffer IRQ	*/
#define OETH_INT_MASK_TXE       0x0002 /*!< Transmit Error IRQ 	*/
#define OETH_INT_MASK_RXF       0x0004 /*!< Receive Frame IRQ  */
#define OETH_INT_MASK_RXE       0x0008 /*!< Receive Error IRQ  */
#define OETH_INT_MASK_BUSY      0x0010 /*!< Busy IRQ  	*/
#define OETH_INT_MASK_TXC       0x0020 /*!< Tx Control Frame IRQ	*/
#define OETH_INT_MASK_RXC   	0x0040 /*!< Rx Control Frame IRQ */
/*@}*/

/**
 * @defgroup _CTRLMODER Control Module Mode Register
 */
/*@{*/
#define OETH_CTRLMODER_PASSALL  0x0001 /*!< Pass Control Frames */
#define OETH_CTRLMODER_RXFLOW   0x0002 /*!< Rx Control Flow Enable 	*/
#define OETH_CTRLMODER_TXFLOW   0x0004 /*!< Tx Control Flow Enable 	*/
/*@}*/

/**
 * @defgroup _MODERMII MII Mode Register
 */
/*@{*/
#define OETH_MIIMODER_CLKDIV    0x00FF /*!< Clock Divider 	*/
#define OETH_MIIMODER_NOPRE     0x0100 /*!< No Preamble 	*/
#define OETH_MIIMODER_RST       0x0200 /*!< MIIM Reset 	*/
/*@}*/

/**
 * @defgroup _CMDR MII Command Register
 */
/*@{*/
#define OETH_MIICMD_SCANSTAT  	0x0001 /*!< Scan Status		*/
#define OETH_MIICMD_RSTAT     	0x0002 /*!< Read Status	*/
#define OETH_MIICMD_WCTRLDATA 	0x0004 /*!< Write Control Data	*/
/*@}*/

/**
 * @defgroup _MIIA MII Address Register
 */
/*@{*/
#define OETH_MIIADDRESS_FIAD    0x001F /*!< PHY Address	*/
#define OETH_MIIADDRESS_RGAD    0x1F00 /*!< RGAD Address */
/*@}*/

/**
 * @defgroup _MIISR MII Status Register
 */
/*@{*/
#define OETH_MIISTATUS_LINKFAIL 0x0001 /*!< Link Fail */
#define OETH_MIISTATUS_BUSY     0x0002 /*!< MII Busy */
#define OETH_MIISTATUS_NVALID   0x0004 /*!< MIIStatusReg data invalid	*/
/*@}*/

/**
 * @defgroup _MISCR Misc defines
 * The current default KS8721 PHY address (on EM01)
 */
/*@{*/
#define Z077_PHY_ADDRESS 		1
#define MII_100_BASE_TX_PHY		0x1f
#define MII_100BASE_PWR			(1<<10)
#define Z077_WEIGHT             0x40 /* shall be equal to ETH frame storage capacity */
/*@}*/

/**
 * @defgroup _BDDEF Buffer Descriptor defines
 * Buffer Descriptor defines. ETH MAC can maintain up to 128 BDs in total.
 * (0x80) BD location = base + 0x400 - base + 0x7ff ( =0x400 )
 */
/*@{*/
#define Z077_BDSIZE		0x8		 /*!< size in byte of one BD */
#define Z077_TBD_NUM	0x40	 /*!< default number of TBDs */
#define Z077_RBD_NUM	0x40  	 /*!< Z077_TBD_NUM make both equal	*/
#define Z077_BDALIGN    (1<<10)  /*!< 10bit adress alignment in RAM */
/*@}*/

/**
 * @defgroup _BDAREA address defines within one Buffer descriptor
 */
/*@{*/
#define Z077_BD_AREA_SIZE ((Z077_TBD_NUM+Z077_RBD_NUM)*Z077_BDSIZE)
#define Z077_BD_OFFS_STAT	0x0	 /*!< offset of status within  BD 	*/
#define Z077_BD_OFFS_LEN	0x2	 /*!< offset of length within  BD 	*/
#define Z077_BD_OFFS_ADR	0x4	 /*!< offset of Tx/Rx frame within this BD */
/*@}*/

/**
 * @defgroup _TXBDBITS Transmit Buffer Descriptor bit definitions
 */
/*@{*/
#define Z077_TBD_RDY     0x8000	/*!< Tx buffer ready: 1: ready for TX */
#define Z077_TBD_IRQ	 0x4000	/*!< IRQ enable 1: generate. IRQ after send */
#define Z077_TBD_WRAP    0x2000	/*!< Wrap indicate 1: last BD in table */
#define Z077_TBD_PAD     0x1000	/*!< pad enable 1: pad short frames */
#define Z077_TBD_CRC     0x0800 /*!< CRC enable 1: add CRC at packet end */
#define Z077_TBD_UR      0x0100 /*!< underrun 1: underrun while send */
#define Z077_TBD_RETRY   0x00f0	/*!< [7:4]: retry # till frame was sent */
#define Z077_TBD_RTRANS	 0x0008	/*!< retransm.limit reached(collisions etc) */
#define Z077_TBD_LCOLL	 0x0004 /*!< 1: late collision occured */
#define Z077_TBD_DEFER	 0x0002 /*!< Deferred indication */
#define Z077_TBD_CSLOST	 0x0001 /*!< Carrier sense lost while transmit */
/*@}*/

/**
 * @defgroup _RXBDBITS Receive Buffer Descriptors bit definitions
 */
/*@{*/
#define Z077_RBD_EMP    0x8000	/*!< Rx buffer empty */
#define Z077_RBD_IRQ	0x4000	/*!< IRQ enable, generate IRQ after recv */
#define Z077_RBD_WRAP   Z077_TBD_WRAP
#define Z077_RBD_CFRM   0x0100 /*!< 1: received Control Frame 0: normal frame */
#define Z077_RBD_MISS   0x0080 /*!< 1: frame accepted due to promisc mode */
#define Z077_RBD_OV		0x0040 /*!< overrun occured 	*/
#define Z077_RBD_IS		0x0020 /*!< invalid symbol (not in 5B4B table) */
#define Z077_RBD_DN	 	0x0010 /*!< dribble nibble (extra nibble received) */
#define Z077_RBD_TL	 	0x0008 /*!< received Frame longer then PACKETLEN */
#define Z077_RBD_SF	 	0x0004 /*!< short Frame,shorter then MINFL 	*/
#define Z077_RBD_CERR 	0x0002 /*!< CRC error occured */
#define Z077_RBD_LC 	0x0001 /*!< late collision */
/*@}*/


/* Cache macros */
#define Z77WRITE_D32(ma,offs,val) 		writel(val, (void*)(ma+offs))
#define Z77WRITE_D16(ma,offs,val) 		writew(val, (void*)(ma+offs))
#define Z77READ_D32(ma,offs) 	 		readl((void*)(ma+offs))
#define Z77READ_D16(ma,offs) 	 		readw((void*)(ma+offs))

/**
 * @defgroup _MACRO0 Macros acting on Registers within the IP Core
 */
/*@{*/
#define Z077_DISABLE_IRQ(n)	\
        Z77WRITE_D32(Z077_BASE, Z077_REG_INT_MASK, \
         (Z77READ_D32(Z077_BASE,Z077_REG_INT_MASK)) & ~(n) )
/*!< disable IRQ source n in Interrupt Mask Register */

#define Z077_ENABLE_IRQ(n) \
        Z77WRITE_D32(Z077_BASE, Z077_REG_INT_MASK, \
            Z77READ_D32(Z077_BASE, Z077_REG_INT_MASK)|(n))
/*!< enable IRQ source n in Interrupt Mask Register */

#define Z077_CLR_MODE_FLAG(f) \
		Z77WRITE_D32(Z077_BASE, Z077_REG_MODER, \
        Z77READ_D32(Z077_BASE, Z077_REG_MODER) &~(f))
/*!< clear bit f in MODE Register */

#define Z077_SET_MODE_FLAG(f) \
		Z77WRITE_D32(Z077_BASE, Z077_REG_MODER, \
        Z77READ_D32(Z077_BASE, Z077_REG_MODER)|(f))
/*!< set bit f in MODE Register */
/*@}*/


/**
 * helper structs for more comfortable dumping of Registers\n
 * used to maintain info about the current PHY settings
 */
 /*@{*/
typedef struct z077_reg_info
{
	const char 	*name; 	/*!< clear Register name */
	unsigned int addr;	/*!< offset of the Register	*/
} Z077_REG_INFO;
/*@}*/

/**
 * helper struct to keep info about MII Phy
 */
 /*@{*/
typedef struct phy_device_tbl
{
	unsigned int ident; /*!< primary key, equal to ID in MII Reg */
	const char  *name; 	/*!< clear name string */
} PHY_DEVICE_TBL;
/*@}*/


/**
 * Buffer Descriptors, located between z77_base+0x400 and z77_base+0x800
 */
 /*@{*/
typedef struct z077_bd {
    unsigned short 	BdLen;		/*!< # of bytes to transmit/receive			 */
    unsigned short 	BdStat;		/*!< the 16bit Statusword 					 */
    void * 			BdAddr;		/*!< pointer to buffer (cpu addr) 			 */
	dma_addr_t		hdlDma;		/*!< return handle from pci_alloc_consistent */
#if defined(Z77_USE_VLAN_TAGGING)
	u32				vlan_tag;	/*!< if VLANs used: VLAN tag of this frame 	 */
#define RXD_LEN_MASK	0x0000ffff
#define RXD_LEN_SHIFT	0

	u32				type_flags;
#define RXD_FLAG_VLAN			0x0040
	u32				err_vlan;

#define RXD_VLAN_MASK			0x0000ffff

#endif

} Z077_BD;
/*@}*/
#endif /* #ifndef _MEN_Z77_ETH_ */

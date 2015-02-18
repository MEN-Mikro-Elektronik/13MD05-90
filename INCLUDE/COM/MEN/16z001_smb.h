/***********************  I n c l u d e  -  F i l e  ***********************/
/**
 *         \file 16z001_smb.h
 *
 *       \author cs
 *        $Date: 2009/07/28 14:32:43 $
 *    $Revision: 3.5 $
 *
 *        \brief Header file for MEN 16Z001_SMB FPGA module
 *
 *     Switches: none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: 16z001_smb.h,v $
 * Revision 3.5  2009/07/28 14:32:43  sy
 * add support to 64-bit aligned register address mapping
 *
 * Revision 3.4  2008/06/30 18:09:29  CSchuster
 * R: defines of BYPASS register bits where missing
 * M: added defines for BYPASS reister bits
 *
 * Revision 3.3  2007/01/05 17:28:45  cs
 * added:
 *   + define for I2C command
 *   + define of STS_INUSE bit
 * fixed:
 *   - don't include STS_INUSE in STS_CLEAR
 *
 * Revision 3.2  2006/08/18 08:54:11  rla
 * Cosmetic
 *
 * Revision 3.1  2005/11/25 12:06:03  cschuster
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#ifndef _Z001_SMB_H
#define _Z001_SMB_H

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/********************************
 *     Registers
 *******************************/

#ifndef _Z001_ADDR_DWORD_ALINGED_ 
/** \name registers  */
# define Z001_SMB_HSI		0x00	/**< Host Interface Config 				*/
# define Z001_SMB_HCBC		0x02	/**< Host Controller Base Clock Setting */
# define Z001_SMB_STS		0x04	/**< Host Status Register 				*/
# define Z001_SMB_CMD		0x05	/**< Host Command 						*/
# define Z001_SMB_START_PRT	0x06	/**< Start Register, wr starts cycle	*/
# define Z001_SMB_ADDR		0x07	/**< SMbus Address Register				*/
# define Z001_SMB_DATA_A		0x08	/**< SMBus Data A 						*/
# define Z001_SMB_DATA_B		0x09	/**< SMBus Data B 						*/
# define Z001_SMB_BLK		0x0A	/**< SMbus Block Register	 			*/
# define Z001_SMB_HSTCOM		0x0B	/**< SMbus Command Register for Host ctrl */
# define Z001_SMB_BYP		0x0C	/**< SMbus Controller Bypass Register 	*/
#else
# define Z001_SMB_HSI		0x00	/**< Host Interface Config 				*/
# define Z001_SMB_HCBC	0x08	/**< Host Controller Base Clock Setting */
# define Z001_SMB_STS		0x10	/**< Host Status Register 				*/
# define Z001_SMB_CMD		0x14	/**< Host Command 						*/
# define Z001_SMB_START_PRT	0x18	/**< Start Register, wr starts cycle	*/
# define Z001_SMB_ADDR		0x1C	/**< SMbus Address Register				*/
# define Z001_SMB_DATA_A		0x20	/**< SMBus Data A 						*/
# define Z001_SMB_DATA_B		0x24	/**< SMBus Data B 						*/
# define Z001_SMB_BLK		0x28	/**< SMbus Block Register	 			*/
# define Z001_SMB_HSTCOM	0x2C	/**< SMbus Command Register for Host ctrl */
# define Z001_SMB_BYP		0x30	/**< SMbus Controller Bypass Register 	*/
#endif
/********************************
 *     Register Settings
 *******************************/
/** \name register bits  */
#define Z001_SMB_HCBC_BASECLK_149KHZ	(0x0<<5) /**< I2C clk 149kHz */
#define Z001_SMB_HCBC_BASECLK_74KHZ		(0x1<<5) /**< I2C clk  74kHz */
#define Z001_SMB_HCBC_BASECLK_37KHZ		(0x2<<5) /**< I2C clk  37kHz */
#define Z001_SMB_HCBC_BASECLK_223KHZ	(0x4<<5) /**< I2C clk 223kHz */
#define Z001_SMB_HCBC_BASECLK_111KHZ	(0x5<<5) /**< I2C clk 111kHz */
#define Z001_SMB_HCBC_BASECLK_55KHZ		(0x6<<5) /**< I2C clk  55kHz */

#define Z001_SMB_HCBC_IDLE_BASECLK64	(0x0<<3) /**< IDLE delay BaseClk*64  */
#define Z001_SMB_HCBC_IDLE_BASECLK32	(0x1<<3) /**< IDLE delay BaseClk*32  */
#define Z001_SMB_HCBC_IDLE_BASECLK128	(0x2<<3) /**< IDLE delay BaseClk*128 */

/* FPGA16Z001 command constants */
#define Z001_SMB_CMD_I2C_STPDIS 0x02		/**< I2C send STOP disable */
#define Z001_SMB_CMD_ABORT      0x04		/**< ABORT transaction */
#define Z001_SMB_CMD_TOUT       0x08		/**< force TIMEOUT  */
#define Z001_SMB_CMD_QUICK      (0x0 << 4)	/**< QUICK timing */
#define Z001_SMB_CMD_BYTE       (0x1 << 4)	/**< Send/Receive Byte */
#define Z001_SMB_CMD_BYTE_DATA  (0x2 << 4)	/**< Read/Write Byte */
#define Z001_SMB_CMD_WORD_DATA  (0x3 << 4)	/**< Read/Write Word */
#define Z001_SMB_CMD_BLOCK_DATA (0x4 << 4)	/**< Read/Write Block */
#define Z001_SMB_CMD_I2C        (0x5 << 4)	/**< Read/Write I2C message */
#define Z001_SMB_CMD_BLOCK_CLR  0x80		/**< reset block registers pointer*/

/* FPGA16Z001 status register bits */
#define Z001_SMB_STS_CLEAR	0xFE

#define Z001_SMB_STS_INUSE	0x01	/**< INUSE_STS */
									/*!< 1: Host controller is not available \n
									 *   0: Host controller is available */
#define Z001_SMB_STS_IDLE	0x04	/**< IDL_STS */
									/*!< 1: Bus is idle \n
									 *   0: Bus is free */
#define Z001_SMB_STS_BUSY	0x08	/**< HST_BSY */
									/*!< 1: Host controller is completing
									 *      a command \n
									 *   0: Bus is ready */
#define Z001_SMB_STS_DONE	0x10	/**< SMI_I_STS */
									/*!< 1: the host controllers interrupt was
									 *      caused after completing a command */
#define Z001_SMB_STS_DEV	0x20	/**< DEVICE_ERR */
									/*!< 1: the host controllers interrupt was
									 *      caused by the controller or the device
									 *      due to the generation of an error */
#define Z001_SMB_STS_COLL	0x40	/**< BUS_COLLI */
									/*!< 1: the host controllers interrupt was
									 *      caused by the collision of bus
									 *      transactions or no acknowledge */
#define Z001_SMB_STS_TERM	0x80	/**< Terminate */
									/*!< 1: the host controllers interrupt was
									 *      caused by a terminated bus transaction
									 *      in response to "ABORT" */
#define Z001_SMB_STS_ERR	0xE0	/**< all error bits */

/* Host Interface Configuration Bits */
#define Z001_SMB_HSI_HCIE	0x01	/**< Host Controller Interface Enable */
#define Z001_SMB_HSI_IE		0x04	/**< Interrupt Enable */

/* SMB Controller Bypass Register */
#define Z001_SMB_BYP_CLK_EX	 (0x10) /**< I2C clk signal state  (R) */
#define Z001_SMB_BYP_CLK	 (0x08) /**< I2C clk signal ctrl   (W) */
#define Z001_SMB_BYP_DATA_EX (0x04)	/**< I2C data signal state (R) */
#define Z001_SMB_BYP_DATA	 (0x02) /**< I2C data signal ctrl  (W) */
#define Z001_SMB_BYP_SWITCH	 (0x01) /**< 1: Switch controller into normal mode */


#endif /* _Z001_SMB_H */

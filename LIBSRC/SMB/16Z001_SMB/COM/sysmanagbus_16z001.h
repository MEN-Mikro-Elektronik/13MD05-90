/***********************  I n c l u d e  -  F i l e  ***********************/
/**
 *         \file sysmanagbus_16z001.h
 *
 *       \author rla
 *        $Date: 2006/07/11 16:11:36 $
 *    $Revision: 1.1 $
 *
 *        \brief Header file for 16Z001_SMB library
 *
 *     Switches:
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: sysmanagbus_16z001.h,v $
 * Revision 1.1  2006/07/11 16:11:36  rla
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2006 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _16Z001_SMB_H
#define _16Z001_SMB_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/**
 * @defgroup _REG_OFFSET Register offsets
 * @{ */
#define SMBHSI		0x00	/**< SMbus Host Config 			*/
#define SMBHCBC		0x02	/**< Base Clock Setting 		*/
#define SMBSTS		0x04	/**< Status Register 			*/
#define SMBCMD		0x05	/**< SMB Command 				*/
#define SMBSTART	0x06	/**< Start Register, wr starts cycle 	*/
#define SMBADR		0x07	/**< SMbus Address Register		*/
#define SMBDAA		0x08	/**< SMBus Data A 				*/
#define SMBDAB		0x09	/**< SMBus Data B 				*/
#define SMBBLK		0x0A	/**< SMbus Block Register	 	*/
#define SMBHCMD		0x0B	/**< SMbus Cmd Register for Host ctrl	*/
#define SMBBYP		0x0C	/**< SMbus Bypass Register		*/
/** @} */

/**
 * @defgroup _REG_SETTINGS Register settings
 * @{ */
#define SMBCLK_149KHZ				(0x0<<5) /**< I2C clk 149kHz */
#define SMBCLK_74KHZ				(0x1<<5) /**< I2C clk  74kHz */
#define SMBCLK_37KHZ				(0x2<<5) /**< I2C clk  37kHz */
#define SMBCLK_223KHZ				(0x4<<5) /**< I2C clk 223kHz */
#define SMBCLK_111KHZ				(0x5<<5) /**< I2C clk 111kHz */
#define SMBCLK_55KHZ				(0x6<<5) /**< I2C clk  55kHz */

#define SMBHCBC_IDLE_BASECLK64		(0x0<<3) /**< IDLE delay BaseClk*64  */
#define SMBHCBC_IDLE_BASECLK32		(0x1<<3) /**< IDLE delay BaseClk*32  */
#define SMBHCBC_IDLE_BASECLK128		(0x2<<3) /**< IDLE delay BaseClk*128 */
/** @} */

/**
 * @defgroup _CMD_CONST Command constants
 * @{ */
#define FPGA16Z001_CMD_ABORT      	0x04		/**< ABORT transaction */
#define FPGA16Z001_CMD_TOUT       	0x08		/**< force TIMEOUT  */
#define FPGA16Z001_CMD_QUICK      	0x0			/**< QUICK timing */
#define FPGA16Z001_CMD_BYTE       	(0x1 << 4)  /**< Send/Receive Byte */
#define FPGA16Z001_CMD_BYTE_DATA  	(0x2 << 4)	/**< Read/Write Byte */
#define FPGA16Z001_CMD_WORD_DATA  	(0x3 << 4)	/**< Read/Write Word */
#define FPGA16Z001_CMD_BLOCK_DATA 	(0x4 << 4)	/**< Read/Write Block */
#define FPGA16Z001_CMD_BLOCK_CLR  	(0x1 << 7)	/**< Reset block registers pointer */
/** @} */

/**
 * @defgroup _STAT_BIT_DEF Status register bits
 * @{ */
#define FPGA16Z001_STS_IDLE		0x04	/**< IDL_STS */
                                        /*!< 1: Bus is idle \n
										 *   0: Bus is free */
#define FPGA16Z001_STS_BUSY		0x08	/**< HST_BSY */
                                        /*!< 1: Host controller is completing
										 *      a command \n
										 *   0: Bus is ready */
#define FPGA16Z001_STS_DONE		0x10	/**< Interrupt status */
                                        /*!< 1: The host controller's interrupt was
										 *      caused after completing a command */
#define FPGA16Z001_STS_DEV		0x20	/**< Error status */
                                        /*!< 1: The host controller's interrupt was
										 *      caused by the controller or the device
										 *      due to the generation of an error */
#define FPGA16Z001_STS_COLL		0x40	/**< Bus Collision */
                                        /*!< 1: The host controller's interrupt was
										 *      caused by the collision of bus
										 *      transactions or no acknowledge */
#define FPGA16Z001_STS_TERM		0x80	/**< Terminate */
                                        /*!< 1: The host controller's interrupt was
										 *      caused by a terminated bus transaction
										 *      in response to "ABORT" */
#define FPGA16Z001_STS_ERR		0xE0	/**< all error bits */
/** @} */

/**
 * @defgroup _HOST_IF_DEF Host Interface Configuration Bits
 * @{ */
#define FPGA16Z001_HSI_HCIE		0x01	/**< Host Controller Interface Enable */
#define FPGA16Z001_HSI_IE		0x04	/**< Interrupt Enable */
/** @} */

/* Other settings */
#define MAX_TIMEOUT 				300		/* times 1/100 sec for sleep*/
#define FPGA16Z001_SMB_IOSIZE 		32


/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

#ifdef __cplusplus
	}
#endif

#endif	/* _16Z001_SMB_H */








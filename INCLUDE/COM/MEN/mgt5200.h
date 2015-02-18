/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mgt5200.h
 *
 *       Author: kp/uf
 *        $Date: 2009/12/10 14:09:58 $
 *    $Revision: 2.27 $
 *
 *  Description: register layout for MGT5200
 *
 *     Switches: MGT5100_ASM_DEFS_ONLY - set this if compiling .s files
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mgt5200.h,v $
 * Revision 2.27  2009/12/10 14:09:58  RLange
 * R: PSC4/5 Uart not supported
 * M: Added Bitdefines for PSC4/5 UART support
 *
 * Revision 2.26  2008/05/07 14:05:27  RLange
 * Added Defines for DEADCYC CTRL Register
 *
 * Revision 2.25  2007/12/14 15:48:17  ufranke
 * added
 *  + MGT5200_USB
 *
 * Revision 2.24  2007/09/11 21:59:15  cs
 * added:
 *   + defines for PSC_MR1, PSC_MR2, PSC_CR
 *
 * Revision 2.23  2007/03/14 16:54:22  ufranke
 * added
 *  + MGT5200_XLBARB_SNOOPWIN__SIZE_1GB
 *
 * Revision 2.22  2006/07/14 14:04:22  ufranke
 * added
 *  + FIFO_CNTRL_TXW_MASK must be set for Rev.B prozessor
 *
 * Revision 2.21  2006/06/09 17:18:43  cs
 * added MGT5200_PSC_SICR_* defines
 *
 * Revision 2.20  2006/05/09 14:43:19  UFRANKE
 * added
 *  + MGT5200_XLB_ARB_PRIO_EN, MGT5200_XLB_ARB_PRIO_REG
 *
 * Revision 2.19  2006/02/22 18:19:25  UFRANKE
 * changed
 *  - bit definition PSC_SR_CSR_UART_DCD_ERROR
 *                to PSC_SR_CSR_UART_RX_READY
 *
 * Revision 2.18  2006/02/15 10:38:09  UFRANKE
 * added
 *  + SDRAM SDELAY and more
 *
 * Revision 2.17  2006/01/27 17:33:16  UFRANKE
 * added
 *  + IIC glitch filter register
 *
 * Revision 2.16  2006/01/19 14:18:41  UFRANKE
 * changed
 *  + _CDM_CFG_PCI_ names
 *
 * Revision 2.15  2005/09/09 20:17:52  CSchuster
 * added defines
 *     + MGT5200_CDM_FDCFG_EXT_USB_SYNC
 *     + MGT5200_PCI_ARB_RESET
 *
 * Revision 2.14  2005/06/24 08:19:32  kp
 * Copyright line changed (sbo)
 *
 * Revision 2.13  2005/04/14 15:53:52  kp
 * added more GPIOPCR defs
 *
 * Revision 2.12  2005/03/09 15:08:57  kp
 * added
 *  - bus timeout registers
 * fixed
 *  - CDM PCI clock divider
 *
 * Revision 2.11  2005/03/09 15:02:44  ufranke
 * added
 *  + IRDA_TX
 *
 * Revision 2.10  2005/02/09 11:17:22  ufranke
 * added
 *  + GPIO defines
 *
 * Revision 2.9  2005/01/14 08:45:36  ufranke
 * added
 *  + PSC4..6
 *
 * Revision 2.8  2004/12/23 12:46:14  ufranke
 * added
 *  + SDRAM controller bits
 *
 * Revision 2.7  2004/11/26 14:27:51  ufranke
 * added
 *  + SPCI definitions
 *
 * Revision 2.6  2004/09/20 15:42:06  ufranke
 * cosmetics
 *
 * Revision 2.5  2004/03/29 15:15:51  UFranke
 * added
 *  + GPIO_CH_MSK
 *
 * Revision 2.4  2004/02/20 17:05:32  UFranke
 * bugfix
 *  - MGT5200_XLBARB_SNOOPWIN__INSIDE is MGT5200_XLBARB_SNOOPWIN__OUTSIDE now
 *
 * Revision 2.3  2003/09/18 12:24:15  UFranke
 * bugfix
 *  - typedef not used in ASM
 *
 * Revision 2.2  2003/09/02 10:55:52  UFranke
 * first MPC5200 revision without D-Cache only 10Mbit/s Ethernet
 *
 * Revision 2.1  2003/07/31 14:21:31  UFranke
 * Initial Revision
 *
 * Revision 2.8  2003/07/04 13:54:35  UFranke
 * changed
 *   -for MGT5200
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _MGT5200_H
#define _MGT5200_H

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#define MGT5200_MBAR_POWER_ON			0x80000000

/* Register offsets - all offsets relative to MBAR */
#define MGT5200_MBAR		0x0000
#define MGT5200_CSxSTART(n)	(0x0004+((n)*8))	/* n=0..5 (or 0..9) */
#define MGT5200_CSxST0P(n) 	(0x0008+((n)*8))
#define MGT5200_SDRAM_CS0	0x0034
#define MGT5200_SDRAM_CS1	0x0038
#define MGT5200_CS6START	0x0058
#define MGT5200_CS6ST0P 	0x005C
#define MGT5200_CS7START	0x0060
#define MGT5200_CS7ST0P 	0x0064
#define MGT5200_PCI1START	0x003c
#define MGT5200_PCI1STOP	0x0040
#define MGT5200_PCI2START	0x0044
#define MGT5200_PCI2STOP	0x0048
#define MGT5200_BOOTSTART	0x004c
#define MGT5200_BOOTSTOP	0x0050
#define MGT5200_ADREN		0x0054

 /* bit definitions */
#define MGT5200_SDRAM_CSx_SIZE_MASK	0x0000001F
#define MGT5200_SDRAM_CSx_SIZE_1MB 	0x00000013
#define MGT5200_SDRAM_CSx_ADDR_MASK	0xFFF00000

#define MGT5200_ADREN__CS_7			0x08000000
#define MGT5200_ADREN__CS_6			0x04000000
#define MGT5200_ADREN__BOOTCS		0x02000000
#define MGT5200_ADREN__CS_5			0x00200000
#define MGT5200_ADREN__CS_4			0x00100000
#define MGT5200_ADREN__CS_3			0x00080000
#define MGT5200_ADREN__CS_2			0x00040000
#define MGT5200_ADREN__CS_1			0x00020000
#define MGT5200_ADREN__CS_0			0x00010000
#define MGT5200_ADREN__WAITSTATE_EN	0x00000001

/* SDRAM controller */
#define MGT5200_SDR_MOD		0x0100	/* (32) mode */
#define MGT5200_SDR_CTR		0x0104	/* (32) control */
#define MGT5200_SDR_CFG1	0x0108	/* (32) config1 */
#define MGT5200_SDR_CFG2	0x010c	/* (32) config2 */
#define MGT5200_SDR_ADRSEL	0x0110	/* (32) address select */
#define MGT5200B_SDR_SDELAY	0x0190	/* (32) tap delay for the MPC5200 B processor */

 /* bit definitions */
#define MGT5200_SDR_MOD_MBA11		0xC0000000
#define MGT5200_SDR_MOD_MBA10		0x80000000
#define MGT5200_SDR_MOD_MBA01		0x40000000 /* extended MODE register (MODEX bellow) */
#define MGT5200_SDR_MOD_MBA00		0x00000000
#define MGT5200_SDR_MOD_CMD			0x00010000

/* (vendor-specific) MA field of mode register (for MBA=00) */
#define MGT5200_SDR_MOD_MA_BURSTLEN_MASK  (0x007 << (2+16))
#define MGT5200_SDR_MOD_MA_BT             (0x008 << (2+16))
#define MGT5200_SDR_MOD_CASLAT_MASK    	  (0x070 << (2+16))
#define MGT5200_SDR_MOD_MA_OPMODE_MASK    (0xf80 << (2+16))
#define MGT5200_SDR_MOD_MA_OPMODE_NORM    (0x000 << (2+16))
#define MGT5200_SDR_MOD_MA_OPMODE_RESDLL  (0x100 << (2+16))

/* (vendor-specific) MA field of extended mode register (for MBA=01) */
#define MGT5200_SDR_MOD_EX_MA_DISDLL        (0x001 << (2+16))
#define MGT5200_SDR_MOD_EX_MA_DS            (0x002 << (2+16))
#define MGT5200_SDR_MOD_EX_MA_OPMODE_MASK   (0xffc << (2+16))


#define MGT5200_SDR_CTR_MODE_EN				0x80000000
#define MGT5200_SDR_CTR_CLK_EN				0x40000000
#define MGT5200_SDR_CTR_DDR					0x20000000
#define MGT5200_SDR_CTR_REFRESH_EN			0x10000000
#define MGT5200_SDR_CTR_HI_ADDR				0x01000000
#define MGT5200_SDR_CTR_DRIVE_DQ_ON_READ	0x00400000
#define MGT5200_SDR_CTR_REFRESH_MASK		0x003F0000
#define MGT5200_SDR_CTR_REFRESH_SHIFT		16
#define MGT5200_SDR_CTR_DQS_OE				0x00000F00
#define MGT5200_SDR_CTR_SOFT_REFRESH		0x00000004
#define MGT5200_SDR_CTR_SOFT_PRECHARGE		0x00000002


/* Clock distribution module */
#define MGT5200_CDM_PORCFG	0x0204	/* (32) power on cfg - read only */
#define MGT5200_CDM_BC		0x0208	/* (32) sw status info over reset */
#define MGT5200_CDM_CFG		0x020c	/* (32) configuration */
#define MGT5200_CDM_FDCFG	0x0210	/* (32) USB/IRDA clock configuration */
#define MGT5200_CDM_CLKEN	0x0214	/* (32) clock enable */

	/* bit definitions */
#define MGT5200_CDM_PORCFG_PLL_CFG_MASK		0x0000001F
#define MGT5200_CDM_PORCFG_XLB_CLK_SEL		0x00000020
#define MGT5200_CDM_PORCFG_SYSPLL_CFG		0x00000040
#define MGT5200_CDM_PORCFG_PPC_TLE			0x00000200
#define MGT5200_CDM_PORCFG_PPC_MSRIP		0x00000400
#define MGT5200_CDM_PORCFG_BOOT_RAM_WAIT	0x00000800
#define MGT5200_CDM_PORCFG_BOOT_RAM_SWAP	0x00001000
#define MGT5200_CDM_PORCFG_BOOT_RAM_SIZE	0x00002000
#define MGT5200_CDM_PORCFG_BOOT_RAM_TYPE	0x00004000
#define MGT5200_CDM_PORCFG_SDRAM_TAP_MASK	0x1F000000
#define MGT5200_CDM_PORCFG_SDRAM_TAP_SHIFT	24

#define MGT5200_CDM_CFG_DDR_MODE        0x01000000
#define MGT5200_CDM_CFG_XLB_CLK         0x00010000  /* do not use for first silicon !*/
#define MGT5200_CDM_CFG_IPB_CLK         	0x00000100  /* XLB/2 */
#define MGT5200_CDM_CFG_PCI_CLK_IPB     	0x00000000  /* PCI=IPB */
#define MGT5200_CDM_CFG_PCI_CLK_HALF_IPB    0x00000001  /* PCI=IPB/2 */
#define MGT5200_CDM_CFG_PCI_CLK_QUARTER_XLB	0x00000002  /* PCI=XLB/4 */

#define MGT5200_CDM_FDCFG_EXT_USB_SYNC     0x04000000
#define MGT5200_CDM_FDCFG_EXT_USB          0x02000000
#define MGT5200_CDM_FDCFG_EXT_IRDA         0x01000000
#define MGT5200_CDM_FDCFG_EN               0x00010000
#define MGT5200_CDM_FDCFG_PHASE_COUNT_MASK 0x0000FFFF

#define MGT5200_CDM_CLKEN__DBGOUT_USB 	0x70000000  /* clock debug on ball B15 */
#define MGT5200_CDM_CLKEN__MEM     		0x00080000
#define MGT5200_CDM_CLKEN__PCI     		0x00040000
#define MGT5200_CDM_CLKEN__LPC     		0x00020000
#define MGT5200_CDM_CLKEN__SLT			0x00010000
#define MGT5200_CDM_CLKEN__SCOM			0x00008000
#define MGT5200_CDM_CLKEN__ATA     		0x00004000
#define MGT5200_CDM_CLKEN__ETH     		0x00002000
#define MGT5200_CDM_CLKEN__USB     		0x00001000
#define MGT5200_CDM_CLKEN__SPI     		0x00000800
#define MGT5200_CDM_CLKEN__PLI     		0x00000400
#define MGT5200_CDM_CLKEN__IRRX    		0x00000200
#define MGT5200_CDM_CLKEN__IRTX    		0x00000100
#define MGT5200_CDM_CLKEN__PSC3    		0x00000080
#define MGT5200_CDM_CLKEN__PSC2    		0x00000040
#define MGT5200_CDM_CLKEN__PSC1    		0x00000020
#define MGT5200_CDM_CLKEN__IRDA    		0x00000010
#define MGT5200_CDM_CLKEN__MSCAN   		0x00000008
#define MGT5200_CDM_CLKEN__I2C     		0x00000004
#define MGT5200_CDM_CLKEN__TIMER   		0x00000002
#define MGT5200_CDM_CLKEN__GPIO    		0x00000001

/* LocalPlus Bus Controller */
#define MGT5200_CSCFG(n)	(0x0300+((n)*4))	/* n=0..5 */

 /* bit definitions */
#define MGT5200_CSCFG__WAITP_MASK		0xFF000000
#define MGT5200_CSCFG__WAITX_MASK		0x00FF0000
#define MGT5200_CSCFG__MUXED			0x00008000
#define MGT5200_CSCFG__ALE_WAIT			0x00004000
#define MGT5200_CSCFG__ACK_ACTIVE		0x00002000
#define MGT5200_CSCFG__CE				0x00001000
#define MGT5200_CSCFG__ADDR_SIZE_8		0x00000000
#define MGT5200_CSCFG__ADDR_SIZE_16		0x00000400
#define MGT5200_CSCFG__ADDR_SIZE_24		0x00000800
#define MGT5200_CSCFG__ADDR_SIZE_25		0x00000C00
#define MGT5200_CSCFG__DATA_SIZE_8		0x00000000
#define MGT5200_CSCFG__DATA_SIZE_16		0x00000040
#define MGT5200_CSCFG__DATA_SIZE_32		0x000000C0
#define MGT5200_CSCFG__WAIT_RW_XplusP	0x00000030
#define MGT5200_CSCFG__WAIT_RX_W_XplusP	0x00000020
#define MGT5200_CSCFG__WAIT_RX_WP		0x00000010
#define MGT5200_CSCFG__WAIT_RX_WX		0x00000000
#define MGT5200_CSCFG__WR_SWAP			0x00000008
#define MGT5200_CSCFG__RD_SWAP			0x00000004
#define MGT5200_CSCFG__WR_ONLY			0x00000002
#define MGT5200_CSCFG__RD_ONLY			0x00000001

#define MGT5200_CS_CTRL		0x0318
 /* bit definitions */
#define MGT5200_CS_CTRL__E_BERR_EN		0x80000000
#define MGT5200_CS_CTRL__I_BERR_EN		0x10000000
#define MGT5200_CS_CTRL__ERR_IRQ_EN		0x08000000
#define MGT5200_CS_CTRL__MODULE_EN		0x01000000

#define MGT5200_DEADCYC_CTRL			0x032C
#define MGT5200_DEADCYC_CTRL_DC7		0x30000000
#define MGT5200_DEADCYC_CTRL_DC6		0x03000000
#define MGT5200_DEADCYC_CTRL_DC5		0x00300000
#define MGT5200_DEADCYC_CTRL_DC4		0x00030000
#define MGT5200_DEADCYC_CTRL_DC3		0x00003000
#define MGT5200_DEADCYC_CTRL_DC2		0x00000300
#define MGT5200_DEADCYC_CTRL_DC1		0x00000030
#define MGT5200_DEADCYC_CTRL_DC0		0x00000003


/* Interrupt Controller */
#define MGT5200_I_PER_MASK	0x500	/* (32) peripheral mask */
#define MGT5200_I_PER_PRI1	0x504	/* (32) peripheral priority and HI/LO 1 */
#define MGT5200_I_PER_PRI2	0x508	/* (32) peripheral priority and HI/LO 2 */
#define MGT5200_I_PER_PRI3	0x50c	/* (32) peripheral priority and HI/LO 3 */
#define MGT5200_I_EXT_EN	0x510	/* (32) external IRQ enable and types */
#define MGT5200_I_CRI_PRI_MAIN_MASK	0x514	/* (32) critical priority / main mask */
#define MGT5200_I_MAIN_PRI1	0x518	/* (32) main priority and INT/SMI sel. 1 */
#define MGT5200_I_MAIN_PRI2	0x51c	/* (32) main priority and INT/SMI sel. 2 */
#define MGT5200_I_PERMAINCRI_STAT	0x524	/* (32) PerStat, MainStat, Critstat enc. */
#define MGT5200_I_CRI_STAT	0x528	/* (32) critical status all */
#define MGT5200_I_MAIN_STAT	0x52c	/* (32) main status all */
#define MGT5200_I_PER_STAT	0x530	/* (32) peripheral status all */
#define MGT5200_I_BUS_ERR_STAT	0x538	/* (32) bus error */

/* General Purpose Timers */
#define MGT5200_GPT_CHANNEL_OFFS 0x10	/* offset between timer N and timer N+1 */
#define MGT5200_GPT_0_CTRL			0x600
#define MGT5200_GPT_0_COUNTER_IN	0x604
#define MGT5200_GPT_0_PWM_CFG		0x608
#define MGT5200_GPT_0_STATUS		0x60C

    /* bit definitions */
#define MGT5200_GPT_CTRL__WDOG_ENABLE			0x00008000
#define MGT5200_GPT_CTRL__COUNTER_ENABLE		0x00001000
#define MGT5200_GPT_CTRL__CONTINOUS	    		0x00000400
#define MGT5200_GPT_CTRL__OPEN_DRAIN    		0x00000200
#define MGT5200_GPT_CTRL__IRQ_ENABLE    		0x00000100
#define MGT5200_GPT_CTRL__GPIO_IN    			0x00000010
#define MGT5200_GPT_CTRL__GPIO_OUT_LOW  		0x00000020
#define MGT5200_GPT_CTRL__GPIO_OUT_HIGH   		0x00000030
#define MGT5200_GPT_CTRL__MODE_SEL_IN_CAPTURE   0x00000001
#define MGT5200_GPT_CTRL__MODE_SEL_OUT_COMPARE  0x00000002
#define MGT5200_GPT_CTRL__MODE_SEL_PWM			0x00000003
#define MGT5200_GPT_CTRL__MODE_SEL_INTERNAL     0x00000004

#define MGT5200_GPT_COUNTER_IN__PRESCALE_MASK	0xFFFF0000
#define MGT5200_GPT_COUNTER_IN__VALUE_MASK		0x0000FFFF

#define MGT5200_GPT_PWM_CFG__WIDTH_MASK			0xFFFF0000
#define MGT5200_GPT_PWM_CFG__POLARITY			0x00000100
#define MGT5200_GPT_PWM_CFG__LOAD				0x00000001

#define MGT5200_GPT_STATUS__CAPTURE_MASK		0xFFFF0000
#define MGT5200_GPT_STATUS__ROLL_OVER_CNT_MASK	0x00007000
#define MGT5200_GPT_STATUS__PIN					0x00000100
#define MGT5200_GPT_STATUS__TEXP				0x00000008
#define MGT5200_GPT_STATUS__PWM					0x00000004
#define MGT5200_GPT_STATUS__COMP				0x00000002
#define MGT5200_GPT_STATUS__CAPT				0x00000001


/* CAN */
#define MGT5200_CAN1	0x0900	/* CAN 1 */
#define MGT5200_CAN2	0x0980	/* CAN 2 */

/* System interface unit */
#define MGT5200_GPIOPCR	0x0b00	/* (32) port config */

	/* simple */
#define MGT5200_GPIOSEN	0x0b04	/* (32) simple gpio enable */
#define MGT5200_GPIOSOD	0x0b08	/* (32) simple gpio open drain */
#define MGT5200_GPIOSDD	0x0b0c	/* (32) simple gpio direction */
#define MGT5200_GPIOSDO	0x0b10	/* (32) simple gpio data output */
#define MGT5200_GPIOSDI	0x0b14	/* (32) simple gpio data input */

#define MGT5200_GPIOS_IRDA_TX	0x10000000
#define MGT5200_GPIOS_USB_8		0x00080000
#define MGT5200_GPIOS_USB_7		0x00040000
#define MGT5200_GPIOS_USB_6		0x00020000
#define MGT5200_GPIOS_USB_0		0x00010000

	/* interrupt */
#define MGT5200_GPIOIEN		0x0b20	/* (32) interrupt gpio pin enable */
#define MGT5200_GPIOIOD		0x0b24	/* (32) interrupt gpio open drain */
#define MGT5200_GPIOIDD		0x0b28	/* (32) interrupt gpio direction */
#define MGT5200_GPIOIDO		0x0b2c	/* (32) interrupt gpio data out */
#define MGT5200_GPIOIIE		0x0b30	/* (32) interrupt gpio interrupt enable */
#define MGT5200_GPIOIIT		0x0b34	/* (32) interrupt gpio interrupt types */
#define MGT5200_GPIOIIME	0x0b38	/* (32) interrupt gpio master enable */
#define MGT5200_GPIOIIST	0x0b3C	/* (32) interrupt gpio interrupt status */

#define MGT5200_GPIOI_ETH16		0x80000000
#define MGT5200_GPIOI_ETH15		0x40000000
#define MGT5200_GPIOI_ETH14		0x20000000
#define MGT5200_GPIOI_ETH13		0x10000000
#define MGT5200_GPIOI_USB9		0x08000000
#define MGT5200_GPIOI_PCS3_8	0x04000000
#define MGT5200_GPIOI_PCS3_5	0x02000000
#define MGT5200_GPIOI_PCS3_4	0x01000000

	/* wakeup */
#define MGT5200_GPIOWEN	0x0c00	/* (32) wakeup gpio enable */
#define MGT5200_GPIOWOD	0x0c04  /* (32) wakeup gpio open drain */
#define MGT5200_GPIOWDD	0x0c08  /* (32) wakeup gpio direction */
#define MGT5200_GPIOWD0	0x0c0c  /* (32) wakeup gpio data out */
#define MGT5200_GPIOWUE	0x0c10  /* (32) wakeup gpio up irq enable */
#define MGT5200_GPIOWIE	0x0c14  /* (32) wakeup gpio simple irq enable */
#define MGT5200_GPIOWIT	0x0c18  /* (32) wakeup gpio irq type */
#define MGT5200_GPIOWME	0x0c1c  /* (32) wakeup gpio master enable */
#define MGT5200_GPIOWDI	0x0c20  /* (32) wakeup gpio data in */
#define MGT5200_GPIOWST	0x0c24  /* (32) wakeup gpio status */

#define MGT5200_GPIOWE	0x0c00	/* (32) wakeup GPIO enables */
#define MGT5200_GPIOWOD	0x0c04	/* (32) wakeup GPIO open drain */
#define MGT5200_GPIOWDD	0x0c08	/* (32) wakeup GPIO direction */
#define MGT5200_GPIOWDO	0x0c0c	/* (32) wakeup GPIO data output */
#define MGT5200_GPIOWI	0x0c20	/* (32) wakeup GPIO data input */

#define MGT5200_GPIOW_PSC2_4  0x02000000



#define MGT5200_GPIO_CH_MSK(_x_) (0x01000000 << _x_)

	/* bit definitions */
#define MGT5200_GPIOPCR__SDRAM1_CS		0x80000000
#define MGT5200_GPIOPCR__LPTZ			0x40000000
#define MGT5200_GPIOPCR__ALT_MASK		0x30000000
#define MGT5200_GPIOPCR__CS7			0x08000000
#define MGT5200_GPIOPCR__CS6			0x04000000
#define MGT5200_GPIOPCR__ATA_MASK		0x03000000
#define MGT5200_GPIOPCR__IR_USB_CLK		0x00800000
#define MGT5200_GPIOPCR__IRDA_MASK		0x00700000
#define MGT5200_GPIOPCR__ETHER_MASK		0x000F0000
#define MGT5200_GPIOPCR__PCI_DIS		0x00008000
#define MGT5200_GPIOPCR__USB_SINGLE_ENDED	0x00004000
#define MGT5200_GPIOPCR__USB_MASK			0x00007000
#define MGT5200_GPIOPCR__USB_1				0x00001000
#define MGT5200_GPIOPCR__PSC4_5_UART	0x00002000
#define MGT5200_GPIOPCR__PCS3_MASK		0x00000F00
#define MGT5200_GPIOPCR__PCS3_UART_SPI	0x00000c00
#define MGT5200_GPIOPCR__PCS3_UART    	0x00000400
#define MGT5200_GPIOPCR__PCS3_GPIO    	0x00000000
#define MGT5200_GPIOPCR__PCS2_MASK		0x00000070
#define MGT5200_GPIOPCR__PCS2_CAN     	0x00000010
#define MGT5200_GPIOPCR__PCS2_GPIO    	0x00000000
#define MGT5200_GPIOPCR__PCS1_MASK		0x00000007
#define MGT5200_GPIOPCR__PCS1_UART    	0x00000004



/* PCI */
#define MGT5200_PCI			0x0d00	/* standard PCI header space  */
#define MGT5200_PCI_CTRL_STAT		0x0d60	/* (32) */
#define MGT5200_PCI_T_ATRANS0		0x0d64	/* (32) */
#define MGT5200_PCI_T_ATRANS1		0x0d68	/* (32) */
#define MGT5200_PCI_T_CTRL			0x0d6c	/* (32) */
#define MGT5200_PCI_I_BASE_TRANS0	0x0d70	/* (32) */
#define MGT5200_PCI_I_BASE_TRANS1	0x0d74	/* (32) */
#define MGT5200_PCI_I_BASE_TRANS2	0x0d78	/* (32) */
#define MGT5200_PCI_I_WIN_CTRL		0x0d80	/* (32) */
#define MGT5200_PCI_I_CTRL			0x0d84	/* (32) */
#define MGT5200_PCI_I_STAT			0x0d88	/* (32) */
#define MGT5200_PCI_ARB				0x0d8c	/* (32) */
#define MGT5200_PCI_CONFIG_ADDR		0x0dF8	/* (32) */

  /* PCI bit definitions */
#define MGT5200_PCI_CTRL_STAT__BM		0x40000000
#define MGT5200_PCI_CTRL_STAT__PE		0x20000000
#define MGT5200_PCI_CTRL_STAT__SE		0x10000000
#define MGT5200_PCI_CTRL_STAT__BM_IE	0x00000000
#define MGT5200_PCI_CTRL_STAT__PE_IE	0x00000000
#define MGT5200_PCI_CTRL_STAT__SE_IE	0x00000000
#define MGT5200_PCI_CTRL_STAT__RESET	0x00000001

#define MGT5200_PCI_T_ATRANS_EN			0x00000001

#define MGT5200_PCI_I_WIN_CTRL__IO					0x08  /* IO - !MEM */
#define MGT5200_PCI_I_WIN_CTRL__MEM_READ_MULTIPL	0x04
#define MGT5200_PCI_I_WIN_CTRL__MEM_READ_LINE		0x02
#define MGT5200_PCI_I_WIN_CTRL__MEM_READ			0x00
#define MGT5200_PCI_I_WIN_CTRL__EN					0x01

#define MGT5200_PCI_I_CTRL__RE_IE		0x04000000
#define MGT5200_PCI_I_CTRL__IA_IE		0x02000000
#define MGT5200_PCI_I_CTRL__TA_IE		0x01000000
#define MGT5200_PCI_I_CTRL__MAX_RETRY	0x000000FF

#define MGT5200_PCI_I_STAT__RE			0x04000000
#define MGT5200_PCI_I_STAT__IA			0x02000000
#define MGT5200_PCI_I_STAT__TA			0x01000000

#define MGT5200_PCI_CONFIG_ADDR__EN		0x80000000

#define MGT5200_PCI_ARB_RESET           0x01000000

/* SPI */
#define MGT5200_SPI_CTRL_1		0x0f00	/* control 1 */
#define MGT5200_SPI_CTRL_2		0x0f01	/* control 2 */
#define MGT5200_SPI_BAUD_RATE   0x0f04	/* clock divider */
#define MGT5200_SPI_STATUS		0x0f05	/* status */
#define MGT5200_SPI_DATA		0x0f09	/* W - tx data / R - rx data */
#define MGT5200_SPI_GPIO_DATA	0x0f0D	/* data of GPIO pins */
#define MGT5200_SPI_DATA_DIR	0x0f10	/* must set for output pins */

	/* bit definitions */
#define MGT5200_SPI_CTRL_1__IRQ_EN		0x80
#define MGT5200_SPI_CTRL_1__SPI_EN		0x40
#define MGT5200_SPI_CTRL_1__MASTER		0x10
#define MGT5200_SPI_CTRL_1__CLK_POL		0x08
#define MGT5200_SPI_CTRL_1__CLK_PHASE	0x04
#define MGT5200_SPI_CTRL_1__CS_OE		0x02
#define MGT5200_SPI_CTRL_1__LSB_FIRST	0x01

#define MGT5200_SPI_CTRL_2__STOP_IN_WAIT	0x00
#define MGT5200_SPI_CTRL_2__BI_DIR			0x00

#define MGT5200_SPI_BAUD_RATE__PRE_DIV_SHIFT	4		/* pre divider = x*2+2 */
#define MGT5200_SPI_BAUD_RATE__PRE_DIV_MASK		0x70
#define MGT5200_SPI_BAUD_RATE__DIV_MASK			0x07    /* devider 2^n  */

#define MGT5200_SPI_STATUS__IRQ			0x80
#define MGT5200_SPI_STATUS__WCOL		0x40
#define MGT5200_SPI_STATUS__RESERVED	0x20
#define MGT5200_SPI_STATUS__MODF		0x10

#define MGT5200_SPI_DATA_DIR_SS			0x08
#define MGT5200_SPI_DATA_DIR_CLK		0x04
#define MGT5200_SPI_DATA_DIR_DO			0x02
#define MGT5200_SPI_DATA_DIR_DI			0x01

/* USB */
#define MGT5200_USB			0x1000	/* USB OHCI */

/* SMARTCOM */
#define MGT5200_SDMA	0x1200	/* SmartComm unit registers */

/* XLBARB */
#define MBAR_XLARB      	  0x1F00
#define MGT5200_XLBARB_CTRL   0x1F40
#define MGT5200_XLBARB_ADD_TOUT	0x1F58
#define MGT5200_XLBARB_DAT_TOUT	0x1F5C
#define MGT5200_XLBARB_BUS_TOUT	0x1F60
#define MGT5200_XLB_ARB_PRIO_EN			0x1f64
#define MGT5200_XLB_ARB_PRIO_REG		0x1f68


#define MGT5200_XLBARB_CTRL__PIPELINE_DIS		0x80000000
#define MGT5200_XLBARB_CTRL__BESTCOMM_SNOOP_DIS	0x00010000
#define MGT5200_XLBARB_CTRL__SNOOP_EN			0x00008000
#define MGT5200_XLBARB_CTRL__USE_WWF			0x00004000
#define MGT5200_XLBARB_CTRL__TIMEBASE_EN		0x00002000
#define MGT5200_XLBARB_CTRL__WAITSTATE			0x00000800
/*#define MGT5200_XLBARB_CTRL__PARKING_MASTER
#define MGT5200_XLBARB_CTRL__PARKING_MODE*/
#define MGT5200_XLBARB_CTRL__BA					0x00000008
#define MGT5200_XLBARB_CTRL__DT					0x00000004
#define MGT5200_XLBARB_CTRL__AT					0x00000002


#define MGT5200_XLBARB_SNOOPWIN__BASE_MASK		0xFFFFF000
#define MGT5200_XLBARB_SNOOPWIN__OUTSIDE		0x00000080
#define MGT5200_XLBARB_SNOOPWIN__SIZE_16MB		0x00000017
#define MGT5200_XLBARB_SNOOPWIN__SIZE_32MB		0x00000018
#define MGT5200_XLBARB_SNOOPWIN__SIZE_64MB		0x00000019
#define MGT5200_XLBARB_SNOOPWIN__SIZE_128MB		0x0000001A
#define MGT5200_XLBARB_SNOOPWIN__SIZE_256MB		0x0000001B
#define MGT5200_XLBARB_SNOOPWIN__SIZE_512MB		0x0000001C
#define MGT5200_XLBARB_SNOOPWIN__SIZE_1GB		0x0000001D

#ifdef _ASM_

#else
typedef struct xlarb_register_set {
	volatile u_int32 reserved0;			/* MBAR_XLARB + 0x00 */
	volatile u_int32 reserved1;			/* MBAR_XLARB + 0x04 */
	volatile u_int32 reserved2;			/* MBAR_XLARB + 0x08 */
	volatile u_int32 reserved3;			/* MBAR_XLARB + 0x0c */
	volatile u_int32 reserved4;			/* MBAR_XLARB + 0x10 */
	volatile u_int32 reserved5;			/* MBAR_XLARB + 0x14 */
	volatile u_int32 reserved6;			/* MBAR_XLARB + 0x18 */
	volatile u_int32 reserved7;			/* MBAR_XLARB + 0x1c */
	volatile u_int32 reserved8;			/* MBAR_XLARB + 0x20 */
	volatile u_int32 reserved9;			/* MBAR_XLARB + 0x24 */
	volatile u_int32 reserved10;		/* MBAR_XLARB + 0x28 */
	volatile u_int32 reserved11;		/* MBAR_XLARB + 0x2c */
	volatile u_int32 reserved12;		/* MBAR_XLARB + 0x30 */
	volatile u_int32 reserved13;		/* MBAR_XLARB + 0x34 */
	volatile u_int32 reserved14;		/* MBAR_XLARB + 0x38 */
	volatile u_int32 reserved15;		/* MBAR_XLARB + 0x3c */

	volatile u_int32 config;			/* MBAR_XLARB + 0x40 */
	volatile u_int32 version;			/* MBAR_XLARB + 0x44 */
	volatile u_int32 status;			/* MBAR_XLARB + 0x48 */
	volatile u_int32 int_enable;		/* MBAR_XLARB + 0x4c */
	volatile u_int32 add_capture;		/* MBAR_XLARB + 0x50 */
	volatile u_int32 bus_sig_capture;	/* MBAR_XLARB + 0x54 */
	volatile u_int32 add_time_out;		/* MBAR_XLARB + 0x58 */
	volatile u_int32 data_time_out;		/* MBAR_XLARB + 0x5c */
	volatile u_int32 bus_time_out;		/* MBAR_XLARB + 0x60 */
	volatile u_int32 priority_enable;	/* MBAR_XLARB + 0x64 */
	volatile u_int32 priority;			/* MBAR_XLARB + 0x68 */
	volatile u_int32 arb_base_addr2;	/* MBAR_XLARB + 0x6c */
	volatile u_int32 snoop_window;		/* MBAR_XLARB + 0x70 */

	volatile u_int32 reserved16;		/* MBAR_XLARB + 0x74 */
	volatile u_int32 reserved17;		/* MBAR_XLARB + 0x78 */
	volatile u_int32 reserved18;		/* MBAR_XLARB + 0x7c */

	volatile u_int32 control;			/* MBAR_XLARB + 0x80 */
	volatile u_int32 init_total_count;	/* MBAR_XLARB + 0x84 */
	volatile u_int32 int_total_count;	/* MBAR_XLARB + 0x88 */

	volatile u_int32 reserved19;		/* MBAR_XLARB + 0x8c */

	volatile u_int32 lower_address;		/* MBAR_XLARB + 0x90 */
	volatile u_int32 higher_address;	/* MBAR_XLARB + 0x94 */
	volatile u_int32 int_window_count;	/* MBAR_XLARB + 0x98 */
	volatile u_int32 window_ter_count;	/* MBAR_XLARB + 0x9c */
} xlarb_regs;

#endif /*_ASM_*/


/*  PSC */
#define MGT5200_PSC1	0x2000	/* UART/modem serial channels */
#define MGT5200_PSC2	0x2200
#define MGT5200_PSC3	0x2400
#define MGT5200_PSC4	0x2600
#define MGT5200_PSC5	0x2800
#define MGT5200_PSC6	0x2c00	/* !!! channel offset is 0x400 for PSC6 !!! */

#define MGT5200_PSC_CHANNEL_OFFSET (MGT5200_PSC2-MGT5200_PSC1)


#define MGT5200_ETH		0x3000	/* Ethernet controller registers */

/* SMART DMA PCI */
#define MGT5200_SPCI_PACKET_SIZE_TX		0x3800
#define MGT5200_SPCI_START_ADDR_TX		0x3804
#define MGT5200_SPCI_CMD_TX				0x3808
#define MGT5200_SPCI_ENABLE_TX			0x380C
#define MGT5200_SPCI_NEXT_ADDR_TX		0x3810
#define MGT5200_SPCI_LAST_WORD_TX		0x3814
#define MGT5200_SPCI_DONE_COUNT_TX		0x3818
#define MGT5200_SPCI_STATUS_TX			0x381C
#define MGT5200_SPCI_FIFO_DATA_TX		0x3840
#define MGT5200_SPCI_FIFO_STATUS_TX		0x3844
#define MGT5200_SPCI_FIFO_CTRL_TX		0x3848
#define MGT5200_SPCI_ALARM_TX			0x384C
#define MGT5200_SPCI_READ_POINTER_TX	0x3850
#define MGT5200_SPCI_WRITE_POINTER_TX	0x3854

	/* bit definitions */
#define MGT5200_SPCI_PACKET_SIZE_TX_SH	16

#define MGT5200_SPCI_CMD_TX_CMD_SH		24
#define MGT5200_SPCI_CMD_TX_RETRY_SH	16
#define MGT5200_SPCI_CMD_TX_MAXBEATS_SH	 8

#define MGT5200_SPCI_ENABLE_TX_RESET 	0x80000000
#define MGT5200_SPCI_ENABLE_TX_MASTER	0x01000000


#define MGT5200_SPCI_PACKET_SIZE_RX		0x3880
#define MGT5200_SPCI_START_ADDR_RX		0x3884
#define MGT5200_SPCI_CMD_RX				0x3888
#define MGT5200_SPCI_ENABLE_RX			0x388C
#define MGT5200_SPCI_NEXT_ADDR_RX		0x3890
#define MGT5200_SPCI_DONE_COUNT_RX		0x3898
#define MGT5200_SPCI_STATUS_RX			0x389C
#define MGT5200_SPCI_FIFO_DATA_RX		0x38C0
#define MGT5200_SPCI_FIFO_STATUS_RX		0x38C4
#define MGT5200_SPCI_FIFO_CTRL_RX		0x38C8
#define MGT5200_SPCI_ALARM_RX			0x38CC
#define MGT5200_SPCI_READ_POINTER_RX	0x38D0
#define MGT5200_SPCI_WRITE_POINTER_RX	0x38D4

	/* bit definitions */
#define MGT5200_SPCI_PACKET_SIZE_RX_SH	16

#define MGT5200_SPCI_CMD_RX_CMD_SH		24
#define MGT5200_SPCI_CMD_RX_RETRY_SH	16
#define MGT5200_SPCI_CMD_RX_MAXBEATS_SH	 8

#define MGT5200_SPCI_ENABLE_RX_RESET 	0x80000000
#define MGT5200_SPCI_ENABLE_RX_MASTER	0x01000000

#define MGT5200_SPCI_STATUS_RX_NORMAL_TERM	0x01000000


#define MGT5200_I2C1	0x3d00	/* I2C 1 */
#define MGT5200_I2C2	0x3d40	/* I2C 2 */


#define MGT5200_SRAM	0x8000	/* 16k internal SRAM */

/*----------------------------------------------------------------------
 * subcompontents (offsets relative to component start)
 */

/* Offset CANx to register: see MEN/mscan.h */

/* Offset PSCx to register */
#define PSC_MR        0x00  /* ( 8) mode 1,2 */
#define PSC_SR_CSR    0x04  /* (16) status / modem mode / clock select */
#define PSC_CR        0x08  /* ( 8) command */
#define PSC_RB_TB     0x0c  /* (32) Rx/Tx fifo */
#define PSC_IPCR_ACR  0x10  /* ( 8) port change (r) / aux control (w) */
#define PSC_ISR_IMR   0x14  /* (16) int status (r) / int mask (w) */
#define PSC_CTUR      0x18  /* ( 8) counter timer upper */
#define PSC_CTLR      0x1c  /* ( 8) counter timer lower */
#define PSC_IP		  0x34  /* ( 8) input port */
#define PSC_OP1		  0x38  /* ( 8) set output port */
#define PSC_OP0		  0x3c  /* ( 8) clear output port */
#define PSC_SICR      0x40  /* ( 8) control */
#define PSC_RFNUM	  0x58 	/* (16) rx fifo count */
#define PSC_TFNUM	  0x5c 	/* (16) tx fifo count */
#define PSC_RFSTAT	  0x64	/* (16) rx fifo status */
#define PSC_RFCNTL	  0x68	/* ( 8) rx fifo control */
#define PSC_RFALARM	  0x6e	/* (16) rx fifo alarm */
#define PSC_TFSTAT	  0x84	/* (16) tx fifo status */
#define PSC_TFCNTL	  0x88	/* ( 8) tx fifo control */
#define PSC_TFALARM	  0x8e	/* (16) tx fifo alarm */

/* PSC bit definitions */
/* MR1 - mode register 1 */
#define PSC_MR1_UART_BC0    0x01    /* bits per char 0 */
#define PSC_MR1_UART_BC1    0x02    /* bits per char 1 */
#define PSC_MR1_UART_PT     0x04    /* parity type */
#define PSC_MR1_UART_PM0    0x08    /* parity mode 0 */
#define PSC_MR1_UART_PM1    0x10    /* parity mode 1 */
#define PSC_MR1_UART_ERR    0x20    /* error mode */
#define PSC_MR1_UART_RX_IRQ 0x40    /* receiver interrupt select */
#define PSC_MR1_UART_RX_RTS 0x80    /* Rx RTS control */

/* Some common modes */
#define PSC_MR1_UART_BITS_CHAR_5    0
#define PSC_MR1_UART_BITS_CHAR_6    PSC_MR1_UART_BC0
#define PSC_MR1_UART_BITS_CHAR_7    PSC_MR1_UART_BC1
#define PSC_MR1_UART_BITS_CHAR_8    (PSC_MR1_UART_BC0 \
                     | PSC_MR1_UART_BC1)
#define PSC_MR1_UART_NO_PARITY  (PSC_MR1_UART_PM1)
#define PSC_MR1_UART_PAR_MODE_MULTI (PSC_MR1_UART_PM0 \
                     | PSC_MR1_UART_PM1)
#define PSC_MR1_UART_EVEN_PARITY    0
#define PSC_MR1_UART_ODD_PARITY PSC_MR1_UART_PT

/* MR2 - mode register 2 */
#define PSC_MR2_UART_SB0    0x01    /* stop bit control 0 */
#define PSC_MR2_UART_SB1    0x02    /* stop bit control 1 */
#define PSC_MR2_UART_SB2    0x04    /* stop bit control 2 */
#define PSC_MR2_UART_SB3    0x08    /* stop bit control 3 */
#define PSC_MR2_UART_TX_CTS 0x10    /* Tx CTS control */
#define PSC_MR2_UART_TX_RTS 0x20    /* Tx RTS control */
#define PSC_MR2_UART_CM0    0x40    /* channel mode 0 */
#define PSC_MR2_UART_CM1    0x80    /* channel mode 1 */

/* Some common bit lengths */
#define PSC_MR2_UART_STOP_BITS_1    (PSC_MR2_UART_SB0 \
                     | PSC_MR2_UART_SB1 \
                     | PSC_MR2_UART_SB2)
#define PSC_MR2_UART_STOP_BITS_2    (PSC_MR2_UART_SB0 \
                     | PSC_MR2_UART_SB1 \
                     | PSC_MR2_UART_SB2 \
                     | PSC_MR2_UART_SB3)

/* SR - status register */
#define PSC_SR_CSR_UART_RECEIVED_BREAK   0x8000  /*  status in UART mode */
#define PSC_SR_CSR_UART_FRAME_ERROR      0x4000  /*  status in UART mode */
#define PSC_SR_CSR_UART_PARITY_ERROR     0x2000  /*  status in UART mode */
#define PSC_SR_CSR_UART_RX_OVERRUN       0x1000  /*  status in UART mode */
#define PSC_SR_CSR_UART_TX_EMPTY		 0x0800  /*  status in UART mode */
#define PSC_SR_CSR_UART_TX_READY		 0x0400  /*  status in UART mode */
#define PSC_SR_CSR_UART_RX_FULL			 0x0200  /*  status in UART mode */
#define PSC_SR_CSR_UART_RX_READY		 0x0100  /*  status in UART mode */

/* CR - command register */
#define PSC_CR_UART_RC0     0x01    /* receiver control bit 0 */
#define PSC_CR_UART_RC1     0x02    /* receiver control bit 1 */
#define PSC_CR_UART_TC0     0x04    /* transmitter control bit 0 */
#define PSC_CR_UART_TC1     0x08    /* transmitter control bit 1 */
#define PSC_CR_UART_MISC0   0x10    /* misc control bit 0 */
#define PSC_CR_UART_MISC1   0x20    /* misc control bit 1 */
#define PSC_CR_UART_MISC2   0x40    /* misc control bit 2 */

/* Some common commands */
#define PSC_CR_UART_TX_ENABLE       (PSC_CR_UART_TC0)
#define PSC_CR_UART_TX_DISABLE      (PSC_CR_UART_TC1)
#define PSC_CR_UART_RX_ENABLE       (PSC_CR_UART_RC0)
#define PSC_CR_UART_RX_DISABLE      (PSC_CR_UART_RC1)
#define PSC_CR_UART_RESET_MODE_PTR  (PSC_CR_UART_MISC0)
#define PSC_CR_UART_RESET_RX        (PSC_CR_UART_MISC1)
#define PSC_CR_UART_RESET_TX        (PSC_CR_UART_MISC0 \
                                    | PSC_CR_UART_MISC1)
#define PSC_CR_UART_RESET_ERR       (PSC_CR_UART_MISC2)
#define PSC_CR_UART_RESET_BRK       (PSC_CR_UART_MISC0 \
                                    | PSC_CR_UART_MISC2)

/* Interrupt Status and Mask Register */
#define PSC_ISR_RxRDY  (1<<9)   /* RxRDY or FFULL */
#define PSC_ISR_TxRDY  (1<<8)
#define PSC_ISR_DEOF   (1<<7)
#define PSC_ISR_DB     (1<<10)
#define PSC_ISR_IPC    (1<<15)

/* Serial interface control register */
#define MPC5200_PSC_SICR_GENCLK  0x00800000
#define MPC5200_PSC_SICR_SYNCPOL 0x00100000

/*----------------------------------------------------------------------
 * I2C register offsets relative to MGT5200_I2Cx
 */
#define MGT5200_I2C_ADDR	0x00 			/* address */
#define MGT5200_I2C_DIV		0x04 			/* frequency divider */
#define MGT5200_I2C_CTRL	0x08 			/* address */
#define MGT5200_I2C_STAT	0x0c 			/* status */
#define MGT5200_I2C_DATA	0x10 			/* data I/O */
#define MGT5200_I2C_ICTRL	0x20 			/* interrupt control */
#define MGT5200_I2C_MIFR	0x24 			/* glitch filter control */

#define MGT5200_I2C_CTRL_EN		0x80
#define MGT5200_I2C_CTRL_IEN	0x40
#define MGT5200_I2C_CTRL_START 	0x20
#define MGT5200_I2C_CTRL_TX		0x10
#define MGT5200_I2C_CTRL_TXAK	0x08
#define MGT5200_I2C_CTRL_RSTART	0x04

#define MGT5200_I2C_STAT_COMPL		0x80
#define MGT5200_I2C_STAT_AAS		0x40
#define MGT5200_I2C_STAT_BUSY		0x20
#define MGT5200_I2C_STAT_ARBLOST	0x10
#define MGT5200_I2C_STAT_SWRITE		0x04
#define MGT5200_I2C_STAT_IRQ		0x02
#define MGT5200_I2C_STAT_RXAK		0x01

#define MGT5200_I2C_DATA_RX		0x01

#define MGT5200_I2C_MIFR_MAX	0x0F000000 			/* glitch filter max 15 IPB clocks */


/*----------------------------------------------------------------------
 * Bit definitions
 */

/*----------------------------------------------------------------------
 * Ethernet controller registers (located at MGT5200_ETH)
 */

#ifndef _ASMLANGUAGE

typedef struct
{
	long td_t;
	long td_t_end;
	long var_t;
	long fd_t_flags;
	long resv1;
	long resv2;
	long context_save;
	long unknown;
}MGT5200_BC_TASK_TBL_ENRTY;

typedef struct {

    /*  Control and status Registers (offset 000-1FF) */

	volatile u_int32 fec_id;				/* MGT5200_ETH + 0x000 */
	volatile u_int32 ievent;				/* MGT5200_ETH + 0x004 */
	volatile u_int32 imask;					/* MGT5200_ETH + 0x008 */

	volatile u_int32 RES0[1];				/* MGT5200_ETH + 0x00C */
	volatile u_int32 r_des_active;			/* MGT5200_ETH + 0x010 */
	volatile u_int32 x_des_active;			/* MGT5200_ETH + 0x014 */
	volatile u_int32 r_des_active_cl;		/* MGT5200_ETH + 0x018 */
	volatile u_int32 x_des_active_cl;		/* MGT5200_ETH + 0x01C */
	volatile u_int32 ivent_set;				/* MGT5200_ETH + 0x020 */
	volatile u_int32 ecntrl;				/* MGT5200_ETH + 0x024 */

	volatile u_int32 RES1[6];				/* MGT5200_ETH + 0x028-03C */
	volatile u_int32 mii_data;				/* MGT5200_ETH + 0x040 */
	volatile u_int32 mii_speed;				/* MGT5200_ETH + 0x044 */
	volatile u_int32 mii_status;			/* MGT5200_ETH + 0x048 */

	volatile u_int32 RES2[5];				/* MGT5200_ETH + 0x04C-05C */
	volatile u_int32 mib_data;				/* MGT5200_ETH + 0x060 */
	volatile u_int32 mib_control;			/* MGT5200_ETH + 0x064 */

	volatile u_int32 RES3[6];				/* MGT5200_ETH + 0x068-7C */
	volatile u_int32 r_activate;			/* MGT5200_ETH + 0x080 */
	volatile u_int32 r_cntrl;				/* MGT5200_ETH + 0x084 */
	volatile u_int32 r_hash;				/* MGT5200_ETH + 0x088 */
	volatile u_int32 r_data;				/* MGT5200_ETH + 0x08C */
	volatile u_int32 ar_done;				/* MGT5200_ETH + 0x090 */
	volatile u_int32 r_test;				/* MGT5200_ETH + 0x094 */
	volatile u_int32 r_mib;					/* MGT5200_ETH + 0x098 */
	volatile u_int32 r_da_low;				/* MGT5200_ETH + 0x09C */
	volatile u_int32 r_da_high;				/* MGT5200_ETH + 0x0A0 */

	volatile u_int32 RES4[7];				/* MGT5200_ETH + 0x0A4-0BC */
	volatile u_int32 x_activate;			/* MGT5200_ETH + 0x0C0 */
	volatile u_int32 x_cntrl;				/* MGT5200_ETH + 0x0C4 */
	volatile u_int32 backoff;				/* MGT5200_ETH + 0x0C8 */
	volatile u_int32 x_data;				/* MGT5200_ETH + 0x0CC */
	volatile u_int32 x_status;				/* MGT5200_ETH + 0x0D0 */
	volatile u_int32 x_mib;					/* MGT5200_ETH + 0x0D4 */
	volatile u_int32 x_test;				/* MGT5200_ETH + 0x0D8 */
	volatile u_int32 fdxfc_da1;				/* MGT5200_ETH + 0x0DC */
	volatile u_int32 fdxfc_da2;				/* MGT5200_ETH + 0x0E0 */
	volatile u_int32 paddr1;				/* MGT5200_ETH + 0x0E4 */
	volatile u_int32 paddr2;				/* MGT5200_ETH + 0x0E8 */
	volatile u_int32 op_pause;				/* MGT5200_ETH + 0x0EC */

	volatile u_int32 RES5[4];				/* MGT5200_ETH + 0x0F0-0FC */
	volatile u_int32 instr_reg;				/* MGT5200_ETH + 0x100 */
	volatile u_int32 context_reg;			/* MGT5200_ETH + 0x104 */
	volatile u_int32 test_cntrl;			/* MGT5200_ETH + 0x108 */
	volatile u_int32 acc_reg;				/* MGT5200_ETH + 0x10C */
	volatile u_int32 ones;					/* MGT5200_ETH + 0x110 */
	volatile u_int32 zeros;					/* MGT5200_ETH + 0x114 */
	volatile u_int32 iaddr1;				/* MGT5200_ETH + 0x118 */
	volatile u_int32 iaddr2;				/* MGT5200_ETH + 0x11C */
	volatile u_int32 gaddr1;				/* MGT5200_ETH + 0x120 */
	volatile u_int32 gaddr2;				/* MGT5200_ETH + 0x124 */
	volatile u_int32 random;				/* MGT5200_ETH + 0x128 */
	volatile u_int32 rand1;					/* MGT5200_ETH + 0x12C */
	volatile u_int32 tmp;					/* MGT5200_ETH + 0x130 */

	volatile u_int32 RES6[3];				/* MGT5200_ETH + 0x134-13C */
	volatile u_int32 fifo_id;				/* MGT5200_ETH + 0x140 */
	volatile u_int32 x_wmrk;				/* MGT5200_ETH + 0x144 */
	volatile u_int32 fcntrl;				/* MGT5200_ETH + 0x148 */
	volatile u_int32 r_bound;				/* MGT5200_ETH + 0x14C */
	volatile u_int32 r_fstart;				/* MGT5200_ETH + 0x150 */
	volatile u_int32 r_count;				/* MGT5200_ETH + 0x154 */
	volatile u_int32 r_lag;					/* MGT5200_ETH + 0x158 */
	volatile u_int32 r_read;				/* MGT5200_ETH + 0x15C */
	volatile u_int32 r_write;				/* MGT5200_ETH + 0x160 */
	volatile u_int32 x_count;				/* MGT5200_ETH + 0x164 */
	volatile u_int32 x_lag;					/* MGT5200_ETH + 0x168 */
	volatile u_int32 x_retry;				/* MGT5200_ETH + 0x16C */
	volatile u_int32 x_write;				/* MGT5200_ETH + 0x170 */
	volatile u_int32 x_read;				/* MGT5200_ETH + 0x174 */

	volatile u_int32 RES7[2];				/* MGT5200_ETH + 0x178-17C */
	volatile u_int32 fm_cntrl;				/* MGT5200_ETH + 0x180 */
	volatile u_int32 rfifo_data;			/* MGT5200_ETH + 0x184 */
	volatile u_int32 rfifo_status;			/* MGT5200_ETH + 0x188 */
	volatile u_int32 rfifo_cntrl;			/* MGT5200_ETH + 0x18C */
	volatile u_int32 rfifo_lrf_ptr;			/* MGT5200_ETH + 0x190 */
	volatile u_int32 rfifo_lwf_ptr;			/* MGT5200_ETH + 0x194 */
	volatile u_int32 rfifo_alarm;			/* MGT5200_ETH + 0x198 */
	volatile u_int32 rfifo_rdptr;			/* MGT5200_ETH + 0x19C */
	volatile u_int32 rfifo_wrptr;			/* MGT5200_ETH + 0x1A0 */
	volatile u_int32 tfifo_data;			/* MGT5200_ETH + 0x1A4 */
	volatile u_int32 tfifo_status;			/* MGT5200_ETH + 0x1A8 */
	volatile u_int32 tfifo_cntrl;			/* MGT5200_ETH + 0x1AC */
	volatile u_int32 tfifo_lrf_ptr;			/* MGT5200_ETH + 0x1B0 */
	volatile u_int32 tfifo_lwf_ptr;			/* MGT5200_ETH + 0x1B4 */
	volatile u_int32 tfifo_alarm;			/* MGT5200_ETH + 0x1B8 */
	volatile u_int32 tfifo_rdptr;			/* MGT5200_ETH + 0x1BC */
	volatile u_int32 tfifo_wrptr;			/* MGT5200_ETH + 0x1C0 */

	volatile u_int32 reset_cntrl;			/* MGT5200_ETH + 0x1C4 */
	volatile u_int32 xmit_fsm;				/* MGT5200_ETH + 0x1C8 */

	volatile u_int32 RES8[3];				/* MGT5200_ETH + 0x1CC-1D4 */
	volatile u_int32 rdes_data0;			/* MGT5200_ETH + 0x1D8 */
	volatile u_int32 rdes_data1;			/* MGT5200_ETH + 0x1DC */
	volatile u_int32 r_length;				/* MGT5200_ETH + 0x1E0 */
	volatile u_int32 x_length;				/* MGT5200_ETH + 0x1E4 */
	volatile u_int32 x_addr;				/* MGT5200_ETH + 0x1E8 */
	volatile u_int32 cdes_data;				/* MGT5200_ETH + 0x1EC */
	volatile u_int32 status;				/* MGT5200_ETH + 0x1F0 */
	volatile u_int32 dma_control;			/* MGT5200_ETH + 0x1F4 */
	volatile u_int32 des_cmnd;				/* MGT5200_ETH + 0x1F8 */
	volatile u_int32 data;					/* MGT5200_ETH + 0x1FC */

    /*  MIB COUNTERS (Offset 200-2FF) */

	volatile u_int32 rmon_t_drop;			/* MGT5200_ETH + 0x200 */
	volatile u_int32 rmon_t_packets;		/* MGT5200_ETH + 0x204 */
	volatile u_int32 rmon_t_bc_pkt;			/* MGT5200_ETH + 0x208 */
	volatile u_int32 rmon_t_mc_pkt;			/* MGT5200_ETH + 0x20C */
	volatile u_int32 rmon_t_crc_align;		/* MGT5200_ETH + 0x210 */
	volatile u_int32 rmon_t_undersize;		/* MGT5200_ETH + 0x214 */
	volatile u_int32 rmon_t_oversize;		/* MGT5200_ETH + 0x218 */
	volatile u_int32 rmon_t_frag;			/* MGT5200_ETH + 0x21C */
	volatile u_int32 rmon_t_jab;			/* MGT5200_ETH + 0x220 */
	volatile u_int32 rmon_t_col;			/* MGT5200_ETH + 0x224 */
	volatile u_int32 rmon_t_p64;			/* MGT5200_ETH + 0x228 */
	volatile u_int32 rmon_t_p65to127;		/* MGT5200_ETH + 0x22C */
	volatile u_int32 rmon_t_p128to255;		/* MGT5200_ETH + 0x230 */
	volatile u_int32 rmon_t_p256to511;		/* MGT5200_ETH + 0x234 */
	volatile u_int32 rmon_t_p512to1023;		/* MGT5200_ETH + 0x238 */
	volatile u_int32 rmon_t_p1024to2047;	/* MGT5200_ETH + 0x23C */
	volatile u_int32 rmon_t_p_gte2048;		/* MGT5200_ETH + 0x240 */
	volatile u_int32 rmon_t_octets;			/* MGT5200_ETH + 0x244 */
	volatile u_int32 ieee_t_drop;			/* MGT5200_ETH + 0x248 */
	volatile u_int32 ieee_t_frame_ok;		/* MGT5200_ETH + 0x24C */
	volatile u_int32 ieee_t_1col;			/* MGT5200_ETH + 0x250 */
	volatile u_int32 ieee_t_mcol;			/* MGT5200_ETH + 0x254 */
	volatile u_int32 ieee_t_def;			/* MGT5200_ETH + 0x258 */
	volatile u_int32 ieee_t_lcol;			/* MGT5200_ETH + 0x25C */
	volatile u_int32 ieee_t_excol;			/* MGT5200_ETH + 0x260 */
	volatile u_int32 ieee_t_macerr;			/* MGT5200_ETH + 0x264 */
	volatile u_int32 ieee_t_cserr;			/* MGT5200_ETH + 0x268 */
	volatile u_int32 ieee_t_sqe;			/* MGT5200_ETH + 0x26C */
	volatile u_int32 t_fdxfc;				/* MGT5200_ETH + 0x270 */
	volatile u_int32 ieee_t_octets_ok;		/* MGT5200_ETH + 0x274 */

	volatile u_int32 RES9[2];				/* MGT5200_ETH + 0x278-27C */
	volatile u_int32 rmon_r_drop;			/* MGT5200_ETH + 0x280 */
	volatile u_int32 rmon_r_packets;		/* MGT5200_ETH + 0x284 */
	volatile u_int32 rmon_r_bc_pkt;			/* MGT5200_ETH + 0x288 */
	volatile u_int32 rmon_r_mc_pkt;			/* MGT5200_ETH + 0x28C */
	volatile u_int32 rmon_r_crc_align;		/* MGT5200_ETH + 0x290 */
	volatile u_int32 rmon_r_undersize;		/* MGT5200_ETH + 0x294 */
	volatile u_int32 rmon_r_oversize;		/* MGT5200_ETH + 0x298 */
	volatile u_int32 rmon_r_frag;			/* MGT5200_ETH + 0x29C */
	volatile u_int32 rmon_r_jab;			/* MGT5200_ETH + 0x2A0 */

	volatile u_int32 rmon_r_resvd_0;		/* MGT5200_ETH + 0x2A4 */

	volatile u_int32 rmon_r_p64;			/* MGT5200_ETH + 0x2A8 */
	volatile u_int32 rmon_r_p65to127;		/* MGT5200_ETH + 0x2AC */
	volatile u_int32 rmon_r_p128to255;		/* MGT5200_ETH + 0x2B0 */
	volatile u_int32 rmon_r_p256to511;		/* MGT5200_ETH + 0x2B4 */
	volatile u_int32 rmon_r_p512to1023;		/* MGT5200_ETH + 0x2B8 */
	volatile u_int32 rmon_r_p1024to2047;	/* MGT5200_ETH + 0x2BC */
	volatile u_int32 rmon_r_p_gte2048;		/* MGT5200_ETH + 0x2C0 */
	volatile u_int32 rmon_r_octets;			/* MGT5200_ETH + 0x2C4 */
	volatile u_int32 ieee_r_drop;			/* MGT5200_ETH + 0x2C8 */
	volatile u_int32 ieee_r_frame_ok;		/* MGT5200_ETH + 0x2CC */
	volatile u_int32 ieee_r_crc;			/* MGT5200_ETH + 0x2D0 */
	volatile u_int32 ieee_r_align;			/* MGT5200_ETH + 0x2D4 */
	volatile u_int32 r_macerr;				/* MGT5200_ETH + 0x2D8 */
	volatile u_int32 r_fdxfc;				/* MGT5200_ETH + 0x2DC */
	volatile u_int32 ieee_r_octets_ok;		/* MGT5200_ETH + 0x2E0 */

	volatile u_int32 RES10[6];				/* MGT5200_ETH + 0x2E4-2FC */

	volatile u_int32 RES11[64];				/* MGT5200_ETH + 0x300-3FF */
} MGT5200_FEC_REGS;



#define MGT5200_FEC_ECNTRL_RESET         0x00000001  /* Reset FEC  */
#define MGT5200_FEC_ECNTRL_ENABLE        0x00000002  /* Enable FEC */
#define MGT5200_FEC_ECNTRL_OE            0x00000004  /* FEC output enable */


#define MGT5200_FEC_R_CTNTRL_MAX_FL			0x05EE0000  /* 1518 */
#define MGT5200_FEC_R_CTNTRL_FCE			0x00000020  /* flow control enable */
#define MGT5200_FEC_R_CTNTRL_BC_REJ			0x00000010  /* broadcast fram e reject */
#define MGT5200_FEC_R_CTNTRL_PROM			0x00000008	/* promiscuous mode */
#define MGT5200_FEC_R_CTNTRL_MII_MODE		0x00000004
#define MGT5200_FEC_R_CTNTRL_DRT			0x00000002  /* disable receive on transmit */
#define MGT5200_FEC_R_CTNTRL_LOOP			0x00000001

#define	MGT5200_FEC_T_CTNTRL_GTS			0x00000001
#define	MGT5200_FEC_T_CTNTRL_HBC			0x00000002
#define	MGT5200_FEC_T_CTNTRL_FDEN			0x00000004
#define	MGT5200_FEC_T_CTNTRL_TFC_PAUSE		0x00000008
#define	MGT5200_FEC_T_CTNTRL_RFC_PAUSE		0x00000010


#define MGT5200_FEC_FIFO_CNTRL_FRAME    0x08000000  /* Frame mode enable */
#define MGT5200_FEC_FIFO_CNTRL_GR       0x04000000  /* Default Granularity value = 4 */
#define MGT5200_FEC_FIFO_CNTRL_TXW_MASK 0x00040000  /* must be set for Rev.B prozessor - undocumented */


/*
 * bit definitions for FEC interrupts
 */
#define MGT5200_FEC_INTR_HBERR		0x80000000	/* heartbeat error				*/
#define MGT5200_FEC_INTR_BABR		0x40000000	/* babbling receive error		*/
#define MGT5200_FEC_INTR_BABT		0x20000000	/* babbling transmit error		*/
#define MGT5200_FEC_INTR_GRA		0x10000000	/* graceful stop complete		*/
#define MGT5200_FEC_INTR_TFINT		0x08000000	/* transmit frame interrupt		*/
#define MGT5200_FEC_INTR_MII		0x00800000	/* MII interrupt				*/
#define MGT5200_FEC_INTR_LATE_COL	0x00200000	/* late collision				*/
#define MGT5200_FEC_INTR_COL_RETRY	0x00100000	/* collision retry limit		*/
#define MGT5200_FEC_INTR_XFIFO_UN	0x00080000	/* transmit FIFO error			*/
#define MGT5200_FEC_INTR_XFIFO_ERR	0x00040000	/* transmit FIFO error			*/
#define MGT5200_FEC_INTR_RFIFO_ERR	0x00020000	/* receive FIFO error			*/



/* Ethernet parameter RAM */
typedef struct {
	u_int32 tbdBase;
	u_int32 tbdNext;
	u_int32 rbdBase;
	u_int32 rbdNext;
} MGT5200_FEC_PRAM;

/* Ethernet Receive & Transmit Buffer Descriptor definitions */
typedef struct {
	volatile u_int16 status;
	volatile u_int16 len;
	volatile u_int32 addr;
} MGT5200_FEC_BD;

/* RBD bits definitions */
#define MGT5200_FEC_RBD_EMPTY  0x8000	/* Buffer is empty */
#define MGT5200_FEC_RBD_WRAP   0x2000	/* Last BD in ring */
#define MGT5200_FEC_RBD_INT    0x1000	/* Interrupt */
#define MGT5200_FEC_RBD_LAST   0x0800	/* Buffer is last in frame(useless) */
#define MGT5200_FEC_RBD_MISS   0x0100	/* Miss bit for prom mode */
#define MGT5200_FEC_RBD_BC     0x0080	/* The received frame is broadcast
										   frame */
#define MGT5200_FEC_RBD_MC     0x0040	/* The received frame is multicast
										   frame */
#define MGT5200_FEC_RBD_LG     0x0020	/* Frame length violation */
#define MGT5200_FEC_RBD_NO     0x0010	/* Nonoctet align frame */
#define MGT5200_FEC_RBD_SH     0x0008	/* Short frame, FEC does not
										   support SH and this bit is
										   always cleared */
#define MGT5200_FEC_RBD_CR     0x0004	/* CRC error */
#define MGT5200_FEC_RBD_OV     0x0002	/* Receive FIFO overrun */
#define MGT5200_FEC_RBD_TR     0x0001	/* The receive frame is truncated */
#define MGT5200_FEC_RBD_ERR    (MGT5200_FEC_RBD_LG  | \
                                MGT5200_FEC_RBD_NO  | \
                                MGT5200_FEC_RBD_CR  | \
                                MGT5200_FEC_RBD_OV  | \
                                MGT5200_FEC_RBD_TR)

/* TBD bits definitions */
#define MGT5200_FEC_TBD_READY  0x8000	/* Buffer is ready */
#define MGT5200_FEC_TBD_WRAP   0x2000	/* Last BD in ring */
#define MGT5200_FEC_TBD_INT    0x1000	/* Interrupt */
#define MGT5200_FEC_TBD_LAST   0x0800	/* Buffer is last in frame */
#define MGT5200_FEC_TBD_TC     0x0400	/* Transmit the CRC */
#define MGT5200_FEC_TBD_ABC    0x0200	/* Append bad CRC */

/* FEC transmit watermark settings */
#define MGT5200_FEC_X_WMRK_64	0x0	/* or 0x1 */
#define MGT5200_FEC_X_WMRK_128	0x2
#define MGT5200_FEC_X_WMRK_192	0x3


/* MII-related definitios */
#define MGT5200_FEC_MII_DATA_ST       0x40000000	/* Start of frame delimiter */
#define MGT5200_FEC_MII_DATA_OP_RD    0x20000000	/* Perform a read operation */
#define MGT5200_FEC_MII_DATA_OP_WR    0x10000000	/* Perform a write operation */
#define MGT5200_FEC_MII_DATA_PA_MSK   0x0f800000	/* PHY Address field mask */
#define MGT5200_FEC_MII_DATA_RA_MSK   0x007c0000	/* PHY Register field mask */
#define MGT5200_FEC_MII_DATA_TA       0x00020000	/* Turnaround */
#define MGT5200_FEC_MII_DATA_DATAMSK  0x0000fff	/* PHY data field */

#define MGT5200_FEC_MII_DATA_RA_SHIFT 0x12	/* MII Register address bits */
#define MGT5200_FEC_MII_DATA_PA_SHIFT 0x17	/* MII PHY address bits */

#define MGT5200_FEC_MIB_DISABLE		  0x80000000  /* set before clear MIB counters */

/*----------------------------------------------------------------------
 * BestComm registers (located at MGT5200_SDMA)
 */
typedef struct {
	volatile u_int32 taskBar;			/* MGT5200_SDMA + 0x00 sdTpb */
	volatile u_int32 currentPointer;	/* MGT5200_SDMA + 0x04 sdMdeComplex */
	volatile u_int32 endPointer;		/* MGT5200_SDMA + 0x08 sdMdeComplex */
	volatile u_int32 variablePointer;	/* MGT5200_SDMA + 0x0c sdMdeComplex */

	volatile u_int8 intVect1;	/* MGT5200_SDMA + 0x10 sdPtd */
	volatile u_int8 intVect2;	/* MGT5200_SDMA + 0x11 sdPtd */
	volatile u_int16 ptdCntrl;	/* MGT5200_SDMA + 0x12 sdPtd */

	volatile u_int32 intPend;	/* MGT5200_SDMA + 0x14 sdPtd */
	volatile u_int32 intMask;	/* MGT5200_SDMA + 0x18 sdPtd */

	volatile u_int32 tcr01;		/* MGT5200_SDMA + 0x1c sdPtd */
	volatile u_int32 tcr23;		/* MGT5200_SDMA + 0x20 sdPtd */
	volatile u_int32 tcr45;		/* MGT5200_SDMA + 0x24 sdPtd */
	volatile u_int32 tcr67;		/* MGT5200_SDMA + 0x28 sdPtd */
	volatile u_int32 tcr89;		/* MGT5200_SDMA + 0x2c sdPtd */
	volatile u_int32 tcrAB;		/* MGT5200_SDMA + 0x30 sdPtd */
	volatile u_int32 tcrCD;		/* MGT5200_SDMA + 0x34 sdPtd */
	volatile u_int32 tcrEF;		/* MGT5200_SDMA + 0x38 sdPtd */

	volatile u_int8 ipr0;		/* MGT5200_SDMA + 0x3c sdPtd */
	volatile u_int8 ipr1;		/* MGT5200_SDMA + 0x3d sdPtd */
	volatile u_int8 ipr2;		/* MGT5200_SDMA + 0x3e sdPtd */
	volatile u_int8 ipr3;		/* MGT5200_SDMA + 0x3f sdPtd */
	volatile u_int8 ipr4;		/* MGT5200_SDMA + 0x40 sdPtd */
	volatile u_int8 ipr5;		/* MGT5200_SDMA + 0x41 sdPtd */
	volatile u_int8 ipr6;		/* MGT5200_SDMA + 0x42 sdPtd */
	volatile u_int8 ipr7;		/* MGT5200_SDMA + 0x43 sdPtd */
	volatile u_int8 ipr8;		/* MGT5200_SDMA + 0x44 sdPtd */
	volatile u_int8 ipr9;		/* MGT5200_SDMA + 0x45 sdPtd */
	volatile u_int8 ipr10;		/* MGT5200_SDMA + 0x46 sdPtd */
	volatile u_int8 ipr11;		/* MGT5200_SDMA + 0x47 sdPtd */
	volatile u_int8 ipr12;		/* MGT5200_SDMA + 0x48 sdPtd */
	volatile u_int8 ipr13;		/* MGT5200_SDMA + 0x49 sdPtd */
	volatile u_int8 ipr14;		/* MGT5200_SDMA + 0x4a sdPtd */
	volatile u_int8 ipr15;		/* MGT5200_SDMA + 0x4b sdPtd */
	volatile u_int8 ipr16;		/* MGT5200_SDMA + 0x4c sdPtd */
	volatile u_int8 ipr17;		/* MGT5200_SDMA + 0x4d sdPtd */
	volatile u_int8 ipr18;		/* MGT5200_SDMA + 0x4e sdPtd */
	volatile u_int8 ipr19;		/* MGT5200_SDMA + 0x4f sdPtd */
	volatile u_int8 ipr20;		/* MGT5200_SDMA + 0x50 sdPtd */
	volatile u_int8 ipr21;		/* MGT5200_SDMA + 0x51 sdPtd */
	volatile u_int8 ipr22;		/* MGT5200_SDMA + 0x52 sdPtd */
	volatile u_int8 ipr23;		/* MGT5200_SDMA + 0x53 sdPtd */
	volatile u_int8 ipr24;		/* MGT5200_SDMA + 0x54 sdPtd */
	volatile u_int8 ipr25;		/* MGT5200_SDMA + 0x55 sdPtd */
	volatile u_int8 ipr26;		/* MGT5200_SDMA + 0x56 sdPtd */
	volatile u_int8 ipr27;		/* MGT5200_SDMA + 0x57 sdPtd */
	volatile u_int8 ipr28;		/* MGT5200_SDMA + 0x58 sdPtd */
	volatile u_int8 ipr29;		/* MGT5200_SDMA + 0x59 sdPtd */
	volatile u_int8 ipr30;		/* MGT5200_SDMA + 0x5a sdPtd */
	volatile u_int8 ipr31;		/* MGT5200_SDMA + 0x5b sdPtd */

	volatile u_int32 res1;		/* reserved */
	volatile u_int32 res2;		/* reserved */
	volatile u_int32 res3;		/* reserved */
	volatile u_int32 mdeDebug;	/* MGT5200_SDMA + 0x68 sdMdeComplex */
	volatile u_int32 adsDebug;	/* MGT5200_SDMA + 0x6c sdAdsTop */
	volatile u_int32 value1;	/* MGT5200_SDMA + 0x70 sdDbg */
	volatile u_int32 value2;	/* MGT5200_SDMA + 0x74 sdDbg */
	volatile u_int32 control;	/* MGT5200_SDMA + 0x78 sdDbg */
	volatile u_int32 status;	/* MGT5200_SDMA + 0x7c sdDbg */
	volatile u_int32 eu00;		/* MGT5200_SDMA + 0x80 sdMac macer reg */
	volatile u_int32 eu01;		/* MGT5200_SDMA + 0x84 sdMac macemr reg */
	volatile u_int32 eu02;		/* MGT5200_SDMA + 0x88 unused */
	volatile u_int32 eu03;		/* MGT5200_SDMA + 0x8c unused */
	volatile u_int32 eu04;		/* MGT5200_SDMA + 0x90 unused */
	volatile u_int32 eu05;		/* MGT5200_SDMA + 0x94 unused */
	volatile u_int32 eu06;		/* MGT5200_SDMA + 0x98 unused */
	volatile u_int32 eu07;		/* MGT5200_SDMA + 0x9c unused */
	volatile u_int32 eu10;		/* MGT5200_SDMA + 0xa0 unused */
	volatile u_int32 eu11;		/* MGT5200_SDMA + 0xa4 unused */
	volatile u_int32 eu12;		/* MGT5200_SDMA + 0xa8 unused */
	volatile u_int32 eu13;		/* MGT5200_SDMA + 0xac unused */
	volatile u_int32 eu14;		/* MGT5200_SDMA + 0xb0 unused */
	volatile u_int32 eu15;		/* MGT5200_SDMA + 0xb4 unused */
	volatile u_int32 eu16;		/* MGT5200_SDMA + 0xb8 unused */
	volatile u_int32 eu17;		/* MGT5200_SDMA + 0xbc unused */
	volatile u_int32 eu20;		/* MGT5200_SDMA + 0xc0 unused */
	volatile u_int32 eu21;		/* MGT5200_SDMA + 0xc4 unused */
	volatile u_int32 eu22;		/* MGT5200_SDMA + 0xc8 unused */
	volatile u_int32 eu23;		/* MGT5200_SDMA + 0xcc unused */
	volatile u_int32 eu24;		/* MGT5200_SDMA + 0xd0 unused */
	volatile u_int32 eu25;		/* MGT5200_SDMA + 0xd4 unused */
	volatile u_int32 eu26;		/* MGT5200_SDMA + 0xd8 unused */
	volatile u_int32 eu27;		/* MGT5200_SDMA + 0xdc unused */
	volatile u_int32 eu30;		/* MGT5200_SDMA + 0xe0 unused */
	volatile u_int32 eu31;		/* MGT5200_SDMA + 0xe4 unused */
	volatile u_int32 eu32;		/* MGT5200_SDMA + 0xe8 unused */
	volatile u_int32 eu33;		/* MGT5200_SDMA + 0xec unused */
	volatile u_int32 eu34;		/* MGT5200_SDMA + 0xf0 unused */
	volatile u_int32 eu35;		/* MGT5200_SDMA + 0xf4 unused */
	volatile u_int32 eu36;		/* MGT5200_SDMA + 0xf8 unused */
	volatile u_int32 eu37;		/* MGT5200_SDMA + 0xfc unused */
} MGT5200_SDMA_REGS;

/*--- Macros for smartcomm ---*/

/* Disable SmartDMA task */
#define MGT5200_SDMA_TASK_DISABLE(mbar,tasknum)                     \
{                                                      \
    volatile u_int16 *tcr = (u_int16 *) (mbar+MGT5200_SDMA+0x0000001c+2*(tasknum)); \
    *tcr = (*tcr) & (~0x8000);                         \
}

/* Enable SmartDMA task */
#define MGT5200_SDMA_TASK_ENABLE(mbar,tasknum)                      \
{                                                      \
    volatile u_int16 *tcr = (u_int16 *) (mbar+MGT5200_SDMA+0x0000001c+2*(tasknum)); \
    *tcr = (*tcr)  | 0x8040;                           \
}

/* Enable interrupt */
#define MGT5200_SDMA_INT_ENABLE(mbar,tasknum)                       \
{                                                      \
    MGT5200_SDMA_REGS* sdma = (MGT5200_SDMA_REGS*)(mbar+MGT5200_SDMA); \
    sdma->intMask &= ~(1 << (tasknum));                  \
}

/* Disable interrupt */
#define MGT5200_SDMA_INT_DISABLE(mbar,tasknum)   \
{                                                      \
    MGT5200_SDMA_REGS* sdma = (MGT5200_SDMA_REGS*)(mbar+MGT5200_SDMA); \
    sdma->intMask |= (1 << (tasknum));                   \
}


/* Clear interrupt pending bits */
#define MGT5200_SDMA_CLEAR_IEVENT(mbar,tasknum)  \
{                                                      \
    MGT5200_SDMA_REGS* sdma = (MGT5200_SDMA_REGS*)(mbar+MGT5200_SDMA); \
    sdma->intPend = (1 << (tasknum));                    \
}

/* get interupt pending bit of a task */
#define MGT5200_SDMA_GET_PENDINGBIT(mbar,tasknum)                   \
        ((*(volatile u_int32 *)(mbar + MGT5200_SDMA + 0x14)) & (1<<(tasknum)))

/* get interupt mask bit of a task */
#define MGT5200_SDMA_GET_MASKBIT(mbar,tasknum)                      \
        ((*(volatile u_int32 *)(mbar + MGT5200_SDMA + 0x18)) & (1<<(tasknum)))

#endif /* _ASMLANGUAGE */


#endif /* _MGT5200_H */


















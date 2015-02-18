/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: pci9050.h
 *
 *       Author: ds
 *        $Date: 1999/09/08 09:30:31 $
 *    $Revision: 2.1 $
 *
 *  Description: Register and bit definitions for PCI9050 controller
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: pci9050.h,v $
 * Revision 2.1  1999/09/08 09:30:31  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef PCI9050_H_
#define PCI9050_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
/*
 *  registers locations
 */
/* Local Address Space 0-3 Bus Region Descriptors */
#define RR_BUSREG(mSlot)	( 0x28 + ((mSlot)*4) )

#define RR_INTCTL			0x4c		/* interrupt control/status register */
#define RR_MISC				0x50		/* Misc. Control                     */
#define RR_SIZE				0x54		/* size of runtime register          */

/*
 * bit definitions
 */
/* ---------- Bus Region Descriptors ---------- */
#define BUSREG_BUSWIDTH         0x00C00000  /* Bus_width (2 bits) */
#define BUSREG_BUSWIDTH_D16     0x00400000  /* Bus_width = D16    */
#define BUSREG_BUSWIDTH_D32		0x00800000  /* Bus_width = D32    */

/* --- Interrupt Control/Status Register  ---- */
#define INTCTL_LINT1_ENABLE     0x00000001	/* LINT1_en */
#define INTCTL_LINT2_ENABLE     0x00000008	/* LINT2_en */
#define INTCTL_LINT_ENABLE      0x00000040	/* LINT_en  */

/* ---------- Misc. Control Register ---------- */
#define MISC_IO0_MODE           0x00000001
#define MISC_IO0_DIR            0x00000002
#define MISC_IO0_DATA           0x00000004

#define MISC_IO1_MODE           0x00000008
#define MISC_IO1_DIR            0x00000010
#define MISC_IO1_DATA           0x00000020

#define MISC_IO2_MODE           0x00000040
#define MISC_IO2_DIR            0x00000080
#define MISC_IO2_DATA           0x00000100

#define MISC_IO3_MODE           0x00000200
#define MISC_IO3_DIR            0x00000400
#define MISC_IO3_DATA           0x00000800

#define MISC_CLK_EPR            0x01000000
#define MISC_CS_EPR             0x02000000
#define MISC_TX_EPR             0x04000000
#define MISC_RX_EPR             0x08000000
#define MISC_VAL_EPR            0x10000000

#define MISC_RELOAD             0x20000000
#define MISC_SOFTRES            0x40000000

#ifdef __cplusplus
    }
#endif

#endif /* PCI9050_H_ */


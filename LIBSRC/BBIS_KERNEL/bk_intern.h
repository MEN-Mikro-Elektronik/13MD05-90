/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bk_intern.h
 *
 *       Author: kp
 *        $Date: 2013/10/24 10:02:18 $
 *    $Revision: 1.7 $
 *
 *  Description: MDIS4LINUX BBIS kernel internal header file
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bk_intern.h,v $
 * Revision 1.7  2013/10/24 10:02:18  ts
 * R: system.h include not longer needed (deprecated)
 * M: removed include
 *
 * Revision 1.6  2012/08/08 19:59:12  ts
 * R: IOAPIC address might vary depending on the linux distro
 * M: made address evaluation of IOAPIC flexible, cleaned up defines for SC24
 *
 * Revision 1.5  2012/08/07 18:57:21  ts
 * R: 1. Mapping of SC24 FPGA into LPC space required pciset commands on init
 *    2. SC24 FPGA IRQ is connected to IRQ14 with default type edge, needs level
 * M: 1. added automatic PCI config access to map FPGA if SC24 is found
 *    2. added IRQ type adjustment for chameleon FPGA if SC24 is found
 *
 * Revision 1.4  2006/08/02 11:03:58  ts
 * removed include of <linux/segment.h>
 *
 * Revision 1.3  2004/06/09 09:07:44  kp
 * 2.6 enhancements
 *
 * Revision 1.2  2002/05/31 15:10:07  kp
 * include slab.h
 *
 * Revision 1.1  2001/01/19 14:37:59  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/kernel.h> 	/* printk() */
#include <asm/uaccess.h> 	/* copy_to_user */
#include <linux/errno.h>  	/* error codes */
#include <linux/types.h>  	/* size_t */
#include <linux/fcntl.h>    /* O_ACCMODE */
#include <linux/kmod.h>
#include <linux/slab.h> 	/* kmalloc() */
#include <linux/pci.h>
#include <linux/fs.h>

#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/desc.h>
#include <MEN/maccess.h>
#include <MEN/bb_defs.h>
#include <MEN/bb_entry.h>
#include <MEN/bbis_bk.h>
#include <MEN/mdis_err.h>

/* Defines */

#define DBG_MYLEVEL bk_dbglevel
#define DBH			G_dbh
#define OSH			G_osh

#define BK_MAX_DRVNAME	39		/* maximum length of BB driver name */

/*
 * special patch for SC24 to map LPC space and adjust IRQ type
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
# define PCI_FIND_FUNC pci_get_device
#else
# define PCI_FIND_FUNC pci_find_device
#endif

#define FPGA_ADDR_SC24_LPC 		0xe000e000
#define PCI_DEV_ATI_LPC 		0x439d	/* not present in earlier pci_ids.h */

/* index in APIC for IRQ 14 setting */
#define	IOAPIC_DATA_OFFS		0x10
#define IOAPIC_IRQ_TO_REG(x)	(((x)<<1)+0x10)
#define IOAPIC_IRQ_POLARITY  	(1<<13)	/* interrupt pin polarity: 0=high active, 1=low active */
#define IOAPIC_IRQ_TRIGMODE  	(1<<15)	/* interrupt pin trigger mode: 0=edge, 1=level */

/* control registers in LPC controller PCI configuration space  */
#define SC24_LPC_CTRL_DEV		0x14
#define SC24_LPC_CTRL_FCT		0x03

#define SC24_LPC_CTRL_REG		0x48
#define SC24_LPC_ADDR_REG		0x60
#define SC24_LPC_ENABLE			0x27
#define SC24_LPC_FPGA_HIBYTE	0xe0

/* macros to lock global BBIS sempahore */
#define BK_LOCK(err)		\
 err=OSS_SemWait(OSH,G_bkLockSem,OSS_SEM_WAITFOREVER)

#define BK_UNLOCK OSS_SemSignal(OSH,G_bkLockSem)

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/* driver node structure */
typedef struct {
	OSS_DL_NODE node;			  /* node in registered drivers list */
	char drvName[BK_MAX_DRVNAME+1]; /* driver name */
	void (*getEntry)(BBIS_ENTRY *); /* GetEntry function ptr */
	struct module	*module;		/* LL driver linux module structure */
} BK_DRV;

/* BBIS device structure */
typedef struct {
	OSS_DL_NODE node;			  /* node in registered devices list */
	char		    devName[BK_MAX_DEVNAME+1]; /* device name */
    int				useCount;		/* number of opens */

	OSS_HANDLE		*osh;			/* bb driver's OSS handle */

	/* bb driver */
	BK_DRV			*drv;			/* driver structure */
	BBIS_HANDLE		*bb;			/* bb driver's handle */
} BK_DEV;


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/



/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l-serflash.c
 *
 *      \author  Adam Wujek CERN
 *
 *  	 \brief  Driver to read Boards Status via Serial flash indirect interface
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; version
 *	2 of the License.
 *
 */
/*---------------------------------------------------------------------------
 * (c) Copyright 2017 CERN
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

#include <linux/version.h>
/* #if !(defined AUTOCONF_INCLUDED) && (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)) */
/*  #include <linux/config.h> */
/* #endif */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */

#include <MEN/men_chameleon.h>
#include <MEN/serflash.h>
#include "vme4l-core.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/* r/w macros using virtual address instead of offsets (e.g. for SRAM regs) */
#define VME_GENREG_READ32(vaddr)	(readl((char *)vaddr))
#define VME_GENREG_WRITE32(vaddr, val)	(writel(val, (char *)vaddr))

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/** structure to maintain variable used to ioremap bridge regs etc. */
typedef struct {
	unsigned long phys;		/**< phys. address  */
	unsigned long size;		/**< size of region  */
	void *vaddr;			/**< mapped virt. address  */
	int memReq;			/**< flag memory has been requested  */
	int cache;			/**< region cacheing flags (0=no cache)  */
} VME4L_RESRC;

/** bridge drivers private data */
typedef struct {
	CHAMELEONV2_UNIT_T * chu;	/**< chameleon unit for vme control registers  */
	VME4L_RESRC sfii;		/**< serial flash indirect interface, to get status of loading FPGA */
} VME4L_SERFLASH_HANDLE;



/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static VME4L_SERFLASH_HANDLE G_bHandle;

static const u16 G_devIdArr[] = {
	CHAMELEON_16Z126_SERFLASH,
	CHAMELEONV2_DEVID_END
};

static int vme4l_probe(CHAMELEONV2_UNIT_T *chu);
static int vme4l_remove(CHAMELEONV2_UNIT_T *chu);

static CHAMELEONV2_DRIVER_T G_driver = {
    .name     = "men-vme4l",
    .devIdArr = G_devIdArr,
    .probe    = vme4l_probe,
    .remove   = vme4l_remove
};

static int debug = DEBUG_DEFAULT;  /**< enable debug printouts */

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Enable debugging printouts (default " \
			M_INT_TO_STR(DEBUG_DEFAULT) ")");

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/


/*******************************************************************/
/** check mem region/request it and ioremap it
 */
static int MapRegSpace(VME4L_RESRC *res, const char *name)
{
	res->vaddr = NULL;

	if (!request_mem_region(res->phys, res->size, name)) {
		return -EBUSY;
	}

	res->vaddr = ioremap_nocache( res->phys, res->size );
	VME4LDBG("serflash: MapRegSpace %s: phys:%p, size: 0x%x, vaddr=%p\n",
		 name, res->phys, res->size, res->vaddr );

	if(res->vaddr == NULL) {
		release_mem_region(res->phys, res->size);
		return -ENOMEM;
	}
	res->memReq = 1;

	return 0;
}

/*******************************************************************/
/** free ioremapped mem region
 */
static void FreeRegSpace(VME4L_RESRC *res)
{
	if(res->vaddr)
		iounmap(res->vaddr);

	if(res->memReq)
		release_mem_region(res->phys, res->size);
}


static int vme4l_probe(CHAMELEONV2_UNIT_T *chu)
{
	int rv;
	VME4L_SERFLASH_HANDLE *h = &G_bHandle;
	CHAMELEONV2_UNIT_T u;
	int fpga_image_status = SFII_BSR_BS_INV;
	char *fpga_image_status_str = NULL;

	printk(KERN_INFO "vme4l_probe: probing serflash unit\n");

	/* save chameleon unit */
	h->chu = chu;

	/* gather all the other chameleon units. Only A25 has 8 units, A21 and others 7 */
	if (men_chameleonV2_unit_find( CHAMELEON_16Z126_SERFLASH, 0, &u) != 0) {
			printk(KERN_ERR "Did not find serial flash unit %d\n");
		rv = -EINVAL;
		goto CLEANUP;
	}

	printk(KERN_INFO "vme-serflash-cham: found serial flash unit 16Z126_SERFLASH (rev %d)\n",
	       chu->unitFpga.revision);

	h->sfii.phys = (unsigned long)chu->unitFpga.addr + SFII_SPACE;
	h->sfii.size = SFII_SIZE;

	if( (rv = MapRegSpace( &h->sfii, "serflash-sfii" )) )
		goto CLEANUP;

	fpga_image_status = VME_GENREG_READ32(h->sfii.vaddr + SFII_BSR) & SFII_BSR_BS_MASK;

	switch (fpga_image_status) {
	case SFII_BSR_BS_FALLBACK:
		fpga_image_status_str = "FPGA is programmed with the FPGA Fallback Image and no configuration error has occurred";
		break;
	case SFII_BSR_BS_IMG_OK:
		fpga_image_status_str = "FPGA is programmed with the primary FPGA Image";
		break;
	case SFII_BSR_BS_IMG_ERR:
		fpga_image_status_str = "FPGA has returned to FPGA Fallback Image after a configuration error";
		break;
	case SFII_BSR_BS_INV:
	default:
		fpga_image_status_str = "FPGA Fallback Image and no configuration error has occurred";
		break;
	}

	if (fpga_image_status != SFII_BSR_BS_IMG_OK)
		printk(KERN_INFO "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", fpga_image_status);

	printk(KERN_INFO "vme-serflash-cham: %s (board_status=%d, last_reconf_trigger_con=%d)\n",
	       fpga_image_status_str, fpga_image_status,
	       (VME_GENREG_READ32(h->sfii.vaddr + SFII_BSR) & SFII_BSR_LRTC_MASK) >> SFII_BSR_LRTC_SHIFT);

	if (fpga_image_status != SFII_BSR_BS_IMG_OK)
		printk(KERN_INFO "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", fpga_image_status);

	if (rv != 0)
		goto CLEANUP;

	return rv;

 CLEANUP:
	printk(KERN_ERR "vme-serflash-cham: Init error %d\n", -rv);

	FreeRegSpace(&h->sfii);

	return rv;
}

static int __init vme4l_serflash_init_module(void)
{
	printk(KERN_INFO "%s ", __FUNCTION__);

	if (!men_chameleonV2_register_driver(&G_driver))
		return -ENODEV;  /* couldnt find requested unit */
	else
		return 0;
}

static void __exit vme4l_serflash_cleanup_module(void)
{
	men_chameleonV2_unregister_driver(&G_driver);
}

static int vme4l_remove(CHAMELEONV2_UNIT_T *chu)
{
	VME4L_SERFLASH_HANDLE *h = &G_bHandle;
	printk(KERN_DEBUG "vme4l_serflash_cleanup_module\n");

	FreeRegSpace(&h->sfii);

	return 0;

}

module_init(vme4l_serflash_init_module);
module_exit(vme4l_serflash_cleanup_module);

MODULE_AUTHOR("Adam Wujek <adam.wujek@cern.ch>");
MODULE_DESCRIPTION("16Z126_SERFLASH driver");
MODULE_LICENSE("GPL");
#ifdef MAK_REVISION
MODULE_VERSION(MENT_XSTR(MAK_REVISION));
#endif

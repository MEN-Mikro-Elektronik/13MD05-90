/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  men_chameleon.c
 *
 *      \author  kp/ub/ts
 *        $Date: 2013/08/29 16:49:09 $
 *    $Revision: 1.34 $
 *
 *        \brief MEN Chameleon FPGA driver/device registration
 *
 * Derived from men_chameleon.c contained in EM04A Linux BSP by Klaus Popp.
 *
 * Registers each chameleon FPGA found to the PCI subsystem.
 * Provides a driver registration/probe interface similar to the PCI
 * subsystem.
 *
 * Module parameters:
 * usePciIrq : if 1, use the IRQ from the PCI config table for all
 *             devices in the FPGA - default
 *             if 0 use the IRQ from the Chameleon table. Can be different
 *             for every device. Currently makes sense with EM04/A only.
 *
 * Switches: MEN_CHAMV2_IRQ_OFFSET - needed for onboard chameleon FPGAs
 *                                   with IRQs connected to external CPU
 *                                   IRQs instead of PCI interrupt.
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 *-------------------------------[ History ]---------------------------------
 *
 * ------------- from here on maintained in Git -------------------------
 *
 * $Log: men_chameleon.c,v $
 * Revision 1.34  2013/08/29 16:49:09  ts
 * R: 1. qualifier __devinitdata doesnt exist from 3.8.0 on
 *    2. chameleon IP core variant not taken from chameleon table
 * M: 1. undefine qualifier depending on kernel version
 *    2. also copy IP core variant
 *
 * Revision 1.33  2011/04/14 17:36:26  CRuff
 * R: 1. support of pci domains
 *    2. cosmetics
 * M: 1. in pci_init_one, get the corresponding pci domain to the given bus
 *       number and hand it over to the chameleon library
 *    2. added casts of pci_resource_start to U_INT32_OR_64 before casting it
 *       to a pointer
 *
 * Revision 1.32  2011/02/17 15:18:34  CRuff
 * R: 1. support for kernel versions up to 2.6.37
 * M: 1a) include linux/semaphore.h from kernel 1.6.35
 *    1b) use DEFINE_SEMAPHORE instead of DECLARE_MUTEX from kernel 2.6.36
 *
 * Revision 1.31  2010/10/19 11:59:24  CRuff
 * R: Symbol exports are still needed
 * M: undid changes of revision 1.30
 *
 * Revision 1.30  2010/08/23 17:27:38  CRuff
 * R: 1. chameleon lib is now built as core library (ALL_CORE_LIBS)
 *    2. cosmetics
 * M: 1. removed symbol exports for kernel-builtin module; symbols are now
 *       exported in any case in chameleonv2.c
 *    2. takeover of module description from nat_module.c (is not longer used)
 *
 * Revision 1.29  2010/01/27 20:02:42  rt
 * R: 1) Cosmetics.
 * M: 1) Added debug prints to men_chameleonV2_register_driver().
 *
 * Revision 1.28  2010/01/25 16:27:40  rt
 * R: 1) Add support for onboard FPGAs with IRQs connected to external
 *       CPU IRQs instead of PCI IRQs.
 * M: 1) Added MEN_CHAMV2_IRQ_OFFSET parameter.
 *
 * Revision 1.27  2009/08/19 18:06:15  CRuff
 * R: CHAM_InitIo not available on PPC systems
 * M: call CHAM_InitMem on PPC systems, regardless of the cham table mapping
 *
 * Revision 1.26  2009/08/19 17:05:00  CRuff
 * R: pci_device_id(): variable declaration masked out by define
 * M: move variable declaration out of #ifndef block
 *
 * Revision 1.25  2009/08/19 11:02:07  CRuff
 * R: 1. support I/O mapped chameleon tables
 *    2. double termination of cham func table in error case
 * M: 1a) remove CHAM_INIT defines (done in chameleon.h)
 *    1b) for PCI devices, initialize the Cham table as I/O or Mem mapped,
 *        dependent on the BAR mapping
 *    2. remove Term() on cham func table (already done by InitPci)
 *
 * Revision 1.24  2009/02/06 15:02:24  GLeonhardt
 * R:1. header for sysparam not found
 * M:1. change path for include
 *
 * Revision 1.23  2009/01/27 15:48:38  ts
 * R: EXPORT() calls for chameleon-functions in case of kernel builtin driver were missing
 * M: merged latest changes from rt and added symbol exports for CHAM_InitMem,
 *    CHAM_DevIdToName, CHAM_ModCodeToDevId, CHAM_DevIdToModCode
 *
 * Revision 1.4  2008/09/09 11:53:00  acs
 * PR#1923-elinos: Added latest update from MEN (Mr. Schnuerer).
 *
 * Revision 1.22  2008/12/02 18:10:30  rt
 * R:1.FPGA interrupts not working at EM3/EM9/A17
 * M:1.Set usePciIrq=0 at EM3/EM9/A17 since no PCI interrupts available
 *
 * Revision 1.21  2008/09/05 15:22:12  ts
 * R: 1. ElinOS 4.2 EM1 BSP didnt work for a Customer with Framebuffer support,
 *       because only 16 IPcores per FPGA were seeked in ElinOS 4.2
 * M: 1. merged changes from MEN and Sysgo to last recent, updated Sysgo with
 *       this Version
 *
 * Revision 1.20  2008/08/27 17:38:23  aw
 * R: chameleon base address was written false to / false read from flash
 * M: use %X instead of %d
 *
 * Revision 1.19  2008/06/27 12:12:52  aw
 * R: Base address of chameleon table was needed
 * M: Added SysParamGet to get address from system parameter
 *
 * Revision 1.18  2008/03/13 16:58:08  aw
 * initialize pdev with a initial value at case no pci
 *
 * Revision 1.17  2008/03/12 10:51:21  aw
 * changed define NO_PCI to CONFIG_PCI
 *
 * Revision 1.16  2008/02/22 14:48:28  aw
 * bugfix, last checkin changed moduleparam perms to 666 now its 0664 again
 *
 * Revision 1.15  2008/02/21 15:16:42  aw
 * supports NIOS
 *
 * Revision 1.14  2007/12/10 12:11:06  ts
 * module param permissions changed to 0664
 * using pci_driver_register() instead pci_init_module() from 2.6.22 on
 *
 * Revision 1.2  2007/11/21 16:34:33  con
 * PR#1850: Added support for EM1N and chameleon V2 driver.
 *
 * Revision 1.13  2007/10/24 14:15:33  ts
 * changed:
 * search through chamtable until end marker found, not only max. 16 cores/FPGA
 *
 * Revision 1.12  2007/10/08 13:53:10  ts
 * added seamless Chameleon V2 support
 * dump all found Units unconditionally now, not only if DBG defined
 * removed unnecessary oss_slot_pci_devnbrs[] array, already present in oss.c
 * to be used in men_lx_chameleon.ko module and also as BSP built-in driver
 *
 * Revision 1.11  2007/05/08 14:05:46  ts
 * Cosmetics
 *
 * Revision 1.10  2007/03/28 15:55:12  ts
 * Added support for V2 Tables by calling ChameleonV2 functions
 *
 * Revision 1.8  2007/01/10 18:40:05  ts
 * use correct Macro KERNEL_VERSION
 *
 * Revision 1.7  2007/01/09 12:41:29  ts
 * MODULE_PARM is gone in Kernels > 2.6.14
 * make Parameter usePciIrq = 1 the default
 *
 * Revision 1.6  2005/07/22 10:50:40  ts
 * Bugfix in men_chameleon_find_unit: really copy the found unit data to *unit
 *
 * Revision 1.5  2005/01/25 15:59:23  ub
 * added support for new Chameleon table format (magic word 0xcdef)
 *
 * Revision 1.4  2005/01/05 15:58:34  ub
 * Adapted for Linux kernel 2.6
 *
 * Revision 1.3  2004/12/13 12:30:29  ub
 * cosmetics
 *
 * Revision 1.2  2004/11/29 11:34:19  ub
 * Fixed: mem/io region is now released at driver shutdown
 *
 * Revision 1.1  2004/11/23 09:33:57  ub
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004-2009 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

#include <MEN/men_chameleon.h>
#include <MEN/oss.h>
#include <MEN/chameleon.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
#include <linux/semaphore.h>
#endif

/* __devinit qualifiers are removed */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
# define __devinit
# define __devinitdata
#endif

#define STR_HELPER(x) 		#x
#define M_INT_TO_STR(x) 	STR_HELPER(x)
#define CHAM_LX_DBG_PREFIX 	printk

#ifdef DBG
#define DEBUG_DEFAULT 1
#else
#define DEBUG_DEFAULT 0
#endif

#define CHAMLXDBG(fmt, args...) \
	do { \
		if (debug) { \
			CHAM_LX_DBG_PREFIX( KERN_DEBUG fmt, ## args ); \
		} \
	} while (0)

/* length of Cham. table file */
#define CHAM_TBL_FILE_LEN    12

/* module parameters */
static int usePciIrq = 1; /* true for all ESMs except EM8, ... */

module_param( usePciIrq, int, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( usePciIrq, "usePciIrq=1: IRQ# from PCI header. usePciIrq=0: Use IP Core IRQ#");

static int debug = DEBUG_DEFAULT;  /**< enable debug printouts */

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Enable debugging printouts (default " \
			M_INT_TO_STR(DEBUG_DEFAULT) ")");

/*--------------------------------------+
 |   DEFINES & CONSTS                    |
 +--------------------------------------*/
#define CFGTABLE_READWORD(tbl,idx)   (h->ioMapped ? \
    inw((u16)((u32)tbl + (idx)*4)) : readw((u16*)((u32)tbl + (idx)*4)))

#define NR_CHAM_TBL_ATTRS	4	/**< sysfs files per table: fpgafile,model,revision,magic */
#define NR_CHAM_IPCORE_ATTRS	10	/**< sysfs files per IP core: (Unit),devId,Grp,Rev,Var,Inst,IRQ,BAR,Offset,Addr (Unit is derived from devID) */
#define CHAM_SYSFS_MODE		0644 	/**< sysfs attributs access mode */
#define CHAM_TBL_DFLT_LEN	24	/**< sysfs attribute default string length */
#define CHAM_TBL_UNIT_LEN	32	/**< string length for IP core Unit names */

/* similar exists in sysfs.h already but we need a different syntax */
#define CHAM_ATTR_SET(atr,attname,mod,showfct,storefct) \
	atr.attr.name 	= attname; \
	atr.attr.mode 	= mod;  \
	atr.show 	= showfct; \
	atr.store 	= storefct;	/* set to NULL in this driver, the sysfs exports are readonly */

/*
 * IP cores per FPGA (v0Unit[] and v2Unit[] arrays), mind that by Spec its
 * unlimited! TODO: use chained List instead v0Unit[] and v2Unit[] !
 */
#define CORES_PER_FPGA		 	256

enum attribute_indices {
        ATTR_OFS_UNIT=0,
	ATTR_OFS_DEVID,
	ATTR_OFS_GRP,
	ATTR_OFS_REV,
	ATTR_OFS_VAR,
	ATTR_OFS_INST,
	ATTR_OFS_IRQ,
	ATTR_OFS_BAR,
	ATTR_OFS_OFF,
	ATTR_OFS_ADDR
};

/** data structure providing sysfs entries for one IP core within the table */
typedef struct {
	struct list_head node; /**< node in list of IP core sysfs entries 	*/
	struct kobject *ipCoreObj; /* parent node for single IP core folder */
        struct kobj_attribute attr_ip[NR_CHAM_IPCORE_ATTRS];
	struct attribute *ipCoreAttrs[NR_CHAM_IPCORE_ATTRS+1]; /* the above incl. NULL term. */
	struct attribute_group ipCoreAttrGrp;
	u32 sysattr[NR_CHAM_IPCORE_ATTRS];
	void *addr;
} CHAM_IPCORE_SYSFS_T;

/** data structure that handles one instance of a chameleon FPGA */
typedef struct {
	struct list_head node; 		/**< node in list of chameleon FPGAs 		*/
	struct list_head ipcores; 	/**< head of list of CHAM_IPCORE_SYSENTRY's 	*/
	struct pci_dev *pdev; 		/**< corresponding pci device 			*/
	int numUnits; 			/**< number of chameleon units of this FPGA 	*/
	void *cfgTbl; 			/**< mapped config table in bar0 		*/
	u32 cfgTblPhys; 		/**< phys addr of config table in bar0 		*/
	int ioMapped; 			/**< config table in io-mapped bar 		*/
	char variant;			/**< usually 'A', the table model 		*/
	int revision;
	int chamNum;
	CHAMELEON_UNIT_T v0Unit[CORES_PER_FPGA];
	CHAMELEONV2_UNIT_T v2Unit[CORES_PER_FPGA];
	/* kobject and sysfs attributes for each table */
	struct kobject *chamTblObj; 	/* parent node for chameleon table sysfs entries */
        struct kobj_attribute attr_cham[NR_CHAM_TBL_ATTRS];
	struct attribute *chamTblAttrs[NR_CHAM_TBL_ATTRS+1]; /* the above incl. NULL term. */
	struct attribute_group chamTblAttrGrp;
	char fpgafile[CHAM_TBL_FILE_LEN];
	char revstr[CHAM_TBL_DFLT_LEN];
	char magic[CHAM_TBL_DFLT_LEN];
} CHAMELEON_HANDLE_T;

static int G_chamInit = 0; 		/**< men_chameleon_init was called  	*/
static LIST_HEAD( G_chamLst); 		/**< list of chameleon FPGAs 		*/
static LIST_HEAD( G_drvLst); 		/**< list of registered drivers 	*/
static LIST_HEAD( G_drvV2Lst); 		/**< list of registered V2 drivers  	*/
static CHAM_FUNCTBL G_chamFctTable; 	/**< Chameleon function table       	*/
static struct device *G_cham_devs;  	/**< base node for sysfs entries  	*/

/* helpers for sysfs attribute names */
static const char *G_sysChamTblAttrname[NR_CHAM_TBL_ATTRS] = { "fpga_file","model","revision", "magic" };
static const char *G_sysIpCoreAttrname[NR_CHAM_IPCORE_ATTRS] = { "Unit", "devId", "Grp", "Rev", "Var", "Inst", "IRQ", "BAR", "Offset", "Addr" };

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static DEFINE_SEMAPHORE(cham_probe_sem);
#else
static DECLARE_MUTEX(cham_probe_sem);
#endif

static void cham_probe_lock(void)
{
	down(&cham_probe_sem);
}

static void cham_probe_unlock(void)
{
	up(&cham_probe_sem);
}

/*******************************************************************/
/** Probes driver if it can handle the new unit
 *
 * - does nothing if the unit has already a driver attached
 * - checks if the unit's module code is to be handled by \a drv
 * - if so, calls the driver's probe function
 * - on success, assigns the driver to the unit
 *
 * \return 1 if \a drv assigned to unit, 0 if not
 */
static int chameleon_announce(CHAMELEON_UNIT_T *unit, CHAMELEON_DRIVER_T *drv)
{
	const u16 *modCodeP = drv->modCodeArr;
	int rv = 0;

	if(unit->driver)
		return 0; /* already a driver attached */

	while(*modCodeP != CHAMELEON_MODCODE_END) {
		if(*modCodeP == unit->modCode) {

			/* code match, call driver probe */
			cham_probe_lock();
			if(drv->probe(unit) >= 0) {
				unit->driver = drv;
				rv = 1;
				cham_probe_unlock();
				break;
			}
			cham_probe_unlock();
		}
		modCodeP++;
	}
	return rv;
}

/*************************************************************************/
/** Probes driver if it can handle the new unit
 *
 * - does nothing if the unit has already a driver attached
 * - checks if the unit's module code is to be handled by \a drv
 * - if so, calls the driver's probe function
 * - on success, assigns the driver to the unit
 *
 * \return 1 if \a drv assigned to unit, 0 if not
 */
static int chameleonV2_announce(CHAMELEONV2_UNIT_T *unit,
		CHAMELEONV2_DRIVER_T *drv)
{
	const u16 *devIdP = drv->devIdArr;
	int rv = 0;

	if(unit->driver)
		return 0; /* already a driver attached */

	while(*devIdP != CHAMELEONV2_DEVID_END) {
		if(*devIdP == unit->unitFpga.devId) {

			/* code match, call driver probe */
			cham_probe_lock();
			if(drv->probe(unit) >= 0) {
				unit->driver = drv;
				rv = 1;
				cham_probe_unlock();
				break;
			}
			cham_probe_unlock();
		}
		devIdP++;
	}
	return rv;
}

/*******************************************************************/
/** probes all units of new FPGA against all registered drivers
 *
 */
static void chameleon_announce_fpga(CHAMELEON_HANDLE_T *h)
{
	struct list_head *pos;
	int i;

	for(i = 0; i < h->numUnits; i++) {
		list_for_each(pos, &G_drvLst)
		{
			CHAMELEON_DRIVER_T *drv = list_entry(pos,
					CHAMELEON_DRIVER_T, node);

			chameleon_announce(&h->v0Unit[i], drv);
		}
		list_for_each(pos, &G_drvV2Lst)
		{
			CHAMELEONV2_DRIVER_T *drv = list_entry(pos,
					CHAMELEONV2_DRIVER_T, node);

			chameleonV2_announce(&h->v2Unit[i], drv);
		}
	}
}

/*******************************************************************/
/** register a new chameleon driver
 *
 * Adds the driver structure to the list of registered drivers.
 * Immediately checks if any known FPGA unit can be handled by the
 * new driver and calls it probe() function if so.
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 *
 * \return the number of chameleon units which were claimed by the driver
 *          during registration.  The driver remains registered even if the
 *          return value is zero.
 */
int men_chameleon_register_driver(CHAMELEON_DRIVER_T *drv)
{
	struct list_head *pos;
	int i;
	int count = 0;
	CHAMELEON_HANDLE_T *h;

	list_add_tail(&drv->node, &G_drvLst);

	list_for_each(pos, &G_chamLst)
	{
		h = list_entry(pos, CHAMELEON_HANDLE_T, node);

		for(i = 0; i < h->numUnits; i++) {
			if(chameleon_announce(&h->v0Unit[i], drv)) {
				count++;
			}
		}
	}
	return count;
}

/*******************************************************************/
/** register a new chameleon V2 driver
 *
 * Adds the driver structure to the list of registered drivers.
 * Immediately checks if any known FPGA unit can be handled by the
 * new driver and calls it probe() function if so.
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 *
 * \return the number of chameleon units which were claimed by the driver
 *          during registration.  The driver remains registered even if the
 *          return value is zero.
 */
int men_chameleonV2_register_driver(CHAMELEONV2_DRIVER_T *drv)
{
	struct list_head *pos;
	int i;
	int count = 0;

	list_add_tail(&drv->node, &G_drvV2Lst);

	list_for_each(pos, &G_chamLst)
	{
		CHAMELEON_HANDLE_T *h = list_entry(pos, CHAMELEON_HANDLE_T,
				node);

		for(i = 0; i < h->numUnits; i++) {
			CHAMLXDBG("reg_driver, devId: %d\n", h->v2Unit[i].unitFpga.devId);
			if(chameleonV2_announce(&h->v2Unit[i], drv)) {
				CHAMLXDBG("\t-->\t accepted\n");
				count++;
			}
		}
	}
	return count;
}

/*******************************************************************/
/** unregister a chameleon driver
 *
 * Deletes the driver structure from the list of registered chameleon drivers,
 * gives it a chance to clean up by calling its remove() function for
 * each device it was responsible for, and marks those devices as
 * driverless.
 *
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 */
void men_chameleon_unregister_driver(CHAMELEON_DRIVER_T *drv)
{
	struct list_head *pos;
	int i;

	list_del(&drv->node);

	list_for_each(pos, &G_chamLst)
	{
		CHAMELEON_HANDLE_T *h = list_entry(pos, CHAMELEON_HANDLE_T,
				node);

		for(i = 0; i < h->numUnits; i++) {
			if(h->v0Unit[i].driver == drv) {
				if(drv->remove) {
					drv->remove(&h->v0Unit[i]);
				}
				h->v0Unit[i].driver = NULL;
				h->v0Unit[i].driver_data = NULL;
			}
		}
	}
}

/*******************************************************************/
/** unregister a chameleon V2 driver
 *
 * Deletes the driver structure from the list of registered chameleon drivers,
 * gives it a chance to clean up by calling its remove() function for
 * each device it was responsible for, and marks those devices as
 *
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 */
void men_chameleonV2_unregister_driver(CHAMELEONV2_DRIVER_T *drv)
{
	struct list_head *pos;
	int i;

	list_del(&drv->node);

	list_for_each(pos, &G_chamLst)
	{
		CHAMELEON_HANDLE_T *h = list_entry(pos, CHAMELEON_HANDLE_T,
				node);

		for(i = 0; i < h->numUnits; i++) {
			if(h->v2Unit[i].driver == drv) {
				if(drv->remove) {
					drv->remove(&h->v2Unit[i]);
				}
				h->v2Unit[i].driver = NULL;
				h->v2Unit[i].driver_data = NULL;
			}
		}
	}
}

/*******************************************************************/
/** Find the system wide nth occurrance of a chameleon module \a modCode
 *
 * \param modCode   \IN  module code to search
 * \param idx       \IN  nth occurance of module (system wide!)
 * \param unit      \OUT filled with unit information
 * \return 0 on success or negative linux error number
 */
int men_chameleon_unit_find(int modCode, int idx, CHAMELEON_UNIT_T *unit)
{
	struct list_head *pos;
	int count = 0, i;

	list_for_each(pos, &G_chamLst)
	{
		CHAMELEON_HANDLE_T *h = list_entry(pos, CHAMELEON_HANDLE_T,
				node);
		for(i = 0; i < h->numUnits; i++) {
			if(h->v0Unit[i].modCode == modCode) {
				if(idx == count) {
					/* found, copy unit data */
					memcpy(unit, &(h->v0Unit[i]),
							sizeof(CHAMELEON_UNIT_T));
					return 0;
				}
				count++;
			}
		}
	}
	return -ENODEV;
}

/*******************************************************************/
/** Find the system wide nth occurrance of a chameleon module \a devId
 *
 * \param devId     \IN  device id to search
 * \param idx       \IN  nth occurance of module (system wide!)
 * \param unit      \OUT filled with unit information
 * \return 0 on success or negative linux error number
 */
int men_chameleonV2_unit_find(int devId, int idx, CHAMELEONV2_UNIT_T *unit)
{
	struct list_head *pos;
	int count = 0, i;

	list_for_each(pos, &G_chamLst)
	{
		CHAMELEON_HANDLE_T *h = list_entry(pos, CHAMELEON_HANDLE_T,
				node);
		for(i = 0; i < h->numUnits; i++) {
			if(h->v2Unit[i].unitFpga.devId == devId) {
				if(idx == count) {
					/* found, copy unit data */
					memcpy(unit, &(h->v2Unit[i]),
							sizeof(CHAMELEONV2_UNIT_T));
					return 0;
				}
				count++;
			}
		}
	}
	return -ENODEV;
}

/*******************************************************************/
/**   sysfs read function (writing is not supported)
 *
 * The common sysfs show function searches the list of found
 * chameleon tables and each IP core present in each table. When the
 * matching kobj pointer is found, it's requested attribute files
 * data is returned.
 * the kobj argument can either be one of the 10 per-IP-core
 * attributes or one of the 4 per-cham-table attributes.
 *
 *  \param kobj  kobject of sysfs parent entry (=IP core)
 *  \param attr  kobj_attribute (= &ip->attr_ip[0-9]), the
 *               requested sysfs file per IP core)
 *  \param buf   pointer to which data are to be written
 *  \param size  # of bytes to return
 *
 *  \return      # of written bytes or error code (negative number).
 */
static ssize_t cham_sysfs_read(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	struct list_head *posTbl = NULL;
	struct list_head *posIpcore = NULL;
	struct list_head *tmp1 = NULL, *tmp2 = NULL;
	CHAM_IPCORE_SYSFS_T *ip = NULL;
	CHAMELEON_HANDLE_T *h = NULL;
	int i=0;

	CHAMLXDBG( "->cham_sysfs_read: kobj=%p attr=%p\n", kobj, attr );

	list_for_each_safe(posTbl, tmp1, &G_chamLst)
	{
		/* iterate through all found chameleon tables */
		h = list_entry(posTbl, CHAMELEON_HANDLE_T, node);

		if ( kobj == h->chamTblObj ) {
			CHAMLXDBG( "cham table attribute '%s' requested.\n", attr->attr.name );
			/* kobj points to h->chamTblObj => one of the 4 chameleon table attributes is requested */
			if ( attr == &h->attr_cham[0]) { /* fpga_file */
				return scnprintf( buf, CHAM_TBL_FILE_LEN+1, "%s\n", h->fpgafile );
			} else if ( attr == &h->attr_cham[1]) { /* model */
				return scnprintf( buf, CHAM_TBL_DFLT_LEN, "%c\n", h->variant );
			} else if ( attr == &h->attr_cham[2]) { /* FPGA revision */
				return scnprintf( buf, CHAM_TBL_DFLT_LEN, "%s\n", h->revstr );
			} else if ( attr == &h->attr_cham[3]) { /* magic */
				return scnprintf( buf, CHAM_TBL_DFLT_LEN, "%s\n", h->magic );
			}
		} else {
			/* if kobj != chamTblObj then one of the 10 IP core attributes is requested */
			list_for_each_safe( posIpcore, tmp2, &h->ipcores )
			{
				CHAMLXDBG( "IP core attribute '%s' requested.\n", attr->attr.name );
				ip = list_entry( posIpcore, CHAM_IPCORE_SYSFS_T, node);
				if ( kobj == ip->ipCoreObj ) {
					/* kobj points to ip->ipCoreObj => one of the 10 IP core attributes is requested */
					for ( i=0; i < NR_CHAM_IPCORE_ATTRS; i++) {
						if ( attr == &ip->attr_ip[i] ) {
							/* Unitname and address need special formatting, all other values are simply printed as hex values */
							if ( i == ATTR_OFS_UNIT  ) /* Unit is derived from devID */
								return scnprintf( buf, CHAM_TBL_UNIT_LEN, "%s\n", CHAM_DevIdToName( ip->sysattr[ATTR_OFS_DEVID] ));
							else if ( i == ATTR_OFS_ADDR )
								return scnprintf( buf, CHAM_TBL_DFLT_LEN, "0x%p\n", ip->addr );
							else
								return scnprintf( buf, CHAM_TBL_DFLT_LEN, "0x%x\n", ip->sysattr[i]  );
						}
					}
				}
			}
		}
	}
	return -EIO; /* shouldn't come here but complain at least */
}

/*******************************************************************/
/** Probe/initialize Chameleon FPGA.
 *
 * Gets called (during execution of pci_register_driver for already
 * existing devices or later if a new device gets inserted) for all
 * PCI devices which match the ID table and are not handled by the
 * other drivers yet. This function gets passed a pointer to the
 * pci_dev structure representing the device and also which entry in
 * the ID table did the device match. It returns zero when the driver
 * has accepted the device or an error code (negative number)
 * otherwise.
 *
 *  \param pdev     pci_dev structure representing the device
 *  \param ent      entry in PCI table that matches
 *
 *  \return     0 when the driver has accepted the device or
 *              an error code (negative number) otherwise.
 */
static int __devinit pci_init_one(struct pci_dev *pdev,
		const struct pci_device_id *ent)
{

	OSS_HANDLE *osH = NULL;
	CHAMELEON_HANDLE_T *h = NULL; /* this is the native struct */
	CHAM_IPCORE_SYSFS_T *ip = NULL;
	CHAMELEONV2_UNIT info;
	CHAMELEON_UNIT_T *v0unit;
	CHAMELEONV2_UNIT_T *v2unit;
	CHAMELEONV2_TABLE table;
	CHAMELEONV2_HANDLE *chamHdl;
	char name[21]; /* buffer to keep unit name/index */
	char tblfile[CHAM_TBL_FILE_LEN + 1];
	int rv = -ENOMEM, err = 0, idx, i,j=0;
	int32 chamResult;
	u32 value32;

	if((err = OSS_Init("men_chameleon", &osH))) {
		printk( KERN_ERR "*** Error during OSS_Init!()\n");
		return err;
	}

	/* allocate internal handles */
	if((h = kzalloc(sizeof(*h), GFP_KERNEL)) == NULL)
		goto CLEANUP;

	printk( KERN_INFO "\nFound MEN Chameleon FPGA at bus %d dev %02x\n", pdev->bus->number, pdev->devfn >> 3);

	rv = dma_set_mask(&pdev->dev, DMA_BIT_MASK(32));
	if(rv == 0)
		dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(32));

	if (rv) {
		printk(KERN_ERR "No 32bit DMA support on this CPU, trying 32bit\n" );
		goto CLEANUP;
	} else
	printk(KERN_INFO "setting 32bit DMA support\n" );

	/* initialize Chameleon handle depending on IO or mem mapped BAR0 */
	pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0, &value32);

	if(value32 & PCI_BASE_ADDRESS_SPACE_IO) { /* io mapped */
#ifndef CONFIG_PPC
		chamResult = CHAM_InitIo(&G_chamFctTable);
#else
		/* for PPCs, no I/O Mapping is supported; init as Mem mapped */
		chamResult = CHAM_InitMem( &G_chamFctTable );
#endif
	} else {
		chamResult = CHAM_InitMem(&G_chamFctTable);
	}

	if(chamResult != CHAMELEON_OK) {
		printk(KERN_ERR "*** Error during Chameleon_Init!\n");
		return chamResult;
	}

	/* Initialize Chameleon library */
	chamResult = G_chamFctTable.InitPci(osH, OSS_MERGE_BUS_DOMAIN(pdev->bus->number, pci_domain_nr(pdev->bus)), pdev->devfn >> 3, 0, &chamHdl);
	if(chamResult != CHAMELEON_OK) {
		printk( KERN_ERR "*** Error during Chameleon_Init (InitPci)!\n");
		return chamResult;
	}

	/* Ident Table */
	chamResult = G_chamFctTable.TableIdent(chamHdl, 0, &table);
	h->revision = table.revision;
	h->variant = table.model;

	/* store right aligned chars from table.file[] to normal left aligned tblfile[] and skip leading zeroes */
	for(i = 0, j = 0; i < CHAM_TBL_FILE_LEN; i++) {
		if (table.file[i] != 0) {
			tblfile[j++] = table.file[i];
		}
	}
	tblfile[j] = '\0';

	/* store table info for sysfs read function */
	strncpy( h->fpgafile, tblfile, CHAM_TBL_FILE_LEN );
	snprintf( h->revstr, CHAM_TBL_DFLT_LEN, "%d.%d", table.revision, table.minRevision );
	snprintf( h->magic, CHAM_TBL_DFLT_LEN, "0x%04X", table.magicWord);

	printk( KERN_INFO "Information about the Chameleon FPGA:\n");
	printk( KERN_INFO "FPGA File='%s' table model=0x%02x('%c') Revision %d.%d Magic \n",
			tblfile, table.model, table.model, table.revision,table.minRevision, table.magicWord );
	printk( KERN_INFO " Unit                devId   Grp Rev  Var  Inst IRQ   BAR  Offset       Addr\n");
	printk( "================================================================================\n");

	/* add this table as subdirectory to G_cham_devs and create table info files in it */
	h->chamTblObj = kobject_create_and_add( tblfile,  &G_cham_devs->kobj );

	/* assemble attribute group and populate info entries per chameleon table */
	for ( i=0; i < NR_CHAM_TBL_ATTRS; i++) {
		CHAM_ATTR_SET(h->attr_cham[i],G_sysChamTblAttrname[i],CHAM_SYSFS_MODE,cham_sysfs_read,NULL);
		h->chamTblAttrs[i] = &h->attr_cham[i].attr;
	}
	h->chamTblAttrGrp.attrs = h->chamTblAttrs;

	if ( sysfs_create_group( h->chamTblObj, &h->chamTblAttrGrp )) {
		kobject_put( h->chamTblObj );
		h->chamTblObj = NULL;
	}
	INIT_LIST_HEAD( &h->ipcores);

	/* gather all IP cores until end marker found */
	idx = 0;

	while((chamResult = G_chamFctTable.UnitIdent(chamHdl, idx, &info))
			!= CHAMELEONV2_NO_MORE_ENTRIES) {

		v0unit = &h->v0Unit[idx];
		v2unit = &h->v2Unit[idx];

		/* format Unit index and Name so tab spacing never breaks */
		sprintf(name, "%02d %-17s", idx, CHAM_DevIdToName(info.devId));
		printk(KERN_INFO " %s" /* name 			*/
				"0x%04x %2d   %2d   %2d" /* devId/Group/Rev/Var. */
				"   0x%02x" /* instance 		*/
				"\t0x%02x" /* interrupt 		*/
				"   %d   0x%08x" /* BAR / offset  	*/
				"   0x%p\n", /* addr 			*/
				name, info.devId, info.group, info.revision, info.variant, info.instance, info.interrupt, info.bar, (unsigned int)info.offset, info.addr);

		/* Copy the Units info */
		v0unit->modCode = CHAM_DevIdToModCode(info.devId);
		v0unit->revision = info.revision;
		v0unit->instance = info.instance;
		v0unit->irq = info.interrupt;
		v0unit->bar = info.bar;
		v0unit->offset = info.offset;
		v0unit->phys = (void *)(U_INT32_OR_64)(pci_resource_start(pdev, info.bar) + info.offset);

		v2unit->unitFpga.devId = info.devId;
		v2unit->unitFpga.group 	= info.group;
		v2unit->unitFpga.revision = info.revision;
		v2unit->unitFpga.instance = info.instance;
		v2unit->unitFpga.variant = info.variant;
		v2unit->unitFpga.interrupt = info.interrupt;
		v2unit->unitFpga.bar = info.bar;
		v2unit->unitFpga.offset = info.offset;
		v2unit->unitFpga.size = info.size;
		v2unit->unitFpga.addr =	(void *)(U_INT32_OR_64)(pci_resource_start(pdev, info.bar) + info.offset);

		/* if specified use IRQ given by PCI config space */
		if(usePciIrq) {
			v0unit->irq = pdev->irq;
			v2unit->unitFpga.interrupt = pdev->irq;
		}

		v0unit->pdev = pdev;
		v0unit->chamNum = h->chamNum;
		v2unit->pdev = pdev;
		v2unit->chamNum = h->chamNum;

		/* --- create the set of sysfs entries for that core and hook it to list --- */
		if((ip = kzalloc(sizeof(CHAM_IPCORE_SYSFS_T), GFP_KERNEL)) == NULL)
			goto CLEANUP;

		/* add subdirectory with IP cores name to this table and create table info files in it.
		 * Because multiple IP cores with same name can occur, we add the index as prefix. */
		snprintf( name, sizeof(name), "%02d_%s", idx, CHAM_DevIdToName(info.devId));
		ip->ipCoreObj = kobject_create_and_add( name,  h->chamTblObj );

		/* store values for quick retrieval in sysfs */
		ip->sysattr[ATTR_OFS_DEVID] = info.devId;
		ip->sysattr[ATTR_OFS_GRP] = info.group;
		ip->sysattr[ATTR_OFS_REV] = info.revision;
		ip->sysattr[ATTR_OFS_VAR] = info.variant;
		ip->sysattr[ATTR_OFS_INST] = info.instance;
		ip->sysattr[ATTR_OFS_IRQ] = info.interrupt;
		ip->sysattr[ATTR_OFS_BAR] = info.bar;
		ip->sysattr[ATTR_OFS_OFF] = info.offset;
		ip->addr = v2unit->unitFpga.addr;

		/* assemble attribute group and populate info entries per chameleon table */
		for ( i=0; i < NR_CHAM_IPCORE_ATTRS; i++) {
			CHAM_ATTR_SET(ip->attr_ip[i], G_sysIpCoreAttrname[i],CHAM_SYSFS_MODE,cham_sysfs_read,NULL);
			ip->ipCoreAttrs[i] = &ip->attr_ip[i].attr;
		}
		ip->ipCoreAttrGrp.attrs = ip->ipCoreAttrs;

		if ( sysfs_create_group( ip->ipCoreObj, &ip->ipCoreAttrGrp )) {
			kobject_put( ip->ipCoreObj );
			ip->ipCoreObj = NULL;
		}
		list_add_tail( &ip->node, &h->ipcores );

		idx++;
	}

	h->numUnits = idx;

	list_add_tail(&h->node, &G_chamLst);

	rv = pci_enable_device(pdev);
	if(rv) {
		printk(KERN_ERR "failed to pci_enable_device(). Something is very wrong...\n");
		return -ENODEV;
	}

	/*--------------------------------+
	 |  Inform all registered drivers  |
	 +--------------------------------*/
	chameleon_announce_fpga(h);
	rv = 0;
	return rv;

	CLEANUP: if(h) {
		if(h->cfgTbl)
			iounmap(h->cfgTbl);

		if(h->cfgTblPhys) {
			if(h->ioMapped)
				release_region(h->cfgTblPhys,
						CHAMELEON_CFGTABLE_SZ);
			else
				release_mem_region(h->cfgTblPhys,
						CHAMELEON_CFGTABLE_SZ);
		}
		kfree(h);
	}
	return rv;
}

/*
 * PCI Vendor/Device ID table.
 * Driver will handle all devices that have these codes
 */
static struct pci_device_id G_pci_tbl[] __devinitdata = {{
CHAMELEON_PCI_VENID_ALTERA, 0x5104, PCI_ANY_ID, PCI_ANY_ID}, /* EM04 				*/
{CHAMELEON_PCI_VENID_ALTERA, 0x454d, PCI_ANY_ID, 0x0441}, /* EM04A MEN PCI core	*/
{CHAMELEON_PCI_VENID_ALTERA, 0x0008, PCI_ANY_ID, PCI_ANY_ID}, /* EM07  				*/
{CHAMELEON_PCI_VENID_ALTERA, 0x000a, PCI_ANY_ID, PCI_ANY_ID}, /* F401  				*/
{CHAMELEON_PCI_VENID_ALTERA, 0x000b, PCI_ANY_ID, PCI_ANY_ID}, /* F206  				*/
{CHAMELEON_PCI_VENID_ALTERA, 0x0013, PCI_ANY_ID, PCI_ANY_ID}, /* F206i 				*/
{CHAMELEON_PCI_VENID_ALTERA, 0x0009, PCI_ANY_ID, PCI_ANY_ID}, /* F206 Trainguard 		*/
{CHAMELEON_PCI_VENID_ALTERA, 0x4d45, PCI_ANY_ID, PCI_ANY_ID}, /* Chameleon general ID */
/* care for future devices with new MEN own PCI vendor ID. Look at every PCI device with this ID. */
{CHAMELEON_PCI_VENID_MEN, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID}, {0, }};

static struct pci_driver G_pci_driver = {.name = "men-chameleon", .id_table =
		G_pci_tbl, .probe = pci_init_one, };

int men_chameleon_init(void)
{
	int rv;

	if(G_chamInit)
		return 0; /* already initialized */

	/* the root sysfs entry.. */
	G_cham_devs = root_device_register("men_chameleon");
	if(G_cham_devs == NULL) {
		printk( KERN_INFO "*** couldn't create sysfs entry!\n" );
		return -EIO;
	}

	printk( KERN_INFO "Init MEN Chameleon PNP subsystem\n" );

	if((rv = pci_register_driver(&G_pci_driver)) < 0)
		return rv;

	G_chamInit++;
	return 0;
}

void men_chameleon_cleanup(void)
{
	struct list_head *posTbl = NULL;
	struct list_head *posIpcore = NULL;
	struct list_head *tmp1 = NULL, *tmp2 = NULL; /* separate tmp storage required! */
	CHAM_IPCORE_SYSFS_T *ip = NULL;

	printk( KERN_INFO "men_chameleon_cleanup\n" );

	list_for_each_safe(posTbl, tmp1, &G_chamLst)
	{
		CHAMELEON_HANDLE_T *h = list_entry(posTbl, CHAMELEON_HANDLE_T, node);

		/* free attribute resources of IP cores in this table */
		list_for_each_safe( posIpcore, tmp2, &h->ipcores)
		{
			ip = list_entry( posIpcore, CHAM_IPCORE_SYSFS_T, node);
			CHAMLXDBG( " ip = %p ip->unit: %s\n", ip, ip->ipCoreObj->name );
			sysfs_remove_group( ip->ipCoreObj , &ip->ipCoreAttrGrp );
			kobject_put( ip->ipCoreObj );
			list_del( posIpcore );
			kfree(ip);
		}

		if(h->cfgTblPhys) {
			if(h->ioMapped)
				release_region(h->cfgTblPhys, CHAMELEON_CFGTABLE_SZ);
			else
				release_mem_region(h->cfgTblPhys, CHAMELEON_CFGTABLE_SZ);
		}

		sysfs_remove_group( h->chamTblObj, &h->chamTblAttrGrp );
		kobject_put( h->chamTblObj );
		h->chamTblObj = NULL;
		list_del(posTbl);
		kfree(h);
	}
	G_chamInit--;

	root_device_unregister(G_cham_devs);

	pci_unregister_driver(&G_pci_driver);
}

EXPORT_SYMBOL( men_chameleon_unit_find);
EXPORT_SYMBOL( men_chameleon_register_driver);
EXPORT_SYMBOL( men_chameleon_unregister_driver);
EXPORT_SYMBOL( men_chameleonV2_unit_find);
EXPORT_SYMBOL( men_chameleonV2_register_driver);
EXPORT_SYMBOL( men_chameleonV2_unregister_driver);

/* are we kernel-builtin? export V2 funcs or BBIS drivers cant find them. */
#ifdef CONFIG_MEN_CHAMELEON
EXPORT_SYMBOL(CHAM_InitMem);
EXPORT_SYMBOL(CHAM_DevIdToName);
EXPORT_SYMBOL(CHAM_ModCodeToDevId);
EXPORT_SYMBOL(CHAM_DevIdToModCode);
#endif

MODULE_LICENSE( "GPL" );
module_init( men_chameleon_init);
module_exit( men_chameleon_cleanup);

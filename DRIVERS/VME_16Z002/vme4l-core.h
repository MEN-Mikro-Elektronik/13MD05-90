/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  vme4l-core.h
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2014/07/22 14:15:27 $
 *    $Revision: 1.13 $
 *
 *  	 \brief  VME4L header for shared info between vme4l-core and bridge
 *				 drivers.
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l-core.h,v $
 * Revision 1.13  2014/07/22 14:15:27  ts
 * R: 1. additional API functions for AM setting needed
 *    2. struct for VME4L entries needed in different source files
 * M: 1. added address modifier get/set functions
 *    2. moved struct here from vme4l_core.c
 *
 * Revision 1.12  2009/07/09 10:38:25  rt
 * R: 1.) Large zero-copy DMAs  (>64k at PLDZ002; >408k at TSI148)
 *        read/write wrong data.
 * M: 1.) Changed vmeAddr parameter to pointer to vmeAddr.
 *
 * Revision 1.11  2009/06/03 18:22:35  rt
 * R: 1.) TSI148 needs physical Address to perform RMW cycle
 * M: 1.) physAddr parameter added to rmwCycle()
 *
 * Revision 1.10  2009/04/30 21:46:19  rt
 * R: 1) Support for newer kernels.
 * M: 1) AUTOCONF_INCLUDED switch added.
 *
 * Revision 1.9  2007/12/10 11:59:30  ts
 * define VERSION_CODE_NEW_IRQFLAGS added for new IRQ handler Flags
 *
 * Revision 1.8  2007/05/10 14:49:33  ts
 * Bugfix: Include <linux/devfs_fs_kernel.h> only when CONFIG_DEVFS_FS is set
 *
 * Revision 1.7  2006/09/26 11:01:04  ts
 * adapted for either classic RTAI or Xenomai usage
 *
 * Revision 1.6  2005/08/11 11:32:48  ts
 * define CONFIG_MEN_VME_RTAI_KERNELIF if (MDIS_RTAI_SUPPORT &&
 * CONFIG_MEN_VME_KERNELIF) defined
 *
 * Revision 1.5  2005/01/19 14:13:12  ts
 * reordered some includes from RTAI
 *
 * Revision 1.4  2004/12/04 18:46:33  ts
 * added VME RTAI support
 *
 * Revision 1.3  2004/07/26 16:31:25  kp
 * intermediate alpha release
 * - support for slave windows, mailbox, location
 * - Linux 2.6 support (zerocopy DMA untested!)
 *
 * Revision 1.2  2004/06/09 10:52:08  kp
 * include men_vme_kernelif.h from different paths depend on build environment
 *
 * Revision 1.1  2003/12/15 15:02:10  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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
#if !(defined AUTOCONF_INCLUDED) && (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))
 #include <linux/config.h>
#endif

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h> 	/* kmalloc() 		*/
#include <linux/vmalloc.h> 	/* vmalloc() 		*/
#include <linux/fs.h>     	/* everything...	*/
#include <linux/errno.h>  	/* error codes 		*/
#include <linux/types.h>  	/* size_t.. 		*/
#include <linux/proc_fs.h>
#include <linux/pagemap.h>
#include <linux/sched.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
# include <linux/sched/signal.h>
#endif
#include <linux/ioport.h>
#include <linux/fcntl.h>        /* O_ACCMODE 		*/
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>  /* DMA-API */
#include <linux/scatterlist.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/pci.h>
#include <asm/uaccess.h>        /* put_user */
#include <asm/io.h>

/* include men_vme_kernelif.h depending on build environment */
#ifdef _ONE_NAMESPACE_PER_DRIVER_
/* this is for MDIS build env */
# include <MEN/men_vme_kernelif.h>
#else
# include <linux/men_vme_kernelif.h>
#endif
#include <MEN/vme4l.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/* VME4L_ADRSWIN.flags */
/*#define VME4L_AW_POSTED_WR	0x01*/
#define VME4L_AW_HW_SWAP1	0x02

/* DMA finished pseude irq level */
#define VME4L_IRQLEV_DMAFINISHED	0x80

#ifndef COMPILE_VME_BRIDGE_DRIVER
typedef void *VME4L_BRIDGE_HANDLE;
#endif

#define STR_HELPER(x) #x
#define M_INT_TO_STR(x) STR_HELPER(x)

#define VME_DBG_PREFIX 	printk

#ifdef DBG
#define DEBUG_DEFAULT 1
#else
#define DEBUG_DEFAULT 0
#endif

#define VME4LDBG(fmt, args...) \
	do { \
		if (debug) { \
			VME_DBG_PREFIX( KERN_DEBUG fmt, ## args ); \
		} \
	} while (0)

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef struct {
	dma_addr_t dmaAddress;
	uint32_t dmaLength;
} VME4L_SCATTER_ELEM;

/** Low level VME bridge interface */

typedef struct VME4L_BRIDGE_DRV {

	/***********************************************************************/
    /** Get bridge driver info string
	 *
	 * bridge driver shall sprintf HW and bridge driver revision information
	 * into \a buf (100 bytes max)
	 */
	void (*revisionInfo)( VME4L_BRIDGE_HANDLE *h, char *buf );

	/***********************************************************************/
    /** Request VME master address window
	 *
	 * This requests a VME master window from the VME bridge. The bridge shall
	 * locate an unused window that matches the VME space \a spc and
	 * shall setup the window so that it is opened for at least \a *vmeAddrP
	 * and \a *sizeP.
	 *
	 * The bridge may return however a window with a much larger size than
	 * requested.
	 *
	 * \param h				brigde private handle
	 * \param spc			VME4L space number
	 * \param vmeaddrP		(IN)  contains the requested VME start address
	 *						(OUT) receives the VME start address of the window
	 * \param sizeP			(IN)  contains the requested VME window size
	 *						(OUT) receives the window size
	 * \param physAddrP		(OUT) receives the CPU physical address of
	 *							  the window
	 * \param flags			window flags (VME4L_AW_POSTED_WR).
	 *						Serves as an hint to the bridge driver only.
	 *						Bridge may ignore this flag.
	 * \param bDrvDataP		(OUT) bridge driver may store some window related
	 *							  pointer here.
     *
	 * \return 0 on success, or negative error number:\n
	 *	- -EINVAL if unsupported parameter
	 *  - -EBUSY  if no window available
	 */
	int (*requestAddrWindow)(
		VME4L_BRIDGE_HANDLE *h,
		VME4L_SPACE spc,
		vmeaddr_t *vmeAddrP,
		size_t *sizeP,
		void **physAddrP,
		int flags,
		void **bDrvDataP);

	/***********************************************************************/
    /** Release VME master address window
	 *
	 * This releases a VME window previously requested with
	 * requestAddrWindow().
	 *
	 * \param h				brigde private handle
	 * \param spc			VME4L space number
	 * \param vmeaddr		windows VME start address
	 * \param size			windows VME window size
	 * \param flags			windows flags (VME4L_AW_POSTED_WR).
	 * \param bDrvData		the pointer returned by requestAddrWindow()
     *
	 * \return 0 on success, or negative error number
	 */
	int (*releaseAddrWindow)(
		VME4L_BRIDGE_HANDLE *h,
		VME4L_SPACE spc,
		vmeaddr_t vmeAddr,
		size_t size,
		int flags,
		void *bDrvData);

	/***********************************************************************/
    /** Turn on/off VME irq level or special level (unconditionally)
	 *
	 * VME4L_IRQLEV_BUSERR cannot be controlled with this function.
	 *
	 * \param h				brigde private handle
	 * \param level			VME irq level, see \ref VME4L_IRQLEV
	 * \param set			0=disable level, 1=enable level
	 *
	 * \return 0 on success, or negative error number
	 */
	int (*irqLevelCtrl)(
		VME4L_BRIDGE_HANDLE *h,
		int level,
		int set );

	/***********************************************************************/
    /** Read 8 bit value from master window
	 *
	 * This should perform a byte read from VMEbus and check if
	 * this transfer caused a bus error.
	 *
	 * \param h				brigde private handle
	 * \param vaddr			virtual address pointing to byte to transfer
	 *						into VME master window
	 * \param dataP		   	(OUT) data read
	 * \param flags			not yet used
	 * \param bDrvData		the pointer returned by requestAddrWindow()
	 *
	 * \return 0 on success, or negative error number:\n
	 * - -EIO on bus error
	 */
	int (*readPio8)(
		VME4L_BRIDGE_HANDLE *h,
		void *vaddr,
		uint8_t *dataP,
		int flags,
		void *bDrvData);

	/***********************************************************************/
    /** Read 16 bit value from master window
	 * \sa readPio8
	 */
	int (*readPio16)(
		VME4L_BRIDGE_HANDLE *h,
		void *vaddr,
		uint16_t *dataP,
		int flags,
		void *bDrvData);

	/***********************************************************************/
    /** Read 32 bit value from master window
	 * \sa readPio8
	 */
	int (*readPio32)(
		VME4L_BRIDGE_HANDLE *h,
		void *vaddr,
		uint32_t *dataP,
		int flags,
		void *bDrvData);

	/***********************************************************************/
    /** Write 8 bit value to master window
	 * \sa readPio8
	 */
	int (*writePio8)(
		VME4L_BRIDGE_HANDLE *h,
		void *vaddr,
		uint8_t *dataP,
		int flags,
		void *bDrvData);

	/***********************************************************************/
    /** Write 16 bit value to master window
	 * \sa readPio8
	 */
	int (*writePio16)(
		VME4L_BRIDGE_HANDLE *h,
		void *vaddr,
		uint16_t *dataP,
		int flags,
		void *bDrvData);

	/***********************************************************************/
    /** Write 32 bit value to master window
	 * \sa readPio8
	 */
	int (*writePio32)(
		VME4L_BRIDGE_HANDLE *h,
		void *vaddr,
		uint32_t *dataP,
		int flags,
		void *bDrvData);


	/**********************************************************************/
	/** Get VMEbus address modifier
	 *
	 * (this function is optional and can be NULL)
	 *
	 * \param spc			VME4L space number for this DMA transfer
	 * \param h				brigde private handle
	 */
	int (*addrModifierGet)( VME4L_SPACE spc, VME4L_BRIDGE_HANDLE *h );

	/**********************************************************************/
	/** Set VMEbus address modifier
	 *
	 * (this function is optional and can be NULL)
	 *
	 * \param spc			VME4L space number for this DMA transfer
	 * \param h				brigde private handle
	 * \param addrMod		address modifier for space spc
	 */
	int (*addrModifierSet)( VME4L_SPACE spc, VME4L_BRIDGE_HANDLE *h, char addrMod);


	/***********************************************************************/
    /** Write zero-copy DMA scatter list to DMA controller
	 *
	 * (this function is optional and can be NULL)
	 *
	 * \param h				brigde private handle
	 * \param spc			VME4L space number for this DMA transfer
	 * \param sgList		list with \a sgElems scatter elements
	 * \param sgNelems		number of valid elements in \a sgList
	 * \param direction		0=read from VME 1=write to VME
	 * \param modeFlags		flags for DMA swapping or DMA for non BLTs
	 * \param vmeAddr		(IN) vme start address
	 *				(OUT) next vme start address
	 * \param flags			flags passed from VME4L_Read/Write
	 *
	 * \return >=0 number of scatter elements setup, or negative error number
	 */
	int (*dmaSetup)(
		VME4L_BRIDGE_HANDLE *h,
		VME4L_SPACE spc,
		VME4L_SCATTER_ELEM *sgList,
		int sgNelems,
		int direction,
		int swapMode,
		vmeaddr_t *vmeAddr,
		int flags);

	/***********************************************************************/
	/** Setup DMA for bounce buffer
	 *
	 * This function is used if dmaSetup is NULL.
	 *
	 * DMA bouncing is used whenever the brigde is not able to do a
	 * zero-copy DMA into user space.
	 *
	 * For example, on VME reads, the bounce buffer is filled by DMA
	 * and copied by vme4l-core into user space.
	 *
	 * Bounce buffer must be maintained by bridge driver.
	 *
	 * (this function is optional and can be NULL)
	 *
	 * \param h				brigde private handle
	 * \param spc			VME4L space number for this DMA transfer
	 * \param size			number of bytes to transfer
	 * \param direction		0=read from VME 1=write to VME
	 * \param vmeAddr		vme start address
	 * \param modeFlags		flags for DMA swapping or DMA for non BLTs
	 * \param bounceBufP	(OUT) will receive the virtual address of
	 *						the bounce buffer
	 *
	 * \return >=0 size of bounce buffer, or negative error number
	 */
	int (*dmaBounceSetup)(
		VME4L_BRIDGE_HANDLE *h,
		VME4L_SPACE spc,
		size_t size,
		int direction,
		int swapMode,
		vmeaddr_t vmeAddr,
		void **bounceBufP);

	/***********************************************************************/
    /** Free DMA bounce buffer
	 *
	 * (this function is optional and can be NULL)
	 *
	 * \param h				brigde private handle
     * \param bounceBuf		bounce buffer to be freed
	 *
	 * \return >=0 size of bounce buffer, or negative error number
	 */
	int (*dmaBounceBufRelease)(
		VME4L_BRIDGE_HANDLE *h,
		void *bounceBuf );

	/***********************************************************************/
    /** Start DMA with the scatter list or bounce buffer.
	 *
	 * DMA must have been initialized by dmaSetup or dmaBounceSetup
	 *
	 * \param h				brigde private handle
	 * \return 0 on success or negative error number
	 */
	int (*dmaStart)(
		VME4L_BRIDGE_HANDLE *h);

	/***********************************************************************/
    /** Stop DMA
	 *
	 * \param h				brigde private handle
	 * \return 0 on success or negative error number
	 */
	int (*dmaStop)(
		VME4L_BRIDGE_HANDLE *h);

	/***********************************************************************/
    /** Get DMA status (after DMA was started with dmaStart)
	 *
	 * \param h				brigde private handle
	 * \return 0=DMA finished ok, <0=finished with error >0 DMA running
	 */
	int (*dmaStatus)(
		VME4L_BRIDGE_HANDLE *h);

	/**********************************************************************/
    /** Generate a VMEbus interrupt
	 */
	int (*irqGenerate)(
		VME4L_BRIDGE_HANDLE *h,
		int level,
		int vector);

    /**********************************************************************/
    /** Check if generated interrupt has been acknowledged
	 */
	int (*irqGenAcked)(
		VME4L_BRIDGE_HANDLE *h,
		int id);

    /**********************************************************************/
    /** Clear pending interrupter
	 */
 	int (*irqGenClear)(
		VME4L_BRIDGE_HANDLE *h,
		int id);

	/***********************************************************************/
    /** Check if system controller function has been enabled
	 *	\return 0 if VME bridge is not system controller\n
	 *			1 if VME bridge is system controller\n
	 *			<0 on error
	 */
	int (*sysCtrlFuncGet)(
		VME4L_BRIDGE_HANDLE *h);

    /***********************************************************************/
    /** Enable/Disable system controller function
	 *
	 * \param state	(0=disable,1=enable)
	 */
	int (*sysCtrlFuncSet)(
		VME4L_BRIDGE_HANDLE *h,
		int state);

    /***********************************************************************/
    /** retrieve system IRQ this bridge uses
	 */
	unsigned int (*retrieveSysIrq)(
		VME4L_BRIDGE_HANDLE *h);

    /***********************************************************************/
    /** Generate VME system reset
	 */
	int (*sysReset)(
		VME4L_BRIDGE_HANDLE *h);

    /***********************************************************************/
    /** Check if arbiter has detected a timeout
	 */
	int (*arbToutGet)(
		VME4L_BRIDGE_HANDLE *h,
		int clear);

    /**********************************************************************/
    /** Get information about last VME bus error
	 */
	int (*busErrGet)(
		VME4L_BRIDGE_HANDLE *h,
		int *attrP,
		vmeaddr_t *addrP,
		int clear );

    /**********************************************************************/
    /** Get VMEbus requester mode
	 */
	int (*requesterModeGet)(
		VME4L_BRIDGE_HANDLE *h);

    /**********************************************************************/
    /** Set VMEbus requester mode
	 */
	int (*requesterModeSet)(
		VME4L_BRIDGE_HANDLE *h,
		int state);

    /**********************************************************************/
    /** Set VMEbus requester level
	 */
	int (*requesterLevelSet)(
		VME4L_BRIDGE_HANDLE *h,
		int state);

    /**********************************************************************/
    /** Get VMEbus requester level
	 */
	int (*requesterLevelGet)(
		VME4L_BRIDGE_HANDLE *h);

    /**********************************************************************/
    /** Get slot nr of VME board
	 */
	int (*geoAddrGet)(
		VME4L_BRIDGE_HANDLE *h);

    /***********************************************************************/
    /** Get VMEbus master write mode
     */
	int (*postedWriteModeGet)(
		VME4L_BRIDGE_HANDLE *h);

    /***********************************************************************/
    /** Control VMEbus master write mode
	 */
	int (*postedWriteModeSet)(
		VME4L_BRIDGE_HANDLE *h,
		int state);

    /**********************************************************************/
    /** Perform VME Read-Modify-Write cycle
	 */
	int (*rmwCycle)(
		VME4L_BRIDGE_HANDLE *h,
		void *vaddr,
		void *physAddr,
		int accWidth,
		uint32_t mask,
		uint32_t *rvP);

    /**********************************************************************/
    /** Perform VME Address-Only cycle
	 */
	int (*aOnlyCycle)(
		VME4L_BRIDGE_HANDLE *h,
		void *vAddr );

	/***********************************************************************/
    /** Setup VME slave window
	 *
	 * This sets up a VME slave window for the requested space.
	 *
	 * should setup the VME bridge address comparators so that
	 * other VME master may access the window.
	 *
	 * For bus mastered windows, shall allocate a physically
	 * contiguous buffer and return the phys address of it.
	 *
	 * \param h				brigde private handle
	 * \param spc			VME4L space number
	 * \param vmeaddr		requested VME start address
	 * \param size			requested VME window size. 0=disable window
	 * \param physAddrP	\IN	 address of previous mapping
	 *					\OUT receives the CPU physical address of
	 *						 the window. Either the dedicated RAM address
	 *						 or phys address of pci_alloc_consistent.
	 * \param bDrvDataP	\OUT bridge driver may store some window related
	 *						pointer here.
     *
	 * \return 0 on success, or negative error number:\n
	 *	- -EINVAL if unsupported parameter (size,vmeAddr,alignment)
	 *  - -ENOTTY  window not present
	 */
	int (*slaveWindowCtrl)(
		VME4L_BRIDGE_HANDLE *h,
		VME4L_SPACE spc,
		vmeaddr_t vmeAddr,
		size_t size,
		void **physAddrP,
		void **bDrvDataP);


    /**********************************************************************/
    /** Read mailbox value
	 */
	int (*mboxRead)(
		VME4L_BRIDGE_HANDLE *h,
		int mbox,
		uint32_t *valP);

    /**********************************************************************/
    /** Write mailbox value
	 */
	int (*mboxWrite)(
		VME4L_BRIDGE_HANDLE *h,
		int mbox,
		uint32_t val);

    /**********************************************************************/
    /** Read location monitor reg
	 */
	int (*locMonRegRead)(
		VME4L_BRIDGE_HANDLE *h,
		int reg,
		uint32_t *valP);

    /**********************************************************************/
    /** Write location monitor value
	 */
	int (*locMonRegWrite)(
		VME4L_BRIDGE_HANDLE *h,
		int reg,
		uint32_t val);

      /********************************************************************/
    /** Get struct pci_dev of underlaying VME bridge
     */
        struct pci_dev * (*pciDevGet)(
		VME4L_BRIDGE_HANDLE *h);

	/* leave space for future expansion */
	uint32_t reserved[5];

} VME4L_BRIDGE_DRV;


/** structure that maintains space specific variables */
typedef struct {
	char *devName;				/**< name that appears under /dev  */
	int isSlv;					/**< true if this describes a slave window
								   in this case, isBlt, max* is ignored */
	int isBlt;					/**< is a BLT space */
	vmeaddr_t spcEnd;			/**< VME end addr of this space  */
	int maxWidth;				/**< max accessWidth  */
	struct list_head lstAdrsWins; /**< list of address windows  */
} VME4L_SPACE_ENT;


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
int vme4l_register_bridge_driver( VME4L_BRIDGE_DRV *drv,
								  VME4L_BRIDGE_HANDLE *drvData );
void vme4l_unregister_bridge_driver(void);

void vme4l_irq( int level, int vector, struct pt_regs *regs);

VME4L_SPACE_ENT* vme4l_get_space_ent(unsigned int idx);

VME4L_BRIDGE_HANDLE* vme_bridge_get_handle(void);

VME4L_BRIDGE_DRV* vme_bridge_get_driver(void);

int vme4l_register_client(VME4L_BRIDGE_HANDLE *h);
int vme4l_unregister_client(VME4L_BRIDGE_HANDLE *h);

int vme4l_rw(VME4L_SPACE spc, VME4L_RW_BLOCK *blk, int swapMode);

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l-core.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2013/10/24 10:08:03 $
 *    $Revision: 1.18 $
 *
 *  	 \brief  MENs VME core
 *
 *     Switches: VME4L_MAJOR - major number for vme4l devnodes (default 230)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; version
 *	2 of the License.
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l-core.c,v $
 * Revision 1.18  2013/10/24 10:08:03  ts
 * R: 1. Linux 2.4 and RTAI not longer supported
 *    2. for A21 a different driver source was used
 * M: 1. removed Linux 2.4 and RTAI specific code
 *    2. moved structs from c file to header which is included in other source
 *
 * Revision 1.17  2010/10/22 11:49:28  rt
 * R: 1) DMA may not work if user space buffer is in highmem (check
 *       /proc/meminfo for HighTotal>0).
 * M: 1) Replaced page_address()/virt_to_bus().
 *
 * Revision 1.16  2010/06/29 13:31:59  rt
 * R: 1) After unloading the VME4L module a subsequent modprobe may fail.
 * M: 1) Removed #ifdef CONFIG_DEVFS_FS.
 *
 * Revision 1.15  2009/09/24 10:52:45  CRuff
 * R: 1. compiler warnings on kernel 2.6.28
 * M: 1a) do not use cli() any more on kernels > 2.6.10 (deprecated)
 *    1b) use unsigned long variable to store the interrupt flags in
 *        vme4l_irq()
 *
 * Revision 1.14  2009/07/09 13:56:21  rt
 * R: 1.) Not compilable with kernel 2.5.3..2.6.9.
 *    2.) Support for last address in A64 space.
 *    3.) System may freeze if kernel is compiled with CONFIG_SMP set.
 *    4.) Kernel access of bad area may occur if VME4L_Read/Write is
 *        used with size>0x80000.
 *    5.) Cosmetics
 *    6.) Large zero-copy DMAs  (>64k at PLDZ002; >408k at TSI148)
 *        read/write wrong data.
 * M: 1.a) VME4L_REMAP macro changed.
 *      b) Switch for module_param changed.
 *      c) Switch for send_sig changed.
 *    2.) Changed maxSize to spcEnd in VME4L_SPACE_ENT.
 *    3.) Dead lock in vme4l_discard_adrswin removed.
 *    4.) Determination of useSize in vme4l_make_ioremap_region() changed.
 *    5.) printk("vme4l_rw_pio: ") removed, since not marked as debug printk.
 *    6.) Changed vmeAddr parameter to pointer to vmeAddr in dmaSetup().
 *
 * Revision 1.13  2009/06/03 18:21:54  rt
 * R: 1.) TSI148 needs physical Address to perform RMW cycle
 * M: 1.) physAddr parameter added to rmwCycle()
 *
 * Revision 1.12  2009/04/30 21:57:54  rt
 * R: 1) Support for kernel versions > 2.6.18.
 *    2) Support for TSI148 VME bridge.
 *    3) Wrong debug messages.
 * M: 1) Removed parameter from IRQ handler.
 *    2a) New address spaces added.
 *     b) SMP support fixed.
 *    3) Debug messages reformated, arguments added/removed.
 *
 * Revision 1.11  2009/02/27 14:01:39  rt
 * R:1. Not compilable with kernel 2.6.24.
 * M:1. Fixed permissions parameter at module_param call.
 *
 * Revision 1.10  2007/12/10 11:58:33  ts
 * Cosmetics, some Debug prints added (VME4LDBG)
 *
 * Revision 1.9  2007/05/10 14:48:31  ts
 * Bugfix: Make devfs dependent functions depending on CONFIG_DEVFS_FS
 *
 * Revision 1.8  2007/03/28 16:54:35  ts
 * added vme4l_send_sig() to avoid sending to nonexistent tasks
 *
 * Revision 1.7  2006/09/26 11:00:41  ts
 * adapted for either classic RTAI or Xenomai usage
 * removed remap_page_range()
 *
 * Revision 1.6  2006/05/22 16:04:03  ts
 * remap_page_range is removed in 2.6.11 and up
 *
 * Revision 1.5  2005/01/19 15:41:52  ts
 * corrected DEVFS registering bug under Kernel 2.6
 *
 * Revision 1.4  2005/01/19 14:10:09  ts
 * several bugs corrected, RTAI IRQs are processed properly now.
 *
 * Revision 1.3  2004/12/04 18:27:21  ts
 * added VME RTAI support
 *
 * Revision 1.2  2004/07/26 16:31:23  kp
 * intermediate alpha release
 * - support for slave windows, mailbox, location
 * - Linux 2.6 support (zerocopy DMA untested!)
 *
 * Revision 1.1  2003/12/15 15:02:09  kp
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
#include "vme4l-core.h"
#include <linux/seq_file.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define PFX "vme4l_core: "
#define KERN_ERR_PFX KERN_ERR "!!!" PFX

#ifndef VME4L_MAJOR
# define VME4L_MAJOR			230 /* use 0 to auto-assign major number */
#endif

/* if defined, start of user data from dma transfers is dumped */
#undef VME4L_DBG_DMA_DATA

/** max number of VME/PCI master address windows */
#define VME4L_MAX_ADRS_WINS		16

/** max number of ioremap-cached regions */
#define VME4L_MAX_IOREMAP_CACHE	16

/** VME4L_IRQ_ENTRY.flags for old VME4L compat. */
#define VME4L_IRQ_OLDHANDLER	0x8000

/* irq entry types */
#define VME4L_USER_IRQ			0
#define VME4L_KERNEL_IRQ		1

/** locks interrupt vector&level variables */
#define VME4L_LOCK_VECTORS(ps) 	 spin_lock_irqsave(&G_lockVectTbl, ps)
#define VME4L_UNLOCK_VECTORS(ps) spin_unlock_irqrestore(&G_lockVectTbl, ps)

/** locks DMA irq variables */
#define VME4L_LOCK_DMA(ps) 		spin_lock_irqsave(&G_lockDma, ps)
#define VME4L_UNLOCK_DMA(ps) 	spin_unlock_irqrestore(&G_lockDma, ps)

/** locks address window lists */
#define VME4L_LOCK_MSTRLISTS()	spin_lock(&G_lockMstrLists);
#define VME4L_UNLOCK_MSTRLISTS() spin_unlock(&G_lockMstrLists);

/* page remapping changed to remap_pfn_range - use correct page parameter! */
#define VME4L_REMAP(a,b,c,d,e) remap_pfn_range((a),(b),(c)>>PAGE_SHIFT,(d),(e))

/* Macros to disable hard irqs. */

/* the spin_lock can safely be used on UP and SMP machines */
#ifdef CONFIG_SMP
# define SAVE_FLAGS_AND_CLI(__flags)  spin_lock_irqsave(&G_lockFlags, __flags)
# define RESTORE_FLAGS(__flags)  spin_unlock_irqrestore(&G_lockFlags, __flags)
#else /* UP machine */
# define SAVE_FLAGS_AND_CLI(__flags) local_irq_save(__flags)
# define RESTORE_FLAGS(__flags)       local_irq_restore(__flags)
#endif /* CONFIG_SMP */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/** structure that is kept in file->private_data */
typedef struct {
	int minor;					/**< minor number  */
	int	swapMode;				/**< swapping mode  */
} VME4L_FILE_PRIV;

/** structure that descibes a VME window that is mapped into PCI space */
typedef struct {
	struct list_head node;		/**< list node within spcEnt->lstAdrsWins  */
	VME4L_SPACE spc;			/**< VME space number */
	vmeaddr_t vmeAddr;			/**< VME start address in this space  */
	size_t size;				/**< size of VME window (bytes) */
	void *physAddr;				/**< mapped CPU physical address of window */
	int useCount;				/**< window usage count  */
	int flags;					/**< window flags  */
	struct list_head lstIoremap; /**< cached ioremap regions */
	void *bDrvData;				/**< bridge driver private data  */
} VME4L_ADRSWIN;

/** structure for ioremap cache region
 *
 * ioremap cache is used only for user VME4L_Write()/VME4L_Read() calls
 */
typedef struct {
	struct list_head winNode;	/**< list node within VME4L_ADRSWIN */
	struct list_head cacheNode;	/**< list node within G_lstIoremapCache */
	vmeaddr_t vmeAddr;			/**< VME start address of this region  */
	size_t size;				/**< size of region (bytes) */
	void *vaddr;				/**< ioremapped address */
	int isValid;				/**< true if valid  */
} VME4L_IOREMAP_REGION;


/** structure to maintain registered VME irqs */
typedef struct {
	struct list_head node;		/**< list node */
	int flags;					/**< VME4L_IRQ_ROAK | VME4L_IRQ_ENBL*/
	int entType;				/**< how to interpret following structure  */
	int level;					/**< VME level */
	union {
		/** for entType == VME4L_USER_IRQ */
		struct {
			struct task_struct *task;	/**< where signal will be send */
			struct file *file;			/**< associated file number  */
			int	   signal;              /**< user installed signal */
		} user;

		/** for entType == VME4L_KERNEL_IRQ or VME4L_RTAI_IRQ **/
		struct {
			const char *device;	 		/**< kernel mode device name */
			void *dev_id;				/**< handler's private data */
			union {
                /** Linux kernel handler */
 #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
				void (*lHandler)(int, void *);
 #else
				void (*lHandler)(int, void *, struct pt_regs *);
 #endif /*LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)*/
			} h;
		} kernel;
	} u;
} VME4L_IRQ_ENTRY;


typedef struct DL_NODE {	/* Double linked list node */
	struct DL_NODE *next;
	struct DL_NODE *prev;
} DL_NODE;

typedef struct DL_LIST {	/* Double linked list header */
	DL_NODE *head;
	DL_NODE *tail;
	DL_NODE *tailpred;
} DL_LIST;

/** structure to pass pending interrupts from RT-IRQ-handler
	to Linux-IRQ handler */
typedef struct vme4l_rt_pend_irq {
	DL_NODE	node;
	int vector;			/**< pending interrupt vector (0x100=BERR) */
	int level;			/**< pending interrupt level */
} VME4L_RT_PEND_IRQ;


/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static VME4L_BRIDGE_DRV 	*G_bDrv; 	/**< current bridge driver 		 */
static VME4L_BRIDGE_HANDLE	*G_bHandle;	/**< bridge driver's data  		 */

/** address window pool  */
static VME4L_ADRSWIN		G_adrsWinPool[VME4L_MAX_ADRS_WINS];
static struct list_head		G_freeAdrsWins;

/** ioremap cache */
static VME4L_IOREMAP_REGION	G_ioremapCache[VME4L_MAX_IOREMAP_CACHE];
static struct list_head		G_lstIoremapCache;
static uint32_t				G_ioremapRegSize = 0x100000;

/** spin lock for VME address windows & ioremappings */
static spinlock_t			G_lockMstrLists;
/** spin lock for IRQ vector list */
static spinlock_t			G_lockVectTbl;
/** spin lock for DMA controller */
static spinlock_t			G_lockDma;
/** object to wait for DMA to finish */
static wait_queue_head_t	G_dmaWq;

#ifdef CONFIG_SMP
/** spin lock to SAVE_FLAGS_AND_CLI on SMP machines*/
static spinlock_t			G_lockFlags;
#endif

/** mutex for DMA */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static DEFINE_SEMAPHORE(G_dmaMutex);
#else
DECLARE_MUTEX(G_dmaMutex);
#endif

#ifdef CONFIG_PROC_FS
static struct proc_dir_entry *vme4l_root;
#endif

/** table that maintains space specific variables.
 * indexed by minor number
 */
VME4L_SPACE_ENT G_spaceTbl[] = {
	/* devName         isSlv isBlt  spcEnd                   width */
	{ "vme4l_a16d16"    , 0,  0,    0xFFFF,                    2 },  /* spc 0 */
	{ "vme4l_a24d64_blt", 0,  1,    0xFFFFFF,                  8 },
	{ "vme4l_a16d32"    , 0,  0,    0xFFFF,                    4 },
	{ ""                , 0,  0,    0x0,                       4 }, /* ts@men kept in for now to assure minor numbers in tests etc. are remaining the same */
	{ "vme4l_a24d16",     0,  0,    0xFFFFFF,                  2 },
	{ "vme4l_a24d16_blt", 0,  1,    0xFFFFFF,                  2 },
	{ "vme4l_a24d32",     0,  0,    0xFFFFFF,                  4 },
	{ "vme4l_a24d32_blt", 0,  1,    0xFFFFFF,                  4 },
	{ "vme4l_a32d32",     0,  0,    0xFFFFFFFFULL,             4 },
	{ "vme4l_a32d32_blt", 0,  1,    0xFFFFFFFFULL,             4 },
	{ "vme4l_a32d64_blt", 0,  1,    0xFFFFFFFFULL,             8 },
	{ "vme4l_slave0",     1 },
	{ "vme4l_slave1",     1 },
	{ "vme4l_slave2",     1 },
	{ "vme4l_slave3",     1 },
	{ "vme4l_slave4",     1 },
	{ "vme4l_slave5",     1 },
	{ "vme4l_slave6",     1 },
	{ "vme4l_slave7",     1 },
	{ "vme4l_master0",    0,  0,    ~0,                         8 },
	{ "vme4l_master1",    0,  0,    ~0,                         8 },
	{ "vme4l_master2",    0,  0,    ~0,                         8 },
	{ "vme4l_master3",    0,  0,    ~0,                         8 },
	{ "vme4l_master4",    0,  0,    ~0,                         8 },
	{ "vme4l_master5",    0,  0,    ~0,                         8 },
	{ "vme4l_master6",    0,  0,    ~0,                         8 },
	{ "vme4l_master7",    0,  0,    ~0,                         8 },
	{ "vme4l_a64d32",     0,  0,    0xFFFFFFFFFFFFFFFFULL,      4 },
	{ "vme4l_a64_2evme",  0,  1,    0xFFFFFFFFFFFFFFFFULL,      8 },
	{ "vme4l_a64_2esst",  0,  1,    0xFFFFFFFFFFFFFFFFULL,      8 },
	{ "vme4l_cr_csr",     0,  0,    0xFFFFFF,                   4 } /* spc. 30 */
};

/** number of entries in #G_spaceTbl */
#define VME4L_SPACE_TBL_SIZE (sizeof(G_spaceTbl)/sizeof(VME4L_SPACE_ENT))

VME4L_SPACE_ENT* vme4l_get_space_ent(unsigned int idx)
{
    return idx < VME4L_SPACE_TBL_SIZE ? &G_spaceTbl[idx] : NULL;
}

/** list for each possible VME vector and pseudo vectors */
static struct list_head		G_vectTbl[VME4L_NUM_VECTORS];

/** array to keep track of number of enables/disables for each IRQ level */
static int G_irqLevEnblCount[VME4L_NUM_LEVELS];
static int G_postedWriteMode; 	 /** for old VME4L compat  */
static int major = VME4L_MAJOR;  /**< major device number for /dev/vme4l_xx */

module_param( major, int, 0664 );
MODULE_PARM_DESC(major, "VME4L devices major number");

static int debug = DEBUG_DEFAULT;  /**< enable debug printouts */

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Enable debugging printouts (default " \
			M_INT_TO_STR(DEBUG_DEFAULT) ")");


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int vme4l_send_sig( VME4L_IRQ_ENTRY *ent, int priv);
static void __exit vme4l_cleanup_module(void);
void *vme4l_destroy_ioremap_region( VME4L_IOREMAP_REGION *region );


/***********************************************************************/
/** wrapper to send a user signal
 *
 * Send a signal to a user process who installed it when a VME interrupt
 * occurs. Test in which state the task represented by task_struct *p is.
 *
 * \param ent		\IN	 VME4L_IRQ_ENTRY from G_vectTbl
 * \param sig		\IN	 signal to send
 * \param p			\IN	 task which registered this signal
 * \param priv		\IN  private data
 *
 * \return error code or 0
 */
static int vme4l_send_sig( VME4L_IRQ_ENTRY *ent, int priv)
{

	if (ent->u.user.task->state < EXIT_ZOMBIE )
		return send_sig( ent->u.user.signal, ent->u.user.task, priv);
	else
		return -EINVAL;

}

/***********************************************************************/
/** Enable VME IRQ level or special level
 *
 * The level is enabled only if there are no more pending disables
 *
 * \param level			irq level to enable
 * \return 0 on success or negative error number
 */
static int vme4l_irqlevel_enable( int level )
{
	int rv=0;
	VME4LDBG("vme4l_irqlevel_enable %d\n", level );
	if( (level == VME4L_IRQLEV_UNKNOWN) || (level >= VME4L_NUM_LEVELS) )
		return -EINVAL;

	if( G_irqLevEnblCount[level] == 0 ){
		if( (rv = G_bDrv->irqLevelCtrl( G_bHandle, level, 1 )) == 0 )
			G_irqLevEnblCount[level] = 1;
	}

	return rv;
}

/***********************************************************************/
/** Find address window for VME space and addr
 *
 * Tries to find a VME address window that has been already mapped into
 * PCI space.
 *
 * If found, increments windows useCount.
 *
 * \param spc			VME4L space number
 * \param vmeAddr		requested VME start address
 * \param size			requested VME size
 * \param flags			requested window flags
 *
 * \return window if found or NULL if not found
 */
static VME4L_ADRSWIN *vme4l_find_adrswin(
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	int flags)
{
	struct list_head *pos;
	VME4L_SPACE_ENT *spcEnt = &G_spaceTbl[spc];

	VME4L_LOCK_MSTRLISTS();

	list_for_each( pos, &spcEnt->lstAdrsWins ) {
		VME4L_ADRSWIN *win = list_entry( pos, VME4L_ADRSWIN, node );

		if( (win->vmeAddr <= vmeAddr) &&
			((win->vmeAddr + win->size) >= (vmeAddr+size)) &&
			(win->flags == flags)){
			win->useCount++;
			VME4L_UNLOCK_MSTRLISTS();
			return win;
		}
	}
	VME4L_UNLOCK_MSTRLISTS();
	return NULL;
}


/** Free the resources of an address window.
 *
 * \param win is the window to be freed
 * \param checkUseCount selects whether the use counter should be checked or
 * 	the window should be freed forcibly.
 * \return 0 on success or negative error number
 */
static int do_free_adrswin(VME4L_ADRSWIN *win, bool checkUseCount)
{
	int rv = 0;
	bool lastReference = false;

	VME4L_LOCK_MSTRLISTS();

	if(checkUseCount && win->useCount > 0) {
		--win->useCount;

		/* mark the window as safe to be removed */
		lastReference = (win->useCount == 0);
	}

	if(!checkUseCount || lastReference) {
		rv = G_bDrv->releaseAddrWindow(G_bHandle,
			win->spc, win->vmeAddr, win->size,
			win->flags, win->bDrvData);

		if( !rv ) {
			list_del(&win->node);

			/*--- free all ioremapped regions ---*/
			while(!list_empty(&win->lstIoremap)) {
				VME4L_IOREMAP_REGION *reg =
					list_entry(win->lstIoremap.next,
						VME4L_IOREMAP_REGION, winNode);
				void *va;

				if((va = vme4l_destroy_ioremap_region(reg))) {
					iounmap(va);
				}

				/* move it to end of cache list */
				list_del(&reg->cacheNode);
				list_add_tail(&reg->cacheNode,
						&G_lstIoremapCache);
			}

			list_add_tail(&win->node, &G_freeAdrsWins);
		}
		else
		{
			VME4LDBG("do_free_adrswin failed\n");
		}
	}

	VME4L_UNLOCK_MSTRLISTS();

	return rv;
}


/***********************************************************************/
/** Unconditionally release address window \a win
 *
 * also free all ioremapped regions for that window
 */
static void vme4l_discard_adrswin( VME4L_ADRSWIN *win )
{
	VME4LDBG("vme4l_discard_adrswin spc=%d vmeAddr=0x%llx sz=0x%llx phys=%p "
			 "flg=0x%x\n", win->spc, win->vmeAddr, (uint64_t) win->size,
			 win->physAddr, win->flags);

	do_free_adrswin(win, false);
}

/***********************************************************************/
/** Release address window \a win
 *
 */
static void vme4l_release_adrswin( VME4L_ADRSWIN *win )
{
	VME4LDBG("vme4l_release_adrswin spc=%d vmeAddr=0x%llx sz=0x%llx phys=%p "
			 "flg=0x%x\n", win->spc, win->vmeAddr, (uint64_t) win->size,
			 win->physAddr, win->flags);

	do_free_adrswin(win, true);
}


/***********************************************************************/
/** Free all VME address windows of all spaces whose useCount is 0
 *
 */
static void vme4l_release_unused_adrswins(void)
{
	VME4L_SPACE_ENT *spcEnt = G_spaceTbl;
	VME4L_SPACE spc;

	VME4L_LOCK_MSTRLISTS();

	for( spc=0; spc<VME4L_SPACE_TBL_SIZE; spc++, spcEnt++ ){
		struct list_head *pos, *tmpPos;

		if( spcEnt->isSlv )
			continue;			/* don't free slave windows */

		list_for_each_safe( pos, tmpPos, &spcEnt->lstAdrsWins ) {
			VME4L_ADRSWIN *win = list_entry( pos, VME4L_ADRSWIN, node );

			if( win->useCount == 0 ){
				VME4L_UNLOCK_MSTRLISTS();
				vme4l_discard_adrswin( win );
				VME4L_LOCK_MSTRLISTS();
			}
		}
	}
	VME4L_UNLOCK_MSTRLISTS();
}

static VME4L_ADRSWIN *vme4l_alloc_adrswin(void)
{
	VME4L_ADRSWIN *win;
	VME4L_LOCK_MSTRLISTS();

	/* get an unused addr window */
	if( list_empty( &G_freeAdrsWins )){
		VME4L_UNLOCK_MSTRLISTS();
		return NULL;
	}

	win = list_entry( G_freeAdrsWins.next, VME4L_ADRSWIN, node );
	list_del( &win->node );

	VME4L_UNLOCK_MSTRLISTS();
	return win;
}

/***********************************************************************/
/** Try to Request VME->PCI address window from bridge driver
 *
 * give up if no address windows available (even unused)!
 *
 * \param spc	   \IN	VME4L space number
 * \param vmeAddr  \IN	requested VME start address
 * \param size	   \IN	requested VME size
 * \param flags	   \IN	requested window flags
 * \param winP	   \OUT receives window pointer
 *
 * \return 0 on success or negative error number
 */
static int vme4l_try_request_adrswin(
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	int flags,
	VME4L_ADRSWIN **winP)
{
	VME4L_ADRSWIN *win;
	int rv;

	VME4LDBG("vme4l_try request_adrswin spc=%d vmeAddr=0x%llx sz=0x%llx "
			 "flg=0x%x\n", spc, vmeAddr, (uint64_t) size, flags );

	/*--- validate request first ---*/
	if( (vmeAddr + size - 1) > G_spaceTbl[spc].spcEnd )
		return -EINVAL;

	if( (win = vme4l_alloc_adrswin()) == NULL )
		return -ENOSPC;

	/*--- ask bridge driver to setup address window ---*/
	win->vmeAddr = vmeAddr;
	win->size = size;

	if( (rv = G_bDrv->requestAddrWindow(
			G_bHandle,
			spc,
			&win->vmeAddr,
			&win->size,
			&win->physAddr,
			flags,
			&win->bDrvData
			)) != 0 ){
		/* failed, win back to unused */
		VME4L_LOCK_MSTRLISTS();
		list_add_tail( &win->node, &G_freeAdrsWins );
		VME4L_UNLOCK_MSTRLISTS();

		printk(KERN_ERR_PFX "%s: failed\n", __func__);
		return rv;
	}

	win->spc 		= spc;
	win->flags 		= flags;
	win->useCount 	= 1;

	/*--- add window to list of available windows for space ---*/
	VME4L_LOCK_MSTRLISTS();
	list_add_tail( &win->node, &G_spaceTbl[spc].lstAdrsWins );
	VME4L_UNLOCK_MSTRLISTS();

	VME4LDBG("vme4l_try_request_adrswin exit ok. "
			 "spc=%d vmeAddr=0x%llx sz=0x%llx phys=0x%p "
			 "flg=0x%x\n", spc, win->vmeAddr, (uint64_t) win->size,
			 win->physAddr, win->flags);

	*winP = win;

	return 0;
}

/***********************************************************************/
/** Request VME->PCI address window from bridge driver
 *
 * \param spc	   \IN	VME4L space number
 * \param vmeAddr  \IN	requested VME start address
 * \param size	   \IN	requested VME size
 * \param flags	   \IN	requested window flags
 * \param winP	   \OUT receives window pointer
 *
 * \return 0 on success or negative error number
 */
static int vme4l_request_adrswin(
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	int flags,
	VME4L_ADRSWIN **winP)
{
	VME4L_ADRSWIN *win;
	int rv;

	VME4LDBG("vme4l_request_adrswin spc=%d vmeAddr=0x%llx sz=0x%llx flg=0x%x\n", spc, vmeAddr, (uint64_t) size, flags);

	/* try to find an already mapped VME window */
	if( (win = vme4l_find_adrswin( spc, vmeAddr, size, flags )) == NULL ){

		/* not found, try to setup a new one */
		if( vme4l_try_request_adrswin( spc, vmeAddr, size, flags, &win ) < 0){
			/* perhaps all windows already setup */
			vme4l_release_unused_adrswins();

			if( (rv = vme4l_try_request_adrswin( spc,
												 vmeAddr,
												 size,
												 flags,
												 & win)) < 0 ){
				printk(KERN_ERR_PFX "%s: adrswin request failed\n",
				       __func__);
				return rv;
			}
		}
	}

	VME4LDBG("vme4l_request_adrswin ok win=%p\n", win);

	*winP = win;
	return 0;
}


/***********************************************************************/
/** Destroy an ioremap region (iounmap it)
 *
 * Removes it from the address windows list.
 * Does not touch the position in the global cachelist
 *
 * Call this with VME4L_LOCK_MSTRLISTS() set!
 * \returns NULL=nothing to do. Otherwise the virtual address to be iounmapped
 */
void *vme4l_destroy_ioremap_region( VME4L_IOREMAP_REGION *region )
{
	void *vaddr=NULL;

	if( region->isValid ){
		list_del( &region->winNode ); 	/* remove it from windows list */

		vaddr = region->vaddr;
		region->isValid = 0;

	}
	return vaddr;
}

/***********************************************************************/
/** Create an ioremap region
 *
 * Take the least recently used entry from the ioremap region cache
 * and create an ioremapped region around the requested address
 *
 * \param win		\IN	 VME/PCI window to use
 * \param vmeAddr	\IN	 requested VME start address
 * \param size		\IN  requested VME size
 * \param regionP	\OUT receives created region
 *
 * \return 0 on success or negative error number
 */
static int vme4l_make_ioremap_region(
	VME4L_ADRSWIN *win,
	vmeaddr_t vmeAddr,
	size_t size,
	VME4L_IOREMAP_REGION **regionP)
{
	VME4L_IOREMAP_REGION *region;
	vmeaddr_t vmeStart;
	size_t maxSize;
	size_t useSize = 0;
	uintptr_t physAddr;
	int rv=0;
	void *vaddr;

	VME4LDBG( "vme4l_make_ioremap_region: vmeAddr %llx size %llx\n",
			  vmeAddr, (uint64_t) size );

	VME4L_LOCK_MSTRLISTS();
	/* get the least recently used region */
	region = list_entry( G_lstIoremapCache.prev,
						 VME4L_IOREMAP_REGION,
						 cacheNode );

	/* free previous content (if any) */
	vaddr = vme4l_destroy_ioremap_region( region );

	list_del( &region->cacheNode );

	VME4L_UNLOCK_MSTRLISTS();

	if( vaddr )
		iounmap(vaddr);

	/*
	 * try to map a bit more than the user has requested.
	 * at least map G_ioremapRegSize/2 before the requested address
	 * with a size of G_ioremapRegSize (or more if user requested more)
	 */
	if( vmeAddr > G_ioremapRegSize/2 )
		vmeStart = vmeAddr - G_ioremapRegSize/2;
	else
		vmeStart = 0;

	if( vmeStart < win->vmeAddr )
		vmeStart = win->vmeAddr;

	useSize = size + (vmeAddr - vmeStart); /* make shure whole area requested
											  by user is mapped */
	maxSize = win->vmeAddr + win->size - vmeStart;

	if( useSize < G_ioremapRegSize )
		useSize = G_ioremapRegSize;

	if( useSize > maxSize )
		useSize = maxSize;

	physAddr = (vmeStart - win->vmeAddr) + (uintptr_t)win->physAddr;

	VME4LDBG( "vme4l_make_ioremap_region: requested: 0x%llx (0x%llx). "
			  "Map vme=0x%llx (0x%llx) pa=0x%x\n", vmeAddr, (uint64_t) size,
			  vmeStart, (uint64_t) useSize, physAddr );

	if( (region->vaddr = ioremap_nocache( physAddr, useSize )) != NULL ){
		VME4LDBG( "ioremap ok: Mapped 0x%08x to 0x%p\n",
				  physAddr, region->vaddr );

		/* remap ok, add it to window list and in front of cache list */
		region->vmeAddr = vmeStart;
		region->size	= useSize;
		region->isValid	= 1;

		VME4L_LOCK_MSTRLISTS();
		list_add( &region->winNode, &win->lstIoremap );
		list_add( &region->cacheNode, &G_lstIoremapCache );
		VME4L_UNLOCK_MSTRLISTS();
		*regionP = region;
	}
	else {
		VME4L_LOCK_MSTRLISTS();
		list_add_tail( &region->cacheNode, &G_lstIoremapCache );
		VME4L_UNLOCK_MSTRLISTS();
		rv = -ENOSPC;
	}
	return rv;
}

/***********************************************************************/
/** Find cached ioremap region for \a win
 *
 * If it is found, it is moved to the head of the global cache list (LRU)
 *
 * \param win		\IN	 VME/PCI window to use
 * \param vmeAddr	\IN	 requested VME start address
 * \param size		\IN  requested VME size
 *
 * \return window if found or NULL if not found
 */
static VME4L_IOREMAP_REGION *vme4l_find_ioremap_region(
	VME4L_ADRSWIN *win,
	vmeaddr_t vmeAddr,
	size_t size)
{
	struct list_head *pos;
	VME4L_IOREMAP_REGION *region;

	VME4L_LOCK_MSTRLISTS();

	list_for_each( pos, &win->lstIoremap ){
		region = list_entry( pos, VME4L_IOREMAP_REGION, winNode );

		if( (region->vmeAddr <= vmeAddr) &&
			(region->vmeAddr + region->size) >= (vmeAddr+size)){

			list_del( &region->cacheNode );
			list_add( &region->cacheNode, &G_lstIoremapCache );

			VME4L_UNLOCK_MSTRLISTS();

			return region;
		}
	}
	VME4L_UNLOCK_MSTRLISTS();
	return NULL;
}

/***********************************************************************/
/** Generate 6 inline functions for 8,16,32 bit PIO transfers
 *
 * Macro parameters:
 * - \a size 			use 8,16 or 32 bit transfers
 * - \a type			one of uint8_t, uint16_t or uint32_t
 *
 * \param win			The address window used for the transfer
 * \param vaddr			starting virtual address for this transfer
 * \param dataP			user space data start
 * \param xsize			number of bytes to transfer
 * \param swAdrSwap		0=no software address swapping\n
 *						1=software address swapping
 *
 * \return 0 on success or negative error number
 */
#define DO_READ_PIO_XX(size,type) \
static int inline DoReadPio##size ( \
    VME4L_ADRSWIN *win,\
	void *vaddr,\
	void *dataP,\
	size_t xsize,\
	long swAdrSwap)\
{\
	 type buf;\
     type *userSpc = (type *)dataP;\
	 int rv=0, count;\
     long adrSwapMask = swAdrSwap ? \
       ((sizeof(type)==1) ? 1 : 0 ) : 0;\
     VME4LDBG("vme4l_do_read_pio%d xsize=%x\n", size, xsize);\
     if( !G_bDrv->readPio##size ) return -EINVAL; \
	 for( count=0; count<xsize; count+=sizeof(type), \
			  vaddr+=sizeof(type), userSpc++ ){\
		 if( (rv = G_bDrv->readPio##size ( \
				  G_bHandle, (void *)((uintptr_t)vaddr^adrSwapMask), &buf, 0, \
				  win->bDrvData)) < 0 )\
			 break;\
		 __put_user( buf, userSpc );\
	 }\
     return rv;\
}

#define DO_WRITE_PIO_XX(size,type) \
static int inline DoWritePio##size ( \
    VME4L_ADRSWIN *win,\
	void *vaddr,\
	void *dataP,\
	size_t xsize,\
	int swAdrSwap)\
{\
	 type buf;\
     type *userSpc = (type *)dataP;\
	 int rv=0, count;\
     uint32_t adrSwapMask = swAdrSwap ? \
       ((sizeof(type)==1) ? 1 : 0 ) : 0;\
     VME4LDBG("vme4l_do_write_pio%d xsize=%x\n", size, xsize);\
     if( !G_bDrv->writePio##size ) return -EINVAL; \
	 for( count=0; count<xsize; count+=sizeof(type), \
			  vaddr+=sizeof(type), userSpc++ ){\
         if( __get_user( buf, userSpc ) ) return -EFAULT;\
		 if( (rv = G_bDrv->writePio##size ( \
				  G_bHandle, (void *)((uintptr_t)vaddr^adrSwapMask), &buf, 0, \
				  win->bDrvData)) < 0 )\
			 break;\
	 }\
     return rv;\
}

DO_READ_PIO_XX( 8, uint8_t )
DO_READ_PIO_XX( 16, uint16_t )
DO_READ_PIO_XX( 32, uint32_t )
DO_WRITE_PIO_XX( 8, uint8_t )
DO_WRITE_PIO_XX( 16, uint16_t )
DO_WRITE_PIO_XX( 32, uint32_t )

/***********************************************************************/
/** Find/create master window and setup ioremap region
 *
 * \param spc			VME4L space number
 * \param vmeAddr		starting VME address
 * \param size			size in VME space
 * \param accWitdh		access witdh
 * \param winFlags		requested window flags
 * \param winP			(OUT) receives window pointer
 * \param vaddrP		(OUT) receives virtual address for vmeAddr
 * \return 0 on success, or negative error number
 */
static int vme4l_setup_pio(
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	int accWidth,
	int winFlags,
	VME4L_ADRSWIN **winP,
	char **vaddrP)
{
	int rv=0;
	VME4L_ADRSWIN *win;
	VME4L_IOREMAP_REGION *region;
	char *vaddr;
	VME4L_SPACE_ENT *spcEnt = &G_spaceTbl[spc];

	/* check alignment and accWidth <= maxWidth */
	if( (vmeAddr & (accWidth-1)) ||	(size & (accWidth-1)) || (accWidth > spcEnt->maxWidth) ) {
		printk(KERN_ERR_PFX "%s: Bad alignment/size/width 0x%lx %d %d\n",
		       __func__, vmeAddr, size, accWidth);
		return -EINVAL;
	}

	/*--- find/request a usable VME mapping window ---*/
	if( (rv = vme4l_request_adrswin( spc, vmeAddr, size, winFlags, &win )) < 0 ){
		return rv;
	}

	/*--- ioremap this region (or use it from the ioremap cache) ---*/
	if( (region = vme4l_find_ioremap_region( win, vmeAddr, size )) == NULL ){

		/* not cached, setup a new one */
		if( (rv = vme4l_make_ioremap_region( win, vmeAddr, size,
											 &region )) < 0 ){
			printk(KERN_ERR_PFX "%s: failed to make_ioremap_region\n",
			       __func__);
			return rv;
		}

	}

	vaddr = vmeAddr - region->vmeAddr + (char *)region->vaddr;

	*vaddrP = vaddr;
	*winP = win;
	return rv;
}



/***********************************************************************/
/** Handler for VME4L_IO_RW_BLOCK PIO transfers
 *
 * \param spc			VME4L space number
 * \param blk			ioctl argument from user
 * \param swapMode		window swapping mode
 * \return >=0 number of bytes transferred, or negative error number
 */
static int vme4l_rw_pio( VME4L_SPACE spc, VME4L_RW_BLOCK *blk, int swapMode )
{
	int rv=0;
	VME4L_ADRSWIN *win;
	int winFlags;
	char *vaddr;
	int swAdrSwap = !!(swapMode & VME4L_SW_ADR_SWAP);

	winFlags = (swapMode & VME4L_HW_SWAP1) ?
		VME4L_AW_HW_SWAP1 : 0;

	if( (rv = vme4l_setup_pio( spc, blk->vmeAddr, blk->size, blk->accWidth, winFlags, &win, &vaddr )))
		return rv;

	/*--- perform access here ---*/
	if( blk->direction == READ) {

		/* read from VME */
		switch( blk->accWidth ) {

		case 1:
			rv = DoReadPio8 ( win, vaddr, blk->dataP, blk->size, swAdrSwap );
			break;
		case 2:
			rv = DoReadPio16( win, vaddr, blk->dataP, blk->size, swAdrSwap );
			break;
		case 4:
			rv = DoReadPio32( win, vaddr, blk->dataP, blk->size, swAdrSwap );
			break;
		default:
			rv = -EINVAL;
			break;
		}
	}
	else {
		/* write to VME */
		switch( blk->accWidth ){

		case 1: rv = DoWritePio8 ( win, vaddr, blk->dataP, blk->size,
								  swAdrSwap ); break;
		case 2: rv = DoWritePio16( win, vaddr, blk->dataP, blk->size,
								  swAdrSwap ); break;
		case 4: rv = DoWritePio32( win, vaddr, blk->dataP, blk->size,
								  swAdrSwap ); break;

		default:
			rv = -EINVAL;
			break;
		}
	}
	if( rv == 0 )
		rv = blk->size;

	/*--- release VME address mapping window ---*/
	vme4l_release_adrswin( win );
	return rv;
}

/***********************************************************************/
/** Start DMA and wait for DMA to finish
 *
 * Starts DMA and waits until finished ok, bus error or DMA timeout.
 * This function ignores all signals while waiting for the DMA
 *
 * \return 0 on success, or negative error number
 */
static int vme4l_start_wait_dma(void)
{
	int rv;
	uint32_t ticks = 5 * HZ;

	wait_queue_t __wait;

	/* Add to wait queue before starting DMA */
	init_waitqueue_entry(&__wait, current);
	add_wait_queue(&G_dmaWq, &__wait);
	set_current_state(TASK_UNINTERRUPTIBLE);

	/* start DMA */
	if( (rv = G_bDrv->dmaStart( G_bHandle )) < 0 ){
		if (rv < 0)
			printk(KERN_ERR_PFX "%s: DMA dmaStart rv=%d\n",
			       __func__, rv );
		goto ABORT;
	}

	for (;;) {
		VME4LDBG("vme4l_start_wait_dma: going to sleep %d\n", ticks);
		ticks = schedule_timeout( ticks );

		if( ticks == 0 ){
			printk(KERN_ERR_PFX "%s: DMA timeout\n", __func__);
		}

		/* check DMA state */
		rv = G_bDrv->dmaStatus( G_bHandle );
		if( rv <= 0 ){
			/* error or ok */
			if (rv < 0)
				printk(KERN_ERR_PFX "%s: DMA status %d\n",
				       __func__, rv);
			break;
		}

		if( ticks == 0 ){
			printk(KERN_ERR_PFX "%s: DMA timeout DMA not finished\n", __func__);
			/* DMA timed out */
			rv = -ETIME;
			break;
		}
		VME4LDBG("vme4l_start_wait_dma: remaining %d ticks\n", ticks );

		/* In case we execute more loop iterations task has go to
		   TASK_UNINTERRUPTIBLE again */
		set_current_state(TASK_UNINTERRUPTIBLE);
	}

	set_current_state(TASK_RUNNING);
 ABORT:


	remove_wait_queue(&G_dmaWq, &__wait);

	if (rv<0)
		printk(KERN_ERR_PFX "%s: exit rv=%d\n", __func__, rv);

	return rv;
}

/***********************************************************************/
/** Perform zero-copy DMA with VME bridge
 *
 * \param spc			VME4L space number
 * \param sgList		list with \a sgElems scatter elements
 * \param sgNelems		number of valid elements in \a sgList
 * \param direction		0=read from VME 1=write to VME
 *
 * \return 0 on success, or negative error number
 */
static int vme4l_perform_zc_dma(
	VME4L_SPACE spc,
	VME4L_SCATTER_ELEM *sgList,
	int sgNelems,
	int direction,
	vmeaddr_t vmeAddr,
	int swapMode,
	int flags)
{
	int rv=0;

	if( down_interruptible( &G_dmaMutex ))
		return -ERESTARTSYS;

	while( sgNelems > 0 ){

		/* setup DMA */
		rv = G_bDrv->dmaSetup(
			G_bHandle,
			spc,
			sgList,
			sgNelems,
			direction,
			swapMode,
			&vmeAddr,
			flags);

		VME4LDBG( "vme4l_perform_zc_dma: dmaSetup rv=%d, next vmeAddr=0x%lx\n", rv, vmeAddr );

		if( rv < 0 ) {
			printk(KERN_ERR_PFX "%s: dmaSetup rv=%d\n",
			       __func__, rv);
			goto ABORT;
		}

		if( rv == 0 || rv > sgNelems){
			rv = -EINVAL;		/* bug in bridge driver... */
			goto ABORT;
		}

		sgNelems -= rv;
		sgList += rv;

		/* start&wait for DMA */
		rv = vme4l_start_wait_dma();

		if (rv < 0) {
			printk(KERN_ERR_PFX "%s: vme4l_start_wait_dma rv=%d\n",
			       __func__, rv);
			goto ABORT;
		}
	}

 ABORT:
	up( &G_dmaMutex );
	return rv;
}

#ifdef VME4L_DBG_DMA_DATA
/**
 * dump received/sent data right after DMA
 * \param nr_pages	Number of pages to dump
 * \param pages		User pages for the given buffer
 * \param len		#bytes to dump for each page
 * \param initOffset	offset of userdata in page
 *
 *\return 0 on success, or a negative number
 */

static void vme4l_user_pages_print(unsigned int nr_pages,
				   struct page **pages,
				   unsigned int len,
				   unsigned int initOffset )
{
	unsigned char *pDat;
	int i;
	int j;
	unsigned int offset = initOffset;

	for (i = 0; i < nr_pages; i++)
	{
		printk("page %d first 0x%03x byte:\n", i, len );
		pDat = (unsigned char*)page_address( pages[i] ) + offset;

		for ( j = 0; j < len / 16; j++ ) {
			printk("%04X | %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", j * 16,
				   pDat[0],pDat[1],pDat[2],pDat[3],pDat[4],pDat[5],pDat[6],pDat[7],
				   pDat[8],pDat[9],pDat[10],pDat[11],pDat[12],pDat[13],pDat[14],pDat[15]);

			pDat+=16;
		}
		printk( "\n" );
		offset = 0; /* if data amount to dump > 1 page, all other data starts at 0 then */
	}
}
#endif

/***********************************************************************/
/** prepare zero-copy DMA with VME bridge
 *
 * \param spc			VME4L space number
 * \param blk			pointer with kernel address of user buffer
 * \param swapMode		if 1 swap Data in VME core
 *
 * \return 0 on success, or negative error number
 */
static int vme4l_zc_dma( VME4L_SPACE spc, VME4L_RW_BLOCK *blk, int swapMode )
{
	int rv = 0, i;
	uintptr_t uaddr 	= (uintptr_t)blk->dataP;
	int flags 		= blk->flags;
	size_t count 		= blk->size;
	unsigned int nr_pages	= 0;
	struct page **pages 	= NULL;
	uint32_t totlen		= 0;
	unsigned int offset 	= 0;
	int direction 		= 0;
	struct pci_dev *pciDev	= NULL;
	struct device *pDev	= NULL;
	void *pKmalloc		= NULL;
        unsigned char *pVirtAddr= NULL;
	int locked		= 0;
	dma_addr_t dmaAddr 	= 0;
	VME4L_SCATTER_ELEM *sgListStart = NULL, *sgList;
	int to_user		= 0;
	void *addr 		= NULL;
#ifdef VME4L_DBG_DMA_DATA
	unsigned int initOffs 	= 0;
#endif

	/* direction as seen from  DMA API context */
	direction = ( blk->direction == READ ) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	to_user = !(blk->flags & VME4L_RW_KERNEL_SPACE_DMA);

	/* be paranoid.. */
	if (count == 0)
		return 0;

	pciDev = G_bDrv->pciDevGet( G_bHandle );
	pDev = &pciDev->dev;

	/*--- Allocate array to hold page pointers ---*/
	nr_pages = ((uaddr & ~PAGE_MASK) + count + ~PAGE_MASK) >> PAGE_SHIFT;

	/* User attempted Overflow! */
	if ((uaddr + count) < uaddr) {
		printk(KERN_ERR_PFX "%s: User attempted Overflow "
		       "(uaddr + count) < uaddr, (uaddr + count)0x%x, "
		       "uaddr 0x%x, count 0x%x\n",
		       __func__, (uaddr + count), uaddr, count);
		return -EINVAL;
	}

	if ((pages = kmalloc(nr_pages * sizeof(*pages), GFP_ATOMIC)) == NULL)
		return -ENOMEM;

	/* allocate scatter list */
	sgListStart = sgList = kmalloc(sizeof(*sgList) * nr_pages, GFP_ATOMIC);
	if( sgListStart == NULL ) {
		rv = -ENOMEM;
		goto CLEANUP;
	}

	if (to_user) {
		VME4LDBG("To/from Userspace DMA transfer\n");
		rv = get_user_pages_fast( uaddr, nr_pages, direction, pages);
		if (rv < 0)
			printk(KERN_ERR_PFX "%s: get_user_pages_fast failed rv"
			       "%d nr pages %d\n",
			       __func__, rv, nr_pages);

		if (rv < nr_pages) {
			goto CLEANUP;
		}
	} else {
		VME4LDBG("To/from Kernelspace DMA transfer\n");
		addr = (void *)uaddr;
		if (is_vmalloc_addr(addr)) {
			for (i = 0; i < nr_pages; i++)
				pages[i] = vmalloc_to_page(addr + PAGE_SIZE * i);
		} else {
			/* Note: this supports lowmem pages only */
			if (!virt_addr_valid(uaddr)) {
				printk(KERN_ERR_PFX "%s: virt_addr_valid not valid\n",
				       __func__);
				return -EINVAL;
			}
			for (i = 0; i < nr_pages; i++)
				pages[i] = virt_to_page(uaddr + PAGE_SIZE * i);
		}
	}

	/* pages are now locked in memory */
	locked++;

	/*--- build scatter/gather list ---*/
	offset = uaddr & ~PAGE_MASK; /* ts@men this gives initial offset betw. userdata and first mapped page, often > 0 */
#ifdef VME4L_DBG_DMA_DATA
	initOffs = offset;
#endif
	for (i = 0; i < nr_pages; ++i, sgList++) {
		struct page *page = pages[i];
		sgList->dmaLength  = PAGE_SIZE - offset;
            pVirtAddr = ((unsigned char*)(page_address( page ))) + offset;

		if( totlen + sgList->dmaLength > count ){
			sgList->dmaLength = count - totlen;
		}

            dmaAddr = dma_map_single( pDev, pVirtAddr, sgList->dmaLength, direction );
		if ( dma_mapping_error(pDev, dmaAddr ) ) {
                   printk(KERN_ERR_PFX "%s: *** error mapping DMA space!\n" ,
			  __func__);
			goto CLEANUP;
		} else {
                   sgList->dmaAddress = dmaAddr;      /* store page address for later dma_unmap_page */
		}


		VME4LDBG(" sglist %d: pageAddr=%p off=0x%04lx dmaAddr=%p length=0x%04x\n", i, page_address(page), offset, dmaAddr, sgList->dmaLength);
		totlen += sgList->dmaLength;
		offset = 0;
	}

	/*--- now do DMA in HW (device touches memory) ---*/
	rv = vme4l_perform_zc_dma( spc, sgListStart, nr_pages, blk->direction, blk->vmeAddr, swapMode, flags);

CLEANUP:
	/*--- free pages ---*/
	if( locked) {
		sgList = sgListStart;
		for (i = 0; i < nr_pages; i++, sgList++) {
			dma_unmap_single( pDev, sgList->dmaAddress, sgList->dmaLength , direction );

			if (to_user)
				put_page( pages[i] ); /* release pages locked with get_user_pages */
			}
	}

#ifdef VME4L_DBG_DMA_DATA
	vme4l_user_pages_print(nr_pages, pages, 32, initOffs );
#endif

	if( sgListStart )
		kfree( sgListStart );
	if( pages )
		kfree( pages );
	if (pKmalloc)
		kfree ( pKmalloc );

	if (rv < 0)
		printk(KERN_ERR_PFX "%s: rv=%d\n", __func__, rv);

	return rv >= 0 ? totlen : rv;
}

/***********************************************************************/
/** Handler for VME4L_IO_RW_BLOCK bounce buffer DMA transfers
 *
 * \param spc			VME4L space number
 * \param blk			ioctl argument from user
 * \param swapMode		window swapping mode
 * \return >=0 number of bytes transferred, or negative error number
 */
static int vme4l_bounce_dma( VME4L_SPACE spc, VME4L_RW_BLOCK *blk,
							 int swapMode )
{
	int rv=0;
	size_t len = blk->size;
	vmeaddr_t vmeAddr = blk->vmeAddr;
	char *userSpc = blk->dataP;

	if( down_interruptible( &G_dmaMutex ))
		return -ERESTARTSYS;

	VME4LDBG("-> vme4l_bounce_dma\n");

	if (blk->flags & VME4L_RW_NOVMEINC)
		printk(KERN_ERR_PFX "%s: NOVMEINC not supported in "
		       "bounce buffer DMA mode!\n",
		       __func__);

	while( len > 0 ){
		void *bounceBuf;
		size_t curLen;

		rv = G_bDrv->dmaBounceSetup(
			G_bHandle,
			spc,
			len,
			blk->direction,
			swapMode,
			vmeAddr,
			&bounceBuf);

		VME4LDBG("vme4l_bounce_dma: dmaBounceSetup rv=%d bounceBuf=%p\n",
				 rv, bounceBuf );

		if( rv < 0 )
			goto ABORT;

		if( rv == 0 || rv > len){
			rv = -EINVAL;		/* bug in bridge driver... */
			goto ABORT;
		}
		curLen = rv;

		/* for VME writes, copy user's data to bounce buffer */
		if( blk->direction == WRITE ){

			if (__copy_from_user( bounceBuf, userSpc, curLen ))
				goto ABORT;
		}

		/* start&wait for DMA */
		if( (rv = vme4l_start_wait_dma()) < 0 )
		  goto ABORT;

		/* for VME reads, copy data to user space */
		if( blk->direction == READ ){
			if (__copy_to_user( userSpc, bounceBuf, curLen ))
				goto ABORT;
		}

		/* release bounce buffer (if needed) */
		if( G_bDrv->dmaBounceBufRelease )
			G_bDrv->dmaBounceBufRelease( G_bHandle, bounceBuf );

		vmeAddr += curLen;
		userSpc += curLen;
		len 	-= curLen;

	}
 ABORT:
	up( &G_dmaMutex );
	VME4LDBG("<- vme4l_bounce_dma\n");
	return rv < 0 ? rv : blk->size;
}

/***********************************************************************/
/** Handler for VME4L_IO_RW_BLOCK
 *
 * \param spc			VME4L space number
 * \param blk			ioctl argument from user
 * \return >=0 number of bytes transferred, or negative error number
 * \param swapMode		window swapping mode
 */
int vme4l_rw(VME4L_SPACE spc, VME4L_RW_BLOCK *blk, int swapMode)
{
	int rv;
	VME4L_SPACE_ENT *spcEnt = &G_spaceTbl[spc];
	VME4LDBG("vme4l_rw %s spc=%d vmeAddr=0x%lx acc=%d sz=0x%lx dataP=0x%p swp=0x%x\n",
			 blk->direction ? "write":"read",
			 spc,
			 blk->vmeAddr,
			 blk->accWidth,
			 blk->size,
			 blk->dataP,
			 swapMode );

	if( spcEnt->isBlt || (blk->flags & VME4L_RW_USE_SGL_DMA))
	{
		if( G_bDrv->dmaSetup == NULL ){
			if( G_bDrv->dmaBounceSetup == NULL ){
				rv = -EINVAL;			/* bridge has no DMA */
				goto ABORT;
			}
			/* bounce buffer DMA */
			VME4LDBG("calling vme4l_bounce_dma()\n" );
			rv = vme4l_bounce_dma( spc, blk, swapMode );
		}
		else {
			/* zero copy DMA */
		        VME4LDBG("calling vme4l_zc_dma()\n" );
			rv = vme4l_zc_dma( spc, blk, swapMode );
		}
	}
	else {
		rv = vme4l_rw_pio( spc, blk, swapMode );
	}

	if (rv < 0)
		printk(KERN_ERR_PFX "%s: %s rv=%d, spc=%d vmeAddr=0x%lx "
		       "acc=%d sz=0x%lx dataP=0x%p flags=0x%x, swp=0x%x\n",
		       __func__,
		       blk->direction ? "write":"read",
		       rv,
		       spc,
		       blk->vmeAddr,
		       blk->accWidth,
		       blk->size,
		       blk->dataP,
		       blk->flags,
		       swapMode);

 ABORT:
	VME4LDBG("vme4l_rw exit rv=%d\n", rv);
	return rv;
}

/***********************************************************************/
/** Handler for VME4L_IO_RMW_CYCLE
 *
 * \param spc			VME4L space number
 * \param blk			ioctl argument from user
 * \param swapMode		window swapping mode
 * \return >=0 number of bytes transferred, or negative error number
 */
static int vme4l_rmw_cycle( VME4L_SPACE spc, VME4L_RMW_CYCLE *blk,
							int swapMode )
{
	int rv, winFlags;
	VME4L_ADRSWIN *win;
	char *vaddr;
	void *physAddr;
	VME4L_SPACE_ENT *spcEnt = &G_spaceTbl[spc];

	VME4LDBG("vme4l_rmw_cycle spc=%d vmeAddr=0x%08lx acc=%d mask=%x\n",
			 spc, blk->vmeAddr, blk->accWidth, blk->mask);

	winFlags = (swapMode & VME4L_HW_SWAP1) ?
		VME4L_AW_HW_SWAP1 : 0;

	if( spcEnt->isBlt )
		rv = -EINVAL;
	else {
		if( (rv = vme4l_setup_pio( spc, blk->vmeAddr, blk->accWidth,
								   blk->accWidth,
								   winFlags, &win, &vaddr )) < 0)
			goto ABORT;

		physAddr = blk->vmeAddr - win->vmeAddr + win->physAddr;

		if( G_bDrv->rmwCycle )
			rv = G_bDrv->rmwCycle( G_bHandle, vaddr, physAddr, blk->accWidth,
								   blk->mask, &blk->rv );
		else
			rv = -ENOTTY;

		/*--- release VME address mapping window ---*/
		vme4l_release_adrswin( win );
	}
 ABORT:
	VME4LDBG("vme4l_rmw_cycle exit rv=%d\n", rv);
	return rv;
}

/***********************************************************************/
/** Handler for VME4L_IO_AONLY_CYCLE
 *
 * \param spc			VME4L space number
 * \param blk			ioctl argument from user
 * \param swapMode		window swapping mode
 * \return >=0 number of bytes transferred, or negative error number
 */
static int vme4l_aonly_cycle( VME4L_SPACE spc, VME4L_AONLY_CYCLE *blk,
							  int swapMode )
{
	int rv, winFlags;
	VME4L_ADRSWIN *win;
	char *vaddr;
	VME4L_SPACE_ENT *spcEnt = &G_spaceTbl[spc];

	VME4LDBG("vme4l_aonly_cycle spc=%d vmeAddr=0x%08lx\n",
			 spc, blk->vmeAddr);

	winFlags = (swapMode & VME4L_HW_SWAP1) ?
		VME4L_AW_HW_SWAP1 : 0;

	if( spcEnt->isBlt )
		rv = -EINVAL;
	else {
		if( (rv = vme4l_setup_pio( spc, blk->vmeAddr, 1, 1,
								   winFlags, &win, &vaddr )) < 0)
			goto ABORT;

		if( G_bDrv->aOnlyCycle )
			rv = G_bDrv->aOnlyCycle( G_bHandle, vaddr );
		else
			rv = -ENOTTY;

		/*--- release VME address mapping window ---*/
		vme4l_release_adrswin( win );
	}
 ABORT:
	VME4LDBG("vme4l_aonly_cycle exit rv=%d\n", rv);
	return rv;
}

/***********************************************************************/
/** Enable VME IRQ level or special level
 *
 *
 *
 * \param spc		VME4L space number
 * \param vmeAddr	slave window start address on VME
 * \param size		size in bytes of window. If 0, disable window
 * \return 0 on success or negative error number
 */
static int vme4l_slave_window_ctrl( VME4L_SPACE spc, vmeaddr_t vmeAddr,
									size_t size )
{
	VME4L_SPACE_ENT *spcEnt = &G_spaceTbl[spc];
	VME4L_ADRSWIN *win=NULL;
	void *physAddr = NULL;
	int newWin=0;
	int rv=0;

	VME4LDBG("vme4l_slave_window_ctrl spc=%d vmeAddr=0x%lx sz=0x%lx\n",
			 spc, vmeAddr, size );

	if( !spcEnt->isSlv || G_bDrv->slaveWindowCtrl==NULL ){
		printk(KERN_ERR_PFX "%s: ERROR: not a slave window\n",
		       __func__);
		return -ENOTTY;			/* must be a slave window space */
	}


	VME4L_LOCK_MSTRLISTS();

	/* get the window that was previously setup (if any) */
	if( !list_empty( &spcEnt->lstAdrsWins ) )
		win = list_entry( spcEnt->lstAdrsWins.next, VME4L_ADRSWIN, node );

	VME4L_UNLOCK_MSTRLISTS();

	if( size ){

		/* new opened window or size change request */
		if( list_empty( &spcEnt->lstAdrsWins ) ){

			/* new opened */
			if( (win = vme4l_alloc_adrswin()) == NULL ){
				printk(KERN_ERR_PFX "%s: ERROR: no Space\n",
				       __func__);
				return -ENOSPC;
			}

			win->spc 		= spc;
			win->vmeAddr 	= 0xffffffff;
			win->size 		= 0;
			win->physAddr 	= NULL;
			win->useCount 	= 0;
			win->flags 		= 0;

			newWin 			= 1;
		}
		physAddr = win->physAddr;
	}

	if( win && ((size != win->size) || (vmeAddr != win->vmeAddr)) ){

		/*
		 * check if someone had the window mapped.
		 * Don't allow size change if so
		 */
		if( win->useCount ){
			printk(KERN_ERR_PFX "%s: ERROR: window in use\n",
			       __func__);
			return -EBUSY;
		}

		/* advise bridge driver to setup window */
		rv = G_bDrv->slaveWindowCtrl( G_bHandle,
									  spc,
									  vmeAddr,
									  size,
									  &physAddr,
									  &win->bDrvData);
		if( rv == 0 ){
			win->vmeAddr 	= vmeAddr;
			win->size 		= size;
			win->physAddr 	= physAddr;
			VME4LDBG(" win paddr=%p\n", win->physAddr );
		}
	}


	VME4L_LOCK_MSTRLISTS();

	if( size == 0 && win){
		/* back to free list */
		list_del ( &win->node );
		list_add_tail( &win->node, &G_freeAdrsWins);
	}

	if( newWin ){
		if( rv != 0 )
			/* back to free list */
			list_add_tail( &win->node, &G_freeAdrsWins);
		else {
			/* set it as active window */
			list_add_tail( &win->node, &spcEnt->lstAdrsWins );
		}
	}

	VME4L_UNLOCK_MSTRLISTS();
	VME4LDBG(" leaving vme4l_slave_window_ctrl: rv = %d\n",rv );
	return rv;
}


/***********************************************************************/
/** Disable VME IRQ level or special level
 *
 * \param level			irq level to disable (1..7)
 * \return 0 on success or negative error number
 */
static int vme4l_irqlevel_disable( int level )
{
	int rv=0;
	VME4LDBG("vme4l_irqlevel_disable %d\n", level );
	if( (level == VME4L_IRQLEV_UNKNOWN) || (level >= VME4L_NUM_LEVELS) )
		return -EINVAL;

	if( (rv = G_bDrv->irqLevelCtrl( G_bHandle, level, 0 )) == 0 )
		G_irqLevEnblCount[level]--;

	return rv;
}



/***********************************************************************/
/** Install IRQ vector entry
 *
 * \param entry			irq entry struct to install
 * \param vector		vector to install entry
 * \return 0 on success or negative error number
 *
 * \brief				This function simply hooks the new vme handler
 *						into the global G_vectTbl[] vector table.
 */
static int vme4l_irq_install( VME4L_IRQ_ENTRY *ent, int vme_vector )
{
	unsigned long ps;
	int rv=0;

	if( vme_vector >= VME4L_NUM_VECTORS )
		return -EINVAL;

	VME4L_LOCK_VECTORS(ps);

	list_add_tail( &ent->node, &G_vectTbl[vme_vector] );

	/* enable irq if requested */
	if( ent->flags & VME4L_IRQ_ENBL )
		rv = vme4l_irqlevel_enable( ent->level );

	VME4L_UNLOCK_VECTORS(ps);

	return rv;
}

/***********************************************************************/
/** Handler for VME4L_IO_SIG_INSTALL2
 *
 * \param blk			ioctl argument from user
 * \return 0 on success or negative error number
 */
static int vme4l_signal_install( VME4L_SIG_INSTALL2 *blk, struct file *file )
{
	VME4L_IRQ_ENTRY *ent;
	int rv;
	VME4LDBG("%s()\n", __FUNCTION__ );
	if( (ent = kmalloc( sizeof( *ent ), GFP_KERNEL )) == NULL )
		return -ENOMEM;
	memset( ent, 0, sizeof(*ent));

	ent->flags	 		= blk->flags;
	ent->level 			= blk->level;
	ent->u.user.signal	= blk->signal;
	ent->u.user.task	= current;
	ent->u.user.file	= file;

	/* install a LINUX USER irq (aka signal..)*/
	ent->entType		= VME4L_USER_IRQ;
	if( (rv = vme4l_irq_install( ent, blk->vector )) < 0 )
		kfree( ent );

	return rv;
}



/***********************************************************************/
/** Handler for VME4L_IO_SIG_UNINSTALL
 *
 * This removes \b all installed signals on this vector for the current
 * process created on top of \a file
 *
 * \param vector		vector to uninstall
 * \param file			only vectors associated with \a file are removed
 *
 * \return 0 on success or negative error number
 */
static int vme4l_signal_uninstall( int vector, struct file *file )
{
	VME4L_IRQ_ENTRY *ent;
	struct list_head *pos, *tmp;
	unsigned long ps;

	if( vector >= VME4L_NUM_VECTORS )
		return -EINVAL;

	VME4L_LOCK_VECTORS(ps);

	list_for_each_safe( pos, tmp, &G_vectTbl[vector] ){
		ent = list_entry( pos, VME4L_IRQ_ENTRY, node );
		if( ent->entType == VME4L_USER_IRQ ){
			if( ent->u.user.task == current && ent->u.user.file == file ){

				/* remove entry */
				list_del( &ent->node );

				VME4L_UNLOCK_VECTORS(ps);
				kfree( ent );
				VME4L_LOCK_VECTORS(ps);
			}
		}
	}
	VME4L_UNLOCK_VECTORS(ps);
	return 0;
}


/***********************************************************************/
/** vme4l Interrupt handler
 *
 * This should be called from the bridge drivers interrupt routine
 *
 * The special interrupts >= VME4L_IRQLEV7 must be cleared
 * by the bridge driver before calling this function.
 *
 * \param level		the interrupt level code, see \ref VME4L_IRQLEV
 * \param vector 	VME vector or pseudo vector
 * \param regs		regs argument passed to bridge irq (for whatever)
 *
 * \brief   the occuring Interrupts are dispatched to one of
 *		   	the 3 possible handling environments. These are:
 *
 *		   	- Linux Kernel handlers  ent->entType = VME4L_KERNEL_IRQ
 *		   	- Linux Usermode signals ent->entType = VME4L_USER_IRQ
 *
 */
void vme4l_irq( int level, int vector, struct pt_regs *regs)
{
	VME4L_IRQ_ENTRY *ent;
	struct list_head *pos;
	unsigned long ps;
	static unsigned long _flags=0; /* Adam: Why static? */
	int doDisable=0;

	VME4LDBG("vme4l_irq() level=%d vector=%d \n", level, vector);

	if( vector == VME4L_IRQVEC_SPUR )
		printk( KERN_WARNING "VME4L: spurious interrupt level %d\n", level );
	else if(level == VME4L_IRQLEV_DMAFINISHED /* DMA finished */
		|| (level == VME4L_IRQLEV_BUSERR && vector == 0) /* DMA failed */
		){
		VME4LDBG("DMA finished, wake G_dmaWq\n");
		/* wake up waiting task */
		wake_up( &G_dmaWq );
	}
	else
	{  /* brace2 */
		VME4L_LOCK_VECTORS(ps);

		if( list_empty( &G_vectTbl[vector]) && (level != VME4L_IRQLEV_BUSERR)){
			VME4LDBG( "VME4L: Uninitialized VME Interrupt lev %d vect %d\n",
					  level, vector);
		}

		/* --- Skip thru list of all registered Handlers for vector --  */
		list_for_each( pos, &G_vectTbl[vector] )
		{
			ent = list_entry( pos, VME4L_IRQ_ENTRY, node );
			switch( ent->entType ){

			case VME4L_KERNEL_IRQ:

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
				ent->u.kernel.h.lHandler( level, ent->u.kernel.dev_id );
#else
				ent->u.kernel.h.lHandler( level, ent->u.kernel.dev_id, regs );
#endif /*LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)*/
				break;


			case VME4L_USER_IRQ:
				/* send signal to process */
				if( (level == VME4L_IRQLEV_BUSERR) &&
					(ent->flags & VME4L_IRQ_OLDHANDLER )){
					if( G_postedWriteMode || (current == ent->u.user.task) ){
						vme4l_send_sig( ent, 1 );
					}
				}
				else {
					vme4l_send_sig( ent, 1 );
				}
				if( ((vector < 0x100) ||
					 (vector == VME4L_IRQVEC_ACFAIL) ||
					 (vector == VME4L_IRQVEC_SYSFAIL)) &&
					!(ent->flags & VME4L_IRQ_ROAK))
					/* level must be reenabled by USER application */
					doDisable++;
				break;

			default:
				printk( KERN_WARNING "VME4L: unknown handler type"
						"lev %d vec %d\n", level, vector);
				break;
			} /* /switch */
		}/* /list_for_each */

		if( doDisable ){
			SAVE_FLAGS_AND_CLI(_flags);
			vme4l_irqlevel_disable( level );
			RESTORE_FLAGS(_flags);
		}


		VME4L_UNLOCK_VECTORS(ps);
	} /* /brace2 */
}

/* -- LINUX DRIVER ENTRY POINTS -- */

/***********************************************************************/
/** Open entry point of VME4L driver
 *
 * \param inode			pointer to inode structure
 * \param file			pointer to file structure
 * \return 0=ok, or negative error number
 */
static int vme4l_open(
	struct inode  *inode,
	struct file   *file
)
{
	int minor;
	VME4L_FILE_PRIV *fp;

	minor = MINOR(inode->i_rdev);

	if( minor >= VME4L_SPACE_TBL_SIZE )
		return -ENODEV;

	/* no bridge registered? abort! */
	if( !G_bDrv )
		return -ENXIO;

	VME4LDBG("vme4l_open %s\n", G_spaceTbl[minor].devName );

	if( ( fp = kmalloc( sizeof( VME4L_FILE_PRIV ),
						GFP_KERNEL )) == NULL )
		return -ENOMEM;

	fp->minor = minor;
	fp->swapMode = VME4L_NO_SWAP;
	file->private_data = fp;


	return 0;
}

/***********************************************************************/
/** Close entry point of VME4L driver
 *
 * \param inode			pointer to inode structure
 * \param file			pointer to file structure
 * \return 0=ok, or negative error number
 */
static int vme4l_release(
	struct inode  *inode,
	struct file   *file
)
{
	int minor = MINOR(inode->i_rdev);
	int vector;

	VME4LDBG("vme4l_close %s\n", G_spaceTbl[minor].devName );

	/* remove all signals associated with this file */
	for( vector=0; vector<VME4L_NUM_VECTORS; vector++ )
		vme4l_signal_uninstall( vector, file );

	kfree( file->private_data );
	file->private_data = NULL;

	return 0;
}

/***********************************************************************/
/** This is a verbatim copy from drivers/char/mem.c
 *
 */
#ifndef pgprot_noncached

/*
 * This should probably be per-architecture in <asm/pgtable.h>
 */
static inline pgprot_t pgprot_noncached(pgprot_t _prot)
{
	unsigned long prot = pgprot_val(_prot);

#if defined(__i386__) || defined(__x86_64__)
	/* On PPro and successors, PCD alone doesn't always mean
	    uncached because of interactions with the MTRRs. PCD | PWT
	    means definitely uncached. */
	if (boot_cpu_data.x86 > 3)
		prot |= _PAGE_PCD | _PAGE_PWT;
#elif defined(__powerpc__)
	prot |= _PAGE_NO_CACHE | _PAGE_GUARDED;
#elif defined(__mc68000__)
#ifdef SUN3_PAGE_NOCACHE
	if (MMU_IS_SUN3)
		prot |= SUN3_PAGE_NOCACHE;
	else
#endif
	if (MMU_IS_851 || MMU_IS_030)
		prot |= _PAGE_NOCACHE030;
	/* Use no-cache mode, serialized */
	else if (MMU_IS_040 || MMU_IS_060)
		prot = (prot & _CACHEMASK040) | _PAGE_NOCACHE_S;
#endif

	return __pgprot(prot);
}

#endif /* !pgprot_noncached */

/***********************************************************************/
/** Unmap VM area mapped by vme4l_mmap
 *
 * Kernel will call this when VM area is destroyed
 */
void vme4l_mmap_close( struct vm_area_struct *vma )
{
	VME4L_ADRSWIN *win = (VME4L_ADRSWIN *)vma->vm_private_data;
	vme4l_release_adrswin( win ); /* decrease use count */
}

static struct vm_operations_struct vme4l_remap_vm_ops = {
	.close = vme4l_mmap_close,
};


/***********************************************************************/
/** Mmap entry point of VME4L driver
 *
 * \param file			pointer to file structure
 * \param vma			vm_area_struct
 * \return 				0=ok, or negative error number
 */
static int vme4l_mmap(
	struct file * file,
    struct vm_area_struct *vma
)
{
	VME4L_SPACE spc;
	VME4L_ADRSWIN *win = NULL;
	VME4L_FILE_PRIV *fp;
	VME4L_SPACE_ENT *spcEnt;
	int rv=0;
	/* hmm, vm_pgoff is unsigned long -> 32 bit at 32 bit systems, so how we
	   can map VME addresses > 4 GB ??? */
	uintptr_t vmeAddr	= vma->vm_pgoff << PAGE_SHIFT;
	unsigned long size		= vma->vm_end - vma->vm_start;
	uintptr_t physAddr;

	fp = (VME4L_FILE_PRIV *) file->private_data;
	spc = fp->minor;
	spcEnt = &G_spaceTbl[spc];

	if( ! spcEnt->isSlv ){
		/*---------------+
		|  Master space  |
		+---------------*/
		if( fp->swapMode & VME4L_SW_ADR_SWAP )
			return -EINVAL;

		VME4LDBG("vme4l_mmap %s vmeAddr=%lx (%lx) swapMode=%x\n",
				 G_spaceTbl[spc].devName,
				 vmeAddr, size, fp->swapMode );

		if( (rv = vme4l_request_adrswin( spc, vmeAddr, size,
										 (fp->swapMode & VME4L_HW_SWAP1) ?
										 VME4L_AW_HW_SWAP1 : 0,
										 &win )) < 0 )
			goto ABORT;

		physAddr = (uintptr_t)win->physAddr + (vmeAddr - win->vmeAddr);
	}
	else
	{
		/*---------------+
		|  Slave Window  |
		+---------------*/
		unsigned long offset = vmeAddr;

		VME4L_LOCK_MSTRLISTS();

		/* get the window that was previously setup (if any) */
		if( !list_empty( &spcEnt->lstAdrsWins ) )
			win = list_entry( spcEnt->lstAdrsWins.next, VME4L_ADRSWIN, node );

		VME4L_UNLOCK_MSTRLISTS();

		VME4LDBG("vme4l_mmap %s for slave win win=%p off=%lx (%lx)\n",
				 G_spaceTbl[spc].devName, win,
				 offset, size );

		if( ! win ){
			rv = -ENODEV;		/* window not setup */
			goto ABORT;
		}

		VME4LDBG(" phys %p size 0x%lx\n",  win->physAddr, win->size );

		if( offset + size > win->size ){
			rv = -EINVAL;
			goto ABORT;
		}

		physAddr = (uintptr_t)win->physAddr + offset;
	}

	/* Accessing VME must be done non-cached / guarded. */

	/* replace discontinued VM_RESERVED as stated in Torvalds' mail:
	https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/commit/?id=547b1e81afe3119f7daf702cc03b158495535a25 */
	vma->vm_flags |= ( VM_IO | VM_DONTEXPAND |
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
			  VM_DONTDUMP
#else
			  VM_RESERVED
#endif
			 );
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	/*
	 * Setup a callback to free our window when user unmaps area
	 */
	vma->vm_private_data = (void *)win;
	vma->vm_ops = &vme4l_remap_vm_ops;

	if( VME4L_REMAP(vma,
					vma->vm_start,
					physAddr,
					size,
					vma->vm_page_prot )) {
		rv = -EAGAIN;
		goto ABORT;
	}

	if( spcEnt->isSlv )
		win->useCount++;
	return 0;

 ABORT:
	printk(KERN_ERR_PFX "%s: vme4l_mmap failed for spc %d addr %lx (%lx), rv=%d\n",
	       __func__, spc, vmeAddr, size, rv );

	return rv;
}

/***********************************************************************/
/** Ioctl entry point of VME4L driver
 *
 * \param inode			pointer to inode structure
 * \param file			pointer to file structure
 * \param cmd			ioctl number
 * \param arg			argument to ioctl
 *
 * \return 0=ok, or negative error number
 */
static long vme4l_ioctl(
	struct file *file,
	unsigned int  cmd,
	unsigned long arg
)
{
	/* FIXME I might need some locking here */
	int rv;
	unsigned long ps;
	VME4L_FILE_PRIV *fp= (VME4L_FILE_PRIV *)file->private_data;
	VME4L_SPACE spc = fp->minor;

	VME4LDBG( "vme4l_ioctl %s cmd %08x arg %08lx\n",
			  G_spaceTbl[(int)spc].devName, cmd, arg );

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (_IOC_TYPE(cmd) != VME4L_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > VME4L_IOC_MAXNR) return -ENOTTY;


	switch( cmd ){

	case VME4L_IO_RW_BLOCK:
	{
		VME4L_RW_BLOCK blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}

		if( !access_ok( (blk.direction == WRITE) ? VERIFY_READ : VERIFY_WRITE,
						blk.dataP, blk.size)){
			rv = -EFAULT;
			break;
		}

		VME4LDBG( "vme4l_ioctl: VME4L_IO_RW_BLOCK: %s with size 0x%lx, data @ %p\n", blk.direction ? "write" : "read", blk.size, blk.dataP );

		rv = vme4l_rw( spc, &blk, fp->swapMode );
		break;
	}

	case VME4L_IO_IRQ_ENABLE2:
	{
		int level = arg & ~VME4L_IO_IRQ_ENABLE_DISABLE_MASK;

		VME4L_LOCK_VECTORS(ps);

		if( arg & VME4L_IO_IRQ_ENABLE_DISABLE_MASK )
			rv = vme4l_irqlevel_disable( level );
		else
			rv = vme4l_irqlevel_enable( level );

		VME4L_UNLOCK_VECTORS(ps);

		break;
	}

	case VME4L_IO_SIG_INSTALL2:
	{
		VME4L_SIG_INSTALL2 blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}
		rv = vme4l_signal_install( &blk, file );

		break;
	}

	case VME4L_IO_SIG_UNINSTALL:
		rv = vme4l_signal_uninstall( arg, file );
		break;


	case VME4L_IO_SYS_CTRL_FUNCTION_GET:
		rv = -ENOTTY;
		if( G_bDrv->sysCtrlFuncGet )
			rv = G_bDrv->sysCtrlFuncGet( G_bHandle );
		break;

	case VME4L_IO_SYS_CTRL_FUNCTION_SET:
		rv = -ENOTTY;
		if( G_bDrv->sysCtrlFuncSet )
			rv = G_bDrv->sysCtrlFuncSet( G_bHandle, arg );
		break;

	case VME4L_IO_SYS_RESET:
		rv = -ENOTTY;
		if( G_bDrv->sysReset )
			rv = G_bDrv->sysReset( G_bHandle );
		break;

	case VME4L_IO_ARBITRATION_TIMEOUT_GET:
		rv = -ENOTTY;
		if( G_bDrv->arbToutGet )
			rv = G_bDrv->arbToutGet( G_bHandle, arg );
		break;

	case VME4L_IO_BUS_ERROR_GET:
	{
		VME4L_BUS_ERROR_INFO blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}

		rv 			 = -ENOTTY;
		blk.addr	 = 0xffffffff;
		blk.attr	 = 0;

		if( G_bDrv->busErrGet )
			rv = G_bDrv->busErrGet( G_bHandle, &blk.attr, &blk.addr, blk.clear );

		if( copy_to_user( (void *)arg, &blk, sizeof(blk)))
			rv = -EFAULT;

		break;
	}

	case VME4L_IO_REQUESTER_MODE_GET:
		rv = -ENOTTY;
		if( G_bDrv->requesterModeGet )
			rv = G_bDrv->requesterModeGet( G_bHandle );
		break;

	case VME4L_IO_REQUESTER_MODE_SET:
		rv = -ENOTTY;
		if( G_bDrv->requesterModeSet )
			rv = G_bDrv->requesterModeSet( G_bHandle, arg );
		break;

  	case VME4L_IO_GEO_ADDR_GET:
		rv = -ENOTTY;
		if( G_bDrv->geoAddrGet )
		   rv = G_bDrv->geoAddrGet( G_bHandle );
		break;

  	case VME4L_IO_REQUESTER_LVL_SET:
		rv = -ENOTTY;
		if( G_bDrv->requesterLevelSet )
		  rv = G_bDrv->requesterLevelSet( G_bHandle, arg );
		break;

	case VME4L_IO_REQUESTER_LVL_GET:
		rv = -ENOTTY;
		if( G_bDrv->requesterLevelGet )
		  rv = G_bDrv->requesterLevelGet( G_bHandle );
		break;

	case VME4L_IO_ADDR_MOD_GET:
		rv = -ENOTTY;
		if( G_bDrv->addrModifierGet )
			rv = G_bDrv->addrModifierGet( spc, G_bHandle );
		break;

	case VME4L_IO_ADDR_MOD_SET:
		rv = -ENOTTY;
		if( G_bDrv->addrModifierSet )
			rv = G_bDrv->addrModifierSet( spc, G_bHandle, arg );
		break;

	case VME4L_IO_POSTED_WRITE_MODE_GET:
		rv = -ENOTTY;
		if( G_bDrv->postedWriteModeGet )
			rv = G_bDrv->postedWriteModeGet( G_bHandle );
		break;

	case VME4L_IO_IRQ_GENERATE2:
		rv = -ENOTTY;
		if( G_bDrv->irqGenerate )
			rv = G_bDrv->irqGenerate( G_bHandle,
									  VME4L_LEVEL_GET(arg),
									  VME4L_VECTOR_GET(arg));
		break;

	case VME4L_IO_IRQ_GEN_ACKED:
		rv = -ENOTTY;
		if( G_bDrv->irqGenAcked )
			rv = G_bDrv->irqGenAcked( G_bHandle, arg );
		break;

	case VME4L_IO_IRQ_GEN_CLEAR:
		rv = -ENOTTY;
		if( G_bDrv->irqGenClear )
			rv = G_bDrv->irqGenClear( G_bHandle, arg );
		break;

	case VME4L_IO_RMW_CYCLE:
	{
		VME4L_RMW_CYCLE blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}

		rv = vme4l_rmw_cycle( spc, &blk, fp->swapMode );

		if( copy_to_user( (void *)arg, &blk, sizeof(blk)))
			rv = -EFAULT;
		break;
	}

	case VME4L_IO_AONLY_CYCLE:
	{
		VME4L_AONLY_CYCLE blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}

		rv = vme4l_aonly_cycle( spc, &blk, fp->swapMode );
		break;
	}

	case VME4L_IO_SWAP_MODE_SET:
	{
		VME4LDBG("VME4L_IO_SWAP_MODE_SET 0x%lx\n", arg );

		rv = 0;
#ifdef __powerpc__
		/* don't allow swapping modes on PowerPC */
		if( arg != 0 )
			rv = -EINVAL;
#else
		/* swapping modes cannot be combined */
		if( (arg & (VME4L_HW_SWAP1 | VME4L_SW_ADR_SWAP)) ==
			(VME4L_HW_SWAP1 | VME4L_SW_ADR_SWAP)){
			rv = -EINVAL;
			break;
		}
		fp->swapMode = arg;
#endif /* __powerpc__ */
		break;
	}

	case VME4L_IO_SLAVE_WINDOW_CTRL:
	{
		VME4L_SLAVE_WINDOW_CTRL blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}

		rv = vme4l_slave_window_ctrl( spc, blk.vmeAddr, blk.size );
		break;
	}

	case VME4L_IO_MBOX_RW:
	{
		VME4L_MBOX_RW blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}

		rv = -EINVAL;

		if( blk.direction == READ ){
			if( G_bDrv->mboxRead ){
				rv = G_bDrv->mboxRead( G_bHandle, blk.mbox, &blk.val );
				if( copy_to_user( (void *)arg, &blk, sizeof(blk)))
					rv = -EFAULT;
			}
		}
		else {
			if( G_bDrv->mboxWrite )
				rv = G_bDrv->mboxWrite( G_bHandle, blk.mbox, blk.val );
		}

		break;
	}

	case VME4L_IO_LOCMON_REG_RW:
	{
		VME4L_MBOX_RW blk;

		if( copy_from_user( &blk, (void *)arg, sizeof(blk)) ){
			rv = -EFAULT;
			break;
		}

		rv = -EINVAL;

		if( blk.direction == READ ){
			if( G_bDrv->locMonRegRead ){
				rv = G_bDrv->locMonRegRead( G_bHandle, blk.mbox, &blk.val );

				if( copy_to_user( (void *)arg, &blk, sizeof(blk)))
					rv = -EFAULT;
			}
		}
		else {
			if( G_bDrv->locMonRegWrite )
				rv = G_bDrv->locMonRegWrite( G_bHandle, blk.mbox, blk.val );
		}

		break;
	}

	/*---------------------------------+
	|  ioctls identical with old VME4L |
	+---------------------------------*/
	case VME4L_IO_POSTED_WRITE_MODE_SET:
		VME4L_LOCK_VECTORS(ps);

		rv = -ENOTTY;
		if( G_bDrv->postedWriteModeSet )
			rv = G_bDrv->postedWriteModeSet( G_bHandle, arg );

		G_postedWriteMode = arg;

		VME4L_UNLOCK_VECTORS(ps);
		break;


	/*------------------------------+
	|  Old VME4L compatible ioctls  |
	+------------------------------*/
#ifdef __powerpc__
	case VME4L_IO_IRQ_ENABLE:
	{
		int level = arg & ~VME4L_IO_IRQ_ENABLE_DISABLE_MASK;

		if( (level == VME4L_IO_IRQ_ENABLE_BUS_ERROR) ){
			/*
			 * Old VME4L Backward compatibility hack!
			 *
			 * When in non-posted write mode and the user tries
			 * to enable bus error interrupts, install a BUSERR signal
			 * for the current task (to emulate behaviour of old VME4L)
			 */
			VME4L_SIG_INSTALL2 blk;

			blk.vector 	= VME4L_IRQVEC_BUSERR;
			blk.level 	= VME4L_IRQLEV_BUSERR;
			blk.signal	= SIGBUS;
			blk.flags	= VME4L_IRQ_OLDHANDLER;

			if( !(arg & VME4L_IO_IRQ_ENABLE_DISABLE_MASK )){
				rv = 0;
				if( !G_postedWriteMode ){
					rv = vme4l_signal_install( &blk, file );
				}
			}
			else
				rv = vme4l_signal_uninstall( blk.vector, file );

			break;
		}

		VME4L_LOCK_VECTORS(ps);

		if( arg & VME4L_IO_IRQ_ENABLE_DISABLE_MASK )
			rv = G_bDrv->irqLevelCtrl( G_bHandle, level, 0 );
		else
			rv = G_bDrv->irqLevelCtrl( G_bHandle, level, 1 );

		VME4L_UNLOCK_VECTORS(ps);
		break;
	}

	case VME4L_IO_BUS_PROBE:
	{
		VME4L_IO_BUS_PROBE_STRUCT prb;
		VME4L_RW_BLOCK blk;

		if( copy_from_user( &prb, (void *)arg, sizeof(prb)) ){
			rv = -EFAULT;
			break;
		}


		blk.vmeAddr 	= (vmeaddr_t)(u32)prb.vmeAddr;
		blk.direction 	= prb.direction;
		blk.accWidth  	= prb.size;
		blk.size		= prb.size;
		blk.dataP		= prb.buf;
		blk.flags		= 0;

		rv = vme4l_rw( spc, &blk, VME4L_NO_SWAP );
		if( rv >= 0 )
			rv = 0;
		if( rv == -EIO )
			rv = EACCES;
		break;
	}

	case VME4L_IO_IRQ_SIG_INSTALL:
	{
		VME4L_SIG_DATA dat;
		VME4L_SIG_INSTALL2 blk;

		if( copy_from_user( &dat, (void *)arg, sizeof(dat)) ){
			rv = -EFAULT;
			break;
		}
		if( dat.signal != SIGBUS ){
			blk.vector 	= dat.vector;
			blk.level 	= VME4L_IRQLEV_UNKNOWN;
			blk.flags	= dat.irqReleaseMode==VME4L_IO_IRQ_RELEASE_MODE_ROAK ?
				VME4L_IRQ_ROAK : 0;
		}
		else {
			blk.vector 	= VME4L_IRQVEC_BUSERR;
			blk.level 	= VME4L_IRQLEV_BUSERR;
			blk.flags	= VME4L_IRQ_OLDHANDLER;
		}
		blk.signal 	= dat.signal;

		if( dat.install )
			rv = vme4l_signal_install( &blk, file );
		else
			rv = vme4l_signal_uninstall( blk.vector, file );

		break;

	}
	case VME4L_IO_IRQ_GENERATE:
		rv = -ENOTTY;
		if( G_bDrv->irqGenerate )
			rv = G_bDrv->irqGenerate( G_bHandle, VME4L_LEVEL_GET(arg), VME4L_VECTOR_GET(arg));
		if( rv >= 0 )
			rv = 0;
		break;

	case VME4L_IO_GET_IRQ_PENDING:
		rv = -ENOTTY;
		if( G_bDrv->irqGenAcked )
			/* pass default interrupter ID */
			rv = G_bDrv->irqGenAcked( G_bHandle, 1 );

		if( rv >= 0 ){
			rv = !rv;
			if( put_user( rv, (int *)arg ) )
				rv = -EFAULT;
			else
				rv = 0;
		}
		break;

#endif /* __powerpc__ */

	default:
		rv = -ENOTTY;
	}
	return rv;
}


/***********************************************************************/
/** Register VME bridge driver to VME4L core
 *
 * \return 	0=ok, negative error number on error
 *
 */
int vme4l_register_bridge_driver(
	VME4L_BRIDGE_DRV *drv,
	VME4L_BRIDGE_HANDLE *drvData )
{
	if( G_bDrv != NULL )
		return -EBUSY;

	G_bDrv 	  = drv;
	G_bHandle = drvData;
	{
		char buf[200];
		G_bDrv->revisionInfo( G_bHandle, buf );
		printk( KERN_INFO "VME4L bridge driver has registered:\n%s\n", buf );
	}

	return 0;
}

/***********************************************************************/
/** Unregister VME bridge driver to VME4L core
 *
 */
void vme4l_unregister_bridge_driver(void)
{
	G_bDrv    = NULL;
	G_bHandle = NULL;
}

/*---------------------------------------------------------------------+
|                                                                      |
|              LINUX VME KERNEL MODE INTERFACE                         |
|																	   |
|                                                                      |
+---------------------------------------------------------------------*/


/***********************************************************************/
/** Convert VME address to CPU physical address
 *
 *  The returned address will be ioremap'ed by the calling driver.
 *
 *  Note: This kernel interface function needs to be changed in the future.
 *  There is no free/release counterpart for this function, therefore
 *  a VME address window setup by a call to vme_bus_to_phys() cannot
 *  be released anymore...
 *
 *  Another problem is that there is no "requested" size parameter...
 *  Who has created this interface? Was it me?...
 *
 *  \param space	  VME space
 *  \param vmeadrs	  address on VME space
 *  \param physadrs_p pointer to variable where converted physical
 *					  address will be stored
 *
 *  \return             0 | Linux error number (negative)
 *  \retval *physadrs_p converted address
 */
int vme_bus_to_phys( int space, u32 vmeadrs, void **physadrs_p )
{
	VME4L_SPACE spc;
	VME4L_ADRSWIN *win;
	int size = 0x1000;			/* assume this size... */
	int rv;

	/* no bridge registered? abort! */
	if( !G_bDrv )
		return -ENXIO;

	switch( space ){
	case VME_A16_SPACE | VME_D16_ACCESS: spc = VME4L_SPC_A16_D16; break;
	case VME_A16_SPACE | VME_D32_ACCESS: spc = VME4L_SPC_A16_D32; break;
	case VME_A24_SPACE | VME_D16_ACCESS: spc = VME4L_SPC_A24_D16; break;
	case VME_A24_SPACE | VME_D32_ACCESS: spc = VME4L_SPC_A24_D32; break;
	case VME_A32_SPACE | VME_D32_ACCESS: spc = VME4L_SPC_A32_D32; break;
	case VME_CSR_SPACE | VME_D16_ACCESS: spc = VME4L_SPC_A24_D16; break;

	default: return -EINVAL;
	}

	/*--- find/request a ususable VME mapping window ---*/
	if( (rv = vme4l_request_adrswin( spc, vmeadrs, size, 0, &win )) < 0 )
		return rv;

	*physadrs_p = (void *)((vmeadrs - win->vmeAddr) + (char *)win->physAddr);

	return 0;
}

/***********************************************************************/
/** Install a handler for VME interrupt.
 *
 *  This implementation allows multiple handlers to be installed on
 *  the same vector.
 *
 *  This call does not implicitely enable interrupts.
 *  See vme_ilevel_control()
 *
 *  \param vme_irq		VME irq number to install
 *  \param handler		handler to call
 *  \param flags		(currently not used)
 *  \param device		device name registered for this irq
 *  \param dev_id		ptr that is passed to handler
 *
 *  \return             0 | Linux error number (negative)
 */
int VME_REQUEST_IRQ(
	unsigned int vme_irq,
 #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
	void (*handler)(int, void * ),
#else
	void (*handler)(int, void *, struct pt_regs * ),
#endif
	unsigned long flags,
	const char *device,
	void *dev_id
)
{
	VME4L_IRQ_ENTRY *ent;

	/* no bridge registered? abort! */
	if( !G_bDrv )
		return -ENXIO;

	if( (ent = kmalloc( sizeof( *ent ), GFP_KERNEL )) == NULL )
		return -ENOMEM;

	memset( ent, 0, sizeof(*ent));

	ent->flags 					= VME4L_IRQ_ROAK*0;
	ent->level 					= VME4L_IRQLEV_UNKNOWN;
	ent->u.kernel.device		= device;
	ent->u.kernel.dev_id		= dev_id;
	ent->u.kernel.h.lHandler	= handler;

	/* install a LINUX KERNEL irq */
	ent->entType				= VME4L_KERNEL_IRQ;
	return vme4l_irq_install( ent, vme_irq );

}



/***********************************************************************/
/** Remove handler for VME interrupt
 *
 *  \param vme_irq		VME irq number to uninstall
 *  \param dev_id		must match \em dev_id that is passed to
 *						vme_request_irq
 *
 */
void VME_FREE_IRQ(unsigned int vme_irq, void *dev_id)
{
	VME4L_IRQ_ENTRY *ent;
	struct list_head *pos, *tmp;
	unsigned long ps;

	/* no bridge registered? abort! */
	if( !G_bDrv )
		return;
	/* sanity check */
	if( vme_irq >= VME4L_NUM_VECTORS )
		return;

	VME4L_LOCK_VECTORS(ps);

	list_for_each_safe( pos, tmp, &G_vectTbl[vme_irq] ){
		ent = list_entry( pos, VME4L_IRQ_ENTRY, node );

		if( ent->entType == VME4L_KERNEL_IRQ ){
			if( ent->u.kernel.dev_id == dev_id ){
				/* remove entry */
				list_del( &ent->node );

				VME4L_UNLOCK_VECTORS(ps);
				kfree( ent );
				VME4L_LOCK_VECTORS(ps);
			}
		}
	}
	VME4L_UNLOCK_VECTORS(ps);
	return;
}

/***********************************************************************/
/**  Enable or disable a VMEbus interrupt level.
 *
 *  \param level		VME bus irq level to modify (1..7)
 *  \param enable		0 or 1
 *
 *  \return             0 | Linux error number (negative)
 */
int vme_ilevel_control( int level, int enable )
{
	int rv;
	unsigned long ps;

	VME4LDBG("vme_ilevel_control: %sable level %d\n",
			 enable ? "en":"dis", level );

	VME4L_LOCK_VECTORS(ps);

	rv = enable ? vme4l_irqlevel_enable(level) : vme4l_irqlevel_disable(level);

	VME4L_UNLOCK_VECTORS(ps);

	return rv;
}

/*
 *	file operations
 */
static struct file_operations vme4l_fops = {
    .open           = vme4l_open,
    .unlocked_ioctl = vme4l_ioctl,
    .release        = vme4l_release,
    .mmap           = vme4l_mmap
};


/***********************************************************************/
/**  Make vme4l revision info string
 *
 * \a buf shall have a size of 200 bytes.
 */
static char *vme4l_rev_info( char *buf )
{
	char *p = buf;

	p += sprintf( p, "vme4l-core $Revision: 1.18 $,  ");

	if( G_bDrv && G_bDrv->revisionInfo){
		G_bDrv->revisionInfo( G_bHandle, p );
	}
	else {
		p += sprintf( p, "(No bridge driver attached)" );
	}

	return buf;
}

/***********************************************************************/
/**  Read entry point for /proc/vme4l file
 *
 */
#ifndef CONFIG_PROC_FS
static void vme_bridge_procfs_register(void) {}
static void vme_bridge_procfs_unregister(void) {}
#else

static int vme4l_info_proc_show(struct seq_file *m, void *data)
{
	char buf[200];

	seq_printf(m, "%s\n\n", vme4l_rev_info(buf));

	return 0;
}

static int vme4l_window_proc_show(struct seq_file *m, void *data)
{
	VME4L_SPACE_ENT *spcEnt = G_spaceTbl;
	struct list_head *pos, *pos2;
	int spc;

	/*--- master address spaces ---*/
	seq_printf(m, "ADDR SPACES\n");

	VME4L_LOCK_MSTRLISTS();
	for(spc = 0; spc < VME4L_SPACE_TBL_SIZE; spc++, spcEnt++){

		seq_printf(m, "SPACE %d %s\n", spc, spcEnt->devName);
		list_for_each(pos, &spcEnt->lstAdrsWins){
			VME4L_ADRSWIN *win = list_entry(pos, VME4L_ADRSWIN, node);

			seq_printf(m, " ADRSWIN %p: vme=%llx (%x) phys=%p use=%d flg=%x\n",
						win, win->vmeAddr, win->size, win->physAddr,
						win->useCount, win->flags );

			list_for_each( pos2, &win->lstIoremap ){
				VME4L_IOREMAP_REGION *region = list_entry(
					pos2, VME4L_IOREMAP_REGION, winNode );

				seq_printf(m, "  IOREMAPREGION %p: vme=%llx (%x) vaddr=%p "
						   "valid=%d\n",
						   region, region->vmeAddr, region->size,
						   region->vaddr, region->isValid );
			}
		}
	}
	VME4L_UNLOCK_MSTRLISTS();

	return 0;
}

static int vme4l_interrupts_proc_show(struct seq_file *m, void *data)
{
	int tries = 10;
	int i;
	int ret;

	VME4L_IRQ_STAT irqs;
	seq_printf(m, "Interrupts\n");

	if (!G_bDrv) {
		printk(KERN_ERR_PFX "%s: G_bDrv not initialized\n",
		       __func__);
		return -1;
	}

	if (!G_bDrv->getIrqStats) {
		printk(KERN_ERR_PFX "%s: G_bDrv->getIrqStats not initialized\n",
		       __func__);
		return -1;
	}

	while (1) {
		ret = G_bDrv->getIrqStats(G_bHandle, &irqs, sizeof(irqs));
		if (!ret) {
			/* we have a valid and consistent data */
			break;
		}
		tries--;
		udelay(1000);
		if (!tries) {
			printk(KERN_ERR_PFX "%s: Unable to get consistent "
			       "irq stats for the VME bridge!\n", __func__);
			return -1;
		}
	}

	seq_printf(m, "------------------------------------------\n");
	seq_printf(m, "ISR stats:\n");
	seq_printf(m, "VME interrupts                  %10lld\n", irqs.vme);
	seq_printf(m, "Bus error interrupts            %10lld\n", irqs.ber);
	seq_printf(m, "DMA interrupts                  %10lld\n", irqs.dma);
	seq_printf(m, "MailBox interrupts              %10lld\n", irqs.mbox);
	seq_printf(m, "Mocation Monitor interrupts     %10lld\n", irqs.mon);
	seq_printf(m, "Total HW interrupts             %10lld\n", irqs.hw_total);
	seq_printf(m, "Handled interrupts              %10lld\n", irqs.handled);
	seq_printf(m, "Not handled interrupts          %10lld\n", irqs.spurious);
	seq_printf(m, "Handled + spurious              %10lld\n", irqs.handled + irqs.spurious);
	/* If this is more than 0, it means that during at least one
	 * HW interrupt, at least two interrupt sources were handled */
	seq_printf(m, "(Handled + spurious) - total    %10lld\n", (irqs.handled + irqs.spurious) - irqs.hw_total);
	seq_printf(m, "------------------------------------------\n");
	seq_printf(m, "IRQ level unknown               %10lld\n", irqs.levels[VME4L_IRQLEV_UNKNOWN]);
	for (i = 0; i < VME4L_IRQLEV_NUM; i++) {
		seq_printf(m, "IRQ level %d                     %10lld\n", i + VME4L_IRQLEV_1, irqs.levels[i + VME4L_IRQLEV_1]);
	}
	seq_printf(m, "IRQ level bus error             %10lld\n", irqs.levels[VME4L_IRQLEV_BUSERR]);
	seq_printf(m, "IRQ level ACFAIL                %10lld\n", irqs.levels[VME4L_IRQLEV_ACFAIL]);
	seq_printf(m, "IRQ level SYSFAIL               %10lld\n", irqs.levels[VME4L_IRQLEV_SYSFAIL]);
	for (i = 0; i < VME4L_IRQLEV_MBOXWR_NNUM; i++) {
		seq_printf(m, "IRQ level RX mailbox %d          %10lld\n", i, irqs.levels[VME4L_IRQLEV_MBOXRD(i)]);
		seq_printf(m, "IRQ level TX mailbox %d          %10lld\n", i, irqs.levels[VME4L_IRQLEV_MBOXWR(i)]);
	}
	for (i = 0; i < VME4L_IRQLEV_LOCMON_NUM; i++) {
		seq_printf(m, "IRQ level location monitor %2d   %10lld\n", i, irqs.levels[VME4L_IRQLEV_LOCMON(i)]);
	}

	return 0;
}

static int vme4l_irq_proc_show(struct seq_file *m, void *data)
{
	int vector;
	unsigned long ps;
	struct list_head *pos_v;
	VME4L_IRQ_ENTRY *ent;

	/*--- IRQ vectors ---*/
	seq_printf(m, "\n");
	seq_printf(m, "VME VECTORS\n");
	VME4L_LOCK_VECTORS(ps);

	for(vector = 0; vector < VME4L_NUM_VECTORS; vector++){
		if (!list_empty(&G_vectTbl[vector])) {
			seq_printf(m, " Vec %d:\n", vector);

			list_for_each(pos_v, &G_vectTbl[vector]) {
				ent = list_entry(pos_v, VME4L_IRQ_ENTRY, node);
				seq_printf(m, "   Lev %d flg=0x%x",
							ent->level, ent->flags);

				switch(ent->entType){

				case VME4L_USER_IRQ:
					seq_printf(m, ", user sig=%d task=%p\n",
								ent->u.user.signal, ent->u.user.task);
					break;

				case VME4L_KERNEL_IRQ:
					seq_printf(m, ", kernel dev=%s id=%p\n",
						    ent->u.kernel.device,
						    ent->u.kernel.dev_id);
					break;
				}
			}
		}
	}
	VME4L_UNLOCK_VECTORS(ps);

	return 0;
}

static int vme4l_irq_levels_enable_proc_show(struct seq_file *m, void *data)
{
	int level;

	/*--- IRQ levels ---*/

	seq_printf(m, "\n");
	seq_printf(m, "VME LEVELS\n");

	for (level = VME4L_IRQLEV_1; level < VME4L_NUM_LEVELS; level++){

		seq_printf(m, "%d: %d\n", level, G_irqLevEnblCount[level]);
	}
	seq_printf(m, "\n");

	return 0;
}

static int vme4l_supported_bitstreams_proc_show(struct seq_file *m, void *data)
{
	if (G_bDrv && G_bDrv->getSupportedBitstreams){
		G_bDrv->getSupportedBitstreams(m);
	}

	return 0;
}

static int vme4l_supported_bitstreams_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, vme4l_supported_bitstreams_proc_show, NULL);
}

static const struct file_operations vme4l_supported_bitstreams_proc_ops = {
	.open		= vme4l_supported_bitstreams_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int vme4l_info_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, vme4l_info_proc_show, NULL);
}

static const struct file_operations vme4l_info_proc_ops = {
	.open		= vme4l_info_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int vme4l_window_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, vme4l_window_proc_show, NULL);
}

static const struct file_operations vme4l_window_proc_ops = {
	.open		= vme4l_window_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int vme4l_irq_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, vme4l_irq_proc_show, NULL);
}

static const struct file_operations vme4l_irq_proc_ops = {
	.open		= vme4l_irq_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int vme4l_interrupts_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, vme4l_interrupts_proc_show, NULL);
}

static const struct file_operations vme4l_interrupts_proc_ops = {
	.open		= vme4l_interrupts_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int vme4l_irq_levels_enable_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, vme4l_irq_levels_enable_proc_show, NULL);
}

static const struct file_operations vme4l_irq_levels_enable_proc_ops = {
	.open		= vme4l_irq_levels_enable_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void vme_bridge_procfs_register(void)
{
	struct proc_dir_entry *entry;

	vme4l_root = proc_mkdir("vme4l", NULL);

	entry = proc_create("info", S_IFREG | S_IRUGO, vme4l_root, &vme4l_info_proc_ops);
	if (!entry)
		printk(KERN_WARNING "vme4l: Failed to create proc info node\n");

	entry = proc_create("windows", S_IFREG | S_IRUGO, vme4l_root, &vme4l_window_proc_ops);
	if (!entry)
		printk(KERN_WARNING "vme4l: Failed to create proc windows node\n");

	entry = proc_create("irq", S_IFREG | S_IRUGO, vme4l_root, &vme4l_irq_proc_ops);
	if (!entry)
		printk(KERN_WARNING "vme4l: Failed to create proc irq node\n");

	entry = proc_create("interrupts", S_IFREG | S_IRUGO, vme4l_root, &vme4l_interrupts_proc_ops);
	if (!entry)
		printk(KERN_WARNING "vme4l: Failed to create proc interrupts node\n");

	entry = proc_create("irq_levels_enable", S_IFREG | S_IRUGO, vme4l_root, &vme4l_irq_levels_enable_proc_ops);
	if (!entry)
		printk(KERN_WARNING "vme4l: Failed to create proc irq node\n");

	entry = proc_create("supported_bitstreams", S_IFREG | S_IRUGO, vme4l_root, &vme4l_supported_bitstreams_proc_ops);
	if (!entry)
		printk(KERN_WARNING "vme4l: Failed to create proc supported_bitstreams node\n");
}

static void vme_bridge_procfs_unregister(void)
{
	remove_proc_entry("supported_bitstreams", vme4l_root);
	remove_proc_entry("irq_levels_enable", vme4l_root);
	remove_proc_entry("interrupts", vme4l_root);
	remove_proc_entry("irq", vme4l_root);
	remove_proc_entry("windows", vme4l_root);
	remove_proc_entry("info", vme4l_root);
	remove_proc_entry("vme4l", NULL);
}
#endif /* CONFIG_PROC_FS */

static void vme4l_cleanup(void)
{
	vme_bridge_procfs_unregister();

	/*-------------------------+
	|  Cleanup device entries  |
	+-------------------------*/
	{
		int minor;
		VME4L_SPACE_ENT *ent = G_spaceTbl;

		for( minor=0; minor<VME4L_SPACE_TBL_SIZE; minor++, ent++ )
		{
			while( ! list_empty( &ent->lstAdrsWins ) ){
				VME4L_ADRSWIN *win =
					list_entry( ent->lstAdrsWins.next, VME4L_ADRSWIN, node );
				vme4l_discard_adrswin( win );
			}
		}
	}
	unregister_chrdev(major, "vme4l");

}

VME4L_BRIDGE_HANDLE* vme_bridge_get_handle(void)
{
    return G_bHandle;
}

VME4L_BRIDGE_DRV* vme_bridge_get_driver(void)
{
    return G_bDrv;
}

/***********************************************************************/
/**  Modularized driver init function
 *
 *
 */
static int __init vme4l_init_module(void)
{
	int rv;
	{
		char buf[200];
		printk( KERN_INFO "%s\n", vme4l_rev_info( buf ));
	}

	/*------------------------+
	|  Create device entries  |
	+------------------------*/

	if( (rv =       register_chrdev( major, "vme4l", &vme4l_fops )) < 0)
	{
		printk(KERN_ERR_PFX "%s: Unable to get major %d\n",
		       __func__, major);
		goto CLEANUP;
	}

	if( major == 0 )
		major = rv;

	/*--- create spinlocks/waitqueues ---*/
	spin_lock_init( &G_lockMstrLists );
	spin_lock_init( &G_lockVectTbl );
	spin_lock_init( &G_lockDma );
#ifdef CONFIG_SMP
	spin_lock_init( &G_lockFlags );
#endif
	init_waitqueue_head( &G_dmaWq );

	VME4LDBG("vme4l: using major %d\n", major);
  	{
		int minor;
		VME4L_SPACE_ENT *ent = G_spaceTbl;

		for( minor=0; minor<VME4L_SPACE_TBL_SIZE; minor++, ent++ ){
			/* init list headers */
			INIT_LIST_HEAD( &ent->lstAdrsWins );

		}
	}

	/* init all address windows as unused */
	INIT_LIST_HEAD( &G_freeAdrsWins );
	{
		int i;
		VME4L_ADRSWIN *win = G_adrsWinPool;
		memset( G_adrsWinPool, 0, sizeof(G_adrsWinPool));

		for( i=0; i<VME4L_MAX_ADRS_WINS; i++, win++ ){
			list_add_tail( &win->node, &G_freeAdrsWins );

			/* no cached ioremap regions */
			INIT_LIST_HEAD( &win->lstIoremap );
		}
	}

	/* init ioremap cache */
	INIT_LIST_HEAD( &G_lstIoremapCache );
	{
		int i;
		VME4L_IOREMAP_REGION *reg = G_ioremapCache;
		memset( G_ioremapCache, 0, sizeof(G_ioremapCache));

		for( i=0; i<VME4L_MAX_IOREMAP_CACHE; i++, reg++ )
			list_add_tail( &reg->cacheNode, &G_lstIoremapCache );
	}

	/* init IRQ vector lists */
	{
		int i;
		for( i=0; i<VME4L_NUM_VECTORS; i++ )
			INIT_LIST_HEAD( &G_vectTbl[i] );
	}

	/* create proc interface */
	vme_bridge_procfs_register();

	return 0;

 CLEANUP:
	vme4l_cleanup();
	return -ENODEV;
}

static void __exit vme4l_cleanup_module(void)
{
	printk(KERN_INFO "vme4l_exit_module\n");
	vme4l_cleanup();
}

module_init(vme4l_init_module);
module_exit(vme4l_cleanup_module);

MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
MODULE_DESCRIPTION("VME4L - MEN VME core");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(vme4l_get_space_ent);
EXPORT_SYMBOL(vme4l_rw);
EXPORT_SYMBOL(vme4l_register_bridge_driver);
EXPORT_SYMBOL(vme4l_unregister_bridge_driver);
EXPORT_SYMBOL(vme4l_irq);

EXPORT_SYMBOL(VME_REQUEST_IRQ);
EXPORT_SYMBOL(VME_FREE_IRQ);
EXPORT_SYMBOL(vme_bus_to_phys);
EXPORT_SYMBOL(vme_ilevel_control);

EXPORT_SYMBOL(vme_bridge_get_handle);
EXPORT_SYMBOL(vme_bridge_get_driver);

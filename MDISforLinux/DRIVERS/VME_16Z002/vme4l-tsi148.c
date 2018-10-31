/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l-tsi148.c
 *
 *      \author  Ralf Truebenbach / T. Schnuerer
 *
 *        \brief MEN VME bridge driver for Tundra TSI148 (A17/A19/A20...).
 *
 *     Switches: -
 */
/*-------------------------------[ Licence ]---------------------------------
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *---------------------------------------------------------------------------
 * (c) Copyright 2008-2016 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/pci.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
 #include <asm/semaphore.h>
#else
 #include <linux/semaphore.h>
#endif

#include <MEN/men_typs.h>
#ifndef MAC_MEM_MAPPED
 #define MAC_MEM_MAPPED
#endif
#include <MEN/maccess.h>
#include <MEN/oss.h>
#include <MEN/tsi148.h>

#include "vme4l-tsi148.h"

/*-----------------------------+
|  TYPEDEFS					   |
+------------------------------*/
/**	Translation window */
typedef	union _VME4L_TSI148_WIN {
	/* outbound (master) */
	struct {
		uint64_t	spaceEnd;		/**< end addr of VME space */
		uint32_t	otat;			/**< outbound translation attributes */
		uint32_t	otbs;			/**< outbound 2eSST broadcast select */
		size_t		minSize;		/**< minimal size of VME window	*/
	} out;
	/* inbound (slave) */
	struct {
		uint64_t	spaceEnd;		/**< end addr of VME space */
		uint32_t	itat;			/**< outbound translation attributes */
		uint16_t	venId;			/**< target PCI vendor ID */
		uint16_t	devId;			/**< target PCI device ID */
		uint8_t		instance;		/**< instance of target PCI device */
		uint8_t		flags;			/**< flags for target type */
		#define VME4L_TSI148_WIN_FLAGS_KERN		0x80
		#define VME4L_TSI148_WIN_FLAGS_REGS		0x40
		#define VME4L_TSI148_WIN_FLAGS_PCI 		0x20
		#define VME4L_TSI148_WIN_FLAGS_TYP_MASK	0xe0
		#define VME4L_TSI148_WIN_FLAGS_BAR_MASK	0x07
		uint16_t	offset;			/**< offset from PCI BAR  */
	} in;
	/* DMA */
	struct {
		uint64_t	spaceEnd;		/**< end addr of VME space */
		uint32_t	dxat;			/**< DMA translation attributes */
		uint32_t	dxbs;			/**< DMA 2eSST broadcast select */
	} dma;
} VME4L_TSI148_WIN;

/**	structure to maintain variable used	to ioremap bridge regs etc.	*/
typedef	struct _VME4L_RESRC{
	struct resource	pciResrc;	/**< PCI resource */
	size_t			size;		/**< size of region */
	uint64_t		phys;		/**< phys. address */
	void *			vaddr;		/**< mapped	virt. address */
	int				spc;		/**< VME space number */
	int				memReq	:1;	/**< flag memory has been requested */
	int				inUse	:1;	/**< flag resource is in use */
} VME4L_RESRC;

/**	bridge drivers private data	*/
typedef	struct {
	struct pci_dev		*pdev;			/**< PCI device	struct */
	unsigned long		bar;			/**< copy of PCI bars (bus adrs) */
	VME4L_RESRC			regs;			/**< bridge	register */
	VME4L_RESRC			vmeOut[TSI148_OUTBOUND_NO];
	VME4L_RESRC			vmeIn[TSI148_OUTBOUND_NO];
	uint64_t			berrAddr;		/**< VME Exception Address */
	uint32_t			berrAttr;		/**< VME Exception Attributes */
	TSI148_DMA_LL_DESC 	*dmaDescBuf;	/**< virt addr of DMA descriptor
											 buffer */
	char				pciRevision;	/**< PCI revision from cfg space */

	spinlock_t			lockState;		/**< spin lock for VME bridge registers	
											 and handle	state */
	int refCounter;		/**< number of registered clients */
} VME4L_BRIDGE_HANDLE;

#define COMPILE_VME_BRIDGE_DRIVER
#include "vme4l-core.h"


static int debug = DEBUG_DEFAULT;  /**< enable debug printouts */

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Enable debugging printouts (default " \
			M_INT_TO_STR(DEBUG_DEFAULT) ")");

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define	TSI148_ERROR   (-1)
#define	TSI148_OK	   (0)

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0)
#define DEVINIT     __devinit
#define DEVINITDATA __devinitdata
#define DEVEXIT     __devexit
#define DEVEXIT_P   __devexit_p
#else
#define DEVINIT
#define DEVINITDATA
#define DEVEXIT
#define DEVEXIT_P
#endif

/**********************************************************************/
/** \defgroup TSI148_SLVx_TARGET Settings for slave windows.
 * Helper to set-up slave windows target memory locations. In most cases
 * target memory will be kernel memory. But e.g. for VME mailboxes you
 * will need a slave window pointed to the TSI148 registers. Or maybe
 * you want to access another PCI device from the VME side directly
 * (without stressing the CPU).
 *  @{
 */
/** unshared kernel memory (own memory for every addr space) */
#define TSI148_SLVX_UNSHAREDKERNMEM		0, 0, 0, VME4L_TSI148_WIN_FLAGS_KERN, 0
/** shared kernel memory (same memory for all addr spaces) */
#define TSI148_SLVX_SHAREDKERNMEM		#not implemented /*0, 0, 0, 0x81, 0*/
/** TSI148 registers (CRG) */
#define TSI148_SLVX_REGS				0, 0, 0, VME4L_TSI148_WIN_FLAGS_REGS, 0
/** directly to another PCI device (untested) */
#define TSI148_SLVX_PCIDEV( _ven_id_, _dev_id_, _instance_, _bar_, _offset_ ) \
							_ven_id_, _dev_id_, _instance_, \
							VME4L_TSI148_WIN_FLAGS_PCI|((_bar_) \
							& VME4L_TSI148_WIN_FLAGS_BAR_MASK), _offset_
/*! @} */

#define	TSI148_SUBSYS_VEN_ID	TSI148_VEN_ID /**< TSI148 PCI sub vendor ID */
#define	TSI148_SUBSYS_ID		0x0000		/**< TSI148 PCI sub ID */
#define	TSI148_CTRL_SPACE		0x1000		/**< TSI148 register size */

/* Macros to lock accesses to bridge driver	handle and VME bridge regs */
static VME4L_BRIDGE_HANDLE *vme4l_bh;
#define	TSI148_LOCK_STATE()			spin_lock( &vme4l_bh->lockState )
#define	TSI148_UNLOCK_STATE()			spin_unlock( &vme4l_bh->lockState )
#define	TSI148_LOCK_STATE_IRQ(ps)		spin_lock_irqsave( \
											&vme4l_bh->lockState, ps )
#define	TSI148_UNLOCK_STATE_IRQ(ps)		spin_unlock_irqrestore( \
											&vme4l_bh->lockState, ps )

#define TSI148_INTERRUPTER_ID	1	/**< always 1 (since only one interrupter
										 available ) */

/** No. of pages (2^x) used for DMA controller linked-list descriptors
    (note: there is a 5 sec time-out for DMA transfers, this will limit the
    maximal allowed page no. depending on PAGE_SIZE and transfer speed ). */
#define TSI148_DMA_DESC_PAGES	0

/** maximal no. of DMA controller linked-list descriptors */
#define TSI148_DMA_DESC_MAX ( PAGE_SIZE / sizeof(TSI148_DMA_LL_DESC) \
							  * (1u<<TSI148_DMA_DESC_PAGES) )

/* Endianess of	VME	read/writes	is not a matter	of this	driver,	so we use
   raw_read/raw_write functions	since read/write functions may be swapped at
   some	architectures. */
#ifndef __raw_readq
 #define __raw_readq(_addr_) (*(__force volatile __u64 *)(_addr_))
#endif /*__raw_readq*/
#ifndef __raw_writeq
 #define __raw_writeq(_val_, _addr_) (	*(__force volatile __u64 *)(_addr_) \
 										= (_val_))
#endif /*__raw_writeq*/
#define	TSI148_WIN_READ8(vaddr)			__raw_readb(vaddr)
#define	TSI148_WIN_READ16(vaddr)		__raw_readw(vaddr)
#define	TSI148_WIN_READ32(vaddr)		__raw_readl(vaddr)
#define	TSI148_WIN_READ64(vaddr)		__raw_readq(vaddr)
#define	TSI148_WIN_WRITE8(vaddr,val)	__raw_writeb(val,vaddr)
#define	TSI148_WIN_WRITE16(vaddr,val)	__raw_writew(val,vaddr)
#define	TSI148_WIN_WRITE32(vaddr,val)	__raw_writel(val,vaddr)
#define	TSI148_WIN_WRITE64(vaddr,val)	__raw_writeq(val,vaddr)

/* MEN macros to access	TSI148 regs	*/
#define	TSI148_OFFSETOF( _type, _part ) \
		((int)((char*)&((_type*)0)->_part-(char*)0))

#define	TSI148_ASS64(_high,_low) (((uint64_t)(_high)<<32)|((uint64_t)(_low)))
#define	TSI148_DISASS64(_val64,_high,_low) { _high = (uint32_t)((_val64)>>32);\
											 _low = (uint32_t)(_val64); }

/* TSI148 has big endian regs, so we have to swap */
#define	TSI148_SWAP_32(dword)	( ((dword)>>24)	| ((dword)<<24)	| \
								  (((dword)>>8) & 0x0000ff00) |	\
								  (((dword)<<8) & 0x00ff0000) )

#define	TSI148_SWAP_16(word)	( (((word)>>8) & 0x00ff) |	\
								  (((word)<<8) & 0xff00) )

#define	TSI148_CTRL_READ( _reg ) \
		TSI148_SWAP_32( readl( G_vme4l_bh.regs.vaddr \
							   + TSI148_OFFSETOF(TSI148_CRG, _reg) ) )

#define	TSI148_CTRL_WRITE( _reg, _value ) \
		writel(	TSI148_SWAP_32(_value), G_vme4l_bh.regs.vaddr \
				+ TSI148_OFFSETOF(TSI148_CRG, _reg) )

#define	TSI148_DESC_READ( _virtAddr ) \
		TSI148_SWAP_32( readl(_virtAddr) )

#define	TSI148_DESC_WRITE( _virtAddr, _value ) \
		writel(	TSI148_SWAP_32(_value), _virtAddr )

#define	TSI148_CTRL_SETMASK( _reg, _mask ) \
		TSI148_CTRL_WRITE( _reg, TSI148_CTRL_READ(_reg) | (_mask) )

#define	TSI148_CTRL_CLRMASK( _reg, _mask ) \
		TSI148_CTRL_WRITE( _reg, TSI148_CTRL_READ(_reg) & ~(_mask) )


/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static VME4L_BRIDGE_HANDLE G_vme4l_bh;
/** Outbound (master) and inbound (slave) windows settings */
static const VME4L_TSI148_WIN G_winSettings[] = {
	/** #VME4L_SPC_A16_D16 */
	{
		out: { TSI148_MA16D16_END, TSI148_MA16D16_OTAT, TSI148_MA16D16_OTBS,
		TSI148_MA16D16_MINSIZE }
	},
	/** #VME4L_SPC_A16_D16_BLT (n/a) */
	{
		dma: { 0, 0, 0 }
	},
	/** #VME4L_SPC_A16_D32 */
	{
		out: { TSI148_MA16D32_END, TSI148_MA16D32_OTAT, TSI148_MA16D32_OTBS,
		TSI148_MA16D32_MINSIZE }
		
	},
	/** #VME4L_SPC_A16_D32_BLT (n/a) */
	{
		dma: { 0, 0, 0 }
	},
	/** #VME4L_SPC_A24_D16 */
	{
		out: { TSI148_MA24D16_END, TSI148_MA24D16_OTAT, TSI148_MA24D16_OTBS,
		TSI148_MA24D16_MINSIZE }
	},
	/** #VME4L_SPC_A24_D16_BLT (only via DMA) */
	{
		dma: { TSI148_MA24D16BLT_END, TSI148_MA24D16BLT_DXAT,
		TSI148_MA24D16BLT_DXBS }
	},
	/** #VME4L_SPC_A24_D32 */
	{
		out: { TSI148_MA24D32_END, TSI148_MA24D32_OTAT, TSI148_MA24D32_OTBS,
		TSI148_MA24D32_MINSIZE }
	},
	/** #VME4L_SPC_A24_D32_BLT (only via DMA) */
	{
		dma: { TSI148_MA24D32BLT_END, TSI148_MA24D32BLT_DXAT,
		TSI148_MA24D32BLT_DXBS }
	},
	/** #VME4L_SPC_A32_D32 */
	{
		out: { TSI148_MA32D32_END, TSI148_MA32D32_OTAT, TSI148_MA32D32_OTBS,
		TSI148_MA32D32_MINSIZE }
	},
	/** #VME4L_SPC_A32_D32_BLT (only via DMA) */
	{
		dma: { TSI148_MA32D32BLT_END, TSI148_MA32D32BLT_DXAT,
		TSI148_MA32D32BLT_DXBS }
	},
	/** #VME4L_SPC_A32_D64_BLT (MBLT; only via DMA) */
	{
		dma: { TSI148_MA32D64BLT_END, TSI148_MA32D64BLT_DXAT,
		TSI148_MA32D64BLT_DXBS }
	},
	/** #VME4L_SPC_SLV0 */
	{
		in: { TSI148_SLV0_END, TSI148_SLV0_ITAT, TSI148_SLV0_TARGET }
	},
	/** #VME4L_SPC_SLV1 */
	{
		in: { TSI148_SLV1_END, TSI148_SLV1_ITAT, TSI148_SLV1_TARGET }
	},
	/** #VME4L_SPC_SLV2 */
	{
		in: { TSI148_SLV2_END, TSI148_SLV2_ITAT, TSI148_SLV2_TARGET }
	},
	/** #VME4L_SPC_SLV3 */
	{
		in: { TSI148_SLV3_END, TSI148_SLV3_ITAT, TSI148_SLV3_TARGET }
	},
	/** #VME4L_SPC_SLV4 */
	{
		in: { TSI148_SLV4_END, TSI148_SLV4_ITAT, TSI148_SLV4_TARGET }
	},
	/** #VME4L_SPC_SLV5 */
	{
		in: { TSI148_SLV5_END, TSI148_SLV5_ITAT, TSI148_SLV5_TARGET }
	},
	/** #VME4L_SPC_SLV6 */
	{
		in: { TSI148_SLV6_END, TSI148_SLV6_ITAT, TSI148_SLV6_TARGET }
	},
	/** #VME4L_SPC_SLV7 */
	{
		in: { TSI148_SLV7_END, TSI148_SLV7_ITAT, TSI148_SLV7_TARGET }
	},
	/** #VME4L_SPC_MST0 */
	{
		out: { TSI148_MST0_END, TSI148_MST0_OTAT, TSI148_MST0_OTBS,
		TSI148_MST0_MINSIZE }
	},
	/** #VME4L_SPC_MST1 */
	{
		out: { TSI148_MST1_END, TSI148_MST1_OTAT, TSI148_MST1_OTBS,
		TSI148_MST1_MINSIZE }
	},
	/** #VME4L_SPC_MST2 */
	{
		out: { TSI148_MST2_END, TSI148_MST2_OTAT, TSI148_MST2_OTBS,
		TSI148_MST2_MINSIZE }
	},
	/** #VME4L_SPC_MST3 */
	{
		out: { TSI148_MST3_END, TSI148_MST3_OTAT, TSI148_MST3_OTBS,
		TSI148_MST3_MINSIZE }
	},
	/** #VME4L_SPC_MST4 */
	{
		out: { TSI148_MST4_END, TSI148_MST4_OTAT, TSI148_MST4_OTBS,
		TSI148_MST4_MINSIZE }
	},
	/** #VME4L_SPC_MST5 */
	{
		out: { TSI148_MST5_END, TSI148_MST5_OTAT, TSI148_MST5_OTBS,
		TSI148_MST5_MINSIZE }
	},
	/** #VME4L_SPC_MST6 */
	{
		out: { TSI148_MST6_END, TSI148_MST6_OTAT, TSI148_MST6_OTBS,
		TSI148_MST6_MINSIZE }
	},
	/** #VME4L_SPC_MST7 */
	{
		out: { TSI148_MST7_END, TSI148_MST7_OTAT, TSI148_MST7_OTBS,
		TSI148_MST7_MINSIZE }
	},
	/** #VME4L_SPC_A64_D32 */
	{
		out: { TSI148_MA64D32_END, TSI148_MA64D32_OTAT, TSI148_MA64D32_OTBS,
		TSI148_MA64D32_MINSIZE }
	},
	/** #VME4L_SPC_A64_2EVME (only via DMA) */
	{
		dma: { TSI148_MA642EVME_END, TSI148_MA642EVME_DXAT,
		TSI148_MA642EVME_DXBS }
	},
	/** #VME4L_SPC_A64_2ESST (only via DMA) */
	{
		dma: { TSI148_MA642ESST_END, TSI148_MA642ESST_DXAT,
		TSI148_MA642ESST_DXBS }
	}
};
#define VME4L_TSI148_WINSETTINGS_NO \
		(sizeof(G_winSettings)/sizeof(VME4L_TSI148_WIN))


/*----------------------+
 |       FUNCTIONS      |
 +----------------------*/

/***********************************************************************/
/** Get bit mask for requested IRQ level.
 *
 * \param level  \IN  interrupt level
 * \param maskP  \OUT bit mask
 *
 * \return TSI148_OK or TSI148_ERROR
 */
static int Tsi148_IrqLevelToMask(
	int level,
	uint32_t *maskP )
{
	switch( level ) {
		
		case VME4L_IRQLEV_1: *maskP = TSI148_INTEX_IRQ1; break;
		case VME4L_IRQLEV_2: *maskP = TSI148_INTEX_IRQ2; break;
		case VME4L_IRQLEV_3: *maskP = TSI148_INTEX_IRQ3; break;
		case VME4L_IRQLEV_4: *maskP = TSI148_INTEX_IRQ4; break;
		case VME4L_IRQLEV_5: *maskP = TSI148_INTEX_IRQ5; break;
		case VME4L_IRQLEV_6: *maskP = TSI148_INTEX_IRQ6; break;
		case VME4L_IRQLEV_7: *maskP = TSI148_INTEX_IRQ7; break;
		case VME4L_IRQLEV_BUSERR: *maskP = TSI148_INTEX_VERR
										   | TSI148_INTEX_PERR; break;
		case VME4L_IRQLEV_ACFAIL: *maskP = TSI148_INTEX_ACFL; break;
		case VME4L_IRQLEV_SYSFAIL: *maskP = TSI148_INTEX_SYSFL; break;
		case VME4L_IRQLEV_MBOXWR(0): *maskP = TSI148_INTEX_MB0; break;
		case VME4L_IRQLEV_MBOXWR(1): *maskP = TSI148_INTEX_MB1; break;
		case VME4L_IRQLEV_MBOXWR(2): *maskP = TSI148_INTEX_MB2; break;
		case VME4L_IRQLEV_MBOXWR(3): *maskP = TSI148_INTEX_MB3; break;
		case VME4L_IRQLEV_LOCMON(0): *maskP = TSI148_INTEX_LM0; break;
		case VME4L_IRQLEV_LOCMON(1): *maskP = TSI148_INTEX_LM1; break;
		case VME4L_IRQLEV_LOCMON(2): *maskP = TSI148_INTEX_LM2; break;
		case VME4L_IRQLEV_LOCMON(3): *maskP = TSI148_INTEX_LM3; break;
		
		default:
			*maskP = 0;
			return( TSI148_ERROR );			
	}

	return( TSI148_OK );
} /* Tsi148_IrqLevelToMask */


/*******************************************************************/
/** Init bridge regs to a default state.
 */
static void Tsi148_InitBridge( void )
{
	TSI148_CTRL_WRITE( pcfs.pcixcap, TSI148_SWAP_32(TSI148_PCIXCAP) );

	/*--------------------+
	 |  RMW functions     |
	 +-------------------*/
	TSI148_CTRL_WRITE( lcsr.rmwen, 0 );
	TSI148_CTRL_WRITE( lcsr.rmwau, 0 );
	TSI148_CTRL_WRITE( lcsr.rmwal, 0 );
	TSI148_CTRL_WRITE( lcsr.rmwc,  0 );
	TSI148_CTRL_WRITE( lcsr.rmws,  0 );

	/*--------------------------+
	 | In-/Outbound Translation |
	 +-------------------------*/
	/* Outbound Translation */
	{
		uint32_t i;
		for(i=0; i < TSI148_OUTBOUND_NO; i++) {
			TSI148_CTRL_WRITE( lcsr.outbound[i].otat,  0 ); /* disable first */
			TSI148_CTRL_WRITE( lcsr.outbound[i].otsau, 0 );
			TSI148_CTRL_WRITE( lcsr.outbound[i].otsal, 0 );
			TSI148_CTRL_WRITE( lcsr.outbound[i].oteau, 0 );
			TSI148_CTRL_WRITE( lcsr.outbound[i].oteal, 0 );
			TSI148_CTRL_WRITE( lcsr.outbound[i].otofu, 0 );
			TSI148_CTRL_WRITE( lcsr.outbound[i].otofl, 0 );
			TSI148_CTRL_WRITE( lcsr.outbound[i].otbs,  0 );
		}
	}
	/* Inbound Translation */
	{
		uint32_t i;
		for(i=0; i < TSI148_INBOUND_NO; i++) {
			TSI148_CTRL_WRITE( lcsr.inbound[i].itat,  0 ); /* disable first */
			TSI148_CTRL_WRITE( lcsr.inbound[i].itsau, 0 );
			TSI148_CTRL_WRITE( lcsr.inbound[i].itsal, 0 );
			TSI148_CTRL_WRITE( lcsr.inbound[i].iteau, 0 );
			TSI148_CTRL_WRITE( lcsr.inbound[i].iteal, 0 );
			TSI148_CTRL_WRITE( lcsr.inbound[i].itofu, 0 );
			TSI148_CTRL_WRITE( lcsr.inbound[i].itofl, 0 );
		}
	}

	/*---------------------+
	 |  Outbound functions |
	 +--------------------*/
	/* VMEbus Control */
	TSI148_CTRL_WRITE( lcsr.vmctrl, TSI148_VMCTRL );
	TSI148_CTRL_WRITE( lcsr.vctrl,  TSI148_VCTRL );
	TSI148_CTRL_WRITE( lcsr.vstat,  TSI148_VSTAT_CPURST );

	/* VME Filters */
	TSI148_CTRL_WRITE( lcsr.vmefl, TSI148_VMEFL );

	/* VME Exception Attributes */
	TSI148_CTRL_WRITE( lcsr.veat, TSI148_VEAT_VESCL ); 		/* clear */

	/* PCI Error Status */
	TSI148_CTRL_WRITE( lcsr.edpat, TSI148_EDPAT_EDPCL ); 	/* clear */

	/*--------------------+
	 |  Inbound functions |
	 +-------------------*/
	/* Inbound Translation GCSR */
	TSI148_CTRL_WRITE( lcsr.gcsrat, 0 ); /* disable first */
	TSI148_CTRL_WRITE( lcsr.gbau,   0 );
	TSI148_CTRL_WRITE( lcsr.gbal,   0 );

	/* Inbound Translation CRG */
	TSI148_CTRL_WRITE( lcsr.crgat,  0 ); /* disable first */
	TSI148_CTRL_WRITE( lcsr.cbau,   0 );
	TSI148_CTRL_WRITE( lcsr.cbal,   0 );

	/* Inbound Translation CR/CSR */
	TSI148_CTRL_WRITE( lcsr.crou,   0 );
	TSI148_CTRL_WRITE( lcsr.crol,   0 );
	TSI148_CTRL_WRITE( lcsr.crat,   0 );

	/* Inbound Translation Location Monitor */
	TSI148_CTRL_WRITE( lcsr.lmat,   0 ); /* disable first */
	TSI148_CTRL_WRITE( lcsr.lmbau,  0 );
	TSI148_CTRL_WRITE( lcsr.lmbal,  0 );

	/*----------------------+
	 |  Interrupt functions |
	 +---------------------*/
	/* Local Bus Interrupt Control */
	TSI148_CTRL_WRITE( lcsr.inteo, 0 );	/* disable/clear interrupts */
	TSI148_CTRL_WRITE( lcsr.intc,  ~0 );
	TSI148_CTRL_WRITE( lcsr.inten, 0 );
	TSI148_CTRL_WRITE( lcsr.intm1, 0 );	/* all to INTA */
	TSI148_CTRL_WRITE( lcsr.intm2, 0 );

	/* VMEbus Interrupt Control */
	TSI148_CTRL_WRITE( lcsr.bcu64, 0 );
	TSI148_CTRL_WRITE( lcsr.bcl64, 0 );
	TSI148_CTRL_WRITE( lcsr.bpgtr, 0x00000022 );
	TSI148_CTRL_WRITE( lcsr.bpctr, 0x000003e8 );
	TSI148_CTRL_WRITE( lcsr.vicr,  0x0000000f );

	/*----------------------+
	 |  DMA controllers     |
	 +---------------------*/
	/* DMA 0 */
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dctl,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dsau,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dsal,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].ddau,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].ddal,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dsat,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].ddat,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dnlau, 0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dnlal, 0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dcnt,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].ddbs,  0 );

	/* DMA 1 */
	TSI148_CTRL_WRITE( lcsr.dmactl[1].dctl,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].dsau,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].dsal,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].ddau,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].ddal,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].dsat,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].ddat,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].dnlau, 0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].dnlal, 0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].dcnt,  0 );
	TSI148_CTRL_WRITE( lcsr.dmactl[1].ddbs,  0 );

}/* Tsi148_InitBridge */


/*******************************************************************/
/** Get the interrupt with the highest priority
 * (since we only handle one interrupt at once)
 *
 * \param vme4l_bh \IN VME4L bridge handle
 * \param istat	   \IN Value of ISTAT
 *
 * \return IRQ with highest priority
 */
static uint32_t Tsi148_GetIrqWithHighestPrio(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	uint32_t istat )
{
	uint32_t irq = 0;
	int i;
	
	for( i=0; i<(sizeof(G_IrqPriority)/sizeof(uint32_t)); i++ ) {
		if( istat & G_IrqPriority[i] ) {
			irq = G_IrqPriority[i];
			break;
		}
	}
	
	return irq;
}


/*******************************************************************/
/** Process VME IRQ1-7
 *
 * \param vme4l_bh \IN VME4L bridge handle
 * \param istat    \IN pending VME interrupt (TSI148_INTEX_IRQ1-
 *                     TSI148_INTEX_IRQ7)
 * \param vecP	   \OUT VME IRQ vector
 * \param levP	   \OUT VME level
 *
 */
static inline void Tsi148_ProcessVmeInterrupt(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	uint32_t istat,
	int *vecP,
	int *levP )
{
	int no = 0;

	switch( istat ) {
		case TSI148_INTEX_IRQ7:	no = 6;	break;
		case TSI148_INTEX_IRQ6:	no = 5;	break;
		case TSI148_INTEX_IRQ5:	no = 4;	break;
		case TSI148_INTEX_IRQ4:	no = 3;	break;
		case TSI148_INTEX_IRQ3:	no = 2;	break;
		case TSI148_INTEX_IRQ2:	no = 1;	break;
		case TSI148_INTEX_IRQ1:	no = 0;	break;		
		default:
			printk( KERN_ERR "*** vme4l(%s): unsupported IRQ bit 0x%x\n",
					__FUNCTION__, istat );
			return;
	}

	*levP = VME4L_IRQLEV_1 + no;

	/* fetch vector (8bit VME IACK cycle should be supported by most boards) */
	*vecP = readb( vme4l_bh->regs.vaddr
				   + TSI148_OFFSETOF(TSI148_CRG, lcsr.viack[no]) + 3 );

	/* check for bus error during IACK (spurious irq) */
	if( TSI148_CTRL_READ( lcsr.veat ) & TSI148_VEAT_VES ){
		/* clear bus error */
		TSI148_CTRL_WRITE( lcsr.veat, TSI148_VEAT_VESCL );
		*vecP = VME4L_IRQVEC_SPUR;
	}
}


/***************************************************************************/
/** Central HW dependent VME Interrupt handler, processing acknowledge flags
 *
 * \param irq		\IN PIC irq vector (not VME vector!)
 * \param dev_id	\IN device specific handle
 *
 * \return IRQ_NONE or IRQ_HANDLED
 *
 * \brief		This is the standard Linux kernel IRQ handler for VME
 *				bus devices. From here we dispatch everything to vme4l-core
 *
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
static irqreturn_t Tsi148_IrqHandler( int irq, void *dev_id, struct pt_regs *regs )
{
#else
static irqreturn_t Tsi148_IrqHandler( int irq, void *dev_id )
{
	struct pt_regs *regs = NULL;
#endif /*LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)*/

	int vector=0;
	int level=VME4L_IRQLEV_UNKNOWN;
	VME4L_BRIDGE_HANDLE *vme4l_bh 	= (VME4L_BRIDGE_HANDLE *)dev_id;
	uint32_t istat;

	if( vme4l_bh != &G_vme4l_bh ){
		printk( KERN_ERR "*** vme4l(%s): unexpected handle (0x%p!=0x%p)\n",
				__FUNCTION__, vme4l_bh, &G_vme4l_bh );
		return IRQ_NONE;
	}
	
	TSI148_LOCK_STATE();
	
	/* read irq status reg */
	istat = TSI148_CTRL_READ( lcsr.inteo );	/* ignore not enabled interrupts */
	istat &= TSI148_CTRL_READ( lcsr.ints );

	if( istat == 0 ) {
		/* not our IRQ */
		TSI148_UNLOCK_STATE();
		return IRQ_NONE;
	}

	/* get IRQ with highest priority */
	istat = Tsi148_GetIrqWithHighestPrio( vme4l_bh, istat );

	/* clear chosen interrupt */
	TSI148_CTRL_WRITE( lcsr.intc, istat );

	/* pending VME interrupt (perform IACK) */
	if( istat & TSI148_INTEX_IRQX_MASK ) {
		Tsi148_ProcessVmeInterrupt( vme4l_bh, istat, &vector, &level );
	}
	/* other IRQ causes (DMA/Mailbox/location monitor) */
	else if ( istat & TSI148_INTEX_DMAX_MASK ) {
		/* disable DMA interrupt */
		TSI148_CTRL_CLRMASK( lcsr.inteo, TSI148_INTEX_DMA0 );
		TSI148_CTRL_CLRMASK( lcsr.inten, TSI148_INTEX_DMA0 );
		level = VME4L_IRQLEV_DMAFINISHED;
	}
	else if ( istat & TSI148_INTEX_LMX_MASK ) {
		int locMonNo = 0;
		switch( istat ) {
			case TSI148_INTEX_LM0: locMonNo = 0; break;
			case TSI148_INTEX_LM1: locMonNo = 1; break;
			case TSI148_INTEX_LM2: locMonNo = 2; break;
			case TSI148_INTEX_LM3: locMonNo = 3; break;
		}
		level = VME4L_IRQLEV_LOCMON( locMonNo );
		vector = VME4L_IRQVEC_LOCMON( locMonNo );
	}
	else if ( istat & TSI148_INTEX_MBX_MASK ) {
		int mailBoxNo = 0;
		switch( istat ) {
			case TSI148_INTEX_MB0: mailBoxNo = 0; break;
			case TSI148_INTEX_MB1: mailBoxNo = 1; break;
			case TSI148_INTEX_MB2: mailBoxNo = 2; break;
			case TSI148_INTEX_MB3: mailBoxNo = 3; break;
		}
		level = VME4L_IRQLEV_MBOXWR( mailBoxNo );
		vector = VME4L_IRQVEC_MBOXWR( mailBoxNo );
	}
	/* bus errors */
	else {
		switch( istat ) {	
			case TSI148_INTEX_PERR:
				VME4LDBG( "vme4l(%s): PCI exception at 0x%08x_%08x (edpat=0x%08x)\n", __FUNCTION__,
						  TSI148_CTRL_READ(lcsr.edpau),
						  TSI148_CTRL_READ(lcsr.edpal),
						  TSI148_CTRL_READ(lcsr.edpat) );
				/* clear attributes */
				TSI148_CTRL_WRITE( lcsr.edpat, TSI148_EDPAT_EDPCL );
				level = VME4L_IRQLEV_BUSERR;
				vector = VME4L_IRQVEC_BUSERR;
				break;
				
			case TSI148_INTEX_VERR: {
				uint32_t veau = TSI148_CTRL_READ(lcsr.veau);
				uint32_t veal = TSI148_CTRL_READ(lcsr.veal);
				uint32_t veat = TSI148_CTRL_READ(lcsr.veat);
				
				/* remember */
				vme4l_bh->berrAddr = TSI148_ASS64( veau, veal );
				vme4l_bh->berrAttr = veat;
				
				VME4LDBG( "vme4l(%s): VME exception at 0x%08x_%08x (veat="
						  "0x%08x)\n", __FUNCTION__, veau, veal, veat );
				
				/* clear attributes */
				TSI148_CTRL_WRITE( lcsr.veat, TSI148_VEAT_VESCL );
				level = VME4L_IRQLEV_BUSERR;
				vector = VME4L_IRQVEC_BUSERR;
				break;
			}
			
			case TSI148_INTEX_SYSFL:
				level = VME4L_IRQLEV_SYSFAIL;
				vector = VME4L_IRQVEC_SYSFAIL;
				break;
	
			case TSI148_INTEX_ACFL:
				level = VME4L_IRQLEV_ACFAIL;
				vector = VME4L_IRQVEC_ACFAIL;
				break;
			
			default:
				printk( KERN_ERR "*** vme4l(%s): unhandled IRQ (ints bit="
						"0x%x)\n", __FUNCTION__, istat );
		}
	}

	TSI148_UNLOCK_STATE();

	VME4LDBG( "vme4l(%s): VME IRQ (level=0x%x vector=0x%x)\n", __FUNCTION__,
			  level, vector );

	if( level == VME4L_IRQLEV_UNKNOWN ) {
		/* this should never happen */
		printk( KERN_ERR "*** vme4l(%s): driver error\n", __FUNCTION__ );
		return IRQ_NONE;
	}
	
	/* call core IRQ handler */
	vme4l_irq( level, vector, regs );

	return IRQ_HANDLED;
}


/***************************************************************************/
/** Allocate memory space for TSI148 registers
 *
 * \param resourceP \IN memory resource
 * \param nameP     \IN name of mem region
 *
 * \return TSI148_OK or negativ error no.
 */
static int Tsi148_AllocRegSpace(
	VME4L_RESRC *resourceP,
	char *nameP )
{
	uint64_t temp, ba;
	int rv = -EINVAL;

	ba = resourceP->phys;

	VME4LDBG( "vme4l(%s): BAR0=0x%llx (size=0x%llx)\n",
			__FUNCTION__, ba, (uint64_t) resourceP->size );

	if( !request_mem_region( ba, resourceP->size, nameP )) {
		printk( KERN_ERR "*** vme4l(%s): memory already in use\n",
				__FUNCTION__);
		return -EBUSY;
	}
	
	/* map 4K register space into the kernel */
	if( (resourceP->vaddr = ioremap(ba, resourceP->size)) == NULL ) {
		printk( KERN_ERR "*** vme4l(%s): register ioremap failed\n",
				__FUNCTION__ );
		rv = -ENOMEM;
		goto CLEANUP;
	}

	if( (temp = readl(resourceP->vaddr) & 0x0000ffff) != TSI148_VEN_ID ) {
		printk( KERN_ERR "*** vme4l(%s): invalid vendor id 0x%llx\n",
				__FUNCTION__, temp);
		goto CLEANUP;
	}
	
	VME4LDBG( "vme4l(%s): Tsi148 regs phys=0x%llx virt=0x%p size=0x%llx\n",
			  __FUNCTION__, resourceP->phys, resourceP->vaddr,
			  (uint64_t)resourceP->size);

	/* mapped tsi148 register */
	resourceP->memReq 	= 1;
	resourceP->inUse 	= 1;
	rv = TSI148_OK;
	
CLEANUP:
	if( rv != TSI148_OK ) {
		if( resourceP->vaddr != NULL ){
			iounmap(resourceP->vaddr);
		}
		release_mem_region(	resourceP->phys, resourceP->size);
	}
	
	return rv;
}/* Tsi148_AllocRegSpace */


/***************************************************************************/
/** Free alloced memory for TSI148 registers
 *
 * \param resourceP \IN memory resource
 *
 */
static void Tsi148_FreeRegSpace( VME4L_RESRC *resourceP )
{
	/* free allocated memory */
	if( resourceP->vaddr != NULL ){
		iounmap(resourceP->vaddr);
	}
	
	if( resourceP->memReq ) {
		release_mem_region(	resourceP->phys, resourceP->size);
	}
}/* Tsi148_FreeRegSpace */


/***************************************************************************/
/** Determine PCI memory space available for VME outbound windows.
 *
 * \param vme4l_bh \IN VME4L bridge handle
 *
 * \return memory size available (in bytes)
 *
 */
static uint64_t Tsi148_VmeMemoryAvailable( VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	struct resource pciMemRes;
	struct resource *vmePciMemP;
	uint64_t vmeMemSize = 0;

	memset(&pciMemRes, 0, sizeof(pciMemRes));
	pciMemRes.flags = IORESOURCE_MEM;

	vmePciMemP = pci_find_parent_resource(vme4l_bh->pdev, &pciMemRes);

	if( vmePciMemP == NULL ){
		printk( KERN_ERR "*** vme4l(%s): cannot get VME memory space size\n",
				__FUNCTION__);
	}
	else {
		vmeMemSize = (vmePciMemP->end-vmePciMemP->start+1) - TSI148_CTRL_SPACE;
	}

	return vmeMemSize;
}/* Tsi148_VmeMemoryAvailable */


/***************************************************************************/
/** Set VME outbound window.
 *
 * This routine initializes a VME outbound window.
 *
 * \param winNo   \IN window 0 - 7
 * \param spc     \IN VME addr space
 * \param srcAddr \IN source (PCI) address
 * \param dstAddr \IN destination (VME) address
 * \param size    \IN size of Window
 *
 * \return TSI148_OK, or TSI148_ERROR
 *
 */
static int Tsi148_OutboundWinSet(
	unsigned int winNo,
	VME4L_SPACE spc,
	uint64_t srcAddr,
	uint64_t dstAddr,
	uint64_t size )
{
	TSI148_OUTBOUND win;
	uint64_t reg64;

	VME4LDBG( "vme4l(%s): win=%d spc=%d src=0x%llx dst=0x%llx size=0x%llx\n",
			  __FUNCTION__, winNo, spc, srcAddr, dstAddr, size);

	/* check parameters */
	if( winNo >= TSI148_INBOUND_NO || spc >= VME4L_TSI148_WINSETTINGS_NO
		|| srcAddr & ~TSI148_OTOF_OFFL_MASK || size & ~TSI148_OTOF_OFFL_MASK
		|| dstAddr & ~TSI148_OTOF_OFFL_MASK )
	{
		printk( KERN_ERR "*** vme4l(%s): wrong paramter winNo=%u spc=%d "
				"srcAddr=0x%llx dstAddr=0x%llx size=0x%llx\n", __FUNCTION__,
				winNo, spc, srcAddr, dstAddr, size );
		return TSI148_ERROR;
	}

	/* get start address */
	TSI148_DISASS64(srcAddr, win.otsau, win.otsal);

	/* get end address */
	reg64 = srcAddr + size - 0x10000;  /* bits A15-A0 are not compared */
	TSI148_DISASS64(reg64, win.oteau, win.oteal);

	/* get offset */
	reg64 = dstAddr - srcAddr;
	TSI148_DISASS64(reg64, win.otofu, win.otofl);

	/* get 2eSST broadcast select */
	win.otbs = G_winSettings[spc].out.otbs;
	
	/* get outbound attributes */
	win.otat = G_winSettings[spc].out.otat;

	/* set-up registers */
	VME4LDBG( "vme4l(%s):\n", __FUNCTION__ );
	VME4LDBG( "\tOTSA 0x%x_%x\n", win.otsau, win.otsal );
	VME4LDBG( "\tOTEA 0x%x_%x\n", win.oteau, win.oteal );
	VME4LDBG( "\tOTOF 0x%x_%x\n", win.otofu, win.otofl );
	VME4LDBG( "\tOTBS 0x%x\n", win.otbs );
	VME4LDBG( "\tOTAT 0x%x\n", win.otat );

	/* disable window first */
	TSI148_CTRL_CLRMASK( lcsr.outbound[winNo].otat, TSI148_OTAT_EN );

	/* set passed settings */
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].otsau, win.otsau);
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].otsal, win.otsal);
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].oteau, win.oteau);
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].oteal, win.oteal);
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].otofu, win.otofu);
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].otofl, win.otofl);
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].otbs,  win.otbs);
	TSI148_CTRL_WRITE( lcsr.outbound[winNo].otat,  win.otat);

	return TSI148_OK;
} /* Tsi148_OutboundWinSet */


/**********************************************************************/
/** Set VME inbound window.
 *
 * This routine initializes a VME inbound window.
 *
 * \param winNo     \IN window 0 - 7
 * \param spc       \IN VME addr space
 * \param srcSddr   \IN source (VME) address
 * \param dstAddr   \IN destination (PCI) address
 * \param size      \IN size of Window
 *
 * \return TSI148_OK, or TSI148_ERROR
 *
 */
static int Tsi148_InboundWinSet(
	unsigned int winNo,
	VME4L_SPACE spc,
	uint64_t srcAddr,
	uint64_t dstAddr,
	uint64_t size )
{
	TSI148_INBOUND win;
	uint64_t reg64;

	VME4LDBG( "vme4l(%s): win=%d spc=%d src=0x%llx dst=0x%llx size=0x%llx\n",
			  __FUNCTION__, winNo, spc, srcAddr, dstAddr, size);

	/* check parameters */
	if( winNo >= TSI148_INBOUND_NO || spc >= VME4L_TSI148_WINSETTINGS_NO
		|| srcAddr & ~TSI148_ITXAX_MASK || size & ~TSI148_ITXAX_MASK
		|| dstAddr & ~TSI148_ITXAX_MASK )
	{
		printk( KERN_ERR "*** vme4l(%s): wrong paramter winNo=%u spc=%d "
				"srcAddr=0x%llx dstAddr=0x%llx size=0x%llx\n", __FUNCTION__,
				winNo, spc, srcAddr, dstAddr, size );
		return TSI148_ERROR;
	}

	/* get start address */
	reg64 = srcAddr;
	TSI148_DISASS64( reg64, win.itsau, win.itsal );

	/* get end address */
	reg64 = srcAddr + size - 1;
	TSI148_DISASS64( reg64, win.iteau, win.iteal );

	/* get offset */
	reg64 = dstAddr - srcAddr;
	TSI148_DISASS64(reg64, win.itofu, win.itofl);

	/* get inbound attributes */
	win.itat = G_winSettings[spc].in.itat;
	if( size == 0 ) {
		win.itat &= ~TSI148_ITAT_EN;	/* disable */
	}

	/* set-up registers */
	VME4LDBG( "vme4l(%s):\n", __FUNCTION__ );
	VME4LDBG( "\tITSA 0x%08x_%08x\n", win.itsau, win.itsal );
	VME4LDBG( "\tITEA 0x%08x_%08x\n", win.iteau, win.iteal );
	VME4LDBG( "\tITOF 0x%08x_%08x\n", win.itofu, win.itofl );
	VME4LDBG( "\tITAT 0x%08x\n", win.itat );

	/* disable window first */
	TSI148_CTRL_CLRMASK( lcsr.inbound[winNo].itat, TSI148_ITAT_EN );

	/* set passed settings */
	TSI148_CTRL_WRITE( lcsr.inbound[winNo].itsau, win.itsau );
	TSI148_CTRL_WRITE( lcsr.inbound[winNo].itsal, win.itsal );
	TSI148_CTRL_WRITE( lcsr.inbound[winNo].iteau, win.iteau );
	TSI148_CTRL_WRITE( lcsr.inbound[winNo].iteal, win.iteal );
	TSI148_CTRL_WRITE( lcsr.inbound[winNo].itofu, win.itofu );
	TSI148_CTRL_WRITE( lcsr.inbound[winNo].itofl, win.itofl );
	TSI148_CTRL_WRITE( lcsr.inbound[winNo].itat,  win.itat );

	return TSI148_OK;
}/* Tsi148_InboundWinSet */


/**********************************************************************/
/** Allocate PCI memory for outbound windows.
 *
 * \param vme4l_bh  \IN VME4L bridge handle
 * \param winNo     \IN window 0 - 7
 * \param winSize   \IN size of Window
 * \param spc       \IN VME addr space
 *
 * \return TSI148_OK, or negativ error no.
 *
 */
static int Tsi148_AllocOutbResource(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	unsigned int winNo,
	size_t winSize,
	VME4L_SPACE spc )
{
	char *resNameP = NULL;
	VME4L_RESRC *winResP = &vme4l_bh->vmeOut[winNo];
	int rv = -ENOMEM;
	int allocFailed = 1;

	/* Allocate memory for resource name (see /proc/iomem) */
	if ( (resNameP = kmalloc(32, GFP_KERNEL)) == NULL ){
		printk(KERN_ERR "*** vme4l(%s): kmalloc() failed\n", __FUNCTION__);
		goto CLEANUP;
	}
	sprintf(resNameP, "VME Outbound Window %1d (spc %03d)", winNo, spc);

	winResP->size = winSize;
	winResP->pciResrc.name = resNameP;
	winResP->pciResrc.start = 0;
	winResP->pciResrc.end = ~0;
	winResP->pciResrc.flags = IORESOURCE_MEM;

	/* get pci memory area */
	if( (allocFailed = pci_bus_alloc_resource( vme4l_bh->pdev->bus, &winResP->pciResrc, winSize, (~TSI148_OTOF_OFFL_MASK)+1,
							PCIBIOS_MIN_MEM, 0, /* non prefetching */ NULL,	NULL)) != 0 ) {

		printk( KERN_ERR "*** vme4l(%s): failed to allocate PCI memory (size 0x%lx), check BIOS settings\n", __FUNCTION__, winSize );

		goto CLEANUP;
	}

	winResP->phys = winResP->pciResrc.start;

	VME4LDBG( "vme4l(%s): outbound window %d allocated 0x%llx..0x%llx\n", __FUNCTION__, winNo, winResP->pciResrc.start, winResP->pciResrc.end );
	
	winResP->memReq = 1;
	winResP->inUse = 1;
	rv = TSI148_OK;
	
CLEANUP:
	if( rv != TSI148_OK ) {
		if( resNameP ) {
			kfree( resNameP );
		}
		if( !allocFailed ) {
			release_resource(&winResP->pciResrc);
		}
		memset( winResP, 0, sizeof(*winResP) );
	}
	
	return rv;
}/* Tsi148_AllocOutbResource */


/***********************************************************************/
/** Request VME master address window.
 *
 * \param flags     \IN not used
 * \param bDrvDataP \OUT outbound window used
 *
 * \sa requestAddrWindow
 *
 */	
static int Tsi148_RequestAddrWindow(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	VME4L_SPACE spc,
	vmeaddr_t *vmeAddrP,
	size_t *sizeP,
	void **physAddrP,
	int flags,
	void **bDrvDataP )
{
	size_t size = 0;
	int rv = -EINVAL;
	unsigned int winNo;
	VME4L_RESRC* winResP;
	vmeaddr_t startAddr;
	vmeaddr_t endAddr;
	unsigned long ps;

	TSI148_LOCK_STATE_IRQ( ps );

	/* check parameter */
	if( spc >= VME4L_TSI148_WINSETTINGS_NO || spc < VME4L_SPC_A16_D16
		|| spc > VME4L_SPC_A64_D32
		|| (spc>VME4L_SPC_A32_D64_BLT && spc<VME4L_SPC_MST0) ) {
		
		printk( KERN_ERR "*** vme4l(%s): wrong VME addr. space %d\n",
				__FUNCTION__, spc );
		goto CLEANUP;
	}
	
	/* get unused outbound window */
	for( winNo=0; winNo<TSI148_OUTBOUND_NO; winNo++ ) {
		if( !vme4l_bh->vmeOut[winNo].inUse )
			break;	/* free window found */
	}
	if( winNo>=TSI148_OUTBOUND_NO ) {
		VME4LDBG( "vme4l(%s): all outbound windows in use\n", __FUNCTION__ );
		rv = -EBUSY;
		goto CLEANUP;
	}
	winResP = &vme4l_bh->vmeOut[winNo];

	/* determine size & addr */
	startAddr = *vmeAddrP & ~((uint64_t)(G_winSettings[spc].out.minSize-1));
	endAddr = startAddr + G_winSettings[spc].out.minSize;
	while( endAddr < (*vmeAddrP + *sizeP)
		   && endAddr < (endAddr + G_winSettings[spc].out.minSize) ) {
		endAddr += G_winSettings[spc].out.minSize;
	}
	size = endAddr - startAddr;

	/* check determined values */
	if( size < (~TSI148_OTOF_OFFL_MASK+1) ) {
		printk( KERN_ERR "*** vme4l(%s): wrong size 0x%llx\n",
				__FUNCTION__, (uint64_t) size );
		goto CLEANUP;
	}
	if( (endAddr-1) >  G_winSettings[spc].out.spaceEnd ) {

		printk( KERN_ERR "*** vme4l(%s): wrong window end addr 0x%lx\n", __FUNCTION__, endAddr-1 );

		goto CLEANUP;
	}

	VME4LDBG( "vme4l(%s): set-up outbound window %d (vmeAddr=0x%lx size=0x%lx)\n", __FUNCTION__, winNo, startAddr, size);

	/* get PCI memory */
	if( Tsi148_AllocOutbResource(vme4l_bh, winNo, size, spc) != TSI148_OK ){
		goto CLEANUP;
	}

	/* set-up outbound window */
	if( Tsi148_OutboundWinSet( winNo, spc, vme4l_bh->vmeOut[winNo].phys,
							   startAddr, size ) != TSI148_OK ){
		printk( KERN_ERR "*** vme4l(%s): could not set outbound win %d\n",
				__FUNCTION__, winNo);
		rv = -EINVAL;
		goto CLEANUP;
	}

	/* set return values */
	*physAddrP = (void *)(U_INT32_OR_64) vme4l_bh->vmeOut[winNo].phys;
	*sizeP = size;
	*vmeAddrP = startAddr;
	*bDrvDataP = (void *) winNo;
	winResP->spc = spc;

	rv = TSI148_OK;
CLEANUP:
	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_RequestAddrWindow */


/***********************************************************************/
/** Release VME master address window.
 *
 * \param flags     \IN not used
 * \param bDrvDataP \IN outbound window
 *
 * \sa releaseAddrWindow
 *
 */	
static int Tsi148_ReleaseAddrWindow(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	int flags,
	void *bDrvData )
{
	unsigned long ps;
	unsigned int winNo;
	int rv = -EINVAL;

	TSI148_LOCK_STATE_IRQ( ps );

	winNo = (unsigned long) bDrvData;

	if( !vme4l_bh || winNo >= TSI148_OUTBOUND_NO ) {
		printk( KERN_ERR "*** vme4l(%s): could not release win no. %d",
				__FUNCTION__, winNo );
		goto CLEANUP;	
	}

	if( vme4l_bh->vmeOut[winNo].memReq ) {
		
		VME4LDBG( "vme4l(%s): release outbound window %d\n",
				  __FUNCTION__, winNo );
		
		if( vme4l_bh->vmeOut[winNo].pciResrc.name ) {
			kfree( vme4l_bh->vmeOut[winNo].pciResrc.name );
		}
		release_resource( &vme4l_bh->vmeOut[winNo].pciResrc );
		
		memset( &vme4l_bh->vmeOut[winNo], 0, sizeof(vme4l_bh->vmeOut[winNo]) );
	}

	rv = TSI148_OK;
CLEANUP:
	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_ReleaseAddrWindow */


/**********************************************************************/
/** Perform VME Read-Modify-Write cycle.
 *
 * \sa rmwCycle
 *
 */
static int Tsi148_RmwCycle(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	void *vaddr,
	void *physAddr,
	int accWidth,
	uint32_t mask,
	uint32_t *rvP )
{
	unsigned long ps;
	int rv = -EINVAL;
	uint64_t pciAddr = (U_INT32_OR_64) physAddr;
	
	/* remember: bit 1 & 0 of RMWAL are ignored */
	if( accWidth == 1 ) {
		mask <<= (~pciAddr & 0x3) * 8;
	}
	else if( accWidth == 2 ) {
		mask <<= (~pciAddr & 0x2) * 8;
	}
	pciAddr &= ~0x3ULL;

	VME4LDBG( "vme4l(%s): vaddr=0x%p physAddr=0x%p pciAddr=0x%llx mask=0x%x\n",
			  __FUNCTION__, vaddr, physAddr, pciAddr, mask );

	TSI148_LOCK_STATE_IRQ( ps );

	/* set-up RMW registers */
	TSI148_CTRL_CLRMASK( lcsr.vmctrl, TSI148_VMCTRL_RMWEN );
	TSI148_CTRL_WRITE( lcsr.rmwau, (uint32_t)(pciAddr>>32) );
	TSI148_CTRL_WRITE( lcsr.rmwal, (uint32_t)pciAddr );
	TSI148_CTRL_WRITE( lcsr.rmwen, mask );	/* OR */
	TSI148_CTRL_WRITE( lcsr.rmwc, 0x0 );
	TSI148_CTRL_WRITE( lcsr.rmws, ~0x0 );
	TSI148_CTRL_SETMASK( lcsr.vmctrl, TSI148_VMCTRL_RMWEN );
	
	/* clear bus errors */
	TSI148_CTRL_WRITE( lcsr.veat, TSI148_VEAT_VESCL );
	
	switch( accWidth ) {
		case 1:
			*rvP = readb( vaddr );
			break;
		case 2:
			*rvP = TSI148_SWAP_16( readw( vaddr ) );
			break;
		case 4:
			*rvP = TSI148_SWAP_32( readl( vaddr ) );
			break;
		default:
			printk( KERN_ERR "*** vme4l(%s): wrong accWidth %d",
					__FUNCTION__, accWidth );
			goto CLEANUP;
	}
	
	/* check for bus errors */
	if( TSI148_CTRL_READ(lcsr.veat) & TSI148_VEAT_VES ) {
		VME4LDBG( "vme4l(%s): VME bus error at 0x%08x_%08x (veat=0x%08x)\n",
				  __FUNCTION__, TSI148_CTRL_READ(lcsr.veau),
				  TSI148_CTRL_READ(lcsr.veal), TSI148_CTRL_READ(lcsr.veat) );
		TSI148_CTRL_WRITE(lcsr.veat, TSI148_VEAT_VESCL);
		rv = -EIO;
		goto CLEANUP;
	}
	
	rv = TSI148_OK;
CLEANUP:
	TSI148_CTRL_CLRMASK( lcsr.vmctrl, TSI148_VMCTRL_RMWEN );
	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}


/***********************************************************************/
/** Request VME slave address window in kernel memory.
 *
 * \param vme4l_bh \IN VME4L bridge handle
 * \param winResP  \OUT allocated window resource
 * \param size     \IN size of window
 * \param dmaAddrP \OUT phys. address
 *
 * \return TSI148_OK or TSI148_ERROR
 */	
static inline int Tsi148_SlaveWindowAllocKernSpc(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	VME4L_RESRC *winResP,
	size_t size,
	dma_addr_t *dmaAddrP )
{
	winResP->vaddr = pci_alloc_consistent( vme4l_bh->pdev, size, dmaAddrP);
	VME4LDBG( "vme4l(%s): alloced PCI mem virt=0x%p phys=0x%llx (0x%llx)\n",
			  __FUNCTION__, winResP->vaddr, (uint64_t) *dmaAddrP,
			  (uint64_t) size );
	
	if( winResP->vaddr == NULL ){
		printk( KERN_ERR "*** vme4l(%s): cannot alloc BM slave window of "
				"0x%llx bytes\n", __FUNCTION__, (uint64_t) size );
		return TSI148_ERROR;
	}
	else {
		winResP->memReq	= 1;
	}
	
	/* clear region */
	memset( winResP->vaddr, 0, size );

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,10)
	/*
	 * The SetPageReserved() is an important issue!
	 * If this is not done, remap_page_range don't work
	 * for BM slave windows (returns 0, but maps zero pages)
	 *
	 * Note: SetPageReserved does the same as
	 * mem_map_reserve(page), but exists for 2.4 and 2.6
	 */
	{
		struct page *page, *pend;
		void *rawbuf = winResP->vaddr;
		pend = virt_to_page(rawbuf + size - 1);
	
		for( page = virt_to_page(rawbuf); page <= pend; page++ ) {
			SetPageReserved(page);
		}
	}
#endif

	return TSI148_OK;
}


/***********************************************************************/
/** Request VME slave address window.
 *
 * \param bDrvDataP  \OUT inbound window used
 *
 * \sa slaveWindowCtrl
 */	
static int Tsi148_SlaveWindowCtrl(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	void **physAddrP,
	void **bDrvDataP
){
	int rv = -EINVAL;
	unsigned int winNo;
	VME4L_RESRC* winResP;
	dma_addr_t dmaAddr = 0;
	vmeaddr_t vmeMask;
	unsigned long ps;
	
	TSI148_LOCK_STATE_IRQ( ps );

	VME4LDBG( "vme4l(%s): spc=%d vmeAddr=0x%lx size=0x%lx\n",  __FUNCTION__, spc, vmeAddr, size);

	/* check parameters */
	if( spc >= VME4L_TSI148_WINSETTINGS_NO || spc < VME4L_SPC_SLV0
		|| spc > VME4L_SPC_SLV7  ) {
		printk( KERN_ERR "*** vme4l(%s): wrong VME addr. space %d\n",
				__FUNCTION__, spc );
		goto CLEANUP;
	}
	switch( G_winSettings[spc].in.itat & TSI148_ITAT_AS_MASK ) {
		case TSI148_ITAT_AS_A64: vmeMask=TSI148_ITXAX_MASK_A64; break;
		case TSI148_ITAT_AS_A32: vmeMask=TSI148_ITXAX_MASK_A32; break;
		case TSI148_ITAT_AS_A24: vmeMask=TSI148_ITXAX_MASK_A24; break;
		default: vmeMask=TSI148_ITXAX_MASK_A16; break;
	}
	if( vmeAddr & ~vmeMask ) {
		printk( KERN_ERR "*** vme4l(%s): VME addr 0x%lx invalid\n",
				__FUNCTION__, vmeAddr );
		goto CLEANUP;
	}
	if( size & ~vmeMask ) {
		printk( KERN_ERR "*** vme4l(%s): size 0x%llx invalid\n",
				__FUNCTION__, (uint64_t) size );
		goto CLEANUP;
	}
	if( (vmeAddr+size-1) > G_winSettings[spc].in.spaceEnd ) {
		printk( KERN_ERR "*** vme4l(%s): 0x%lx..0x%lx outside addr range\n",
				__FUNCTION__, vmeAddr, vmeAddr+size-1 );
		goto CLEANUP;
	}
	
	/* get inbound window */
	for( winNo=0; winNo<TSI148_INBOUND_NO; winNo++ ) {
		if( spc == vme4l_bh->vmeIn[winNo].spc ) {
			break;	/* already existing window found */
		}
	}
	
	if( winNo>=TSI148_INBOUND_NO ) {
		/* no existing window -> get unused one */
		for( winNo=0; winNo<TSI148_INBOUND_NO; winNo++ ) {
			if( !vme4l_bh->vmeIn[winNo].inUse ) {
				break;	/* free window found */
			}
		}

		if( winNo>=TSI148_INBOUND_NO ) {
			printk( KERN_ERR "*** vme4l(%s): all inbound windows in use\n",
					__FUNCTION__ );
			rv = -EBUSY;
			goto CLEANUP;
		}
	}
	winResP = &vme4l_bh->vmeIn[winNo];

	if( size ) {
		/* alloc window... */
		if( winResP->size ) {
			
			/* already setup */
			if( winResP->size != size ){
				printk( KERN_ERR "*** vme4l(%s): cannot change slave window "
						"size if used\n", __FUNCTION__ );
				rv = -EBUSY;
				goto CLEANUP;
			}
			else {
				/* phys addr remains unchanged */
				dmaAddr = winResP->phys;
			}
		}
		else {

			/* currently not setup, allocate a new one */
			switch( G_winSettings[spc].in.flags
					& VME4L_TSI148_WIN_FLAGS_TYP_MASK ) {
			
				case VME4L_TSI148_WIN_FLAGS_KERN:
					if( Tsi148_SlaveWindowAllocKernSpc( vme4l_bh, winResP,
						size, &dmaAddr ) != TSI148_OK ) {
						rv = -ENOSPC;
						goto CLEANUP;
					}
					else {
						winResP->inUse = 1;
					}
					break;
				
				case VME4L_TSI148_WIN_FLAGS_REGS:
					dmaAddr = vme4l_bh->regs.phys;
					winResP->inUse = 1;
					break;
				
				case VME4L_TSI148_WIN_FLAGS_PCI: {
					struct pci_dev *dev = NULL;
					u_int8 i = 0;
					u_int8 bar = G_winSettings[spc].in.flags
								 & VME4L_TSI148_WIN_FLAGS_BAR_MASK;
					
					if( bar > 5 ) {
						goto CLEANUP;
					}
					while( (dev = pci_get_device(G_winSettings[spc].in.venId,
							G_winSettings[spc].in.devId, dev)) ) {
						if( i++ == G_winSettings[spc].in.instance ) {
							dmaAddr = dev->resource[bar].start;
						}
					}
					
					if( !dmaAddr ) {
						printk( KERN_ERR "*** vme4l(%s): PCI target not "
								"found\n", __FUNCTION__ );
					}
					else {
						winResP->inUse = 1;
					}
					break;
				}
				
				default:
					printk( KERN_ERR "*** vme4l(%s): wrong flags in "
							"G_winSettings[%d].in.flags (0x%x)\n",
							__FUNCTION__, spc, G_winSettings[spc].in.flags );
					rv = -EINVAL;
					goto CLEANUP;
			}

		}
	}
	else {
		/* free window... */
		if( winResP->memReq ) {
			pci_free_consistent(vme4l_bh->pdev, winResP->size, winResP->vaddr,
								winResP->phys);
		}
		winResP->memReq = 0;
		winResP->inUse = 0;
		winResP->size = 0;
		spc = 0;
	}
	
	VME4LDBG( "vme4l(%s): %s inbound window %d (vmeAddr=0x%lx size=0x%lx)\n",
			  __FUNCTION__, (size) ? "set-up" : "disable", winNo, vmeAddr, size );
	
	if( Tsi148_InboundWinSet(winNo, spc, vmeAddr, dmaAddr, size)
		!= TSI148_OK ){
		printk( KERN_ERR "*** vme4l(%s): could not set inbound win %d\n",
				__FUNCTION__, winNo);
		rv = -EINVAL;
		goto CLEANUP;
	}

	/* set return values */
	winResP->phys = dmaAddr;
	winResP->size = size;
	winResP->spc = spc;
	*physAddrP = (void *) dmaAddr;
	*bDrvDataP = (void *) winNo;

	rv = TSI148_OK;
CLEANUP:
	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_SlaveWindowCtrl */


/***********************************************************************/
/** Read PIO functions.
 *
 * Bus error proof, but slow.
 *
 * \sa readPio8, readPio16, readPio32
 *
 */
#define TSI148_READ_PIO_XX(size,type) \
static int Tsi148_ReadPio##size ( \
	VME4L_BRIDGE_HANDLE *vme4l_bh,\
	void *vaddr,\
	type *dataP,\
	int flags,\
	void *bDrvData)\
{\
	unsigned long ps;\
	int rv = 0;\
	\
	TSI148_LOCK_STATE_IRQ( ps );\
	\
	/* clear bus errors */\
	TSI148_CTRL_WRITE(lcsr.veat, TSI148_VEAT_VESCL);\
	\
	/* read */\
	*dataP = TSI148_WIN_READ##size(vaddr);\
	\
	/* check for bus errors */\
	if( TSI148_CTRL_READ(lcsr.veat) & TSI148_VEAT_VES ) {\
		VME4LDBG( "*** vme4l(%s): VME bus error at 0x%08x_%08x (veat="\
				  "0x%08x)\n", __FUNCTION__, TSI148_CTRL_READ(lcsr.veau),\
				  TSI148_CTRL_READ(lcsr.veal), TSI148_CTRL_READ(lcsr.veat) );\
		TSI148_CTRL_WRITE(lcsr.veat, TSI148_VEAT_VESCL);\
		rv = -EIO;\
	}\
	\
	TSI148_UNLOCK_STATE_IRQ( ps );\
	return rv;\
}/* Tsi148_ReadPio## */

TSI148_READ_PIO_XX(  8, uint8_t  )
TSI148_READ_PIO_XX( 16, uint16_t )
TSI148_READ_PIO_XX( 32, uint32_t )


/***********************************************************************/
/** Write PIO functions.
 *
 * Bus error proof (not 100% since posted writes cannot be disabled), but slow.
 *
 * \sa writePio8, writePio16, writePio32
 *
 */
#define TSI148_WRITE_PIO_XX(size,type) \
static int Tsi148_WritePio##size ( \
	VME4L_BRIDGE_HANDLE *vme4l_bh,\
	void *vaddr,\
	type *dataP,\
	int flags,\
	void *bDrvData)\
{\
	unsigned long ps;\
	int rv = 0;\
	\
	TSI148_LOCK_STATE_IRQ( ps );\
	\
	/* clear bus errors */\
	TSI148_CTRL_WRITE(lcsr.veat, TSI148_VEAT_VESCL);\
	\
	/* write */\
	TSI148_WIN_WRITE##size(vaddr,*dataP);\
	\
	/* check for bus errors */\
	if( TSI148_CTRL_READ(lcsr.veat) & TSI148_VEAT_VES ) {\
		VME4LDBG( "*** vme4l(%s): VME bus error at 0x%08x_%08x (veat="\
				  "0x%08x)\n", __FUNCTION__, TSI148_CTRL_READ(lcsr.veau),\
				  TSI148_CTRL_READ(lcsr.veal), TSI148_CTRL_READ(lcsr.veat) );\
		TSI148_CTRL_WRITE(lcsr.veat, TSI148_VEAT_VESCL);\
		rv = -EIO;\
	}\
	\
	TSI148_UNLOCK_STATE_IRQ( ps );\
	\
	return rv;\
}/* Tsi148_WritePio## */

TSI148_WRITE_PIO_XX(  8, uint8_t  )
TSI148_WRITE_PIO_XX( 16, uint16_t )
TSI148_WRITE_PIO_XX( 32, uint32_t )


/**********************************************************************/
/** Read mailbox value.
 *
 * \sa mboxRead
 *
 */
int Tsi148_MboxRead(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int mbox,
	uint32_t *valP)
{
	if( mbox<0 || mbox>3 )
		return -EINVAL;

	*valP = TSI148_CTRL_READ( gcsr.mbox[mbox] );
	
	return 0;
}

/**********************************************************************/
/** Write mailbox value.
 *
 * \sa mboxWrite
 *
 */
int Tsi148_MboxWrite(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int mbox,
	uint32_t val)
{
	uint32_t inten;
	unsigned long ps;

	if( mbox<0 || mbox>3 )
		return -EINVAL;
	
	TSI148_LOCK_STATE_IRQ( ps );	
	
	/* disable mbox IRQ */
	inten = TSI148_CTRL_READ( lcsr.inten );
	TSI148_CTRL_WRITE( lcsr.inten, inten & ~(TSI148_INTEX_MB0<<mbox) );
	
	/* write to mbox */
	TSI148_CTRL_WRITE( gcsr.mbox[mbox], val );

	/* restore IRQ settings */
	TSI148_CTRL_WRITE( lcsr.inten, inten );

	TSI148_UNLOCK_STATE_IRQ( ps );
	
	return 0;
}


/**********************************************************************/
/** Read location monitor reg.
 *
 * \sa locMonRegRead
 *
 */
static int Tsi148_LocMonRegRead(
	VME4L_BRIDGE_HANDLE *h,
	int reg,
	uint32_t *valP)
{
	switch( reg ){
	case 0:
		*valP = TSI148_CTRL_READ( lcsr.lmbau );
		break;
	case 1:
		*valP = TSI148_CTRL_READ( lcsr.lmbal );
		break;
	case 2:
		*valP = TSI148_CTRL_READ( lcsr.lmat );
		break;
	default:
		return -EINVAL;
	}

	return 0;		
}


/**********************************************************************/
/** Write location monitor value (addresses must be 32 byte alligned).
 *
 * \sa locMonRegWrite
 *
 */
static int Tsi148_LocMonRegWrite(
	VME4L_BRIDGE_HANDLE *h,
	int reg,
	uint32_t val)
{
	switch( reg ){
	case 0:
		TSI148_CTRL_WRITE( lcsr.lmbau, val );
		break;
	case 1:
		if( val & ~TSI148_LMBA_MASK ) {
			VME4LDBG( "*** vme4l(%s): warning, address should be 32 byte "
					  "alligned (see TSI148 User Manual)\n", __FUNCTION__ );
		}
		TSI148_CTRL_WRITE( lcsr.lmbal, val );
		break;
	case 2:
		TSI148_CTRL_WRITE( lcsr.lmat, val );
		break;
	default:
		return -EINVAL;
	}

	return 0;
}


/***********************************************************************/
/** Write DMA controller linked-list descriptors.
 *
 * \sa dmaSetup
 *
 */
static int Tsi148_DmaSetup(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	VME4L_SPACE spc,
	VME4L_SCATTER_ELEM *sgList,
	int sgNelems,
	int direction,
	int swapMode,
	vmeaddr_t *vmeAddr,
	dma_addr_t *dmaAddr,
	int *dmaLeft,
	int vme_block_size,
	int flags)
{
	int i;
	int rv=-EINVAL;
	int endBd;
	uint64_t tmp64;
	TSI148_DMA_LL_DESC *bdVirtP;

	/* check parameter */
	if( spc >= VME4L_TSI148_WINSETTINGS_NO
		|| (spc >= VME4L_SPC_SLV0 && spc <= VME4L_SPC_SLV7)  ) {
		printk( KERN_ERR "*** vme4l(%s): wrong VME addr. space %d\n",
				__FUNCTION__, spc );
		goto CLEANUP;
	}
	
	/* TSI148 does not support hardware swap mode */
	if( swapMode & VME4L_HW_SWAP1 ) {
		printk( KERN_ERR "*** vme4l(%s): VME4L_HW_SWAP1 not supported\n",
				__FUNCTION__ );
		goto CLEANUP;
	}
	
	/* get buffer for DMA linked-list descriptors */
	if( vme4l_bh->dmaDescBuf == NULL ) {
		if( (bdVirtP = (TSI148_DMA_LL_DESC*) __get_free_pages(GFP_KERNEL,
			TSI148_DMA_DESC_PAGES)) == NULL ) {
	
			printk( KERN_ERR "*** vme4l(%s): could not alloc mem for DMA "
					"linked-list descriptors\n", __FUNCTION__ );
			rv = -ENOMEM;
			goto CLEANUP;
		}
		vme4l_bh->dmaDescBuf = bdVirtP;
	}
	else {
		bdVirtP = vme4l_bh->dmaDescBuf;
	}
	endBd = (sgNelems < TSI148_DMA_DESC_MAX) ? sgNelems : TSI148_DMA_DESC_MAX;
	VME4LDBG( "vme4l(%s): bdVaddr=0x%p endBd=%d\n", __FUNCTION__,
			  bdVirtP, endBd );

	/* tell TSI148 where to find linked-list (at PCI bus) */
	tmp64 = virt_to_bus(bdVirtP);
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dnlau, (uint32_t) (tmp64>>32) );
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dnlal, (uint32_t) tmp64 );

	/* setup linked-list */
	for( i=0; i<endBd; i++, sgList++, bdVirtP++ ){

		if( direction ){
			/* write to VME */
			tmp64 = sgList->dmaAddress;
			TSI148_DESC_WRITE( &bdVirtP->dsau, (uint32_t)(tmp64>>32) );
			TSI148_DESC_WRITE( &bdVirtP->dsal, (uint32_t) tmp64 );
			TSI148_DESC_WRITE( &bdVirtP->ddau, (uint32_t)(*vmeAddr>>32) );
			TSI148_DESC_WRITE( &bdVirtP->ddal, (uint32_t) *vmeAddr );
			TSI148_DESC_WRITE( &bdVirtP->dsat, TSI148_DXAT_TYP_PCIX );
			TSI148_DESC_WRITE( &bdVirtP->ddat, TSI148_DXAT_TYP_VME
							   | ( G_winSettings[spc].dma.dxat &
							     (TSI148_DXAT_SSTM_MASK|TSI148_DXAT_TM_MASK
							      |TSI148_DXAT_DBW_MASK|TSI148_DXAT_SUP
								  |TSI148_DXAT_PGM|TSI148_DXAT_AM_MASK) ) );	
		}
		else {
			/* read from VME */
			TSI148_DESC_WRITE( &bdVirtP->dsau, (uint32_t)(*vmeAddr>>32) );
			TSI148_DESC_WRITE( &bdVirtP->dsal, (uint32_t) *vmeAddr );
			tmp64 = sgList->dmaAddress;
			TSI148_DESC_WRITE( &bdVirtP->ddau, (uint32_t)(tmp64>>32) );
			TSI148_DESC_WRITE( &bdVirtP->ddal, (uint32_t) tmp64 );
			TSI148_DESC_WRITE( &bdVirtP->dsat, TSI148_DXAT_TYP_VME
							   | ( G_winSettings[spc].dma.dxat &
							     (TSI148_DXAT_SSTM_MASK|TSI148_DXAT_TM_MASK
							      |TSI148_DXAT_DBW_MASK|TSI148_DXAT_SUP
								  |TSI148_DXAT_PGM|TSI148_DXAT_AM_MASK) ) );	
			TSI148_DESC_WRITE( &bdVirtP->ddat, TSI148_DXAT_TYP_PCIX );
		}
		tmp64 = virt_to_bus(bdVirtP+1);
		TSI148_DESC_WRITE( &bdVirtP->dnlau, (uint32_t) (tmp64>>32) );
		TSI148_DESC_WRITE( &bdVirtP->dnlal, (uint32_t) tmp64
						   | ((i==endBd-1) ? TSI148_DNLAL_LLA : 0) );
		TSI148_DESC_WRITE( &bdVirtP->dcnt, sgList->dmaLength );
		TSI148_DESC_WRITE( &bdVirtP->ddbs, G_winSettings[spc].dma.dxbs );

		*vmeAddr += sgList->dmaLength;
	}

#ifdef DBG
    {
		uint32_t *p = (uint32_t*) vme4l_bh->dmaDescBuf;

		for(i=0; i<endBd; i++ ){
			if( i<3 || i>=endBd-3 ) {
				VME4LDBG( " DMA BD %d @0x%p: 0x%08x_%08x -> 0x%08x_%08x\n",
						  i, p,
						  TSI148_DESC_READ( &p[0] ), TSI148_DESC_READ( &p[1] ),
						  TSI148_DESC_READ( &p[2] ), TSI148_DESC_READ( &p[3] )
						);
				VME4LDBG( "%22s 0x%08x 0x%08x 0x%08x 0x%08x\n",
						  "",
						  TSI148_DESC_READ( &p[4] ), TSI148_DESC_READ( &p[5] ),
						  TSI148_DESC_READ( &p[6] ), TSI148_DESC_READ( &p[7] )
						);
			}
			else if( i==3 ) {
				VME4LDBG( " ...\n" );
			}
			p+=sizeof(TSI148_DMA_LL_DESC)/sizeof(uint32_t);
		}
	}
#endif
	
	rv = endBd;
 CLEANUP:
	
	return rv;
}


/***********************************************************************/
/** Start DMA with the linked-list setup by dmaSetup.
 *
 * \sa dmaStart
 *
 */
static int Tsi148_DmaStart( VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	if( TSI148_CTRL_READ( lcsr.dmactl[0].dsta ) & TSI148_DSTA_BSY ){
		printk( KERN_ERR "*** vme4l(%s): DMA busy\n", __FUNCTION__ );
		return -EBUSY;
	}
	
	/* enable DMA interrupt */
	TSI148_CTRL_SETMASK( lcsr.inteo, TSI148_INTEX_DMA0 );
	TSI148_CTRL_SETMASK( lcsr.inten, TSI148_INTEX_DMA0 );
	
	/* start DMA */
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dctl,
					   TSI148_DCTL_DGO | TSI148_BLT_DCTL );
	
	return 0;
}


/***********************************************************************/
/** Stop DMA transfer.
 *
 * \sa dmaStop
 *
 */
static int Tsi148_DmaStop( VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	/* disable DMA interrupt */
	TSI148_CTRL_CLRMASK( lcsr.inteo, TSI148_INTEX_DMA0 );
	TSI148_CTRL_CLRMASK( lcsr.inten, TSI148_INTEX_DMA0 );
	
	/* abort DMA */
	TSI148_CTRL_WRITE( lcsr.dmactl[0].dctl, TSI148_DCTL_ABT );

	return 0;
}


/***********************************************************************/
/** Get DMA status (after DMA was started with dmaStart).
 *
 * \sa dmaStatus
 *
 */
static int Tsi148_DmaStatus( VME4L_BRIDGE_HANDLE *h )
{
	uint32_t dsta = TSI148_CTRL_READ( lcsr.dmactl[0].dsta );

	VME4LDBG("vme4l(%s): dsta=0x%08x\n", __FUNCTION__, dsta );

	if( dsta & TSI148_DSTA_BSY )
		return 1;				/* runnig */

	if( dsta & TSI148_DSTA_ERR )
		return -EIO;			/* error */

	return 0;					/* ok */
}


/***********************************************************************/
/** Generate a VMEbus interrupt.
 *
 * \sa irqGenerate
 *
 */
static int Tsi148_IrqGenerate(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int level,
	int vector
){
	unsigned long ps;
	uint32_t vicr = 0;
	int rv = -EINVAL;

	if( level & ~(TSI148_VICR_IRQL_MASK>>TSI148_VICR_IRQL_BIT)
		|| vector & ~TSI148_VICR_STID_MASK ){
		
		printk( KERN_ERR "*** vme4l(%s): invalid interrupt level/vector "
				"(%d/%d)\n", __FUNCTION__, level, vector);
		return( -EINVAL );
	}

	TSI148_LOCK_STATE_IRQ( ps );
	
	/* interrupt busy check */
	vicr = TSI148_CTRL_READ( lcsr.vicr );
	if( vicr & TSI148_VICR_IRQS ){
		printk( KERN_ERR "*** vme4l(%s): processing another IRQ (irq busy"
				" state)\n", __FUNCTION__);
		rv = -EBUSY;
	}
	else {
		/* write irq vector and level */
		vicr &= ~TSI148_VICR_STID_MASK; /* clear current vector */
		vicr |= (level<<TSI148_VICR_IRQL_BIT) | vector;
		TSI148_CTRL_WRITE( lcsr.vicr, vicr ); /* generate interrupt */

		rv = TSI148_INTERRUPTER_ID;
	}

	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_IrqGenerate */


/***********************************************************************/
/** Check if generated interrupt has been acknowledged.
 *
 * \sa irqGenAcked
 *
 */
static int Tsi148_IrqGenAcked(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int id
){
	int rv = -EINVAL;
	unsigned long ps;

	/* we have only one interrupter */
	if( id != TSI148_INTERRUPTER_ID ) {
		printk( KERN_ERR "*** vme4l(%s): unknown interrupter ID %d\n",
				__FUNCTION__, id );
		return -EINVAL;
	}

	TSI148_LOCK_STATE_IRQ( ps );

	rv = (TSI148_CTRL_READ(lcsr.vicr) & TSI148_VICR_IRQS) ? 0 : 1;

	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_IrqGenAcked */


/***********************************************************************/
/** Clear pending interrupter.
 *
 * \sa irqGenClear
 *
 */
static int Tsi148_IrqGenClear(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int id
){
	unsigned long ps;

	/* we have only one interrupter */
	if( id != TSI148_INTERRUPTER_ID ) {
		printk( KERN_ERR "*** vme4l(%s): unknown interrupter ID %d\n",
				__FUNCTION__, id );
		return -EINVAL;
	}

	TSI148_LOCK_STATE_IRQ( ps );

	TSI148_CTRL_SETMASK( lcsr.vicr, TSI148_VICR_IRQC );

	TSI148_UNLOCK_STATE_IRQ( ps );

	return TSI148_OK;
}/* Tsi148_IrqGenClear */


/***********************************************************************/
/** Check if we are system controller (Slot 1)
 *
 * NOTE: Slot detection may fail at older VME racks.
 *
 * \sa sysCtrlFuncGet
 *
 */
static int Tsi148_SysCtrlFuncGet( VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	return( !!(TSI148_CTRL_READ(lcsr.vstat) & TSI148_VSTAT_SCONS) );
}/* Tsi148_SysCtrlFuncGet */


/***********************************************************************/
/** Get VMEbus requester mode
 *
 * \sa requesterModeGet
 *
 */
static int Tsi148_RequesterModeGet( VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	uint32_t vrel;
	int rv = 0;
	unsigned long ps;

	TSI148_LOCK_STATE_IRQ( ps );

	vrel = TSI148_CTRL_READ( lcsr.vmctrl ) & TSI148_VMCTRL_VREL_MASK;

	switch( vrel ) {
		/* release on request */
		case TSI148_VMCTRL_VREL_ROR: rv = 0; break;
		/* release when done */
		case TSI148_VMCTRL_VREL_RWD: rv = 1; break;
		/* unknown */
		default:
			rv = 256;
			VME4LDBG( "vme4l(%s): user defined requester mode (vrel=0x%x)\n",
					  __FUNCTION__, vrel );
	}

	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
} /* Tsi148_RequesterModeGet */


/***********************************************************************/
/** Set VMEbus requester mode
 *
 * \sa requesterModeSet
 *
 */
static int Tsi148_RequesterModeSet(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int state )
{
	uint32_t vmctrl;
	int rv = -EINVAL;
	unsigned long ps;

	TSI148_LOCK_STATE_IRQ( ps );

	vmctrl = TSI148_CTRL_READ( lcsr.vmctrl );
	vmctrl &= ~TSI148_VMCTRL_VREL_MASK;
	switch( state ) {
		/* release on request */
		case 0: vmctrl |= TSI148_VMCTRL_VREL_ROR; break;
		/* release when done */
		case 1: vmctrl |= TSI148_VMCTRL_VREL_RWD; break;
		/* unknown */
		default:
			printk( KERN_ERR "*** vme4l(%s): Requester state %d not "
					"supported\n", __FUNCTION__, state );
			goto CLEANUP;
	}

	TSI148_CTRL_WRITE( lcsr.vmctrl, vmctrl );

	rv = TSI148_OK;
CLEANUP:
	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_RequesterModeSet */


/***********************************************************************/
/** Get VMEbus master write mode
 *
 * \sa postedWriteModeGet
 *
 */			
static int Tsi148_PostedWriteModeGet( VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	/* always posted */
	return 1;
}


/***********************************************************************/
/** Get information about last VME bus error
 *
 * \sa busErrGet
 *
 */		
static int Tsi148_BusErrGet(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int *attrP,
	vmeaddr_t *addrP,
	int clear )
{
	int rv = 0;
	unsigned long ps;

	TSI148_LOCK_STATE_IRQ( ps );

	if( vme4l_bh->berrAttr ) {
		rv = 1;
		*addrP = vme4l_bh->berrAddr;
	}

	if( clear ) {
		vme4l_bh->berrAttr = 0;
	}

	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_BusErrGet */


/***********************************************************************/
/** Generate VME system reset
 *
 * Note: Maybe it's better to do a reboot to generate a VME reset.
 *
 * \sa sysReset
 *
 */
static int Tsi148_SysReset(	VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	if( TSI148_CTRL_READ(lcsr.vstat) & TSI148_VSTAT_SCONS ) {
		
		TSI148_CTRL_SETMASK( lcsr.vctrl, TSI148_VCTRL_SRESET );
	
		mdelay(250);	/* > 200 ms */
	
		TSI148_CTRL_CLRMASK( lcsr.vctrl, TSI148_VCTRL_SRESET );
	}
	else {
		printk( KERN_ERR "*** vme4l(%s): only Slot 1 can do this\n",
				__FUNCTION__);
		return -EPERM;
	}

	return TSI148_OK;
}/* Tsi148_SysReset */


/***********************************************************************/
/** Get system IRQ no.
 *
 * \sa retrieveSysIrq
 *
 */
static unsigned int Tsi148_RetrieveSysIrq( VME4L_BRIDGE_HANDLE *vme4l_bh )
{
	unsigned int irq;

	irq = vme4l_bh->pdev->irq;

	if (!irq) {
		printk( KERN_ERR "*** vme4l(%s): invalid system IRQ\n", __FUNCTION__);
	}
	
	return irq;
}/* Tsi148_RetrieveSysIrq */


/***********************************************************************/
/** Get bridge driver info string
 */
static void Tsi148_RevisionInfo(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	char *buf )
{

	sprintf( buf, "TSI148 VME bridge (rev %d), vme4l-tsi148 $Revision: 1.7 $",
			 vme4l_bh->pciRevision );
}/* Tsi148_RevisionInfo */


/***********************************************************************/
/** Turn on/off VME irq level or special level (unconditionally)
 */
static int Tsi148_IrqLevelCtrl(
	VME4L_BRIDGE_HANDLE *vme4l_bh,
	int level,
	int set )
{
	unsigned long ps;
	uint32_t mask;
	int rv = -EINVAL;

	TSI148_LOCK_STATE_IRQ( ps );

	VME4LDBG( "vme4l(%s): %sable level %d IRQ\n", __FUNCTION__,
			  set ? "en":"dis", level );

	if( Tsi148_IrqLevelToMask( level, &mask ) != TSI148_OK ) {
		printk( KERN_ERR "*** vme4l(%s): unsupported IRQ level %d",
				__FUNCTION__, level );
	}
	else {
		if( set ) {
			TSI148_CTRL_SETMASK( lcsr.inteo, mask );
			TSI148_CTRL_SETMASK( lcsr.inten, mask );
		}
		else {
			TSI148_CTRL_CLRMASK( lcsr.inteo, mask );
			TSI148_CTRL_CLRMASK( lcsr.inten, mask );
		}
		rv = TSI148_OK;
	}

	TSI148_UNLOCK_STATE_IRQ( ps );

	return rv;
}/* Tsi148_IrqLevelCtrl */

/*----------------------+
 | LINUX specific stuff |
 +----------------------*/

static VME4L_BRIDGE_DRV G_tsi148Drv = {
	.revisionInfo		= Tsi148_RevisionInfo,
	.requestAddrWindow 	= Tsi148_RequestAddrWindow,
	.releaseAddrWindow 	= Tsi148_ReleaseAddrWindow,
	.irqLevelCtrl		= Tsi148_IrqLevelCtrl,
	.readPio8			= Tsi148_ReadPio8,
	.readPio16			= Tsi148_ReadPio16,
	.readPio32			= Tsi148_ReadPio32,
	.writePio8			= Tsi148_WritePio8,
	.writePio16			= Tsi148_WritePio16,
	.writePio32			= Tsi148_WritePio32,
	.dmaSetup			= Tsi148_DmaSetup,
	.dmaBounceSetup		= NULL,	/* not needed by TSI148 */
	.dmaStart			= Tsi148_DmaStart,
	.dmaStop			= Tsi148_DmaStop,
	.dmaStatus			= Tsi148_DmaStatus,
	.irqGenerate		= Tsi148_IrqGenerate,
	.irqGenAcked		= Tsi148_IrqGenAcked,
	.irqGenClear		= Tsi148_IrqGenClear,
	.sysCtrlFuncGet		= Tsi148_SysCtrlFuncGet,
	.sysCtrlFuncSet		= NULL, /* not supported by TSI148 */
	.retrieveSysIrq 	= Tsi148_RetrieveSysIrq,
	.sysReset			= Tsi148_SysReset,
	.arbToutGet			= NULL, /* not supported by TSI148 */
	.busErrGet			= Tsi148_BusErrGet,
	.requesterModeGet	= Tsi148_RequesterModeGet,
	.requesterModeSet	= Tsi148_RequesterModeSet,
	.postedWriteModeGet	= Tsi148_PostedWriteModeGet,
	.postedWriteModeSet	= NULL, /* not supported by TSI148 */
	.rmwCycle			= Tsi148_RmwCycle,
	.aOnlyCycle			= NULL, /* not supported by TSI148 */
	.slaveWindowCtrl	= Tsi148_SlaveWindowCtrl,
	.mboxRead           = Tsi148_MboxRead,
	.mboxWrite          = Tsi148_MboxWrite,
	.locMonRegRead      = Tsi148_LocMonRegRead,
	.locMonRegWrite     = Tsi148_LocMonRegWrite,
	.reserved           = { 0, 0, 0, 0, 0 }
};/* G_tsi148Drv */


/*******************************************************************/
/** Probe/initialize on VME bridge.
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
 *  \param pdev		pci_dev structure representing the device
 *  \param ent		entry in PCI table that matches
 *
 *  \return 	0 when the driver has accepted the device or
 *				an error code (negative number) otherwise.
 */
static int DEVINIT tsi148_pci_init_one(
	struct pci_dev *pdev,
	const struct pci_device_id *ent )
{
	VME4L_BRIDGE_HANDLE *vme4l_bh = &G_vme4l_bh;
	unsigned long vmeMemSize = 0;
	int rv = 0;
	int irqReq = 0;
	
	if( !pdev || !ent || !vme4l_bh ) {
		printk( KERN_ERR "*** vme4l(%s): nullpointer assignment (pdev=%p, "
				"ent=%p, h=%p)\n", __FUNCTION__, pdev, ent, vme4l_bh);
		goto CLEANUP;
	}

	VME4LDBG( "vme4l(%s): vme4l_bh=0x%p\n", __FUNCTION__, vme4l_bh);

	/* clear handle */
	memset( vme4l_bh, 0, sizeof(*vme4l_bh));	
	vme4l_bh->pdev = pdev;

	pci_read_config_byte( pdev, PCI_REVISION_ID, &vme4l_bh->pciRevision );

	VME4LDBG( "vme4l(%s): found TSI148 bridge (rev=%d irq=%d)\n",
			  __FUNCTION__, vme4l_bh->pciRevision, pdev->irq );

	if( (vmeMemSize = Tsi148_VmeMemoryAvailable(vme4l_bh)) == 0 ){
		printk( KERN_ERR "*** vme4l(%s): no PCI memory for VME available, "
				"check BIOS settings\n", __FUNCTION__ );
		goto CLEANUP;
	}
	else {
		VME4LDBG( "vme4l(%s): available VME mem size = 0x%llx (%d MB)\n",
				  __FUNCTION__, (uint64_t)vmeMemSize,
				  (unsigned int)(vmeMemSize/1024/1024) );
	}

	/* sets the correct irq also in ACPI mode */
	if ( (rv = pci_enable_device(pdev)) != 0 ) {
		printk( KERN_ERR "*** vme4l(%s): error pci_enable_device\n",
				__FUNCTION__ );
		goto CLEANUP;
	}

	/* TSI148 only uses BAR0 */
	vme4l_bh->bar = vme4l_bh->regs.phys =
		(uint32_t) pci_resource_start(pdev, 0);
	vme4l_bh->regs.size = TSI148_CTRL_SPACE;

	Tsi148_AllocRegSpace( (VME4L_RESRC *)&vme4l_bh->regs, "vme-tsi148-regs" );

	VME4LDBG( "vme4l(%s): irq remapped to IRQ %d\n", __FUNCTION__, pdev->irq );

	/* set bridge register to initial state */
	Tsi148_InitBridge();

	spin_lock_init(&vme4l_bh->lockState);

	/* Tsi148_IrqHandler is the standard linux IRQ handler */
	if( (rv = request_irq( pdev->irq,
						   Tsi148_IrqHandler,
						   IRQF_SHARED,
						   "tsi148",
						   vme4l_bh)) < 0 ) {
		VME4LDBG( "vme4l(%s): error request_irq !\n", __FUNCTION__);
		goto CLEANUP;
	}

	irqReq++;

	/* register to VME4L core */
	if( (rv=vme4l_register_bridge_driver( &G_tsi148Drv, vme4l_bh )) != 0 ) {
		VME4LDBG( "vme4l(%s): error registering tsi148 bridge driver\n",
				  __FUNCTION__);
			goto CLEANUP;
	}

	VME4LDBG( "vme4l(%s): system controller function is %sabled\n",
			  __FUNCTION__, Tsi148_SysCtrlFuncGet( vme4l_bh ) ? "en" : "dis" );

	return rv;

CLEANUP:
	printk( KERN_ERR "*** vme4l(%s): fatal driver initialization error\n",
		__FUNCTION__);

	if( irqReq ){
		free_irq( pdev->irq, vme4l_bh );
	}
	
	Tsi148_FreeRegSpace(&vme4l_bh->regs);
	
	return TSI148_ERROR;
}/* tsi148_pci_init_one */


static void DEVEXIT tsi148_pci_remove_one( struct pci_dev *pdev )
{
	int i;
	VME4L_BRIDGE_HANDLE *vme4l_bh = &G_vme4l_bh;
	VME4L_RESRC *winResP;
	
	vme4l_unregister_bridge_driver();

	/* free resources: */

	/* inbound windows */
	for( i=0; i<TSI148_INBOUND_NO; i++ ) {
		winResP = &vme4l_bh->vmeIn[i];
		
		Tsi148_InboundWinSet( i, VME4L_SPC_SLV0+i, 0, 0, 0 /*disable*/ );
		if( winResP->memReq ) {
			pci_free_consistent( vme4l_bh->pdev, winResP->size, winResP->vaddr,
								 winResP->phys );
		}
	}

	/* outbound windows */
	for( i=0; i<TSI148_OUTBOUND_NO; i++ ) {
		Tsi148_ReleaseAddrWindow( vme4l_bh, 0, 0, 0, 0, (void *) i );
	}

	/* registers */
	Tsi148_InitBridge();	/* set TSI148 register to defaults */

	/* irq */
	free_irq( pdev->irq, vme4l_bh );

	/* pages for DMA desc buffers */
	if( vme4l_bh->dmaDescBuf ) {
		free_pages( (unsigned long)vme4l_bh->dmaDescBuf, TSI148_DMA_DESC_PAGES );
	}

	/* free allocated memory */
	Tsi148_FreeRegSpace(&vme4l_bh->regs);

	VME4LDBG( "vme4l_tsi148(%s): TSI148 PCI device is removed\n",
			  __FUNCTION__);
}/* tsi148_pci_remove_one */


/*******************************************************************/
/** PCI Vendor/Device ID table.
 *
 * Driver will handle all devices that have these codes.
 */
static struct pci_device_id G_pci_tbl[] DEVINITDATA  = {
	/* TSI148 */
	{
		TSI148_VEN_ID,
		TSI148_DEV_ID,
		TSI148_SUBSYS_VEN_ID,
		TSI148_SUBSYS_ID,
	},
	{
		0,
	}
};/* G_pci_tbl */

MODULE_DEVICE_TABLE( pci, G_pci_tbl );


/*******************************************************************/
/** Linux pci driver struct.
 */
static struct pci_driver G_pci_driver = {
	name:		"vme4l-tsi148",
	id_table:	G_pci_tbl,
	probe:		tsi148_pci_init_one,
	remove:		DEVEXIT_P(tsi148_pci_remove_one),
};/* G_pci_driver */


/*******************************************************************/
/** Kernel module initialization function.
 */
static int __init tsi148_init_module( void )
{
	int rv;

	printk("tsi148_init_module...\n" );

	if( (rv =  pci_register_driver( &G_pci_driver )) < 0 ){
		printk( KERN_ERR "*** vme4l(%s): PCI device initialization failed\n",
				__FUNCTION__ );
		return rv;
	}

	return 0;
}/* tsi148_init_module */


/*******************************************************************/
/** Kernel module clean-up function.
 */
static void __exit tsi148_cleanup_module( void )
{
	VME4LDBG( "vme4l_tsi148(%s): deinit bridge and unregister driver\n", __FUNCTION__);
	Tsi148_InitBridge();
	pci_unregister_driver(&G_pci_driver);
}/* tsi148_cleanup_module */

int vme4l_register_client( VME4L_BRIDGE_HANDLE *h )
{
	TSI148_LOCK_STATE();
	++h->refCounter;
	TSI148_UNLOCK_STATE();

	return 0;
}
EXPORT_SYMBOL_GPL(vme4l_register_client);

int vme4l_unregister_client( VME4L_BRIDGE_HANDLE *h )
{
	TSI148_LOCK_STATE();
	if (h->refCounter <= 0)
		return -EINVAL;

	--h->refCounter;
	TSI148_UNLOCK_STATE();

	return 0;
}
EXPORT_SYMBOL_GPL(vme4l_unregister_client);

module_init(tsi148_init_module);
module_exit(tsi148_cleanup_module);

MODULE_AUTHOR("Ralf T./Thomas S.");
MODULE_DESCRIPTION("VME4L - MEN VME TSI148 bridge driver");
MODULE_LICENSE("GPL");

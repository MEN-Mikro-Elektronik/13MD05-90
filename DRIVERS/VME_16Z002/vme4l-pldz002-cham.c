/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l-menpldz002.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/07/09 10:52:05 $
 *    $Revision: 1.12 $
 *
 *  	 \brief  MEN VME bridge driver for PLDZ002 (A13/A15...)
 *				 Also supports the A12/B11 VME bridge (which is a subset
 *				 of PLDZ002)
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; version
 *	2 of the License.
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l-pldz002.c,v $
 * Revision 1.12  2009/07/09 10:52:05  rt
 * R: 1.) Large zero-copy DMAs  (>64k) read/write wrong data.
 *    2.) Not compilable with kernel > 2.6.21.
 * M: 1.) Changed vmeAddr parameter to pointer to vmeAddr.
 *    2.) Add switch for pci_module_init.
 *
 * Revision 1.11  2009/06/03 18:26:17  rt
 * R: 1.) TSI148 needs physical Address to perform RMW cycle
 *    2.) Not compilable with kernel > 2.6.19
 * M: 1.) physAddr parameter added to rmwCycle()
 *    2.) regs parameter removed from PldZ002Irq for kernel > 2.6.19
 *
 * Revision 1.10  2009/04/30 21:50:24  rt
 * R: 1) Support for newer kernels.
 *    2) Wrong debug messages.
 * M: 1a) AUTOCONF_INCLUDED switch added.
 *     b) pci_module_init renamed.
 *    2) Debug messages reformated, arguments added/removed.
 *
 * Revision 1.9  2007/12/10 12:00:28  ts
 * use new Irq handler Flags (IRQF_xxx) from 2.6.20 on
 *
 * Revision 1.8  2006/09/26 11:02:23  ts
 * adapted for either classic RTAI or Xenomai usage
 *
 * Revision 1.7  2005/01/19 14:15:54  ts
 * Name of called IRQ sharing functions changed
 *
 * Revision 1.6  2004/12/04 18:48:46  ts
 * added VME RTAI support
 *
 * Revision 1.5  2004/10/27 12:11:31  kp
 * bug fix in address validity check for slave windows (A12/B11 only)
 *
 * Revision 1.4  2004/10/26 16:09:29  kp
 * adapted to PLDZ002 Rev 18 (decimal). Support bus master DMA
 *
 * Revision 1.3  2004/07/27 07:56:17  kp
 * cosmetic
 *
 * Revision 1.2  2004/07/26 16:31:28  kp
 * intermediate alpha release
 * - support for slave windows, mailbox, location
 * - Linux 2.6 support (zerocopy DMA untested!)
 *
 * Revision 1.1  2003/12/15 15:02:11  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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

#include <MEN/men_typs.h>
#include <MEN/men_chameleon.h>
#include <MEN/pldz002-cham.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define PFX "vme4l_pldz002_cham: "
#define KERN_ERR_PFX KERN_ERR "!!!" PFX

/** DMA bounce buffer uses last 256K of bridge SRAM */
#define BOUNCE_SRAM_A21ADDR	 0x0   /* 0xff000 */    /* ts: was c0000 */
#define BOUNCE_DMABD_BASE	 0xff000

#define PLDZ002_BOUNCE_CHUNKSIZE_LIMIT_A32D32	0x3fffc /* max. address in a 256k chunk for D32 spaces */
#define PLDZ002_BOUNCE_CHUNKSIZE_LIMIT_A32D64	0x3fff8 /* max. address in a 256k chunk for D64 spaces */

#define LOCAL_SRAM_A21ADDR	0x0   /* ts: was c0000 */

#define BOUNCE_SRAM_A21SIZE	0x100000    /* 1MB SRAM  */
#define BOUNCE_SRAM_A15SIZE	0x40000
#define PLDZ002_MAX_UNITS	8
#define BOUNCE_SRAM_SIZE 	BOUNCE_SRAM_A21SIZE

#define PLDZ002_VAR_VMEA32      8  /* Variant of that PLDZ002 core that represents A32 space.
									  on new Z002 core this unit's BAR can have different size
									  depending on VHDL generic */
#define PLDZ002_A32D32_SIZE_512M	0x20000000
#define PLDZ002_A32D32_SIZE_256M	0x10000000
#define PLDZ002_A32D32_SIZE_128M	 0x8000000
#define PLDZ002_A32D32_SIZE_64M		 0x4000000
#define PLDZ002_A32D32_SIZE_32M		 0x2000000
#define PLDZ002_A32D32_SIZE_16M		 0x1000000

/* convert a chameleon BAR entry to its location in PCI config space (0x10, 0x14..) */
#define CHAM_BAR2PCI_BASE_ADDR(x) (((x)*4)+0x10)

/* Macros to lock accesses to bridge driver handle and VME bridge regs */
#define PLDZ002_LOCK_STATE() 			spin_lock(&h->lockState)
#define PLDZ002_UNLOCK_STATE() 			spin_unlock(&h->lockState)
#define PLDZ002_LOCK_STATE_IRQ(ps) 		spin_lock_irqsave(&h->lockState, ps)
#define PLDZ002_UNLOCK_STATE_IRQ(ps) 	spin_unlock_irqrestore(&h->lockState, ps)

/* macros to write to PLD internal regs */
#define VME_CTRL_SETMASK(reg,mask)  \
 VME_REG_WRITE8((reg),VME_REG_READ8(reg)|(mask))
#define VME_CTRL_CLRMASK(reg,mask)	\
 VME_REG_WRITE8((reg),VME_REG_READ8(reg)&~(mask))


#define VME_REG_READ8(reg)			(readb((char *)h->regs.vaddr + (reg)))
#define VME_REG_WRITE8(reg, val)	\
 (writeb(val, (char *)h->regs.vaddr + (reg)))
#define VME_REG_SETMASK8(reg,mask)  \
 VME_REG_WRITE8((reg),VME_REG_READ8(reg)|(mask))
#define VME_REG_CLRMASK8(reg,mask)	\
 VME_REG_WRITE8((reg),VME_REG_READ8(reg)&~(mask))

#define VME_REG_READ16(reg)			(readw((char *)h->regs.vaddr + (reg)))
#define VME_REG_WRITE16(reg, val)	\
 (writew(val, (char *)h->regs.vaddr + (reg)))

/* ts: for A21 we need different Macros to address DMA BDs */
#define VME_REG_DMABD_RD32(reg)	\
	(readl((char *)(h->bounce.vaddr+BOUNCE_DMABD_BASE+(reg))))

#define VME_REG_DMABD_WR32(reg, val) \
	(writel((val), (char*)(h->bounce.vaddr+BOUNCE_DMABD_BASE+(reg))))

#define VME_REG_READ32(reg)			(readl((char *)h->regs.vaddr + (reg)))
#define VME_REG_WRITE32(reg, val)	\
 (writel(val, (char *)h->regs.vaddr + (reg)))

/* r/w macros using virtual address instead of offsets (e.g. for SRAM regs) */
#define VME_GENREG_READ32(vaddr)		(readl((char *)vaddr))
#define VME_GENREG_WRITE32(vaddr, val)	(writel(val, (char *)vaddr))

/* macros to read/write from VME address spaces */
#if defined(__powerpc__)

/* on powerpc, the normal readx() macros do implicit swapping! */
# define VME_WIN_READ8(vaddr)	 	__raw_readb(vaddr)
# define VME_WIN_READ16(vaddr)	 	__raw_readw(vaddr)
# define VME_WIN_READ32(vaddr)	 	__raw_readl(vaddr)
# define VME_WIN_WRITE8(vaddr,val) 	__raw_writeb(val,vaddr)
# define VME_WIN_WRITE16(vaddr,val) __raw_writew(val,vaddr)
# define VME_WIN_WRITE32(vaddr,val) __raw_writel(val,vaddr)
#else
# define VME_WIN_READ8(vaddr)	 	readb(vaddr)
# define VME_WIN_READ16(vaddr)	 	readw(vaddr)
# define VME_WIN_READ32(vaddr)	 	readl(vaddr)
# define VME_WIN_WRITE8(vaddr,val) 	writeb(val,vaddr)
# define VME_WIN_WRITE16(vaddr,val) writew(val,vaddr)
# define VME_WIN_WRITE32(vaddr,val) writel(val,vaddr)
#endif /* __powerpc__ */

/** VME4L_RESRC.cache flags */
#define _PLDZ002_WRITETHROUGH 	0x1


#define _PLDZ002_FS3(h) 			(0)
#define _PLDZ002_USE_BOUNCE_DMA(h) 	(bounce_buffer)
#define MEN_PLDZ002_DMABD_OFFS 		((char *)h->sramRegs.vaddr + 0x100)

/* The A15 cannot perform direct VMA<->RAM DMA */
#if defined (MEN_A15)
# define _PLDZ002_FS3(h) 				(h->chu->pdev->revision >= 99)
# define _PLDZ002_USE_BOUNCE_DMA(h) 		(0)
#endif

/** interrupter dummy ID */
#define _PLDZ002_INTERRUPTER_ID	1

#define _PLDZ002_SWAP32(dword)	( ((dword)>>24) | ((dword)<<24) | \
							  (((dword)>>8) & 0x0000ff00) | \
							  (((dword)<<8) & 0x00ff0000)     )
/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/** structure to maintain variable used to ioremap bridge regs etc. */
typedef struct {
	unsigned long phys;				/**< phys. address  */
	unsigned long size;				/**< size of region  */
	void *vaddr;				/**< mapped virt. address  */
	int memReq;					/**< flag memory has been requested  */
	int cache;					/**< region cacheing flags (0=no cache)  */
} VME4L_RESRC;

/* the value of the enum denotes the variant tag
 * of the entry in the chameleon table */
typedef enum {
	CHAM_VME = 1,
	CHAM_SPC_A16_D16 = 3,
	CHAM_SPC_A16_D32,
	CHAM_SPC_SRAM,
	CHAM_SPC_A24_D16,
	CHAM_SPC_A24_D32,
	CHAM_SPC_A32_D32,
	CHAM_SPC_CR_CSR,	/* variant 9 = CR/CSR */
	CHAM_SPC_END,
} VME_SPACE_CHAM;

/** bridge drivers private data */
typedef struct {
	CHAMELEONV2_UNIT_T * chu;		/**< chameleon unit for vme control registers  */
	unsigned long spaces[CHAM_SPC_END]; /**< chameleon units of the separate spaces*/
	VME4L_RESRC regs;			/**< bridge regs [+ PLD internal RAM if any] */
	VME4L_RESRC	sramRegs;		/**< PLDZ002>=Rev17 registers in SRAM  */
	VME4L_RESRC iack;			/**< IACK space */
	VME4L_RESRC bounce;			/**< part of SRAM for DMA bouncing  */

	/* the following two are not ioremapped */
	VME4L_RESRC	sram;			/**< SRAM as slave window  */
	VME4L_RESRC	bmShmem;		/**< bus master slave window  */
	int  a32LongAddUsed;		/**< A32 LONGADD reg in use count  */
	uint8_t mstrShadow;			/**< MSTR register shadow reg */
    uint8_t reqLevel;           /**< VME requester Level */
    uint32_t bLongaddAdjustable;/**< 1: new PLDZ002 Var.2 with adjustable LONGADD register
								  	 0: default LONGADD register with 3bit (8x512MB) */
    uint32_t A32BARsize;        /**< VME requester Level */
    uint8_t longaddWidth;       /**< bit width of the LONGADD: 512MB=3, 256MB=4, 128MB=5... 16MB=8  */
    uint8_t mstrAMod;			/**< Address modifier shadow reg (A21) */
	uint16_t addrModShadow[255];/**< address modifiers shadow reg  */
	uint8_t haveBerr;			/**< bus error recorded */
	uint32_t berrAddr;			/**< bus error causing address */
	uint32_t berrAcc;			/**< bus error causing properties */
	uint32_t hasExtBerrInfo;
	spinlock_t lockState;		/**< spin lock for VME bridge registers and handle state */
	int refCounter;		/**< number of registered clients */
} VME4L_BRIDGE_HANDLE;

#define COMPILE_VME_BRIDGE_DRIVER
#include "vme4l-core.h"

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static VME4L_BRIDGE_HANDLE G_bHandle;

static const u16 G_devIdArr[] = { 2,
								  CHAMELEONV2_DEVID_END };

static int vme4l_probe( CHAMELEONV2_UNIT_T *chu );
static int vme4l_remove( CHAMELEONV2_UNIT_T *chu );

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

#define BOUNCE_BUFFER_DEFAULT 0 /* Use zero-copy DMA */
static int bounce_buffer = BOUNCE_BUFFER_DEFAULT;  /**< enable bounce buffer */

module_param(bounce_buffer, int, S_IRUGO);
MODULE_PARM_DESC(debug, "Use bounce buffer DMA instead of zero-copy (default " \
			M_INT_TO_STR(BOUNCE_BUFFER_DEFAULT) ")");


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/


/***********************************************************************/
/** Get system IRQ no.
 */
unsigned int GetSysIrq( VME4L_BRIDGE_HANDLE *h )
{
	unsigned int irq;

	irq = h->chu->pdev->irq;

	if (!irq)
		printk(KERN_ERR_PFX "%s: error invalid system IRQ!\n",
		       __func__);
	return irq;

}

/***********************************************************************/
/** Get bridge driver info string
 */
void RevisionInfo( VME4L_BRIDGE_HANDLE *h, char *buf )
{
	sprintf(buf,
			"VME bridge (rev %d ), vme4l-pldz002 $Revision: 1.12 $",
			h->chu->unitFpga.revision );
}

/***********************************************************************/
/** Request VME master address window
 */
static int RequestAddrWindow(
	VME4L_BRIDGE_HANDLE *h,
	VME4L_SPACE spc,
	vmeaddr_t *vmeAddrP,
	size_t *sizeP,
	void **physAddrP,
	int flags,
	void **bDrvDataP)
{
	void *physAddr = NULL;
	size_t size=0;
	vmeaddr_t vmeAddr=0;
	int rv = 0;

	*bDrvDataP = NULL;			/* don't need it for now */
	PLDZ002_LOCK_STATE();

	switch( spc ){
	case VME4L_SPC_A16_D16:
		physAddr = (void *)h->spaces[CHAM_SPC_A16_D16];
		vmeAddr  = 0;
		size 	 = PLDZ002_A16Dxx_SIZE;
		break;

	case VME4L_SPC_A16_D32:
		physAddr = (void *)h->spaces[CHAM_SPC_A16_D32];
		vmeAddr  = 0;
		size     = PLDZ002_A16Dxx_SIZE;
		break;

	case VME4L_SPC_A24_D16:
		physAddr = (void *)h->spaces[CHAM_SPC_A24_D16];
		vmeAddr  = 0;
		size	 = PLDZ002_A24Dxx_SIZE;
		break;

	case VME4L_SPC_A24_D32:
		physAddr = (void *)h->spaces[CHAM_SPC_A24_D32];
		vmeAddr  = 0;
		size     = PLDZ002_A24Dxx_SIZE;
		break;

	case VME4L_SPC_CR_CSR:
		physAddr = (void *)h->spaces[CHAM_SPC_CR_CSR];
		vmeAddr  = 0;
		size     = PLDZ002_CR_CSR_SIZE;
		break;

	case VME4L_SPC_A32_D32:
		if( h->a32LongAddUsed ){	/* already a A32 window setup */
			rv = -EBUSY;
			break;
		}

		/* check for <A32 BAR size> crossing in requested window */
		vmeAddr = *vmeAddrP & ( h->A32BARsize - 1 );
		if( vmeAddr + *sizeP > h->A32BARsize )
		  break;

		physAddr = (void *)h->spaces[CHAM_SPC_A32_D32];
		vmeAddr  = *vmeAddrP & ~( h->A32BARsize - 1 );
		size     = h->A32BARsize;
		h->a32LongAddUsed++;

		/*  the new LONGADD register consists of the adress highbyte. According to IC designer
		 *  lower address bits are unused for a certain size, so we just write the highbyte */
		if ( h->bLongaddAdjustable ) {
		  VME4LDBG("set LONGADD = 0x%02x\n", (vmeAddr >> 24) & 0xff );
		  VME_REG_WRITE8( PLDZ002_LONGADD, (vmeAddr >> 24) & 0xff );
		} else {
		  VME4LDBG("set LONGADD 0x%02x\n", (vmeAddr >> 29 ) & 0xff );
		  VME_REG_WRITE8( PLDZ002_LONGADD, vmeAddr >> 29 );
		}
		break;

	default:
		break;
	}
	if( physAddr == 0 ){
		rv = -EINVAL;
	}
	else {
		*physAddrP = physAddr;
		*vmeAddrP  = vmeAddr;
		*sizeP	   = size;
	}

	PLDZ002_UNLOCK_STATE();

	return rv;
}

/*******************************************************************/
/** centralized storing and clearing of buserror events.
 *
 * \param h  VME bridge driver handle
 *
 */
static void StoreAndClearBuserror( VME4L_BRIDGE_HANDLE *h )
{
	/* store error causing info */
	h->haveBerr = 1;
	h->berrAddr = VME_REG_READ32(PLDZ002_BERR_ADDR);
	h->berrAcc = VME_REG_READ32(PLDZ002_BERR_ACC);

	/* report it, extended or default depending on core version */
	if ( h->hasExtBerrInfo ) {
		printk(KERN_ERR_PFX "PldZ002Irq bus error. Cause: %s VME addr 0x%lx, AM=0x%02x (in %s state)\n",
		       (h->berrAcc & PLDZ002_BERR_RW_DIR) ? "read from" : "write to",
		       h->berrAddr,
		       (h->berrAcc & PLDZ002_BERR_ACC_AM_MASK),
		       (h->berrAcc & PLDZ002_BERR_IACK) ? "IACK" : "normal" );
	} else {
		printk(KERN_ERR_PFX "PldZ002Irq bus error\n");
	}

	/* and clear */
	VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow | PLDZ002_MSTR_BERR );
}

/***********************************************************************/
/** Release VME master address window
 */
static int ReleaseAddrWindow(
	VME4L_BRIDGE_HANDLE *h,
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	int flags,
	void *bDrvData)
{
	PLDZ002_LOCK_STATE();

	switch( spc ){
	case VME4L_SPC_A32_D32:
		if( h->a32LongAddUsed > 0 )
			h->a32LongAddUsed--;
		break;
	default:
		break;
	}
	PLDZ002_UNLOCK_STATE();

	return 0;
}

/***********************************************************************/
/** Turn on/off VME irq level or special level (unconditionally)
 */
int IrqLevelCtrl(
	VME4L_BRIDGE_HANDLE *h,
	int level,
	int set )
{
	unsigned long ps;
	int rv = -EINVAL;

	PLDZ002_LOCK_STATE_IRQ(ps);

	/* interrupts in IMASK reg */
	if( (level >= VME4L_IRQLEV_1 && level <= VME4L_IRQLEV_7) ||
		(level == VME4L_IRQLEV_ACFAIL)){

		if( level == VME4L_IRQLEV_ACFAIL ) level = 0;

		if( set ){
			VME_REG_SETMASK8( PLDZ002_IMASK, 1<<level );
		}
		else {
			VME_REG_CLRMASK8( PLDZ002_IMASK, 1<<level );
		}
		rv = 0;
	}

	else {
		/* Mailbox */
		if( level >= VME4L_IRQLEV_MBOXRD(0) &&
				level <= VME4L_IRQLEV_MBOXWR(3)){

			level -= VME4L_IRQLEV_MBOXRD(0);
			if( set ){
				VME_REG_SETMASK8( PLDZ002_MAIL_IRQ_CTRL, 1<<level );
			}
			else {
				VME_REG_CLRMASK8( PLDZ002_MAIL_IRQ_CTRL, 1<<level );
			}
			rv = 0;
		}
		/* location monitor */
		else if( level == VME4L_IRQLEV_LOCMON(0)){
			VME_REG_WRITE8( PLDZ002_LM_STAT_CTRL_0,
					(VME_REG_READ8( PLDZ002_LM_STAT_CTRL_0 ) &
					 ~PLDZ002_LM_STAT_CTRL_IRQ_EN) |
					(set ? PLDZ002_LM_STAT_CTRL_IRQ_EN : 0 ));
			rv = 0;
		}
		else if( level == VME4L_IRQLEV_LOCMON(1)){
			VME_REG_WRITE8( PLDZ002_LM_STAT_CTRL_1,
					(VME_REG_READ8( PLDZ002_LM_STAT_CTRL_1 ) &
					 ~PLDZ002_LM_STAT_CTRL_IRQ_EN) |
					(set ? PLDZ002_LM_STAT_CTRL_IRQ_EN : 0 ));
			rv = 0;
		}
	}

	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return rv;
}


/***********************************************************************/
/** Read/Write PIO functions.
 *
 * Bus error proof, but slow.
 * For writes, posted write mode is temporarily disabled.
 */

#define READ_PIO_XX(size,type) \
static int ReadPio##size ( \
	VME4L_BRIDGE_HANDLE *h,\
	void *vaddr,\
	type *dataP,\
	int flags,\
	void *bDrvData)\
{\
	unsigned long ps;\
	int rv = 0;\
               \
	PLDZ002_LOCK_STATE_IRQ(ps);\
    \
	VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow | PLDZ002_MSTR_BERR );\
	*dataP = VME_WIN_READ##size(vaddr);\
	\
	if( VME_REG_READ8( PLDZ002_MSTR ) & PLDZ002_MSTR_BERR ){\
		StoreAndClearBuserror(h);\
        VME_REG_READ8( PLDZ002_MSTR ); /* dummy read to complete access */\
        rv = -EIO;\
	}\
	\
	PLDZ002_UNLOCK_STATE_IRQ(ps);\
	return rv;\
}

READ_PIO_XX( 8, uint8_t )
READ_PIO_XX( 16, uint16_t )
READ_PIO_XX( 32, uint32_t )

#define WRITE_PIO_XX(size,type) \
static int WritePio##size ( \
	VME4L_BRIDGE_HANDLE *h,\
	void *vaddr,\
	type *dataP,\
	int flags,\
	void *bDrvData)\
{\
	unsigned long ps;\
	int rv = 0;\
               \
	PLDZ002_LOCK_STATE_IRQ(ps);\
    /* clear bus error and disable posted writes */\
	VME_REG_WRITE8( PLDZ002_MSTR, (h->mstrShadow | PLDZ002_MSTR_BERR) & ~PLDZ002_MSTR_POSTWR);\
	VME_WIN_WRITE##size(vaddr,*dataP);\
	\
	if( VME_REG_READ8( PLDZ002_MSTR ) & PLDZ002_MSTR_BERR ){\
		rv = -EIO;\
		StoreAndClearBuserror(h);\
        VME_REG_READ8( PLDZ002_MSTR ); /* dummy read to complete access */\
	}\
    /* reset posted write mode */\
    if( h->mstrShadow & PLDZ002_MSTR_POSTWR	)\
        VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow );\
	\
	PLDZ002_UNLOCK_STATE_IRQ(ps);\
	return rv;\
}

WRITE_PIO_XX( 8, uint8_t )
WRITE_PIO_XX( 16, uint16_t )
WRITE_PIO_XX( 32, uint32_t )

static inline uint32_t DmaSwapMode( int swapMode )
{
#ifdef __powerpc__
	return PLDZ002_DMABD_SWAP;
#else
	return (swapMode & VME4L_HW_SWAP1) ? PLDZ002_DMABD_SWAP : 0;
#endif
}

/***********************************************************************/
/** Write DMA scatter list to DMA controller
 *
 */
static int DmaSetup(
	VME4L_BRIDGE_HANDLE *h,
	VME4L_SPACE spc,
	VME4L_SCATTER_ELEM *sgList,
	int sgNelems,
	int direction,
	int modeFlags,
	vmeaddr_t *vmeAddr,
	int flags)
{
	int alignVme=4, sg, rv=0, endBd;
	uint32_t bdAm;
	char *bdVaddr;

	/* DMA controller supports only BLT spaces */
	switch( spc ){
	case VME4L_SPC_A24_D16_BLT:
	case VME4L_SPC_A24_D32_BLT:
	case VME4L_SPC_A32_D32_BLT:
		bdAm = h->addrModShadow[spc];
		/* Ignore VME4L_RW_USE_DMA for BLT modes */
		flags &= ~VME4L_RW_USE_DMA;
		break;
	case VME4L_SPC_A24_D64_BLT:
	case VME4L_SPC_A32_D64_BLT:
		bdAm = h->addrModShadow[spc];
		alignVme = 8;
		/* Ignore VME4L_RW_USE_DMA for BLT modes */
		flags &= ~VME4L_RW_USE_DMA;
		break;
	case VME4L_SPC_A16_D16:
	case VME4L_SPC_A16_D32:
	case VME4L_SPC_A24_D16:
	case VME4L_SPC_A24_D32:
	case VME4L_SPC_A32_D32:
		bdAm = h->addrModShadow[spc];
		break;
	default:
		return -EINVAL;
	}

	bdVaddr = MEN_PLDZ002_DMABD_OFFS;
	VME4LDBG("DmaSetup: bdVaddr=%p\n", bdVaddr );

	endBd = (sgNelems < PLDZ002_DMA_MAX_BDS) ? sgNelems : PLDZ002_DMA_MAX_BDS;

	/* setup scatter list */
	VME4LDBG("setup scatter list for %s. endBd = %d:\n", direction ? "write" : "read", endBd);

	for( sg=0; sg<endBd; sg++, sgList++, bdVaddr+=PLDZ002_DMABD_SIZE ){

		/*--- check alignment/size ---*/
		if( (*vmeAddr & (alignVme-1)) || (sgList->dmaAddress & (alignVme-1)) ||
			(sgList->dmaLength > 256*1024) || (sgList->dmaLength & (alignVme-1))){
			printk(KERN_ERR_PFX "%s: DMA setup bad alignment/len "
			       "%08llx %08llx %x\n", __func__, *vmeAddr,
			       (uint64_t)sgList->dmaAddress, sgList->dmaLength );
			rv = -EINVAL;
			goto CLEANUP;
		}

		if( direction ) {
			/* write to VME */
			VME_GENREG_WRITE32( bdVaddr+0x0, *vmeAddr );
			VME_GENREG_WRITE32( bdVaddr+0x4, sgList->dmaAddress );
			VME_GENREG_WRITE32( bdVaddr+0x8, (sgList->dmaLength>>2) - 1 ); /* Block Size of DMA transfer in longwords:  (x bytes / 4) -1 */
			VME_GENREG_WRITE32( bdVaddr+0xc,
							PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_PCI ) |
							PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_VME ) |
							bdAm | ((sg == endBd-1) ? PLDZ002_DMABD_END : 0 ) |
							(( flags & VME4L_RW_USE_DMA ) ? PLDZ002_DMABD_BLK_SGL : 0) );
		}
		else {
			/* read from VME */
			VME_GENREG_WRITE32( bdVaddr+0x0, sgList->dmaAddress );
			VME_GENREG_WRITE32( bdVaddr+0x4, *vmeAddr );
			VME_GENREG_WRITE32( bdVaddr+0x8, (sgList->dmaLength>>2) - 1 );
			VME_GENREG_WRITE32( bdVaddr+0xc,
								PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_VME ) |
								PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_PCI ) |
								bdAm | ((sg == endBd-1) ? PLDZ002_DMABD_END : 0 ) |
								(( flags & VME4L_RW_USE_DMA ) ? PLDZ002_DMABD_BLK_SGL : 0) );
		}
		*vmeAddr += sgList->dmaLength;
	}

	if (debug) {
		int i;
		bdVaddr = MEN_PLDZ002_DMABD_OFFS;
		VME4LDBG("DmaBD setup for %s:\n", direction ? "write" : "read" );
		for(i=0; i < endBd; i++ ){
			VME4LDBG("BD%d@%x: %08x %08x %08x %08x\n",
					 i, bdVaddr,
					 VME_GENREG_READ32( bdVaddr+0x0 ),
					 VME_GENREG_READ32( bdVaddr+0x4 ),
					 VME_GENREG_READ32( bdVaddr+0x8 ),
					 VME_GENREG_READ32( bdVaddr+0xc ));
			bdVaddr+=0x10;
		}
	}

 CLEANUP:
	VME4LDBG("<- DmaSetup\n");
	return rv < 0 ? rv : endBd;
}

/***********************************************************************/
/** Setup DMA for bounce buffer
 *
 */
static int DmaBounceSetup(
	VME4L_BRIDGE_HANDLE *h,
	VME4L_SPACE spc,
	size_t size,
	int direction,
	int modeFlags,
	vmeaddr_t vmeAddr,
	void **bounceBufP )
{
	int alignVme=4, rv=0;
	uint32_t bdAm, bdOff;
	size_t szlimit=PLDZ002_BOUNCE_CHUNKSIZE_LIMIT_A32D32;

	/* DMA controller supports only BLT spaces */
	switch( spc ){
	case VME4L_SPC_A24_D16_BLT:
	case VME4L_SPC_A24_D32_BLT:
	case VME4L_SPC_A32_D32_BLT:
		bdAm = h->addrModShadow[spc];
		break;
	case VME4L_SPC_A32_D64_BLT:
		bdAm = h->addrModShadow[spc];
		alignVme = 8;
		szlimit=PLDZ002_BOUNCE_CHUNKSIZE_LIMIT_A32D64;
		break;
	case VME4L_SPC_A16_D16:
	case VME4L_SPC_A16_D32:
	case VME4L_SPC_A24_D16:
	case VME4L_SPC_A24_D32:
	case VME4L_SPC_A32_D32:
		bdAm = h->addrModShadow[spc];
		break;
	default:
		return -EINVAL;
	}

	bdOff = PLDZ002_DMABD_OFF_RV9(0);
	VME4LDBG("size = 0x%x  direction: %s\n", size, direction ? "write to VME" : "read from VME");

	if( size > szlimit )
		size = szlimit;

	/*--- check alignment/size ---*/
	if( (vmeAddr & (alignVme-1)) || (size & (alignVme-1)) ){
		printk(KERN_ERR_PFX "%s: Bounce DMA setup bad alignment/len "
		       "0x%llx 0x%llx\n", __func__, vmeAddr, (uint64_t) size);
		rv = -EINVAL;
		goto CLEANUP;
	}

	if( direction ){
		/* write to VME */
		VME_REG_DMABD_WR32( bdOff+0x0, vmeAddr );
		VME_REG_DMABD_WR32( bdOff+0x4, LOCAL_SRAM_A21ADDR );
		VME_REG_DMABD_WR32( bdOff+0x8, (size>>2) - 1 );
		VME_REG_DMABD_WR32( bdOff+0xc,
							 PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_SRAM ) |
							 PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_VME ) |
							 bdAm | PLDZ002_DMABD_END);
	}
	else {
		/* read from VME */
		VME_REG_DMABD_WR32( bdOff+0x0, BOUNCE_SRAM_A21ADDR );
		VME_REG_DMABD_WR32( bdOff+0x4, vmeAddr );
		VME_REG_DMABD_WR32( bdOff+0x8, (size>>2) - 1 );
		VME_REG_DMABD_WR32( bdOff+0xc,
						 PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_VME ) |
						 PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_SRAM ) |
						 bdAm | PLDZ002_DMABD_END );
	}

	VME4LDBG("bounce DMA BD setup:\n");
	VME4LDBG("phys 0x%x virt 0x%08x = 0x%08x",
			 h->bounce.phys + bdOff+0x0, h->bounce.vaddr + bdOff+0x0, VME_REG_DMABD_RD32(bdOff+0x0));
	VME4LDBG("phys 0x%x virt 0x%08x = 0x%08x",
			 h->bounce.phys + bdOff+0x4, h->bounce.vaddr + bdOff+0x4, VME_REG_DMABD_RD32(bdOff+0x4));
	VME4LDBG("phys 0x%x virt 0x%08x = 0x%08x",
			 h->bounce.phys + bdOff+0x8, h->bounce.vaddr + bdOff+0x8, VME_REG_DMABD_RD32(bdOff+0x8));
	VME4LDBG("phys 0x%x virt 0x%08x = 0x%08x",
			 h->bounce.phys + bdOff+0xc, h->bounce.vaddr + bdOff+0xc, VME_REG_DMABD_RD32(bdOff+0xc));

 CLEANUP:
	*bounceBufP = h->bounce.vaddr;
	return rv < 0 ? rv : size;
}

/***********************************************************************/
/** Start DMA with the scatter list setup by dmaSetup
 *
 */
static int DmaStart( VME4L_BRIDGE_HANDLE *h )
{

	uint8_t dmastat = VME_REG_READ8( PLDZ002_DMASTA );

	if( dmastat  & PLDZ002_DMASTA_EN ){
		printk(KERN_ERR_PFX "%s: DMA busy! DMASTA=%02x\n",
		       __func__, dmastat );
		return -EBUSY;
	}
	if( dmastat  & PLDZ002_DMASTA_ERR ){
		printk(KERN_ERR_PFX "%s: DMA error pending, DMASTA=%02x. "
		      "Clearing it and continuing.\n", __func__, dmastat);
	}

	VME4LDBG("DmaStart..\n");

	VME_REG_WRITE8( PLDZ002_DMASTA, PLDZ002_DMASTA_IRQ | PLDZ002_DMASTA_ERR );
	/* start DMA and enable DMA interrupt */
	VME_REG_WRITE8( PLDZ002_DMASTA, PLDZ002_DMASTA_EN | PLDZ002_DMASTA_IEN);
	return 0;
}

/***********************************************************************/
/** Stop DMA
 */
static int DmaStop(	VME4L_BRIDGE_HANDLE *h )
{
	VME_REG_WRITE8( PLDZ002_DMASTA, PLDZ002_DMASTA_IRQ | PLDZ002_DMASTA_ERR );

	return 0;
}


/***********************************************************************/
/** Get DMA status (after DMA was started with dmaStart)
 *
 * \return 0=DMA finished ok, <0=finished with error >0 DMA running
 */
static int DmaStatus( VME4L_BRIDGE_HANDLE *h )
{
	uint8_t status = VME_REG_READ8( PLDZ002_DMASTA );

	VME4LDBG("pldz002 DmaStatus: 0x%02x\n", status );

	if( status & PLDZ002_DMASTA_EN )
		return 1;				/* runnig */

	if( status & PLDZ002_DMASTA_ERR )
		return -EIO;

	return 0;					/* ok */
}

/**********************************************************************/
/** Generate a VMEbus interrupt
 */
static int IrqGenerate(
	VME4L_BRIDGE_HANDLE *h,
	int level,
	int vector)
{
	int rv = _PLDZ002_INTERRUPTER_ID;

	if( (level < VME4L_IRQLEV_1) || (level > VME4L_IRQLEV_7) ||
		(vector > 255 ))
		return -EINVAL;

	PLDZ002_LOCK_STATE();

	if( VME_REG_READ8( PLDZ002_INTR ) & PLDZ002_INTR_INTEN )
		rv = -EBUSY;			/* interrupter busy */
	else {
		VME_REG_WRITE8( PLDZ002_INTID, vector );
		VME_REG_WRITE8( PLDZ002_INTR, level );
		VME_REG_WRITE8( PLDZ002_INTR, level | PLDZ002_INTR_INTEN );
	}

	PLDZ002_UNLOCK_STATE();


	return rv;
}

/**********************************************************************/
/** Check if generated interrupt has been acknowledged
 */
static int IrqGenAcked(
	VME4L_BRIDGE_HANDLE *h,
	int id)
{
	int rv;

	if( id != _PLDZ002_INTERRUPTER_ID )
		return -EINVAL;

	if( VME_REG_READ8( PLDZ002_INTR ) & PLDZ002_INTR_INTEN )
		rv = 0;					/* not yet acked */
	else
		rv = 1;					/* acked */
	return rv;
}

/**********************************************************************/
/** Clear pending interrupter
 */
static int IrqGenClear(
	VME4L_BRIDGE_HANDLE *h,
	int id)
{
	if( id != _PLDZ002_INTERRUPTER_ID )
		return -EINVAL;

	PLDZ002_LOCK_STATE();

	VME_REG_CLRMASK8( PLDZ002_INTR, PLDZ002_INTR_INTEN );

	PLDZ002_UNLOCK_STATE();
	return 0;
}


/***********************************************************************/
/** Check if system controller function has been enabled
 */
static int SysCtrlFuncGet( VME4L_BRIDGE_HANDLE *h)
{
	return !!(VME_REG_READ8( PLDZ002_SYSCTL ) & PLDZ002_SYSCTL_SYSCON);
}


/***********************************************************************/
/** Enable/Disable system controller function
 *
 */
static int SysCtrlFuncSet( VME4L_BRIDGE_HANDLE *h, int state)
{
	PLDZ002_LOCK_STATE();

	if( state )
		VME_REG_SETMASK8( PLDZ002_SYSCTL, PLDZ002_SYSCTL_SYSCON);
	else
		VME_REG_CLRMASK8( PLDZ002_SYSCTL, PLDZ002_SYSCTL_SYSCON);

	PLDZ002_UNLOCK_STATE();
	return 0;
}

/***********************************************************************/
/** Generate VME system reset
 */
static int SysReset( VME4L_BRIDGE_HANDLE *h )
{
	PLDZ002_LOCK_STATE();
	VME_REG_SETMASK8( PLDZ002_SYSCTL, PLDZ002_SYSCTL_SYSRES);
	PLDZ002_UNLOCK_STATE();

	return 0;
}

/***********************************************************************/
/** Check if arbiter has detected a timeout
 */
static int ArbToutGet( VME4L_BRIDGE_HANDLE *h, int clear)
{
	int state;

	PLDZ002_LOCK_STATE();
	state = !!(VME_REG_READ8( PLDZ002_SYSCTL ) & PLDZ002_SYSCTL_ATO);
	if( clear )
		VME_REG_SETMASK8( PLDZ002_SYSCTL, PLDZ002_SYSCTL_ATO);
	PLDZ002_UNLOCK_STATE();
	return state;
}

/**********************************************************************/
/** Get information about last VME bus error
 *
 */
static int BusErrGet(
	VME4L_BRIDGE_HANDLE *h,
	int *attrP,
	vmeaddr_t *addrP,
	int clear )
{
	unsigned long ps;
	int rv;

	PLDZ002_LOCK_STATE_IRQ(ps);

	rv = h->haveBerr;
	VME4LDBG( "BusErrGet: h->haveBerr=%d\n", h->haveBerr );
	*attrP = h->berrAcc;
	*addrP = h->berrAddr;

	if( clear ) {
		h->haveBerr = 0;
		h->berrAddr = 0;
		h->berrAcc  = 0;
	}

	PLDZ002_UNLOCK_STATE_IRQ(ps);

	VME4LDBG( "BusErrGet: returning rv=%d\n", rv );
	return rv;
}

/**********************************************************************/
/** Get VMEbus requester mode
 */
static int RequesterModeGet( VME4L_BRIDGE_HANDLE *h )
{
	unsigned long ps;
	int rv;

	PLDZ002_LOCK_STATE_IRQ(ps);
	rv = !!(h->mstrShadow & PLDZ002_MSTR_REQ);
	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return rv;
}

/**********************************************************************/
/** Set VMEbus requester mode
 */
static int RequesterModeSet( VME4L_BRIDGE_HANDLE *h, int state)
{
	unsigned long ps;

	PLDZ002_LOCK_STATE_IRQ(ps);

	if( state )
		h->mstrShadow |= PLDZ002_MSTR_REQ;
	else
		h->mstrShadow &= ~PLDZ002_MSTR_REQ;

	VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow);
	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return 0;
}

/**********************************************************************/
/** Set VMEbus requester level
 *
 */
static int RequesterLevelSet( VME4L_BRIDGE_HANDLE *h, int lvl)
{
	unsigned long ps;

	if ((lvl != PLDZ002_REQ_LEVEL_0 ) && (lvl != PLDZ002_REQ_LEVEL_1 ) &&
	    (lvl != PLDZ002_REQ_LEVEL_2 ) && (lvl != PLDZ002_REQ_LEVEL_3 ))
	{
	    return -EINVAL;
	}

	PLDZ002_LOCK_STATE_IRQ( ps );
	h->reqLevel = lvl;
	VME_REG_WRITE8( PLDZ002_REQUEST_LVL, h->reqLevel );
	PLDZ002_UNLOCK_STATE_IRQ( ps );
	return 0;
}

/**********************************************************************/
/** Get VMEbus requester level
 */
static int RequesterLevelGet( VME4L_BRIDGE_HANDLE *h )
{
	return h->reqLevel;
}

/**********************************************************************/
/** Get slot nr of VME board (=GA[4:0] pins converted to slot nr.)
 */
static int GeoAddrGet( VME4L_BRIDGE_HANDLE *h )
{

  int slotnr = (VME_REG_READ16( PLDZ002_GEO_ADDR ) >> PLDZ002_GEO_ADDR_SHIFT) & PLDZ002_GEO_ADDR_MASK;
  return slotnr;
}

/**********************************************************************/
/** Get underlaying struct pci_dev * of this bridge
 */
static struct pci_dev * PciDevGet( VME4L_BRIDGE_HANDLE *h )
{
       return h->chu->pdev;
}


/**********************************************************************/
/** Get VMEbus address modifier
 */
static int AddrModifierGet( VME4L_SPACE spc, VME4L_BRIDGE_HANDLE *h )
{
	unsigned long ps;
	int rv;

	PLDZ002_LOCK_STATE_IRQ(ps);
	VME4LDBG("AddrModifierGet: spc = %d (%s) AM = 0x%02x\n",
			 spc, vme4l_get_space_ent(spc)->isBlt ? "BLT" : "no BLT", h->addrModShadow[spc] );
	rv = h->addrModShadow[spc];
	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return rv;
}

/**********************************************************************/
/** Set VMEbus address modifier for the space:
 *    0 (00b) non-privileged data access (default)
 *    1 (01b) non-privileged program access
 *    2 (10b) supervisory data access
 *    3 (11b) supervisory program access
 */
static int AddrModifierSet( VME4L_SPACE spc, VME4L_BRIDGE_HANDLE *h, char addrMod)
{
	int retval=0;
	unsigned long ps;
	unsigned int isSupervisor=0;

	PLDZ002_LOCK_STATE_IRQ(ps);

	/* the AM definition is different for BLT/non BLT spaces. Non BLT AMs are
	   set in MSTR[8:13] (or PLDZ002_AMOD), and BLT AMs are considered during DMA BD
	   setup. */
	/* 0: set supervisor , 1: supervisor flag not set (non-privileged) */
	isSupervisor = (addrMod & 0x02) >> 1;

	switch (spc) {
		/* the non BLT spaces: write directly to AMOD, clear previously set CR/CSR access */
	case VME4L_SPC_A16_D16:
	case VME4L_SPC_A16_D32:
		h->mstrAMod &=~VME4l_SPC_A16_AM_MASK;
		h->mstrAMod |= (addrMod & 0x03) << 0;
		VME_REG_WRITE8( PLDZ002_AMOD, h->mstrAMod);
		break;

	case VME4L_SPC_A24_D16:
	case VME4L_SPC_A24_D32:
		h->mstrAMod &=~VME4l_SPC_A24_AM_MASK;
		h->mstrAMod |= (addrMod & 0x03) << 2;
		VME_REG_WRITE8( PLDZ002_AMOD, h->mstrAMod);
		break;

	case VME4L_SPC_A32_D32:
		h->mstrAMod &=~VME4l_SPC_A32_AM_MASK;
		h->mstrAMod |= (addrMod & 0x03) << 4;
		VME_REG_WRITE8( PLDZ002_AMOD, h->mstrAMod);
		break;

		/* the BLT spaces: take AM bit[1] (=supervisory flag) and OR in defaults.
		 * CR/CSR not relevant here. */
	case VME4L_SPC_A24_D16_BLT:
		h->addrModShadow[spc] = PLDZ002_DMABD_AM_A24D16 | (isSupervisor << 8);
		break;
	case VME4L_SPC_A24_D32_BLT:
		h->addrModShadow[spc] =	PLDZ002_DMABD_AM_A24D32 | (isSupervisor << 8);
		break;
	case VME4L_SPC_A24_D64_BLT:
		h->addrModShadow[spc] =	PLDZ002_DMABD_AM_A24D64 | (isSupervisor << 8);
		break;
	case VME4L_SPC_A32_D32_BLT:
		h->addrModShadow[spc] =	PLDZ002_DMABD_AM_A32D32 | (isSupervisor << 8);
		break;
	case VME4L_SPC_A32_D64_BLT:
		h->addrModShadow[spc] =	PLDZ002_DMABD_AM_A32D64 | (isSupervisor << 8) ;
		break;
	default:
		retval = -ENOTTY;
		break;
	}

	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return retval;
}

/***********************************************************************/
/** Get VMEbus master write mode
 */
static int PostedWriteModeGet( VME4L_BRIDGE_HANDLE *h )
{
	unsigned long ps;
	int rv;

	PLDZ002_LOCK_STATE_IRQ(ps);
	rv = !!(h->mstrShadow & PLDZ002_MSTR_POSTWR);
	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return rv;
}

/***********************************************************************/
/** Control VMEbus master write mode
 */
static int PostedWriteModeSet( VME4L_BRIDGE_HANDLE *h, int state)
{
	unsigned long ps;

	PLDZ002_LOCK_STATE_IRQ(ps);

	if( state )
		h->mstrShadow |= PLDZ002_MSTR_POSTWR;
	else
		h->mstrShadow &= ~PLDZ002_MSTR_POSTWR;

	VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow);
	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return 0;
}

/**********************************************************************/
/** Perform VME Read-Modify-Write cycle
 */
static int RmwCycle(
	VME4L_BRIDGE_HANDLE *h,
	void *vaddr,
	void *physAddr,
	int accWidth,
	uint32_t mask,
	uint32_t *rvP)
{
	unsigned long ps;
	uint32_t readVal;
	int rv = 0;

	PLDZ002_LOCK_STATE_IRQ(ps);

	/* clear any BERR, clear posted write, set RMW */
	VME_REG_WRITE8( PLDZ002_MSTR,
					(h->mstrShadow | PLDZ002_MSTR_BERR | PLDZ002_MSTR_RMW )
					& ~PLDZ002_MSTR_POSTWR);

	switch( accWidth ){
	case 1:
		readVal = VME_WIN_READ8( vaddr );
		break;
	case 2:
		readVal = VME_WIN_READ16( vaddr );
		break;
	case 4:
		readVal = VME_WIN_READ32( vaddr );
		break;
	default:
		readVal = 0;
		rv = -EINVAL;
		break;
	}

	*rvP = readVal;				/* return org. value read */
	readVal |= mask;

	switch( accWidth ){
	case 1:
		VME_WIN_WRITE8( vaddr, readVal );
		break;
	case 2:
		VME_WIN_WRITE16( vaddr, readVal );
		break;
	case 4:
		VME_WIN_WRITE32( vaddr, readVal );
		break;
	default:
		rv = -EINVAL;
		break;
	}

	/* check for bus error */
	if( VME_REG_READ8( PLDZ002_MSTR ) & PLDZ002_MSTR_BERR ){
		StoreAndClearBuserror(h);
		rv = -EIO;
	}

	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return rv;
}

/**********************************************************************/
/** Perform VME Address-Only cycle
 */
static int AOnlyCycle( VME4L_BRIDGE_HANDLE *h, void *vAddr )
{
	unsigned long ps;
	PLDZ002_LOCK_STATE_IRQ(ps);

	/* clear any BERR, set AONLY */
	VME_REG_WRITE8( PLDZ002_MSTR,
					h->mstrShadow | PLDZ002_MSTR_BERR | PLDZ002_MSTR_AONLY );

	VME_WIN_READ8( vAddr );

	VME_REG_WRITE8( PLDZ002_MSTR,
					h->mstrShadow | PLDZ002_MSTR_BERR );
	PLDZ002_UNLOCK_STATE_IRQ(ps);

	return 0;
}

/***********************************************************************/
/** Setup VME slave window (for PLDZ002 >= rev 17)
 */
static int SlaveWindowCtrlFs3(
	VME4L_BRIDGE_HANDLE *h,
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	void **physAddrP,
	void **bDrvDataP)
{
	u16 val16;
	u32 val32;
	u32 idx=0;

	const struct {
		size_t minSize;
		size_t maxSize;
		vmeaddr_t maxAddr;
	} sizeTbl[] = {
		{   0x1000,     0x1000,     0x10000 },	/* ctrl 		*/
		{  0x10000,   0x100000,   0x1000000 },	/* a24 sram 	*/
		{ 0x100000,   0x100000,  0xFFFFFFFF },	/* a32 sram 	*/
		{  0x10000,   0x100000,   0x1000000 },	/* a24 ram 		*/
		{ 0x100000, 0x10000000,  0xFFFFFFFF },	/* a32 ram 		*/
	};

	VME4LDBG("pldz002::SlaveWindowCtrlFs3 vmeAddr=%llx size=%llx\n",
			 vmeAddr, size);

	if( (spc < VME4L_SPC_SLV0) || (spc > VME4L_SPC_SLV4 ) )
		return -ENOTTY;

	idx = spc-VME4L_SPC_SLV0;

	if( size ){

		/* enable slave access */
		if( (size < sizeTbl[idx].minSize) ||
			(size > sizeTbl[idx].maxSize) ||
			(vmeAddr >= sizeTbl[idx].maxAddr) ||
			(vmeAddr & (size-1)) ) {

			printk(KERN_ERR_PFX "%s: size=%lx out of range!\n",
			       __func__, size );
			return -EINVAL;
		}

		switch( spc ){
		case VME4L_SPC_SLV0:
			VME4LDBG("pldz002::SlaveWindowCtrlFs3  enable VME4L_SPC_SLV0\n");

			VME_REG_WRITE8( PLDZ002_SLV16,
							((vmeAddr >> 12) & 0xf) | PLDZ002_SLVxx_EN );
			*physAddrP = (void *)h->regs.phys;
			break;

		case VME4L_SPC_SLV1:
			VME4LDBG("pldz002::SlaveWindowCtrlFs3  enable VME4L_SPC_SLV1\n");
			val16 = ((vmeAddr >> 20) & 0xf);
			val16 |= PLDZ002_SLVxx_EN;
			val16 |= (((vmeAddr >> 16) & 0xf) << 8);
			val16 |= ((((size-1)>>16)&0xf) ^ 0xf) << 12;

			VME_REG_WRITE16( PLDZ002_SLV24, val16 );
			*physAddrP = (void *)h->sram.phys;
			break;

		case VME4L_SPC_SLV2:
			VME4LDBG("pldz002::SlaveWindowCtrlFs3  enable VME4L_SPC_SLV2\n");
			val32 = ((vmeAddr >> 28) & 0xf);
			val32 |= PLDZ002_SLVxx_EN;
			val32 |= (((vmeAddr >> 20) & 0xff) << 8);
			val32 |= ((((size-1)>>20)&0xff) ^ 0xff) << 16;

			VME_REG_WRITE32( PLDZ002_SLV32, val32 );
			*physAddrP = (void *)h->sram.phys;
			break;

		case VME4L_SPC_SLV3:
		case VME4L_SPC_SLV4:
			VME4LDBG("pldz002::SlaveWindowCtrlFs3  enable VME4L_SPC_SLV3/4\n");
			/*
			 * bus mastered shared window.
			 * Since both A24 and A32 window hits the same PCI address,
			 * check if they're consistent
			 */
			if( h->bmShmem.size == 0 ){
				/* currently not setup, allocate a new one */
				dma_addr_t dmaAddr = 0;

				h->bmShmem.vaddr = dma_alloc_coherent(&h->chu->pdev->dev, size, &dmaAddr, GFP_KERNEL);
				if (dma_mapping_error( &h->chu->pdev->dev, dmaAddr )) {
					printk(KERN_ERR_PFX "%s: dma_alloc_coherent failed, exiting.\n",
					       __func__);
					return -ENOMEM;
				}
				h->bmShmem.phys = dmaAddr;

				VME4LDBG("pldz002: dma_alloc_coherent: v=%p p=%x (%llx)\n", h->bmShmem.vaddr, h->bmShmem.phys, size );

				if( h->bmShmem.vaddr == NULL ) {
					printk(KERN_ERR_PFX "%s: can't alloc BM slave window of 0x%llx bytes\n",
					       __func__, (uint64_t) size );
					return -ENOSPC;
				}
				h->bmShmem.size = size;

				/* clear region */
				memset( h->bmShmem.vaddr, 0, size );
				VME_REG_WRITE32( PLDZ002_PCI_OFFSET, h->bmShmem.phys );
			}
			else {
				if( h->bmShmem.size != size ){
					printk(KERN_ERR_PFX "%s: won't change BM slave window if used\n",
					       __func__);
					return -EBUSY;
				}
			}

			if( spc == VME4L_SPC_SLV3 ){
				val16 = ((vmeAddr >> 20) & 0xf);
				val16 |= PLDZ002_SLVxx_EN;
				val16 |= (((vmeAddr >> 16) & 0xf) << 8);
				val16 |= ((((size-1)>>16)&0xf) ^ 0xf) << 12;

				VME_REG_WRITE16( PLDZ002_SLV24_PCI, val16 );
			}
			else {
				val32 = ((vmeAddr >> 28) & 0xf);
				val32 |= PLDZ002_SLVxx_EN;
				val32 |= (((vmeAddr >> 20) & 0xff) << 8);
				val32 |= ((((size-1)>>20)&0xff) ^ 0xff) << 16;
				VME_REG_WRITE32( PLDZ002_SLV32_PCI, val32 );
			}
			*physAddrP = (void *)h->bmShmem.phys;

		default:
			break;
		}
	}
	else {
		/* disable slave access */
		switch( spc ){
		case VME4L_SPC_SLV0:
			VME_REG_WRITE8( PLDZ002_SLV16, 0 );
			break;
		case VME4L_SPC_SLV1:
			VME_REG_WRITE16( PLDZ002_SLV24, 0 );
			break;
		case VME4L_SPC_SLV2:
			VME_REG_WRITE32( PLDZ002_SLV32, 0 );
			break;
		case VME4L_SPC_SLV3:
		case VME4L_SPC_SLV4:
			if( spc == VME4L_SPC_SLV3 )
				VME_REG_WRITE16( PLDZ002_SLV24_PCI, 0 );
			else
				VME_REG_WRITE32( PLDZ002_SLV32_PCI, 0 );

			/* if both windows are closed, free kernel mem */
			if( !(VME_REG_READ16( PLDZ002_SLV24_PCI ) & PLDZ002_SLVxx_EN) &&
				!(VME_REG_READ32( PLDZ002_SLV32_PCI ) & PLDZ002_SLVxx_EN)) {

				VME4LDBG("pldz002: dma_free_coherent: v=%p p=%x (%llx)\n",
						 h->bmShmem.vaddr, h->bmShmem.phys, (uint64_t) size );

				dma_free_coherent(&h->chu->pdev->dev,
								  h->bmShmem.size,
								  h->bmShmem.vaddr,
								  h->bmShmem.phys);
				h->bmShmem.size = 0;
			}
			break;
		default:
			break;
		}
	}

	return 0;
}

/**********************************************************************/
/** Read mailbox value
 */
int MboxReadFs3(
	VME4L_BRIDGE_HANDLE *h,
	int mbox,
	uint32_t *valP)
{
	uint32_t val;

	if( mbox<0 || mbox>3 )
		return -EINVAL;

	/*
	 * the content of the mailbox registers (as all other registers
	 * accessible from VME is swapped by the FPGA
	 *
	 * i.e. on PowerPc, no/double SW swapping needed
	 *      on X86, SW swapping needed!
	 */
	val = VME_GENREG_READ32( (char *)h->sramRegs.vaddr +
							 PLDZ002_MAILBOX_RV11(mbox) );
	*valP = _PLDZ002_SWAP32( val );

	return 0;
}

/**********************************************************************/
/** Write mailbox value
 */
int MboxWriteFs3(
	VME4L_BRIDGE_HANDLE *h,
	int mbox,
	uint32_t val)
{
	if( mbox<0 || mbox>3 )
		return -EINVAL;

	/* see swapping note above */
	VME_GENREG_WRITE32( (char *)h->sramRegs.vaddr +
						PLDZ002_MAILBOX_RV11(mbox),
						_PLDZ002_SWAP32(val));
	return 0;
}

/**********************************************************************/
/** Read location monitor reg
 */
int LocMonRegReadFs2(
	VME4L_BRIDGE_HANDLE *h,
	int reg,
	uint32_t *valP)
{
	switch( reg ){
	case 0:
		reg = PLDZ002_LM_STAT_CTRL_0;
		break;
	case 1:
		reg = PLDZ002_LM_STAT_CTRL_1;
		break;
	case 2:
		reg = PLDZ002_LM_ADDR_0;
		break;
	case 3:
		reg = PLDZ002_LM_ADDR_1;
		break;
	default:
		return -EINVAL;
	}

	*valP = VME_REG_READ32( reg );
	return 0;
}

/**********************************************************************/
/** Write location monitor value
 * When CTRL_0/1 is written, don't touch interrupt enable flag
 */
int LocMonRegWriteFs2(
	VME4L_BRIDGE_HANDLE *h,
	int reg,
	uint32_t val)
{
	if( reg<0 || reg>3 )
		return -EINVAL;

	switch( reg ){
	case 0:
		reg = PLDZ002_LM_STAT_CTRL_0;
		val = (VME_REG_READ32( reg ) & PLDZ002_LM_STAT_CTRL_IRQ_EN) |
			  (val & ~PLDZ002_LM_STAT_CTRL_IRQ_EN);
		break;
	case 1:
		reg = PLDZ002_LM_STAT_CTRL_1;
		val = (VME_REG_READ32( reg ) & PLDZ002_LM_STAT_CTRL_IRQ_EN) |
			  (val & ~PLDZ002_LM_STAT_CTRL_IRQ_EN);
		break;
	case 2:
		reg = PLDZ002_LM_ADDR_0;
		break;
	case 3:
		reg = PLDZ002_LM_ADDR_1;
		break;
	default:
		return -EINVAL;
	}

	VME_REG_WRITE32( reg, val);
	return 0;
}


static VME4L_BRIDGE_DRV G_bridgeDrv = {
	.revisionInfo		= RevisionInfo,
	.requestAddrWindow 	= RequestAddrWindow,
	.releaseAddrWindow 	= ReleaseAddrWindow,
	.irqLevelCtrl		= IrqLevelCtrl,
	.readPio8			= ReadPio8,
	.readPio16			= ReadPio16,
	.readPio32			= ReadPio32,
	.writePio8			= WritePio8,
	.writePio16			= WritePio16,
	.writePio32			= WritePio32,
	.dmaSetup			= NULL,
	.dmaBounceSetup		= NULL,
	.dmaStart			= DmaStart,
	.dmaStop			= DmaStop,
	.dmaStatus			= DmaStatus,
	.irqGenerate		= IrqGenerate,
	.irqGenAcked		= IrqGenAcked,
	.irqGenClear		= IrqGenClear,
	.sysCtrlFuncGet		= SysCtrlFuncGet,
	.sysCtrlFuncSet		= SysCtrlFuncSet,
	.retrieveSysIrq 	= GetSysIrq,
	.sysReset			= SysReset,
	.arbToutGet			= ArbToutGet,
	.busErrGet			= BusErrGet,
	.requesterModeGet	= RequesterModeGet,
	.requesterModeSet	= RequesterModeSet,
	.requesterLevelSet  = RequesterLevelSet,
	.requesterLevelGet  = RequesterLevelGet,
	.geoAddrGet         = GeoAddrGet,
	.pciDevGet          = PciDevGet,
	.addrModifierGet	= AddrModifierGet,
	.addrModifierSet	= AddrModifierSet,
	.postedWriteModeGet	= PostedWriteModeGet,
	.postedWriteModeSet	= PostedWriteModeSet,
	.rmwCycle			= RmwCycle,
	.aOnlyCycle			= NULL,
	.slaveWindowCtrl	= SlaveWindowCtrlFs3
};


/*******************************************************************/
/** evaluate the highest Bit set in an 8bit Value
 *
 * \param val  Value to evaluate
 *
 */
static inline int HighestBitSet( u8 val )
{
	int rv;

	if( val & 0x80 ) 	  rv = 7;
	else if( val & 0x40 ) rv = 6;
	else if( val & 0x20 ) rv = 5;
	else if( val & 0x10 ) rv = 4;
	else if( val & 0x08 ) rv = 3;
	else if( val & 0x04 ) rv = 2;
	else if( val & 0x02 ) rv = 1;
	else if( val & 0x01 ) rv = 0;
	else rv = -1;
	return rv;
}

/*******************************************************************/
/** Check if error occured on VME Bus on interrupt occurance
 *
 * \param irq		PIC irq vector (not VME vector!)
 * \param h		  	Handle of VME4L bridge.
 *
 * \return          0 if no bus error or 1 if error.
 *                  -1 if handle invalid.
 */
static int PldZ002_CheckVmeBusError( VME4L_BRIDGE_HANDLE *h,
									 int *vecP,
									 int *levP )
{

	uint8_t mstr=0;

	if (NULL == h) {
		printk(" *** %s: Bridge handle is NULL !\n", __FUNCTION__);
		return -1;
	}

	mstr = VME_REG_READ8( PLDZ002_MSTR );

	if( (mstr & (PLDZ002_MSTR_BERR | PLDZ002_MSTR_IBERREN )) ==	(PLDZ002_MSTR_BERR | PLDZ002_MSTR_IBERREN ) )
	{
		/* bus error detected */
		h->haveBerr = 1;
		*levP 	= VME4L_IRQLEV_BUSERR;
		*vecP 	= VME4L_IRQVEC_BUSERR;

		StoreAndClearBuserror(h);
		return 1;
	}
	return 0;
}


/*******************************************************************/
/** Acknowledge the pending interrupts
 *
 * \param h		  	Handle to VME4L bridge
 * \param vecP		where to store VME IRQ vector
 * \param levP		where to store level
 *
 */
static int PldZ002_ProcessPendingVmeInterrupts( VME4L_BRIDGE_HANDLE *h,
										   int *vecP,
										   int *levP )
{

	uint8_t istat = 0;

	if (NULL == h){
		printk(" *** %s: Bridge handle is NULL!\n", __FUNCTION__);
		return -1;
	}

	istat = VME_REG_READ8( PLDZ002_MSTR ) & 0xff;

    istat = VME_REG_READ8( PLDZ002_ISTAT  );
	istat &= VME_REG_READ8( PLDZ002_IMASK );

    if( istat != 0 ){
		/*--- decode *levP  ---*/
		if( istat & 0x80 ) 	    *levP = VME4L_IRQLEV_7;
		else if( istat & 0x40 ) *levP = VME4L_IRQLEV_6;
		else if( istat & 0x20 ) *levP = VME4L_IRQLEV_5;
		else if( istat & 0x10 ) *levP = VME4L_IRQLEV_4;
		else if( istat & 0x08 ) *levP = VME4L_IRQLEV_3;
		else if( istat & 0x04 ) *levP = VME4L_IRQLEV_2;
		else if( istat & 0x02 ) *levP = VME4L_IRQLEV_1;
		else if( istat & 0x01 ){
			*levP = VME4L_IRQLEV_ACFAIL;
			*vecP = VME4L_IRQVEC_ACFAIL;
			return 1;
		}
		/* fetch vector (VME IACK cycle) */
		*vecP = VME_WIN_READ8( (char *)h->iack.vaddr + ( *levP<<1 ) + 1 );
		/* check for bus error during IACK (spurious irq) */
		if( VME_REG_READ8( PLDZ002_MSTR ) & PLDZ002_MSTR_BERR ){
			/* clear bus error */
			StoreAndClearBuserror(h);
			*vecP = VME4L_IRQVEC_SPUR;
		}
		return 1;
	}

	return 0;

}

/*******************************************************************/
/** Check occurance of other IRQ causes
 *
 * \param h		  	Handle to VME4L bridge
 * \param vecP		where to store VME IRQ vector
 * \param levP		where to store level
 *
 * \brief           In this function all other IRQ causes are
 *				    processed, these are: DMA finished, Mailbox receive,
 *					location monitor
 */
static int PldZ002_CheckMiscVmeInterrupts( VME4L_BRIDGE_HANDLE *h,
										 int *vecP,
										 int *levP)
{
	uint8_t dmastat = 0;

	if (NULL == h){
		printk(" *** %s: Bridge handle is NULL!\n", __FUNCTION__);
		return -1;
	}

	/* check for DMA finished / DMA error */
	dmastat = VME_REG_READ8( PLDZ002_DMASTA );
	if( dmastat & ( PLDZ002_DMASTA_IRQ | PLDZ002_DMASTA_ERR) ) {
		/* 1. check if DMA error occured ? if yes, stop DMA activity and clear DMA error & clear DMA */
		if( dmastat & PLDZ002_DMASTA_IRQ ) {
			/* regular finished DMA. Reset DMA irq */
		VME_REG_WRITE8( PLDZ002_DMASTA, PLDZ002_DMASTA_IRQ );
		*levP = VME4L_IRQLEV_DMAFINISHED;
		} else {
			printk(KERN_ERR_PFX "%s: DMA error occured, stop DMA and clear DMA IRQ and error\n",
			       __func__);
			/* clear by writing '1' to the bits, DMA also stopped by setting its bit to 0 */
			VME_REG_WRITE8( PLDZ002_DMASTA, PLDZ002_DMASTA_IRQ | PLDZ002_DMASTA_ERR);
			*levP = VME4L_IRQLEV_BUSERR;
		}
		*vecP = 0;
		return 1;
	}

	/* check for mailbox irqs */
	*vecP = HighestBitSet( VME_REG_READ8( PLDZ002_MAIL_IRQ_STAT ));
	if( *vecP >= 0 ){
		*levP  = *vecP;
		/* clear pending bit */
		VME_REG_WRITE8( PLDZ002_MAIL_IRQ_STAT, 1<<*levP );
		/* warning: numbering must match vme4l.h! */
		*vecP 	+= VME4L_IRQVEC_MBOXRD(0);
		*levP 	+= VME4L_IRQLEV_MBOXRD(0);
		return 1;
	}
	/* check for location monitor irqs */
	if( (VME_REG_READ8( PLDZ002_LM_STAT_CTRL_0 ) &
				(PLDZ002_LM_STAT_CTRL_IRQ | PLDZ002_LM_STAT_CTRL_IRQ_EN)) ==
			(PLDZ002_LM_STAT_CTRL_IRQ | PLDZ002_LM_STAT_CTRL_IRQ_EN)){
		*vecP = VME4L_IRQVEC_LOCMON(0);
		*levP  = VME4L_IRQLEV_LOCMON(0);
		VME_REG_SETMASK8( PLDZ002_LM_STAT_CTRL_0,PLDZ002_LM_STAT_CTRL_IRQ);
		return 1;
	}
	if( (VME_REG_READ8( PLDZ002_LM_STAT_CTRL_1) &
				(PLDZ002_LM_STAT_CTRL_IRQ | PLDZ002_LM_STAT_CTRL_IRQ_EN)) ==
			(PLDZ002_LM_STAT_CTRL_IRQ | PLDZ002_LM_STAT_CTRL_IRQ_EN)){
		*vecP = VME4L_IRQVEC_LOCMON(1);
		*levP  = VME4L_IRQLEV_LOCMON(1);
		VME_REG_SETMASK8( PLDZ002_LM_STAT_CTRL_1,PLDZ002_LM_STAT_CTRL_IRQ);
		return 1;
	}

	return 0;
}

/***************************************************************************/
/** Central HW dependent VME Interrupt handler, processing acknowledge flags
 *
 * \param irq		PIC irq vector (not VME vector!)
 * \param dev_id	device specific handle
 *
 * \brief		this is the standard Linux kernel IRQ handler for VME
 *				bus devices. From here we dispatch everything to vme4l-core
 *
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
static irqreturn_t PldZ002Irq(int irq, void *dev_id, struct pt_regs *regs)
{
#else
static irqreturn_t PldZ002Irq(int irq, void *dev_id )
{
	struct pt_regs *regs = NULL;
# endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) */

	int vector=0, level=VME4L_IRQLEV_UNKNOWN;
	VME4L_BRIDGE_HANDLE *h 	= (VME4L_BRIDGE_HANDLE *)dev_id;
	int handled=1;
	/* VME4LDBG */
	PLDZ002_LOCK_STATE();

	/* 1. check for bus errors */
	if (PldZ002_CheckVmeBusError( h, &vector, &level ))
		goto DONE;

	/* 2. get pending VME interrupts, perform IACK */
	if (PldZ002_ProcessPendingVmeInterrupts( h, &vector, &level ))
		goto DONE;

	/* 3. check the other IRQ causes (DMA/Mailbox/location monitor) */
	if (PldZ002_CheckMiscVmeInterrupts(h, &vector, &level))
		goto DONE;

	/* no interrupt source -> exit */
	handled=0;

	printk(KERN_ERR_PFX "%s: unhandled irq! vec=%d lev=%d\n",
	       __func__, vector, level );
	goto EXIT;

 DONE:
	VME4LDBG("PldZ002Irq: vector=%d level=%d\n", vector, level );

	vme4l_irq( level, vector, regs );

 EXIT:
	PLDZ002_UNLOCK_STATE();

	return handled ? IRQ_HANDLED : IRQ_NONE;
}


/*******************************************************************/
/** check mem region/request it and ioremap it
 */
static int MapRegSpace( VME4L_RESRC *res, const char *name )
{
	res->vaddr = NULL;

	if (!request_mem_region( res->phys, res->size, name )) {
		return -EBUSY;
	}

#if 0
	/*
	 * this could be enabled on PowerPC to mark area for the
	 * bounce buffer as write through, so that reads will
	 * result in PCI bursts. However, in this case we need
	 * flush the cache before reading from SRAM and I don't
	 * know how...
	 */
	if( res->cache == _PLDZ002_WRITETHROUGH ){
		res->vaddr = __ioremap( res->phys, res->size, _PAGE_WRITETHRU);
	}
	else {
		res->vaddr = ioremap_nocache( res->phys, res->size );
	}
#else
	res->vaddr = ioremap_nocache( res->phys, res->size );
#endif
	VME4LDBG("PLDZ002: MapRegSpace %s: phys:%p, size: 0x%x, vaddr=%p\n", name,
			 res->phys, res->size, res->vaddr );

	if( res->vaddr == NULL ) {
		release_mem_region( res->phys, res->size );
		return -ENOMEM;
	}
	res->memReq = 1;

	return 0;
}

/*******************************************************************/
/** free ioremapped mem region
 */
static void FreeRegSpace( VME4L_RESRC *res )
{
	if( res->vaddr )
		iounmap( res->vaddr );

	if( res->memReq )
		release_mem_region( res->phys, res->size );
}

/*******************************************************************/
/** init bridge regs
 */
static void InitBridge( VME4L_BRIDGE_HANDLE *h )
{
	h->a32LongAddUsed = 0;
	h->mstrShadow = 0;
	h->haveBerr = 0;

	/* preset the DMA AMs with defaults */
	h->addrModShadow[VME4L_SPC_A16_D16]     = PLDZ002_DMABD_AM_A16D16;
	h->addrModShadow[VME4L_SPC_A16_D32]     = PLDZ002_DMABD_AM_A16D32;
	h->addrModShadow[VME4L_SPC_A24_D16]     = PLDZ002_DMABD_AM_A24D16;
	h->addrModShadow[VME4L_SPC_A24_D16_BLT] = PLDZ002_DMABD_AM_A24D16;
	h->addrModShadow[VME4L_SPC_A24_D32]     = PLDZ002_DMABD_AM_A24D32;
	h->addrModShadow[VME4L_SPC_A24_D32_BLT] = PLDZ002_DMABD_AM_A24D32;
	h->addrModShadow[VME4L_SPC_A24_D64_BLT] = PLDZ002_DMABD_AM_A24D64;
	h->addrModShadow[VME4L_SPC_A32_D32]     = PLDZ002_DMABD_AM_A32D32;
	h->addrModShadow[VME4L_SPC_A32_D32_BLT] = PLDZ002_DMABD_AM_A32D32;
	h->addrModShadow[VME4L_SPC_A32_D64_BLT] = PLDZ002_DMABD_AM_A32D64;

	VME_REG_WRITE8( PLDZ002_INTR, 0x00 );
	VME_REG_WRITE8( PLDZ002_IMASK, 0x00 );
	VME_REG_WRITE8( PLDZ002_MSTR, PLDZ002_MSTR_BERR );
	VME_REG_WRITE8( PLDZ002_SLV24, 0x00 );

	VME_REG_WRITE8( PLDZ002_SYSCTL, VME_REG_READ8(PLDZ002_SYSCTL)
					& ~PLDZ002_SYSCTL_SYSRES);

	VME_REG_WRITE8( PLDZ002_SLV16, 0 );
	VME_REG_WRITE8( PLDZ002_SLV32, 0 );

	/* clear DMA */
	VME_REG_WRITE8( PLDZ002_DMASTA,
			PLDZ002_DMASTA_IRQ | PLDZ002_DMASTA_ERR );

	/* clear locmon */
	VME_REG_WRITE8( PLDZ002_LM_STAT_CTRL_0, PLDZ002_LM_STAT_CTRL_IRQ );
	VME_REG_WRITE8( PLDZ002_LM_STAT_CTRL_1, PLDZ002_LM_STAT_CTRL_IRQ );

	/* clear mbox */
	VME_REG_WRITE8( PLDZ002_MAIL_IRQ_CTRL, 0 );
	VME_REG_WRITE8( PLDZ002_MAIL_IRQ_STAT, 0xff );

	VME_REG_WRITE16( PLDZ002_SLV24_PCI, 0 );
	VME_REG_WRITE32( PLDZ002_SLV32_PCI, 0 );
}


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

static int vme4l_probe( CHAMELEONV2_UNIT_T *chu )
{
	int rv, i, irqReq=0;
	VME4L_BRIDGE_HANDLE *h = &G_bHandle;
	unsigned int barval=0, barsave=0, barsize=0, barA32=0;
	CHAMELEONV2_UNIT_T u;

	printk(KERN_INFO "vme4l_probe: probing 16Z002 unit\n");
	switch ( chu->unitFpga.variant ) {
		case 2: /* control registers of new PLDZ002, w/ flex. A32 space */
		  memset( h, 0, sizeof(*h));	/* clear handle */
		  h->hasExtBerrInfo 	 = 1; /* this core supports also BERR address and attribute return */
		  h->bLongaddAdjustable  = 1;
		  h->A32BARsize          = PLDZ002_A32D32_SIZE_512M;
		  break;
		case 1:/* control registers of classic PLDZ002, perform further init */
		  memset( h, 0, sizeof(*h));	/* clear handle */
		  h->bLongaddAdjustable  = 0;
		  h->A32BARsize          = PLDZ002_A32D32_SIZE_512M;
		  break;
		case 0:
		  printk(KERN_ERR_PFX "%s: Variant 0 should not be defined for VME Core\n",
			 __func__);
		  return -EINVAL;
		default:
		  return 0;
	}

	/* save chameleon unit */
	h->chu = chu;

	/* gather all the other chameleon units. Only A25 has 8 units, A21 and others 7 */
	for (i = 0; i < PLDZ002_MAX_UNITS ; i++)
	{
		if (men_chameleonV2_unit_find( CHAMELEON_16Z002_VME, i, &u) != 0) {
				printk(KERN_ERR_PFX "%s: Did not find PLDZ002 unit %d\n",
				       __func__, i);
			rv = -EINVAL;
			goto CLEANUP;
		}

		h->spaces[u.unitFpga.variant] = (unsigned long) u.unitFpga.addr;
		VME4LDBG("found Z002 inst. %d, variant %d, address %p\n", i, u.unitFpga.variant,u.unitFpga.addr);

		if ( u.unitFpga.variant == PLDZ002_VAR_VMEA32 && h->bLongaddAdjustable ) {
		    barA32 = u.unitFpga.bar;
		    /* check BAR size. Tried using struct pci_dev.resource[] but end addr is not available there.. */
		    pci_read_config_dword(  chu->pdev, CHAM_BAR2PCI_BASE_ADDR(barA32), &barsave );
		    pci_write_config_dword( chu->pdev, CHAM_BAR2PCI_BASE_ADDR(barA32), 0xffffffff );
		    pci_read_config_dword(  chu->pdev, CHAM_BAR2PCI_BASE_ADDR(barA32), &barval  );
		    barsize = ~(barval & PCI_BASE_ADDRESS_MEM_MASK) + 1;
		    pci_write_config_dword( chu->pdev, PCI_BASE_ADDRESS_3, barsave); /* restore BAR */

		    switch (barsize) {
		    case  PLDZ002_A32D32_SIZE_512M:
		      h->longaddWidth = 3;
		      break;
		    case  PLDZ002_A32D32_SIZE_256M:
		      h->longaddWidth = 4;
		      break;
		    case  PLDZ002_A32D32_SIZE_128M:
		      h->longaddWidth = 5;
		      break;
		    case  PLDZ002_A32D32_SIZE_64M:
		      h->longaddWidth = 6;
		      break;
		    case  PLDZ002_A32D32_SIZE_32M:
		      h->longaddWidth = 7;
		      break;
		    case  PLDZ002_A32D32_SIZE_16M:
		      h->longaddWidth = 8;
		      break;
		    default: /* something is seriously wrong with that BAR... */
		      printk(KERN_ERR_PFX "%s: invalid size of A32 space BAR3: 0x%08x\n",
			     __func__, barsize );
		      rv = -EINVAL;
		      goto CLEANUP;
		    }

		    VME4LDBG("adjustable LONGADD Reg.: (bit width %d) (BAR%d size 0x%08x)\n",  h->longaddWidth, barA32, barsize );

		    h->A32BARsize = barsize;
		}

	}
	printk( KERN_INFO "vme-pldz002-cham: found bridge (rev %d), irq %d\n",
		   chu->unitFpga.revision, chu->pdev->irq);

	h->regs.phys = (unsigned long)chu->unitFpga.addr + PLDZ002_CTRL_SPACE;
	h->regs.size = PLDZ002_CTRL_SIZE;

	h->iack.phys = (unsigned long)chu->unitFpga.addr + PLDZ002_IACK_SPACE;
	h->iack.size = PLDZ002_IACK_SIZE;

	/*--- request/map permanent spaces ---*/
	if( (rv = MapRegSpace( &h->regs, "pldz002-regs" )) )
		goto CLEANUP;

	if( (rv = MapRegSpace( &h->iack, "pldz002-iack" )) )
		goto CLEANUP;

	if( _PLDZ002_USE_BOUNCE_DMA(h) ){
		h->bounce.phys = h->spaces[CHAM_SPC_SRAM] /* + BOUNCE_DMABD_BASE */;
		h->bounce.size = BOUNCE_SRAM_SIZE;
		h->bounce.cache = _PLDZ002_WRITETHROUGH;

		if( (rv = MapRegSpace( &h->bounce, "pldz002-bounce" )) )
			goto CLEANUP;
	}
	else {
		h->sramRegs.phys = h->spaces[CHAM_SPC_SRAM] + PLDZ002_RV11_SRAMREGS;
		h->sramRegs.size = 0x800;
		if( (rv = MapRegSpace( &h->sramRegs, "pldz002-sramreg" )) )
			goto CLEANUP;
	}

	/* sram as slave window (don't ioremap) */
	h->sram.phys =  h->spaces[CHAM_SPC_SRAM];
	h->sram.size = PLDZ002_LRAM_SIZE;
	if( _PLDZ002_USE_BOUNCE_DMA(h) )
		h->sram.size -= BOUNCE_SRAM_SIZE;

	rv = pci_enable_msi(chu->pdev); /* using pci_enable_msi_block would be nicer,
					   but powerpc linux doesn't support it... */
	if (rv != 0 ) {
		printk(KERN_ERR_PFX "%s: Could not allocate enough msi interrupts: %d\n",
		       __func__, rv);
		goto CLEANUP;
	} else {
		/*normal linux kernel mode: PldZ002Irq is a standard linux IRQ handler */
		if( (rv = request_irq( chu->pdev->irq, PldZ002Irq,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
						IRQF_SHARED,
#else
						SA_SHIRQ,
#endif
						"Z002_LX",
						h))<0 )
			goto CLEANUP;
	}
	VME4LDBG("Got MSI %x\n", chu->pdev->irq);

	irqReq++;

	/*--- init bridge ---*/
	InitBridge( h );

	/* allow bus error interrupts */
	h->mstrShadow = PLDZ002_MSTR_IBERREN;
	VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow );

	/* initialise Requester level shadow register with default value */
	h->reqLevel = PLDZ002_REQ_LEVEL_3;

	spin_lock_init( &h->lockState );

	/*--- setup function pointers depending on feature level ---*/
	if( _PLDZ002_USE_BOUNCE_DMA(h) ) {
		VME4LDBG("G_bridgeDrv: using DmaBounceSetup\n");
		G_bridgeDrv.dmaBounceSetup 	= DmaBounceSetup;
	}
	else {
		VME4LDBG("G_bridgeDrv: using DmaSetup (direct RAM->VME transfer)\n");
		G_bridgeDrv.dmaSetup 		= DmaSetup;
		pci_set_master( h->chu->pdev );	/* enable bus mastering */
	}

	G_bridgeDrv.aOnlyCycle		= AOnlyCycle;
	G_bridgeDrv.locMonRegRead	= LocMonRegReadFs2;
	G_bridgeDrv.locMonRegWrite	= LocMonRegWriteFs2;
	G_bridgeDrv.slaveWindowCtrl	= SlaveWindowCtrlFs3;
	G_bridgeDrv.mboxRead		= MboxReadFs3;
	G_bridgeDrv.mboxWrite		= MboxWriteFs3;
	/*--- register to VME4L core ---*/
	rv = vme4l_register_bridge_driver( &G_bridgeDrv, h );

	if( rv != 0 )
		goto CLEANUP;

	return rv;

 CLEANUP:
	printk(KERN_ERR_PFX "%s: Init error %d\n", __func__, -rv);

	if( irqReq ){

		free_irq( chu->pdev->irq, h );
		pci_disable_msi(chu->pdev);
	}

	FreeRegSpace( &h->regs 		);
	FreeRegSpace( &h->sramRegs 	);
	FreeRegSpace( &h->iack 		);
	FreeRegSpace( &h->bounce 	);

	return rv;
}


static int __init vme4l_pldz002_init_module(void)
{
	printk( KERN_INFO "%s ", __FUNCTION__);

    if (!men_chameleonV2_register_driver( &G_driver ))
		return -ENODEV;  /* couldnt find requested unit */
	else
		return 0;
}

static void __exit vme4l_pldz002_cleanup_module(void)
{
    men_chameleonV2_unregister_driver( &G_driver );
}

static int vme4l_remove( CHAMELEONV2_UNIT_T *chu )
{
  int var = chu->unitFpga.variant;

       if (( var == 1) || (var == 2 )) {
		VME4L_BRIDGE_HANDLE *h = &G_bHandle;
		printk( KERN_DEBUG "vme4l_pldz002_cleanup_module\n");
		InitBridge(h);

		vme4l_unregister_bridge_driver();
		InitBridge( h );			/* clear regs to default state */
		FreeRegSpace( &h->regs 		);
		FreeRegSpace( &h->sramRegs 	);
		FreeRegSpace( &h->iack 		);
		FreeRegSpace( &h->bounce 	);

		free_irq( chu->pdev->irq, h 	);
		pci_disable_msi(chu->pdev);
	}

	return 0;

}

int vme4l_register_client( VME4L_BRIDGE_HANDLE *h )
{
	PLDZ002_LOCK_STATE();
	++h->refCounter;
	PLDZ002_UNLOCK_STATE();

	return 0;
}
EXPORT_SYMBOL_GPL(vme4l_register_client);

int vme4l_unregister_client( VME4L_BRIDGE_HANDLE *h )
{
	PLDZ002_LOCK_STATE();
	if (h->refCounter <= 0)
		return -EINVAL;

	--h->refCounter;
	PLDZ002_UNLOCK_STATE();

	return 0;
}
EXPORT_SYMBOL_GPL(vme4l_unregister_client);

module_init(vme4l_pldz002_init_module);
module_exit(vme4l_pldz002_cleanup_module);

MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
MODULE_DESCRIPTION("VME4L - MEN VME PLDZ002 bridge driver");
MODULE_LICENSE("GPL");


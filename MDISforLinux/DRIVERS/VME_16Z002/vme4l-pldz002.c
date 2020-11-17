/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l-menpldz002.c
 *
 *      \author  klaus.popp@men.de
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
/*
 *---------------------------------------------------------------------------
 * Copyright 2004-2020, MEN Mikro Elektronik GmbH
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
#include <MEN/men_typs.h>
#include <MEN/pldz002.h>

#define VERSION_CODE_NEW_IRQFLAGS 0 /* evaluates to 0 by compiler if not defined */

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

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/** DMA bounce buffer uses last 256K of bridge SRAM */
#define BOUNCE_SRAM_ADDR	0xc0000
#define BOUNCE_SRAM_SIZE	0x40000

/* Macros to lock accesses to bridge driver handle and VME bridge regs */
#define PLDZ002_LOCK_STATE() 			spin_lock(&h->lockState)
#define PLDZ002_UNLOCK_STATE() 			spin_unlock(&h->lockState)
#define PLDZ002_LOCK_STATE_IRQ(ps) 		spin_lock_irqsave(&h->lockState, ps)
#define PLDZ002_UNLOCK_STATE_IRQ(ps) 	spin_unlock_irqrestore(&h->lockState,\
                                                               ps)

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

/** interrupter dummy ID */
#define _PLDZ002_INTERRUPTER_ID	1

/* definition of featuresets (see 13Z014-90 Spec) */
#define _PLDZ002_FS2(h)			(h->isPldZ002)
#define _PLDZ002_FS3(h) 		(h->isPldZ002 && (h->pciRevision >= 17))

#define _PLDZ002_USE_BOUNCE_DMA(h) 	(_PLDZ002_FS2(h) && !_PLDZ002_FS3(h))
#define _PLDZ002_USE_BM_DMA(h) 	   	(_PLDZ002_FS3(h))
#define _PLDZ002_USE_BM_SHMEM(h) 	(_PLDZ002_FS3(h))

#define _PLDZ002_SWAP32(dword)	( ((dword)>>24) | ((dword)<<24) | \
							  (((dword)>>8) & 0x0000ff00)   | \
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

/** bridge drivers private data */
typedef struct {
	struct pci_dev *pdev;		/**< PCI device struct  */
	int  isPldZ002;				/**< false if it's an A12/B11 bridge */
	unsigned long  bar[6];			/**< copy of PCI bars (bus adrs) */
	VME4L_RESRC regs;			/**< bridge regs [+ PLD internal RAM if any] */
	VME4L_RESRC	sramRegs;		/**< PLDZ002>=Rev17 registers in SRAM  */
	VME4L_RESRC iack;			/**< IACK space */
	VME4L_RESRC bounce;			/**< part of SRAM for DMA bouncing  */

	/* the following two are not ioremapped */
	VME4L_RESRC	sram;			/**< SRAM as slave window  */
	VME4L_RESRC	bmShmem;		/**< bus master slave window  */
	int  a32LongAddUsed;		/**< A32 LONGADD reg in use count  */
	uint8_t pciRevision;		/**< PCI revision from cfg space  */
	uint8_t mstrShadow;			/**< MSTR register shadow reg  */
	uint8_t haveBerr;			/**< bus error recorded  */

	spinlock_t lockState;		/**< spin lock for VME bridge registers
								   and handle state */
} VME4L_BRIDGE_HANDLE;

#define COMPILE_VME_BRIDGE_DRIVER

#include "vme4l-core.h"

static int debug = DEBUG_DEFAULT;  /**< enable debug printouts */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static VME4L_BRIDGE_HANDLE G_bHandle;


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/


/***********************************************************************/
/** Get system IRQ no.
 */
unsigned int GetSysIrq( VME4L_BRIDGE_HANDLE *h )
{
	unsigned int irq;

	irq = h->pdev->irq;

	if (!irq)
		printk(KERN_ERR " *** vme-pldz002: error invalid system IRQ!\n");
	return irq;

}

/***********************************************************************/
/** Get bridge driver info string
 */
void RevisionInfo( VME4L_BRIDGE_HANDLE *h, char *buf )
{
	sprintf(buf,
			"%s VME bridge (rev %d ), vme4l-pldz002",
			h->isPldZ002 ? "PLDZ002" : "A12/B11",
			h->pciRevision );
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
#if defined(__powerpc__)
	int isPowerPc = 1;
#else
	int isPowerPc = 0;
#endif
	uint32_t barA24A16, barA32D32, bar4Off;


	*bDrvDataP = NULL;			/* don't need it for now */

	if( isPowerPc || (flags & VME4L_AW_HW_SWAP1) ){
		barA24A16 = 0;
		barA32D32 = 1;
		bar4Off = PLDZ002_A24D32SW_SPACE;
	}
	else {
		barA24A16 = 2;
		barA32D32 = 3;
		bar4Off = PLDZ002_A24D32_SPACE;
	}

	PLDZ002_LOCK_STATE();

	switch( spc ){
	case VME4L_SPC_A16_D16:
	  physAddr = (void*)(h->bar[barA24A16] + PLDZ002_A16D16_SPACE);
		vmeAddr  = 0;
		size 	 = PLDZ002_A16Dxx_SIZE;
		break;

	case VME4L_SPC_A16_D32:
		if( h->isPldZ002 ){
			physAddr 	= (void *)(h->bar[barA24A16] + PLDZ002_A16D32_SPACE);
			vmeAddr 	= 0;
			size 		= PLDZ002_A16Dxx_SIZE;
		}
		break;

	case VME4L_SPC_A24_D16:
		physAddr = (void *)(h->bar[barA24A16] + PLDZ002_A24D16_SPACE);
		vmeAddr  = 0;
		size	 = PLDZ002_A24Dxx_SIZE;
		break;

	case VME4L_SPC_A24_D32:
		if( h->isPldZ002 ){
			physAddr 	= (void *)(h->bar[4] + bar4Off);
			vmeAddr 	= 0;
			size 		= PLDZ002_A24Dxx_SIZE;
		}
		break;

	case VME4L_SPC_A32_D32:
		if( h->isPldZ002 ){
			
			if( h->a32LongAddUsed ){	/* already a A32 window setup */
				rv = -EBUSY;
				break;
			}

			/* check for 512MB crossing */
			vmeAddr = *vmeAddrP & (PLDZ002_A32D32_SIZE-1);
			if( vmeAddr + *sizeP > PLDZ002_A32D32_SIZE)
				break;

			physAddr 	= (void *)(h->bar[barA32D32] +
								   PLDZ002_A32D32_SPACE);
			vmeAddr 	= *vmeAddrP & ~(PLDZ002_A32D32_SIZE-1);
			size 		= PLDZ002_A32D32_SIZE;
			h->a32LongAddUsed++;

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
		if( _PLDZ002_FS2(h) ){
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
        VME4LDBG("*** PLDZ002 bus error at vaddr=%p\n", vaddr );\
		rv = -EIO;\
		VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow | PLDZ002_MSTR_BERR );\
        VME_REG_READ8( PLDZ002_MSTR ); /* dummy read to complete access */\
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
    \
    /* clear bus error and disable posted writes */\
	VME_REG_WRITE8( PLDZ002_MSTR, (h->mstrShadow | PLDZ002_MSTR_BERR)\
                    & ~PLDZ002_MSTR_POSTWR);\
	VME_WIN_WRITE##size(vaddr,*dataP);\
	\
	if( VME_REG_READ8( PLDZ002_MSTR ) & PLDZ002_MSTR_BERR ){\
        VME4LDBG("*** PLDZ002 bus error at vaddr=%p\n", vaddr );\
		rv = -EIO;\
        /* clear bus error */\
	    VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow | PLDZ002_MSTR_BERR );\
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
	int swapMode,
	vmeaddr_t *vmeAddr,
	int flags)
{
	int alignVme=4, sg, rv=0, endBd;
	uint32_t bdAm;
	char *bdVaddr;

	/* DMA controller supports only BLT spaces */
	switch( spc ){
	case VME4L_SPC_A24_D16_BLT:
		bdAm = PLDZ002_DMABD_AM_A24D16;
		break;
	case VME4L_SPC_A24_D32_BLT:
		bdAm = PLDZ002_DMABD_AM_A24D32;
		break;
	case VME4L_SPC_A32_D32_BLT:
		bdAm = PLDZ002_DMABD_AM_A32D32;
		break;
	case VME4L_SPC_A32_D64_BLT:
		bdAm = PLDZ002_DMABD_AM_A32D64;
		alignVme = 8;
		break;
	default:
		return -EINVAL;
	}

	bdVaddr = _PLDZ002_FS3(h) ?
		(char *)h->sramRegs.vaddr + PLDZ002_DMABD_OFF_RV11(0) :
		(char *)h->regs.vaddr + PLDZ002_DMABD_OFF_RV9(0);
	VME4LDBG("DmaSetup: bdVaddr=%p\n", bdVaddr );

	endBd = (sgNelems < PLDZ002_DMA_MAX_BDS) ? sgNelems : PLDZ002_DMA_MAX_BDS;

	/* setup scatter list */
	for( sg=0; sg<endBd; sg++, sgList++, bdVaddr+=PLDZ002_DMABD_SIZE ){

		/*--- check alignment/size ---*/
		if( (*vmeAddr & (alignVme-1)) || (sgList->dmaAddress & 3) ||
			(sgList->dmaLength > 256*1024) || (sgList->dmaLength & 3)){
			VME4LDBG( "*** pldz002 DMA setup bad alignment/len "
					  "%08llx %08llx %x\n", *vmeAddr,
					  (uint64_t) sgList->dmaAddress, sgList->dmaLength );
			rv = -EINVAL;
			goto CLEANUP;
		}

		if( direction ){
			/* write to VME */
			VME_GENREG_WRITE32( bdVaddr+0x0, *vmeAddr );
			VME_GENREG_WRITE32( bdVaddr+0x4, sgList->dmaAddress );
			VME_GENREG_WRITE32( bdVaddr+0x8, sgList->dmaLength>>2 );
			VME_GENREG_WRITE32( bdVaddr+0xc,
							 PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_PCI ) |
							 PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_VME ) |
							 bdAm | DmaSwapMode(swapMode) |
							 ((sg == endBd-1) ? PLDZ002_DMABD_END : 0 ));
		}
		else {
			/* read from VME */
			VME_GENREG_WRITE32( bdVaddr+0x0, sgList->dmaAddress );
			VME_GENREG_WRITE32( bdVaddr+0x4, *vmeAddr );
			VME_GENREG_WRITE32( bdVaddr+0x8, sgList->dmaLength>>2 );
			VME_GENREG_WRITE32( bdVaddr+0xc,
							 PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_VME ) |
							 PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_PCI ) |
							 bdAm | DmaSwapMode(swapMode) |
							 ((sg == endBd-1) ? PLDZ002_DMABD_END : 0 ));
		}									
		*vmeAddr += sgList->dmaLength;
	}
#ifdef DBG
    {
		int i;
		uint32_t *p;
		bdVaddr = _PLDZ002_FS3(h) ?
			(char *)h->sramRegs.vaddr + PLDZ002_DMABD_OFF_RV11(0) :
			(char *)h->regs.vaddr + PLDZ002_DMABD_OFF_RV9(0);

		p = (uint32_t *)bdVaddr;

		for(i=0; i<16; i++ ){
			VME4LDBG("DMA BD %d %p: %08x %08x %08x %08x\n",
					 i, p, p[0], p[1], p[2], p[3] );
			p+=4;
		}
	}
#endif
 CLEANUP:
	
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
	int swapMode,
	vmeaddr_t vmeAddr,
	void **bounceBufP)
{
	int alignVme=4, rv=0;
	uint32_t bdAm, bdOff;

	/* DMA controller supports only BLT spaces */
	switch( spc ){
	case VME4L_SPC_A24_D16_BLT:
		bdAm = PLDZ002_DMABD_AM_A24D16;
		break;
	case VME4L_SPC_A24_D32_BLT:
		bdAm = PLDZ002_DMABD_AM_A24D32;
		break;
	case VME4L_SPC_A32_D32_BLT:
		bdAm = PLDZ002_DMABD_AM_A32D32;
		break;
	case VME4L_SPC_A32_D64_BLT:
		bdAm = PLDZ002_DMABD_AM_A32D64;
		alignVme = 8;
		break;
	default:
		return -EINVAL;
	}

	bdOff = PLDZ002_DMABD_OFF_RV9(0);

	if( size > BOUNCE_SRAM_SIZE )
		size = BOUNCE_SRAM_SIZE;

	/*--- check alignment/size ---*/
	if( (vmeAddr & (alignVme-1)) || (size & 3) ){
		VME4LDBG("*** pldz002 Bounce DMA setup bad alignment/len "
				 "0x%llx 0x%llx\n",
				 vmeAddr, (uint64_t) size);
		rv = -EINVAL;
		goto CLEANUP;
	}

	if( direction ){
		/* write to VME */
		VME_REG_WRITE32( bdOff+0x0, vmeAddr );
		VME_REG_WRITE32( bdOff+0x4, BOUNCE_SRAM_ADDR );
		VME_REG_WRITE32( bdOff+0x8, size>>2 );
		VME_REG_WRITE32( bdOff+0xc,
						 PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_SRAM ) |
						 PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_VME ) |
						 bdAm | DmaSwapMode(swapMode) | PLDZ002_DMABD_END);
	}
	else {
		/* read from VME */
		VME_REG_WRITE32( bdOff+0x0, BOUNCE_SRAM_ADDR );
		VME_REG_WRITE32( bdOff+0x4, vmeAddr );
		VME_REG_WRITE32( bdOff+0x8, size>>2 );
		VME_REG_WRITE32( bdOff+0xc,
						 PLDZ002_DMABD_SRC( PLDZ002_DMABD_DIR_VME ) |
						 PLDZ002_DMABD_DST( PLDZ002_DMABD_DIR_SRAM ) |
						 bdAm | DmaSwapMode(swapMode) | PLDZ002_DMABD_END );
	}									
#ifdef DBG
    {
		uint32_t *p = (uint32_t *)((char *)h->regs.vaddr +
								   PLDZ002_DMABD_OFF_RV9(0));
		VME4LDBG("DMA BD %p: %08x %08x %08x %08x\n",
				 p, p[0], p[1], p[2], p[3] );
	}
#endif
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
	if( VME_REG_READ8( PLDZ002_DMASTA ) & PLDZ002_DMASTA_EN ){
		VME4LDBG("*** pldz002: dmaStart: DMA busy! %02x\n",
				 VME_REG_READ8( PLDZ002_DMASTA ));
		return -EBUSY;
	}
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
	
	if( !h->isPldZ002 ){
		/* on A12, we must wait and manually clear SYSRES */
		udelay(2000);
		PLDZ002_LOCK_STATE();
		VME_REG_CLRMASK8( PLDZ002_SYSCTL, PLDZ002_SYSCTL_SYSRES);
		PLDZ002_UNLOCK_STATE();	
	}
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
 * PLDZ002 doesn't yet have a buserror space/address register
 */
static int BusErrGet(
	VME4L_BRIDGE_HANDLE *h,
	int *spaceP,
	vmeaddr_t *addrP,
	int clear )
{
	unsigned long ps;
	int rv;

	PLDZ002_LOCK_STATE_IRQ(ps);
	rv = h->haveBerr;
	if( clear )
		h->haveBerr = 0;
	PLDZ002_UNLOCK_STATE_IRQ(ps);

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
        VME4LDBG("*** PLDZ002 RMW bus error at vaddr=%p\n", vaddr );
		rv = -EIO;
	}
	/* restore org. mode */
	VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow | PLDZ002_MSTR_BERR );
	
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
/** Setup VME slave window (for A12/B11)
 */
static int SlaveWindowCtrlFs1(
	VME4L_BRIDGE_HANDLE *h,
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	void **physAddrP,
	void **bDrvDataP)
{

	VME4LDBG("pldz002::SlaveWindowCtrlFs1 vmeAddr= %llx size=%llx\n",
			 vmeAddr, (uint64_t) size );

	if( (spc != VME4L_SPC_SLV1) )
		return -ENOTTY;

	if( size ){
		/* enable slave access */
		if( (size != 0x100000) || (vmeAddr & (0x100000-1)) ){
			VME4LDBG("*** SlaveWindowCtrlFs1 Addr 0x%llx size %llx invalid!\n",
					 vmeAddr, (uint64_t) size );
			return -EINVAL;
		}
		
		if( vmeAddr >= 0x1000000L ){
			VME4LDBG( "*** SlaveWindowCtrlFs1: vmeAddr > 16M! \n" );			
			return -EINVAL;
		}

		VME_REG_WRITE8( PLDZ002_SLV24,
						((vmeAddr >> 20) & 0xf) | PLDZ002_SLVxx_EN );
		*physAddrP = (void *)h->sram.phys;
	}
	else {
		/* disable slave access */
		VME_REG_WRITE8( PLDZ002_SLV24, 0 );
	}

	return 0;
}

/***********************************************************************/
/** Setup VME slave window (for PLDZ002 < rev 10)
 */
static int SlaveWindowCtrlFs2(
	VME4L_BRIDGE_HANDLE *h,
	VME4L_SPACE spc,
	vmeaddr_t vmeAddr,
	size_t size,
	void **physAddrP,
	void **bDrvDataP)
{
	const u32 sizeTbl[] = { 0x1000, 0x100000, 0x10000000 };

	if( (spc < VME4L_SPC_SLV0) || (spc > VME4L_SPC_SLV2 ) )
		return -ENOTTY;

	if( size ){
		u32 winSize = sizeTbl[spc-VME4L_SPC_SLV0];

		/* enable slave access */
		if( (size != winSize) ||
			(vmeAddr & (winSize-1)) )
			return -EINVAL;
		

		switch( spc ){
		case VME4L_SPC_SLV0:
			if( vmeAddr >= 0x10000 )
				return -EINVAL;

			VME_REG_WRITE8( PLDZ002_SLV16,
							((vmeAddr >> 12) & 0xf) | PLDZ002_SLVxx_EN );
			*physAddrP = (void *)h->regs.phys;
			break;

		case VME4L_SPC_SLV1:
			if( vmeAddr >= 0x1000000L )
				return -EINVAL;

			VME_REG_WRITE8( PLDZ002_SLV24,
							((vmeAddr >> 20) & 0xf) | PLDZ002_SLVxx_EN );
			*physAddrP = (void *)h->sram.phys;
			break;

		case VME4L_SPC_SLV2:
			if( vmeAddr >= 0x100000000LL )
				return -EINVAL;

			VME_REG_WRITE8( PLDZ002_SLV32,
							((vmeAddr >> 28) & 0xf) | PLDZ002_SLVxx_EN );
			*physAddrP = (void *)h->sram.phys;
			break;
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
			VME_REG_WRITE8( PLDZ002_SLV24, 0 );
			break;
		case VME4L_SPC_SLV2:
			VME_REG_WRITE8( PLDZ002_SLV32, 0 );
			break;
		default:
			break;
		}
	}

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

	const struct {
		u32 minSize;
		u32 maxSize;
		vmeaddr_t maxAddr;	
	} sizeTbl[] = {
		{   0x1000,     0x1000,     0x10000 },		/* ctrl 		*/
		{  0x10000,   0x100000,   0x1000000 },		/* a24 sram 	*/
		{ 0x100000,   0x100000, 0x100000000LL },	/* a32 sram 	*/
		{  0x10000,   0x100000,   0x1000000 },		/* a24 ram 		*/
		{ 0x100000, 0x10000000, 0x100000000LL },	/* a32 ram 		*/
	};

	VME4LDBG("pldz002::SlaveWindowCtrlFs3 vmeAddr=%llx size=%llx\n",
			 vmeAddr, (uint64_t) size);

	if( (spc < VME4L_SPC_SLV0) || (spc > VME4L_SPC_SLV4 ) )
		return -ENOTTY;

	if( size ){

		/* enable slave access */
		if( (size < sizeTbl[spc-VME4L_SPC_SLV0].minSize) ||
			(size > sizeTbl[spc-VME4L_SPC_SLV0].maxSize) ||
			(vmeAddr >= sizeTbl[spc-VME4L_SPC_SLV0].maxAddr) ||
			(vmeAddr & (size-1)) ) {
			
			VME4LDBG("*** SlaveWindowCtrlFs3: size=%llx out of range!\n",
					 (uint64_t) size);

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

				h->bmShmem.vaddr = pci_alloc_consistent(h->pdev,
														size,
														&dmaAddr);
				h->bmShmem.phys = dmaAddr;

				VME4LDBG("pldz002: pci_alloc_consistent: v=%p p=%x (%llx)\n",
						 h->bmShmem.vaddr, h->bmShmem.phys, (uint64_t) size );

				if( h->bmShmem.vaddr == NULL ){
					VME4LDBG("*** pldz002: can't alloc BM slave window "
							 "of 0x%llx bytes\n", (uint64_t) size );
					return -ENOSPC;
				}
				h->bmShmem.size = size;

				/* clear region */
				memset( h->bmShmem.vaddr, 0, size );

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
					void *rawbuf = h->bmShmem.vaddr;
					pend = virt_to_page(rawbuf + size - 1);

					for (page = virt_to_page(rawbuf); page <= pend; page++)
						SetPageReserved(page);
				}
#endif
				VME_REG_WRITE32( PLDZ002_PCI_OFFSET, h->bmShmem.phys );
			}
			else {
				if( h->bmShmem.size != size ){
					VME4LDBG("*** pldz002: won't change BM slave window if "
							 "used\n");
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

				VME4LDBG("pldz002: pci_free_consistent: v=%p p=%x (%llx)\n",
						 h->bmShmem.vaddr, h->bmShmem.phys, (uint64_t) size );

				pci_free_consistent(h->pdev,
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
int MboxReadFs2(
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
	val = VME_REG_READ32( PLDZ002_MAILBOX_RV9(mbox) );
	*valP = _PLDZ002_SWAP32( val );

	return 0;
}

/**********************************************************************/
/** Write mailbox value
 */
int MboxWriteFs2(
	VME4L_BRIDGE_HANDLE *h,
	int mbox,
	uint32_t val)
{
	if( mbox<0 || mbox>3 )
		return -EINVAL;

	/* see swapping note above */
	VME_REG_WRITE32( PLDZ002_MAILBOX_RV9(mbox), _PLDZ002_SWAP32(val));
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
	.dmaSetup			= DmaSetup,
//	.dmaSetup			= NULL, /* this function is optional */ 
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
	.postedWriteModeGet	= PostedWriteModeGet,
	.postedWriteModeSet	= PostedWriteModeSet,
	.rmwCycle			= RmwCycle,
	.aOnlyCycle			= NULL,
	.slaveWindowCtrl	= SlaveWindowCtrlFs1
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
 *
 */
static int PldZ002_CheckVmeBusError( VME4L_BRIDGE_HANDLE *h,
									 int *vecP, 	
									 int *levP )
{
	
	uint8_t mstr;

	if (NULL == h){
		printk(" *** %s: Bridge handle is NULL !\n", __FUNCTION__);
	}

	mstr = VME_REG_READ8( PLDZ002_MSTR );
	
	if( (mstr & (PLDZ002_MSTR_BERR | PLDZ002_MSTR_IBERREN )) ==
		(PLDZ002_MSTR_BERR | PLDZ002_MSTR_IBERREN ) ){
		VME4LDBG("*** PldZ002Irq bus error\n");
		/* bus error detected */
		h->haveBerr = 1;
		*levP 	= VME4L_IRQLEV_BUSERR;
		*vecP 	= VME4L_IRQVEC_BUSERR;
		/* clear bus error */
		VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow | PLDZ002_MSTR_BERR );
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
	}

	istat = VME_REG_READ8( PLDZ002_MSTR );	

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
			VME_REG_WRITE8( PLDZ002_MSTR_BERR,h->mstrShadow|PLDZ002_MSTR_BERR);
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
	
	if (NULL == h){
		printk(" *** %s: Bridge handle is NULL!\n", __FUNCTION__);
	}
	
	if( _PLDZ002_FS2(h) ){
		/* check for DMA finished */
		if( VME_REG_READ8( PLDZ002_DMASTA ) & PLDZ002_DMASTA_IRQ ){
			/* reset DMA irq, leave DMA_ERR untouched */
			VME_REG_WRITE8( PLDZ002_DMASTA, PLDZ002_DMASTA_IRQ );
			*levP = VME4L_IRQLEV_DMAFINISHED;
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
	/* struct pt_regs *regs = NULL; */
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) */

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

	VME4LDBG("PldZ002Irq: unhandled irq! vec=%d lev=%d\n", vector, level );
	goto EXIT;
	
 DONE:
	VME4LDBG("PldZ002Irq: vector=%d level=%d\n", vector, level );

	vme4l_irq( level, vector, NULL );


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
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
	if( check_mem_region( res->phys, res->size ))
		return -EBUSY;
#endif

	request_mem_region( res->phys, res->size, name );
	
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,5,0)
	res->vaddr = ioremap( res->phys, res->size );
#else
	res->vaddr = ioremap_nocache( res->phys, res->size );
#endif
#endif
	VME4LDBG("PLDZ002: MapRegSpace %s: vaddr=%p\n", name, res->vaddr );

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

	VME_REG_WRITE8( PLDZ002_INTR, 0x00 );
	VME_REG_WRITE8( PLDZ002_IMASK, 0x00 );
	VME_REG_WRITE8( PLDZ002_MSTR, PLDZ002_MSTR_BERR );
	VME_REG_WRITE8( PLDZ002_SLV24, 0x00 );

	VME_REG_WRITE8( PLDZ002_SYSCTL, VME_REG_READ8(PLDZ002_SYSCTL)
					& ~PLDZ002_SYSCTL_SYSRES);

	if( _PLDZ002_FS2(h) ){
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
	}
	if( _PLDZ002_FS3(h) ){
		VME_REG_WRITE16( PLDZ002_SLV24_PCI, 0 );
		VME_REG_WRITE32( PLDZ002_SLV32_PCI, 0 );
	}
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
static int DEVINIT pci_init_one (
	struct pci_dev *pdev,
	const struct pci_device_id *ent)
{
	int rv, n, irqReq=0;
	VME4L_BRIDGE_HANDLE *h = &G_bHandle;
	
	memset( h, 0, sizeof(*h));	/* clear handle */

	if( ent->subvendor == PLDZ002_SUBSYS_VEN_ID )
		h->isPldZ002 = 1;

	h->pdev = pdev;

	pci_read_config_byte( pdev, PCI_REVISION_ID, &h->pciRevision );

	printk( KERN_INFO "vme-pldz002: found %s bridge (rev %d), irq %d\n",
		   h->isPldZ002 ?
		   "PLDZ002" : "A12", h->pciRevision, pdev->irq);

	if( h->isPldZ002 ){
		if( h->pciRevision < 7 ){
			printk( KERN_ERR "vme-pldz002: The VME PLD revision %d is too old"
					". Expecting 7 or greater\n", h->pciRevision );
			rv = -ENODEV;
			goto CLEANUP;
		}
		if( ! _PLDZ002_FS3(h)  )
			printk( KERN_WARNING "vme-pldz002: The VME PLD revision %d is old"
					". Not all features of VME4L will be available\n",
					h->pciRevision );
	}

	/*--- read BARs ---*/
	for( n = 0; n < 6; n++ ){
		h->bar[n] = pci_resource_start(pdev, n);
		VME4LDBG("BAR%d: 0x%x\n", n, h->bar[n] );
	}
	
	h->regs.phys = h->bar[0]+PLDZ002_CTRL_SPACE;
	h->regs.size = PLDZ002_CTRL_SIZE;

	h->iack.phys = h->bar[0]+PLDZ002_IACK_SPACE;
	h->iack.size = PLDZ002_IACK_SIZE;

	/*--- request/map permanent spaces ---*/
	if( (rv = MapRegSpace( &h->regs, "pldz002-regs" )) )
		goto CLEANUP;

	if( (rv = MapRegSpace( &h->iack, "pldz002-iack" )) )
		goto CLEANUP;

	if( _PLDZ002_USE_BOUNCE_DMA(h) ){
		h->bounce.phys = h->bar[0] + PLDZ002_LRAM_SPACE + BOUNCE_SRAM_ADDR;
		h->bounce.size = BOUNCE_SRAM_SIZE;
		h->bounce.cache = _PLDZ002_WRITETHROUGH;

		if( (rv = MapRegSpace( &h->bounce, "pldz002-bounce" )) )
			goto CLEANUP;
	}

	if( _PLDZ002_FS3(h) ){
		h->sramRegs.phys = h->bar[0] + PLDZ002_LRAM_SPACE +
			PLDZ002_RV11_SRAMREGS;
		h->sramRegs.size = 0x800;
		if( (rv = MapRegSpace( &h->sramRegs, "pldz002-sramreg" )) )
			goto CLEANUP;
	}

	/* sram as slave window (don't ioremap) */
	h->sram.phys = h->bar[0] + PLDZ002_LRAM_SPACE;
	h->sram.size = PLDZ002_LRAM_SIZE;
	if( _PLDZ002_USE_BOUNCE_DMA(h) )
		h->sram.size -= BOUNCE_SRAM_SIZE;

	/*normal linux kernel mode: PldZ002Irq is a standard linux IRQ handler */
	if( (rv = request_irq( pdev->irq, PldZ002Irq,
#if LINUX_VERSION_CODE >= VERSION_CODE_NEW_IRQFLAGS
						   IRQF_SHARED,
#else
						   SA_SHIRQ,
#endif
						   "Z002_LX", h))<0 )
		goto CLEANUP;


	irqReq++;
	if ( (rv = pci_enable_device(pdev)) != 0)
		goto CLEANUP;

	/*--- init bridge ---*/
	InitBridge( h );

	/* allow bus error interrupts */
	h->mstrShadow = PLDZ002_MSTR_IBERREN;
	VME_REG_WRITE8( PLDZ002_MSTR, h->mstrShadow );
	
	spin_lock_init( &h->lockState );
	
	/*--- setup function pointers depending on feature level ---*/
	if( _PLDZ002_USE_BOUNCE_DMA(h) )
		G_bridgeDrv.dmaBounceSetup 	= DmaBounceSetup;

	if( _PLDZ002_USE_BM_DMA(h) ){
		G_bridgeDrv.dmaSetup 		= DmaSetup;
		pci_set_master( pdev );	/* enable bus mastering */
	}
		
	if( _PLDZ002_FS2(h) ){
		G_bridgeDrv.aOnlyCycle		= AOnlyCycle;
		G_bridgeDrv.slaveWindowCtrl	= SlaveWindowCtrlFs2;
		G_bridgeDrv.mboxRead		= MboxReadFs2;
		G_bridgeDrv.mboxWrite		= MboxWriteFs2;
		G_bridgeDrv.locMonRegRead	= LocMonRegReadFs2;
		G_bridgeDrv.locMonRegWrite	= LocMonRegWriteFs2;		
	}
	if( _PLDZ002_FS3(h) ){
		G_bridgeDrv.slaveWindowCtrl	= SlaveWindowCtrlFs3;
		G_bridgeDrv.mboxRead		= MboxReadFs3;
		G_bridgeDrv.mboxWrite		= MboxWriteFs3;
	}

	/*--- register to VME4L core ---*/
	rv = vme4l_register_bridge_driver( &G_bridgeDrv, h );
	if( rv != 0 )
		goto CLEANUP;

	return rv;


 CLEANUP:
	printk( KERN_ERR "vme-menpldz002: Init error %d\n", -rv );

	if( irqReq ){
		free_irq( pdev->irq, h );

	}

	FreeRegSpace( &h->regs 		);
	FreeRegSpace( &h->sramRegs 	);
	FreeRegSpace( &h->iack 		);
	FreeRegSpace( &h->bounce 	);
	
	return -ENODEV;
}	


static void DEVEXIT pci_remove_one (struct pci_dev *pdev)
{
	VME4L_BRIDGE_HANDLE *h = &G_bHandle;

	vme4l_unregister_bridge_driver();
	InitBridge( h );			/* clear regs to default state */

	free_irq( pdev->irq, h 	);

	FreeRegSpace( &h->regs 		);
	FreeRegSpace( &h->sramRegs 	);
	FreeRegSpace( &h->iack 		);
	FreeRegSpace( &h->bounce 	);		
}


/*
 * PCI Vendor/Device ID table.
 * Driver will handle all devices that have these codes
 */
static struct pci_device_id G_pci_tbl[] DEVINITDATA = {
	/* PLDZ002 */
	{ PLDZ002_VEN_ID, PLDZ002_DEV_ID, PLDZ002_SUBSYS_VEN_ID,
	  PLDZ002_SUBSYS_ID, },
	/* A12 PLD */
	{ PLDZ002_VEN_ID, PLDZ002_DEV_ID, PCI_ANY_ID, PCI_ANY_ID, },
	{ 0,}
};
MODULE_DEVICE_TABLE(pci, G_pci_tbl);

/* linux pci driver struct */
static struct pci_driver G_pci_driver = {
	name:		"vme4l-pldz002",
	id_table:	G_pci_tbl,
	probe:		pci_init_one,
	remove:		DEVEXIT_P(pci_remove_one),
};



static int __init vme4l_pldz002_init_module(void)
{

	int rv;
	printk( KERN_INFO "Initializing vme4l_pldz002...\n");

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)
	if( (rv =  pci_module_init( &G_pci_driver )) < 0 ) {
#else
	if( (rv =  pci_register_driver( &G_pci_driver )) < 0 ) {
#endif
		return rv;
	}

	return 0;
}

static void __exit vme4l_pldz002_cleanup_module(void)
{
	VME4L_BRIDGE_HANDLE *h = &G_bHandle;
	printk( KERN_INFO "vme4l_pldz002_cleanup_modul\n");
	InitBridge(h);
	pci_unregister_driver(&G_pci_driver);
}

module_init(vme4l_pldz002_init_module);
module_exit(vme4l_pldz002_cleanup_module);

MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
MODULE_DESCRIPTION("VME4L - MEN VME PLDZ002 bridge driver");
MODULE_LICENSE("GPL");
#ifdef MAK_REVISION
MODULE_VERSION(MENT_XSTR(MAK_REVISION));
#endif

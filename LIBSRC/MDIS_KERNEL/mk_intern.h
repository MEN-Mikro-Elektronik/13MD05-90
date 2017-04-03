/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mk_intern.h
 *
 *       Author: kp
 *        $Date: 2013/10/24 09:57:27 $
 *    $Revision: 1.20 $
 *
 *  Description: MDIS4LINUX kernel internal header file
 *
 *     Switches: MK_MAJOR - major number to use for /dev/mdis
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mk_intern.h,v $
 * Revision 1.20  2013/10/24 09:57:27  ts
 * R: gcc 4.6 under Ubuntu 12.10 warned about type mismatch of mk_dbglevel
 * M: changed int32 to int
 *
 * Revision 1.19  2013/08/29 16:13:27  ts
 * R: (D.Robinson) Locking occured in multithreaded application
 * M: Remove semaphore protection in mk_unl_ioctl
 *
 * Revision 1.18  2012/05/14 20:27:01  ts
 * R: Compiler warned about data type declaration of parameter mk_dbglevel
 * M: Changed data type to ulong instead u_int
 *
 * Revision 1.17  2012/04/23 12:32:55  ts
 * R: 1. warning about implicitely defined module functions in Ubuntu 11.04
 *    2. previous MDIS version used BKL, is now depecated and removed
 * M: 1. added include <linux/module.h>
 *    2. added semaphore to protect ioctls if HAVE_UNLOCKED_IOCTL defined
 *
 * Revision 1.16  2011/05/18 17:05:46  CRuff
 * R: 1. support of pci domains
 * M: 1. added pciDomainNbr to MK_DEV
 *
 * Revision 1.15  2009/08/04 16:52:53  CRuff
 * R: make 64bit compatible
 * M: change function signature to 64bit compatibility for
 *       - MDIS_LlGetStat
 *       - MDIS_BbGetStat
 *       - MDIS_MkGetStat
 *       (use INT32_OR_64* for valueP)
 *
 * Revision 1.14  2009/04/22 14:32:36  rt
 * R: 1.) Support for kernel version 2.6.19.
 * M: 1.) Fixed MDIS_IRQFUNC (regs was removed from irq handler at 2.6.19 and not 2.6.20).
 *
 * Revision 1.13  2007/12/10 12:58:59  ts
 * define VERSION_CODE_NEW_IRQFLAGS (from 2.6.20 on) added
 * central function Macro MDIS_IRQHANDLER defined instead numerous #ifdefs
 *
 * Revision 1.12  2007/01/08 12:24:56  ts
 * removed unnecessary include of config.h
 *
 * Revision 1.11  2006/08/04 11:01:21  ts
 * removed include <asm/segment.h>
 *
 * Revision 1.10  2005/01/19 11:12:24  ts
 * changed include order (sysdep.h/dbg.h) due to problem with RTAI
 *
 * Revision 1.9  2004/12/06 12:05:15  ts
 * modified for RTAI VME support
 *
 * Revision 1.8  2004/06/09 11:12:30  kp
 * - include men_vme_kernelif from MEN include dir (not kernel)
 * - Linux 2.6 support
 *
 * Revision 1.7  2003/04/11 16:26:20  kp
 * finalized RTAI port
 *
 * Revision 1.6  2003/02/21 13:34:39  kp
 * added support for RTAI and non-mdis drivers
 *
 * Revision 1.5  2003/02/21 10:11:58  kp
 * fixed problem when mapping PCI I/O spaces
 *
 * Revision 1.4  2002/05/31 15:23:01  kp
 * added possibility to set MK_MAJOR during compile time
 *
 * Revision 1.3  2002/05/06 11:52:49  kp
 * include slab.h rather malloc.h
 *
 * Revision 1.2  2001/08/16 10:15:50  kp
 * support VME devices, added COMPRESS_ERRNO macro
 *
 * Revision 1.1  2001/01/19 14:58:34  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
#include <linux/version.h> 
#include <linux/kernel.h> 	/* printk() 	*/
#include <linux/slab.h> 	/* kmalloc() 	*/
#include <linux/vmalloc.h> 	/* vmalloc() 	*/
#include <linux/fs.h>     	/* everything... */
#include <linux/errno.h>  	/* error codes 	*/
#include <linux/types.h>  	/* size_t 		*/
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/kmod.h>
#include <linux/interrupt.h>

#include <asm/fixmap.h>     /* fix_to_virt() */
#include <asm/uaccess.h>     /* copy_to/from_user */


#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/desc.h>
#include <MEN/maccess.h>
#include <MEN/ll_defs.h>
#include <MEN/ll_entry.h>
#include <MEN/bb_defs.h>
#include <MEN/bb_entry.h>
#include <MEN/bbis_bk.h>
#include <MEN/mdis_mk.h>
#include <MEN/mdis_err.h>
#include <MEN/mdis_com.h>
#include <MEN/mdis_api.h>
#include <MEN/mk_nonmdisif.h>

/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
#ifndef MK_MAJOR
# define MK_MAJOR 0				/* use dynamic major number */
#endif

#define DBG_MYLEVEL 		mk_dbglevel
#define DBH					G_dbh
#define OSH					G_osh

#define MK_MAX_DRVNAME		39		/* maximum length of LL driver name */
#define MK_MAX_DEVNAME		39		/* maximum length of device name */

/* macros to lock global MDIS sempahore */
#define MK_LOCK(err)	err=OSS_SemWait(OSH,G_mkLockSem,OSS_SEM_WAITFOREVER)
#define MK_UNLOCK 		OSS_SemSignal(OSH,G_mkLockSem)

#define MK_DRV_PREFIX	"men_ll_"

/* SPACE.flags */
#define MK_MAPPED		0x1
#define MK_REQUESTED	0x2

/*
 * COMPRESS_ERRNO macro invokes a special routine in case we're running
 * on PowerPC. On this machine, error numbers returned by system calls
 * cannot be higher than 515. So try to compress MDIS error numbers so that
 * they fit into this range.
 */
#if defined(PPC)
# define COMPRESS_ERRNO(x) CompressErrno(x)
#else
# define COMPRESS_ERRNO(x) (x)
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/* driver node structure */
typedef struct {
	OSS_DL_NODE node;			  	/* node in registered drivers list */
	char drvName[MK_MAX_DRVNAME+1]; /* driver name */
	void (*getEntry)(LL_ENTRY *); 	/* GetEntry function ptr */
	struct module	*module;		/* LL driver linux module structure */
} MK_DRV;

/* address space */
typedef struct {
	u_int32	addrMode;	/* address mode */
	u_int32	dataMode;	/* data mode */
	u_int32	reqSize;	/* required size (device) */
	u_int32	availSize;	/* available size (board) */
	void    *physAddr;	/* physical address */
	void    *virtAddr;	/* virtual address */
	u_int32 flags;		/* mapped/requested flags */
	u_int32 type;		/* OSS_ADDRSPACE_MEM or IO */
} SPACE;



/* MDIS device structure */
typedef struct {
	OSS_DL_NODE 	node;			/* node in registered devices list */
	char		    devName[MK_MAX_DEVNAME+1]; /* device name */
	char		    brdName[BK_MAX_DEVNAME+1]; /* board device name */
	u_int32		   	useCount;		/* number of opens */
	int 			persist;		/* don't remove device on last close */
	int			externalDev;	/* flags external, non-MDIS driver */

	OSS_HANDLE		*osh;			/* device's OSS handle */

	u_int32			lockMode;		/* device locking mode  */
	OSS_SEM_HANDLE *semDev;			/* device semaphore */
	OSS_SEM_HANDLE *semRead;		/* read semaphore (call locking) */
	OSS_SEM_HANDLE *semWrite;		/* write semaphore (call locking) */
	OSS_SEM_HANDLE *semBlkRead;		/* blockread semaphore (call locking) */
	OSS_SEM_HANDLE *semBlkWrite;	/* blockwrite semaphore (call locking) */
	OSS_SEM_HANDLE *semSetStat;		/* setstat semaphore (call locking) */
	OSS_SEM_HANDLE *semGetStat;		/* getstat semaphore (call locking) */
	OSS_SEM_HANDLE **semChanP;		/* chan semaphore array (chan locking) */
	u_int32			semChanAlloc;	/* size allocated for semChanP */
	/* device params */
	u_int32			devSlot;		/* device slot number on board */
	u_int32			subDevOffset; 	/* subdevice address offset 0 */
	u_int32			devNrChan;		/* number of channels */
	u_int32			devAddrMode;	/* device address mode */
	u_int32			devDataMode;	/* device data mode */
	/* board params */
	u_int32			busType;		/* board bus type */
	u_int32			devBusType;		/* device bus type */
	u_int32			irqInfo;		/* irq info */
	u_int32			addrSpaceType;  /* OSS_ADDRSPACE_MEM/IO */
	/* addr spaces */
	u_int32			spaceCnt;		/* nr of address spaces */
	SPACE			space[MDIS_MAX_MSPACE];	/* addr space params */
	MACCESS			ma[MDIS_MAX_MSPACE];	/* addr space access handles */
	/* device interrupt */
	u_int32			irqUse;			/* irq required from device */
	u_int32			irqInstalled;	/* irq installed */
	u_int32 		irqEnableKey;	/* value of IRQ_ENABLE key in DESC */
	u_int32			irqEnable;		/* irq enable 	*/
	u_int32			irqVector;		/* irq vector (can be PIC or VME) */
	u_int32			irqLevel;		/* irq level (VME, =[1..7])	*/
	u_int32			irqMode;		/* irq mode flags */
	u_int32			irqCnt;			/* irq global counter */
	/* pci params */
	u_int32			pciDomainNbr;	/* PCI domain number of device */
	u_int32			pciBusNbr;		/* PCI bus number of device */
	u_int32			pciDevNbr;		/* PCI device number of device */
	u_int32			pciFuncNbr;		/* PCI function number of device */
	u_int32			pciVendorId; 	/* PCI parameters from desc */
	u_int32			pciDeviceId; 	
	u_int32			pciSubSysVendorId; 	
	u_int32			pciSubSysId; 	
	/* board handler */
	BBIS_ENTRY		brdJumpTbl;		/* board handler's jump table */
	BBIS_HANDLE		*brd;			/* board handler's handle */
	u_int32			irqSrvInitFunc;	/* board has irq init function */
	u_int32			irqSrvExitFunc;	/* board has irq init function */
	/* ll driver */
	MK_DRV			*drv;			/* driver structure */
	LL_ENTRY		llJumpTbl;		/* ll driver's jump table */
	LL_HANDLE		*ll;			/* ll driver's handle */
	int			initialized; 	/* flags device sucessfuly initialized */
	int			exceptionOccurred; /* number of exception interrupts */
} MK_DEV;

/* MK path structure (held in fip->private) */
typedef struct {
  	int32 chan;					/* current MDIS channel number */
	int32 ioMode;				/* current MDIS I/O mode */
	MK_DEV *dev;				/* device entry */
} MK_PATH;

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
extern DBG_HANDLE 		*G_dbh;
extern OSS_HANDLE		*G_osh;
extern int 			mk_dbglevel;
extern OSS_SEM_HANDLE 	*G_mkLockSem;
extern OSS_DL_LIST		G_drvList;
extern OSS_DL_LIST		G_devList;

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

/* mk_module.c */
MK_DRV *MDIS_FindDrvByName( const char *name );
MK_DEV *MDIS_FindDevByName( const char *name );


/*
 * define the IRQ handler depending on the kernel version (arg changes.. )
 * and maintain it as one kernel-independent Macro throughout the MDIS kernel.
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
irqreturn_t MDIS_IrqHandler( int irq, void *dev_id );
# define MDIS_IRQHANDLER(a,b,c) MDIS_IrqHandler(a,b)
# define FUNCTYPE irqreturn_t
#else
irqreturn_t MDIS_IrqHandler( int irq, void *dev_id, struct pt_regs *regs );
# define MDIS_IRQHANDLER(a,b,c) MDIS_IrqHandler(a,b,c)
# define FUNCTYPE irqreturn_t
#endif

#define MDIS_IRQFUNC MDIS_IrqHandler

int32 MDIS_DevLock( MK_PATH *mkPath, OSS_SEM_HANDLE *callSem );
void MDIS_DevUnLock( MK_PATH *mkPath, OSS_SEM_HANDLE *callSem );
int32 MDIS_LlGetStat(MK_PATH *mkPath, int32 code, INT32_OR_64 *valueP);
int32 MDIS_BbGetStat(MK_PATH *mkPath, int32 code, INT32_OR_64 *valueP);
int32 MDIS_LlSetStat(MK_PATH *mkPath, int32 code, void *value);
int32 MDIS_BbSetStat(MK_PATH *mkPath, int32 code, void *value);

/* open.c */
int32 MDIS_InitialOpen(
	char *devName,
	DESC_HANDLE *devDescHdl,
	DESC_SPEC *devDesc,
	char *brdName,
	DESC_SPEC *brdDesc,
	int persist,
	MK_DEV **devP);
int32 MDIS_InitLockMode(MK_DEV *dev);
int32 MDIS_EnableIrq(MK_DEV *dev, u_int32 enable);
int32 MDIS_InstallSysirq(MK_DEV *dev);
int32 MDIS_RemoveSysirq(MK_DEV *dev);
int32 MDIS_GetBoardParams(MK_DEV *dev);
int32 MDIS_GetPciBoardParams(MK_DEV *dev, DESC_HANDLE *devDescHdl);
int32 MDIS_SetMiface( MK_DEV *dev );
int32 MDIS_GetIrqParams(MK_DEV *dev);

/* close.c */
int32 MDIS_FinalClose( MK_DEV *dev );
void MDIS_TermLockMode( MK_DEV *dev );

/* ident.c */
char* MDIS_Ident( void );

/* mk_calls.c */
int32 MDIS_MkSetStat( MK_PATH *mkPath, u_int32 code, void *arg );
int32 MDIS_MkGetStat(MK_PATH *mkPath, int32 code, INT32_OR_64 *valueP);


#ifdef CONFIG_MEN_VME_KERNELIF
extern int vme_request_irq(	unsigned int vme_irq,
							void (*handler)(int, void *, struct pt_regs * ),
							unsigned long flags,
							const char *device,
							void *dev_id);
extern int vme_ilevel_control( int level, int enable );
extern void vme_free_irq(unsigned int vme_irq, void *dev_id);
#endif


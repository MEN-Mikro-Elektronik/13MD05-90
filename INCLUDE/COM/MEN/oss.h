/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  oss.h
 *
 *      \author  uf
 *        $Date: 2011/05/18 17:00:25 $
 *    $Revision: 1.54 $
 *
 *	   \project  MDIS4Linux
 *	     \brief  Operating System Services module interface
 *
 *     \switches (set in oss_os.h):
 *		- NO_SHARED_MEM	:				shared memory not supported
 *      - NO_CALLBACK	:				callback not supported
 *		- OSS_HAS_UNASSIGN_RESOURCES:	OSS_UnAssignResources available
 *      - MAC_USERSPACE:                compile OSS_library for user space
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss.h,v $
 * Revision 1.54  2011/05/18 17:00:25  CRuff
 * R: 1. support of pci domains
 * M: 1. added macros to merge pci bus and domain number, and to extract them
 *       from a merged bus/domain number
 *
 * Revision 1.53  2010/11/29 16:10:19  dpfeuffer
 * R: OSS_SpinLockXxx function introduction
 * M: OSS_SpinLockXxx declaration added
 *
 * Revision 1.52  2009/11/16 15:50:16  ufranke
 * R: OSS_SWAP32 compile error with MSVC
 * M: removed spaces behind \
 *
 * Revision 1.51  2009/11/13 14:00:48  ufranke
 * R: OSS_SWAP32 produces unexpected results if used with signed integers
 *    i.e. OSS_SWAP32( 1<<31 ) delivers 0xFFFFFF80 instead of 0x00000080
 * M: fixed OSS_SWAP32
 *
 * Revision 1.50  2009/10/08 10:58:25  ufranke
 * R: GCC warning: integer constant is too large for 'unsigned long' type
 * M: OSS_SWAP64 improved with LL
 *
 * Revision 1.49  2009/03/17 13:59:38  dpfeuffer
 * R: 1. no bustype define for chameleon devices
 *    2. compiler warnings wit VC2008 64bit compiler
 * M: 1. OSS_BUSTYPE_CHAM added
 *    2. debug prints with pointers changed to %p
 *
 * Revision 1.48  2009/02/12 10:54:55  ww
 * R: OS9 (68K) doesn't support u_int64
 * M: Added switch OS9000 && OS9 for u_int64
 *
 * Revision 1.47  2008/08/08 10:01:43  aw
 * R: OS9 doesn't support u_int64
 * M: Added switch OS9000 for u_int64
 *
 * Revision 1.46  2008/05/29 14:48:01  CKauntz
 * Fixed OSS_SWAP64 macro
 *
 * Revision 1.44  2008/05/27 16:44:22  CKauntz
 * Added OSS_Swap64 macro
 *
 * Revision 1.43  2008/05/20 10:25:17  dpfeuffer
 * OSS_MemGet(): prototype corrected according oss_specification.c
 *
 * Revision 1.42  2006/08/02 12:28:45  ts
 * Cosmetics
 *
 * Revision 1.41  2006/06/28 15:19:40  ts
 * + OSS_BUSTYPE_SMB
 *
 * Revision 1.39  2005/11/25 09:58:44  cschuster
 * changed OSS_VME_DM/OSS_VME_AM docu from single-line style to common style
 *
 * Revision 1.38  2005/11/24 16:19:00  dpfeuffer
 * added OSS_GetSmbHdl(), OSS_SetSmbHdl() prototype
 *
 * Revision 1.37  2005/07/14 10:40:05  cs
 * changed defines OSS_VME4L_* to OSS_VME_*
 *
 * Revision 1.36  2005/07/08 11:32:46  cs
 * added support for userspace
 * added some bit mappings for PCI config header
 * changed header to doxygen
 *
 * Revision 1.35  2005/06/23 16:03:01  kp
 * Copyright line changed
 *
 * Revision 1.34  2004/12/20 08:36:11  kp
 * added OSS_DL_InsertAfter prototype
 *
 * Revision 1.33  2004/05/26 10:54:12  dpfeuffer
 * - OSS_VME_DM/AM defines added
 * - OSS_MapVmeAddr(), OSS_UnMapVmeAddr() prototype added
 *
 * Revision 1.32  2004/04/29 15:55:53  cs
 * bugfix in prototype for OSS_MemFill, didn't match exactly with definition
 *
 * Revision 1.31  2003/05/09 12:27:17  kp
 * added protos for OSS_AlarmMask()/OSS_AlarmRestore
 *
 * Revision 1.30  2003/05/02 12:50:08  dschmidt
 * OSS_PNP_SUPPORT related functions removed
 *
 * Revision 1.29  2003/04/30 17:15:58  dschmidt
 * PnP support added
 * OSS_BUSTYPE_UNKNOWN added
 * OSS_PCI_INVALID added
 *
 * Revision 1.28  2003/04/11 16:13:00  kp
 * added protos for OSS_IrqMaskR/OSS_IrqRestore
 *
 * Revision 1.27  2003/02/21 11:24:56  kp
 * mini change in declaration of OSS_DL_LIST
 *
 * Revision 1.26  2001/01/19 14:38:59  kp
 * added OSS_UnAssignResources
 *
 * Revision 1.25  2000/04/20 15:08:13  gromann
 * Bug fixed in OSS_SWAP16
 *
 * Revision 1.24  2000/03/09 16:26:48  Schmidt
 * NO_SHARED_MEM/NO_CALLBACK switch added for MDIS without shared mem/callback
 *
 * Revision 1.23  1999/07/30 11:57:31  Schmidt
 * OSS_ADDRSPACE_PCICFG added
 *
 * Revision 1.22  1999/07/26 15:56:39  Schmidt
 * OSS_BUSTYPE_MMODULE added
 * function prototype OSS_PciSetConfig added
 *
 * Revision 1.21  1999/06/08 10:47:19  kp
 * again added typedef OSS_CALLBACK_SET and prototypes for
 * OSS_CallbackSet/Clear
 *
 * Revision 1.20  1999/05/06 13:44:01  Schmidt
 * OSS_UnMapVirtAddr : parameter addrSpace added
 *
 * Revision 1.19  1999/04/23 13:31:05  kp
 * added functions and prototypes for double linked lists
 *
 * Revision 1.18  1999/04/22 10:20:55  Schmidt
 * swap macros OSS_SWAP16 and OSS_SWAP32 added
 * prototypes for OSS_Swap16 and OSS_Swap32 added
 *
 * Revision 1.17  1999/04/21 17:08:30  kp
 * removed OSS_CALLBACK_xxx structures (now OS-specific)
 * removed sigCode to OSS_CallbackCreate()
 * removed OSS_CallbackInfo/Set/Clear
 * added OSS_CallbackGetStat/SetStat
 *
 * Revision 1.16  1999/03/30 09:03:36  Schmidt
 * OSS_IrqLevelToVector function added
 * OSS_MapPhysToVirtAddr : parameter 'addrSpace' added
 * OSS_ADDRSPACE_xxx defines added
 * OSS_RES_xxx added
 *
 * Revision 1.15  1999/03/23 16:58:12  Schmidt
 * OSS_ISA_MEMORY_ADDR renamed to OSS_ISA_MEM_ADDR
 * OSS_ISA_PORT_ADDR renamed to OSS_ISA_IO_ADDR
 *
 * Revision 1.14  1999/03/23 09:58:48  Schmidt
 * OSS_BUSTYPE_ISAPNP added
 *
 * Revision 1.13  1999/02/26 11:48:22  see
 * prototype for OSS_GetPid added
 *
 * Revision 1.12  1999/02/15 13:57:42  see
 * missing OSS_CallbackGet prototype added
 * OSS_SharedMemSet/Link prototypes changed
 * OSS_SHMEM_CLEAR: linkCnt added
 *
 * Revision 1.11  1999/01/29 14:20:34  Schmidt
 * OSS_IsaGetConfig prototype added
 * OSS_ISA_xxx defines added
 *
 * Revision 1.10  1998/09/18 11:47:51  see
 * prototypes for (*funct)() added
 * OSS_CallbackCreate: prototype changed
 * OSS_CALLBACK_INFO: maxCallback removed
 *
 * Revision 1.9  1998/08/25 16:20:10  see
 * OSS_CALLBACK_INFO: queue overflow counter added
 *
 * Revision 1.8  1998/08/20 13:45:30  see
 * callback prototypes added
 * old DBG macros (if _OLD_DBG) removed
 *
 * Revision 1.7  1998/06/16 09:43:01  see
 * OSS_AlarmSet prototype changed
 *
 * Revision 1.6  1998/05/29 16:34:06  see
 * added OSS_AlarmXXX prototypes
 *
 * Revision 1.5  1998/05/18 11:13:26  kp
 * added OSS_TickRateGet/OSS_TickGet
 * added OSS_StrCpy
 *
 * Revision 1.4  1998/05/14 16:04:34  kp
 * OSS_VME_AM_Axx defs renamed to OSS_VME_Axx
 * OSS_VME_Dxx defs added
 * illegal OSS_VME_AM_A08 removed
 *
 * Revision 1.3  1998/04/22 15:21:41  see
 * OSS_DBG_OFF def changed to DBG_OFF
 *
 * Revision 1.2  1998/04/17 15:40:56  see
 * OSS_DbgMemdump prototype removed
 * all DBG defs+macros are now in dbg.h
 * include dbg.h for compatibility (if not NO_OLD_DBG)
 * OSS_DbgLevelGet/Set prototypes changed
 * OSS_Sprintf, OSS_Vsprintf prototypes added
 *
 * Revision 1.1  1998/02/23 10:12:06  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _OSS_H_
#define _OSS_H_

#ifndef MAC_USERSPACE
#	include <MEN/oss_os.h>  /* os specific types and header */
#else
#	include <MEN/ossu_os.h>  /* os specific types and header for user space */
#endif /* MAC_USERSPACE */


#ifdef __cplusplus
   extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/* for easy casting function pointers (callback) */
typedef void (*OSS_FKT_VOIDP1)(void*);
typedef void (*OSS_FKT_VOIDP2)(void*, void*);

/* for OSS_AssignResources */
typedef struct {
    int32   type;           /* resource type */
        union {
            struct {
                void    *physAddr;  /* physical address */
                u_int32 size;       /* size of address space */
            }mem;

            struct {
                int32   level;      /* interrupt level */
                int32   vector;     /* interrupt vector */
            }irq;

            int32   spare[32];      /* size of union */
        } u;
} OSS_RESOURCES;

typedef struct {                /* OSS_CallbackSet: */
    u_int32    callNr;          /* callback function index (0..n) */
    void       (*funct)(        /* callback function pointer */
                   void *appArg,
                   void *drvArg);
    void       *appArg;         /* function argument from application */
} OSS_CALLBACK_SET;

/*--- SHMEM block status ---*/
typedef struct {				/* M_LL_BLK_SHMEM_SET: */
    u_int32    smNr;			/* IN:  shared memory area index (0..n) */
	u_int32    size;			/* IN:  shared memory size [bytes] */
	void       *drvAddr;		/* OUT: shared memory address (driver) */
} OSS_SHMEM_SET;

typedef struct {				/* M_LL_BLK_SHMEM_LINK: */
    u_int32    smNr;			/* IN:  shared memory area index (0..n) */
	u_int32    size;			/* OUT: shared memory size [bytes] */
	void       *drvAddr;		/* OUT: shared memory address (driver) */
} OSS_SHMEM_LINK;

typedef struct {				/* M_LL_BLK_SHMEM_CLEAR: */
    u_int32    smNr;			/* IN:  shared memory area index (0..n) */
    u_int32    linkCnt;			/* OUT: shared memory link counter */
} OSS_SHMEM_CLEAR;

typedef struct OSS_DL_NODE {	/* Double linked list node */
	struct OSS_DL_NODE *next;
	struct OSS_DL_NODE *prev;
} OSS_DL_NODE;

typedef struct OSS_DL_LIST {	/* Double linked list header */
	OSS_DL_NODE *head;
	OSS_DL_NODE *tail;
	OSS_DL_NODE *tailpred;
} OSS_DL_LIST;

#ifdef MAC_USERSPACE
/* VME interface */
/** address-window type */
typedef enum {
	OSS_VME_WinT_master = 0, /**< master */
    OSS_VME_WinT_slave = 1,	 /**< slave */
} OSS_VME_WINTYPE;

/** Structure that describes address-window list nodes */
typedef struct {
	OSS_DL_NODE		n;
	int32			fd;				/**< handle to opened device */
	OSS_VME_WINTYPE winType;		/**< address-window type */
	u_int64			vmeAddr;		/**< VMEbus addresses to map (from user) */
	u_int32			locAddr;		/**< local address (translated) (should be u_int64?) */
	u_int32			size;			/**< number of bytes to map (from user) */
	/* master exclusive usage */
	u_int32			addrMode;		/**< OSS_VME_AM_X address mode (from user) */
	u_int32			maxDataMode;	/**< OSS_VME_DM_max data mode (from user) */
	u_int32			flags;			/**< flags (from user) */
	u_int32			useNbr;			/**< number of usage */
	/* slave exclusive usage */
	u_int32			slvSpc;			/**< number of VME slave space */
	u_int32			slvSpcSize;		/**< slave space size */
	u_int32			mapNbr;			/**< number of mappings */
} OSS_VME_ADDRWIN_NODE;

#endif /* MAC_USERSPACE */

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/* semaphores */
#define OSS_SEM_BIN    0
#define OSS_SEM_COUNT  1
#define OSS_SEM_NOWAIT 0
#define OSS_SEM_WAITFOREVER  -1

/* for mem permission check */
#define OSS_READ     0
#define OSS_WRITE    1

/* for OSS_AssignResources */
#define OSS_RES_MEM      1
#define OSS_RES_IRQ      2
#define OSS_RES_IO       3

/* for OSS_BusToPhysAddr and BBIS BrdInfo */
#define OSS_BUSTYPE_NONE        0   /* device is local */
#define OSS_BUSTYPE_VME         1
#define OSS_BUSTYPE_PCI         2
#define OSS_BUSTYPE_ISA         3
#define OSS_BUSTYPE_ISAPNP      4
#define OSS_BUSTYPE_MMODULE     5
#define OSS_BUSTYPE_SMB		    6
#define OSS_BUSTYPE_CHAM		7
#define OSS_BUSTYPE_MSI			8
#define OSS_BUSTYPE_UNKNOWN   255

/* for OSS_MapPhysToVirtAddr */
#define OSS_ADDRSPACE_MEM         0
#define OSS_ADDRSPACE_IO          1
#define OSS_ADDRSPACE_PCICFG      2

/* VMEbus addr range */
#define OSS_VME_AXX          0x00ff	/* (mask) */
#define OSS_VME_A16          0x0001	/* short i/o */
#define OSS_VME_A24          0x0002	/* standard i/o */
#define OSS_VME_A32          0x0003	/* extended i/o */

/* VMEbus data width */
#define OSS_VME_DXX          0xff00	/* (mask) */
#define OSS_VME_D16          0x0100
#define OSS_VME_D24          0x0200
#define OSS_VME_D32          0x0300

/* OSS_MapVmeAddr - Data Modes */
#define OSS_VME_DM_8		0x01	/**<  8-bit data access */
#define OSS_VME_DM_16		0x02	/**< 16-bit data access */
#define OSS_VME_DM_32		0x04	/**< 32-bit data access */
#define OSS_VME_DM_64		0x08	/**< 64-bit data access */

/* OSS_MapVmeAddr - Address Modes (VMEbus address modifiers) */
/* VME_AM_<16:short/24:std/32:ext/64:long><U:user/S:superv><D:data/P:prgr/B:BLT/B64:D64-MBLT> */
#define OSS_VME_AM_16UD		0x29	/**< Short non-privileged access		 */
#define OSS_VME_AM_24UD		0x39	/**< Standard non-privileged data access */
#define OSS_VME_AM_24UB		0x3B	/**< Standard non-privileged BLT		 */
#define OSS_VME_AM_32UD		0x09	/**< Extended non-privileged data access */
#define OSS_VME_AM_32UB		0x0B	/**< Extended non-privileged BLT		 */
#define OSS_VME_AM_32UB64	0x08	/**< Extended non-privileged D64-MBLT	 */

/* for OSS_PciGetConfig */
#define OSS_PCI_INVALID	             0
#define OSS_PCI_VENDOR_ID            1
#define OSS_PCI_DEVICE_ID            2
#define OSS_PCI_COMMAND              3
#define OSS_PCI_STATUS               4
#define OSS_PCI_REVISION_ID          5
#define OSS_PCI_CLASS                6
#define OSS_PCI_SUB_CLASS            7
#define OSS_PCI_PROG_IF              8
#define OSS_PCI_CACHE_LINE_SIZE      9
#define OSS_PCI_PCI_LATENCY_TIMER   10
#define OSS_PCI_HEADER_TYPE         11     /* 0=device; 1=bridge */
#define OSS_PCI_BIST                12
#define OSS_PCI_ADDR_0              13
#define OSS_PCI_ADDR_1              14
#define OSS_PCI_ADDR_2              15
#define OSS_PCI_ADDR_3              16
#define OSS_PCI_ADDR_4              17
#define OSS_PCI_ADDR_5              18
#define OSS_PCI_CIS                 19
#define OSS_PCI_SUBSYS_VENDOR_ID    20
#define OSS_PCI_SUBSYS_ID           21
#define OSS_PCI_EXPROM_ADDR         22
#define OSS_PCI_INTERRUPT_PIN       23
#define OSS_PCI_INTERRUPT_LINE      24
#define OSS_PCI_ACCESS_8			0x01000000
#define OSS_PCI_ACCESS_16			0x02000000
#define OSS_PCI_ACCESS_32			0x04000000
#define OSS_PCI_ACCESS				0x07000000

/* Bit encodings for  PCI_COMMON_CONFIG.HeaderType */
#define OSS_PCI_HEADERTYPE_MULTIFUNCTION	0x80
#define OSS_PCI_HEADERTYPE_DEVICE_TYPE		0x00
#define OSS_PCI_HEADERTYPE_BRIDGE_TYPE		0x01

/* Bit encodings for PCI_COMMON_CONFIG.Command */
#define OSS_PCI_COMMAND_ENABLE_IO_SPACE					0x0001
#define OSS_PCI_COMMAND_ENABLE_MEM_SPACE				0x0002
#define OSS_PCI_COMMAND_ENABLE_BUS_MASTER				0x0004
#define OSS_PCI_COMMAND_ENABLE_SPECIAL_CYCLES			0x0008
#define OSS_PCI_COMMAND_ENABLE_WRITE_AND_INVALIDATE		0x0010
#define OSS_PCI_COMMAND_ENABLE_VGA_COMPATIBLE_PALETTE	0x0020
#define OSS_PCI_COMMAND_ENABLE_PARITY					0x0040  /* (ro+) */
#define OSS_PCI_COMMAND_ENABLE_WAIT_CYCLE				0x0080  /* (ro+) */
#define OSS_PCI_COMMAND_ENABLE_SERR						0x0100  /* (ro+) */
#define OSS_PCI_COMMAND_ENABLE_FAST_BACK_TO_BACK		0x0200  /* (ro)  */

/* Bit encodings for PCI_COMMON_CONFIG.Status */
#define OSS_PCI_STATUS_FAST_BACK_TO_BACK        0x0080  /* (ro) */
#define OSS_PCI_STATUS_DATA_PARITY_DETECTED     0x0100
#define OSS_PCI_STATUS_DEVSEL                   0x0600  /* 2 bits wide */
#define OSS_PCI_STATUS_SIGNALED_TARGET_ABORT    0x0800
#define OSS_PCI_STATUS_RECEIVED_TARGET_ABORT    0x1000
#define OSS_PCI_STATUS_RECEIVED_MASTER_ABORT    0x2000
#define OSS_PCI_STATUS_SIGNALED_SYSTEM_ERROR    0x4000
#define OSS_PCI_STATUS_DETECTED_PARITY_ERROR    0x8000

/* Bit encodings for PCI_COMMON_CONFIG.Status.Devsel bits */
#define PCI_STATUS_DEVSEL_SPEED_FAST               0x0000
#define PCI_STATUS_DEVSEL_SPEED_MEDIUM             0x0200
#define PCI_STATUS_DEVSEL_SPEED_SLOW               0x0400

/* for OSS_IsaGetConfig */
#define OSS_ISA_MEM_ADDR			0x10
#define OSS_ISA_IO_ADDR				0x20
#define OSS_ISA_IRQ_LEVEL			0x30

/* swap macros */
#define OSS_SWAP16(word)	( (((word)>>8) & 0xff) | (((word)<<8)&0xff00) )

#define OSS_SWAP32(dword)	( (((dword)>>24)& 0x000000ff)   | \
							  (((dword)<<24)& 0xff000000)   | \
							  (((dword)>>8) & 0x0000ff00)   | \
							  (((dword)<<8) & 0x00ff0000)     )

#define OSS_SWAP64(qword)  (  ((qword>>56) & 0x00000000000000ffLL) | \
							  ((qword<<56) & 0xff00000000000000LL) | \
							  ((qword>>40) & 0x000000000000ff00LL) | \
							  ((qword<<40) & 0x00ff000000000000LL) | \
							  ((qword>>24) & 0x0000000000ff0000LL) | \
							  ((qword<<24) & 0x0000ff0000000000LL) | \
							  ((qword>>8)  & 0x00000000ff000000LL) | \
							  ((qword<<8)  & 0x000000ff00000000LL)   )

/* merge the pci domain number into the pci bus number */
#define OSS_MERGE_BUS_DOMAIN(bus, domain) \
		(( bus & 0xff ) | ( ( domain & 0xff ) << 16 ))
	
#define OSS_BUS_NBR( merged_bus ) 		( merged_bus & 0xff )
#define OSS_DOMAIN_NBR( merged_bus )	( ( merged_bus >> 16 ) & 0xff )							


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
extern char* OSS_Ident( void );

extern void *OSS_MemGet(
    OSS_HANDLE *osHdl,
    u_int32    size,
    u_int32    *gotsizeP);

extern int32 OSS_MemFree(
    OSS_HANDLE *osHdl,
    void       *addr,
    u_int32    size);

extern int32 OSS_MemChk(
    OSS_HANDLE *osHdl,
    void       *addr,
    u_int32    size,
    int32      mode);

extern void OSS_MemCopy(
   OSS_HANDLE    *osHdl,
   u_int32       size,
   char          *src,
   char          *dest);

extern void OSS_MemFill(
    OSS_HANDLE *osHdl,
    u_int32    size,
    char       *adr,
    int8       value);

extern void OSS_StrCpy(
    OSS_HANDLE  *oss,
    char        *from,
	char        *to);

extern u_int32 OSS_StrLen(
    OSS_HANDLE *osHdl,
    char *string);

extern int32 OSS_StrCmp(
    OSS_HANDLE *osHdl,
    char *str1,
    char *str2);

extern int32 OSS_StrNcmp(
    OSS_HANDLE *osHdl,
    char    *str1,
    char    *str2,
    u_int32 nbrOfBytes);

extern char* OSS_StrTok(
    OSS_HANDLE *osHdl,
    char *string,
    char *separator,
    char **lastP);

#ifndef MAC_USERSPACE

#ifdef OSS_HAS_IRQMASKR
extern OSS_IRQ_STATE OSS_IrqMaskR(
	OSS_HANDLE *oss,
	OSS_IRQ_HANDLE* irqHandle );

extern void OSS_IrqRestore(
	OSS_HANDLE *oss,
	OSS_IRQ_HANDLE* irqHandle,
	OSS_IRQ_STATE oldState );
#endif /* OSS_HAS_IRQMASKR */

extern void OSS_IrqMask(
    OSS_HANDLE *osHdl,
    OSS_IRQ_HANDLE* irqHandle);

extern void OSS_IrqUnMask(
    OSS_HANDLE *osHdl,
    OSS_IRQ_HANDLE *irqHandle);

extern int32 OSS_IrqLevelToVector(
    OSS_HANDLE *osHdl,
    int32      busType,
	int32      level,
	int32      *vector);

extern int32 OSS_SpinLockCreate(
	OSS_HANDLE			*oss,
	OSS_SPINL_HANDLE	**spinlP );

extern int32 OSS_SpinLockRemove(
	OSS_HANDLE			*oss,
	OSS_SPINL_HANDLE	**spinlP );

extern int32 OSS_SpinLockAcquire(
	OSS_HANDLE			*oss,
	OSS_SPINL_HANDLE	*spinl );

extern int32 OSS_SpinLockRelease(
	OSS_HANDLE			*oss,
	OSS_SPINL_HANDLE	*spinl );

extern int32 OSS_SigCreate(
    OSS_HANDLE*       osHdl,
    int32            value,
    OSS_SIG_HANDLE** sigHandleP);

extern int32 OSS_SigSend(
    OSS_HANDLE *osHdl,
    OSS_SIG_HANDLE* sigHandle);

extern int32 OSS_SigRemove(
    OSS_HANDLE *osHdl,
    OSS_SIG_HANDLE** sigHandleP);

extern int32 OSS_SigInfo(
    OSS_HANDLE     *osHdl,
    OSS_SIG_HANDLE *sigHdl,
    int32          *signalNbrP,
    int32          *processIdP);

#endif /* MAC_USERSPACE */

extern int32 OSS_SemCreate(
    OSS_HANDLE *osHdl,
    int32          semType,
    int32          initVal,
    OSS_SEM_HANDLE** semHandleP);

extern int32 OSS_SemRemove(
    OSS_HANDLE *osHdl,
    OSS_SEM_HANDLE** semHandleP);

extern int32 OSS_SemWait(
    OSS_HANDLE *osHdl,
    OSS_SEM_HANDLE*   semHandle,
    int32           msec);

extern int32 OSS_SemSignal(
    OSS_HANDLE *osHdl,
    OSS_SEM_HANDLE* semHandle);

extern void OSS_DbgLevelSet(
    OSS_HANDLE *osHdl,
    u_int32 newLevel);

extern u_int32 OSS_DbgLevelGet(
    OSS_HANDLE *osHdl);

extern int32 OSS_Delay(
    OSS_HANDLE *osHdl,
    int32      msec);

extern int32 OSS_TickRateGet(
    OSS_HANDLE *osHdl);

extern u_int32 OSS_TickGet(
    OSS_HANDLE *osHdl);

extern u_int32 OSS_GetPid(
	OSS_HANDLE *osHdl);

extern int32 OSS_MikroDelayInit(
    OSS_HANDLE *osHdl);

extern int32 OSS_MikroDelay(
    OSS_HANDLE *osHdl,
    u_int32 mikroSec);

extern int32 OSS_BusToPhysAddr(
    OSS_HANDLE *osHdl,
    int32       busType,
    void       **physicalAddrP,
    ...);

#ifdef OSS_HAS_MAP_VME_ADDR
extern int32 OSS_MapVmeAddr(
	OSS_HANDLE	*osHdl,
	u_int64 	vmeBusAddr,
	u_int32		addrMode,
	u_int32		maxDataMode,
	u_int32		size,
	u_int32		flags,
	void		**locAddrP,
	void		**addrWinHdlP);
extern int32 OSS_UnMapVmeAddr(
	OSS_HANDLE	*osHdl,
	void		*addrWinHdl );
#endif /* OSS_HAS_MAP_VME_ADDR */

extern int32 OSS_PciGetConfig(
    OSS_HANDLE *osHdl,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32      *valueP);

extern int32 OSS_PciSetConfig(
    OSS_HANDLE *osHdl,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       value);

#ifndef MAC_USERSPACE

extern int32 OSS_PciSlotToPciDevice(
    OSS_HANDLE *osHdl,
    int32      busNbr,
    int32      mechSlot,
    int32      *pciDevNbrP);

extern int32 OSS_IsaGetConfig(
	OSS_HANDLE	*osHdl,
	int8		*cardVendId,
	int16		cardProdId,
	int8		*devVendId,
	int16		devProdId,
	int32       devNbr,
	int32       resType,
	int32       resNbr,
	u_int32		*valueP);

#endif /* MAC_USERSPACE */

extern int32 OSS_AssignResources(
    OSS_HANDLE      *osHdl,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
    OSS_RESOURCES   res[]);

#ifdef OSS_HAS_UNASSIGN_RESOURCES
extern int32 OSS_UnAssignResources(
    OSS_HANDLE      *osHdl,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
    OSS_RESOURCES   res[]);
#endif

extern int32 OSS_MapPhysToVirtAddr(
    OSS_HANDLE *osHdl,
    void       *physAddr,
    u_int32    size,
	int32      addrSpace,
    int32      busType,
    int32      busNbr,
    void       **virtAddrP);

extern int32 OSS_UnMapVirtAddr(
    OSS_HANDLE *osHdl,
    void       **virtAddrP,
    u_int32    size,
	int32      addrSpace);

extern int32 OSS_Sprintf(
    OSS_HANDLE  *oss,
    char *str,
    const char *fmt,
    ...);

extern int32 OSS_Vsprintf(
    OSS_HANDLE  *oss,
    char *str,
    const char *fmt,
    va_list arg);

#ifndef MAC_USERSPACE

/* alarm prototypes */
extern int32 OSS_AlarmCreate(
    OSS_HANDLE       *oss,
    void             (*funct)(void *arg),
	void             *arg,
    OSS_ALARM_HANDLE **alarmP);

extern int32 OSS_AlarmRemove(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE **alarmP);

extern int32 OSS_AlarmSet(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE *alarm,
    u_int32          msec,
    u_int32          cyclic,
    u_int32          *realMsec);

extern int32 OSS_AlarmClear(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE *alarm);

#ifdef OSS_HAS_ALARMMASK
OSS_ALARM_STATE OSS_AlarmMask( OSS_HANDLE *oss );
void OSS_AlarmRestore( OSS_HANDLE *oss, OSS_ALARM_STATE oldState );
#endif /* OSS_HAS_ALARMMASK */

#endif /* MAC_USERSPACE */

/* callback prototypes */
#ifndef NO_CALLBACK
extern int32 OSS_CallbackCreate(
    OSS_HANDLE           *oss,
    u_int32	             maxQueue,
    OSS_CALLBACK_HANDLE  **cbHdlP);

extern int32 OSS_CallbackRemove(
    OSS_HANDLE          *oss,
    OSS_CALLBACK_HANDLE **cbHdlP);

extern int32 OSS_CallbackExec(
    OSS_HANDLE          *oss,
    OSS_CALLBACK_HANDLE *cbHdl,
    u_int32             callNr,
    void                *drvArg);

extern int32 OSS_CallbackExec2(
    OSS_HANDLE          *oss,
    OSS_CALLBACK_HANDLE *cbHdl,
    u_int32             callNr,
    void                *appArg,
    void                *drvArg);

extern int32 OSS_CallbackSetStat(
    OSS_HANDLE          *oss,
    OSS_CALLBACK_HANDLE *cbHdl,
	OSS_CALLBACK_SETSTAT *arg);

extern int32 OSS_CallbackGetStat(
    OSS_HANDLE          *oss,
    OSS_CALLBACK_HANDLE *cbHdl,
	OSS_CALLBACK_GETSTAT *arg);

extern int32 OSS_CallbackSet(
    OSS_HANDLE          *oss,
    OSS_CALLBACK_HANDLE *cbHdl,
    OSS_CALLBACK_SET    *cbSet);

extern int32 OSS_CallbackClear(
    OSS_HANDLE          *oss,
    OSS_CALLBACK_HANDLE *cbHdl,
    u_int32             callNr);
#endif

/* shared memory prototypes */
#ifndef NO_SHARED_MEM
extern int32 OSS_SharedMemCreate(
    OSS_HANDLE       *oss,
    OSS_SHMEM_HANDLE **smHdlP);

extern int32 OSS_SharedMemRemove(
    OSS_HANDLE       *oss,
    OSS_SHMEM_HANDLE **smHdlP);

extern int32 OSS_SharedMemCheckNr(
    OSS_HANDLE       *oss,
    OSS_SHMEM_HANDLE *smHdl,
    OSS_SHMEM_SET    *smSet,
	u_int32          minNr,
	u_int32          maxNr,
	u_int32          *smNr);

extern int32 OSS_SharedMemCheckSize(
    OSS_HANDLE       *oss,
    OSS_SHMEM_HANDLE *smHdl,
    OSS_SHMEM_SET    *smSet,
	u_int32          minSize,
	u_int32          maxSize,
	u_int32          *smSize);

extern int32 OSS_SharedMemSet(
    OSS_HANDLE       *oss,
    OSS_SHMEM_HANDLE *smHdl,
    OSS_SHMEM_SET    *smSet);

extern int32 OSS_SharedMemLink(
    OSS_HANDLE       *oss,
    OSS_SHMEM_HANDLE *smHdl,
    OSS_SHMEM_SET    *smLink);

extern int32 OSS_SharedMemClear(
    OSS_HANDLE       *oss,
    OSS_SHMEM_HANDLE *smHdl,
    OSS_SHMEM_CLEAR  *smClear);
#endif

/* SMBus specific OSS function prototypes */
extern int32 OSS_GetSmbHdl(
    OSS_HANDLE  *oss,
    u_int32		busNbr,
	void		**smbHdlP);

extern int32 OSS_SetSmbHdl(
    OSS_HANDLE  *oss,
    u_int32		busNbr,
	void		*smbHdl);

/* swapping prototypes */
extern u_int16 OSS_Swap16( u_int16 word );
extern u_int32 OSS_Swap32( u_int32 dword );

#if !defined(OS9000) && !defined(OS9)
extern u_int64 OSS_Swap64( u_int64 qword );
#endif

/* double linked list prototypes */
extern OSS_DL_LIST *OSS_DL_NewList( OSS_DL_LIST *l );
extern OSS_DL_NODE *OSS_DL_Remove( OSS_DL_NODE *n );
extern OSS_DL_NODE *OSS_DL_RemHead( OSS_DL_LIST *l );
extern OSS_DL_NODE *OSS_DL_AddTail( OSS_DL_LIST *l, OSS_DL_NODE *n );
#ifdef OSS_HAS_DL_INSERT_AFTER
extern OSS_DL_NODE *OSS_DL_InsertAfter( OSS_DL_NODE *n1, OSS_DL_NODE *n2 );
#endif

#ifdef __cplusplus
   }
#endif
#endif /*_OSS_H_*/


































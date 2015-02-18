/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  chameleonv2.c
 *
 *      \author  dieter.pfeuffer@men.de
 *        $Date: 2014/07/22 17:01:01 $
 *    $Revision: 2.30 $
 *
 *        \brief  Chameleon-V2 routines for chameleon library
 *
 *     Switches: MAC_IO_MAPPED / MAC_MEM_MAPPED
 *               MAP_REQUIRED - enables mapping/unmapping
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: chameleonv2.c,v $
 * Revision 2.30  2014/07/22 17:01:01  ts
 * R: member reserved appeared also when CHAM_V0 support activated
 * M: remove reference to reserved member also in CHAM_V0 dependent code
 *
 * Revision 2.29  2014/02/19 12:37:34  ts
 * R: reserved member replaced with minor revision in chameleon.h
 * M: removed assignment of CFGTABLE_READLONG[1] hiword
 *
 * Revision 2.28  2014/02/04 13:53:41  awerner
 * R: No support for fpga header minor Revision
 * M: Added minor Revision to TableIdent Function
 *
 * Revision 2.27  2012/09/10 16:13:02  sy
 * R:1. only single PCI domain supported
 *      2. in line60, the complete BAR setting, including attribute bits are used to determine if the BAR is set up
 * M:1. multiple PCI domain supported, introduced new Macro OSS_VXBUS_SUPPORT
 *     2. attribute bits are filtered out in determining BAR is set up or not
 *
 * Revision 2.26  2012/02/08 10:11:24  dpfeuffer
 * R:1. Windows compiler: fatal error invalid preprocessor command 'warning'
 *   2. Windows compiler: warning due to local declaration of the same name
 *      used in outer scope
 *   3. Windows compiler: warning due to wrong data type
 * M:1. HMap(): #warning !! MAP_REQUIRED is set ! removed
 *   2. HEnumTbl(): nbr renamed to dbgNbr
 *   3. InitPci
 *
 * Revision 2.25  2011/12/19 19:44:34  ts
 * R: compiler vxW68 complained about double define OSS_DOMAIN_NBR
 * M: made define dependent if its previously defined
 *
 * Revision 2.24  2011/09/10 11:39:52  ts
 * R: multiple PCI domains were introduced in MM50
 * M: added defines for domains
 *
 * Revision 2.23  2011/07/19 11:05:10  DPfeuffer
 * R: improvement of debug information
 * M: debug prints changed
 *
 * Revision 2.22  2011/04/14 17:39:42  CRuff
 * R: support of pci domains
 * M: changed debug prints to also show the pci domain (is merged into the bus
 *    number)
 *
 * Revision 2.21  2011/01/18 10:47:31  DPfeuffer
 * R:1. BAR descriptor changed to support io mapped address spaces
 *   2. debug print erroneous
 *   3. UnitIdentNode(): reserved field of UNIT descriptor wrong evaluated
 *   4. InitPci(): may read from BAR0 by using the wrong address mapping
 * M:1. HSearchBarDesc(): evaluates/mask-out LSB of BAx to get address mapping
 *      type / real base address
 *   2. debug print fixed
 *   3. evaluation fixed
 *   4. prevent mem/io lib version to read from io/mem mapped BAR0
 *
 * Revision 2.20  2010/10/19 10:31:09  CRuff
 * R: no native code must be in common files
 * M: undid changes of revision 2.19
 *
 * Revision 2.19  2010/08/23 17:41:57  CRuff
 * R: 1. chameleon lib is now built as core library (ALL_CORE_LIBS)
 * M: 1. added symbol exports for chameleonv2 methods
 *
 * Revision 2.18  2009/08/19 11:08:12  CRuff
 * R: No error message if BAR mapping check fails in InitPci()
 * M: Added debug prints if BAR mapping does not access flag
 *
 * Revision 2.17  2009/03/17 15:21:08  dpfeuffer
 * R: 1. compiler errors with VC2008 64bit compiler
 *    2. compiler warnings with VC2008 64bit compiler
 * M: 1. (U_INT32_OR_64) casts added
 *    2. debug prints with pointers changed to %p
 *
 * Revision 2.16  2008/09/09 14:39:17  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 2.15  2008/03/31 11:31:38  DPfeuffer
 * completely revised to support 'pluggable bridges'
 *
 * Revision 2.14  2007/07/11 09:19:27  CKauntz
 * added CHAMELEONV2_EXPORT_CHAMELEONV0 for changing extern to static
 * changed CHAM_Init has four instances
 *
 * Revision 2.13  2007/05/18 11:24:26  cs
 * added:
 *   + support for MEN PCI vendor ID
 *     (use defines from MEN/chameleon.h)
 * fixed:
 *   - BAR descriptors where not found when placed after CPU/BRIDGE descriptors
 *     fix use of CHAV2_*_LSIZE in HSearchBarDesc()
 *
 * Revision 2.12  2006/12/08 15:08:32  ufranke
 * added
 *  + InstanceFind() missing error check for CPU/BRIDGE
 *
 * Revision 2.11  2006/09/11 10:32:41  cs
 * reduced DBG_LEV1 outputs, moved some to DBG_LEV2
 *
 * Revision 2.10  2006/08/02 10:53:49  DPfeuffer
 * casts added
 *
 * Revision 2.9  2006/08/02 10:12:20  DPfeuffer
 * - InitPci(): don't abort if OSS_BusToPhysAddr() fails
 * - error handling fixed
 *
 * Revision 2.8  2005/12/21 12:42:12  cschuster
 * Bugfix for Cham V1 Tables:
 *     the lib only differentiated between V0 and V2 tables
 *
 * Revision 2.7  2005/12/01 18:52:23  cschuster
 * consider BARs read as 0x00000000 as unassigend as well (VxWorks)
 *
 * Revision 2.6  2005/07/14 11:23:38  cs
 * limited map size of I/O mapped bars to OSS_BAR_IO_MAXSIZE (0x100 Bytes)
 *
 * Revision 2.5  2005/07/08 17:58:09  cs
 * for backward compatibility, CHAV2_PCI_DEVID is not checked any more
 * added some casts
 *
 * Revision 2.4  2005/06/23 16:12:01  kp
 * Copyright line changed (sbo)
 *
 * Revision 2.3  2005/05/09 10:10:24  dpfeuffer
 * InitPci(): uses now translated BAR adresses from OSS_BusToPhysAddr()
 *
 * Revision 2.2  2005/05/04 08:59:38  kp
 * bug fixes when looking for end of table in TableIdent etc.
 * bug fixes in wrong comparision of "nbr" variable in HEnumTbl()
 *
 * Revision 2.1  2005/04/29 14:45:23  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

/***************************************************************************/
/*! \page _cham2func Chameleon-V2 Functions

 The \ref _CHAMV2_FUNC are not directly exported by the chameleon library.
 The functions must be called via function pointers of the #CHAM_FUNCTBL structure.
 The function pointers are initialized with V2::CHAM_Init(), exported as:
 - CHAM_InitMem   : for memory access support
 - CHAM_InitIo    : for io access support
 - CHAM_InitMemSw : for swapped memory access support
 - CHAM_InitIoSw  : for swapped io access support\n
 This provides the possibility for the caller to switch easily from the memory
 access variant to the io access variant or swapped variants of the chameleon library.

 <b>Overview of Chameleon-V2 Functions</b>\n
 V2::InitPci(), V2::InitInside() creates an handle for a chameleon device.
 This handle must be passed to all further V2 functions. V2::Term() destroys
 the created handle.\n
 Note: The handle of the V2 functions is different from the handle of the V0 functions.

 V2::Info() supplies the global information (from the chameleon table header)
 of a chameleon device.\n

 V2::TableIdent() supplies the information about all chameleon tables
 of the chameleon device.\n

 V2::UnitIdent(), V2::BridgeIdent(), V2::CpuIdent supplies information about chameleon
 units/bridges/cpus of the chameleon device.\n

 V2::InstanceFind searches for a chameleon instance (unit/bridge/cpu) with the specified
 characteristic(s).\n

 <b>Chameleon-V2 Bus Architecture</b>\n
 Chameleon-V2 devices may consists of several busses (e.g. Wishbone, Avalon) which are
 connected by bridges. All chameleon instances (unit/bridge/cpu) of a chameleon bus
 are described by a seperate, bus specific, Chameleon-V2 table.\n
 A chameleon bus behind a bridge may be always available (fix connected) or may be
 pluggable. For that reason, the bus enumeration was moved from a fix enumeration
 defined within the Chameleon-V2 tables to a dynamic enumeration implemented within
 the chameleon V2 functions.\n
 Note: Initially, the Chameleon-V2 table header had implemented a 'bus number' entry
 but that entry was removed befor the first real usage of the Chameleon-V2 functions
 with a chameleon device with more than one bus.\n

 For a detailed description of the Chameleon-V2 architecture, please refer to the
 <a href="N:\Entwicklung\Chip Design\DSFI"> MEN DSFI Specification</a>.

 \section _CHAM2_BUSID Chameleon-V2 Bus Enumeration:
 The busId member of the Chameleon-V2 structures (CHAMELEONV2_INFO, CHAMELEONV2_TABLE,
 CHAMELEONV2_UNIT, CHAMELEONV2_BRIDGE, CHAMELEONV2_CPU, CHAMELEONV2_FIND) return the
 bus number assigned from the chameleon library.\n
 The following bus number layout/computation is implemented:\n

 <b>busId=0xJKLM</b>\n
 where <b>J/K/L/M</b> counts the number of bridges (from 0x1..0xe) at the first/second/third/fourth
 bus layer.\n
 Therefore, each table can specify up to fourteen bridges (0x1..0xe) and up to five bus layers
 (a depth of four bridges) are possible within a chameleon device. The root bus (where the
 caller of V2 functions always resides) has always the busId=0x0000.\n

 <b>Example bus tree:</b>\n

 \code
 [CALLER]
   |
<root bus 0x0000>
   |
   |
   +---BRG---<0x1000>---BRG---<0x1100>
   |
   |
   +---BRG---<0x2000>--+--BRG---<0x2100>
   |                   |
   |                   |
   |                   +--BRG---<0x2200>---BRG---<0x2210>---BRG---<0x2211>
   |
   +---BRG---<0x3000>-
  \endcode

 \section _CHAM2_ADDR Chameleon-V2 Addressing
 The <b>bar</b>, <b>offset</b> and <b>*addr</b> members of the Chameleon-V2 structures (CHAMELEONV2_UNIT,
 CHAMELEONV2_BRIDGE) represents the caller's view. That means:\n
 - bar : indicates the BAR where the unit/bridge is accessible from the caller
 - offset : indicates the offset within the BAR where the unit/bridge is accessible
 from the caller
 - *addr : indicates the computed physical address where the unit can be
 accessed from the caller\n
   Note: The physical address may be mapped from the caller into a virtual address
   space to access an unit. The CHAMELEONV2_BRIDGE.addr parameter should be used only
   for informational purposes, because bridges should normally not directly accessed.

 */


#ifdef CHAM_UNITTEST
	#include "../UNITTEST/COM/cha_ut.h"
#endif

#ifndef CHAMELEONV2_EXPORT_CHAMELEONV0
	#define  CHAMELEONV0_STATIC_EXTERN static
	#include "chameleon.c"
#else
	#define  CHAMELEONV0_STATIC_EXTERN extern
#endif /* CHAMELEONV2_EXPORT_CHAMELEONV0 */

#include "chameleon_int.h"

/*--------------------------------------*/
/*	DEFINES			            		*/
/*--------------------------------------*/
/* support Chameleon-V0/V1 devices by using Chameleon-V0 functions */
#define SUPPORT_CHAM_V0

#define PCI_BAR_MEM_MINSIZE	4096 /* 4k */
#define PCI_BAR_IO_MAXSIZE	256  /* I/O bars must not be bigger */
#define LOC_PCI				0x01
#define LOC_INSIDE			0x10
#define UNKN_CHATBL_REV		0xff

#define DBG_INITLEVEL	(DBG_LEV2 | DBG_LEV1 | DBG_LEVERR | DBG_NORM) /* fix debug level */
#define DBH				h->dbgHdl

#define DBG_MYLEVEL		h->dbgLev


/* TODO: expand for multi PCI domain CPUs */
#ifndef OSS_BUS_NBR
#define OSS_BUS_NBR(x) 		x
#endif

#ifndef OSS_DOMAIN_NBR
#define OSS_DOMAIN_NBR(x) 	x
#endif


/*--------------------------------------*/
/*	TYPEDEFS     	                    */
/*--------------------------------------*/
/** PCI device location */
typedef struct {
	u_int32 bus; /*contains pciDomain in upper 8 bit!*/
	u_int32 dev;
	u_int32 func;
} PCILOC;

/* ========== lists ========== */
/** table list */
typedef struct {
	OSS_DL_LIST		l;		/* DL-List */
	u_int16			nbr;	/* number of list entries */
} TBL_LIST;

/** unit list */
typedef struct {
	OSS_DL_LIST		l;		/* DL-List */
	u_int16			nbr;	/* number of list entries */
} UNIT_LIST;

/** cpu list */
typedef struct {
	OSS_DL_LIST		l;		/* DL-List */
	u_int16			nbr;	/* number of list entries */
} CPU_LIST;

/** bridge list */
typedef struct {
	OSS_DL_LIST		l;		/* DL-List */
	u_int16			nbr;	/* number of list entries */
} BRG_LIST;

/* ========== nodes ========== */
/** table node (bus specific) */
typedef struct _TBL_NODE {
	OSS_DL_NODE			n;
	MACCESS				ma;				/* virt. table base address */
	void				*addr;			/* phys. table base address */
	u_int16				busId;			/* bus number of bus where table resides (0x000...0xEEEE) */
	u_int16				brgBusCount;	/* count of bridged busses in table 0x0..0xE */
	u_int8				usedBars;		/* bit filed: bit 5..0 = bar 5..0 (1=used, 0=unused) */
	u_int32				structSize;		/* struct size [bytes] */
} TBL_NODE;

/** unit node */
typedef struct _UNIT_NODE {
	OSS_DL_NODE			n;
	MACCESS				ma;
	struct _TBL_NODE	*tblN;		/* ptr to belonging table */
	struct _BRG_NODE	*prevBrgN;	/* ptr to previous bridge or NULL */
	u_int32				structSize;	/* struct size [bytes] */
} UNIT_NODE;

/** cpu node */
typedef struct _CPU_NODE {
	OSS_DL_NODE			n;
	MACCESS				ma;
	struct _TBL_NODE	*tblN;		/* ptr to belonging table */
	u_int32				structSize;	/* struct size [bytes] */
} CPU_NODE;

/** bridge node */
typedef struct _BRG_NODE {
	OSS_DL_NODE			n;
	MACCESS				ma;
	u_int32				offset;
	u_int32				size;
	u_int8				bar;		/* bar of bridge window at source bus */
	u_int8				dbar;		/* destination bar behind bridge */
	u_int16				destBusId;	/* destination bus number for bus behind bridge */
	struct _TBL_NODE	*tblN;		/* ptr to belonging table */
	struct _TBL_NODE	*destTblN;	/* ptr to destination table */
	struct _BRG_NODE	*prevBrgN;	/* ptr to previous bridge or NULL */
	u_int32				structSize;	/* struct size [bytes] */
} BRG_NODE;

/** chameleon V2 handle */
typedef struct {
	u_int32			structSize;	/* struct size [bytes] */
	OSS_HANDLE		*osh;		/* OSS handle */
	u_int8			chaRev;		/* chameleon revision */
	DBGCMD(DBG_HANDLE *dbgHdl;)	/* debug handle */
	DBGCMD(u_int32 dbgLev;)		/* debug level */

	/* callers location and addresses */
	u_int8			location;			/* LOC_PCI or LOC_INSIDE */
	PCILOC			pciLoc;				/* PCI specific location info */
	CHAMELEONV2_BA	ba[NBR_OF_BARS];	/* base addresses for calling CPU */

	/* lists */
	TBL_LIST		tblList;	/* list of all chameleon tables */
	UNIT_LIST		unitList;	/* list of all chameleon units */
	CPU_LIST		cpuList;	/* list of all chameleon cpus */
	BRG_LIST		brgList;	/* list of all chameleon briges */
#ifdef SUPPORT_CHAM_V0
	CHAMELEON_HANDLE	*h0;	/* chameleon-V0 handle */
#endif
} CHAMELEONV2_HDL;

/*--------------------------------------*/
/*	PROTOTYPES     	                    */
/*--------------------------------------*/
/*--------------------------------------*/
/*	PROTOTYPES     	                    */
/*--------------------------------------*/
#ifdef _DOXYGEN_
	#define _STATIC_
#else
	#define _STATIC_ static
#endif
/* Chameleon-V2 functions */
_STATIC_ int32 InitPci(
	OSS_HANDLE *osh,
#ifdef OSS_VXBUS_SUPPORT
	VXB_DEVICE_ID busCtrlID,
#endif	
	u_int32 pciBus,
	u_int32 pciDev,
	u_int32 pciFunc,
	CHAMELEONV2_HANDLE **chahP);
_STATIC_ int32 InitInside(
	OSS_HANDLE *osh,
	void *tblAddr,
	CHAMELEONV2_HANDLE **chahP);
_STATIC_ int32 Info(
	CHAMELEONV2_HANDLE *chah,
	CHAMELEONV2_INFO *info);
_STATIC_ int32 TableIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_TABLE *table);
_STATIC_ int32 UnitIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_UNIT *unit);
_STATIC_ int32 BridgeIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_BRIDGE *bridge);
_STATIC_ int32 CpuIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_CPU *cpu);
_STATIC_ int32 InstanceFind(
	CHAMELEONV2_HANDLE *chah,
	int32				idx,
	CHAMELEONV2_FIND	find,
	CHAMELEONV2_UNIT	*unit,
	CHAMELEONV2_BRIDGE	*bridge,
	CHAMELEONV2_CPU		*cpu);
_STATIC_ void Term(
	CHAMELEONV2_HANDLE **chahP);

static int32 HInit( OSS_HANDLE *osh, CHAMELEONV2_HDL **hP );
static int32 HCheckTable( CHAMELEONV2_HDL *h, MACCESS ma, u_int8 *chaRev );
static int32 HEnumTbl( CHAMELEONV2_HDL *h, void *tblAddr, BRG_NODE *prevBrgN );
static int32 HMap( CHAMELEONV2_HDL *h, void *addr, MACCESS *maP );
static void HUnmap( CHAMELEONV2_HDL *h, MACCESS ma );
static int32 HInitTable( CHAMELEONV2_HDL *h, void *tblAddr, TBL_NODE **tblNP );
static int32 HSearchBarDesc( CHAMELEONV2_HDL *h, TBL_NODE	*tblN );
static int32 InstanceFindBrg( CHAMELEONV2_HDL *h, CHAMELEONV2_FIND *find,
	CHAMELEONV2_BRIDGE *bridge );
static int32 BridgeIdentNode( CHAMELEONV2_HDL *h, u_int32 idx,
	CHAMELEONV2_BRIDGE *bridge, BRG_NODE **brgNP );
static int32 UnitIdentNode(	CHAMELEONV2_HANDLE *chah, u_int32 idx,
	CHAMELEONV2_UNIT *unit, UNIT_NODE **unitNP);

/**
 * \defgroup _CHAMV2_FUNC Chameleon-V2 Functions
 *  @{
 */

/**********************************************************************/
/** Library initialization routine
 *
 *  The function fills the #CHAM_FUNCTBL structure with the Chameleon-V2
 *  function pointers.
 *
 *	\param fP		pointer to structure where function pointers will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
extern int32 CHAM_Init( CHAM_FUNCTBL *fP )
{
	if( !fP )
		return CHAMELEONV2_HANDLE_ILL;

	fP->InitPci		= InitPci;
	fP->InitInside	= InitInside;
	fP->Info		= Info;
	fP->TableIdent	= TableIdent;
	fP->UnitIdent	= UnitIdent;
	fP->BridgeIdent = BridgeIdent;
	fP->CpuIdent	= CpuIdent;
	fP->InstanceFind= InstanceFind;
	fP->Term		= Term;

	return 0;
}

/**********************************************************************/
/** Chameleon-V2 init routine to access a chameleon device from PCI side
 *
 *  This is the initialization function for callers outside a chameleon
 *  device that resides on a PCI bus. The chameleon device must be
 *  addressed by the \a pciBus, \a pciDev, \a  pciFunc parameters.
 *
 *  The function returns a handle for the specified chameleon device.
 *
 *  This routine
 *  - Checks the presence of the specified chameleon device
 *	- Reads the base address registers
 *	- Checks if the configuration table contains the sync word
 *  - Determines the number of units in the configuration table
 *  - Maps the address space of the config table (if required for the OS)
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *
 *  \param osh		OSS handle, returned by OSS_Init
 *  \param pciBus	PCI Bus Id (0..255, upper 8 bit contain pci domain!)
 *  \param pciDev	PCI Device Id (0..31)
 *  \param pciFunc	PCI Function Id (0..7)
 *	\param chahP	pointer to variable where chameleon handle
 *					 will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 InitPci(
	OSS_HANDLE *osh,
#ifdef OSS_VXBUS_SUPPORT
	VXB_DEVICE_ID busCtrlID,
#endif
	u_int32 pciBus,
	u_int32 pciDev,
	u_int32 pciFunc,
	CHAMELEONV2_HANDLE **chahP)
{
	CHAMELEONV2_HDL		hTmp;
	CHAMELEONV2_HDL		*h = &hTmp;  /* give it temporary memory on stack */
	int32				val=0, i;
	int32				error=0;
	void				*tblAddr=NULL;
	u_int32				barVal;

	if( !chahP )
		return CHAMELEONV2_HANDLE_ILL;
	*chahP = NULL;

	/* prepare temporary debugging */
	DBGCMD( DBH = NULL );
	DBGINIT((NULL,&DBH));
	DBGCMD( DBG_MYLEVEL = DBG_INITLEVEL );
	DBGWRT_1((DBH,"CHAM - InitPci: pci-device domain/bus/dev/func=%d/%d/%d/%d\n",
				OSS_DOMAIN_NBR( pciBus ), OSS_BUS_NBR( pciBus ), pciDev, pciFunc ));
	
	/*------------------------------+
	|  check if PCI device present  |
	+------------------------------*/	
	if( OSS_PciGetConfig(osh,
#ifdef OSS_VXBUS_SUPPORT
		busCtrlID,
#endif
		pciBus,pciDev,pciFunc,
		OSS_PCI_VENDOR_ID,&val) ){
		error = CHAMELEONV2_PCIDEV_ERR;
		goto CLEANUP;
	}
	
	if( val != CHAMELEON_PCI_VENID_MEN    &&
		val != CHAMELEON_PCI_VENID_ALTERA ){
		DBGWRT_ERR((DBH,"*** CHAM - InitPci: illegal pci-vendor-id=0x%x at domain/bus/dev/func %d/%d/%d/%d\n",
			val, OSS_DOMAIN_NBR( pciBus ), OSS_BUS_NBR( pciBus ), pciDev, pciFunc ));
		error = CHAMELEONV2_PCIDEV_ILL;
		goto CLEANUP;
	}

	if( OSS_PciGetConfig(osh,
#ifdef OSS_VXBUS_SUPPORT
		busCtrlID,
#endif			
		pciBus,pciDev,pciFunc,
		OSS_PCI_DEVICE_ID,&val) ){
		error = CHAMELEONV2_PCIDEV_ERR;
		goto CLEANUP;
	}

/* not to be checked, otherwise not backwards compatible any more
	if( val != CHAMELEON_PCI_DEV_ID ){
		DBGWRT_ERR((tempDbgHdl,"*** CHAM - InitPci: illegal pci-device-id=0x%x\n", val));
		error = CHAMELEONV2_PCIDEV_ILL;
		goto CLEANUP;
	}
*/
	#ifdef DBG
		if( DBH )
		{
			DBGEXIT((&DBH));
		}
	#endif /* DBG */
	h = NULL; /* reset from hTmp */
	
	/* create CHAMELEONV2_HDL struct and init lists */
	if( (error = HInit( osh, &h )) )
		goto CLEANUP;

	h->location = LOC_PCI;
	h->pciLoc.bus = pciBus;
	h->pciLoc.dev = pciDev;
	h->pciLoc.func = pciFunc;

	/* reinit debugging */
	DBGINIT((NULL,&DBH));
	DBGCMD( DBG_MYLEVEL = DBG_INITLEVEL );

	*chahP = (CHAMELEONV2_HANDLE*)h;
	
	/*-------------------------+
	|  Read base address regs  |
	+-------------------------*/
	for( i=0; i<NBR_OF_BARS; i++ ){

		/* get BAR info (assigned or not, mem/io ) */
		OSS_PciGetConfig(osh,
#ifdef OSS_VXBUS_SUPPORT
			busCtrlID,
#endif		
			pciBus, pciDev, pciFunc,
			OSS_PCI_ADDR_0+i, (int32*)&barVal);
		
		/* bar unassigned? */
		/* cs: on VxWorks unassigned bars are shown as 0x00000000 */
		if( ((barVal & 0xFFFFFFF0) == 0xfffffff0) || ((barVal & 0xFFFFFFF0) == 0x00000000) ){
			 h->ba[i].addr = 0;
			 h->ba[i].type = -1;	/* unused */
		}
		/* bar assigned! */
		else{
			/* io mapped? */
			if( barVal & 0x01 )
				 h->ba[i].type = 1;
			/* mem mapped! */
			else
				 h->ba[i].type = 0;

			h->ba[i].size = 0;		/* unknown */

			/* get translated address of the BAR */
			if( OSS_BusToPhysAddr(osh, OSS_BUSTYPE_PCI,
				(void**)&h->ba[i].addr, pciBus, pciDev, pciFunc, i ) ){
				/* no translated address available */
				h->ba[i].addr = 0;
				h->ba[i].type = -1;	/* unused */
				
			}
		}

		DBGWRT_1((DBH,"BAR%d: h->ba[i].addr = %08x type=%d\n", i, h->ba[i].addr, h->ba[i].type ));

	}

#ifdef SUPPORT_CHAM_V0
{
	MACCESS			ma;

	/* am I able to access bar 0 address space type? */
  #ifdef MAC_IO_MAPPED
	if( h->ba[0].type == 1 )	/* io */
  #else
	if( h->ba[0].type == 0 )	/* mem */
  #endif
	{
		/* determine chameleon revision */
		if( (error=HMap( h, (void*)(U_INT32_OR_64)h->ba[0].addr, &ma )) )
			goto CLEANUP;
		
		DBGWRT_1((DBH,"calling HCheckTable with ma=%08p, h->ba[0].addr=%08x \n",
				  (void*)ma, h->ba[0].addr ));
		error = HCheckTable( h, ma, &h->chaRev );
		
		HUnmap( h, ma );
		
		if( error )
			goto CLEANUP;

		if( h->chaRev == 0 || h->chaRev == 1 ){
			CHAMELEON_UNIT	cha0Unit;

			if( (error=ChameleonInit( h->osh,
#ifdef OSS_VXBUS_SUPPORT
				busCtrlID,
#endif					
				h->pciLoc.bus, h->pciLoc.dev, &h->h0 )) ){
				goto CLEANUP;
			}
			/* determine number of units, dump info while we're here */
			DBGWRT_1((DBH, " Unit                modCode Rev    Inst\tIRQ\tBAR Offset   Addr\n"));
				i=0;
			while( ChameleonUnitIdent( h->h0, i, &cha0Unit ) == CHAMELEON_OK ) {
	
				DBGWRT_1((DBH, " %02d %-17s"  		/* idx + name		*/
							   "0x%04x %2d"			/* devId/Rev. */
							   "   0x%02x"			/* instance 		*/
							   "\t0x%02x"			/* interrupt 		*/
							   "\t%d   0x%04x"		/* BAR / offset  	*/
							   "   0x%p\n",			/* addr 			*/
				   i, ChameleonModName( cha0Unit.modCode), cha0Unit.modCode, cha0Unit.revision, cha0Unit.instance,
				   cha0Unit.interrupt, cha0Unit.bar, (unsigned int)cha0Unit.offset, cha0Unit.addr));
					i++;
			}
			h->unitList.nbr = (u_int16)i;

			/* finish */
			goto CLEANUP;
		}
	}
	else{
#ifdef MAC_IO_MAPPED
		DBGWRT_ERR((DBH,"*** CHAM - InitPci(V0): bar 0 MEM mapped. NOT supported!\n"));
#else 	
		DBGWRT_ERR((DBH,"*** CHAM - InitPci(V0): bar 0 I/O mapped. NOT supported!\n"));
#endif
	} /* if */
}
#endif	
	/*if Chameleon V0 is not supported or
	 * Chameleon V0 is supported but not found
	 */
	/*---------------------------------+
	|  Search base table in Bar0..5    |
	+---------------------------------*/
	for( i=0; i<NBR_OF_BARS; i++ ){		
		/* am I able to access bar address space type? */
	#ifdef MAC_IO_MAPPED
		if( h->ba[i].type == 1 )	/* io */
	#else
		if( h->ba[i].type == 0 )	/* mem */
	#endif
		{
			tblAddr = ((void*)(U_INT32_OR_64)h->ba[i].addr);
			
			/* enumerate the table */
			error = HEnumTbl( h, tblAddr, NULL );
			
			/* table found in current bar? */
			if( error == 0 ){
				break;
			/* no table found in current bar? */
			} else if( error == CHAMELEONV2_TABLE_NOT_FOUND ){
				continue;
			/* table found but error occured! */
			} else{
				goto CLEANUP;
			}
		} else {
	#ifdef MAC_IO_MAPPED
			DBGWRT_ERR((DBH,"*** CHAM - InitPci(V2): bar %d MEM mapped. NOT supported!\n",i));
	#else 	
			DBGWRT_ERR((DBH,"*** CHAM - InitPci(V2): bar %d I/O mapped. NOT supported!\n",i));
	#endif
		} /* if */
	} /* for */

	if( i == NBR_OF_BARS ){
		DBGWRT_ERR((DBH,"*** CHAM - InitPci: no valid chameleon table found\n"));
		error = CHAMELEONV2_TABLE_NOT_FOUND;
		goto CLEANUP;
	}

CLEANUP:
	if( error )
	{
		if( *chahP )
		{
			Term( chahP );
			*chahP = NULL;
		}
		else
		{
			#ifdef DBG
				if( DBH )
				{
					DBGEXIT((&DBH));
				}
			#endif /* DBG */
		}
	}

	return error;
}

/**********************************************************************/
/** Chameleon-V2 init routine for chameleon devices at non PCIbus location
 *
 *  This is the initialization function for callers inside a chameleon
 *  device or when the chameleon device resides at a non PCIbus
 *  (e.g. LPC bus).
 *  The chameleon device must be addressed by the \a tblAddr parameter.
 *
 *  The function returns a handle for the specified chameleon device.
 *
 *  This routine
 *  - Checks the presence of the specified chameleon device
 *	- Checks if the configuration table contains the sync word
 *  - Determines the number of units in the configuration table
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V2 devices only.
 *  The function returns #CHAMELEONV2_TABLE_REV_UNSUP for Chameleon-V0/V1
 *  devices.
 *
 *  \param osh			OSS handle, returned by OSS_Init
 *  \param tblAddr		(unmapped) address of the chameleon table
 *                       for the bus where the caller resides
 *	\param chahP		pointer to variable where chameleon handle
 *						 will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 InitInside(
	OSS_HANDLE *osh,
	void *tblAddr,
	CHAMELEONV2_HANDLE **chahP)
{
	CHAMELEONV2_HDL		*h = NULL;
	int32				error=0;

	if( !chahP )
		return CHAMELEONV2_HANDLE_ILL;

	*chahP = NULL;

	/* create CHAMELEONV2_HDL struct and init lists */
	if( (error = HInit( osh, &h )) )
		goto CLEANUP;

	/* prepare debugging */
	DBGINIT((NULL,&DBH));
	DBGCMD( DBG_MYLEVEL = DBG_INITLEVEL ;)
	DBGWRT_1((DBH,"CHAM - InitInside: tblAddr=%08p\n", tblAddr));

	h->location = LOC_INSIDE;

	*chahP = (CHAMELEONV2_HANDLE*)h;


#ifdef SUPPORT_CHAM_V0
{
	MACCESS	ma;

	/* determine chameleon revision */
	if( (error=HMap( h, tblAddr, &ma )) )
		goto CLEANUP;

	error = HCheckTable( h, ma, &h->chaRev );
	HUnmap( h, ma );
	if( error )
		goto CLEANUP;

	if( h->chaRev == 0 || h->chaRev == 1 ){
		DBGWRT_ERR((DBH,"*** HInitTable: FPGA internal CPUs not supported "
			"for Chameleon-V0/V1 devices\n"));
		error = CHAMELEONV2_TABLE_REV_UNSUP;
		goto CLEANUP;
	}
}
#endif

	/* enumerate the table */
	error = HEnumTbl( h, tblAddr, NULL );

CLEANUP:
	if( error )
	{
		if( *chahP )
		{
			Term( chahP );
			*chahP = NULL;
		}
		else
		{
			#ifdef DBG
				if( DBH )
				{
					DBGEXIT((&DBH));
				}
			#endif /* DBG */
		}
	}

	return error;
}

/**********************************************************************/
/** Chameleon-V2 routine to get some global info about Chameleon device
 *
 *  See #CHAMELEONV2_INFO for the parameters that will be provided.
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *  See #CHAMELEONV2_INFO struct restrictions for Chameleon-V0/V1 devices.
 *
 *	\param chah		chameleon handle returned from InitXx()
 *  \param info		ptr to #CHAMELEONV2_INFO struct where information
 *					 will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 Info(
	CHAMELEONV2_HANDLE *chah,
	CHAMELEONV2_INFO *info)
{
	CHAMELEONV2_HDL *h = (CHAMELEONV2_HDL*)chah;
	u_int8			i;

	DBGWRT_1((DBH,"CHAM - Info: entered\n"));

	if( !chah )
		return CHAMELEONV2_HANDLE_ILL;

	info->chaRev	= h->chaRev;
	for( i=0; i<NBR_OF_BARS; i++ )
		info->ba[i]	= h->ba[i];
	info->unitNbr	= h->unitList.nbr;

#ifdef SUPPORT_CHAM_V0
	if( h->chaRev == 0 || h->chaRev == 1 ){

		info->busId		= 0;
		info->tableNbr	= 1;
		info->bridgeNbr = 0;
		info->cpuNbr	= 0;

		return 0;
	}
#endif

	info->busId		= ((TBL_NODE*)h->tblList.l.head)->busId;
	info->tableNbr	= h->tblList.nbr;
	info->bridgeNbr = h->brgList.nbr;
	info->cpuNbr	= h->cpuList.nbr;

	return 0;
}

/**********************************************************************/
/** Chameleon-V2 routine to identify all Chameleon tables
 *
 *  Retrieve information about all Chameleon tables within the Chameleon
 *  table tree (see #CHAMELEONV2_TABLE).
 *
 *  The \a idx parameter is used as an consecutive index and specifies
 *  for which table the information should be returned in \a table.
 *  The function returns #CHAMELEONV2_NO_MORE_ENTRIES if no (more) unit with
 *  the given index exist.
 *
 *  Note: Use idx=0 to get the information about the table of the bus
 *        where the caller resides.
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *  See #CHAMELEONV2_TABLE struct restrictions for Chameleon-V0/V1 devices.
 *
 *	\param chah		chameleon handle returned from InitXx()
 *  \param idx		index of table to query (0..n)
 *  \param table	ptr to #CHAMELEONV2_TABLE struct where information
 *					 will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 TableIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_TABLE *table)
{
	CHAMELEONV2_HDL *h = (CHAMELEONV2_HDL*)chah;
	u_int32			val, i;
	TBL_NODE		*tblN;

	DBGWRT_1((DBH,"CHAM - TableIdent: idx=%d\n", idx));

	if( !chah )
		return CHAMELEONV2_HANDLE_ILL;

#ifdef SUPPORT_CHAM_V0
	if( h->chaRev == 0 || h->chaRev == 1){

		int32			error;
		char			variant;
		int				revision;

		if( idx != 0 )
			return CHAMELEONV2_NO_MORE_ENTRIES;

		if( (error = ChameleonGlobalIdent( h->h0, &variant, &revision)) )
			return error;

		table->model = variant;
		table->revision = (u_int16)revision;
		table->busId	= 0;
		table->busType  = CHAMELEONV2_BUSTYPE_WB;
		if( h->chaRev == 0 )
			table->magicWord = CHAMELEON_MAGIC_00;
		else
			table->magicWord = CHAMELEON_MAGIC_01;
		table->minRevision = 0;
		table->file[0] = '\0';

		return 0;
	}
#endif

	if( idx >= h->tblList.nbr )
		return CHAMELEONV2_NO_MORE_ENTRIES;

	/* get node for index */
	for( tblN=(TBL_NODE*)h->tblList.l.head, i=0;
		 tblN->n.next && i<idx;
		 tblN = (TBL_NODE*)tblN->n.next, i++ );

	/* 0x00 */
	val = CFGTABLE_READLONG( tblN->ma, 0 );
	table->revision = (u_int16)( val      & 0xff);
	table->model	= (char)   ((val>> 8) & 0xff);
	table->busId	= tblN->busId;
	table->minRevision = (u_int16)((val >> 16) & 0xff);
	table->busType  = (u_int16)((val>>24) & 0xff);

	/* 0x04 */
	val = CFGTABLE_READLONG( tblN->ma, 1 );
	table->magicWord = (u_int16)( val      & 0xffff);

	/* 0x08 */
	val = CFGTABLE_READLONG( tblN->ma, 2 );
	table->file[ 0] = (char)( val      & 0xff);
	table->file[ 1] = (char)((val>> 8) & 0xff);
	table->file[ 2] = (char)((val>>16) & 0xff);
	table->file[ 3] = (char)((val>>24) & 0xff);

	/* 0x0c */
	val = CFGTABLE_READLONG( tblN->ma, 3 );
	table->file[ 4] = (char)( val      & 0xff);
	table->file[ 5] = (char)((val>> 8) & 0xff);
	table->file[ 6] = (char)((val>>16) & 0xff);
	table->file[ 7] = (char)((val>>24) & 0xff);

	/* 0x10 */
	val = CFGTABLE_READLONG( tblN->ma, 4 );
	table->file[ 8] = (char)( val      & 0xff);
	table->file[ 9] = (char)((val>> 8) & 0xff);
	table->file[10] = (char)((val>>16) & 0xff);
	table->file[11] = (char)((val>>24) & 0xff);

	table->file[12] = '\0';

	return 0;
}

/**********************************************************************/
/** Chameleon-V2 routine to identify all General Device units
 *
 *  Retrieve information about all General Device units implemented
 *  in the chameleon device (see #CHAMELEONV2_UNIT).
 *
 *  The \a idx parameter is used as an consecutive index and specifies
 *  for which unit the information should be returned in \a unit.
 *  The function returns #CHAMELEONV2_NO_MORE_ENTRIES if no (more) unit with
 *  the given index exist.
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *  See #CHAMELEONV2_UNIT struct restrictions for Chameleon-V0/V1 devices.
 *
 *	\param chah		chameleon handle returned from InitXx()
 *  \param idx		index of unit to query (0..n)
 *  \param unit		ptr to #CHAMELEONV2_UNIT struct where information
 *					 will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 UnitIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_UNIT *unit)
{
	CHAMELEONV2_HDL *h = (CHAMELEONV2_HDL*)chah;
	UNIT_NODE		*unitN;

	DBGWRT_3((DBH,"CHAM - UnitIdent: idx=%d\n", idx));

	if( !chah )
		return CHAMELEONV2_HANDLE_ILL;

	return UnitIdentNode(h, idx, unit, &unitN );
}

/**********************************************************************/
/** Helper function for UnitIdent() to get the UNIT_NODE
 *
 *	\param h		ptr to CHAMELEONV2_HDL
 *  \param idx		index of unit to query (0..n)
 *  \param unit		ptr to #CHAMELEONV2_UNIT struct where information
 *					 will be stored
 *  \param unitNP	OUT: ptr to UNIT_NODE ptr, points to used UNIT_NODE
 *
 *  \return 0=ok, or chameleon error code
 */
static int32 UnitIdentNode(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_UNIT *unit,
	UNIT_NODE **unitNP)
{
	CHAMELEONV2_HDL *h = (CHAMELEONV2_HDL*)chah;
	u_int32			val, i, brgOff=0;
	UNIT_NODE		*unitN;
	u_int8			bar;
	u_int32			offset;

	DBGWRT_3((DBH,"CHAM - UnitIdentNode: idx=%d\n", idx));

	*unitNP = NULL;

#ifdef SUPPORT_CHAM_V0
	if( h->chaRev == 0 || h->chaRev == 1 ){

		CHAMELEON_UNIT	cha0Unit;
		int32			error;

		if( (error = ChameleonUnitIdent( h->h0, idx, &cha0Unit )) ){
			if( error == CHAMELEON_OUT_OF_RANGE )
				error = CHAMELEONV2_NO_MORE_ENTRIES;
			return error;
		}

		unit->interrupt = cha0Unit.interrupt;
		unit->revision  = cha0Unit.revision;
		unit->bar       = cha0Unit.bar;
		unit->instance  = cha0Unit.instance;
		unit->offset	= cha0Unit.offset;
		unit->addr		= cha0Unit.addr;

		unit->variant   = 0;
		unit->group     = 0;
		unit->reserved  = 0;
		unit->size		= 0;
		unit->busId		= 0;

		unit->devId = CHAM_ModCodeToDevId( cha0Unit.modCode );

		return 0;
	}
#endif

	if( idx >= h->unitList.nbr )
		return CHAMELEONV2_NO_MORE_ENTRIES;

	/* get node for index */
	for( unitN=(UNIT_NODE*)h->unitList.l.head, i=0;
		 unitN->n.next && i<idx;
		 unitN = (UNIT_NODE*)unitN->n.next, i++ );

	/* 0x00 */
	val = CFGTABLE_READLONG( unitN->ma, 0 );
	unit->interrupt = (u_int16)( val      & 0x3f);
	unit->revision  = (u_int16)((val>> 6) & 0x3f);
	unit->variant   = (u_int16)((val>>12) & 0x3f);
	unit->devId     = (u_int16)((val>>18) & 0x3ff);

	/* 0x04 */
	val = CFGTABLE_READLONG( unitN->ma, 1 );
	bar				= (u_int8) ( val      & 0x7);
	unit->instance  = (u_int16)((val>> 3) & 0x3f);
	unit->group     = (u_int16)((val>> 9) & 0x3f);
	unit->reserved  =           (val>>15) & 0x1ffff;

	/* 0x08 */
	offset = CFGTABLE_READLONG( unitN->ma, 2 );

	/* 0x0c */
	unit->size = CFGTABLE_READLONG( unitN->ma, 3 );

	unit->busId = unitN->tblN->busId;

	/*
	 * Address computation
	 */

	/* fits unit-window into bar-window? */
	if( (h->ba[bar].size != 0 ) &&	/* known size? */
		(h->ba[bar].size < unit->size) ){
		DBGWRT_ERR((DBH,"*** UnitIdent: unit-size (0x%x) > bar-size (0x%x)\n",
			unit->size, h->ba[bar].size ));
		return CHAMELEONV2_TABLE_ERR;
	}

	/* behind a bridge? */
	if( unitN->prevBrgN ){

		/* fits unit-window into bridge-window? */
		if( unit->size > unitN->prevBrgN->size ){
			DBGWRT_ERR((DBH,"*** UnitIdent: devId=%03d, instance=%d: "
				"unit-size (0x%x) > brg-size (0x%x)\n",
				unit->devId, unit->instance, unit->size, unitN->prevBrgN->size ));
			return CHAMELEONV2_TABLE_ERR;
		}
		
		bar = unitN->prevBrgN->bar;
		brgOff = unitN->prevBrgN->offset;
	}

	/* compute unit's address */
	unit->addr = (void*)(U_INT32_OR_64)(h->ba[bar].addr + brgOff + offset);

	DBGWRT_3((DBH," unit address=%08p (= BAR%d=0x%x + brgOffs=0x%x + offset=0x%x)\n",
		unit->addr, bar, h->ba[bar].addr, brgOff, offset));

	unit->bar = (u_int16)bar;
	unit->offset = brgOff + offset;

	*unitNP = unitN;

	return 0;
}

/**********************************************************************/
/** Chameleon-V2 routine to identify all CPUs
 *
 *  Retrieve information about all CPUs implemented in the chameleon
 *  device (see #CHAMELEONV2_CPU).
 *
 *  The \a idx parameter is used as an consecutive index and specifies
 *  for which CPU the information should be returned in \a cpu.
 *  The function returns #CHAMELEONV2_NO_MORE_ENTRIES if no (more) CPU with
 *  the given index exist.
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *  The function returns #CHAMELEONV2_NO_MORE_ENTRIES for Chameleon-V0/V1
 *  devices.
 *
 *	\param chah		chameleon handle returned from InitXx()
 *  \param idx		index of CPU to query (0..n)
 *  \param cpu		ptr to #CHAMELEONV2_CPU struct where information
 *					 will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 CpuIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_CPU *cpu)
{
	CHAMELEONV2_HDL *h = (CHAMELEONV2_HDL*)chah;
	u_int32			val, i;
	CPU_NODE		*cpuN;

	DBGWRT_1((DBH,"CHAM - CpuIdent: idx=%d\n", idx));

	if( !chah )
		return CHAMELEONV2_HANDLE_ILL;

	if( idx >= h->cpuList.nbr )
		return CHAMELEONV2_NO_MORE_ENTRIES;

	/* get node for index */
	for( cpuN=(CPU_NODE*)h->cpuList.l.head, i=0;
		 cpuN->n.next && i<idx;
		 cpuN = (CPU_NODE*)cpuN->n.next, i++ );

	/* 0x00 */
	val = CFGTABLE_READLONG( cpuN->ma, 0 );
	cpu->interrupt = (u_int16)( val      & 0x3f);
	cpu->revision  = (u_int16)((val>> 6) & 0x3f);
	cpu->variant   = (u_int16)((val>>12) & 0x3f);
	cpu->devId     = (u_int16)((val>>18) & 0x3ff);

	/* 0x04 */
	val = CFGTABLE_READLONG( cpuN->ma, 1 );
	cpu->reserved  = (u_int16)( val      & 0xffff8007);
	cpu->instance  = (u_int16)((val>> 3) & 0x3f);
	cpu->group     = (u_int16)((val>> 9) & 0x3f);

	/* 0x08 */
	cpu->bootAddr = CFGTABLE_READLONG( cpuN->ma, 2 );

	cpu->busId = cpuN->tblN->busId;

	return 0;
}

/**********************************************************************/
/** Chameleon-V2 routine to identify all bridges
 *
 *  Retrieve information about all bridges implemented in the chameleon
 *  device (see #CHAMELEONV2_BRIDGE).
 *
 *  The \a idx parameter is used as an consecutive index and specifies
 *  for which bridge the information should be returned in \a bridge.
 *  The function returns #CHAMELEONV2_NO_MORE_ENTRIES if no (more) bridge with
 *  the given index exist.
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *  The function returns #CHAMELEONV2_NO_MORE_ENTRIES for Chameleon-V0/V1
 *  devices.
 *
 *	\param chah		chameleon handle returned from InitXx()
 *  \param idx		index of bridge to query (0..n)
 *  \param bridge	ptr to CHAMELEONV2_BRIDGE struct where information
 *					 will be stored
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 BridgeIdent(
	CHAMELEONV2_HANDLE *chah,
	u_int32 idx,
	CHAMELEONV2_BRIDGE *bridge)
{
	CHAMELEONV2_HDL *h = (CHAMELEONV2_HDL*)chah;
	BRG_NODE		*brgN=NULL;

	DBGWRT_1((DBH,"CHAM - BridgeIdent: idx=%d\n", idx));

	if( !chah )
		return CHAMELEONV2_HANDLE_ILL;

	return BridgeIdentNode( h, idx, bridge, &brgN );
}

/**********************************************************************/
/** Helper function for BridgeIdent() to get the BRG_NODE
 *
 *	\param h		ptr to CHAMELEONV2_HDL
 *  \param idx		index of bridge to query (0..n)
 *  \param bridge	ptr to CHAMELEONV2_BRIDGE struct where information
 *					 will be stored
 *  \param brgNP	ptr to BRG_NODE ptr (tricky usage):
 *                   IN #1 if *brgNP=NULL: use BRG_NODE node for the specified idx
 *                   IN #2 if *brgNP=<*BRG_NODE>: use *brgNP
 *                   OUT: *brgNP points to used BRG_NODE
 *
 *  \return 0=ok (always for #2), or chameleon error code
 */
static int32 BridgeIdentNode(
	CHAMELEONV2_HDL *h,
	u_int32 idx,
	CHAMELEONV2_BRIDGE *bridge,
	BRG_NODE **brgNP )
{
	u_int32		val, i;
	BRG_NODE	*brgN;

	DBGWRT_3((DBH,"CHAM - BridgeIdentNode: idx=%d, *brgNP=%08p\n", idx, *brgNP));

	/* BRG_NODE specified? */
	if( *brgNP ){
		brgN = *brgNP;
	}
	else{
		if( idx >= h->brgList.nbr )
			return CHAMELEONV2_NO_MORE_ENTRIES;

		/* get BRG_NODE for index */
		for( brgN=(BRG_NODE*)h->brgList.l.head, i=0;
			 brgN->n.next && i<idx;
			 brgN = (BRG_NODE*)brgN->n.next, i++ );
	}

	/* 0x00 */
	val = CFGTABLE_READLONG( brgN->ma, 0 );
	bridge->interrupt = (u_int16)( val      & 0x3f);
	bridge->revision  = (u_int16)((val>> 6) & 0x3f);
	bridge->variant   = (u_int16)((val>>12) & 0x3f);
	bridge->devId     = (u_int16)((val>>18) & 0x3ff);

	/* 0x04 */
	val = CFGTABLE_READLONG( brgN->ma, 1 );
	bridge->instance  = (u_int16)((val>> 3) & 0x3f);
	bridge->dbar      = (u_int16)((val>> 9) & 0x7);
	bridge->group     = (u_int16)((val>>12) & 0x3f);

	bridge->bar     = brgN->bar;
	bridge->offset	= brgN->offset;
	bridge->size	= brgN->size;
	bridge->busId	= brgN->tblN->busId;

	/* Note: 'bridge->nextBus = brgN->destTblN->busId'
	         cannot be used, because there is no table
			 if the bus behind the bus is unplugged */
	bridge->nextBus = brgN->destBusId;

	bridge->addr = (void*)(U_INT32_OR_64)(h->ba[brgN->bar].addr + brgN->offset);

	DBGWRT_3((DBH," bridge address=%08p (= BAR%d=0x%x + offset=0x%x)\n",
		bridge->addr, brgN->bar, h->ba[brgN->bar].addr, brgN->offset));

	*brgNP = brgN;

	return 0;
}

/**********************************************************************/
/** Chameleon-V2 routine to search for a General Device unit
 *
 *  Searches the chameleon device for a unit/bridge/cpu with the
 *  specified characteristic(s). The demanded characterstics must be
 *  specified with the #CHAMELEONV2_FIND structure. The following table
 *  shows which CHAMELEONV2_FIND characteristic applies to unit/bridge/cpu:
 *
 *  \verbatim
  	characteristic  |  unit  | bridge |   cpu
  	----------------+--------+--------+--------
  	devId           |    x   |    x   |    x
  	variant         |    x   |    x   |    x
  	instance        |    x   |    x   |    x
  	busId           |    x   |    x   |    x
  	group           |    x   |    x   |    x
  	bootAddr        |    -   |    -   |    x    \endverbatim
 *
 *  A charactersitic can be ignored by specifying -1. If none
 *  characteristic is specified, all implemented units/bridges/cpus will
 *  be found.
 *
 *  With the 'flags' member of the #CHAMELEONV2_FIND structure, the search
 *  behaviour can be adjusted:\n
 *  The #CHAMELEONV2_FF_BRGALL flag causes that, besides all matching modules,
 *  all units and bridges within a bridged destination window of a matching
 *  bridge will be found, regardless if the units and bridges within the window
 *  match or match not with the specified find characteristics.
 *  Note: Only units and bridges at the next destination bus of the bridge will
 *  be considered. Units and bridges at further busses behind further bridges
 *  within a matching bridge window are not considered.
 *
 *  The function searches after a unit/bridge/cpu if a ptr to the belonging
 *  CHAMELEONV2_UNIT/BRIDGE/CPU structure is provided. If NULL is specified
 *  for \a unit, \a bridge or \a cpu this instance types will not be querried.
 *
 *  Example: \a InstanceFind( chah, idx, find, unit, NULL, cpu ) searches for
 *           units and cpus but not for bridges that match the specified
 *           characteristics.
 *
 *  The \a idx parameter is used as an consecutive index and specifies
 *  for which found unit/bridge/cpu match the information should be returned.
 *
 *  The function indicates with the return value which instance (unit/bridge/cpu)
 *  was actually found or if no (more) instances with the specified characteristic(s)
 *  and index was found.:
 *  - #CHAMELEONV2_UNIT_FOUND		: \a unit points to the found unit information
 *  - #CHAMELEONV2_BRIDGE_FOUND		: \a bridge points to the found bridge information
 *  - #CHAMELEONV2_CPU_FOUND		: \a cpu points to the found cpu information
 *  - #CHAMELEONV2_NO_MORE_ENTRIES	: no unit/bridge/cpu found
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *  For Chameleon-V0/V1 devices:
 *  - searches only for units
 *  - the variant, busId and group parameters are alway 0
 *  - see #CHAMELEONV2_UNIT struct restrictions
 *
 *	\param chah		chameleon handle returned from InitXx()
 *  \param idx		index of unit to query (0..n)
 *  \param find		#CHAMELEONV2_FIND strucure with the charateristics to search
 *  \param unit		ptr to #CHAMELEONV2_UNIT struct or NULL
 *  \param bridge	ptr to CHAMELEONV2_BRIDGE struct or NULL
 *  \param cpu		ptr to CHAMELEONV2_CPU struct or NULL
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ int32 InstanceFind(
	CHAMELEONV2_HANDLE *chah,
	int32				idx,
	CHAMELEONV2_FIND	find,
	CHAMELEONV2_UNIT	*unit,
	CHAMELEONV2_BRIDGE	*bridge,
	CHAMELEONV2_CPU		*cpu)
{
	CHAMELEONV2_HDL		*h = (CHAMELEONV2_HDL*)chah;
	u_int32				i;
	int32				error=0, match=0;
	BRG_NODE			*brgN;

	DBGWRT_2((DBH,"CHAM - InstanceFind: idx=%d, unit=%08p, bridge=%08p, cpu=%08p\n",
		idx, unit, bridge, cpu));

	if( !chah )
		return CHAMELEONV2_HANDLE_ILL;

	/* unit requested? */
	if( unit ){
		UNIT_NODE	*unitN;
		DBGWRT_2((DBH,"CHAM - InstanceFind Unit: devId=0x%03x (idx/inst/var/grp=%d/%d/%d/%d)\n",
					find.devId, idx, find.instance, find.variant, find.group ));

		/* search in all units */
		for( i=0; i<h->unitList.nbr; i++ ){

			if( (error = UnitIdentNode( chah, i, unit, &unitN )) )
				return error;

			if(
				/* matching characteristic(s)? */
				((find.devId    == -1) || (find.devId    == unit->devId)) &&
				((find.variant  == -1) || (find.variant  == unit->variant)) &&
				((find.busId	== -1) || (find.busId    == unit->busId)) &&
				((find.instance == -1) || (find.instance == unit->instance)) &&
				((find.group    == -1) || (find.group    == unit->group)) ){

				/* demanded match? */
				if( idx == match )
					return CHAMELEONV2_UNIT_FOUND;

				match++;
			}
			/* no match - check for match of prev-brg: */
			/* _FF_BRGALL flag set AND behind a bridge? */
			else if( (find.flags & CHAMELEONV2_FF_BRGALL) &&
					 unitN && /* is NULL for chaRev=0/1 */
					 (unitN->prevBrgN) ){
				
				/* fill a CHAMELEONV2_BRIDGE struct for prev-brg node */
				CHAMELEONV2_BRIDGE	bridgePrev;
				brgN = unitN->prevBrgN;
				BridgeIdentNode( h, 0, &bridgePrev, &brgN );

				/* prev-brg: matching characteristic(s)? */
				if( InstanceFindBrg(h, &find, &bridgePrev) ){

					/* demanded match? */
					if( idx == match ){
						DBGWRT_2((DBH," _FF_BRGALL flag set: unit found for prev-brg match!\n"));
						return CHAMELEONV2_UNIT_FOUND;
					}

					match++;
				}
			}
		}
	}

#ifdef SUPPORT_CHAM_V0
	if( h->chaRev == 0 || h->chaRev == 1 )
		return CHAMELEONV2_NO_MORE_ENTRIES;
#endif

	/* cpu requested? */
	if( cpu ){
		DBGWRT_2((DBH,"CHAM - InstanceFind CPU: devId=0x%03x (idx/inst/var/grp=%d/%d/%d/%d)\n",
					find.devId, idx, find.instance, find.variant, find.group ));
		/* search in all cpu */
		for( i=0; i<h->cpuList.nbr; i++ ){
			
			if( (error = CpuIdent( chah, i, cpu )) )
				return error;

			if(
				/* matching characteristic(s)? */
				((find.devId    == -1) || (find.devId    == cpu->devId)) &&
				((find.variant  == -1) || (find.variant  == cpu->variant)) &&
				((find.busId	== -1) || (find.busId	 == cpu->busId)) &&
				((find.instance == -1) || (find.instance == cpu->instance)) &&
				((find.bootAddr == -1) || (find.bootAddr == (int32)cpu->bootAddr)) &&
				((find.group    == -1) || (find.group    == cpu->group)) ){

				/* demanded match? */
				if( idx == match )
					return CHAMELEONV2_CPU_FOUND;

				match++;
			}
		}
	}

	/* bridge requested? */
	if( bridge ){
		DBGWRT_2((DBH,"CHAM - InstanceFind bridge: idx %d\n", idx ));
		/* search in all bridges */
		for( i=0; i<h->brgList.nbr; i++ ){
			
			brgN = NULL;
			if( (error = BridgeIdentNode( h, i, bridge, &brgN )) )
				return error;

			/* matching characteristic(s)? */
			if( InstanceFindBrg(h, &find, bridge) ){

				/* demanded match? */
				if( idx == match )
					return CHAMELEONV2_BRIDGE_FOUND;
				
				match++;
			}
			/* no match - check for match of prev-brg: */
			/* _FF_BRGALL flag set AND behind a bridge? */
			else if( (find.flags & CHAMELEONV2_FF_BRGALL) &&
					 (brgN->prevBrgN) ){
				
				/* fill a CHAMELEONV2_BRIDGE struct for prev-brg node */
				CHAMELEONV2_BRIDGE	bridgePrev;
				brgN = brgN->prevBrgN;
				BridgeIdentNode( h, 0, &bridgePrev, &brgN );

				/* prev-brg: matching characteristic(s)? */
				if( InstanceFindBrg(h, &find, &bridgePrev) ){

					/* demanded match? */
					if( idx == match ){
						DBGWRT_2((DBH," _FF_BRGALL flag set: bridge found for prev-brg match!\n"));
						return CHAMELEONV2_BRIDGE_FOUND;
					}
					match++;
				}
			}
		}
	}

	return CHAMELEONV2_NO_MORE_ENTRIES;
}

/**********************************************************************/
/** Helper function for InstanceFind() to find bridges
 *
 *	\param h		ptr to CHAMELEONV2_HDL
 *  \param find		#CHAMELEONV2_FIND strucure with the charateristics to search
 *  \param bridge	ptr to CHAMELEONV2_BRIDGE struct or NULL
 *
 *  \return 0=ok, or chameleon error code
 */
static int32 InstanceFindBrg(
	CHAMELEONV2_HDL		*h,
	CHAMELEONV2_FIND	*find,
	CHAMELEONV2_BRIDGE	*bridge)
{
	DBGWRT_3((DBH,"CHAM - InstanceFindBrg\n"));

	if(
		/* matching characteristic(s)? */
		((find->devId    == -1) || (find->devId    == bridge->devId)) &&
		((find->variant  == -1) || (find->variant  == bridge->variant)) &&
		((find->busId	 == -1) || (find->busId    == bridge->busId)) &&
		((find->instance == -1) || (find->instance == bridge->instance)) &&
		((find->group    == -1) || (find->group    == bridge->group)) ){

		return TRUE;
	}

	return FALSE;
}

/**********************************************************************/
/** Chameleon-V2 routine to teminate Chameleon device
 *
 *  The function destroys the #CHAMELEONV2_HANDLE handle for that device
 *  and free the memory and sets the contents of *chahP to NULL
 *
 *	\param chahP	pointer to chameleon handle returned from
 *					 InitXx()
 *
 *  <b>Compatibility\n</b>
 *  Supports Chameleon-V0/V1 and Chameleon-V2 devices.
 *
 *  \return 0=ok, or chameleon error code
 */
_STATIC_ void Term(
	CHAMELEONV2_HANDLE **chahP)
{
	CHAMELEONV2_HDL	*h;
	TBL_NODE		*tblN;
	UNIT_NODE		*unitN;
	CPU_NODE		*cpuN;
	BRG_NODE		*brgN;

	if( !chahP )
		return;

	h = (CHAMELEONV2_HDL *)*chahP;

	DBGWRT_1((DBH,"CHAM - Term: entered\n"));

	*chahP = NULL;

#ifdef SUPPORT_CHAM_V0
	if( h->chaRev == 0 || h->chaRev == 1 ){
		if( h->h0 )
			ChameleonTerm( &h->h0 );
	}
	else{
#endif

	/* unmap mapped addresses and free all table nodes */
	while( (tblN = (TBL_NODE*)OSS_DL_RemHead(&h->tblList.l)) ){
		HUnmap( h, tblN->ma );
		OSS_MemFree(h->osh,(void*)tblN, tblN->structSize);
	}

	/* free all unit nodes */
	while( (unitN = (UNIT_NODE*)OSS_DL_RemHead(&h->unitList.l)) )
		OSS_MemFree(h->osh,(void*)unitN, unitN->structSize);

	/* free all cpu nodes */
	while( (cpuN = (CPU_NODE*)OSS_DL_RemHead(&h->cpuList.l)) )
		OSS_MemFree(h->osh,(void*)cpuN, cpuN->structSize);

	/* free all bridge nodes */
	while( (brgN = (BRG_NODE*)OSS_DL_RemHead(&h->brgList.l)) )
		OSS_MemFree(h->osh,(void*)brgN, brgN->structSize);

#ifdef SUPPORT_CHAM_V0
	}
#endif

	DBGEXIT((&h->dbgHdl));

	OSS_MemFree(h->osh,(void*)h,h->structSize);
}
/*! @} */

/**********************************************************************/
/** HInit helper function
 *
 *  \param osh		OSS handle, returned by OSS_Init
 *	\param hP		pointer to variable where chameleon handle
 *					 will be stored
 *
 *  \return 0=ok, or error number
 */
static int32 HInit( OSS_HANDLE *osh, CHAMELEONV2_HDL **hP )
{
	u_int32			gotSize=0;
	CHAMELEONV2_HDL *h;

	/*-----------------------------+
	|  Create and init handle      |
	+-----------------------------*/
	if( (h = (CHAMELEONV2_HDL*)OSS_MemGet(
				osh, sizeof(CHAMELEONV2_HDL), &gotSize )) == NULL )
		return CHAMELEONV2_OUT_OF_RES;

	OSS_MemFill( osh, sizeof(CHAMELEONV2_HDL), (char*)h, 0 );

	h->structSize = gotSize;
	h->osh = osh;
	h->chaRev = UNKN_CHATBL_REV;

	/*-----------------------------+
	|  Initialize list headers     |
	+-----------------------------*/
	OSS_DL_NewList( &h->tblList.l );
	OSS_DL_NewList( &h->unitList.l );
	OSS_DL_NewList( &h->cpuList.l );
	OSS_DL_NewList( &h->brgList.l );

	*hP = h;

	return 0;
}

/**********************************************************************/
/** HEnumTbl helper function
 *
 *  Enumerates one chameleon table
 *
 *	\param h		chameleon handle
 *	\param tblAddr	address to table header
 *	\param prevBrgN	bridge that links to the table or NULL (if base table)
 *
 *  \return 0=ok, or error number
 */
static int32 HEnumTbl( CHAMELEONV2_HDL *h, void *tblAddr, BRG_NODE *prevBrgN )
{
	int32		error=0;
	MACCESS		ma;
	u_int32		dtype;
	u_int16		longOff;	/* 4-byte offset to current desc entry */
	u_int8		end, baCount;
	TBL_NODE	*tblN;
	u_int32		val, gotSize=0;
	u_int16		bar, nbr;

	static int32 recLev = 0;

#ifdef DBG
	char *spP, space[] = "                                          ";
#endif

	if( prevBrgN )
		recLev++;
	else
		recLev = 0;

#ifdef DBG
	spP = &space[(sizeof(space)-1) - (3*recLev)];
#endif

	DBGWRT_1((DBH,"%sHEnumTbl#%d : ---------------- scanning table=%08p ---------------\n",
		spP, recLev, tblAddr));

	/*----------+
	|  HEADER   |
	+----------*/
	/*
	 * Search in all table-nodes?
	 */
    for( tblN=(TBL_NODE*)h->tblList.l.head;
         tblN->n.next;
         tblN = (TBL_NODE*)tblN->n.next ){

		 /* table already scanned? (we must be behind a bridge)*/
		if( tblAddr == tblN->addr ){
			break;
		}
	}

	/* new table! */
	if( tblN->n.next == NULL ){
		if( (error=HInitTable( h, tblAddr, &tblN )) )
			goto CLEANUP;

		/* behind a bridge? */
		if( prevBrgN )
			tblN->busId = prevBrgN->destBusId;
		/* at root bus #0 */
		else
			tblN->busId = 0;
	}

	/* behind a bridge? */
	if( prevBrgN ){
		DBGWRT_2((DBH,"%sHEnumTbl#%d : table for busId=0x%04x behind bridge for DBAR=%d\n",
				spP, recLev, tblN->busId, prevBrgN->dbar));
		/* set ptr to this table for the previous bridge */
		prevBrgN->destTblN = tblN;
	}
	/* at root bus */
	else{
		DBGWRT_2((DBH,"%sHEnumTbl#%d : table for busId=0x%04x (root bus)\n",
				spP, recLev, tblN->busId));
		if( h->location == LOC_INSIDE ){

			/* search bar descriptor */
			if( (error=HSearchBarDesc( h, tblN )) )
				goto CLEANUP;
		}
	}

	/*
	 * a, FIRST SCAN: Enumerate all UNIT/CPU descriptor entries
	 */
	longOff = CHAV2_HEADER_LSIZE;
	nbr=0; end=0;
	do{
		ma = (MACCESS)((u_int32*)tblN->ma + longOff);

		val = CFGTABLE_READLONG( ma, 0 );
		dtype = (CHAV2_DTYPE_MASK & val) >> CHAV2_DTYPE_SHIFT;

		switch(dtype){

		/*---------+
		|  UNIT    |
		+---------*/
		case CHAV2_DTYPE_UNIT:
		{
			UNIT_NODE	*unitN;

			longOff += CHAV2_UNIT_LSIZE;

			/* read bar at 0x04 */
			bar = (u_int16)(CFGTABLE_READLONG( ma, 1 ) & 0x7);

			/* behind a bridge? */
			if( prevBrgN ){
				/* prev-bridge dest-bar match to unit-bar? */
				if( bar == prevBrgN->dbar ){
					DBGWRT_3((DBH,"%sHEnumTbl#%da: UNIT #%d found at tblAddr=%08p "
						" (in evaluated brg-window)\n",
						spP, recLev, h->unitList.nbr, (void*)ma));
				}
				else{
					DBGWRT_3((DBH,"%sHEnumTbl#%da: UNIT #%d found at tblAddr=%08p "
						" (bar=%d in other brg-window - skipped)\n",
						spP, recLev, h->unitList.nbr, (void*)ma, bar));
					break;
				}
			}
			else{
				DBGWRT_3((DBH,"%sHEnumTbl#%da: UNIT #%d found at tblAddr=%08p "
					" (at root bus)\n",
					spP, recLev, h->unitList.nbr, (void*)ma));
			}

			/* create and add new node */
			if( (unitN = (UNIT_NODE*)OSS_MemGet( h->osh, sizeof(UNIT_NODE),
				&gotSize )) == NULL ){
				error = CHAMELEONV2_OUT_OF_RES;
				goto CLEANUP;
			}

			OSS_DL_AddTail( &h->unitList.l, &unitN->n );

			/* init table entry */
			unitN->structSize = gotSize;
			unitN->ma = ma;
			unitN->tblN = tblN;
			unitN->prevBrgN = prevBrgN;
			h->unitList.nbr++;

			/* indicate bar of the table as used */
			tblN->usedBars |= BIT4BAR(bar);
		}
			break;

		/*---------+
		|  BRIDGE  |
		+---------*/
		case CHAV2_DTYPE_BRG:
			DBGWRT_3((DBH,"%sHEnumTbl#%da: BRIDGE #%d found at tblAddr=%08p (skipped)\n",
				spP, recLev, h->brgList.nbr, (void*)ma));
			longOff += CHAV2_BRG_LSIZE;
			break;

		/*---------+
		|  CPU     |
		+---------*/
		case CHAV2_DTYPE_CPU:
		{
			CPU_NODE	*cpuN;

			DBGWRT_3((DBH,"%sHEnumTbl#%da: CPU #%d found at tblAddr=%08p",
				spP, recLev, h->cpuList.nbr, (void*)ma));

			/* behind a bridge? */
			if( prevBrgN ){
				DBGWRT_3((DBH,"%sHEnumTbl#%da: CPU #%d found at tblAddr=%08p "
					" (in brg-window - skipped)\n",
					spP, recLev, h->cpuList.nbr, (void*)ma));
				break;
			}
			else{
				DBGWRT_3((DBH,"%sHEnumTbl#%da: CPU #%d found at tblAddr=%08p "
					" (at root bus)\n",
					spP, recLev, h->cpuList.nbr, (void*)ma));
			}

			/* create and add new node */
			if( (cpuN = (CPU_NODE*)OSS_MemGet( h->osh, sizeof(CPU_NODE),
				&gotSize )) == NULL ){
				error = CHAMELEONV2_OUT_OF_RES;
				goto CLEANUP;
			}
			
			OSS_DL_AddTail( &h->cpuList.l, &cpuN->n );

			/* init table entry */
			cpuN->structSize = gotSize;
			cpuN->ma = ma;
			cpuN->tblN = tblN;
			h->cpuList.nbr++;

			longOff += CHAV2_CPU_LSIZE;
			break;
		}

		/*---------+
		|  BAR     |
		+---------*/
		case CHAV2_DTYPE_BAR:
			DBGWRT_3((DBH,"%sHEnumTbl#%da: BAR found at tblAddr=%08p (skipped)\n",
				spP, recLev, (void*)ma));
			baCount = (u_int8)(val & 0x7);
			longOff += CHAV2_BA_LSIZE(baCount);
			break;

		/*---------+
		|  END     |
		+---------*/
		case CHAV2_DTYPE_END:
			end=1;
			DBGWRT_3((DBH,"%sHEnumTbl#%da: END found at tblAddr=%08p\n",
				spP, recLev, (void*)ma));
			break;

		default:
			DBGWRT_ERR((DBH,"%s*** HEnumTbl#%da: unsupported dtype=0x%x found at tblAddr=%08p\n",
				spP, recLev, dtype, (void*)ma));
			error = CHAMELEONV2_DTYPE_UNSUP;
			goto CLEANUP;
		}

		/* ill table? */
		if( nbr++ > 1000 ){
			error = CHAMELEONV2_TABLE_ERR;
			goto CLEANUP;
		}

	} while( !end); /* FIRST SCAN */

	/*
	 * b, SECOND SCAN: Enumerate all BRIDGE descriptor entries
	 */
	longOff = CHAV2_HEADER_LSIZE;
	nbr=0; end=0;
	do{
		ma = (MACCESS)((u_int32*)tblN->ma + longOff);

		val = CFGTABLE_READLONG( ma, 0 );
		dtype = (CHAV2_DTYPE_MASK & val) >> CHAV2_DTYPE_SHIFT;

		switch(dtype){

		/*---------+
		|  UNIT    |
		+---------*/
		case CHAV2_DTYPE_UNIT:
			DBGWRT_3((DBH,"%sHEnumTbl#%db: UNIT #%d found at tblAddr=%08p (skipped)\n",
				spP, recLev, h->unitList.nbr, (void*)ma));
			longOff += CHAV2_UNIT_LSIZE;
			break;

		/*---------+
		|  BRIDGE  |
		+---------*/
		case CHAV2_DTYPE_BRG:
		{
			BRG_NODE	*brgN;
			TBL_NODE	*scanTblN;
			void*		nextTblAddr;
			u_int32		chamOff, brgOff=0;
			u_int8		dbar, instance;
			u_int16		bar4acc;
			u_int32		offset;
			u_int32		size;

			longOff += CHAV2_BRG_LSIZE;

			/* read bar and dbar at 0x04 */
			val = CFGTABLE_READLONG( ma, 1 );
			bar = (u_int8)(val & 0x7);
			instance = (u_int8)((val>> 3) & 0x3f);
			dbar = (u_int8)((val>> 9) & 0x7);

			/* unassigned BAR? */
			if( h->ba[bar].type == -1 ){
				DBGWRT_ERR((DBH,"%s*** HEnumTbl#%db: BRIDGE #%d uses unassigned BAR=%d\n",
					spP, recLev, h->brgList.nbr, bar));
				error = CHAMELEONV2_BAR_UNASSIGNED;
				goto CLEANUP;
			}

			/* read cham-offset at 0x08 */
			chamOff = CFGTABLE_READLONG( ma, 2 );
			/* read offset at 0x0c */
			offset = CFGTABLE_READLONG( ma, 3 );
			/* read size at 0x10 */
			size = CFGTABLE_READLONG( ma, 4 );

			/* behind a bridge? */
			if( prevBrgN ){
				/* previous bridge-window to small? */
				if( size > prevBrgN->size ){
					DBGWRT_ERR((DBH,"%s*** HEnumTbl#%db: BRIDGE #%d with instance=%d: "
						"brg-size (0x%x) > prev. brg-size (0x%x)\n",
						spP, recLev, h->brgList.nbr, instance, size, prevBrgN->size ));
					error = CHAMELEONV2_TABLE_ERR;
					goto CLEANUP;
				}

				/* prev-bridge dest-bar not the bridge-bar? */
				if( bar != prevBrgN->dbar ){
					DBGWRT_3((DBH,"%sHEnumTbl#%db: BRIDGE #%d found with dbar=%d at tblAddr=%08p "
						" (bar=%d in other brg-window - skipped)\n",
						spP, recLev, h->brgList.nbr, dbar, (void*)ma, bar));
					break;
				}

				DBGWRT_2((DBH,"%sHEnumTbl#%db: BRIDGE #%d found with dbar=%d at tblAddr=%08p "
					" (in evaluated brg-window)\n",
					spP, recLev, h->brgList.nbr, dbar, (void*)ma));

				/* window of bridge is accessible via bar and offset from previous bridge */
				bar4acc = prevBrgN->bar;
				brgOff = prevBrgN->offset;
			}
			/* at root bus */
			else{
				DBGWRT_2((DBH,"%sHEnumTbl#%db: BRIDGE #%d found with dbar=%d at tblAddr=%08p "
					" (at root bus)\n",
					spP, recLev, h->brgList.nbr, dbar, (void*)ma));

				bar4acc = bar;
			}

			/*
			 * Compute table address at bus behind bridge
			 * Note: If bridge behind a bridge:
			 *       - use bar of prev. bridge
			 *       - consider offset from prev. bridge
			 */
			nextTblAddr = (void*)(U_INT32_OR_64)(h->ba[bar4acc].addr + brgOff + chamOff);
			DBGWRT_2((DBH,"%sHEnumTbl#%db: next table address=%08p "
				"(= BAR%d=0x%x + brgOffs=0x%x + chamOffs=0x%x)\n",
				spP, recLev, nextTblAddr, bar4acc, h->ba[bar4acc].addr, brgOff, chamOff));

			/* Search in all existing table-nodes */
			for( scanTblN=(TBL_NODE*)h->tblList.l.head;
				 scanTblN->n.next;
				 scanTblN = (TBL_NODE*)scanTblN->n.next ){

				/* table behind bridge already scanned? */
				 if( nextTblAddr == scanTblN->addr )			
					break; /* leave for-loop */
			}

			/* table behind bridge already scanned? */
			if( scanTblN->n.next ){
				/* for destination BAR? */
				if( scanTblN->usedBars & BIT4BAR(dbar) ){
					DBGWRT_2((DBH,"%sHEnumTbl#%db: table at tblAddr=%08p behind bridge for DBAR=%d already "
						"considered, skip repeated enum\n",
						spP, recLev, nextTblAddr, dbar));
					break; /* leave switch-case */
				}
			}
			/* table behind bridge not yet scanned */
			else{
				/* increment count of bridged busses */
				tblN->brgBusCount++;

				/* Only 0xe bridges per bus allowed because the busId (0x0000..0xeeee)
				   must fit into a signed int16 variable */
				if( tblN->brgBusCount > 0xe ){
					DBGWRT_ERR((DBH,"%s*** HEnumTbl#%db: more than 0xe bridges per bus not supported\n",
						spP, recLev ));
					error = CHAMELEONV2_TABLE_ERR;
					goto CLEANUP;
				}
			}

			/* Only four bridges may be cascaded because the busId (0x0000..0xeeee)
			   must fit into a signed int16 variable. */
			if( recLev+1 > 4 ){
				DBGWRT_ERR((DBH,"%s*** HEnumTbl#%db: more than four bridges cascaded (not supported)\n",
					spP, recLev ));
				error = CHAMELEONV2_TABLE_ERR;
				goto CLEANUP;
			}

			/* create and add new node */
			if( (brgN = (BRG_NODE*)OSS_MemGet( h->osh, sizeof(BRG_NODE),
				&gotSize )) == NULL ){
				error = CHAMELEONV2_OUT_OF_RES;
				goto CLEANUP;
			}

			OSS_DL_AddTail( &h->brgList.l, &brgN->n );

			/* init table entry */
			brgN->structSize = gotSize;
			brgN->ma = ma;
			brgN->offset = offset + brgOff; /* If prev brg: consider offset of prev brg */
			brgN->size = size;
			brgN->bar = (u_int8)bar4acc; /* If prev brg: use bar of prev brg */
			brgN->dbar = dbar;
			brgN->tblN = tblN;
			brgN->prevBrgN = prevBrgN;
			brgN->destTblN = NULL; /* not yet known, will be set later */

			/*
			 * Compute busId for destination bus, e.g.:
			 *  busId of bus where bridge resides = 0x1000
			 *  count of bridged busses to previous bus = 3
			 *  recLev = 2  =>  0x0100
			 *  busId = 0x1000 | (3 * 0x0100) = 0x1300
			 */
			brgN->destBusId = (u_int16)( brgN->tblN->busId |
				brgN->tblN->brgBusCount * (0x1000>>(4*recLev)));

			h->brgList.nbr++;

			/* indicate bar of the table as used */
			tblN->usedBars |= BIT4BAR(bar);

			/*
			 * Enum table of bus behind the bridge for DBAR (recursive call)
			 */
			error = HEnumTbl( h, nextTblAddr, brgN );
			/* the table behind a bridge may be unavailable
			   if the destination bus if unplugged, in this case we return no error */
			if( error && (error != CHAMELEONV2_TABLE_NOT_FOUND) )
				goto CLEANUP;

			break;
		} /* case CHAV2_DTYPE_BRG */

		/*---------+
		|  CPU     |
		+---------*/
		case CHAV2_DTYPE_CPU:
			DBGWRT_3((DBH,"%sHEnumTbl#%db: CPU #%d found at tblAddr=%08p (skipped)\n",
				spP, recLev, h->cpuList.nbr, (void*)ma));
			longOff += CHAV2_CPU_LSIZE;
			break;

		/*---------+
		|  BAR     |
		+---------*/
		case CHAV2_DTYPE_BAR:
			DBGWRT_3((DBH,"%sHEnumTbl#%db: BAR found at tblAddr=%08p (skipped)\n",
				spP, recLev, (void*)ma));
			baCount = (u_int8)(val & 0x7);
			longOff += CHAV2_BA_LSIZE(baCount);
			break;

		/*---------+
		|  END     |
		+---------*/
		case CHAV2_DTYPE_END:
			end=1;
			DBGWRT_3((DBH,"%sHEnumTbl#%db: END found at tblAddr=%08p\n",
				spP, recLev, (void*)ma));
			break;

		default:
			DBGWRT_ERR((DBH,"%s*** HEnumTbl#%db: unsupported dtype=0x%x found at tblAddr=%08p\n",
				spP, recLev, dtype, (void*)ma));
			error = CHAMELEONV2_DTYPE_UNSUP;
			goto CLEANUP;
		}

		/* ill table? */
		if( nbr++ > 1000 ){
			error = CHAMELEONV2_TABLE_ERR;
			goto CLEANUP;
		}

	} while( !end); /* SECOND SCAN */

	error = 0;

CLEANUP:
	DBGWRT_2((DBH,"%sHEnumTbl#%d : -------------- leaving with ret=0x%x ---------------\n",
		spP, recLev, error));

#ifdef DBG
	if( recLev == 0 ){
		u_int16				dbgNbr=0;
		CHAMELEONV2_UNIT 	unit;
	
		DBGWRT_1((DBH, "%s Unit                devId   Grp Rev  Inst\tIRQ\tBAR Offset   Addr\n",spP));
		
		while ( (0 == UnitIdent( h, dbgNbr, &unit )) ){
			DBGWRT_1((DBH, " %02d %-17s"  		/* idx + name		*/
			               "0x%04x %2d   %2d"	/* devId/Group/Rev. */
	                       "   0x%02x"			/* instance 		*/
	                       "\t0x%02x"			/* interrupt 		*/
	                       "\t%d   0x%04x"		/* BAR / offset  	*/
	                       "   %p\n",			/* addr 			*/
			   dbgNbr, CHAM_DevIdToName( unit.devId), unit.devId, unit.group, unit.revision, unit.instance,
			   unit.interrupt, unit.bar, (unsigned int)unit.offset, unit.addr));
			dbgNbr++;
		}
	}
#endif /* DBG */

	recLev--;

	return error;
}

/**********************************************************************/
/** HSearchBarDesc helper function
 *
 *	\param h		chameleon handle
 *	\param tblAddr	table address
 *  \param maP		ptr to variable where created table node will be stored
 *
 *  \return 0=ok, or error number
 */
static int32 HSearchBarDesc( CHAMELEONV2_HDL *h, TBL_NODE	*tblN )
{
	u_int8		end, baCount, i;
	MACCESS		ma;
	u_int16		longOff=CHAV2_HEADER_LSIZE;	/* 4-byte offset to first desc entry */
	u_int32		val, dtype, barVal;
	u_int16 	nbr;

	/*
	 * Search for BAR-desc before enumeration
	 */
	nbr=0; end=0; baCount=0;
	do{
		ma = (MACCESS)((u_int32*)tblN->ma + longOff);

		val = CFGTABLE_READLONG( ma, 0 );
		dtype = (CHAV2_DTYPE_MASK & val) >> CHAV2_DTYPE_SHIFT;

		switch(dtype){
		case CHAV2_DTYPE_UNIT: 	longOff += CHAV2_UNIT_LSIZE;	break;
		case CHAV2_DTYPE_BRG: 	longOff += CHAV2_BRG_LSIZE;		break;
		case CHAV2_DTYPE_CPU: 	longOff += CHAV2_CPU_LSIZE;		break;
		case CHAV2_DTYPE_END:	end = 1;						break;
		case CHAV2_DTYPE_BAR:	end = 1;

			baCount = (u_int8)(val & 0x7);
			longOff += CHAV2_BA_LSIZE(baCount);

			DBGWRT_1((DBH,"HSearchBarDesc: BAR-desc found at tblAddr=%08p for root bus\n",
				(void*)ma));

			/* save base addresses */
			for( i=0; i<NBR_OF_BARS; i++ ){

				/* base address specified in BAR descriptor? */
				if( i<baCount ){
					/* read ba-addr at 0x04, 0x0c, ... */
					barVal = CFGTABLE_READLONG( ma, 1 + (i*2) );
					h->ba[i].addr = barVal & 0xfffffffe;
					/* read ba-size at 0x08, 0x10, ... */
					h->ba[i].size = CFGTABLE_READLONG( ma, 2 + (i*2) );
					h->ba[i].type = barVal & 0x00000001; /* 0=mem, 1=io */

					DBGWRT_3((DBH," BAR%d: addr=0x%x, size=0x%x, %s mapped\n",
						i, h->ba[i].addr, h->ba[i].size,
						h->ba[i].type == 0 ? "mem" : "io"));
				}
				/* bar unassigned! */
				else{
					 h->ba[i].type = -1;	/* unused */
					DBGWRT_2((DBH," BAR%d unassigned\n", i));
				}
			}
			break;

		default:
			DBGWRT_ERR((DBH,"*** HSearchBarDesc: unknown entry 0x%x found at tblAddr=%08p\n",
				dtype, (void*)ma));
			return CHAMELEONV2_DTYPE_UNSUP;
		} /* switch */

		/* ill table? */
		if( nbr++ > 1000 )
			return CHAMELEONV2_TABLE_ERR;

	} while( !end );

	if( baCount == 0 ){
		DBGWRT_ERR((DBH,"*** HSearchBarDesc: BAR-desc for root bus missing!\n"));
		return CHAMELEONV2_BARDESC_MISSING;
	}

	return 0;
}

/**********************************************************************/
/** HInitTable helper function
 *
 *	\param h		chameleon handle
 *	\param tblAddr	table address
 *  \param maP		ptr to variable where created table node will be stored
 *
 *  \return 0=ok, or error number
 */
static int32 HInitTable( CHAMELEONV2_HDL *h, void *tblAddr, TBL_NODE **tblNP )
{
	int32		error=0;
	MACCESS		ma;
	u_int8		chaRev;
	TBL_NODE	*tblN;
	u_int32		gotSize=0;

	*tblNP = NULL;

	/* map table space */
	if( (error=HMap( h, tblAddr, &ma )) )
		return error;

	/* check table */
	if( (error = HCheckTable( h, ma, &chaRev )) )
		goto CLEANUP;

	if( h->chaRev == UNKN_CHATBL_REV ){
		 h->chaRev = chaRev;
	}
	else{
		/* all tables of one table tree must have the same rev. */
		if( chaRev != h->chaRev ){
			DBGWRT_ERR((DBH,"*** HInitTable: different table revision (base-tbl: 0x%x, "
				"this-tbl: 0x%x)\n", h->chaRev, chaRev));
			error = CHAMELEONV2_TABLE_ERR;
			goto CLEANUP;
		}
	}

	/* create and add new node */
	if( (tblN = (TBL_NODE*)OSS_MemGet( h->osh, sizeof(TBL_NODE),
			&gotSize )) == NULL ){
		error = CHAMELEONV2_OUT_OF_RES;
		goto CLEANUP;
	}
	OSS_DL_AddTail( &h->tblList.l, &tblN->n );

	/* init table entry */
	tblN->structSize = gotSize;
	tblN->addr = tblAddr;
	tblN->ma = ma;
	tblN->usedBars = 0;
	tblN->brgBusCount = 0;
	h->tblList.nbr++;
	*tblNP = tblN;

CLEANUP:
	if( error )
		HUnmap( h, ma );

	return error;
}

/**********************************************************************/
/** HMap helper function
 *
 *	\param h	chameleon handle
 *	\param addr	address to map
 *  \param maP	ptr to variable where mapped address will be stored
 *
 *  \return 0=ok, or error number
 */
static int32 HMap( CHAMELEONV2_HDL *h, void *addr, MACCESS *maP )
{
#ifdef MAP_REQUIRED
	/* map table space */
	if( ( OSS_MapPhysToVirtAddr(
			  h->osh, addr,
		#ifdef MAC_IO_MAPPED
			   PCI_BAR_IO_MAXSIZE,
			   OSS_ADDRSPACE_IO,
		#else
			   PCI_BAR_MEM_MINSIZE,
			   OSS_ADDRSPACE_MEM,
		#endif
			  (h->location == LOC_PCI) ?
				OSS_BUSTYPE_PCI : OSS_BUSTYPE_NONE,
			  (h->location == LOC_PCI) ?
				h->pciLoc.bus : 0,
			  (void**)maP ) != 0 ) ) {
		return CHAMELEONV2_OUT_OF_RES;
	}
#else
	*maP = (MACCESS)addr;
#endif
	return 0;
}

/**********************************************************************/
/** HUnmap helper function
 *
 *	\param h	chameleon handle
 *  \param ma	mapped address to unmap
 *
 *  \return 0=ok, or error number
 */
static void HUnmap( CHAMELEONV2_HDL *h, MACCESS ma )
{
	#ifdef MAP_REQUIRED
	/* unmap table space */
	OSS_UnMapVirtAddr(
			h->osh, (void**)&ma,
		#ifdef MAC_IO_MAPPED
			PCI_BAR_IO_MAXSIZE,
			OSS_ADDRSPACE_IO );
		#else
			PCI_BAR_MEM_MINSIZE,
			OSS_ADDRSPACE_MEM );
		#endif
	#endif
}

/**********************************************************************/
/** HCheckTable helper function
 *
 *	\param h		chameleon handle
 *	\param ma		address to table header
 *  \param chaRev	ptr to variable where table revision will be stored
 *
 *  \return 0, or error code
 */
static int32 HCheckTable(
	CHAMELEONV2_HDL *h,
	MACCESS			ma,
	u_int8			*chaRev )
{
	u_int32 magicWord;
	*chaRev = UNKN_CHATBL_REV;

	/*--------------------------------------------+
	|  Check if config table contains sync word   |
	+--------------------------------------------*/

	magicWord = CFGTABLE_READWORD( ma, 1 );
	DBGWRT_1((DBH," HCheckTable: magic word = 0x%04x\n", magicWord));
	
	switch( magicWord ){
		case CHAMELEON_MAGIC_00:
#ifdef SUPPORT_CHAM_V0
			*chaRev = 0;
			DBGWRT_1((DBH,"HCheckTable: V0 table found at tblAddr=%08p\n", (void*)ma));
#else
			DBGWRT_ERR((DBH,"*** HCheckTable: V0 table found at tblAddr=%08p "
				"- not supported!\n", (void*)ma));
			return CHAMELEONV2_TABLE_REV_UNSUP;

#endif
			break;
		case CHAMELEON_MAGIC_01:
#ifdef SUPPORT_CHAM_V0
			*chaRev = 1;
			DBGWRT_1((DBH,"HCheckTable: V1 table found at tblAddr=%08p\n",(void*)ma));
#else
			DBGWRT_ERR((DBH,"*** HCheckTable: V1 table found at tblAddr=%08p "
				"- not supported!\n", (void*)ma));
			return CHAMELEONV2_TABLE_REV_UNSUP;
#endif
			break;
		case CHAMELEON_MAGIC_02:
			DBGWRT_1((DBH,"HCheckTable: V2 table found at tblAddr=%08p\n", (void*)ma));
			*chaRev = 2;
			break;
		default:
			DBGWRT_ERR((DBH,"*** HCheckTable: ill HEADER at tblAddr=%08p, magicWord=0x%08x\n",
				(void*)ma, magicWord ));
			return CHAMELEONV2_TABLE_NOT_FOUND;
	}

	return 0;
}





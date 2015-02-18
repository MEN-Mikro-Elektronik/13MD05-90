/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_d201.c
 *      Project: D201 board handler
 *
 *       Author: dieter.pfeuffer@men.de
 *        $Date: 2011/05/19 17:24:42 $
 *    $Revision: 2.21 $
 *
 *  Description: D201 board handler routines
 *
 *  The D201 board handler provides driver variants for the
 *  following carrier boards:
 *         - D201
 *         - F201 / F202
 *         - C203 / C204
 *
 *  To locate the PCI bus number on which the D201 resides, the user can
 *  specify PCI_BUS_PATH or PCI_BUS_NUMBER. PCI_BUS_PATH specifies an
 *  exact geographical location in the system, regardless which PCI
 *  devices are present in the system, so PCI_BUS_PATH is preferrable.
 *
 *  It is also possible to specify the key PCI_BUS_NUMBER in the descriptor.
 *  If present, it overrides PCI_BUS_PATH. But note that the bus number may
 *  change if additional devices are installed in the PCI system.
 *
 *  PCI_BUS_PATH is a list of PCI device numbers, that must belong to
 *  PCI-to-PCI bridges.
 *
 *  Example: To specify the bus of the MEN D002 Compact-PCI bus, carrier board
 *			 the user has to specify the following path:
 *
 *			 PCI_BUS_PATH = BINARY 0x08
 *
 *			 0x08 is the device number on Bus 0 of the PCI-to-PCI bridge
 *			 on the D2, that connects the internal PCI bus to the CPCI
 *			 backplane.
 *
 *
 *  -----------------------
 *  PLX BAR0 bug workaround: The PLX 9050 chip has a bug that might occur
 *  depending on the mapping of BAR0 applied by the BIOS. If Bit7 in BAR0
 *  is set (an address of xxxxxx80), the PLX chip's runtime registers will
 *  not work.
 *
 *  The workaround will be automatically enabled depending of the setting
 *  bit 7 in BAR0. If bit 7 in BAR0 is set, the work around will be enabled
 *
 *  When workaround is enabled, the driver will ignore the BAR0 value set
 *  by the BIOS. Instead, every time the runtime registers need to be accessed,
 *  the driver will write the value of one of BAR2..5 + 0xffff00 into BAR0
 *	and disables BAR2..5. During this time of course BAR2..5 cannot be
 *	accessed. This is not a big problem, since the runtime registers will
 *	be accessed only during BrdInit and SetMiface.
 *
 *	When workaround is enabled, getstats M_BB_BLK_ID_xx and
 *	D201_BLK_EEPROM_DATA are disabled (they return ERR_BBIS_EEPROM)
 *
 *
 *     Required: -
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_, _D201. _F201, _F202, _C203, _C204
 *				 D201_VARIANT=xxx, OSS_HAS_UNASSIGN_RESOURCES
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_d201.c,v $
 * Revision 2.21  2011/05/19 17:24:42  CRuff
 * R: 1. support of pci domains
 * M: 1a) added handling of code BBIS_CFGINFO_PCI_DOMAIN to CHAMELEON_CfgInfo()
 *    1b) added new member domainNbr to BBIS_HANDLE
 *    1c) extend debug prints of pci bus number to also print the domain number
 *    1d) call all occurrences of OSS_PciXetConfig (and other OSS calls)
 *        with merged bus and domain
 *    1e) in ParsePciPath(): look for root device on all busses instead of
 *        expecting root device on bus 0
 *
 * Revision 2.20  2010/01/27 12:13:22  ufranke
 * R: driver revision 2.19 init does'nt work always
 * M: fixed bridge header type mask
 *
 * Revision 2.19  2009/08/05 17:32:24  CRuff
 * R:1. Porting to MDIS5 (according porting guide rev. 0.7)
 * M:1.a) added support for 64bit (API)
 *     b) adapted DBG prints for 64bit pointers
 *     c) support multifunction bridges in PCI bus path
 *
 * Revision 2.18  2009/05/26 14:50:53  ufranke
 * R: compile time error for E500 PPC with mac_mem.h 1.18
 * M: fixed
 *
 * Revision 2.17  2008/09/22 13:55:36  ufranke
 * R: if XXXX_Init() fails not enough debug information
 * M: added PCI bus/devices at error
 *
 * Revision 2.16  2008/09/09 15:57:54  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 2.15  2007/02/23 18:01:00  DPfeuffer
 * dbg.h must be included after oss.h for W2k
 *
 * Revision 2.14  2006/12/20 12:21:22  ufranke
 * fixed
 *  - function pointer setIrqHandle should be NULL if not implemented
 *    or must be implemented completely
 *
 * Revision 2.13  2006/12/15 15:44:24  ts
 * replaced fkt13 with setIrqHandle
 *
 * Revision 2.12  2006/07/21 09:21:21  ufranke
 * cosmetics
 *
 * Revision 2.11  2004/06/21 10:24:58  dpfeuffer
 * BBIS_BRDINFO_BRDNAME changed
 *
 * Revision 2.10  2004/03/10 17:31:26  dpfeuffer
 * D201_BrdInfo(BBIS_BRDINFO_BRDNAME): implemented
 * D201_CfgInfo(BBIS_CFGINFO_SLOT): implemented
 * descriptor keys MISC_SETMASK/MISC_CLRMASK added
 *
 * Revision 2.9  2001/12/05 14:51:49  Franke
 * added DBG message
 *
 * Revision 2.8  2001/11/14 16:51:41  Schmidt
 * GetEntry function renamed to __D201_GetEntry for D201D302 BBIS
 *
 * Revision 2.7  2001/06/20 16:10:59  Schmidt
 * 1) PLX BAR0 bug workaround for F201, C203, C204 carrier board modified
 *    accordingly different eeprom configurations
 * 2) D201_IrqSrvInit: 'get and print irq info' now only in debug driver
 *
 * Revision 2.6  2001/01/11 12:16:28  Schmidt
 * 1) PLX BAR0 bug workaround added
 * 2) Cleanup: OSS_UnAssignResources added -
 *    depends on define OSS_HAS_UNASSIGN_RESOURCES
 *
 * Revision 2.5  2000/09/01 15:10:50  Schmidt
 * D201_BrdInfo(BBIS_BRDINFO_FUNCTION): Bug fix - break was missing
 *
 * Revision 2.4  2000/08/31 14:40:17  Schmidt
 * 1) D201_BrdInfo: Bug fix - BBIS_BRDINFO_FUNCTION was wrong
 * 2) D201_ClrMIface: Set data mode default D16 removed (problems after D32)
 * 3) D201_Init: default for PCI_CHECK_LOCATION set to 0
 *
 * Revision 2.3  2000/08/09 14:31:18  kp
 * Replaced ULONG casts with u_int32
 *
 * Revision 2.2  2000/07/06 13:30:26  Franke
 * added debug displays pci bus and device number
 *
 * Revision 2.1  2000/04/06 16:39:47  Schmidt
 * cosmetics
 *
 * Revision 2.0  2000/03/16 16:16:59  kp
 * Changed preferred method to specify pci bus number to PCI_BUS_PATH
 * Bug fix: If bad device ID detected, no error was returned
 *
 * Revision 1.20  2000/03/09 12:18:51  kp
 * GetEntry function now prefixed by D201_GLOBNAME macro
 *
 * Revision 1.19  2000/03/01 14:56:41  kp
 * EepromReadBuf was a global symbol. Renamed this variant specific
 * Removed all global (static variables). Sustituted with defines
 * Made PLD data const array
 *
 * Revision 1.18  2000/02/29 16:35:46  Franke
 * changed for big endian PLDCB_GetCfgdone(), PLDCB_GetStatus()
 *
 * Revision 1.17  1999/11/03 10:13:40  Gromann
 * added PCI_DEVICE_ID descriptor key
 *
 * Revision 1.16  1999/09/08 09:29:50  Schmidt
 * all new written, old history entries 1.2..1.15 removed
 *
 * Revision 1.1  1998/02/23 17:38:54  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: bb_d201.c,v 2.21 2011/05/19 17:24:42 CRuff Exp $";

#define _NO_BBIS_HANDLE		/* bb_defs.h: don't define BBIS_HANDLE struct */

#ifdef MAC_BYTESWAP
# define PLD_SW
# define ID_SW
#endif

#include <MEN/mdis_com.h>
#include <MEN/men_typs.h>   /* system dependend definitions	*/
#include <MEN/oss.h>        /* oss functions                */
#include <MEN/dbg.h>        /* debug functions              */
#include <MEN/desc.h>       /* descriptor functions         */
#include <MEN/bb_defs.h>    /* bbis definitions				*/
#include <MEN/mdis_err.h>   /* MDIS error codes             */
#include <MEN/mdis_api.h>   /* MDIS global defs             */
#include <MEN/maccess.h>	/* maccess macros				*/
#include <MEN/pld_load.h>	/* PLD loader					*/
#include <MEN/pci9050.h>	/* PCI9050 defines				*/
#include <MEN/modcom.h>     /* id prom functions            */
#include "bb_d201e.h"		/* Eeprom functions				*/
#include "d201.h"			/* BBIS specific defines		*/
#include "brd.h"			/* Board specific defines		*/

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL		brdHdl->debugLevel
#define DBH             brdHdl->debugHdl

#define D201_BBIS_MAX_PATH			16		/* max number of bridges to devices */
#define PCI_SECONDARY_BUS_NUMBER	0x19	/* PCI bridge config */

/* enable LINT1:module, LINT2:timeout, LINT:all */
#define INT_ENABLE( brdHdl ) { PlxBugWorkaroundStart( brdHdl, 0 );		\
	MSETMASK_D32( brdHdl->virtRRaddr, RR_INTCTL, INTCTL_LINT1_ENABLE |	\
				  INTCTL_LINT2_ENABLE | INTCTL_LINT_ENABLE );			\
	PlxBugWorkaroundEnd( brdHdl, 0 ); }

/* disable LINT1:module, LINT2:timeout, LINT:all */
#define INT_DISABLE( brdHdl ) { PlxBugWorkaroundStart( brdHdl, 0 );		\
	MCLRMASK_D32( brdHdl->virtRRaddr, RR_INTCTL, INTCTL_LINT1_ENABLE |	\
				  INTCTL_LINT2_ENABLE | INTCTL_LINT_ENABLE );			\
	PlxBugWorkaroundEnd( brdHdl, 0 ); }

/*-----------------------------------------+
|    TYPEDEFS                              |
+-----------------------------------------*/
typedef struct {
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table					*/
    OSS_HANDLE* osHdl;				/* os specific handle					*/
    DESC_HANDLE *descHdl;			/* descriptor handle					*/
	DBG_HANDLE  *debugHdl;			/* debug handle							*/
    int32       ownMemSize;			/* own memory size                      */
    void        *virtRRaddr;		/* virt runtime register addr			*/
    void        *virtModCtrlAddr[BRD_SLOT_NBR]; /* virt module ctrl reg 	*/
	void		*physRRaddr;		/* physical runtime register addr       */
	u_int32		barValue[6];		/* org. value of BAR0..5 */
    void        *physModAddr[BRD_SLOT_NBR];		/* physical module addr		*/
	int			resourcesAssigned;  /* flag resources assigned  */
    u_int32     debugLevel;			/* debug level for BBIS                 */
    u_int32	    domainNbr;          /* domain number                        */
    u_int32     busNbr;				/* bus number							*/
    u_int32     mechSlot;			/* mechanical slot                      */
    u_int32     pciDev;				/* PCI device number					*/
    u_int32     checkLoc;			/* flag for location check				*/
    u_int32     useId;				/* flag for board Id compare			*/
	u_int32		pldLoad;			/* flag for PLD load					*/
    u_int32     noneA24;			/* flag to use none A24 address space	*/
    u_int32     miscSetMask;		/* bits to set in the Misc. Ctrl. Reg.  */
    u_int32     miscClrMask;		/* bits to clr in the Misc. Ctrl. Reg.  */
    u_int32     expIrqCount;		/* count of exception interrupts		*/
	int32		irqLevel;			/* interrupt level						*/
	int32		irqVector;			/* interrupt vector						*/
	u_int8		pciPath[D201_BBIS_MAX_PATH]; /* PCI path from desc */
	u_int32		pciPathLen;			/* number of bytes in pciPath */
	u_int32		plxBugWa;			/* PLX bug workaround enabled 			*/
	void		*virtRRaddr2;		/* if PLX bug workaround enabled:
									   contains the virtual address of the
									   original BAR 0 */
} BBIS_HANDLE;

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>	/* bbis jumptable			*/
#include <MEN/bb_d201.h>	/* D201 bbis header file	*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* init/exit */
static int32 D201_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 D201_BrdInit(BBIS_HANDLE*);
static int32 D201_BrdExit(BBIS_HANDLE*);
static int32 D201_Exit(BBIS_HANDLE**);
/* info */
static int32 D201_BrdInfo(u_int32, ...);
static int32 D201_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 D201_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 D201_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  D201_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 D201_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 D201_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 D201_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 D201_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 D201_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 D201_SetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64);
static int32 D201_GetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64*);
/* unused */
static int32 D201_Unused(void);
/* miscellaneous */
static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);
static int32  EepromInfo(BBIS_HANDLE*, u_int32, u_int32, void*);
/* callback functions for PLD module */
static void   PLDCB_MsecDelay(void *arg, u_int32 msec);
static u_int8 PLDCB_GetStatus(void *arg);
static u_int8 PLDCB_GetCfgdone(void *arg);
static void   PLDCB_SetData(void *arg, u_int8 state);
static void   PLDCB_SetDclk(void *arg, u_int8 state);
static void   PLDCB_SetConfig(void *arg, u_int8 state);

static int32 ParsePciPath( BBIS_HANDLE *h, u_int32 *pciBusNbrP );
static int32 PciParseDev(
	BBIS_HANDLE *h,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	int32 *vendorIDP,
	int32 *deviceIDP,
	int32 *headTypeP,
	int32 *secondBusP);
static int32 PciCfgErr(
	BBIS_HANDLE *h,
	char *funcName,
	int32 error,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	u_int32 reg );
static void PlxBugWorkaroundStart( BBIS_HANDLE *brdHdl, int32 slot );
static void PlxBugWorkaroundEnd( BBIS_HANDLE *brdHdl, int32 slot );
static int32 CfgInfoSlot( BBIS_HANDLE *brdHdl, va_list argptr );

/**************************** D201_GetEntry *********************************
 *
 *  Description:  Initialize drivers jump table.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bbisP     pointer to the not initialized structure
 *  Output.....:  *bbisP    initialized structure
 *  Globals....:  ---
 ****************************************************************************/
#ifdef _ONE_NAMESPACE_PER_DRIVER_
	extern void BBIS_GetEntry( BBIS_ENTRY *bbisP )
#else
    extern void __D201_GetEntry( BBIS_ENTRY *bbisP )
#endif
{
    /* init/exit */
    bbisP->init         =   D201_Init;
    bbisP->brdInit      =   D201_BrdInit;
    bbisP->brdExit      =   D201_BrdExit;
    bbisP->exit         =   D201_Exit;
    bbisP->fkt04        =   D201_Unused;
    /* info */
    bbisP->brdInfo      =   D201_BrdInfo;
    bbisP->cfgInfo      =   D201_CfgInfo;
    bbisP->fkt07        =   D201_Unused;
    bbisP->fkt08        =   D201_Unused;
    bbisP->fkt09        =   D201_Unused;
    /* interrupt handling */
    bbisP->irqEnable    =   D201_IrqEnable;
    bbisP->irqSrvInit   =   D201_IrqSrvInit;
    bbisP->irqSrvExit   =   D201_IrqSrvExit;
    bbisP->setIrqHandle =   NULL;
    bbisP->fkt14        =   D201_Unused;
    /* exception handling */
    bbisP->expEnable    =   D201_ExpEnable;
    bbisP->expSrv       =   D201_ExpSrv;
    bbisP->fkt17        =   D201_Unused;
    bbisP->fkt18        =   D201_Unused;
    bbisP->fkt19        =   D201_Unused;
    /* */
    bbisP->fkt20        =   D201_Unused;
    bbisP->fkt21        =   D201_Unused;
    bbisP->fkt22        =   D201_Unused;
    bbisP->fkt23        =   D201_Unused;
    bbisP->fkt24        =   D201_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   D201_SetStat;
    bbisP->getStat      =   D201_GetStat;
    bbisP->setMIface    =   D201_SetMIface;
    bbisP->clrMIface    =   D201_ClrMIface;
    bbisP->getMAddr     =   D201_GetMAddr;
    bbisP->fkt30        =   D201_Unused;
    bbisP->fkt31        =   D201_Unused;
}

/****************************** D201_Init ***********************************
 *
 *  Description:  BBIS initialization:
 *
 *                - initializes board structure
 *                - init id function table
 *                - prepare debugging
 *                - scan descriptor
 *                - identify PCI9050 device
 *                - get base addresses
 *                - map used base addresses
 *                - get interrupt information
 *
 *                The following descriptor keys are used:
 *
 *                Deskriptor key        Default            Range
 *                --------------------  -----------------  -------------
 *                DEBUG_LEVEL_DESC      OSS_DBG_DEFAULT    see dbg.h
 *                DEBUG_LEVEL           OSS_DBG_DEFAULT    see dbg.h
 *                PCI_BUS_PATH
 *                PCI_BUS_NUMBER        none               0...255
 *                PCI_DOMAIN_NUMBER     0                  0...255
 *                PCI_BUS_SLOT          none               1...8
 *                PCI_DEVICE_ID         0xFFFF			   0...31
 *                PCI_CHECK_LOCATION    0                  0, 1
 *                ID_CHECK              1                  0, 1
 *                PLD_LOAD              1                  0, 1
 *                NONE_A24              0                  0, 1
 *                MISC_SETMASK          0x0                0x0..0xffffffff
 *                MISC_CLRMASK          0x0                0x0..0xffffffff
 *
 *                Either PCI_BUS_NUMBER or PCI_BUS_PATH must be specified
 *				  If both are present, PCI_BUS_NUMBER takes precedence.
 *
 *                NONE_A24 specifies if the Carrier Board requests only an
 *                A08 address space per module slot from the BIOS.
 *                Normally each board requests an A24 memory address space
 *                per module slot to support A08 and A24 M-Modules.
 *                However, this depends on the boards eeprom configuration.
 *
 *                  0 - A08/A24 address space (normal eeprom configuration)
 *                  1 - A08     address space (requires special eeprom data)
 *
 *                MISC_SETMASK/MISC_CLRMASK sets/clears specified bits in the
 *                Misc. Control register. ONLY FOR TEST & DEBUGGING !!!
 *
 *				  PCI_DEVICE_ID overwrites PCI_BUS_SLOT. The device id can set
 *				  directly.
 *
 *---------------------------------------------------------------------------
 *  Input......:  osHdl     pointer to os specific structure
 *                descSpec  pointer to os specific descriptor specifier
 *                brdHdlP   pointer to not initialized board handle structure
 *  Output.....:  *brdHdlP  initialized board handle structure
 *				  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_Init(
    OSS_HANDLE      *osHdl,
    DESC_SPEC       *descSpec,
    BBIS_HANDLE     **brdHdlP )
{
	u_int32		gotsize, value,	i, j;
    u_int32		venId, devId, subVenId, subDevId;
    int32		status;

    BBIS_HANDLE		*brdHdl = NULL;
    OSS_RESOURCES	res[D201_RES_NBR];

    /*------------------------------+
    | initialize board structure    |
    +------------------------------*/
	/* alloc */
	if((*brdHdlP = brdHdl = (BBIS_HANDLE*)
		OSS_MemGet(osHdl, sizeof(BBIS_HANDLE), &gotsize)) == NULL)
        return ERR_OSS_MEM_ALLOC;

	/* clear */
    OSS_MemFill(osHdl, gotsize, (char*)brdHdl, 0x00);

	/* init */
    brdHdl->ownMemSize = gotsize;
    brdHdl->osHdl = osHdl;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* drivers ident function */
	brdHdl->idFuncTbl.idCall[0].identCall = Ident;
	brdHdl->idFuncTbl.idCall[1].identCall = PldIdent;
	/* libraries ident functions */
	brdHdl->idFuncTbl.idCall[2].identCall = DESC_Ident;
	brdHdl->idFuncTbl.idCall[3].identCall = OSS_Ident;
	brdHdl->idFuncTbl.idCall[4].identCall = PLD_FLEX10K_Identify;
	/* terminator */
	brdHdl->idFuncTbl.idCall[5].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
    /* init descHdl */
    status = DESC_Init( descSpec, osHdl, &brdHdl->descHdl );
    if(status)
		return( Cleanup(brdHdl,status) );

    /* DEBUG_LEVEL_DESC */
    status = DESC_GetUInt32(brdHdl->descHdl, OSS_DBG_DEFAULT, &value,
		"DEBUG_LEVEL_DESC");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

	/* set debug level for DESC module */
	DESC_DbgLevelSet(brdHdl->descHdl, value);

    /* DEBUG_LEVEL */
    status = DESC_GetUInt32( brdHdl->descHdl, OSS_DBG_DEFAULT, &brdHdl->debugLevel,
                "DEBUG_LEVEL");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    DBGWRT_1((DBH,"BB - %s_Init\n",BRD_NAME));

	/* PCI_DOMAIN_NUMBER - optional */
	status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->domainNbr,
							 "PCI_DOMAIN_NUMBER");
							
	if ( status == ERR_DESC_KEY_NOTFOUND ) {
		/* default pci domain is 0 */
		brdHdl->domainNbr = 0;
	}

    /* PCI_BUS_NUMBER - optional */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->busNbr,
							 "PCI_BUS_NUMBER");

	if( status == ERR_DESC_KEY_NOTFOUND ){

		/*--- get PCI path ---*/
		brdHdl->pciPathLen = D201_BBIS_MAX_PATH;

		status = DESC_GetBinary( brdHdl->descHdl, (u_int8*)"", 0,
								 brdHdl->pciPath, &brdHdl->pciPathLen,
								 "PCI_BUS_PATH");

		if( status ){
			DBGWRT_ERR((DBH, "*** BB - %s_Init: Desc Key PCI_BUS_PATH "
						"not found!\n",	BRD_NAME));
			return( Cleanup(brdHdl,status) );
		}
		/*------------------------------------------------------------+
		|  Now parse the PCI_PATH to determine bus number of devices  |
		+------------------------------------------------------------*/
#ifdef DBG	
		DBGWRT_2((DBH, " PCI_PATH="));
		for(i=0; i<brdHdl->pciPathLen; i++){
			DBGWRT_2((DBH, "0x%x", brdHdl->pciPath[i]));
		}
		DBGWRT_2((DBH, "\n"));
#endif
		status = ParsePciPath( brdHdl, &brdHdl->busNbr );

		if( status ){
			return( Cleanup(brdHdl,status));
		}
	} else if( status == ERR_SUCCESS) {
		DBGWRT_1((DBH,"BB - %s: Using main PCI Bus Number from desc %d\n",
				  BRD_NAME, brdHdl->busNbr ));
	}
	else {
        return( Cleanup(brdHdl,status) );
	}

    /* PCI_DEVICE_ID */
    status = DESC_GetUInt32( brdHdl->descHdl, 0xffff, &brdHdl->pciDev,
                "PCI_DEVICE_ID");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

	if(status==ERR_DESC_KEY_NOTFOUND)
	{
	    /* PCI_BUS_SLOT - required - if PCI_DEVICE_ID not given */
    	status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->mechSlot, "PCI_BUS_SLOT");
    	if( status )
        	return( Cleanup(brdHdl,status) );
    }

    /* PCI_CHECK_LOCATION */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->checkLoc, "PCI_CHECK_LOCATION");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    /* ID_CHECK */
    status = DESC_GetUInt32( brdHdl->descHdl, 1, &brdHdl->useId, "ID_CHECK");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    /* PLD_LOAD */
    status = DESC_GetUInt32( brdHdl->descHdl, 1, &brdHdl->pldLoad, "PLD_LOAD");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );	

    /* NONE_A24 */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->noneA24, "NONE_A24");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );	

    /* MISC_SETMASK */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->miscSetMask, "MISC_SETMASK");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );	

    /* MISC_CLRMASK */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->miscClrMask, "MISC_CLRMASK");
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );	

    /* exit descHdl */
    status = DESC_Exit( &brdHdl->descHdl );
    if(status)
        return( Cleanup(brdHdl,status) );

    /*------------------------------+
    | identify PCI9050 device       |
    +------------------------------*/
	if( brdHdl->pciDev == 0xFFFF ) /* device id not set in descriptor */
	{
	    DBGWRT_1((DBH,"BB - %s_Init mechSlot to devId\n",BRD_NAME));
	    /* convert PCI slot into PCI device */
    	if( (status = OSS_PciSlotToPciDevice( osHdl, OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
    		                                  brdHdl->mechSlot, (int32*)&brdHdl->pciDev)) )
			return( Cleanup(brdHdl,status) );
	}

    DBGWRT_2((DBH,"BB - %s_Init identify PCI9050 at PCI domain %d bus %d device %d\n",
              BRD_NAME, brdHdl->domainNbr, brdHdl->busNbr, brdHdl->pciDev ));

	/* get the vendor Id */
    if( (status = OSS_PciGetConfig(osHdl, OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
    	                           brdHdl->pciDev, 0, OSS_PCI_VENDOR_ID, (int32*)&venId)) )
		return( Cleanup(brdHdl,status) );

    /* get the device Id */
    if( (status = OSS_PciGetConfig(osHdl, OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
    	                           brdHdl->pciDev, 0, OSS_PCI_DEVICE_ID, (int32*)&devId)) )
		return( Cleanup(brdHdl,status) );

	/* get the sub vendor Id */
    if( (status = OSS_PciGetConfig(osHdl, OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
    	                           brdHdl->pciDev, 0, OSS_PCI_SUBSYS_VENDOR_ID, (int32*)&subVenId)) )
		return( Cleanup(brdHdl,status) );

    /* get the sub device Id */
    if( (status = OSS_PciGetConfig(osHdl, OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
    	                           brdHdl->pciDev, 0, OSS_PCI_SUBSYS_ID, (int32*)&subDevId)) )
		return( Cleanup(brdHdl,status) );

    /* compare the Ids */
    if( (   venId != D201_PCI_VEN_ID) ||
		(subVenId != D201_PCI_SUBVEN_ID) ){
		DBGWRT_ERR((DBH,"*** BB - %s_Init: d/b/d/f %d/%d/%d/0 illegal vendor id %04x or subvendor id %04x\n",
		            BRD_NAME, brdHdl->domainNbr, brdHdl->busNbr, brdHdl->pciDev, venId, subVenId ));
		return Cleanup(brdHdl,ERR_BBIS_ILL_ID);
    }

    if( (   devId != D201_PCI_DEV_ID) ||
        (subDevId != D201_PCI_SUBDEV_ID) ){
		DBGWRT_ERR((DBH,"*** BB - %s_Init: d/b/d/f %d/%d/%d/0 illegal device id %04x or subdevice id %04x\n",
		            BRD_NAME, brdHdl->domainNbr, brdHdl->busNbr, brdHdl->pciDev, devId, subDevId ));
		return Cleanup(brdHdl,ERR_BBIS_ILL_ID);
    }

    /*------------------------------+
    | get base addresses            |
    +------------------------------*/

	/* read all BARs */
	for( i=0; i<6; i++ )
		OSS_PciGetConfig( osHdl, OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
		                  brdHdl->pciDev, 0, OSS_PCI_ADDR_0+i, (int32 *)&brdHdl->barValue[i] );
	/*
	 * Check if we need to apply the PLX chip workaround!
	 */
	if( brdHdl->barValue[0] & 0x80 ){
		DBGWRT_ERR((DBH,"*** WARNING: BB - %s_Init: Bit7 in BAR0 is set! PLX bug work"
					" around automatically applied!\n", BRD_NAME));
		brdHdl->plxBugWa = TRUE;
	}

    /* board runtime registers */
    if( (status = OSS_BusToPhysAddr( osHdl, OSS_BUSTYPE_PCI,
									 &brdHdl->physRRaddr,
		                             OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
		                             brdHdl->pciDev,
									 0, D201_PCI_RR_MEM )) )
		return( Cleanup(brdHdl,status) );

    /* save resource */
	j = 0;

	res[j].type = OSS_RES_MEM;
	res[j].u.mem.physAddr = brdHdl->physRRaddr;
	res[j].u.mem.size = RR_SIZE;
	j++;

    /* base addresses for M-Module slots */
    for( i=0; i<BRD_SLOT_NBR; i++, j++){
        if( (status = OSS_BusToPhysAddr( osHdl, OSS_BUSTYPE_PCI,
										 &brdHdl->physModAddr[i],
										 OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
										 brdHdl->pciDev,
										 0, D201_PCI_M_MEM(i) )) )
		return( Cleanup(brdHdl,status) );

		/* save resource */
        res[j].type = OSS_RES_MEM;
        if(brdHdl->noneA24)
            res[j].u.mem.physAddr =
				(void*)( (U_INT32_OR_64)brdHdl->physModAddr[i] + D201_M_CTRL_NOA24 );
        else
            res[j].u.mem.physAddr =
				(void*)( (U_INT32_OR_64)brdHdl->physModAddr[i] + D201_M_CTRL );

        res[j].u.mem.size = D201_M_CTRL_SIZE;
    }

    /* assign the resources */
    if( (status = OSS_AssignResources( osHdl, OSS_BUSTYPE_PCI,
    	                               OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
                                       j, res )) )
		return( Cleanup(brdHdl,status) );

	brdHdl->resourcesAssigned = TRUE;

    /*------------------------------+
    | map used base addresses       |
    +------------------------------*/
	/* board runtime registers */
	if( (status = OSS_MapPhysToVirtAddr( osHdl, brdHdl->physRRaddr,
										 RR_SIZE,
										 OSS_ADDRSPACE_MEM,
										 OSS_BUSTYPE_PCI,
										 OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
										 &brdHdl->virtRRaddr )) )
		return( Cleanup(brdHdl,status) );

	brdHdl->virtRRaddr2 = brdHdl->virtRRaddr;

    /* base addresses for M-Module slots */
    for( i=0; i<BRD_SLOT_NBR; i++ ){
        /* map the control register space */
        if(brdHdl->noneA24)
            status = OSS_MapPhysToVirtAddr(
				osHdl,
				(void*)( (U_INT32_OR_64)brdHdl->physModAddr[i] + D201_M_CTRL_NOA24 ),
				D201_M_CTRL_SIZE,
				OSS_ADDRSPACE_MEM,
				OSS_BUSTYPE_PCI,
				OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
				&brdHdl->virtModCtrlAddr[i] );
        else
            status = OSS_MapPhysToVirtAddr(
				osHdl,
				(void*)( (U_INT32_OR_64)brdHdl->physModAddr[i] + D201_M_CTRL ),
				D201_M_CTRL_SIZE,
				OSS_ADDRSPACE_MEM,
				OSS_BUSTYPE_PCI,
				OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
				&brdHdl->virtModCtrlAddr[i] );
        if( status )
			return( Cleanup(brdHdl,status) );
    }

    /*------------------------------+
    | get interrupt information     |
    +------------------------------*/
    /* get interrupt line */
	status = OSS_PciGetConfig( osHdl, OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
	                           brdHdl->pciDev, 0, OSS_PCI_INTERRUPT_LINE, &brdHdl->irqLevel );
	
	/* no interrupt connected */
	if( status || (brdHdl->irqLevel == 0xff) )
	{
		DBGWRT_ERR((DBH,"*** BB - %s_Init: IRQ not mapped by BIOS\n", BRD_NAME ));
		return( Cleanup(brdHdl,ERR_BBIS_NO_IRQ) );
	}/*if*/

	/* convert level to vector */
	if( (status = OSS_IrqLevelToVector( brdHdl->osHdl, OSS_BUSTYPE_PCI,
										brdHdl->irqLevel, &brdHdl->irqVector )) )
		return( Cleanup(brdHdl,status) );

	DBGWRT_2((DBH," IRQ level=0x%x, vector=0x%x\n",
			  brdHdl->irqLevel, brdHdl->irqVector));

    return 0;
}

/****************************** D201_BrdInit ********************************
 *
 *  Description:  Board initialization:
 *
 *                - identify board with EEPROM Id
 *                - load PLD
 *                - check the board location
 *                - enable interrupt
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_BrdInit(
    BBIS_HANDLE     *brdHdl )
{
    int8		geoAddr;
    u_int8		data[D201_EE_ID_LENGTH];
    u_int8		checksum = 0;
    int32		status, i;
    u_int32		flexFileSize;
    u_int32		miscReg;
	u_int8		*ttf = (u_int8 *)PLD_DATA;

    char    *vendorID = D201_EE_VEN_ID;
    char    *productID = BRD_EE_PROD_ID;

	DBGWRT_1((DBH,"BB - %s_BrdInit\n",BRD_NAME));

    /*------------------------------+
    | identify board with EEPROM Id |
    +------------------------------*/
    if(brdHdl->useId){

		PlxBugWorkaroundStart( brdHdl, 0 );

        /* check if the EEPROM is valid */
		miscReg = MREAD_D32(brdHdl->virtRRaddr, RR_MISC);
        if( !(miscReg & MISC_VAL_EPR) ){
			DBGWRT_ERR((DBH,"*** BB - %s_BrdInit: EEPROM not valid 0x%08x\n",BRD_NAME, miscReg));
			PlxBugWorkaroundEnd( brdHdl, 0 );
            return ERR_BBIS_EEPROM;
        }

        /* read EEPROM ID */
        if( (status = EepromInfo( brdHdl, D201_EE_ID,
								   D201_EE_ID_LENGTH, (void*) data )) ){
			DBGWRT_ERR((DBH,"*** BB  - %s_BrdInit: cannot read from EEPROM\n",BRD_NAME));
			PlxBugWorkaroundEnd( brdHdl, 0 );
            return status;
        }

		PlxBugWorkaroundEnd( brdHdl, 0 );

        /* build checksum */
        for( i=0; i<26; i++)
            checksum = (u_int8)( checksum ^ data[i]);

        /* verify checksum */
        if( checksum != data[26] ){
			DBGWRT_ERR((DBH,"*** BB - %s_BrdInit: wrong EEPROM-ID checksum 0x%x\n",
						BRD_NAME,checksum));
            return ERR_BBIS_ILL_ID;
        }

        /* check the vendor ID */
        if( (status = OSS_StrNcmp( brdHdl->osHdl, (char*)(data + D201_EE_VEN_OFFSET),
								   vendorID, D201_EE_VEN_LENGTH )) ){
			DBGWRT_ERR((DBH,"*** BB - %s_BrdInit: illegal EEPROM vendor ID\n",BRD_NAME));
			DBGDMP_ERR((DBH,"*** EEPROM vendor ID",
					    (int8*)data + D201_EE_VEN_OFFSET,D201_EE_VEN_LENGTH,1));
            return ERR_BBIS_ILL_ID;
        }

        /* check the product ID */
        if( (status = OSS_StrNcmp( brdHdl->osHdl,(char*)(data + D201_EE_PROD_OFFSET),
								   productID, D201_EE_PROD_LENGTH )) ){
			DBGWRT_ERR((DBH,"*** BB - %s_BrdInit: illegal EEPROM product ID\n",BRD_NAME));
			DBGDMP_ERR((DBH,"*** EEPROM product ID\n",
						(int8*)data + D201_EE_PROD_OFFSET,D201_EE_PROD_LENGTH,1));
            return ERR_BBIS_ILL_ID;
        }
    }

    if(brdHdl->pldLoad){

		PlxBugWorkaroundStart( brdHdl, 0 );

		/*------------------------------+
		| load PLD                      |
		+------------------------------*/
		/* set CS_EEPROM to 1 */
		MSETMASK_D32(brdHdl->virtRRaddr, RR_MISC, MISC_CS_EPR);

		/* set IO0...IO3 to USER I/O */
		MCLRMASK_D32( brdHdl->virtRRaddr, RR_MISC,
					  MISC_IO0_MODE | MISC_IO1_MODE | MISC_IO2_MODE | MISC_IO3_MODE);

		/* set IO0 and IO1 to Inputs */
		MCLRMASK_D32(brdHdl->virtRRaddr, RR_MISC, MISC_IO0_DIR | MISC_IO1_DIR);

		/* set IO2 and IO3 as userdefinable Outputs */
		MSETMASK_D32(brdHdl->virtRRaddr, RR_MISC, MISC_IO2_DIR | MISC_IO3_DIR);

		/* set IO2 and IO3 to 1 */
		MSETMASK_D32(brdHdl->virtRRaddr, RR_MISC, MISC_IO2_DATA | MISC_IO3_DATA);

		flexFileSize = ((u_int32)ttf[0] << 24) |
					   ((u_int32)ttf[1] << 16) |
					   ((u_int32)ttf[2] <<  8) |
						(u_int32)ttf[3];

		status = PLD_FLEX10K_LoadCallBk( ttf+4, flexFileSize,
										 PLD_FIRSTBLOCK | PLD_LASTBLOCK,
										 (void*)brdHdl, PLDCB_MsecDelay,
										 PLDCB_GetStatus, PLDCB_GetCfgdone,
										 PLDCB_SetData, PLDCB_SetDclk,
										 PLDCB_SetConfig);

		/* set Clock_EEPROM to 1 */
		MSETMASK_D32(brdHdl->virtRRaddr, RR_MISC, MISC_CLK_EPR);

		/* set CS_EEPROM to 1 */
		MSETMASK_D32(brdHdl->virtRRaddr, RR_MISC, MISC_CS_EPR);

		/* set IO0...IO3 to WAITO, LLOCK, CS2, CS3 */
		MSETMASK_D32( brdHdl->virtRRaddr, RR_MISC,
					  MISC_IO0_MODE | MISC_IO1_MODE | MISC_IO2_MODE | MISC_IO3_MODE);

		/* reset local bus */
		MSETMASK_D32(brdHdl->virtRRaddr,RR_MISC,MISC_SOFTRES);
		OSS_Delay( brdHdl->osHdl, 10 );
		MCLRMASK_D32(brdHdl->virtRRaddr,RR_MISC,MISC_SOFTRES);

		PlxBugWorkaroundEnd( brdHdl, 0 );

		/* error from PLD_FLEX10K_LoadCallBk() */
		if(status){
			/* convert status into MDIS error code */
			status += ERR_PLD;
			DBGWRT_ERR((DBH,"*** BB - %s_BrdInit: PLD_FLEX10K_LoadCallBk() "
						"returns error=0x%x\n",BRD_NAME, status));
			return status;
		}
	}

    /*------------------------------+
    | check the board location      |
    +------------------------------*/
    if(brdHdl->checkLoc){

        geoAddr = (int8)MREAD_D16(brdHdl->virtModCtrlAddr[0],D201_M_GEOADDR);

		/* geographic address not supported */
        if( (geoAddr & D201_M_GEOADDR_MASK) == 0x00 ){
			DBGWRT_ERR((DBH,"*** BB - %s_BrdInit: backplane doesn't support "
						"geographic address\n",BRD_NAME));
			return ERR_BBIS_NO_CHECKLOC;
        }
        /* geographic address supported */
        else{
            /* false location */
            if( geoAddr != (int8) brdHdl->mechSlot ){
				DBGWRT_ERR((DBH,"*** BB - %s_BrdInit: false board location\n",BRD_NAME));
                return ERR_BBIS_ILL_SLOT;
            }
        }
    }

    /*------------------------------+
    | set/clr certain bits          |
	| in Misc. Ctrl. register       |
	| ONLY FOR TEST & DEBUGGING     |
    +------------------------------*/
	PlxBugWorkaroundStart( brdHdl, 0 );
    if( brdHdl->miscSetMask ) {
		MSETMASK_D32( brdHdl->virtRRaddr, RR_MISC, brdHdl->miscSetMask );
	}
    if( brdHdl->miscClrMask ) {
	    MCLRMASK_D32( brdHdl->virtRRaddr, RR_MISC, brdHdl->miscClrMask );
	}
	PlxBugWorkaroundEnd( brdHdl, 0 );

    /*------------------------------+
    | enable interrupt              |
    +------------------------------*/
    /* enable LINT1:module, LINT2:timeout, LINT:all */
	INT_ENABLE( brdHdl );

    return 0;
}

/****************************** D201_BrdExit ********************************
 *
 *  Description:  Board deinitialization:
 *
 *                - disable interrupt
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_BrdExit(
    BBIS_HANDLE     *brdHdl )
{

	DBGWRT_1((DBH,"BB - %s_BrdExit\n",BRD_NAME));

    /*------------------------------+
    | disable interrupt             |
    +------------------------------*/
    /* disable LINT1:module, LINT2:timeout, LINT:all */
	INT_DISABLE( brdHdl );

    return 0;
}

/****************************** D201_Exit ***********************************
 *
 *  Description:  BBIS deinitialization:
 *
 *                - cleanup memory
 *                - deinitializes the bbis handle
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdlP   pointer to board handle structure
 *  Output.....:  *brdHdlP  NULL
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_Exit(
    BBIS_HANDLE     **brdHdlP )
{
	int32		error = 0;
    DBGCMD( BBIS_HANDLE	*brdHdl = *brdHdlP; )

    DBGWRT_1((DBH,"BB - %s_Exit\n",BRD_NAME));

    /*------------------------------+
    |  cleanup memory               |
    +------------------------------*/
	error = Cleanup(*brdHdlP, error);
    *brdHdlP = NULL;

    return error;
}

/****************************** D201_BrdInfo ********************************
 *
 *  Description:  Get information about hardware and driver requirements.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  -----------------------------
 *                BBIS_BRDINFO_BUSTYPE      board bustype
 *                BBIS_BRDINFO_DEVBUSTYPE   device bustype
 *                BBIS_BRDINFO_FUNCTION     used optional functions
 *                BBIS_BRDINFO_NUM_SLOTS    number of slots
 *				  BBIS_BRDINFO_INTERRUPTS   interrupt characteristics
 *                BBIS_BRDINFO_ADDRSPACE    address characteristic
 *				  BBIS_BRDINFO_BRDNAME		name of the board hardware
 *
 *                The BBIS_BRDINFO_BUSTYPE code returns the bustype of
 *                the specified board.
 *
 *                The BBIS_BRDINFO_DEVBUSTYPE code returns the bustype of
 *                the specified device - not the board bus type.
 * 				
 *                The BBIS_BRDINFO_FUNCTION code returns the information
 *                if an optional BBIS function is supported or not.
 *
 *                The BBIS_BRDINFO_NUM_SLOTS code returns the number of
 *                supported M-Module slots.
 *
 *                The BBIS_BRDINFO_INTERRUPTS code returns the supported
 *                interrupt capability (BBIS_IRQ_DEVIRQ/BBIS_IRQ_EXPIRQ)
 *                of the specified device.
 *
 *                The BBIS_BRDINFO_ADDRSPACE code returns the address
 *                characteristic (OSS_ADDRSPACE_MEM/OSS_ADDRSPACE_IO)
 *                of the specified device.
 *
 *				  The BBIS_BRDINFO_BRDNAME code returns the short hardware
 *                name and type of the board without any prefix or suffix.
 *                The hardware name must not contain any non-printing
 *                characters. The length of the returned string, including
 *                the terminating null character, must not exceed
 *                BBIS_BRDINFO_BRDNAME_MAXSIZE.
 *                Examples: D201 board, PCI device, Chameleon FPGA
 *
 *---------------------------------------------------------------------------
 *  Input......:  code      reference to the information we need
 *                ...       variable arguments
 *  Output.....:  *...      variable arguments
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_BrdInfo(
    u_int32 code,
    ... )
{
	int32		status = ERR_SUCCESS;
    va_list     argptr;

    va_start(argptr,code);

    switch( code )
    {
	/* supported functions */
    case BBIS_BRDINFO_FUNCTION:
    {
        u_int32 funcCode = va_arg( argptr, u_int32 );
        u_int32 *used = va_arg( argptr, u_int32* );

		switch( funcCode )
		{
			/* supported */
			case BBIS_FUNC_IRQENABLE:
			case BBIS_FUNC_IRQSRVINIT:
			case BBIS_FUNC_IRQSRVEXIT:
				*used = TRUE;
				break;
			/* unsupported */
			default:
				*used = FALSE;
		}
        break;
    }

    /* number of module slots */
    case BBIS_BRDINFO_NUM_SLOTS:
    {
        u_int32 *numSlot = va_arg( argptr, u_int32* );

        *numSlot = BRD_SLOT_NBR;
        break;
    }

    /* bus type */
    case BBIS_BRDINFO_BUSTYPE:
    {
        u_int32 *busType = va_arg( argptr, u_int32* );

        *busType = OSS_BUSTYPE_PCI;
        break;
    }

    /* device bus type */
    case BBIS_BRDINFO_DEVBUSTYPE:
    {
        u_int32 mSlot       = va_arg( argptr, u_int32 );
        u_int32 *devBusType = va_arg( argptr, u_int32* );

        mSlot = mSlot; /* dummy access to avoid compiler warning */
		*devBusType = OSS_BUSTYPE_MMODULE;
        break;
    }

    /* interrupt capability */
    case BBIS_BRDINFO_INTERRUPTS:
    {
        u_int32 mSlot = va_arg( argptr, u_int32 );
        u_int32 *irqP = va_arg( argptr, u_int32* );

        mSlot = mSlot; /* dummy access to avoid compiler warning */
        *irqP = BBIS_IRQ_DEVIRQ | BBIS_IRQ_EXPIRQ;
        break;
    }

    /* address space type */
    case BBIS_BRDINFO_ADDRSPACE:
    {
        u_int32 mSlot      = va_arg( argptr, u_int32 );
        u_int32 *addrSpace = va_arg( argptr, u_int32* );

        mSlot = mSlot; /* dummy access to avoid compiler warning */
		*addrSpace = OSS_ADDRSPACE_MEM;
        break;
    }

    /* board name */
    case BBIS_BRDINFO_BRDNAME:
    {
        char	*brdName = va_arg( argptr, char* );
		char	*from;

		/*
		 * build hw name (e.g. D201 board)
		 */
		from = BRD_NAME;
	    while( (*brdName++ = *from++) );	/* copy string */
		from = " board";
	    while( (*brdName++ = *from++) );	/* copy string */
		
        break;
    }

    /* unknown code */
    default:
        status = ERR_BBIS_UNK_CODE;
    }

    /* all was ok */
    va_end( argptr );
    return status;
}

/****************************** D201_CfgInfo ********************************
 *
 *  Description:  Get information about board configuration.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  ------------------------------
 *                BBIS_CFGINFO_BUSNBR       bus number
 *                BBIS_CFGINFO_PCI_DOMAIN	PCI domain number
 *                BBIS_CFGINFO_IRQ          interrupt parameters
 *                BBIS_CFGINFO_EXP          exception interrupt parameters
 *                BBIS_CFGINFO_SLOT			slot information
 *
 *                The BBIS_CFGINFO_BUSNBR code returns the number of the
 *                bus on which the board resides.
 *
 *                The BBIS_CFGINFO_PCI_DOMAIN code returns the number of
 *                the PCI domain on which the specified device resides
 *                (introduced 2011)
 *
 *                The BBIS_CFGINFO_IRQ code returns the device interrupt
 *                vector, level and mode of the specified device.
 *
 *                The BBIS_CFGINFO_EXP code returns the exception interrupt
 *                vector, level and mode of the specified device.
 *
 *                The BBIS_CFGINFO_SLOT code returns the following
 *                information about the specified device slot:
 *                The slot is occupied or empty, the device id and device
 *                revision of the plugged device, the name of the slot and
 *                the name of the plugged device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                code      reference to the information we need
 *                ...       variable arguments
 *  Output.....:  ...       variable arguments
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_CfgInfo(
    BBIS_HANDLE     *brdHdl,
    u_int32         code,
    ... )
{
	int32	status=ERR_SUCCESS;	
    va_list	argptr;

    DBGWRT_1((DBH,"BB - %s_CfgInfo\n",BRD_NAME));

    va_start(argptr,code);

    switch( code )
    {
    /* bus number */
    case BBIS_CFGINFO_BUSNBR:
    {
        u_int32 *busNbr = va_arg( argptr, u_int32* );
        u_int32 mSlot   = va_arg( argptr, u_int32 );

        *busNbr = brdHdl->busNbr;
		mSlot = mSlot; /* dummy access to avoid compiler warning */
        break;
    }

	/* pci domain number */
	case BBIS_CFGINFO_PCI_DOMAIN:
	{
		/* domain number */
		u_int32 *domainNbr = va_arg( argptr, u_int32* );
        u_int32 mSlot      = va_arg( argptr, u_int32 );

		*domainNbr = brdHdl->domainNbr;
		mSlot = mSlot; /* dummy access to avoid compiler warning */
		break;
	}	
		
    /* interrupt information */
    case BBIS_CFGINFO_IRQ:
    {
        u_int32 mSlot   = va_arg( argptr, u_int32 );
        u_int32 *vector = va_arg( argptr, u_int32* );
        u_int32 *level  = va_arg( argptr, u_int32* );
        u_int32 *mode   = va_arg( argptr, u_int32* );

        mSlot = mSlot; /* dummy access to avoid compiler warning */
        *mode = BBIS_IRQ_SHARED | BBIS_IRQ_EXCEPTION;
		*level = brdHdl->irqLevel;
		*vector = brdHdl->irqVector;

		DBGWRT_2((DBH," mSlot=%d : IRQ mode=0x%x, level=0x%x, vector=0x%x\n",
				  mSlot, *mode, *level, *vector));
        break;
    }

    /* exception interrupt information */
    case BBIS_CFGINFO_EXP:
    {
        u_int32 mSlot   = va_arg( argptr, u_int32 );
        u_int32 *vector = va_arg( argptr, u_int32* );
        u_int32 *level  = va_arg( argptr, u_int32* );
        u_int32 *mode   = va_arg( argptr, u_int32* );

        /* no extra exception interrupt */
        *mode = 0;
        mSlot = mSlot;     /* dummy access to avoid compiler warning */
        *vector = *vector; /* dummy access to avoid compiler warning */
        *level = *level;   /* dummy access to avoid compiler warning */
        break;
    }

    /* slot information for PnP support*/
    case BBIS_CFGINFO_SLOT:
    {
		status = CfgInfoSlot( brdHdl, argptr );
        break;
    }

    /* unknown code */
    default:
		DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",BRD_NAME,code));
        va_end( argptr );
        return ERR_BBIS_UNK_CODE;
    }

    /* all was ok */
    va_end( argptr );
    return status;
}

/****************************** D201_IrqEnable ******************************
 *
 *  Description:  Enable/disable interrupt for specified module slot.
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *                enable    interrupt setting
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_IrqEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         enable )
{
    DBGWRT_1((DBH,"BB - %s_IrqEnable: mSlot=%d enable=%d\n",BRD_NAME,mSlot,enable));

    if(enable)
    {
	    /* enable */
        MSETMASK_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_INT, D201_M_INT_ENABLE );
	}
    else
    {
	    /* disable */
        MCLRMASK_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_INT, D201_M_INT_ENABLE );
	}

	return 0;
}

/****************************** D201_IrqSrvInit *****************************
 *
 *  Description:  Called at the beginning of an interrupt.
 *
 *                - DEBUG: print irq information
 *                - determine interrupt reason
 *
 *                Note: Bus or timeout interrupts will be cleared.
 *                      Module interrupt must be cleared on the module
 *                      itself and by D201_IrqSrvExit().
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    BBIS_IRQ_NO | BBIS_IRQ_MODULE | BBIS_IRQ_EXP
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_IrqSrvInit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
    u_int16		irqSetting;
    int32		irqEvent = 0;
	DBGCMD( u_int16		i; )

	IDBGWRT_1((DBH,"BB - %s_IrqSrvInit: mSlot=%d\n",BRD_NAME,mSlot));

#ifdef DBG
   /*------------------------------+
    | DEBUG: print irq information  |
    +------------------------------*/
	/* get and print interrupt information of module 0..3 */
    if( brdHdl->debugLevel & (DBG_LEV3 | DBG_INTR) )
		for(i=0; i<BRD_SLOT_NBR; i++){
			irqSetting = MREAD_D16(brdHdl->virtModCtrlAddr[i], D201_M_INT);
			IDBGWRT_3((DBH," module=%d D201_M_INT=0x%x\n",i,irqSetting));
		}
#endif

    /*------------------------------+
    | determine interrupt reason    |
    +------------------------------*/
    /* get the interrupt information */
    irqSetting = MREAD_D16(brdHdl->virtModCtrlAddr[mSlot], D201_M_INT);

    /* bus error ? */
    if(irqSetting & D201_M_INT_BUSERR){
		IDBGWRT_ERR((DBH,"*** %s_IrqSrvInit: mSlot=%d - interrupt by buserror\n",
					 BRD_NAME,mSlot));

        irqEvent |= BBIS_IRQ_EXP;
		brdHdl->expIrqCount++;	

		/* clear bus error interrupt */
        MCLRMASK_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_INT,
					  D201_M_INT_BUSERR );
    }

    /* timeout error ? */
    if(irqSetting & D201_M_INT_TIMEOUT){
		IDBGWRT_ERR((DBH,"*** %s_IrqSrvInit: mSlot=%d - interrupt by timeout\n",
					 BRD_NAME,mSlot));

        irqEvent |= BBIS_IRQ_EXP;
		brdHdl->expIrqCount++;	

		/* clear timeout interrupt */
        MCLRMASK_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_INT,
					  D201_M_INT_TIMEOUT );
    }

    /* interrupt disabled ? */
    if( !(irqSetting & D201_M_INT_ENABLE) ){
		IDBGWRT_2((DBH," interrupt not enabled\n"));
        irqEvent |= BBIS_IRQ_NO;
        return irqEvent;
    }

    /* module interrupt ? */
    if(irqSetting & D201_M_INT_INTFLAG){
		IDBGWRT_2((DBH," interrupt by module\n"));
        irqEvent |= BBIS_IRQ_YES;

        /* Note: the module interrupt will be cleared
                 by the module and D201_IrqSrvExit() */
    }
    else{
		IDBGWRT_2((DBH," not my interrupt\n"));
        irqEvent |= BBIS_IRQ_NO;
    }

    return irqEvent;
}

/****************************** D201_IrqSrvExit *****************************
 *
 *  Description:  Called at the end of an interrupt.
 *
 *                - clear module interrupt
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void D201_IrqSrvExit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{

	IDBGWRT_1((DBH,"BB - %s_IrqSrvExit: mSlot=%d\n",BRD_NAME,mSlot));

    /* clear interrupt */
    MCLRMASK_D16(brdHdl->virtModCtrlAddr[mSlot],
		D201_M_INT, D201_M_INT_INTFLAG);
}

/****************************** D201_ExpEnable ******************************
 *
 *  Description:  Exception interrupt enable / disable.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *                enable    interrupt setting
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_ExpEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
	u_int32			enable)
{
	IDBGWRT_1((DBH,"BB - %s_ExpEnable: mSlot=%d\n",BRD_NAME,mSlot));

	return 0;
}

/****************************** D201_ExpSrv *********************************
 *
 *  Description:  Called at the beginning of an exception interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    BBIS_IRQ_NO
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_ExpSrv(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH,"BB - %s_ExpSrv: mSlot=%d\n",BRD_NAME,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** D201_SetMIface ******************************
 *
 *  Description:  Set interface for specified module slot.
 *
 *                - check module slot number
 *                - set data mode
 *                - set address mode
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24
 *                dataMode  MDIS_MODE_D16 | MDIS_MODE_D32
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_SetMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode)
{
	u_int32		rrBusreg, rrBusWidth;
	u_int16		mMode, mDataMode;

	DBGWRT_1((DBH,"BB - %s_SetMIface: mSlot=%d\n",BRD_NAME,mSlot));

    /*------------------------------+
    | check module slot number      |
    +------------------------------*/
    if( mSlot > (BRD_SLOT_NBR-1) ){
		DBGWRT_ERR((DBH,"*** %s_SetMIface: wrong module slot number=%d\n",
					BRD_NAME,mSlot));
        return ERR_BBIS_ILL_SLOT;
    }

    /*------------------------------+
    | set data mode                 |
    +------------------------------*/
    /* D32 or D16/D32 module */
    if( dataMode & MDIS_MD32 ){
		DBGWRT_2((DBH," set module interface to D32\n"));
		rrBusWidth = BUSREG_BUSWIDTH_D32;	/* bus width: D32 */
        mDataMode =  D201_M_MODE_D32;		/* data mode: D32 */
	}
	else{
		/* D16 module or D8 module */
		if( (dataMode & MDIS_MD16) || (dataMode & MDIS_MD08) ){
			DBGWRT_2((DBH," set module interface to D16\n"));
			rrBusWidth = BUSREG_BUSWIDTH_D16;	/* bus width: D16 */
			mDataMode = D201_M_MODE_D16;		/* data mode: D16 */
		}
        else{
			/* unsupported modes (no D16, no D32) */
			DBGWRT_ERR((DBH,"*** %s_SetMIface: mSlot=%d, dataMode=0x%x not supported\n",
						BRD_NAME,mSlot,dataMode));
			return ERR_BBIS_ILL_DATAMODE;
		}
	}

	/* set bus width */
	PlxBugWorkaroundStart( brdHdl, mSlot );

	rrBusreg = MREAD_D32( brdHdl->virtRRaddr, RR_BUSREG(mSlot) );
	rrBusreg = (rrBusreg & ~BUSREG_BUSWIDTH) | rrBusWidth;
	MWRITE_D32( brdHdl->virtRRaddr, RR_BUSREG(mSlot), rrBusreg );

	PlxBugWorkaroundEnd( brdHdl, mSlot );

    /* set data mode */
	mMode = MREAD_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_MODE );
	mMode = (u_int16)((mMode & ~D201_M_MODE_DBITS) | mDataMode);
	MWRITE_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_MODE, mMode );

    /*------------------------------+
    | set address mode              |
    +------------------------------*/
    /* A08 or A24 : nothing to do (always A24) */

    /* unsupported modes (no A08, no A24) */
    if( !(addrMode & (MDIS_MA08 | MDIS_MA24)) ){
		DBGWRT_ERR((DBH,"*** %s_SetMIface: mSlot=%d, addrMode=0x%x not supported\n",
					BRD_NAME,mSlot,addrMode));
		return ERR_BBIS_ILL_ADDRMODE;
    }

    return 0;
}

/****************************** D201_ClrMIface ******************************
 *
 *  Description:  Clear interface for specified module slot.
 *
 *                - check module slot number
 *                - set data mode defaults
 *                - set address mode defaults
 *                - disable module interrupt
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_ClrMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	DBGWRT_1((DBH,"BB - %s_ClrMIface: mSlot=%d\n",BRD_NAME,mSlot));

    /*------------------------------+
    | check module slot number      |
    +------------------------------*/
    if( mSlot > (BRD_SLOT_NBR-1) ){
		DBGWRT_ERR((DBH,"*** %s_ClrMIface: wrong module slot number=%d\n",
					BRD_NAME,mSlot));
        return ERR_BBIS_ILL_SLOT;
    }

    /*------------------------------+
    | set data mode default D16     |
    +------------------------------*/
	/* omitted, because problems (triggering interrupts) if D32 was used */

    /*------------------------------+
    | set address mode default A24  |
    +------------------------------*/
    /* nothing to do (always A24) */

    /*------------------------------+
    | disable module interrupt      |
    +------------------------------*/
	MCLRMASK_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_INT,
				  D201_M_INT_INTFLAG |	/* clear module interrupt	 */
				  D201_M_INT_ENABLE );	/* disable module interrupt  */

    return 0;
}

/****************************** D201_GetMAddr *******************************
 *
 *  Description:  Get physical address description.
 *
 *                - check module slot number
 *                - assign address spaces
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24
 *                dataMode  MDIS_MODE_D16 | MDIS_MODE_D32
 *                mAddr     pointer to address space
 *                mSize     size of address space
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_GetMAddr(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode,
    void            **mAddr,
    u_int32         *mSize )
{
	DBGWRT_1((DBH,"BB - %s_GetMAddr: mSlot=%d\n",BRD_NAME,mSlot));

    /*------------------------------+
    | check module slot number      |
    +------------------------------*/
    if( mSlot > (BRD_SLOT_NBR-1) ){
		DBGWRT_ERR((DBH,"*** %s_GetMAddr: wrong module slot number=%d\n",
					BRD_NAME,mSlot));
        return ERR_BBIS_ILL_SLOT;
    }

    /*------------------------------+
    | dependent on address mode     |
    +------------------------------*/
    switch (addrMode){

    /* A08 */
    case MDIS_MA08 :
        if(brdHdl->noneA24){
			DBGWRT_2((DBH," supply A08 address space (NONE_A24)\n"));
            *mAddr = (void*)( ( int8*)brdHdl->physModAddr[mSlot] + D201_M_A08_NOA24);
            *mSize = D201_M_A08_SIZE;
        }
        else{
			DBGWRT_2((DBH," supply A08 address space\n"));
            *mAddr = (void*)( ( int8*)brdHdl->physModAddr[mSlot] + D201_M_A08);
            *mSize = D201_M_A08_SIZE;
        }
        break;

	/* A24 */
    case MDIS_MA24 :
		DBGWRT_2((DBH," supply A24 address space\n"));
        *mAddr = (void*)( ( int8*)brdHdl->physModAddr[mSlot] + D201_M_A24);
        *mSize = D201_M_A24_SIZE;
        break;

	/* not A08, not A24 */
    default:
		DBGWRT_ERR((DBH,"*** %s_GetMAddr: addrMode=0x%x not supported\n",
					BRD_NAME,addrMode));
        return ERR_BBIS_ILL_ADDRMODE;
    }

    /*------------------------------+
    | dependent on data mode        |
    +------------------------------*/
    switch (dataMode){

	/* D16 or D08 */
    case MDIS_MD08 :
    case MDIS_MD16 :
		DBGWRT_2((DBH," supply D08/D16 data mode\n"));
        /* nothing to do */
        break;

	/* D32 */
    case MDIS_MD32 :
		DBGWRT_2((DBH," supply D32 data mode\n"));
        /* nothing to do */
        break;

	/* no D16, no D32 */
    default:
		DBGWRT_ERR((DBH,"*** %s_GetMAddr: dataMode=0x%x not supported\n",
					BRD_NAME,dataMode));
        return ERR_BBIS_ILL_DATAMODE;
    }

    /* all was ok */
    return 0;
}

/****************************** D201_SetStat ********************************
 *
 *  Description:  Set driver status.
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_BB_DEBUG_LEVEL     board debug level          see dbg.h
 *                M_BB_IRQ_EXP_COUNT   exception irq count        0..max
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    		pointer to board handle structure
 *                mSlot     		module slot number
 *                code      		setstat code
 *                value32_or_64     setstat value or ptr to blocksetstat data
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_SetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     value32_or_64 )
{
	int32 value = (int32) value32_or_64; /* 32bit value */
    DBGWRT_1((DBH,"BB - %s_SetStat: mSlot=%d code=0x%04x value=0x%x\n",
			  BRD_NAME,mSlot,code,value));

    switch(code)
    {
	/*------------------------------+
	| set exception counter         |
	+------------------------------*/
    case M_BB_IRQ_EXP_COUNT:
        brdHdl->expIrqCount = value;
        break;
	/*------------------------------+
	| set debug level               |
	+------------------------------*/
    case M_BB_DEBUG_LEVEL:
        brdHdl->debugLevel = value;
        break;
	/*------------------------------+
	| (unknown)                     |
	+------------------------------*/
    /* unknown */
    default:
        return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** D201_GetStat ********************************
 *
 *  Description:  Get driver status.
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_BB_DEBUG_LEVEL     driver debug level         see dbg.h
 *                M_BB_IRQ_EXP_COUNT   exception irq count        0..max
 *                M_BB_IRQ_VECT        interrupt vector           0..max
 *                M_BB_IRQ_LEVEL       interrupt level            0..max
 *                M_BB_IRQ_PRIORITY    interrupt priority         0
 *                M_BB_ID_CHECK        eeprom-id is checked       0..1
 *                M_BB_ID_SIZE         eeprom id-data size[bytes] 28
 *                M_MK_BLK_REV_ID      ident function table ptr   -
 *                M_BB_BLK_ID_DATA     eeprom id-data             -
 *				  D201_BLK_EEPROM_DATA eeprom raw-data            -
 *
 *                D201_BLK_EEPROM_DATA gets the data of the entire eeprom.
 *                  The entire eeprom size is 128 bytes.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    		pointer to board handle structure
 *                mSlot    			module slot number
 *                code      		getstat code
 *  Output.....:  value32_or_64P    getstat value or ptr to blockgetstat data
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_GetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     *value32_or_64P )
{
    int32		status = ERR_SUCCESS;
    int32  		*valueP = (int32*) value32_or_64P; /* pointer to 32bit value */
	M_SG_BLOCK	*statBlock = (M_SG_BLOCK*) value32_or_64P;


    DBGWRT_1((DBH,"BB - %s_GetStat: mSlot=%d code=0x%04x\n",
			  BRD_NAME,mSlot,code));

    switch(code)
    {
	/*------------------------------+
	| get debug level               |
	+------------------------------*/
    case M_BB_DEBUG_LEVEL:
        *valueP = brdHdl->debugLevel;
        break;
	/*------------------------------+
	| get exception counter         |
	+------------------------------*/
    case M_BB_IRQ_EXP_COUNT:
        *valueP = brdHdl->expIrqCount;
        break;
	/*------------------------------+
	| get interrupt vector          |
	+------------------------------*/
    case M_BB_IRQ_VECT:
		*valueP = brdHdl->irqVector;
		break;
	/*------------------------------+
	| get interrupt level           |
	+------------------------------*/
    case M_BB_IRQ_LEVEL:
		*valueP = brdHdl->irqLevel;
        break;
	/*------------------------------+
	| get interrupt priority        |
	+------------------------------*/
    case M_BB_IRQ_PRIORITY:
        *valueP = 0;
        break;
	/*------------------------------+
	| get eeprom-id check state     |
	+------------------------------*/
    case M_BB_ID_CHECK:
        *valueP = brdHdl->useId;
        break;
	/*------------------------------+
    |   id prom size                |
	+------------------------------*/
    case M_BB_ID_SIZE:
        *valueP = D201_EE_ID_LENGTH;
        break;
	/*------------------------------+
	| get ident table               |
	+------------------------------*/
    case M_MK_BLK_REV_ID:
       *value32_or_64P = (INT32_OR_64)&brdHdl->idFuncTbl;
       break;
	/*------------------------------+
	| read eeprom id data           |
	+------------------------------*/
    /* get eeprom id info */
    case M_BB_BLK_ID_DATA:
		if( brdHdl->plxBugWa ){
			status = ERR_BBIS_EEPROM;
			break;
		}
        status = EepromInfo(
					brdHdl,
					(*((u_int32*)(statBlock->data)+0))+D201_EE_ID,	/* offset */
					*((u_int32*)(statBlock->data)+1),				/* size */
					statBlock->data );
        break;
	/*------------------------------+
	| read entire eeprom data       |
	+------------------------------*/
    case D201_BLK_EEPROM_DATA:
		if( brdHdl->plxBugWa ){
			status = ERR_BBIS_EEPROM;
			break;
		}
		status = EepromInfo(
				brdHdl,
				*((u_int32*)(statBlock->data)+0),	/* offset */
				*((u_int32*)(statBlock->data)+1),	/* size */
				statBlock->data );
		break;
	/*------------------------------+
	| (unknown)                     |
	+------------------------------*/
    default:
        status = ERR_BBIS_UNK_CODE;
	}

    return status;
}

/****************************** D201_Unused *********************************
 *
 *  Description:  Dummy function for unused jump table entries.
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  return	ERR_BBIS_ILL_FUNC
 *  Globals....:  ---
 ****************************************************************************/
static int32 D201_Unused( void )		/* nodoc */
{
    return ERR_BBIS_ILL_FUNC;
}

/****************************** EepromInfo **********************************
 *
 *  Description:  Read 'size' bytes from the EEPROM at offset 'offset'.
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl   pointer to board handle structure
 *                offset   offset in bytes to the EEPROM base address
 *                size     size in bytes to read / of the ‘Data’ buffer
 *                data     pointer to data buffer
 *  Output.....:  *data    data
 *                return   0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 EepromInfo(
    BBIS_HANDLE     *brdHdl,
    u_int32         offset,
    u_int32         size,
    void            *data )		/* nodoc */
{
    u_int16 wcount, woffset;

	DBGWRT_1((DBH,"BB - %s EepromInfo: offset=0x%x, size=0x%x\n",
			  BRD_NAME,offset,size));

    /* the EEPROM size is 128 bytes, check that we only read in this area */
    if( offset+size > 128 ){
		DBGWRT_ERR((DBH,"*** %s EepromInfo: offset+size > 128\n",BRD_NAME));
        return ERR_BBIS_EEPROM;
    }

    /* we read words : convert size and offset */
    wcount  = (u_int16) (size/2);
    woffset = (u_int16) (offset/2);

    /* we can only read words */
    if( ( wcount*2 != (u_int16)size ) || ( woffset*2 != (u_int16)offset ) ){
		DBGWRT_ERR((DBH,"*** %s EepromInfo: error uneven eeprom register\n",BRD_NAME));
        return ERR_BBIS_EEPROM;
    }

    /* read 'wcount' words from the EEPROM, begin at 'woffset' */
    EepromReadBuf( (INT32_OR_64)brdHdl->virtRRaddr + RR_MISC,
									(u_int16*)data, woffset, wcount);

    return 0;
}

/********************************* Cleanup **********************************
 *
 *  Description:  Close all handles, free memory and return error code
 *
 *		          NOTE: The brdHdl handle is invalid after calling this
 *                      function.
 *			
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                retCode	return value
 *  Output.....:  return	retCode
 *  Globals....:  -
 ****************************************************************************/
static int32 Cleanup(
   BBIS_HANDLE  *brdHdl,
   int32        retCode		/* nodoc */
)
{
	int32	i;

    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if(brdHdl->descHdl)
		DESC_Exit(&brdHdl->descHdl);

	/* cleanup debug */
	DBGEXIT((&DBH));

	/* unmap the board runtime registers */
	if( brdHdl->virtRRaddr2 )
		OSS_UnMapVirtAddr(brdHdl->osHdl, &brdHdl->virtRRaddr2, RR_SIZE, OSS_ADDRSPACE_MEM);

    /* unmap already mapped control register spaces */
    for ( i=0; i<BRD_SLOT_NBR; i++)
		if( brdHdl->virtModCtrlAddr[i] )
			OSS_UnMapVirtAddr( brdHdl->osHdl, &brdHdl->virtModCtrlAddr[i],
							   D201_M_CTRL_SIZE, OSS_ADDRSPACE_MEM );

#ifdef OSS_HAS_UNASSIGN_RESOURCES
	if( brdHdl->resourcesAssigned ){
		OSS_RESOURCES	res[D201_RES_NBR];
		/* save resource */
		res[0].type = OSS_RES_MEM;
		res[0].u.mem.physAddr = brdHdl->physRRaddr;
		res[0].u.mem.size = RR_SIZE;

		/* base addresses for M-Module slots */
		for( i=0; i<BRD_SLOT_NBR; i++){
			/* save resource */
			res[i+1].type = OSS_RES_MEM;
			if(brdHdl->noneA24)
				res[i+1].u.mem.physAddr =
					(void*)( (U_INT32_OR_64)brdHdl->physModAddr[i]+D201_M_CTRL_NOA24 );
			else
				res[i+1].u.mem.physAddr =
					(void*)( (U_INT32_OR_64)brdHdl->physModAddr[i] + D201_M_CTRL );

			res[i+1].u.mem.size = D201_M_CTRL_SIZE;
		}
		/* unassign the resources */
		OSS_UnAssignResources( brdHdl->osHdl, OSS_BUSTYPE_PCI,
		                       OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
							   D201_RES_NBR, res );
	}
	
#endif
    /*------------------------------+
    |  free memory                  |
    +------------------------------*/
    /* release memory for the board handle */
    OSS_MemFree( brdHdl->osHdl, (int8*)brdHdl, brdHdl->ownMemSize);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}

/****************************** PLDCB_MsecDelay *****************************
 *
 *  Description:  Callback function for PLD_FLEX10K_LoadCallBk()
 *                delay 'msec' milli seconds
 *
 *---------------------------------------------------------------------------
 *  Input......:  arg       argument pointer
 *                msec      milli seconds to delay
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void PLDCB_MsecDelay(void *arg, u_int32 msec)		/* nodoc */
{
    OSS_Delay(((BBIS_HANDLE*)arg)->osHdl, (int32) msec);
}

/****************************** PLDCB_GetStatus *****************************
 *
 *  Description:  Callback function for PLD_FLEX10K_LoadCallBk()
 *                get setting of PLD STATUS bit
 *
 *---------------------------------------------------------------------------
 *  Input......:  arg       argument pointer
 *  Output.....:  return    setting of PLD STATUS bit
 *  Globals....:  ---
 ****************************************************************************/
static u_int8 PLDCB_GetStatus(void *arg)		/* nodoc */
{
    u_int32 reg;
    reg = MREAD_D32( ((BBIS_HANDLE*)arg)->virtRRaddr, RR_MISC);

	if( D201_PLD_STATUS & reg )
	    return ( (u_int8) 1 );
	else
	    return ( (u_int8) 0 );
}

/****************************** PLDCB_GetCfgdone ****************************
 *
 *  Description:  Callback function for PLD_FLEX10K_LoadCallBk()
 *                get setting of PLD CONFIG DONE bit
 *
 *---------------------------------------------------------------------------
 *  Input......:  arg       argument pointer
 *  Output.....:  return    setting of PLD CONFIG DONE bit
 *  Globals....:  ---
 ****************************************************************************/
static u_int8 PLDCB_GetCfgdone(void *arg)		/* nodoc */
{
    u_int32 reg;
    reg = MREAD_D32( ((BBIS_HANDLE*)arg)->virtRRaddr, RR_MISC);

    if( D201_PLD_CFGDONE & reg )
	    return ( (u_int8) 1 );
	else
	    return ( (u_int8) 0 );
}

/****************************** PLDCB_SetData *******************************
 *
 *  Description:  Callback function for PLD_FLEX10K_LoadCallBk()
 *                set PLD DATA bit
 *
 *---------------------------------------------------------------------------
 *  Input......:  arg       argument pointer
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void PLDCB_SetData(void *arg, u_int8 state)		/* nodoc */
{
    if(state)
    {
        MSETMASK_D32( ((BBIS_HANDLE*)arg)->virtRRaddr,
            RR_MISC, D201_PLD_DATA );
    }
    else
    {
        MCLRMASK_D32( ((BBIS_HANDLE*)arg)->virtRRaddr,
            RR_MISC, D201_PLD_DATA );
    }
}

/****************************** PLDCB_SetDclk *******************************
 *
 *  Description:  Callback function for PLD_FLEX10K_LoadCallBk()
 *                set PLD DCLK bit
 *
 *---------------------------------------------------------------------------
 *  Input......:  arg       argument pointer
 *                state     bit setting
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void PLDCB_SetDclk(void *arg, u_int8 state)		/* nodoc */
{
    if(state)
    {
        MSETMASK_D32( ((BBIS_HANDLE*)arg)->virtRRaddr,
            RR_MISC, D201_PLD_DCLK );
    }
    else
    {
        MCLRMASK_D32( ((BBIS_HANDLE*)arg)->virtRRaddr,
            RR_MISC, D201_PLD_DCLK );
    }
}

/****************************** PLDCB_SetConfig *****************************
 *
 *  Description:  Callback function for PLD_FLEX10K_LoadCallBk()
 *                set PLD CONFIG bit
 *
 *---------------------------------------------------------------------------
 *  Input......:  arg       argument pointer
 *                state     bit setting
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void PLDCB_SetConfig(void *arg, u_int8 state)		/* nodoc */
{
    if(state)
    {
        MSETMASK_D32( ((BBIS_HANDLE*)arg)->virtRRaddr,
            RR_MISC, D201_PLD_CONFIG );
    }
    else
    {
        MCLRMASK_D32( ((BBIS_HANDLE*)arg)->virtRRaddr,
            RR_MISC, D201_PLD_CONFIG );
	}
}

/********************************* ParsePciPath ******************************
 *
 *  Description: Parses the specified PCI_BUS_PATH to find out PCI Bus Number
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: brdHdl			handle
 *  Output.....: returns:	   	error code
 *				 *pciBusNbrP	main PCI bus number of D201
 *  Globals....: -
 ****************************************************************************/
static int32 ParsePciPath( BBIS_HANDLE *brdHdl, u_int32 *pciBusNbrP ) 	/* nodoc */
{
	u_int32 i;
	int32 pciBusNbr=0, pciDevNbr;
	int32 error;
	int32 vendorID, deviceID, headerType;
	int32 secondBus = 0xff;

	/* parse whole pci path until our device is reached */
	for(i=0; i<brdHdl->pciPathLen; i++){

		pciDevNbr = brdHdl->pciPath[i];
		
		if ( ( i==0 ) && ( 0 != brdHdl->domainNbr ) ) {
			/* as we do not know the numbering order of busses on pci domains,
			   try to find the device on all busses instead of looking for the
			   first bus on the domain                                        */
			for(pciBusNbr=0; pciBusNbr<0xff; pciBusNbr++) {
				error = PciParseDev( brdHdl, OSS_MERGE_BUS_DOMAIN(pciBusNbr, brdHdl->domainNbr),
				                     brdHdl->pciPath[0], &vendorID, &deviceID, &headerType,
								     &secondBus );
				if ( error == ERR_SUCCESS )
					break; /* found device */
			}
			
			if ( error != ERR_SUCCESS ) { /* device not found */
				DBGWRT_ERR((DBH,"*** BB - %s: first device 0x%02x in pci bus path "
				                "not found on domain %d!\n",
				                BRD_NAME, brdHdl->pciPath[0], brdHdl->domainNbr ));
				return error;
			}
		} else {
			/* parse device only once */
			if( (error = PciParseDev( brdHdl, OSS_MERGE_BUS_DOMAIN(pciBusNbr, brdHdl->domainNbr),
				                      pciDevNbr, &vendorID, &deviceID, &headerType,
									  &secondBus )))
				return error;
		}

		if( vendorID == 0xffff && deviceID == 0xffff ){
			DBGWRT_ERR((DBH,"*** BB - %s:ParsePciPath: Nonexistant device "
						"domain %d bus %d dev %d\n", BRD_NAME, brdHdl->domainNbr,
						pciBusNbr, pciDevNbr ));
			return ERR_BBIS_NO_CHECKLOC;
		}

		/*--- device is present, is it a bridge ? ---*/
		if( ( headerType & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION ) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE )
		{
			DBGWRT_ERR((DBH,"*** BB - %s:ParsePciPath: Device is not a bridge!"
						"domain %d bus %d dev %d vend=0x%x devId=0x%x headerType %02x\n",
						BRD_NAME, brdHdl->domainNbr, pciBusNbr, pciDevNbr, vendorID,
						deviceID, headerType ));

			return ERR_BBIS_NO_CHECKLOC;
		}
			
		/*--- it is a bridge, determine its secondary bus number ---*/
		DBGWRT_2((DBH, " domain %d bus %d dev %d: vend=0x%x devId=0x%x second bus %d\n",
				  brdHdl->domainNbr, pciBusNbr, pciDevNbr, vendorID, deviceID, secondBus ));

		/*--- continue with new bus ---*/
		pciBusNbr = secondBus;
	}

	DBGWRT_1((DBH,"BB - %s: Main PCI Bus Number is %d\n", BRD_NAME,
	          pciBusNbr ));

	*pciBusNbrP = pciBusNbr;

	return ERR_SUCCESS;
}

/********************************* PciParseDev ******************************
 *
 *  Description: Get parameters from specified PCI device's config space
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: brdHdl		handle
 *				 pciBusNbr	pci bus number (merged with domain)
 *				 pciDevNbr  pci dev number
 *  Output.....: returns: 	error code (only fails if config access error)
 *				 *vendorIDP vendor id
 *				 *deviceIDP device id
 *				 *headerTypeP header type
 *				 *secondBusP secondary bus number (only valid for bridge)
 *  Globals....: -
 ****************************************************************************/
static int32 PciParseDev(
	BBIS_HANDLE *brdHdl,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	int32 *vendorIDP,
	int32 *deviceIDP,
	int32 *headerTypeP,
	int32 *secondBusP)		/* nodoc */
{
	int32 error;
	int32 pciMainDevNbr;
	int32 pciDevFunc;
	
	pciMainDevNbr = pciDevNbr;
	pciDevFunc = 0;

	// special handling for multifunction bridges: set function number
    if ( pciDevNbr > 0x1f ) {
    	// seperate the function number from the device number
    	pciDevFunc = pciDevNbr >> 5;
    	pciMainDevNbr = ( pciDevNbr & 0x0000001f );
    }

	/*--- check to see if device present ---*/
	error = OSS_PciGetConfig( brdHdl->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  OSS_PCI_VENDOR_ID, vendorIDP );
		
	if( error == 0 )
		error = OSS_PciGetConfig( brdHdl->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
									  OSS_PCI_DEVICE_ID, deviceIDP );

	if( error )
		return PciCfgErr(brdHdl,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,OSS_PCI_DEVICE_ID);

	if( *vendorIDP == 0xffff && *deviceIDP == 0xffff )
		return ERR_SUCCESS;		/* not present */

	/*--- device is present, is it a bridge ? ---*/
	error = OSS_PciGetConfig( brdHdl->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  OSS_PCI_HEADER_TYPE, headerTypeP );

	if( error )
		return PciCfgErr(brdHdl,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,OSS_PCI_HEADER_TYPE);
		
	DBGWRT_2((DBH, " domain %d bus %d dev %d: vend=0x%x devId=%d.%d hdrtype %d\n",
			  OSS_DOMAIN_NBR( pciBusNbr ), OSS_BUS_NBR( pciBusNbr ), pciMainDevNbr,
			  pciDevFunc, *vendorIDP, *deviceIDP, *headerTypeP ));

	if( ((*headerTypeP) & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION ) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE )	
		return ERR_SUCCESS;		/* not bridge device */

			
	/*--- it is a bridge, determine its secondary bus number ---*/
	error = OSS_PciGetConfig( brdHdl->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  PCI_SECONDARY_BUS_NUMBER | OSS_PCI_ACCESS_8,
							  secondBusP );

	if( error )
		return PciCfgErr(brdHdl,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,
						 PCI_SECONDARY_BUS_NUMBER | OSS_PCI_ACCESS_8);
		
	return ERR_SUCCESS;
}	



/********************************* PciCfgErr ********************************
 *
 *  Description: Print Debug message
 *			
 *---------------------------------------------------------------------------
 *  Input......: brdHdl			handle
 *               funcName		function name
 *               error			error code
 *				 pciBusNbr		pci bus number
 *				 pciDevNbr		pci device number
 *               reg			register
 *  Output.....: return			error code
 *  Globals....: -
 ****************************************************************************/
static int32 PciCfgErr(
	BBIS_HANDLE *brdHdl,
	char *funcName,
	int32 error,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	u_int32 reg )		/* nodoc */
{
	int32 pciMainDevNbr;
	int32 pciDevFunc;
	
	pciMainDevNbr = pciDevNbr;
	pciDevFunc = 0;

	// extract function number from device number if necessary
    if ( pciDevNbr > 0x1f ) {
    	// seperate the function number from the device number
    	pciDevFunc = pciDevNbr >> 5;
    	pciMainDevNbr = ( pciDevNbr & 0x0000001f );
    }

	DBGWRT_ERR((DBH,"*** BB - %s %s: PCI access error 0x%x "
				"domain %d bus %d dev %d.%d reg 0x%x\n", BRD_NAME, funcName, error,
				OSS_DOMAIN_NBR( pciBusNbr ), OSS_BUS_NBR( pciBusNbr ),
				pciMainDevNbr, pciDevFunc, reg ));
	return error;
}

#if defined _D201 || defined _F202
static const u_int32 localMap[] = {
	0x02000000, 0x03000000, 0x06000000, 0x07000000
};
#endif

#ifdef _F201
static const u_int32 localMap[] = {
	0x01000000, 0x00000000, 0x00000000, 0x00000000
};
#endif

#ifdef _C203
static const u_int32 localMap[] = {
	0x03000000, 0x02000000, 0x01000000, 0x04000000
};
#endif

#ifdef _C204
static const u_int32 localMap[] = {
	0x03000000, 0x00000000, 0x00000000, 0x00000000
};
#endif

/********************************* PlxBugWorkaroundStart *********************
 *
 *  Description: Set BAR0 to the value of another BAR that is at least
 *				 256 byte aligned.
 *			
 *			     Does nothing when PLX bug workaround is not enabled
 *
 *				 This routine should only be called from BrdInit(),
 *				 ClrMiface() or SetMiface().
 *	
 *				 PlxBugWorkaroundEnd() must be called when finished
 *---------------------------------------------------------------------------
 *  Input......: brdHdl			handle
 *				 slot			the slot that is currently in operation
 *  Output.....: brdHdl->virtRRaddr ready for access
 *  Globals....: -
 ****************************************************************************/
static void PlxBugWorkaroundStart( BBIS_HANDLE *brdHdl, int32 slot )	/* nodoc */
{
	if( brdHdl->plxBugWa ){
		
		/*
		 * disable the BARx in LASxBA
		 * Note: the writes will work, but reads to virtRRaddr2 not!
		 */
		MWRITE_D32( brdHdl->virtRRaddr2, 0x14 + (slot * 4), localMap[slot] );
		
		/*--- write BARx's value + 0xffff00 to BAR 0 ---*/
		OSS_PciSetConfig( brdHdl->osHdl,
		                  OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
						  brdHdl->pciDev, 0, OSS_PCI_ADDR_0,
						  brdHdl->barValue[2+slot] + D201_M_CTRL);

		/*--- setup the virtRRaddr accordingly ---*/
		brdHdl->virtRRaddr = brdHdl->virtModCtrlAddr[slot];
	}
}

/********************************* PlxBugWorkaroundEnd ***********************
 *
 *  Description: Finish PLX bug workaround
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: brdHdl			handle
 *				 slot			the slot that is currently in operation
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void PlxBugWorkaroundEnd( BBIS_HANDLE *brdHdl, int32 slot )	/* nodoc */
{
	if( brdHdl->plxBugWa ){

		/*--- rewrite org. value to BAR 0 */
		OSS_PciSetConfig( brdHdl->osHdl,
		                  OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
						  brdHdl->pciDev, 0, OSS_PCI_ADDR_0,
						  brdHdl->barValue[0]);
		
		/*--- enable again BARx space ---*/
		MWRITE_D32( brdHdl->virtRRaddr2, 0x14 + (slot * 4), localMap[slot]+0x1 );

		/*--- rewrite org. value to BAR 2..5 */
		OSS_PciSetConfig( brdHdl->osHdl,
		                  OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
						  brdHdl->pciDev, 0, OSS_PCI_ADDR_2+slot,
						  brdHdl->barValue[2+slot]);
		
		brdHdl->virtRRaddr = brdHdl->virtRRaddr2;
	}
}

/********************************* CfgInfoSlot ******************************
 *
 *  Description:  Fulfils the BB_CfgInfo(BBIS_CFGINFO_SLOT) request
 *
 *				  The variable-argument list (argptr) contains the following
 *                parameters in the given order:
 *
 *                Input
 *                -----
 *                mSlot (u_int32) - device slot number
 *
 *                Output
 *                ------
 *                occupied (u_int32*) - occupied information
 *                 - pluggable device:
 *                   BBIS_SLOT_OCCUP_YES if slot is occupied
 *                   or BBIS_SLOT_OCCUP_NO if slot is empty
 *                 - onboard device:
 *                   BBIS_SLOT_OCCUP_ALW if device is enabled
 *                   or BBIS_SLOT_OCCUP_DIS if device is disabled
 *
 *                devId (u_int32*) - device id (4-byte hex value)
 *                  The device id should identify the type of the device
 *                  but should not contain enough information to differentiate
 *                  between two identical devices. If the device id is unknown
 *                  BBIS_SLOT_NBR_UNK must be returned.
 *                  - M-Module:
 *                    id-prom-magic-word << 16 | id-prom-module-id
 *                    Example: 0x53460024
 *                    Note: The returned device id must be identical to the
 *                          "autoid" value in the device drivers xml file.
 *
 *                devRev (u_int32*) - device revision (4-byte hex value)
 *                  - M-Module: id-prom-layout-revision << 16 |
 *                              id-prom-product-variant
 *                              example: 0x01091400
 *                    or BBIS_SLOT_NBR_UNK if device revision is unknown
 *
 *                slotName (char*) - slot name
 *                  The slot name should consist of the slot type and the
 *                  slot label but must not contain any non-printing
 *                  characters.
 *                  The length of the returned string, including the
 *                  terminating null character, must not exceed
 *                  BBIS_SLOT_STR_MAXSIZE.
 *                  format : "<slot type> <slot label>"
 *
 *                  Examples:
 *                  - M-Module:		"M-Module slot 0"
 *
 *                devName (char*) - device name
 *                  The device name should identify the type of the device
 *                  but should not contain enough information to differentiate
 *                  between two identical devices. Furthermore, the device
 *                  name should refer to the appropriate device driver name if
 *                  possible.
 *
 *                  The returned string must not contain any non-printing
 *                  characters or blanks. The length of the returned string,
 *                  including the terminating null character, must not exceed
 *                  BBIS_SLOT_STR_MAXSIZE.
 *
 *                  Examples:
 *                  - M-Module:		"M34", "MS9"
 *
 *                  If the device name is unknown BBIS_SLOT_STR_UNK must
 *                  be returned.
 *
 *                  Note: The returned device name must be identical to the
 *                        "hwname" value in the device drivers xml file.
 *
 *---------------------------------------------------------------------------
 *  Input......: brdHdl			handle
 *				 argptr			argument pointer
 *  Output.....: return			error code
 *  Globals....: -
 ****************************************************************************/
static int32 CfgInfoSlot( BBIS_HANDLE *brdHdl, va_list argptr )	/* nodoc */
{
	int32	status, status2;
	u_int32	modAddrSize;
	u_int16	irqSetting;
	void	*physModAddr, *virtModAddr;
	u_int32 modType;

	u_int32 mSlot     = va_arg( argptr, u_int32 );
    u_int32 *occupied = va_arg( argptr, u_int32* );
    u_int32 *devId    = va_arg( argptr, u_int32* );
    u_int32 *devRev   = va_arg( argptr, u_int32* );
	char	*slotName = va_arg( argptr, char* );
	char	*devName  = va_arg( argptr, char* );

	/* clear parameters to return */
	*occupied = 0;
	*devId    = 0;
	*devRev   = 0;
	*slotName = '\0';
	*devName  = '\0';

	/* illegal slot? */
	if( mSlot > (BRD_SLOT_NBR-1) ){
		DBGWRT_ERR((DBH,"*** %s_CfgInfoSlot: wrong module slot number=0x%x\n",
					BRD_NAME,mSlot));
		return ERR_BBIS_ILL_SLOT;
	}

	/*
	 * build slot name
	 */
	OSS_Sprintf( brdHdl->osHdl, slotName, "M-Module slot %d", mSlot);

	/*------------------------------+
	| get module information        |
	+------------------------------*/
	INT_DISABLE( brdHdl );	/* disable interrupt */

	/* get physical module addr */
	if( (status = D201_GetMAddr( brdHdl, mSlot, MDIS_MA08, MDIS_MD16,
								 &physModAddr, &modAddrSize )) ){
		INT_ENABLE( brdHdl );	/* enable interrupt */
		return status;
	}
	
	/* init module slot */
	if( (status = D201_SetMIface( brdHdl, mSlot, MDIS_MA08, MDIS_MD16 )) ){
		INT_ENABLE( brdHdl );	/* enable interrupt */
		return status;
	}
	
	/*
	 * map module address space
	 * (quick and dirty without resource assignment)
	 */
	if( (status = OSS_MapPhysToVirtAddr( brdHdl->osHdl, physModAddr, modAddrSize,
										 OSS_ADDRSPACE_MEM, OSS_BUSTYPE_PCI,
										 OSS_MERGE_BUS_DOMAIN(brdHdl->busNbr, brdHdl->domainNbr),
										 &virtModAddr )) ){
		D201_ClrMIface( brdHdl, mSlot );
		INT_ENABLE( brdHdl );	/* enable interrupt */
		return status;
	}

	/* now get the information */
	m_getmodinfo( (U_INT32_OR_64)virtModAddr, &modType,
				  devId, devRev, devName );

	DBGWRT_2((DBH," modType=%d, devId=0x%08x, devRev=0x%08x, devName=%s\n",
				  modType, *devId, *devRev, devName ));

	/*
	 * If a m-module hasn't asserted DTACK# within 16us
	 * after the assertion of CS# we get an timeout interrupt.
	 * In this case, we asume that no m-module is plugged in the slot.
	 */
	OSS_MikroDelay(brdHdl->osHdl, 20);	/* wait 20us */

	/* get the interrupt information */
	irqSetting = MREAD_D16(brdHdl->virtModCtrlAddr[mSlot], D201_M_INT);

	/*------------------------------+
	| slot is not occupied          |
	+------------------------------*/
	if(irqSetting & D201_M_INT_TIMEOUT){
		DBGWRT_2((DBH," mSlot=%d - interrupt by timeout\n", mSlot));

		/* clear timeout interrupt */
		MCLRMASK_D16( brdHdl->virtModCtrlAddr[mSlot], D201_M_INT,
					  D201_M_INT_TIMEOUT );
		
		/* set occupied info */
		*occupied = BBIS_SLOT_OCCUP_NO;
	}
	/*------------------------------+
	| slot is occupied              |
	+------------------------------*/
	else {
		/* set occupied info */
		*occupied = BBIS_SLOT_OCCUP_YES;	
	}

	/* unmap module address space */
	status = OSS_UnMapVirtAddr(brdHdl->osHdl, &virtModAddr,
							   modAddrSize, OSS_ADDRSPACE_MEM);
	/* clear module slot */
	status2 = D201_ClrMIface( brdHdl, mSlot );

	INT_ENABLE( brdHdl );	/* enable interrupt */

	/* return on error */
	if( status )
		return status;
	if( status2 )
		return status2;

	/* return on success */
	return ERR_SUCCESS;
}







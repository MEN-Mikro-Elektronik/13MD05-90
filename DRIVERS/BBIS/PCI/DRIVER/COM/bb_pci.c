/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_pci.c
 *      Project: PCI board handler
 *
 *       Author: kp
 *        $Date: 2012/08/08 11:05:08 $
 *    $Revision: 1.12 $
 *
 *  Description: Generic PCI Base Board handler
 *
 *  Can be used for any PCI base board like D202, F203 or PCI Devices
 *  that reside onboard on CPU cards like D2, A11.
 *
 *  MDIS and PCI BBIS does not use the plug-and-play philosophy of the PCI
 *  Bus, i.e. we do not search for devices. The user has to specify the
 *  location on the bus in the board descriptor.
 *
 *  This ensures that a specific device has always the same name in
 *  the system, even if additional devices were added or the BIOS
 *  rearanges the PCI bus numbers.
 *
 *  One instance of this PCI BBIS handles exactly one bus number in the
 *  PCI system. The user has to specify the path to this bus using the
 *  descriptor key PCI_BUS_PATH. This is a list of PCI device numbers,
 *  that must belong to PCI-to-PCI bridges.
 *
 *  Example: To specify the PCI bus on PC-MIP Compact-PCI carrier board
 *			 in slot 4 of the CPCI backplane the user has to specify the
 *			 following path: (CPU board=MEN D001)
 *
 *			 PCI_BUS_PATH = BINARY 0x14,0x0d
 *
 *			 0x14 is the device number on Bus 0 of the PCI-to-PCI bridge
 *			 on the D1, that connects the internal PCI bus to the CPCI
 *			 backplane.
 *
 *			 0x0d is the device number on the CPCI backplane for slot 4.
 *
 *  PCI_BUS_PATH may be empty, if PCI bus 0 is to be controlled.
 *
 *  It is also possible to specify the key PCI_BUS_NUMBER in the descriptor.
 *  If present, it overrides PCI_BUS_PATH. But note that the bus number may
 *  change if additional devices are installed in the PCI system.
 *
 *  The different devices on the specified PCI bus are referenced using
 *  MDIS slot numbers. Therefore the PCI BBIS maintains a table that
 *  contains the PCI device number for each slot.
 *
 *  The descriptor must contain DEVICE_SLOT_<n> keys, where <n> specified
 *  the MDIS slot number and the value of this key must be the PCI device
 *  number.
 *
 *  E.g. for the A11 onboard PC-MIP slots, the device descriptor must
 *  contain the following entries
 *
 *		DEVICE_SLOT_0		= U_INT32  0x00
 *		DEVICE_SLOT_1		= U_INT32  0x01
 *
 *  because the device selects are mapped 1:1 to the geographical slots.
 *
 *  Enhancement for PnP OS:
 *  -----------------------
 *  Two optional descriptor keys are used for PnP operating systems (e.g. W2k):
 *   - FUNCTION_SLOT_<n> = <PCI function number for MDIS slot number n>
 *   - DEVNAME_SLOT_<n>  = <device name for MDIS slot number n>
 *
 *  The PCI function number (default value = 0) is necessary to build the
 *  slot information on PCI_CfgInfo(BBIS_CFGINFO_SLOT) request. It can be
 *  queried via the PCI_CfgInfo(BBIS_CFGINFO_PCI_FUNCNBR) request.
 *
 *  The device name will be returned on the PCI_CfgInfo(BBIS_CFGINFO_SLOT)
 *  request.
 *
 *  Operation:
 *  ----------
 *  During Init, the PCI BBIS looks through the given PCI_BUS_PATH. Each
 *  specified device must be a PCI-PCI bridge. It then gets the secondary
 *  bus number behind the bridge and looks at the next entry in PCI_BUS_PATH.
 *
 *  It then looks at each device specified with DEVICE_SLOT_<n> to see
 *  if the device has internal bridges. If so, it searches for devices
 *  behind the internal bridge. The search must detect exactly one device.
 *  If more or less are found, an error is returned.
 *  If again a brigde is found, the search continues until a non-bridge
 *  device is found.
 *
 *  If the device has an internal bridge, PCI BBIS returns the PCI bus/dev
 *  number on that internal bus to MDIS
 *
 *     Required: ---
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_pci.c,v $
 * Revision 1.12  2012/08/08 11:05:08  ts
 * R: 1. using bridge as MDIS target MDIS wasnt possible since recursive DevBridgeCheck
 *    2. devices behind Pericom bridge on F223 were not evaluated
 * M: 1. introduced descriptor key SKIP_DEV_BRIDGE_CHECK to skip recursive search
 *    2. fixed bug in condition if a found device is bridge in CheckDevBridge()
 *
 * Revision 1.11  2011/05/19 16:00:42  CRuff
 * R: 1. support of pci domains
 * M: 1a) added handling of code BBIS_CFGINFO_PCI_DOMAIN to CHAMELEON_CfgInfo()
 *    1b) added new member pciDomainNbr to BBIS_HANDLE
 *    1c) extend debug prints of pci bus number to also print the domain number
 *    1d) call all occurrences of OSS_PciXetConfig with merged bus and domain
 *    1e) in ParsePciPath(): look for root device on all busses instead of
 *        expecting root device on bus 0
 *
 * Revision 1.10  2009/08/07 15:33:48  CRuff
 * R:1. Porting to MDIS5 (according porting guide rev. 0.7)
 *   2. DESC handle not closed in init routine
 * M:1.a) added support for 64bit (API)
 *     b) support multifunction bridges in PCI bus path
 *   2. added DESC_Exit() in PCI_Init()
 *
 * Revision 1.9  2006/12/20 12:48:16  ufranke
 * fixed
 *  - function pointer setIrqHandle should be NULL if not implemented
 *    or must be implemented completely
 *
 * Revision 1.8  2006/12/15 16:18:51  ts
 * replaced BBIS fkt13 with setIrqHandle
 *
 * Revision 1.7  2005/06/21 14:48:06  UFranke
 * cosmetics
 *
 * Revision 1.6  2004/09/07 15:58:51  dpfeuffer
 * slot range check was wrong
 *
 * Revision 1.5  2004/06/21 14:06:18  dpfeuffer
 * Enhancements for PnP OS implemented:
 * - PCI_BrdInfo(BBIS_BRDINFO_BRDNAME)
 * - PCI_CfgInfo(BBIS_CFGINFO_SLOT)
 * - PCI_CfgInfo(BBIS_CFGINFO_PCI_FUNCNBR)
 * - descriptor key FUNCTION_SLOT_n and DEVNAME_SLOT_n added
 *
 * Revision 1.4  2001/12/07 09:41:49  Franke
 * cosmetics
 *
 * Revision 1.3  2000/08/30 16:36:36  Schmidt
 * bb_pci.h added
 *
 * Revision 1.2  1999/08/11 15:53:03  Schmidt
 * cosmetics
 * PciParseDev : OSS_PciGetConfig - register access mask was wrong
 *
 * Revision 1.1  1999/08/09 15:25:37  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#define _NO_BBIS_HANDLE		/* bb_defs.h: don't define BBIS_HANDLE struct */

#include <MEN/mdis_com.h>
#include <MEN/men_typs.h>   /* system dependend definitions   */
#include <MEN/dbg.h>        /* debug functions                */
#include <MEN/oss.h>        /* oss functions                  */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/bb_defs.h>    /* bbis definitions				  */
#include <MEN/mdis_err.h>   /* MDIS error codes               */
#include <MEN/mdis_api.h>   /* MDIS global defs               */

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL		h->debugLevel
#define DBH             h->debugHdl


#define BBNAME				"PCI"
#define PCI_BBIS_MAX_DEVS	16		/* max number of devices supported */
#define PCI_BBIS_MAX_PATH	16		/* max number of bridges to devices */
#define PCI_NO_DEV			-1		/* flags pciDevNbr invalid */

#define PCI_SECONDARY_BUS_NUMBER 0x19 /* PCI bridge config */
#define DEVNAME_SIZE		30

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
typedef struct {
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table		*/
    u_int32     ownMemSize;			/* own memory size			*/
    OSS_HANDLE* osHdl;				/* os specific handle		*/
    DESC_HANDLE *descHdl;			/* descriptor handle pointer*/
    u_int32     debugLevel;			/* debug level for BBIS     */
	DBG_HANDLE  *debugHdl;			/* debug handle				*/
	int32		skipDevBridgeCheck;				/* if PCI bridge is given
                                                   use it as MDIS PCI target */	
	int32		pciDomainNbr;					/* PCI domain number for
                                                   handled devices */
	int32		pciBusNbr[PCI_BBIS_MAX_DEVS];	/* PCI bus number for
												   handled devices */
	int32		pciDevNbr[PCI_BBIS_MAX_DEVS];	/* device number of devices */
	int32		pciFuncNbr[PCI_BBIS_MAX_DEVS];	/* function number of devices */
	char		devName[DEVNAME_SIZE][PCI_BBIS_MAX_DEVS];		/* name of devices */
	int32		devCount;			/* number of devices specified in desc */
	u_int8		pciPath[PCI_BBIS_MAX_PATH];	/* PCI path from desc */
	u_int32		pciPathLen;		/* number of bytes in pciPath */
} BBIS_HANDLE;

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>	/* bbis jumptable		*/
#include <MEN/bb_pci.h>		/* PCI bbis header file	*/

	
/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* init/exit */
static int32 PCI_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 PCI_BrdInit(BBIS_HANDLE*);
static int32 PCI_BrdExit(BBIS_HANDLE*);
static int32 PCI_Exit(BBIS_HANDLE**);
/* info */
static int32 PCI_BrdInfo(u_int32, ...);
static int32 PCI_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 PCI_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 PCI_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  PCI_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 PCI_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 PCI_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 PCI_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 PCI_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 PCI_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 PCI_SetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64);
static int32 PCI_GetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64*);
/* unused */
static int32 PCI_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *h, int32 retCode);

static int32 ParsePciPath( BBIS_HANDLE *h, u_int32 *pciBusNbrP );
static int32 CheckDevBridge(
	BBIS_HANDLE *h,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	u_int32 *newPciBusNbr,
	u_int32 *newPciDevNbr);
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

static int32 CfgInfoSlot( BBIS_HANDLE *h, va_list argptr );

/**************************** PCI_GetEntry ***********************************
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
	extern void PCI_GetEntry( BBIS_ENTRY *bbisP )
#endif
{
    /* init/exit */
    bbisP->init         =   PCI_Init;
    bbisP->brdInit      =   PCI_BrdInit;
    bbisP->brdExit      =   PCI_BrdExit;
    bbisP->exit         =   PCI_Exit;
    bbisP->fkt04        =   PCI_Unused;
    /* info */
    bbisP->brdInfo      =   PCI_BrdInfo;
    bbisP->cfgInfo      =   PCI_CfgInfo;
    bbisP->fkt07        =   PCI_Unused;
    bbisP->fkt08        =   PCI_Unused;
    bbisP->fkt09        =   PCI_Unused;
    /* interrupt handling */
    bbisP->irqEnable    =   PCI_IrqEnable;
    bbisP->irqSrvInit   =   PCI_IrqSrvInit;
    bbisP->irqSrvExit   =   PCI_IrqSrvExit;
    bbisP->setIrqHandle =   NULL;
    bbisP->fkt14        =   PCI_Unused;
    /* exception handling */
    bbisP->expEnable    =   PCI_ExpEnable;
    bbisP->expSrv       =   PCI_ExpSrv;
    bbisP->fkt17        =   PCI_Unused;
    bbisP->fkt18        =   PCI_Unused;
    bbisP->fkt19        =   PCI_Unused;
    /* */
    bbisP->fkt20        =   PCI_Unused;
    bbisP->fkt21        =   PCI_Unused;
    bbisP->fkt22        =   PCI_Unused;
    bbisP->fkt23        =   PCI_Unused;
    bbisP->fkt24        =   PCI_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   PCI_SetStat;
    bbisP->getStat      =   PCI_GetStat;
    bbisP->setMIface    =   PCI_SetMIface;
    bbisP->clrMIface    =   PCI_ClrMIface;
    bbisP->getMAddr     =   PCI_GetMAddr;
    bbisP->fkt30        =   PCI_Unused;
    bbisP->fkt31        =   PCI_Unused;
}

/****************************** PCI_Init *************************************
 *
 *  Description:  Allocate and return board handle.
 *
 *                - initializes the board handle
 *                - reads and saves board descriptor entries
 *
 *                The following descriptor keys are used:
 *
 *                Deskriptor key           Default          Range
 *                -----------------------  ---------------  -------------
 *                DEBUG_LEVEL_DESC         OSS_DBG_DEFAULT  see dbg.h
 *                DEBUG_LEVEL              OSS_DBG_DEFAULT  see dbg.h
 *                PCI_BUS_PATH             -
 *				  PCI_BUS_NUMBER		   -
 *                PCI_DOMAIN_NUMBER        0
 * 				  SKIP_DEV_BRIDGE_CHECK    0 (check enabled)
 *                DEVICE_SLOT_n			   -                0...31
 *                FUNCTION_SLOT_n          0                0...7
 *                DEVNAME_SLOT_n         BBIS_SLOT_STR_UNK  DEVNAME_SIZE
 *
 *                ( n = 0..15 )
 *
 *                Either PCI_BUS_NUMBER or PCI_BUS_PATH must be specified
 *				  If both are present, PCI_BUS_NUMBER takes precedence.
 *
 *---------------------------------------------------------------------------
 *  Input......:  osHdl     pointer to os specific structure
 *                descSpec  pointer to os specific descriptor specifier
 *                hP   pointer to not initialized board handle structure
 *  Output.....:  *hP  initialized board handle structure
 *				  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_Init(
    OSS_HANDLE      *osHdl,
    DESC_SPEC       *descSpec,
    BBIS_HANDLE     **hP )
{
    BBIS_HANDLE	*h = NULL;
	u_int32     gotsize, i;
    int32       status;
    u_int32		value, pciBusNbr, devNameSize;

    /*-------------------------------+
    | initialize the board structure |
    +-------------------------------*/
    /* get memory for the board structure */
    *hP = h = (BBIS_HANDLE*) (OSS_MemGet(
        osHdl, sizeof(BBIS_HANDLE), &gotsize ));
    if ( h == NULL )
        return ERR_OSS_MEM_ALLOC;

    /* store data into the board structure */
    h->ownMemSize = gotsize;
    h->osHdl = osHdl;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* drivers ident function */
	h->idFuncTbl.idCall[0].identCall = Ident;
	/* libraries ident functions */
	h->idFuncTbl.idCall[1].identCall = DESC_Ident;
	h->idFuncTbl.idCall[2].identCall = OSS_Ident;
	/* terminator */
	h->idFuncTbl.idCall[3].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    DBGWRT_1((DBH,"BB - %s_Init\n",BBNAME));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
    /* init descHdl */
    status = DESC_Init( descSpec, osHdl, &h->descHdl );
    if (status)
		return( Cleanup(h,status) );

    /* get DEBUG_LEVEL_DESC */
    status = DESC_GetUInt32(h->descHdl, OSS_DBG_DEFAULT, &value,
				"DEBUG_LEVEL_DESC");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(h,status) );

	/* set debug level for DESC module */
	DESC_DbgLevelSet(h->descHdl, value);

    /* get DEBUG_LEVEL */
    status = DESC_GetUInt32( h->descHdl, OSS_DBG_DEFAULT,
							 &(h->debugLevel),
                "DEBUG_LEVEL");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(h,status) );

	/* PCI_DOMAIN_NUMBER - optional */
	status = DESC_GetUInt32( h->descHdl, 0, &h->pciDomainNbr,
							 "PCI_DOMAIN_NUMBER");							

	/* SKIP_DEV_BRIDGE_CHECK - optional */
	status = DESC_GetUInt32( h->descHdl, 0, &h->skipDevBridgeCheck,
							 "SKIP_DEV_BRIDGE_CHECK");

	/*--- get PCI bus ---*/
    status = DESC_GetUInt32( h->descHdl, 0,
							 &pciBusNbr, "PCI_BUS_NUMBER");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(h,status) );

	if( status == ERR_DESC_KEY_NOTFOUND ){

		/*--- get PCI path ---*/
		h->pciPathLen = PCI_BBIS_MAX_PATH;

		status = DESC_GetBinary( h->descHdl, (u_int8*)"", 0,
								 h->pciPath, &h->pciPathLen,
								 "PCI_BUS_PATH");

		if( status ){
			DBGWRT_ERR((DBH, "*** %s_Init: Desc Key PCI_BUS_PATH "
						"not found!\n",	BBNAME));
			return( Cleanup(h,status) );
		}
		/*------------------------------------------------------------+
		|  Now parse the PCI_PATH to determine bus number of devices  |
		+------------------------------------------------------------*/
#ifdef DBG	
		DBGWRT_2((DBH, " PCI_PATH="));
		for(i=0; i<h->pciPathLen; i++){
			DBGWRT_2((DBH, "0x%x", h->pciPath[i]));
		}
		DBGWRT_2((DBH, "\n"));
#endif
		status = ParsePciPath( h, &pciBusNbr );

		if( status ){
			return( Cleanup(h,status));
		}
	} else {
		DBGWRT_1((DBH,"BB - %s: Using main PCI Bus Number from desc %d"
		               "on PCI Domain %d\n",
				  BBNAME, pciBusNbr, h->pciDomainNbr ));
	}
	
	h->devCount = 0;

	for( i=0; i<PCI_BBIS_MAX_DEVS; i++ ){
		h->pciDevNbr[i] = PCI_NO_DEV;

		/* get DEVICE_SLOT_n */
		status = DESC_GetUInt32( h->descHdl, 0, &value,
								 "DEVICE_SLOT_%d", i);
		
		/* DEVICE_SLOT_n specified? */
		if( status == ERR_SUCCESS ){
			
			DBGWRT_2(( DBH, " DEVICE_SLOT_%d = 0x%x\n", i, value ));
		
			/*--- check for valid device number ---*/
			if( value <= 31 )
				h->pciDevNbr[i] = (int32)value;
			else {
				DBGWRT_ERR((DBH, "*** %s_Init: Illegal PCI DevNbr for "
							"slot %d: 0x%x\n", BBNAME, i, value));
				return( Cleanup(h,ERR_BBIS_DESC_PARAM) );
			}
			h->devCount++;
		
			/* get FUNCTION_SLOT_n */
			status = DESC_GetUInt32( h->descHdl, 0, &value,
									 "FUNCTION_SLOT_%d", i );
	#if DBG		
			if( status == ERR_SUCCESS )
				DBGWRT_2(( DBH, " FUNCTION_SLOT_%d = 0x%x\n", i, value ));
	#endif

			/*--- check for valid function number ---*/
			if( value <= 7 ){
				h->pciFuncNbr[i] = (int32)value;
			}
			else {
				DBGWRT_ERR((DBH, "*** %s_Init: Illegal PCI FuncNbr for "
							"slot %d: 0x%x\n", BBNAME, i, value));
				return( Cleanup(h,ERR_BBIS_DESC_PARAM) );
			}

			/* get DEVNAME_SLOT_n */
			devNameSize = DEVNAME_SIZE;
			status = DESC_GetString( h->descHdl, BBIS_SLOT_STR_UNK, h->devName[i],
									 &devNameSize, "DEVNAME_SLOT_%d", i );
			if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
				return( Cleanup(h,status) );
	#if DBG		
			if( status == ERR_SUCCESS )
				DBGWRT_2(( DBH, " DEVNAME_SLOT_%d = %s\n", i, h->devName[i] ));
	#endif
		
		}/* DEVICE_SLOT_n specified? */
	}/*for*/
	
	/* exit descHdl */
    status = DESC_Exit( &h->descHdl );
    if(status)
        return( Cleanup(h,status) );
        		
	/*--- check if any device specified ---*/
	if( h->devCount == 0 ){
		DBGWRT_ERR((DBH, "*** %s_Init: No devices in descriptor!\n", BBNAME));
		return( Cleanup(h,ERR_BBIS_DESC_PARAM) );
	}

	/*----------------------------------------------------+
	|  Check for additional bridges on specified devices  |
	+----------------------------------------------------*/

	for( i=0; i<PCI_BBIS_MAX_DEVS; i++ ){

		if( h->pciDevNbr[i] != PCI_NO_DEV ) {

			status = CheckDevBridge( h, pciBusNbr, h->pciDevNbr[i],
									 (u_int32 *)&h->pciBusNbr[i],
									 (u_int32 *)&h->pciDevNbr[i]);

			if( status ){
				return( Cleanup(h,status));
			}
		}
	}

    return 0;
}

/****************************** PCI_BrdInit **********************************
 *
 *  Description:  Board initialization.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_BrdInit(
    BBIS_HANDLE     *h )
{
	DBGWRT_1((DBH, "BB - %s_BrdInit\n",BBNAME));

	return 0;
}

/****************************** PCI_BrdExit **********************************
 *
 *  Description:  Board deinitialization.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_BrdExit(
    BBIS_HANDLE     *h )
{
	DBGWRT_1((DBH, "BB - %s_BrdExit\n",BBNAME));

    return 0;
}

/****************************** PCI_Exit *************************************
 *
 *  Description:  Cleanup memory.
 *
 *                - deinitializes the bbis handle
 *
 *---------------------------------------------------------------------------
 *  Input......:  hP   pointer to board handle structure
 *  Output.....:  *hP  NULL
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_Exit(
    BBIS_HANDLE     **hP )
{
    BBIS_HANDLE	*h = *hP;
	int32		error = 0;

    DBGWRT_1((DBH, "BB - %s_Exit\n",BBNAME));

    /*------------------------------+
    |  de-init hardware             |
    +------------------------------*/
	/* nothing to do */

    /*------------------------------+
    |  cleanup memory               |
    +------------------------------*/
	error = Cleanup(h, error);
    *hP = NULL;

    return error;
}

/****************************** PCI_BrdInfo **********************************
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
 *                the specified board. (here always PCI)
 *
 *                The BBIS_BRDINFO_DEVBUSTYPE code returns the bustype of
 *                the specified device - not the board bus type.
 * 				  (here always PCI)
 *
 *                The BBIS_BRDINFO_FUNCTION code returns the information
 *                if an optional BBIS function is supported or not.
 *
 *                The BBIS_BRDINFO_NUM_SLOTS code returns the number of
 *                devices used from the driver. (PCI BBIS: always 16)
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
static int32 PCI_BrdInfo(
    u_int32 code,
    ... )
{
	int32		status = ERR_SUCCESS;
    va_list     argptr;
	u_int32		dummy;

    va_start(argptr,code);

    switch ( code ) {

        /* supported functions */
        case BBIS_BRDINFO_FUNCTION:
        {
            u_int32 funcCode = va_arg( argptr, u_int32 );
            u_int32 *status = va_arg( argptr, u_int32* );

			dummy = funcCode;
			/* no optional BBIS function do anything */
            *status = FALSE;
            break;
        }

        /* number of devices */
        case BBIS_BRDINFO_NUM_SLOTS:
        {
            u_int32 *numSlot = va_arg( argptr, u_int32* );

			/*
			 * No board handle here, return maximum
			 */
            *numSlot = PCI_BBIS_MAX_DEVS;
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

			dummy = mSlot;
			*devBusType = OSS_BUSTYPE_PCI;
            break;
        }

        /* interrupt capability */
        case BBIS_BRDINFO_INTERRUPTS:
        {
            u_int32 mSlot = va_arg( argptr, u_int32 );
            u_int32 *irqP = va_arg( argptr, u_int32* );

			dummy = mSlot;
            *irqP = BBIS_IRQ_DEVIRQ;
            break;
        }

        /* address space type */
        case BBIS_BRDINFO_ADDRSPACE:
        {
            u_int32 mSlot      = va_arg( argptr, u_int32 );
            u_int32 *addrSpace = va_arg( argptr, u_int32* );

			dummy = mSlot;
			/*
			 * Indeed we can return anything here. MK will
			 * determine the addresspace isself
			 */
			*addrSpace = OSS_ADDRSPACE_MEM;
            break;
        }

		/* board name */
		case BBIS_BRDINFO_BRDNAME:
		{
			char	*brdName = va_arg( argptr, char* );
			char	*from;

			/*
			 * build hw name
			 */
			from = "PCI device";
			while( (*brdName++ = *from++) );	/* copy string */
			
			break;
		}

        /* error */
        default:
            status = ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** PCI_CfgInfo **********************************
 *
 *  Description:  Get information about board configuration.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  ------------------------------
 *                BBIS_CFGINFO_BUSNBR       PCI bus number
 *                BBIS_CFGINFO_PCI_DOMAIN   PCI domain number
 *				  BBIS_CFGINFO_PCI_DEVNBR   PCI device number
 *				  BBIS_CFGINFO_PCI_FUNCNBR  PCI function number
 *                BBIS_CFGINFO_SLOT         slot information
 *
 *                The BBIS_CFGINFO_BUSNBR code returns the number of the
 *                bus on which the specified device resides
 *
 *                The BBIS_CFGINFO_PCI_DOMAIN code returns the number of
 *                the PCI domain on which the specified device resides
 *                (introduced 2011)
 *
 *                The BBIS_CFGINFO_PCI_DEVNBR code returns the device number
 *                on the PCI bus on which the specified device resides
 *
 *                The BBIS_CFGINFO_PCI_FUNCNBR code returns the function
 *				  number on the PCI bus on which the specified device resides
 *
 *                The BBIS_CFGINFO_SLOT code returns the following
 *                information about the specified device slot:
 *                The slot is occupied or empty, the device id and device
 *                revision of the plugged device, the name of the slot and
 *                the name of the plugged device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  h			pointer to board handle structure
 *                code      reference to the information we need
 *                ...       variable arguments
 *  Output.....:  ...       variable arguments
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_CfgInfo(
    BBIS_HANDLE     *h,
    u_int32         code,
    ... )
{
    va_list		argptr;
    int32       status=0;


    DBGWRT_1((DBH, "BB - %s_CfgInfo\n",BBNAME));

    va_start(argptr,code);

    switch ( code ) {
        /* bus number */
        case BBIS_CFGINFO_BUSNBR:
        {
            u_int32 *busNbr = va_arg( argptr, u_int32* );
            u_int32 mSlot   = va_arg( argptr, u_int32 );

			if ( (mSlot >= PCI_BBIS_MAX_DEVS) ||
				 (h->pciDevNbr[mSlot] == PCI_NO_DEV ))
				status = ERR_BBIS_ILL_SLOT;
			else
				*busNbr = h->pciBusNbr[mSlot];

            break;
        }

		case BBIS_CFGINFO_PCI_DOMAIN:
		{
			/* domain number */
			u_int32 *domainNbr = va_arg( argptr, u_int32* );
            u_int32 mSlot      = va_arg( argptr, u_int32 );

			if ( (mSlot >= PCI_BBIS_MAX_DEVS) ||
				 (h->pciDevNbr[mSlot] == PCI_NO_DEV ))
				status = ERR_BBIS_ILL_SLOT;
			else
				*domainNbr = h->pciDomainNbr;
				
			break;
		}	
		
        case BBIS_CFGINFO_PCI_DEVNBR:
        {
            u_int32 mSlot   = va_arg( argptr, u_int32 );
            u_int32 *devNbr = va_arg( argptr, u_int32* );

			if ( (mSlot >= PCI_BBIS_MAX_DEVS) ||
				 (h->pciDevNbr[mSlot] == PCI_NO_DEV ))
				status = ERR_BBIS_ILL_SLOT;
			else
				*devNbr = h->pciDevNbr[mSlot];

            break;
        }

        case BBIS_CFGINFO_PCI_FUNCNBR:
        {
            u_int32 mSlot   = va_arg( argptr, u_int32 );
            u_int32 *devNbr = va_arg( argptr, u_int32* );

			if ( (mSlot >= PCI_BBIS_MAX_DEVS) ||
				 (h->pciDevNbr[mSlot] == PCI_NO_DEV ))
				status = ERR_BBIS_ILL_SLOT;
			else
				*devNbr = h->pciFuncNbr[mSlot];

            break;
        }

		/* slot information for PnP support*/
		case BBIS_CFGINFO_SLOT:
		{
			status = CfgInfoSlot( h, argptr );
			break;
		}

		/* error */
        default:
			DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",
						BBNAME,code));
            va_end( argptr );
            return ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** PCI_IrqEnable ********************************
 *
 *  Description:  Interrupt enable / disable.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *                enable    interrupt setting
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_IrqEnable(
    BBIS_HANDLE     *h,
    u_int32         mSlot,
    u_int32         enable )
{
    DBGWRT_1((DBH, "BB - %s_IrqEnable: mSlot=%d enable=%d\n",
			  BBNAME,mSlot,enable));

	return 0;
}

/****************************** PCI_IrqSrvInit *******************************
 *
 *  Description:  Called at the beginning of an interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    BBIS_IRQ_UNK
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_IrqSrvInit(
    BBIS_HANDLE     *h,
    u_int32         mSlot)
{
	IDBGWRT_1((DBH, ">> BB - %s_IrqSrvInit: mSlot=%d\n",
			   BBNAME,mSlot));

    return BBIS_IRQ_UNK;
}

/****************************** PCI_IrqSrvExit *******************************
 *
 *  Description:  Called at the end of an interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void PCI_IrqSrvExit(
    BBIS_HANDLE     *h,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, ">> BB - %s_IrqSrvExit: mSlot=%d\n",BBNAME,mSlot));
}

/****************************** PCI_ExpEnable ********************************
 *
 *  Description:  Exception interrupt enable / disable.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *                enable    interrupt setting
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_ExpEnable(
    BBIS_HANDLE     *h,
    u_int32         mSlot,
	u_int32			enable)
{
	IDBGWRT_1((DBH, "BB - %s_ExpEnable: mSlot=%d\n",BBNAME,mSlot));

	return 0;
}

/****************************** PCI_ExpSrv ***********************************
 *
 *  Description:  Called at the beginning of an exception interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    BBIS_IRQ_NO
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_ExpSrv(
    BBIS_HANDLE     *h,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_ExpSrv: mSlot=%d\n",BBNAME,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** PCI_SetMIface ********************************
 *
 *  Description:  Set device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24
 *                dataMode  MDIS_MODE_PCI6 | MDIS_MODE_D32
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_SetMIface(
    BBIS_HANDLE     *h,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode)
{
	DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=%d\n",BBNAME,mSlot));

    return 0;
}

/****************************** PCI_ClrMIface ********************************
 *
 *  Description:  Clear device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_ClrMIface(
    BBIS_HANDLE     *h,
    u_int32         mSlot)
{
	DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=%d\n",BBNAME,mSlot));

    return 0;
}

/****************************** PCI_GetMAddr *********************************
 *
 *  Description:  Get physical address description.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     module slot number
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24
 *                dataMode  MDIS_MODE_PCI6 | MDIS_MODE_D32
 *                mAddr     pointer to address space
 *                mSize     size of address space
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_GetMAddr(
    BBIS_HANDLE     *h,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode,
    void            **mAddr,
    u_int32         *mSize )
{
	DBGWRT_ERR((DBH, "*** BB - %s_GetMAddr: should not be called by MK\n",
				BBNAME,mSlot));

    return 0;
}

/****************************** PCI_SetStat **********************************
 *
 *  Description:  Set driver status
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_BB_DEBUG_LEVEL     board debug level          see dbg.h
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     	module slot number
 *                code      	setstat code
 *                value32_or_64 setstat value or ptr to blocksetstat data
 *  Output.....:  return    	0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_SetStat(
    BBIS_HANDLE     *h,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     value32_or_64 )
{
	int32 value = (int32)value32_or_64; /* 32bit value */
	
    DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=%d code=0x%04x value=0x%x\n",
			  BBNAME, mSlot, code, value));

    switch (code) {
        /* set debug level */
        case M_BB_DEBUG_LEVEL:
            h->debugLevel = value;
            break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** PCI_GetStat **********************************
 *
 *  Description:  Get driver status
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_BB_DEBUG_LEVEL     driver debug level         see dbg.h
 *                M_MK_BLK_REV_ID      ident function table ptr   -
 *
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                mSlot     		module slot number
 *                code      		getstat code
 *  Output.....:  value32_or_64P    getstat value or ptr to blockgetstat data
 *                return    		0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_GetStat(
    BBIS_HANDLE     *h,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64    *value32_or_64P )
{
    int32 *valueP = (int32*)value32_or_64P; /* pointer to 32bit value */

    DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=%d code=0x%04x\n",BBNAME,mSlot,code));

    switch (code) {
        /* get debug level */
        case M_BB_DEBUG_LEVEL:
            *valueP = h->debugLevel;
            break;

        /* ident table */
        case M_MK_BLK_REV_ID:
           *value32_or_64P = (INT32_OR_64)&h->idFuncTbl;
           break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** PCI_Unused ***********************************
 *
 *  Description:  Dummy function for unused jump table entries.
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  return  ERR_BBIS_ILL_FUNC
 *  Globals....:  ---
 ****************************************************************************/
static int32 PCI_Unused( void )		/* nodoc */
{
    return ERR_BBIS_ILL_FUNC;
}

/*********************************** Ident **********************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char* Ident( void )		/* nodoc */
{
	return (
		"PCI - PCI"
		"  Base Board Handler: $Id: bb_pci.c,v 1.12 2012/08/08 11:05:08 ts Exp $" );
}

/********************************* Cleanup **********************************
 *
 *  Description:  Close all handles, free memory and return error code
 *
 *		          NOTE: The h handle is invalid after calling this
 *                      function.
 *			
 *---------------------------------------------------------------------------
 *  Input......:  h    pointer to board handle structure
 *                retCode	return value
 *  Output.....:  return	retCode
 *  Globals....:  -
 ****************************************************************************/
static int32 Cleanup(
   BBIS_HANDLE  *h,
   int32        retCode		/* nodoc */
)
{
    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if (h->descHdl)
		DESC_Exit(&h->descHdl);

	/* cleanup debug */
	DBGEXIT((&DBH));

    /*------------------------------+
    |  free memory                  |
    +------------------------------*/
    /* release memory for the board handle */
    OSS_MemFree( h->osHdl, (int8*)h, h->ownMemSize);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}


/********************************* ParsePciPath ******************************
 *
 *  Description: Parses the specified PCI_BUS_PATH to find out PCI Bus Number
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: h 				handle
 *  Output.....: returns:	   	error code
 *				 *pciBusNbrP	main PCI bus number for devices	
 *  Globals....: -
 ****************************************************************************/
static int32 ParsePciPath( BBIS_HANDLE *h, u_int32 *pciBusNbrP ) 	/* nodoc */
{
	u_int32 i;
	int32 pciBusNbr=0, pciDevNbr;
	int32 error;
	int32 vendorID, deviceID, headerType, secondBus;

	/* parse whole pci path until our device is reached */
	for(i=0; i<h->pciPathLen; i++){

		pciDevNbr = h->pciPath[i];
		
		if ( ( i==0 ) && ( 0 != h->pciDomainNbr ) ) {
			/* as we do not know the numbering order of busses on pci domains,
			   try to find the device on all busses instead of looking for the
			   first bus on the domain                                        */
			for(pciBusNbr=0; pciBusNbr<0xff; pciBusNbr++) {
				error = PciParseDev( h, OSS_MERGE_BUS_DOMAIN(pciBusNbr, h->pciDomainNbr),
				                     h->pciPath[0], &vendorID, &deviceID, &headerType,
								     &secondBus );
				if ( error == ERR_SUCCESS )
					break; /* found device */
			}
			
			if ( error != ERR_SUCCESS ) { /* device not found */
				DBGWRT_ERR((DBH,"*** BB - %s: first device 0x%02x in pci bus path "
				                "not found on domain %d!\n",
				                BBNAME, h->pciPath[0], h->pciDomainNbr ));
				return error;
			}
		} else {
			/* parse device only once */
			if( (error = PciParseDev( h, OSS_MERGE_BUS_DOMAIN(pciBusNbr, h->pciDomainNbr),
				                      pciDevNbr, &vendorID, &deviceID, &headerType,
									  &secondBus )))
				return error;
		}

		if( vendorID == 0xffff && deviceID == 0xffff ){
			DBGWRT_ERR((DBH,"*** %s:ParsePciPath: Nonexistant device "
						"domain %d bus %d dev %d\n", BBNAME, h->pciDomainNbr, pciBusNbr, pciDevNbr ));
			return ERR_BBIS_NO_CHECKLOC;
		}

		/*--- device is present, is it a bridge ? ---*/
		if( (headerType & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE ){
			DBGWRT_ERR((DBH,"*** %s:ParsePciPath: Device is not a bridge! "
						"domain %d bus %d dev %d vend=0x%x devId=0x%x\n",
						BBNAME, h->pciDomainNbr, pciBusNbr, pciDevNbr, vendorID, deviceID ));

			return ERR_BBIS_NO_CHECKLOC;
		}
			
		/*--- it is a bridge, determine its secondary bus number ---*/
		DBGWRT_2((DBH, " domain %d bus %d dev %d: vend=0x%x devId=0x%x second bus %d\n",
				  h->pciDomainNbr, pciBusNbr, pciDevNbr, vendorID, deviceID, secondBus ));

		/*--- continue with new bus ---*/
		pciBusNbr = secondBus;
	}

	DBGWRT_1((DBH,"BB - %s: Main PCI Bus Number is %d\n", BBNAME,
			  pciBusNbr ));

	*pciBusNbrP = pciBusNbr;

	return ERR_SUCCESS;
}

/********************************* CheckDevBridge ***************************
 *
 *  Description: Check if the device has an internal bridge
 *			
 *  If no device detected on given pciBusNbr/pciDevNbr returns NO error
 *
 *	If bridge detected on given pciDomain/pciBusNbr/pciDevNbr, searches for
 *  devices behind the bridge.
 *
 *  If more than one device found, aborts and returns an error.
 *
 *  If only one device found and no bridge, returns the new bus/device
 *  number.
 *
 *  If only one device found and again a bridge found, recurses until
 *  a non-bridge device found
 *---------------------------------------------------------------------------
 *  Input......: h 				handle
 *				 pciBusNbr		pci bus number
 *				 pciDevNbr		pci device number
 *  Output.....: returns		error code
 *				 *newPciBusNbrP	new pci bus number
 *				 *newPciDevNbrP new pci device number
 *  Globals....: -
 ****************************************************************************/
static int32 CheckDevBridge(
	BBIS_HANDLE *h,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	u_int32 *newPciBusNbrP,
	u_int32 *newPciDevNbrP)		/* nodoc */
{
	int32 error;
	int32 vendorID, deviceID, headerType, secondBus;
	int32 devsBehindBridge;

	DBGWRT_1((DBH,"BB - %s: CheckDevBridge domain %d bus %d dev %d\n", BBNAME,
			  h->pciDomainNbr, pciBusNbr, pciDevNbr ));

	*newPciBusNbrP = pciBusNbr;
	*newPciDevNbrP = pciDevNbr;

	/*--- parse device ---*/
	if( (error = PciParseDev( h, OSS_MERGE_BUS_DOMAIN(pciBusNbr, h->pciDomainNbr),
							  pciDevNbr, &vendorID, &deviceID, &headerType,
							  &secondBus )))
		return error;

	if( vendorID == 0xffff && deviceID == 0xffff ){
		DBGWRT_2((DBH," CheckDevBridge: Nonexistant device "
					"domain %d bus %d dev %d\n",
					h->pciDomainNbr, pciBusNbr, pciDevNbr ));
		goto done;
	}

	/*--- device is present, is it a bridge ? if yes but we skip DevBridgeCheck we are done ---*/
	if( ((headerType & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE )
		|| ( h->skipDevBridgeCheck !=0 ))
		goto done;

	DBGWRT_2((DBH, " Found Bridge! domain %d bus %d dev %d\n",
			  h->pciDomainNbr, pciBusNbr, pciDevNbr ));	
			
	/*-------------------------------------+
	|  Now check for devices behind bridge |
	+-------------------------------------*/
	pciBusNbr = secondBus;
	devsBehindBridge = 0;

	for( pciDevNbr=0; pciDevNbr<32; pciDevNbr++ ){
		/*--- parse device ---*/
		if( (error = PciParseDev( h, OSS_MERGE_BUS_DOMAIN(pciBusNbr, h->pciDomainNbr),
								  pciDevNbr, &vendorID, &deviceID, &headerType,
								  &secondBus )))
			return error;

		if( vendorID == 0xffff && deviceID == 0xffff )
			continue;

		devsBehindBridge++;
		
		/*--- check if its a bridge ---*/
		if( (headerType & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) == OSS_PCI_HEADERTYPE_BRIDGE_TYPE ){
			DBGWRT_2((DBH, " Found Bridge(2)! domain %d bus %d dev %d\n",
					  h->pciDomainNbr, pciBusNbr, pciDevNbr ));	
			
			/*--- recourse through bridge ---*/
			error = CheckDevBridge( h, pciBusNbr, pciDevNbr,
									newPciBusNbrP, newPciDevNbrP );

			if( error ) return error;
		}
		else {
			/*--- not a bridge ---*/
			*newPciBusNbrP = pciBusNbr;
			*newPciDevNbrP = pciDevNbr;
		}
	}

	/*--- check if exactly one device found behind bridge ---*/
	if( devsBehindBridge != 1 ){
		DBGWRT_ERR((DBH,"*** %s:CheckDevBridge: expecting exactly one dev, "
					"but found %d! doman %d bus %d\n", BBNAME, devsBehindBridge,
					h->pciDomainNbr, pciBusNbr ));
		return ERR_BBIS_NO_CHECKLOC;
	}

done:
	DBGWRT_1((DBH,"BB - %s: CheckDevBridge exit newbus %d newdev %d\n",
			  BBNAME,
			  *newPciBusNbrP, *newPciDevNbrP ));

	return ERR_SUCCESS;
}

/********************************* PciParseDev ******************************
 *
 *  Description: Get parameters from specified PCI device's config space
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: h			handle
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
	BBIS_HANDLE *h,
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
	error = OSS_PciGetConfig( h->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  OSS_PCI_VENDOR_ID, vendorIDP );
		
	if( error == 0 )
		error = OSS_PciGetConfig( h->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
									  OSS_PCI_DEVICE_ID, deviceIDP );

	if( error )
		return PciCfgErr(h,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,OSS_PCI_DEVICE_ID);

	if( *vendorIDP == 0xffff && *deviceIDP == 0xffff )
		return ERR_SUCCESS;		/* not present */

	/*--- device is present, is it a bridge ? ---*/
	error = OSS_PciGetConfig( h->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  OSS_PCI_HEADER_TYPE, headerTypeP );

	if( error )
		return PciCfgErr(h,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,OSS_PCI_HEADER_TYPE);
		
	DBGWRT_2((DBH, " domain %d bus %d dev %d.%d: vend=0x%x devId=0x%x hdrtype %d\n",
			  OSS_DOMAIN_NBR( pciBusNbr ), OSS_BUS_NBR( pciBusNbr ), pciMainDevNbr,
			  pciDevFunc, *vendorIDP, *deviceIDP, *headerTypeP ));

	if( (*headerTypeP & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE )	
		return ERR_SUCCESS;		/* not bridge device */

			
	/*--- it is a bridge, determine its secondary bus number ---*/
	error = OSS_PciGetConfig( h->osHdl, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  PCI_SECONDARY_BUS_NUMBER | OSS_PCI_ACCESS_8,
							  secondBusP );

	if( error )
		return PciCfgErr(h,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,
						 PCI_SECONDARY_BUS_NUMBER | OSS_PCI_ACCESS_8);
		
	return ERR_SUCCESS;
}	

/********************************* PciCfgErr ********************************
 *
 *  Description: Print Debug message
 *			
 *---------------------------------------------------------------------------
 *  Input......: h				handle
 *               funcName		function name
 *               error			error code
 *				 pciBusNbr		pci bus number
 *				 pciDevNbr		pci device number
 *               reg			register
 *  Output.....: return			error code
 *  Globals....: -
 ****************************************************************************/
static int32 PciCfgErr(
	BBIS_HANDLE *h,
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

	// special handling for multifunction bridges: set function number
    if ( pciDevNbr > 0x1f ) {
    	// seperate the function number from the device number
    	pciDevFunc = pciDevNbr >> 5;
    	pciMainDevNbr = ( pciDevNbr & 0x0000001f );
    }

	DBGWRT_ERR((DBH,"*** %s %s: PCI access error 0x%x "
				"domain %d bus %d dev %d.%d reg 0x%x\n", BBNAME, funcName, error,
				OSS_DOMAIN_NBR( pciBusNbr ), OSS_BUS_NBR( pciBusNbr ), pciMainDevNbr,
				pciDevFunc, reg ));
	return error;
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
 *                  - PCI-Device:
 *                    pci-vendor-id << 16 | pci-device-id
 *                    Example: 0x10b91533
 *                  - Onboard Devices:
 *                    For onboard devices, the device id can be a device
 *                    specific identifier or BBIS_SLOT_NBR_UNK.
 *					  Example: The "F2_GPIO" functionality, is implemented in
 *					  the ALI M1543 Southbridge. Therefore the device id may
 *					  be 0x00001543.
 *
 *                  Note: The returned device id must be identical to the
 *                        "autoid" value in the device drivers xml file.
 *
 *                devRev (u_int32*) - device revision (4-byte hex value)
 *                  M-Module: id-prom-layout-revision << 16 |
 *                            id-prom-product-variant
 *                            example: 0x01091400
 *                  or BBIS_SLOT_NBR_UNK if device revision is unknown
 *
 *                slotName (char*) - slot name
 *                  The slot name should consist of the slot type and the
 *                  slot label but must not contain any non-printing
 *                  characters.
 *                  The length of the returned string, including the
 *                  terminating null character, must not exceed
 *                  BBIS_SLOT_STR_MAXSIZE.
 *                  format : "<slot type> <slot label>"
 *                  example: "M-Module slot 0"
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
 *                  - Mezzanines:		"M34", "MS9"
 *                  - Onboard Devices:	"LM78", "F2_GPIO", "I82527"
 *                                      "D203_TRIG"
 *
 *                  If the device name is unknown BBIS_SLOT_STR_UNK must
 *                  be returned.
 *
 *                  Note: The returned device name must be identical to the
 *                        "hwname" value in the device drivers xml file.
 *
 *---------------------------------------------------------------------------
 *  Input......: h				handle
 *				 argptr			argument pointer
 *  Output.....: return			error code
 *  Globals....: -
 ****************************************************************************/
static int32 CfgInfoSlot( BBIS_HANDLE *h, va_list argptr )	/* nodoc */
{
	u_int32 mSlot     = va_arg( argptr, u_int32 );
    u_int32 *occupied = va_arg( argptr, u_int32* );
    u_int32 *devId    = va_arg( argptr, u_int32* );
    u_int32 *devRev   = va_arg( argptr, u_int32* );
	char	*slotName = va_arg( argptr, char* );
	char	*devName  = va_arg( argptr, char* );

	int32 pciVenId, pciDevId;

	/* clear parameters to return */
	*occupied = 0;
	*devId    = 0;
	*devRev   = 0;
	*slotName = '\0';
	*devName  = '\0';

	/* illegal slot? */
	if ( (mSlot >= PCI_BBIS_MAX_DEVS) ||
		 (h->pciDevNbr[mSlot] == PCI_NO_DEV ))
		return ERR_BBIS_ILL_SLOT;

	/*
	 * set slot name
	 * format: "PCI Slot <slot> (PCI bus <bus>, device <dev>, function <func>)"
	 * note: all numbers must be decimal
	 */
	OSS_Sprintf( h->osHdl, slotName, "PCI Slot %d (PCI domain %d bus %d, device %d, function %d)",
		mSlot, h->pciDomainNbr, h->pciBusNbr[mSlot], h->pciDevNbr[mSlot], h->pciFuncNbr[mSlot] );

	/* set occupied info */
	*occupied = BBIS_SLOT_OCCUP_ALW;

	/*
	 * set device id
	 */
	/* get pci-vendor-id and pci-device-id */
	OSS_PciGetConfig( h->osHdl, OSS_MERGE_BUS_DOMAIN(h->pciBusNbr[mSlot], h->pciDomainNbr),
	                  h->pciDevNbr[mSlot], h->pciFuncNbr[mSlot], OSS_PCI_VENDOR_ID, &pciVenId );

	OSS_PciGetConfig( h->osHdl, OSS_MERGE_BUS_DOMAIN(h->pciBusNbr[mSlot], h->pciDomainNbr),
	                  h->pciDevNbr[mSlot], h->pciFuncNbr[mSlot], OSS_PCI_DEVICE_ID, &pciDevId );

	/* assemble our device id */
	*devId  = ((pciVenId & 0x0000ffff) << 16 ) |
		       (pciDevId & 0x0000ffff);

	/* set revision (use pci-revision) */
	OSS_PciGetConfig( h->osHdl, OSS_MERGE_BUS_DOMAIN(h->pciBusNbr[mSlot], h->pciDomainNbr),
	                  h->pciDevNbr[mSlot], h->pciFuncNbr[mSlot], OSS_PCI_REVISION_ID, (int32*)devRev );

	/* set device name (from descriptor or unknown) */
	OSS_StrCpy( h->osHdl, h->devName[mSlot], devName );

	/* return on success */
	return ERR_SUCCESS;
}





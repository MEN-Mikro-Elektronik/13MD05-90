/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  bb_a203n.c
 *
 *      \author  dieter.pfeuffer@men.de
 *        $Date: 2010/04/20 15:45:24 $
 *    $Revision: 1.9 $
 *
 *      \brief   A203N board handler routines for A203N Carrier Boards
 *
 *     Required: ---
 *
 *     \switches  _ONE_NAMESPACE_PER_DRIVER_, A203N_VARIANT, DBG
 *
 *              A203N_VARIANT - prefix for global symbols
 *              A203N_VARIANT=A203N : standard bbis (supports A203N CPCI boards)
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: bb_a203n.c,v $
 * Revision 1.9  2010/04/20 15:45:24  CKauntz
 * R: VME address window not released on exit
 * M: Added OSS_UnMapVmeAddr at Cleanup to unmap vme address window
 *
 * Revision 1.8  2010/04/12 10:47:51  CKauntz
 * R:1. Insufficient numer of parameters in debug prints
 *   2. value is always 32 bit
 * M:1. Added BRD_NAME parameter
 *   2. Changed format specifier in debug print
 *
 * Revision 1.7  2009/08/06 11:02:14  CRuff
 * R: make 64bit compatible
 * M: CfgInfoSlot(): change address type cast to U_INT32_OR_64
 *
 * Revision 1.6  2009/07/23 15:04:18  cs
 * R:1. update to MDIS5
 * M:1.a) added support for 64bit (API,...)
 *     b) adapted DBG prints for 64bit pointers
 *     c) put all MACCESS macros in conditionals in brackets
 *
 * Revision 1.5  2009/04/20 10:04:07  ts
 * R: build failed for OS who define OSS_HAS_UNASSIGN_RESOURCES (Linux)
 * M: added member pciBusNbr in BBIS_HANDLE struct which was missing
 *
 * Revision 1.4  2007/05/11 10:59:15  cs
 * fixed:
 *   - MDIS on some OS doesn't support OSS_MapVmeAddr()
 *     instead use OSS_BusToPhysAddr(.., OSS_BUSTYPE_VME, ...)
 *     when OSS_HAS_MAP_VME_ADDR is not defined
 *
 * Revision 1.3  2006/12/20 12:28:36  ufranke
 * changed
 *  - fkt13 replaced by setIrqHandle
 *
 * Revision 1.2  2005/10/10 15:05:39  dpfeuffer
 * A203N_Init/Cleanup(): to less resources assigned/unassigned
 *
 * Revision 1.1  2005/08/03 12:03:53  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuernberg, Germany
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
#include <MEN/maccess.h>    /* hw access macros and types     */
#include <MEN/modcom.h>     /* id prom functions              */

#include <MEN/a203n.h>      /* A203N register defines         */


#ifndef A203N_VARIANT
#	error "A203N_VARIANT must be set"
#endif

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL			brdHdl->dbgLev	/**< Debug level */
#define DBH					brdHdl->dbgHdl	/**< Debug handle */

#define MOD_ID_MAGIC		0x5346		/**< M-Module id prom magic word */
#define MOD_ID_MS_MASK		0x5300		/**< mask to indicate MS M-Module */

#define ILL_ADDR			0xffffffff

#define BRD_NAME			"A203N"				/**< board hw-name and func-prefix */
#define BRD_MODULE_NBR		4					/**< number of M-Module slots */

#define BRD_ADDR_SPACES		BRD_MODULE_NBR + 2	/**< number of address spaces (mods + trig + cctrl) */
#define BRD_ADDR_TRIG		BRD_MODULE_NBR		/**< trigger space */
#define BRD_ADDR_CCTRL		BRD_MODULE_NBR +1	/**< common ctrl space */

#define TRIGDEV_SLOT		BBIS_SLOTS_ONBOARDDEVICE_START
#define TEMPSENS_SLOT		(BBIS_SLOTS_ONBOARDDEVICE_START + 1)


/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/** bbis handle */
typedef struct {
	/* common */
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/**< id function table				*/
    u_int32     ownMemSize;			/**< own memory size				*/
    OSS_HANDLE	*osHdl;				/**< os specific handle			*/
    DESC_HANDLE *descHdl;			/**< descriptor handle pointer		*/
    u_int32     dbgLev;				/**< debug level for BBIS			*/
	DBG_HANDLE  *dbgHdl;			/**< debug handle					*/
	/* used resources */
    void        *physAddrMa08;		/**< physical address ctrl and MA08 */
    void        *physAddrMa24;		/**< physical address MA24 */
	u_int32		maxDataModeMa08;	/**< max data mode for MA08 */
	u_int32		maxDataModeMa24;	/**< max data mode for MA24 */
	u_int32		a32AddrMa24;		/**< VME A32 addr */
	void		*addrWinHdlMa08;	/**< MA08 address-window handle */
	void		*addrWinHdlMa24;	/**< MA24 address-window handle */
#ifdef OSS_HAS_UNASSIGN_RESOURCES
	u_int32		pciBusNbr;
	u_int32		resourcesAssigned;  /**< flag resources assigned */
#endif
	OSS_RESOURCES	res[BRD_ADDR_SPACES]; /**< resources to un-/assign */
	void		*virtAddrSpace[BRD_ADDR_SPACES]; /**< virtual addr spaces */
    u_int8      irqVec[BRD_MODULE_NBR];    	/**< irq vectors */
    u_int8      irqLev[BRD_MODULE_NBR];    	/**< irq levels */
    u_int8      irqPri[BRD_MODULE_NBR];    	/**< irq priorities */
    u_int8      forceFound[BRD_MODULE_NBR]; /**< forces that module was found */
} BBIS_HANDLE;

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>	/* bbis jumptable			*/
#include <MEN/bb_a203n.h>	/* A203N bbis header file	*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* init/exit */
static int32 A203N_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 A203N_BrdInit(BBIS_HANDLE*);
static int32 A203N_BrdExit(BBIS_HANDLE*);
static int32 A203N_Exit(BBIS_HANDLE**);
/* info */
static int32 A203N_BrdInfo(u_int32, ...);
static int32 A203N_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 A203N_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 A203N_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  A203N_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 A203N_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 A203N_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 A203N_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 A203N_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 A203N_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 A203N_SetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64);
static int32 A203N_GetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64*);
/* unused */
static int32 A203N_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);
static int32 CfgInfoSlot( BBIS_HANDLE *brdHdl, va_list argptr );


/****************************** A203N_GetEntry ******************************/
/** Initialize driver's jump table
 *
 *  \param bbisP     \OUT Pointer to the initialized jump table structure
 */
#ifdef _ONE_NAMESPACE_PER_DRIVER_
	extern void BBIS_GetEntry( BBIS_ENTRY *bbisP )
#else
	extern void __A203N_GetEntry( BBIS_ENTRY *bbisP )
#endif
{
    /* init/exit */
    bbisP->init         =   A203N_Init;
    bbisP->brdInit      =   A203N_BrdInit;
    bbisP->brdExit      =   A203N_BrdExit;
    bbisP->exit         =   A203N_Exit;
    bbisP->fkt04        =   A203N_Unused;
    /* info */
    bbisP->brdInfo      =   A203N_BrdInfo;
    bbisP->cfgInfo      =   A203N_CfgInfo;
    bbisP->fkt07        =   A203N_Unused;
    bbisP->fkt08        =   A203N_Unused;
    bbisP->fkt09        =   A203N_Unused;
    /* interrupt handling */
    bbisP->irqEnable    =   A203N_IrqEnable;
    bbisP->irqSrvInit   =   A203N_IrqSrvInit;
    bbisP->irqSrvExit   =   A203N_IrqSrvExit;
    bbisP->setIrqHandle =   NULL;
    bbisP->fkt14        =   A203N_Unused;
    /* exception handling */
    bbisP->expEnable    =   A203N_ExpEnable;
    bbisP->expSrv       =   A203N_ExpSrv;
    bbisP->fkt17        =   A203N_Unused;
    bbisP->fkt18        =   A203N_Unused;
    bbisP->fkt19        =   A203N_Unused;
    /* */
    bbisP->fkt20        =   A203N_Unused;
    bbisP->fkt21        =   A203N_Unused;
    bbisP->fkt22        =   A203N_Unused;
    bbisP->fkt23        =   A203N_Unused;
    bbisP->fkt24        =   A203N_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   A203N_SetStat;
    bbisP->getStat      =   A203N_GetStat;
    bbisP->setMIface    =   A203N_SetMIface;
    bbisP->clrMIface    =   A203N_ClrMIface;
    bbisP->getMAddr     =   A203N_GetMAddr;
    bbisP->fkt30        =   A203N_Unused;
    bbisP->fkt31        =   A203N_Unused;
}

/******************************** A203N_Init ********************************/
/** Initialize the bbis board driver.
 *
 *  The function allocates and returns the board handle, queries the board
 *  descriptor, assigns the resources and maps the required address spaces.
 *
 *  The function decodes \ref descriptor_entries "these descriptor entries"
 *  in addition to the general descriptor keys.
 *
 *  Note: No HW access is allowed within this function!
 *
 *  \param osHdl      \IN  OSS handle
 *  \param descP      \IN  Pointer to descriptor data
 *  \param brdHdlP    \OUT Pointer to bbis handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_Init(
    OSS_HANDLE      *osHdl,
    DESC_SPEC       *descP,
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE		*brdHdl = NULL;
	u_int32			gotsize, value, addrMode, addrRange, len, i;
    int32			status;
	u_int32     	a16AddrMa08, a24AddrMa08, physAddrMa08, physAddrMa24;
    u_int8			defaultArr[BRD_MODULE_NBR];


    /*-------------------------------+
    | initialize the board structure |
    +-------------------------------*/
	*brdHdlP = NULL;

    /* get memory for the board structure */
    brdHdl = (BBIS_HANDLE*) (OSS_MemGet(
        osHdl, sizeof(BBIS_HANDLE), &gotsize ));
    if ( brdHdl == NULL )
        return ERR_OSS_MEM_ALLOC;

	/* clear */
    OSS_MemFill(osHdl, gotsize, (char*)brdHdl, 0);

    /* clear defaultArr */
    OSS_MemFill( osHdl, sizeof(defaultArr), (char*)defaultArr, 0 );

    /* store data into the board structure */
    brdHdl->ownMemSize = gotsize;
    brdHdl->osHdl = osHdl;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* drivers ident function */
	brdHdl->idFuncTbl.idCall[0].identCall = Ident;
	/* libraries ident functions */
	brdHdl->idFuncTbl.idCall[1].identCall = DESC_Ident;
	brdHdl->idFuncTbl.idCall[2].identCall = OSS_Ident;
	/* terminator */
	brdHdl->idFuncTbl.idCall[3].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
    /* init descHdl */
    status = DESC_Init( descP, osHdl, &brdHdl->descHdl );
    if (status)
		return( Cleanup(brdHdl,status) );

    /* ----- DEBUG_XXX -----*/
    status = DESC_GetUInt32(brdHdl->descHdl, OSS_DBG_DEFAULT, &value,
				"DEBUG_LEVEL_DESC");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

	/* set debug level for DESC module */
	DESC_DbgLevelSet(brdHdl->descHdl, value);

    status = DESC_GetUInt32( brdHdl->descHdl, OSS_DBG_DEFAULT, &(brdHdl->dbgLev),
                "DEBUG_LEVEL");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    DBGWRT_1((DBH,"BB - %s_Init\n",BRD_NAME));

	/* ----- XXX_ADDR_MMOD08 -----*/
	status = DESC_GetUInt32( brdHdl->descHdl, ILL_ADDR, (u_int32*)(&a16AddrMa08),
				"VME16_ADDR_MMOD08");
	if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
		return( Cleanup(brdHdl,status) );

	status = DESC_GetUInt32( brdHdl->descHdl, ILL_ADDR, (u_int32*)(&a24AddrMa08),
				"VME24_ADDR_MMOD08");
	if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
		return( Cleanup(brdHdl,status) );

	/* Note: PHYS_ADDR_MMOD08 is undocumented and only for test purposes */
	status = DESC_GetUInt32( brdHdl->descHdl, ILL_ADDR, (u_int32*)(&physAddrMa08),
				"PHYS_ADDR_MMOD08");
	if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
		return( Cleanup(brdHdl,status) );

	/* ----- XXX_ADDR_MMOD24 -----*/
	status = DESC_GetUInt32( brdHdl->descHdl, ILL_ADDR, (u_int32*)(&brdHdl->a32AddrMa24),
				"VME32_ADDR_MMOD24");
	if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
		return( Cleanup(brdHdl,status) );

	/* Note: PHYS_ADDR_MMOD24 is undocumented and only for test purposes */
	status = DESC_GetUInt32( brdHdl->descHdl, ILL_ADDR, (u_int32*)(&physAddrMa24),
				"PHYS_ADDR_MMOD24");
	if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
		return( Cleanup(brdHdl,status) );

	/* ----- IRQ_XXX -----*/
	len = BRD_MODULE_NBR;
    status = DESC_GetBinary( brdHdl->descHdl, defaultArr, len,
				brdHdl->irqVec, &len, "IRQ_VECTOR");
    if( status != 0 || len != BRD_MODULE_NBR ){
		DBGWRT_ERR((DBH,"*** %s_Init: illegal IRQ_VECTOR in descriptor\n",
			BRD_NAME) );
		return( Cleanup(brdHdl,ERR_BBIS_DESC_PARAM) );
    }

	len = BRD_MODULE_NBR;
    status = DESC_GetBinary( brdHdl->descHdl, defaultArr, len,
				brdHdl->irqLev, &len, "IRQ_LEVEL");
    if( status != 0 || len != BRD_MODULE_NBR ){
		DBGWRT_ERR((DBH,"*** %s_Init: illegal IRQ_LEVEL in descriptor\n",
			BRD_NAME) );
		return( Cleanup(brdHdl,ERR_BBIS_DESC_PARAM) );
    }

	len = BRD_MODULE_NBR;
    status = DESC_GetBinary( brdHdl->descHdl, defaultArr, len,
				brdHdl->irqPri, &len, "IRQ_PRIORITY");
    if( status != 0 && status != ERR_DESC_KEY_NOTFOUND  ){
		DBGWRT_ERR((DBH,"*** %s_Init: illegal IRQ_PRIORITY in descriptor\n",
			BRD_NAME) );
		return( Cleanup(brdHdl,status) );
    }

	/* ----- PNP_FORCE_FOUND -----*/
	len = BRD_MODULE_NBR;
    status = DESC_GetBinary( brdHdl->descHdl, defaultArr, len, brdHdl->forceFound,
							 &len, "PNP_FORCE_FOUND");
    if( status != 0 && status != ERR_DESC_KEY_NOTFOUND )
		return( Cleanup(brdHdl,status) );

    /* exit descHdl */
    status = DESC_Exit( &brdHdl->descHdl );
    if (status)
        return( Cleanup(brdHdl,status) );

    /*------------------------------+
    |  VME16/24 --> MMOD08          |
    +------------------------------*/
    if( (a16AddrMa08  == ILL_ADDR) &&
		(a24AddrMa08  == ILL_ADDR) &&
		(physAddrMa08 == ILL_ADDR) ){
		DBGWRT_ERR((DBH,"*** %s_Init: no VME16/VME24/PHYS_ADDR_MMOD08 in "
			"descriptor\n",BRD_NAME) );
		return( Cleanup(brdHdl,ERR_BBIS_ILL_PARAM) );
    }

    if( (a16AddrMa08 != ILL_ADDR) &&
		(a24AddrMa08 != ILL_ADDR) ){
        DBGWRT_ERR((DBH,"*** %s_Init: VME16_ and VME24_ADDR_MMOD08 in "
			"descriptor\n",BRD_NAME) );
		return( Cleanup(brdHdl,ERR_BBIS_ILL_PARAM) );
	}

    /* physical address specified? */
	if( physAddrMa08 != ILL_ADDR ){
		DBGWRT_2((DBH," using specified PHYS_ADDR_MMOD08=0x%0x\n",
			physAddrMa08) );

		brdHdl->physAddrMa08 = (void*)((U_INT32_OR_64)physAddrMa08);
	}
	/* map VMEbus space into CPU space */
	else{
		u_int32 vmeAddr;

		/* try to get VME_DM_32 capable window */
		brdHdl->maxDataModeMa08 = OSS_VME_DM_32;

		/* VME16_ADDR_MMOD08 ? */
		if( a16AddrMa08 != ILL_ADDR ){
			vmeAddr   = a16AddrMa08;
			addrMode  = OSS_VME_AM_16UD;
			addrRange = OSS_VME_A16;
		}
		/* VME24_ADDR_MMOD08 */
		else{
			vmeAddr   = a24AddrMa08;
			addrMode  = OSS_VME_AM_24UD;
			addrRange = OSS_VME_A24;
		}

#ifdef OSS_HAS_MAP_VME_ADDR
		status = OSS_MapVmeAddr( osHdl, (u_int64)vmeAddr, addrMode,
						brdHdl->maxDataModeMa08, A203N_A16A24_SIZE,	0x00,
						&brdHdl->physAddrMa08, &brdHdl->addrWinHdlMa08 );
#else
		status = OSS_BusToPhysAddr( osHdl, OSS_BUSTYPE_VME,
									&brdHdl->physAddrMa08, vmeAddr,
									addrRange + OSS_VME_D32, A203N_A16A24_SIZE );
#endif
		/* no VME_DM_32 capable window? */
		if( status ){
			DBGWRT_ERR((DBH,"*** WARNING: %s_Init: OSS_MapVmeAddr() - no %s/"
				"DM_32 space\n",BRD_NAME,a16AddrMa08 ? "AM_16UD" : "AM_24UD") );

			/* try to get VME_DM_16 capable window */
			brdHdl->maxDataModeMa08 = OSS_VME_DM_16;

#ifdef OSS_HAS_MAP_VME_ADDR
			status = OSS_MapVmeAddr( osHdl, (u_int64)vmeAddr, addrMode,
							brdHdl->maxDataModeMa08, A203N_A16A24_SIZE,	0x00,
							&brdHdl->physAddrMa08, &brdHdl->addrWinHdlMa08 );
#else
			status = OSS_BusToPhysAddr( osHdl, OSS_BUSTYPE_VME,
										&brdHdl->physAddrMa08, vmeAddr,
										addrRange + OSS_VME_D16, A203N_A16A24_SIZE );
#endif
			if( status ){
				DBGWRT_ERR((DBH,"*** %s_Init: OSS_MapVmeAddr() - no %s/"
					"DM_16 space\n", BRD_NAME,a16AddrMa08 ? "AM_16UD" : "AM_24UD") );

				return( Cleanup(brdHdl,ERR_BBIS_ILL_PARAM) );
			}
		}
	}

    /*------------------------------+
    |  VME32 --> MMOD24             |
    +------------------------------*/

	/* PHYS_ADDR_MMOD24 specified? */
	if( physAddrMa24 != ILL_ADDR ){

		if( brdHdl->a32AddrMa24 == ILL_ADDR  ){
			DBGWRT_ERR((DBH,"*** %s_Init: PHYS_ADDR_MMOD24 without "
				"VME32_ADDR_MMOD24 in descriptor\n",BRD_NAME) );
			return( Cleanup(brdHdl,ERR_BBIS_ILL_PARAM) );
		}

        DBGWRT_2((DBH," using specified PHYS_ADDR_MMOD24=0x%0x\n",
			physAddrMa24) );

		brdHdl->physAddrMa24 = (void*)((U_INT32_OR_64)physAddrMa24);
	}
	/* only VME32_ADDR_MMOD24 specified? */
	else if( brdHdl->a32AddrMa24 != ILL_ADDR ){

		/* try to get a VME_DM_32 capable window */
		brdHdl->maxDataModeMa24 = OSS_VME_DM_32;

#ifdef OSS_HAS_MAP_VME_ADDR
		status = OSS_MapVmeAddr( osHdl, (u_int64)brdHdl->a32AddrMa24, OSS_VME_AM_32UD,
						brdHdl->maxDataModeMa24, A203N_A32_SIZE, 0x00,
						&brdHdl->physAddrMa24, &brdHdl->addrWinHdlMa24 );
#else
		status = OSS_BusToPhysAddr( osHdl, OSS_BUSTYPE_VME,
									&brdHdl->physAddrMa24, brdHdl->a32AddrMa24,
									OSS_VME_A32 + OSS_VME_D32, A203N_A32_SIZE );
#endif

		/* no VME_DM_32 capable window? */
		if( status ){
			DBGWRT_ERR((DBH,"*** WARNING: %s_Init: OSS_MapVmeAddr() - no AM_32UD/"
				"DM_32 space\n", BRD_NAME));

			/* try to get at least an VME_DM_16 capable window */
			brdHdl->maxDataModeMa24 = OSS_VME_DM_16;

#ifdef OSS_HAS_MAP_VME_ADDR
			status = OSS_MapVmeAddr( osHdl, (u_int64)brdHdl->a32AddrMa24, OSS_VME_AM_32UD,
							brdHdl->maxDataModeMa24, A203N_A32_SIZE, 0x00,
							&brdHdl->physAddrMa24, &brdHdl->addrWinHdlMa24 );
#else
			status = OSS_BusToPhysAddr( osHdl, OSS_BUSTYPE_VME, &brdHdl->physAddrMa24,
										brdHdl->a32AddrMa24,
										OSS_VME_A32 + OSS_VME_D16, A203N_A32_SIZE );
#endif

			if( status ){
				DBGWRT_ERR((DBH,"*** %s_Init: OSS_MapVmeAddr()/"
								"OSS_BusToPhysAddr() - no AM_32UD/DM_16 space\n",
                                BRD_NAME));

				return( Cleanup(brdHdl,ERR_BBIS_ILL_PARAM) );
			}
		}
	}
	/* no MA24 support! */
	else{
		brdHdl->physAddrMa24 = (void*)ILL_ADDR;
	}

    /*---------------------------+
    | assign our spaces          |
    +---------------------------*/
    /* module ctrl-reg spaces */
    for( i=0; i<BRD_MODULE_NBR; i++){
         brdHdl->res[i].type = OSS_RES_MEM;
        brdHdl->res[i].u.mem.physAddr =
			(void*)((U_INT32_OR_64)brdHdl->physAddrMa08 + A203N_MCTRL(i));
        brdHdl->res[i].u.mem.size = A203N_MCTRL_SIZE;
    }

    /* trig space */
	brdHdl->res[BRD_ADDR_TRIG].type = OSS_RES_MEM;
    brdHdl->res[BRD_ADDR_TRIG].u.mem.physAddr =
		(void*)((U_INT32_OR_64)brdHdl->physAddrMa08 + A203N_TRIG);
    brdHdl->res[BRD_ADDR_TRIG].u.mem.size = A203N_TRIG_SIZE;

    /* cctrl space */
	brdHdl->res[BRD_ADDR_CCTRL].type = OSS_RES_MEM;
    brdHdl->res[BRD_ADDR_CCTRL].u.mem.physAddr =
		(void*)((U_INT32_OR_64)brdHdl->physAddrMa08 + A203N_CCTRL);
    brdHdl->res[BRD_ADDR_CCTRL].u.mem.size = A203N_CCTRL_SIZE;

    if( (status = OSS_AssignResources( osHdl, OSS_BUSTYPE_VME, 0,
                                       BRD_ADDR_SPACES, brdHdl->res )) )
		return( Cleanup(brdHdl,status) );

#ifdef OSS_HAS_UNASSIGN_RESOURCES
	brdHdl->resourcesAssigned = TRUE;
#endif

    /*---------------------------+
    | map our spaces             |
    +---------------------------*/
    for( i=0; i<BRD_ADDR_SPACES; i++ ){
        status = OSS_MapPhysToVirtAddr( osHdl,
					brdHdl->res[i].u.mem.physAddr, brdHdl->res[i].u.mem.size,
					OSS_ADDRSPACE_MEM, OSS_BUSTYPE_VME, 0,
					&brdHdl->virtAddrSpace[i] );
        if( status )
			return( Cleanup(brdHdl,status) );
    }

	*brdHdlP = brdHdl;

    return 0;
}

/****************************** A203N_BrdInit *******************************/
/** Board initialization
 *
 *  The function initializes the board with the definitions made in the
 *  descriptor. For the M-Module slots, the interrupt levels and vectors are
 *  configured. The interrupt is disabled.
 *  The A32 VME base address will be set if required.
 *
 *  \param brdHdl     \IN  Bbis handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_BrdInit(
    BBIS_HANDLE     *brdHdl )
{
	u_int16 m, val;

	DBGWRT_1((DBH, "BB - %s_BrdInit\n",BRD_NAME));

	/* A24 M-Module support? */
	if( brdHdl->a32AddrMa24 != ILL_ADDR ){

		/* enable A32 mode */
		MWRITE_D16( brdHdl->virtAddrSpace[BRD_ADDR_CCTRL], A203N_A32ADDR,
			(brdHdl->a32AddrMa24 & 0xf0000000) >> 24 );
	}

    /* module interrupt settings */
    for( m=0; m<BRD_MODULE_NBR; m++){
		val = A203N_ICTRL_IPEND | A203N_ICTRL_LEV(brdHdl->irqLev[m]);
		MWRITE_D16( brdHdl->virtAddrSpace[m], A203N_ICTRL, val );
		MWRITE_D16( brdHdl->virtAddrSpace[m], A203N_IVEC, brdHdl->irqVec[m] );
	}

	return 0;
}

/****************************** A203N_BrdExit *******************************/
/** Board deinitialization
 *
 *  \param brdHdl     \IN  Bbis handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_BrdExit(
    BBIS_HANDLE     *brdHdl )
{
	DBGWRT_1((DBH, "BB - %s_BrdExit\n",BRD_NAME));

    return 0;
}

/****************************** A203N_Exit **********************************/
/** Cleanup memory
 *
 *  \param brdHdlP    \IN  Pointer to bbis handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_Exit(
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE	*brdHdl = *brdHdlP;
	int32		error = 0;

    DBGWRT_1((DBH, "BB - %s_Exit\n",BRD_NAME));

    /*------------------------------+
    |  cleanup memory               |
    +------------------------------*/
	error = Cleanup(brdHdl, error);
    *brdHdlP = NULL;

    return error;
}

/****************************** A203N_BrdInfo *******************************/
/** Get information about hardware and driver requirements
 *
 *  Following info codes are supported:
 *
 *  \code
 *  Code                      Description
 *  ------------------------  ------------------------------------------------
 *  BBIS_BRDINFO_BUSTYPE      Bustype of the specified board.
 *  BBIS_BRDINFO_DEVBUSTYPE   Bustype of the specified device
 *                             (not the board bus type!).
 *  BBIS_BRDINFO_FUNCTION     Information if an optional BBIS function is
 *                             supported or not.
 *  BBIS_BRDINFO_NUM_SLOTS    Number of devices used from the driver.
 *  BBIS_BRDINFO_INTERRUPTS   Supported interrupt capability
 *                             (BBIS_IRQ_DEVIRQ/BBIS_IRQ_EXPIRQ) of the
 *                             specified device.
 *  BBIS_BRDINFO_ADDRSPACE    Address characteristic
 *                             (OSS_ADDRSPACE_MEM/OSS_ADDRSPACE_IO) of the
 *                             specified device.
 *  BBIS_BRDINFO_BRDNAME      Short hardware name and type of the board without
 *                             any prefix or suffix.
 *                             The name must not contain any non-printing characters.
 *                             The length of the returned string, including the
 *                             terminating null character, must not exceed
 *                             BBIS_BRDINFO_BRDNAME_MAXSIZE.
 *                             Examples: D201 board, PCI device, Chameleon FPGA\endcode
 *
 *  \param code       \IN  Reference to the information we need
 *  \param ...        \IN  Argument(s)
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_BrdInfo(
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

    /* number of devices */
    case BBIS_BRDINFO_NUM_SLOTS:
    {
        u_int32 *numSlot = va_arg( argptr, u_int32* );

        *numSlot = BRD_MODULE_NBR + 1;
        break;
    }

	/* bus type */
    case BBIS_BRDINFO_BUSTYPE:
    {
        u_int32 *busType = va_arg( argptr, u_int32* );

		*busType = OSS_BUSTYPE_VME;
        break;
    }

    /* device bus type */
    case BBIS_BRDINFO_DEVBUSTYPE:
    {
        u_int32 mSlot       = va_arg( argptr, u_int32 );
        u_int32 *devBusType = va_arg( argptr, u_int32* );

	    /*  M-Module */
		if( mSlot < BRD_MODULE_NBR ){
			*devBusType = OSS_BUSTYPE_MMODULE;
		}
		/* trigger device or temperature sensor */
		else if(( mSlot == TRIGDEV_SLOT ) ||
			    ( mSlot == TEMPSENS_SLOT) ){
			*devBusType = OSS_BUSTYPE_NONE;	/* device is local */
		}
		/* Illegal module slot number */
		else{
			/*
			 * Note: Don't print an error message here, because some BK's
			 *       enter this point very frequently for device enumeration!
			 */
			status = ERR_BBIS_ILL_SLOT;
		}
        break;
    }

    /* interrupt capability */
    case BBIS_BRDINFO_INTERRUPTS:
    {
        u_int32 mSlot = va_arg( argptr, u_int32 );
        u_int32 *irqP = va_arg( argptr, u_int32* );

		mSlot = mSlot; /* dummy access to avoid compiler warning */
		*irqP = BBIS_IRQ_DEVIRQ;
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
		 * build hw name (e.g. A203N board)
		 */
		from = BRD_NAME;
	    while( (*brdName++ = *from++) );	/* copy string */
		from = " board";
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

/****************************** A203N_CfgInfo *******************************/
/** Get information about board configuration
 *
 *  Following info codes are supported:
 *
 *  \code
 *  Code                      Description
 *  ------------------------  ------------------------------------------------
 *  BBIS_CFGINFO_BUSNBR       Number of the bus on which the board resides.
 *  BBIS_CFGINFO_IRQ          Device interrupt vector, level and mode of the
 *                             specified device.
 *  BBIS_CFGINFO_EXP          Exception interrupt vector, level and mode of
 *                             the specified device.
 *  BBIS_CFGINFO_SLOT         Information, if the specified device slot is
 *                             occupied or empty, the device id and device
 *                             revision of the plugged device, the name of the
 *                             slot and the name of the plugged device.\endcode
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param code       \IN  Reference to the information we need
 *  \param ...        \IN  Argument(s)
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_CfgInfo(
    BBIS_HANDLE     *brdHdl,
    u_int32         code,
    ... )
{
    va_list		argptr;
    int32       status=ERR_SUCCESS;


    DBGWRT_1((DBH, "BB - %s_CfgInfo (code=0x%04x)\n",BRD_NAME,code));

    va_start(argptr,code);

    switch ( code ) {
        /* bus number */
        case BBIS_CFGINFO_BUSNBR:
        {
            u_int32 *busNbr = va_arg( argptr, u_int32* );
            u_int32 mSlot   = va_arg( argptr, u_int32 );

			mSlot = mSlot; /* dummy access to avoid compiler warning */
		    *busNbr = 0;
            break;
        }

        /* interrupt information */
		case BBIS_CFGINFO_IRQ:
		{
			u_int32 mSlot   = va_arg( argptr, u_int32 );
			u_int32 *vector = va_arg( argptr, u_int32* );
			u_int32 *level  = va_arg( argptr, u_int32* );
			u_int32 *mode   = va_arg( argptr, u_int32* );

			/* M-Module */
			if( mSlot < BRD_MODULE_NBR ){
				*mode = BBIS_IRQ_EXCLUSIVE;
				*level = brdHdl->irqLev[mSlot];
				*vector = brdHdl->irqVec[mSlot];
			}
			/* trigger device or temperature sensor */
			else {
				*mode = BBIS_IRQ_NONE;
			}
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

			mSlot = mSlot;		/* dummy access to avoid compiler warning */
			*vector = *vector;	/* dummy access to avoid compiler warning */
			*level = *level;	/* dummy access to avoid compiler warning */
			*mode = 0;			/* no extra exception interrupt */
			break;
        }

		/* slot information for PnP support*/
		case BBIS_CFGINFO_SLOT:
		{
			status = CfgInfoSlot( brdHdl, argptr );
			break;
		}

        /* error */
        default:
			DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",BRD_NAME,code));
			status = ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** A203N_IrqEnable *****************************/
/** Interrupt enable / disable
 *
 *  Enable/disable interrupt for specified module slot
 *
 * \param brdHdl     \IN  Bbis handle
 * \param mSlot      \IN  Module slot number
 * \param enable     \IN  Interrupt setting
 *
 * \return           \c 0 On success or error code
 */
 static int32 A203N_IrqEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         enable )
{
    DBGWRT_1((DBH, "BB - %s_IrqEnable: mSlot=0x%04x enable=%d\n",BRD_NAME,mSlot,enable));

	/* trigger device or temperature sensor */
	if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START ){
		DBGWRT_ERR((DBH,"*** %s_IrqEnable: function for mSlot=0x%x not supported\n"
					,BRD_NAME,mSlot));
		return ERR_BBIS_ILL_FUNC;
	}

    /* enable */
    if(enable) {
        MSETMASK_D16( brdHdl->virtAddrSpace[mSlot], A203N_ICTRL, A203N_ICTRL_IRE );
    /* disable */
    } else {
        MCLRMASK_D16( brdHdl->virtAddrSpace[mSlot], A203N_ICTRL, A203N_ICTRL_IRE );
    }

	return 0;
}

/****************************** A203N_IrqSrvInit ****************************/
/** Called at the beginning of an interrupt
 *
 *  Determine interrupt reason.
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param mSlot      \IN  Module slot number
 *
 *  \return           \c 0 BBIS_IRQ_NO | BBIS_IRQ_YES
 */
static int32 A203N_IrqSrvInit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
    u_int16		ctrlReg;

	IDBGWRT_1((DBH, "BB - %s_IrqSrvInit: mSlot=0x%04x\n",BRD_NAME,mSlot));

    /* get the interrupt information */
    ctrlReg = MREAD_D16(brdHdl->virtAddrSpace[mSlot], A203N_ICTRL);

    /* interrupt pending? */
    if(ctrlReg & A203N_ICTRL_IPEND){
		IDBGWRT_2((DBH," interrupt by module\n"));
		return BBIS_IRQ_YES;
	}
    else{
		IDBGWRT_2((DBH," not my interrupt\n"));
        return BBIS_IRQ_NO;
    }
}

/****************************** A203N_IrqSrvExit ****************************/
/** Called at the end of an interrupt
 *
 *  Clear module interrupt.
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param mSlot      \IN  Module slot number
 */
static void A203N_IrqSrvExit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvExit: mSlot=0x%04x\n",BRD_NAME,mSlot));

    /* clear interrupt */
    MSETMASK_D16(brdHdl->virtAddrSpace[mSlot], A203N_ICTRL, A203N_ICTRL_IPEND);
}

/****************************** A203N_ExpEnable *****************************/
/** Exception interrupt enable / disable
 *
 *  Do nothing.
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param mSlot      \IN  Module slot number
 *  \param enable     \IN  Interrupt setting
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_ExpEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
	u_int32			enable)
{
	IDBGWRT_1((DBH, "BB - %s_ExpEnable: mSlot=0x%04x\n",BRD_NAME,mSlot));

	return 0;
}

/****************************** A203N_ExpSrv ********************************/
/** Called at the beginning of an exception interrupt
 *
 *  Do nothing.
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param mSlot      \IN  Module slot number
 *
 *  \return           \c BBIS_IRQ_NO
 */
static int32 A203N_ExpSrv(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_ExpSrv: mSlot=0x%04x\n",BRD_NAME,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** A203N_SetMIface *****************************/
/** Set device interface
 *
 *  Do nothing.
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param mSlot      \IN  Module slot number
 *  \param addrMode   \IN  MDIS_MODE_A08 | MDIS_MODE_A24
 *  \param dataMode   \IN  MDIS_MODE_A203N6 | MDIS_MODE_D32
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_SetMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode)
{
	DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=0x%04x\n",BRD_NAME,mSlot));

    return 0;
}

/****************************** A203N_ClrMIface *****************************/
/** Clear device interface
 *
 *  Do nothing.
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param mSlot      \IN  Module slot number
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_ClrMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=0x%04x\n",BRD_NAME,mSlot));

    return 0;
}

/****************************** A203N_GetMAddr ******************************/
/** Get physical address description
 *
 *  - check module slot number
 *  - assign address spaces
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param mSlot      \IN  Module slot number
 *  \param addrMode   \IN  MDIS_MA08 | MDIS_MA24
 *  \param dataMode   \IN  MDIS_MD16 | MDIS_MD32
 *  \param mAddr      \IN  Pointer to address space
 *  \param mSize      \IN  Size of address space
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_GetMAddr(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode,
    void            **mAddr,
    u_int32         *mSize )
{
	DBGWRT_1((DBH, "BB - %s_GetMAddr: mSlot=0x%04x\n",BRD_NAME,mSlot));

    /*------------------------------+
    | M-Module                      |
    +------------------------------*/
    if( mSlot < BRD_MODULE_NBR ){

		switch (addrMode){
		/* A08 */
		case MDIS_MA08 :
			DBGWRT_2((DBH," supply A08 address space\n"));
			*mAddr = (void*)((U_INT32_OR_64)(brdHdl->physAddrMa08) + A203N_MMOD(mSlot));
			*mSize = A203N_MMOD_SIZE;
			break;
		/* A24 */
		case MDIS_MA24 :
			/* A24 M-Module support? */
			if( brdHdl->physAddrMa24 != (void*)ILL_ADDR ){
				DBGWRT_2((DBH," supply A24 address space\n"));
				*mAddr = (void*)((U_INT32_OR_64)(brdHdl->physAddrMa24) + A203N_MMOD_A32(mSlot));
				*mSize = A203N_MMOD_A32_SIZE;
			}
			else{
				DBGWRT_ERR((DBH,"*** WARNING: %s_GetMAddr: MA24 not enabled in descriptor\n",
							BRD_NAME));
				return ERR_BBIS_ILL_ADDRMODE;
			}

			break;
		/* unsupported address mode */
		default:
			DBGWRT_ERR((DBH,"*** %s_GetMAddr: addrMode=0x%x not supported\n",
						BRD_NAME,addrMode));
			return ERR_BBIS_ILL_ADDRMODE;
		}
	}
    /*------------------------------+
    | Trigger device                |
    +------------------------------*/
	else if( mSlot == TRIGDEV_SLOT ){

		*mAddr = (void*)((U_INT32_OR_64)(brdHdl->physAddrMa08) + A203N_TRIG);
		*mSize = A203N_TRIG_SIZE;
	}
    /*------------------------------+
    | Temperature sensor            |
    +------------------------------*/
	else if( mSlot == TEMPSENS_SLOT ){

		*mAddr = (void*)((U_INT32_OR_64)(brdHdl->physAddrMa08) +
					A203N_CCTRL + A203N_I2C);
		*mSize = 1;
	}
    /*------------------------------+
    | Illegal module slot number    |
    +------------------------------*/
    else{
		DBGWRT_ERR((DBH,"*** %s_GetMAddr: wrong module slot number=0x%x\n",
					BRD_NAME, mSlot));
        return ERR_BBIS_ILL_SLOT;
    }

	DBGWRT_2((DBH, " mSlot=0x%x : mem address=%08p, length=0x%x\n",
		mSlot, *mAddr, *mSize));

    return 0;
}

/****************************** A203N_SetStat *******************************/
/** Set driver status
 *
 *  Following status codes are supported:
 *
 *  \code
 *  Code                 Description                Values
 *  -------------------  -------------------------  ----------
 *  M_BB_DEBUG_LEVEL     board debug level          see dbg.h\endcode
 *
 *  \param brdHdl			\IN  Bbis handle
 *  \param mSlot			\IN  Module slot number
 *  \param code				\IN  Setstat code
 *  \param value32_or_64	\IN  Setstat value or ptr to blocksetstat data
 *
 *  \return					\c 0 On success or error code
 */
static int32 A203N_SetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     value32_or_64 )
{
	int32		value  = (int32)value32_or_64;	/* 32bit value     */
	/* INT32_OR_64 valueP = value32_or_64; */	/* stores 32/64bit pointer */

    DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=0x%04x code=0x%04x value=0x%0x\n",
			  BRD_NAME, mSlot, code, value));

    switch (code) {

		/* set debug level */
        case M_BB_DEBUG_LEVEL:
            brdHdl->dbgLev = value;
            break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** A203N_GetStat *******************************/
/** Get driver status
 *
 *  Following status codes are supported:
 *
 *  \code
 *  Code                 Description                Values
 *  -------------------  -------------------------  ----------
 *  M_BB_DEBUG_LEVEL     driver debug level         see dbg.h
 *  M_BB_IRQ_VECT        interrupt vector           0..max
 *  M_BB_IRQ_LEVEL       interrupt level            0..max
 *  M_BB_IRQ_PRIORITY    interrupt priority         0
 *  M_BB_ID_CHECK        board id is checked        0..1
 *  M_BB_ID_SIZE         eeprom id-data size[bytes] 0
 *  M_MK_BLK_REV_ID      ident function table ptr   -\endcode
 *
 *  \param brdHdl			\IN  Bbis handle
 *  \param mSlot			\IN  Module slot number
 *  \param code				\IN  Getstat code
 *  \param value32_or_64P	\OUT Getstat value or ptr to blockgetstat data
 *
 *  \return           \c 0 On success or error code
 */
static int32 A203N_GetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     *value32_or_64P )
{
	int32		*valueP	  = (int32*)value32_or_64P; /* pointer to 32bit value */
	INT32_OR_64	*value64P = value32_or_64P;			/* stores 32/64bit pointer */

    DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=0x%04x code=0x%04x\n",BRD_NAME,mSlot,code));

    switch (code) {
        /* get debug level */
        case M_BB_DEBUG_LEVEL:
            *valueP = brdHdl->dbgLev;
            break;

        /* get IRQ vector */
        case M_BB_IRQ_VECT:
			/* trigger device or temperature sensor */
			if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START )
				return ERR_BBIS_ILL_FUNC;
			*valueP = brdHdl->irqVec[mSlot];
			break;

        /* get IRQ level */
        case M_BB_IRQ_LEVEL:
			/* trigger device or temperature sensor */
			if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START )
				return ERR_BBIS_ILL_FUNC;
			*valueP = brdHdl->irqLev[mSlot];
            break;

        /* get IRQ priority */
        case M_BB_IRQ_PRIORITY:
			/* trigger device or temperature sensor */
			if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START )
				return ERR_BBIS_ILL_FUNC;
			*valueP = brdHdl->irqPri[mSlot];
			break;

        /* get board id check state */
        case M_BB_ID_CHECK:
			*valueP =0;
			break;

		/* id prom size */
		case M_BB_ID_SIZE:
			*valueP = 0;
			break;

        /* ident table */
        case M_MK_BLK_REV_ID:
			*value64P = (INT32_OR_64)&brdHdl->idFuncTbl;
			break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** A203N_Unused ********************************/
/** Dummy function for unused jump table entries
 *
 *  \return           \c ERR_BBIS_ILL_FUNC
 */
static int32 A203N_Unused( void )
{
    return ERR_BBIS_ILL_FUNC;
}

/****************************** Ident ***************************************/
/** Return ident string
 *
 *  \return           \c pointer to ident string
 */
static char* Ident( void )
{
	return (
#if A203N_VARIANT==A203N
		"A203N"
#endif
		"  Base Board Handler: $Id: bb_a203n.c,v 1.9 2010/04/20 15:45:24 CKauntz Exp $" );
}

/****************************** Cleanup *************************************/
/** Close all handles, unmap addresses, free memory
 *
 *	NOTE: The brdHdl handle is invalid after calling this function.
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param retCode    \IN  code to return
 *
 *  \return           \c 0 On success or error code
 */
static int32 Cleanup(
   BBIS_HANDLE  *brdHdl,
   int32        retCode
)
{
	int32	i;

    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if (brdHdl->descHdl)
		DESC_Exit(&brdHdl->descHdl);

	/* cleanup debug */
	DBGEXIT((&DBH));

    /*------------------------------+
    |  unmap addresses              |
    +------------------------------*/
    /* unmap already mapped control register spaces */
    for ( i=0; i<BRD_ADDR_CCTRL; i++)
		if( brdHdl->virtAddrSpace[i] )
			OSS_UnMapVirtAddr( brdHdl->osHdl, &brdHdl->virtAddrSpace[i],
							   brdHdl->res[i].u.mem.size, OSS_ADDRSPACE_MEM );

#ifdef OSS_HAS_MAP_VME_ADDR
    if( brdHdl->addrWinHdlMa08 )
		 OSS_UnMapVmeAddr( brdHdl->osHdl, brdHdl->addrWinHdlMa08 );
    if( brdHdl->addrWinHdlMa24 )
		 OSS_UnMapVmeAddr( brdHdl->osHdl, brdHdl->addrWinHdlMa24 );
#endif

    /*------------------------------+
    |  unassign resources           |
    +------------------------------*/
#ifdef OSS_HAS_UNASSIGN_RESOURCES
	if( brdHdl->resourcesAssigned ){
		/* unassign the resources */
		OSS_UnAssignResources( brdHdl->osHdl,
							   OSS_BUSTYPE_VME,
							   brdHdl->pciBusNbr,
							   BRD_ADDR_SPACES,
							   brdHdl->res );
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

/****************************** CfgInfoSlot ********************************/
/** Fulfils the BB_CfgInfo(BBIS_CFGINFO_SLOT) request
 *
 *	The variable-argument list (argptr) contains the following parameters in
 *  the given order:
 *
 *  \code
 *  Input
 *  -----
 *  mSlot (u_int32) - device slot number
 *
 *  Output
 *  ------
 *  occupied (u_int32*) - occupied information
 *   - pluggable device:
 *     BBIS_SLOT_OCCUP_YES if slot is occupied
 *     or BBIS_SLOT_OCCUP_NO if slot is empty
 *   - onboard device:
 *     BBIS_SLOT_OCCUP_ALW if device is enabled
 *     or BBIS_SLOT_OCCUP_DIS if device is disabled
 *
 *  devId (u_int32*) - device id (4-byte hex value)
 *    The device id should identify the type of the device but should not
 *    contain enough information to differentiate between two identical
 *    devices. If the device id is unknown BBIS_SLOT_NBR_UNK must be returned.
 *    - M-Module:
 *      id-prom-magic-word << 16 | id-prom-module-id
 *      Example: 0x53460024
 *      Note: The returned device id must be identical to the "autoid" value
 *            in the device drivers xml file.
 *
 *  devRev (u_int32*) - device revision (4-byte hex value)
 *    - M-Module: id-prom-layout-revision << 16 |
 *                id-prom-product-variant
 *                example: 0x01091400
 *      or BBIS_SLOT_NBR_UNK if device revision is unknown
 *
 *  slotName (char*) - slot name
 *    The slot name should consist of the slot type and the slot label but must
 *    not contain any non-printing characters. The length of the returned
 *    string, including the terminating null character, must not exceed
 *    BBIS_SLOT_STR_MAXSIZE.
 *    format : "<slot type> <slot label>"
 *
 *    Examples:
 *    - M-Module:		"M-Module slot 0"
 *    - Onboard-Dev:	"onboard slot 0x1000"
 *
 *  devName (char*) - device name
 *    The device name should identify the type of the device but should not
 *    contain enough information to differentiate between two identical
 *    devices. Furthermore, the device name should refer to the appropriate
 *    device driver name if possible.
 *
 *    The returned string must not contain any non-printing characters or
 *    blanks. The length of the returned string, including the terminating null
 *    character, must not exceed BBIS_SLOT_STR_MAXSIZE.
 *
 *    Examples:
 *    - M-Module:		"M34", "MS9"
 *    - Onboard-Dev:	"A203N_TRIG"
 *
 *    If the device name is unknown BBIS_SLOT_STR_UNK must be returned.
 *
 *    Note: The returned device name must be identical to the "hwname" value in
 *    the device drivers xml file.\endcode
 *
 *  \param brdHdl     \IN  Bbis handle
 *  \param argptr     \IN  Argument(s)
 *
 *  \return           \c 0 On success or error code
 */
static int32 CfgInfoSlot( BBIS_HANDLE *brdHdl, va_list argptr )
{
	u_int32 mSlot     = va_arg( argptr, u_int32 );
    u_int32 *occupied = va_arg( argptr, u_int32* );
    u_int32 *devId    = va_arg( argptr, u_int32* );
    u_int32 *devRev   = va_arg( argptr, u_int32* );
	char	*slotName = va_arg( argptr, char* );
	char	*devName  = va_arg( argptr, char* );

	/* default parameters to return */
	*occupied = BBIS_SLOT_OCCUP_DIS;
	*devId    = BBIS_SLOT_NBR_UNK;
	*devRev   = BBIS_SLOT_NBR_UNK;
	*slotName = '\0';
	*devName  = '\0';

    /*------------------------------+
    | M-Module                      |
    +------------------------------*/
    if( mSlot < BRD_MODULE_NBR ){

		int32	status, status2;
		u_int32	modAddrSize;
		void	*physModAddr, *virtModAddr;
		u_int32 modType;

		/* build slot name */
		OSS_Sprintf( brdHdl->osHdl, slotName, "M-Module slot %d", mSlot);

		/* force found? */
		if( brdHdl->forceFound[mSlot] ){
			*occupied = BBIS_SLOT_OCCUP_YES;
			return ERR_SUCCESS;
		}

		/*------------------------------+
		| get module information        |
		+------------------------------*/
		/* get physical module addr */
		if( (status = A203N_GetMAddr( brdHdl, mSlot, MDIS_MA08, MDIS_MD16,
									 &physModAddr, &modAddrSize )) ){
			return status;
		}

		/* init module slot */
		if( (status = A203N_SetMIface( brdHdl, mSlot, MDIS_MA08, MDIS_MD16 )) ){
			return status;
		}

		/*
		 * map module address space
		 * (quick and dirty without resource assignment)
		 */
		if( (status = OSS_MapPhysToVirtAddr( brdHdl->osHdl, physModAddr, modAddrSize,
											 OSS_ADDRSPACE_MEM, OSS_BUSTYPE_VME,
											 0, &virtModAddr )) ){
			A203N_ClrMIface( brdHdl, mSlot );
			return status;
		}

		/* now get the information */
		m_getmodinfo( (U_INT32_OR_64)virtModAddr, &modType,
					  devId, devRev, devName );

		DBGWRT_2((DBH," modType=%d, devId=0x%08x, devRev=0x%08x, devName=%s\n",
					  modType, *devId, *devRev, devName ));

		/*
		 * MEN M-Module detected?
		 * Note: modType=MODCOM_MOD_THIRD (magic-id <> 0x5346) is not safe for the
		 *       detection without a DTACK timeout interrupt. Therefore, we support
		 *       here only MEN M-Modules (magic-id=0x5346).
		 */
		if( modType == MODCOM_MOD_MEN )
			*occupied = BBIS_SLOT_OCCUP_YES;
		/* no MEN M-Module detected */
		else
			*occupied = BBIS_SLOT_OCCUP_NO;

		/* unmap module address space */
		status = OSS_UnMapVirtAddr(brdHdl->osHdl, &virtModAddr,
								   modAddrSize, OSS_ADDRSPACE_MEM);
		/* clear module slot */
		status2 = A203N_ClrMIface( brdHdl, mSlot );

		/* return on error */
		if( status )
			return status;
		if( status2 )
			return status2;
	}

    /*------------------------------+
    | Trigger device / temp sensor  |
    +------------------------------*/
	else if(( mSlot == TRIGDEV_SLOT ) ||
			( mSlot == TEMPSENS_SLOT) ){
		/* set occupied info */
		*occupied = BBIS_SLOT_OCCUP_ALW;

		/* build slot name */
		OSS_Sprintf( brdHdl->osHdl, slotName, "onboard slot 0x%x", mSlot);

		/* build device name */
		OSS_Sprintf( brdHdl->osHdl, devName,
			(mSlot == TRIGDEV_SLOT) ? "A203N_TRIG" : "A203N_TEMPSENS");
	}

	/*------------------------------+
    | Illegal module slot number    |
    +------------------------------*/
    else{
		/*
		 * Note: Don't print an error message here, because some BK's
		 *       enter this point very frequently for device enumeration!
		 */
		return ERR_BBIS_ILL_SLOT;
	}

	/* return on success */
	return ERR_SUCCESS;
}






/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_isa.c
 *      Project: ISA board handler
 *
 *       Author: ds
 *        $Date: 2009/08/17 15:25:10 $
 *    $Revision: 1.5 $
 *
 *  Description: Generic ISA Base Board handler
 *
 *  The Base Board handler can be used for any memory or i/o mapped ISA device.
 *  For each ISA device, one board instance (with mSlot=0) must be used.
 *
 *  The user has to specify the device base address and size. If the device
 *  is connected to an interrupt, the IRQ number must be specified too.
 *
 *  Enhancement for PnP OS:
 *  -----------------------
 *  The optional descriptor key DEVICE_NAME are used for PnP operating systems
 *  (e.g. W2k). The device name will be returned on the
 *  PCI_CfgInfo(BBIS_CFGINFO_SLOT) request.
 *
 *     Required: ---
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_	:	one namespace per driver
 *               MAC_MEM_MAPPED				:	memory access
 *				 MAC_IO_MAPPED				:	i/o access
 *				 ISA						:	generic ISA bbis
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_isa.c,v $
 * Revision 1.5  2009/08/17 15:25:10  dpfeuffer
 * R:1. Porting to MDIS5 (according porting guide rev. 0.7)
 * M:1.a) added support for 64bit (ISA_Set/GetStat)
 *     b) adapted DBG prints for 64bit pointers
 *
 * Revision 1.4  2006/12/11 17:01:49  ufranke
 * changed
 *  - fkt13 is now setIrqHandle initialized to NULL
 *
 * Revision 1.3  2005/05/06 11:56:30  dpfeuffer
 * ISA_CfgInfo(): Bugfix: error handling was wrong
 *
 * Revision 1.2  2004/09/06 13:02:52  dpfeuffer
 * Enhancements for PnP OS implemented:
 * - PCI_BrdInfo(BBIS_BRDINFO_BRDNAME)
 * - PCI_CfgInfo(BBIS_CFGINFO_SLOT)
 * - descriptor key DEVICE_NAME added
 * uses now ISA_IO_GetEntry() for IO variant
 *
 * Revision 1.1  2000/02/28 14:19:30  Schmidt
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

#define BBNAME					"ISA"
#define BRD_NBR_OF_BRDDEV		1		/* number of supported devices		*/


/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL		brdHdl->debugLevel
#define DBH             brdHdl->debugHdl

#define DEVNAME_SIZE	30

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
typedef struct {
	MDIS_IDENT_FUNCT_TBL idFuncTbl;				/* id function table			*/
    u_int32     ownMemSize;						/* own memory size				*/
    OSS_HANDLE* osHdl;							/* os specific handle			*/
    DESC_HANDLE *descHdl;						/* descriptor handle pointer	*/
    u_int32     debugLevel;						/* debug level for BBIS         */
	DBG_HANDLE  *debugHdl;						/* debug handle					*/
	u_int32		devAddr;						/* device base address          */
	u_int32		devAddrSize;					/* device address space size    */
	u_int32		irqNbr;							/* ISA device IRQ number        */
	char		devName[DEVNAME_SIZE];			/* name of devices				*/
} BBIS_HANDLE;

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>	/* bbis jumptable				  */
#include <MEN/bb_isa.h>		/* ISA bbis header file			  */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* init/exit */
static int32 ISA_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 ISA_BrdInit(BBIS_HANDLE*);
static int32 ISA_BrdExit(BBIS_HANDLE*);
static int32 ISA_Exit(BBIS_HANDLE**);
/* info */
static int32 ISA_BrdInfo(u_int32, ...);
static int32 ISA_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 ISA_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 ISA_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  ISA_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 ISA_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 ISA_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 ISA_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 ISA_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 ISA_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 ISA_SetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64);
static int32 ISA_GetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64*);
/* unused */
static int32 ISA_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);
static int32 CfgInfoSlot( BBIS_HANDLE *brdHdl, va_list argptr );


/**************************** ISA_GetEntry ***********************************
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
#	ifdef MAC_IO_MAPPED
		extern void ISA_IO_GetEntry( BBIS_ENTRY *bbisP )
#	else
		extern void ISA_GetEntry( BBIS_ENTRY *bbisP )
#	endif
#endif
{
    /* init/exit */
    bbisP->init         =   ISA_Init;
    bbisP->brdInit      =   ISA_BrdInit;
    bbisP->brdExit      =   ISA_BrdExit;
    bbisP->exit         =   ISA_Exit;
    bbisP->fkt04        =   ISA_Unused;
    /* info */
    bbisP->brdInfo      =   ISA_BrdInfo;
    bbisP->cfgInfo      =   ISA_CfgInfo;
    bbisP->fkt07        =   ISA_Unused;
    bbisP->fkt08        =   ISA_Unused;
    bbisP->fkt09        =   ISA_Unused;
    /* interrupt handling */
    bbisP->irqEnable    =   ISA_IrqEnable;
    bbisP->irqSrvInit   =   ISA_IrqSrvInit;
    bbisP->irqSrvExit   =   ISA_IrqSrvExit;
    bbisP->setIrqHandle =   NULL;
    bbisP->fkt14        =   ISA_Unused;
    /* exception handling */
    bbisP->expEnable    =   ISA_ExpEnable;
    bbisP->expSrv       =   ISA_ExpSrv;
    bbisP->fkt17        =   ISA_Unused;
    bbisP->fkt18        =   ISA_Unused;
    bbisP->fkt19        =   ISA_Unused;
    /* */
    bbisP->fkt20        =   ISA_Unused;
    bbisP->fkt21        =   ISA_Unused;
    bbisP->fkt22        =   ISA_Unused;
    bbisP->fkt23        =   ISA_Unused;
    bbisP->fkt24        =   ISA_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   ISA_SetStat;
    bbisP->getStat      =   ISA_GetStat;
    bbisP->setMIface    =   ISA_SetMIface;
    bbisP->clrMIface    =   ISA_ClrMIface;
    bbisP->getMAddr     =   ISA_GetMAddr;
    bbisP->fkt30        =   ISA_Unused;
    bbisP->fkt31        =   ISA_Unused;
}

/****************************** ISA_Init *************************************
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
 *                DEVICE_ADDR			   none				0..max
 *                DEVICE_ADDRSIZE		   none				0..max
 *                IRQ_NUMBER               0 (=no IRQ)      0..max
 *                DEVICE_NAME            BBIS_SLOT_STR_UNK  DEVNAME_SIZE
 *
 *---------------------------------------------------------------------------
 *  Input......:  osHdl     pointer to os specific structure
 *                descSpec  pointer to os specific descriptor specifier
 *                brdHdlP   pointer to not initialized board handle structure
 *  Output.....:  *brdHdlP  initialized board handle structure
 *				  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_Init(
    OSS_HANDLE      *osHdl,
    DESC_SPEC       *descSpec,
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE	*brdHdl = NULL;
	u_int32     gotsize;
    int32       status;
    u_int32		value, devNameSize;


    /*-------------------------------+
    | initialize the board structure |
    +-------------------------------*/
    /* get memory for the board structure */
    *brdHdlP = brdHdl = (BBIS_HANDLE*) (OSS_MemGet(
        osHdl, sizeof(BBIS_HANDLE), &gotsize ));
    if ( brdHdl == NULL )
        return ERR_OSS_MEM_ALLOC;

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

    DBGWRT_1((DBH,"BB - %s_Init\n",BBNAME));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
    /* init descHdl */
    status = DESC_Init( descSpec, osHdl, &brdHdl->descHdl );
    if (status)
		return( Cleanup(brdHdl,status) );

    /* get DEBUG_LEVEL_DESC (optional) */
    status = DESC_GetUInt32(brdHdl->descHdl, OSS_DBG_DEFAULT, &value,
				"DEBUG_LEVEL_DESC");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

	/* set debug level for DESC module */
	DESC_DbgLevelSet(brdHdl->descHdl, value);

    /* get DEBUG_LEVEL (optional) */
    status = DESC_GetUInt32( brdHdl->descHdl, OSS_DBG_DEFAULT, &(brdHdl->debugLevel),
                "DEBUG_LEVEL");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    /* get DEVICE_ADDR */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &(brdHdl->devAddr),
                "DEVICE_ADDR");
    if ( status ){
		DBGWRT_ERR((DBH,"*** %s_Init: cannot get device address from descriptor\n",BBNAME));
        return( Cleanup(brdHdl,status) );
	}

    /* get DEVICE_ADDRSIZE */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &(brdHdl->devAddrSize),
                "DEVICE_ADDRSIZE");
    if ( status ){
		DBGWRT_ERR((DBH,"*** %s_Init: cannot get device address space size from descriptor\n",BBNAME));
        return( Cleanup(brdHdl,status) );
	}

    /* get IRQ_NUMBER (optional) */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &(brdHdl->irqNbr),
                "IRQ_NUMBER");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

	/* get DEVICE_NAME (optional) */
	devNameSize = DEVNAME_SIZE;
	status = DESC_GetString( brdHdl->descHdl, BBIS_SLOT_STR_UNK, brdHdl->devName,
							 &devNameSize, "DEVICE_NAME" );
	if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
		return( Cleanup(brdHdl,status) );
#if DBG		
	if( status == ERR_SUCCESS )
		DBGWRT_2(( DBH, " DEVICE_NAME=%s\n", brdHdl->devName ));
#endif

    /* exit descHdl */
    status = DESC_Exit( &brdHdl->descHdl );
    if (status)
        return( Cleanup(brdHdl,status) );

    return 0;
}

/****************************** ISA_BrdInit **********************************
 *
 *  Description:  Board initialization.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_BrdInit(
    BBIS_HANDLE     *brdHdl )
{
	DBGWRT_1((DBH, "BB - %s_BrdInit\n",BBNAME));

	return 0;
}

/****************************** ISA_BrdExit **********************************
 *
 *  Description:  Board deinitialization.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_BrdExit(
    BBIS_HANDLE     *brdHdl )
{
	DBGWRT_1((DBH, "BB - %s_BrdExit\n",BBNAME));

    return 0;
}

/****************************** ISA_Exit *************************************
 *
 *  Description:  Cleanup memory.
 *
 *                - deinitializes the bbis handle
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdlP   pointer to board handle structure
 *  Output.....:  *brdHdlP  NULL
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_Exit(
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE	*brdHdl = *brdHdlP;
	int32		error = 0;

    DBGWRT_1((DBH, "BB - %s_Exit\n",BBNAME));

    /*------------------------------+
    |  de-init hardware             |
    +------------------------------*/
	/* nothing to do */

    /*------------------------------+
    |  cleanup memory               |
    +------------------------------*/
	error = Cleanup(brdHdl, error);
    *brdHdlP = NULL;

    return error;
}

/****************************** ISA_BrdInfo **********************************
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
 *                devices used from the driver.
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
static int32 ISA_BrdInfo(
    u_int32 code,
    ... )
{
	int32		status = ERR_SUCCESS;
    va_list     argptr;

    va_start(argptr,code);

    switch ( code ) {

        /* supported functions */
        case BBIS_BRDINFO_FUNCTION:
        {
            u_int32 funcCode = va_arg( argptr, u_int32 );
            u_int32 *status = va_arg( argptr, u_int32* );

			/* no optional BBIS function do anything */
            *status = FALSE;
            funcCode = funcCode; /* dummy access to avoid compiler warning */
            break;
        }

        /* number of devices */
        case BBIS_BRDINFO_NUM_SLOTS:
        {
            u_int32 *numSlot = va_arg( argptr, u_int32* );

            *numSlot = BRD_NBR_OF_BRDDEV;
            break;
        }
		
		/* bus type */
        case BBIS_BRDINFO_BUSTYPE:
        {
            u_int32 *busType = va_arg( argptr, u_int32* );

			*busType = OSS_BUSTYPE_ISA;
            break;
        }

        /* device bus type */
        case BBIS_BRDINFO_DEVBUSTYPE:
        {
            u_int32 mSlot       = va_arg( argptr, u_int32 );
            u_int32 *devBusType = va_arg( argptr, u_int32* );

			*devBusType = OSS_BUSTYPE_ISA;
            mSlot = mSlot; /* dummy access to avoid compiler warning */
            break;
        }

        /* interrupt capability */
        case BBIS_BRDINFO_INTERRUPTS:
        {
            u_int32 mSlot = va_arg( argptr, u_int32 );
            u_int32 *irqP = va_arg( argptr, u_int32* );

            *irqP = BBIS_IRQ_DEVIRQ;
            mSlot = mSlot; /* dummy access to avoid compiler warning */
            break;
        }

        /* address space type */
        case BBIS_BRDINFO_ADDRSPACE:
        {
            u_int32 mSlot      = va_arg( argptr, u_int32 );
            u_int32 *addrSpace = va_arg( argptr, u_int32* );

#ifdef MAC_MEM_MAPPED
			*addrSpace = OSS_ADDRSPACE_MEM;
#else
			*addrSpace = OSS_ADDRSPACE_IO;
#endif
            mSlot = mSlot; /* dummy access to avoid compiler warning */
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
#ifdef MAC_MEM_MAPPED
			from = "ISA device";
#else
			from = "ISA_IO device";
#endif
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

/****************************** ISA_CfgInfo **********************************
 *
 *  Description:  Get information about board configuration.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  ------------------------------
 *                BBIS_CFGINFO_BUSNBR       bus number
 *                BBIS_CFGINFO_IRQ          interrupt parameters
 *                BBIS_CFGINFO_EXP          exception interrupt parameters
 *                BBIS_CFGINFO_SLOT			slot information
 *
 *                The BBIS_CFGINFO_BUSNBR code returns the number of the
 *                bus on which the specified device resides
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
static int32 ISA_CfgInfo(
    BBIS_HANDLE     *brdHdl,
    u_int32         code,
    ... )
{
    va_list		argptr;
    int32       status=0;


    DBGWRT_1((DBH, "BB - %s_CfgInfo (code=0x%04x)\n",BBNAME,code));

    va_start(argptr,code);

    switch ( code ) {
        /* bus number */
        case BBIS_CFGINFO_BUSNBR:
        {
            u_int32 *busNbr = va_arg( argptr, u_int32* );
            u_int32 mSlot   = va_arg( argptr, u_int32 );

            *busNbr = 0;
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

		    /* check device number */
			if ( mSlot != 0){
				DBGWRT_ERR((DBH,"*** %s_CfgInfo: mSlot=0x%04x not supported\n",BBNAME,mSlot));
		        va_end( argptr );
			    return ERR_BBIS_ILL_SLOT;
			}

			/* interrupt connected? */
			if( brdHdl->irqNbr ){
				*mode  = BBIS_IRQ_EXCLUSIVE;
				*level = brdHdl->irqNbr;
				/* convert level to vector */
				status = OSS_IrqLevelToVector(
							brdHdl->osHdl,
							OSS_BUSTYPE_ISA,
							brdHdl->irqNbr,
							(int32*)vector );
			}
			/* no interrupt */
			else{
				*mode  = BBIS_IRQ_NONE;
				vector = vector;	/* dummy access to avoid compiler warning */
				level = level;		/* dummy access to avoid compiler warning */
			}

			DBGWRT_2((DBH, " mSlot=0x%04x : IRQ mode=0x%x, level=0x%x, vector=0x%x\n",
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
            *mode = BBIS_IRQ_NONE;
            mSlot = mSlot;		/* dummy access to avoid compiler warning */
            vector = vector;	/* dummy access to avoid compiler warning */
            level = level;		/* dummy access to avoid compiler warning */
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
			DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",BBNAME,code));
            va_end( argptr );
            return ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** ISA_IrqEnable ********************************
 *
 *  Description:  Interrupt enable / disable.
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
static int32 ISA_IrqEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         enable )
{
    DBGWRT_1((DBH, "BB - %s_IrqEnable: mSlot=0x%04x enable=%d\n",BBNAME,mSlot,enable));

	return 0;
}

/****************************** ISA_IrqSrvInit *******************************
 *
 *  Description:  Called at the beginning of an interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    BBIS_IRQ_UNK
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_IrqSrvInit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvInit: mSlot=0x%04x\n",BBNAME,mSlot));

    return BBIS_IRQ_UNK;
}

/****************************** ISA_IrqSrvExit *******************************
 *
 *  Description:  Called at the end of an interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void ISA_IrqSrvExit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvExit: mSlot=0x%04x\n",BBNAME,mSlot));
}

/****************************** ISA_ExpEnable ********************************
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
static int32 ISA_ExpEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
	u_int32			enable)
{
	IDBGWRT_1((DBH, "BB - %s_ExpEnable: mSlot=0x%04x\n",BBNAME,mSlot));

	return 0;
}

/****************************** ISA_ExpSrv ***********************************
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
static int32 ISA_ExpSrv(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_ExpSrv: mSlot=0x%04x\n",BBNAME,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** ISA_SetMIface ********************************
 *
 *  Description:  Set device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24
 *                dataMode  MDIS_MODE_D16 | MDIS_MODE_D32
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_SetMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode)
{
	DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=0x%04x\n",BBNAME,mSlot));

    return 0;
}

/****************************** ISA_ClrMIface ********************************
 *
 *  Description:  Clear device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure
 *                mSlot     module slot number
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_ClrMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=0x%04x\n",BBNAME,mSlot));

    return 0;
}

/****************************** ISA_GetMAddr *********************************
 *
 *  Description:  Get physical address description.
 *
 *                - check device number
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
static int32 ISA_GetMAddr(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode,
    void            **mAddr,
    u_int32         *mSize )
{
	DBGWRT_1((DBH, "BB - %s_GetMAddr: mSlot=0x%04x\n",BBNAME,mSlot));

	/* check device number */
	if ( mSlot != 0){
		DBGWRT_ERR((DBH,"*** %s_CfgInfo: mSlot=0x%04x not supported\n",BBNAME,mSlot));
		return ERR_BBIS_ILL_SLOT;
	}

	/* assign address spaces */
	*mAddr = (void*)brdHdl->devAddr;
    *mSize = brdHdl->devAddrSize;

	DBGWRT_2((DBH, " mSlot=0x%04x : address=%08p, length=0x%x\n",
		mSlot, *mAddr, *mSize));

    return 0;
}

/****************************** ISA_SetStat **********************************
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
 *  Input......:  brdHdl        pointer to board handle structure
 *                mSlot         module slot number
 *                code          setstat code
 *                value32_or_64 setstat value or ptr to blocksetstat data
 *  Output.....:  return        0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_SetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     value32_or_64 )
{
	int32 value = (int32)value32_or_64; /* 32bit value */

    DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=0x%04x code=0x%04x value=0x%x\n",
			  BBNAME, mSlot, code, value));

    switch (code) {
        /* set debug level */
        case M_BB_DEBUG_LEVEL:
            brdHdl->debugLevel = value;
            break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** ISA_GetStat **********************************
 *
 *  Description:  Get driver status
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_BB_DEBUG_LEVEL     driver debug level         see dbg.h
 *                M_BB_IRQ_VECT        interrupt vector           0..max
 *                M_BB_IRQ_LEVEL       interrupt level            0..max
 *                M_BB_IRQ_PRIORITY    interrupt priority         0
 *                M_MK_BLK_REV_ID      ident function table ptr   -
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl         pointer to board handle structure
 *                mSlot          module slot number
 *                code           getstat code
 *  Output.....:  value32_or_64P getstat value or ptr to blockgetstat data
 *                return         0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_GetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     *value32_or_64P )
{
    int32	status;
	int32	*valueP = (int32*)value32_or_64P; /* pointer to 32bit value */

    DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=0x%04x code=0x%04x\n",BBNAME,mSlot,code));

    switch (code) {
        /* get debug level */
        case M_BB_DEBUG_LEVEL:
            *valueP = brdHdl->debugLevel;
            break;

        /* get IRQ vector */
        case M_BB_IRQ_VECT:
			/* convert level to vector */
			status = OSS_IrqLevelToVector(
						brdHdl->osHdl,
						OSS_BUSTYPE_ISA,
						brdHdl->irqNbr,
						valueP );
			if (status)
				return status;
			break;

        /* get IRQ level */
        case M_BB_IRQ_LEVEL:
			*valueP = brdHdl->irqNbr;
            break;

        /* get IRQ priority */
        case M_BB_IRQ_PRIORITY:
            *valueP = 0;
            break;

        /* ident table */
        case M_MK_BLK_REV_ID:
           *value32_or_64P = (INT32_OR_64)&brdHdl->idFuncTbl;
           break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** ISA_Unused ***********************************
 *
 *  Description:  Dummy function for unused jump table entries.
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  return  ERR_BBIS_ILL_FUNC
 *  Globals....:  ---
 ****************************************************************************/
static int32 ISA_Unused( void )		/* nodoc */
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
		"ISA - ISA Base Board Handler: $Id: bb_isa.c,v 1.5 2009/08/17 15:25:10 dpfeuffer Exp $" );
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
    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if (brdHdl->descHdl)
		DESC_Exit(&brdHdl->descHdl);

	/* cleanup debug */
	DBGEXIT((&DBH));

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
 *  Input......: brdHdl			handle
 *				 argptr			argument pointer
 *  Output.....: return			error code
 *  Globals....: -
 ****************************************************************************/
static int32 CfgInfoSlot( BBIS_HANDLE *brdHdl, va_list argptr )	/* nodoc */
{
	char	irqStr[20];
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
	if( mSlot >= BRD_NBR_OF_BRDDEV )
		return ERR_BBIS_ILL_SLOT;

	*occupied = BBIS_SLOT_OCCUP_ALW;
	*devId    = BBIS_SLOT_NBR_UNK;
	*devRev   = BBIS_SLOT_NBR_UNK;

	/*
	 * set slot name
	 * format: "ISA Slot <slot> (addr <addr>, size <size>[, irq <irq>])"
	 * note: all numbers must be decimal
	 */
	if( brdHdl->irqNbr )
		OSS_Sprintf( brdHdl->osHdl, irqStr, ", irq %d", brdHdl->irqNbr );
	else
		irqStr[0] = '\0';

	OSS_Sprintf( brdHdl->osHdl, slotName, "ISA Slot %d (addr 0x%x, size 0x%x%s)",
		mSlot, brdHdl->devAddr, brdHdl->devAddrSize, irqStr);

	/* set device name (from descriptor or unknown) */
	OSS_StrCpy( brdHdl->osHdl, brdHdl->devName, devName );

	/* return on success */
	return ERR_SUCCESS;
}








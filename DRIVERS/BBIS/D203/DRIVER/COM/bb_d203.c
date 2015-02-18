/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_d203.c
 *      Project: D203 board handler
 *
 *       Author: ds
 *        $Date: 2013/11/28 17:00:24 $
 *    $Revision: 1.18 $
 *
 *  Description: D203 board handler routines for D203/F204/F205 Carrier Boards
 *
 *               The D203 is a M-Module carrier board for up to four
 *               M-Modules and supports PXI trigger capability.
 *
 *               Slot assignment:
 *
 *                slot   |  device
 *               --------+--------------------------------
 *                     0 |  M-Module slot 0
 *                     1 |  M-Module slot 1
 *                     2 |  M-Module slot 2
 *                     3 |  M-Module slot 3
 *
 *     Required: ---
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_, D203_VARIANT, DBG 
 *
 *               D203_VARIANT - prefix for global symbols
 *               D203 - standard variant for D203/F204/F205 carriers 
 *               D203_A24 - variant for D203/F204/F205 carriers with A24 support 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_d203.c,v $
 * Revision 1.18  2013/11/28 17:00:24  ts
 * R: MDIS projects with F50P didnt work on VxWorks versions without multiple PCI domains
 * M: changed behavior for PCI_BUS_PATH with VxWorks versions <=6.6
 *
 * Revision 1.17  2011/05/20 10:48:28  CRuff
 * R: cosmetics
 * M: in PciParseDev(), adjusted debug print format (device/function)
 *
 * Revision 1.16  2011/05/20 09:59:59  CRuff
 * R: 1. support of pci domains
 * M: 1a) added handling of code BBIS_CFGINFO_PCI_DOMAIN to CHAMELEON_CfgInfo()
 *    1b) added new member pciDomainNbr to BBIS_HANDLE
 *    1c) extend debug prints of pci bus number to also print the domain number
 *    1d) call all occurrences of OSS_PciXetConfig (and other OSS calls)
 *        with merged bus and domain
 *    1e) in ParsePciPath(): look for root device on all busses instead of
 *        expecting root device on bus 0
 *
 * Revision 1.15  2009/08/21 16:04:07  dpfeuffer
 * R: CfgInfoSlot(): revised
 * M: CfgInfoSlot(): cosmetics
 *
 * Revision 1.14  2009/08/05 12:07:03  CRuff
 * R:1. Porting to MDIS5 (according porting guide rev. 0.7)
 *   2. PCI bus path does not support multifunction bridges
 * M:1.a) added support for 64bit (API)
 *     b) adapted DBG prints for 64bit pointers
 *   2. consider the function number of devices in the bus path
 *
 * Revision 1.13  2009/05/26 15:03:59  ufranke
 * R: compile time error for E500 PPC with mac_mem.h 1.18
 * M: fixed
 *
 * Revision 1.12  2009/03/10 15:41:24  dpfeuffer
 * R: SetStat interface parameter names differ from function desccription
 * M: Changed SetStat parameter names
 *
 * Revision 1.11  2009/03/10 15:31:23  dpfeuffer
 * R: SetStat/GetStat interface parameter names differ from function desccription
 * M: Changed SetStat/GetStat parameter names
 *
 * Revision 1.10  2008/09/16 18:41:16  CKauntz
 * R: No 64 bit support
 * M: Changed SetStat and GetStat to 64 bit interface
 *
 * Revision 1.9  2006/12/20 12:39:49  ufranke
 * fixed
 *  - function pointer setIrqHandle should be NULL if not implemented
 *    or must be implemented completely
 *
 * Revision 1.8  2006/12/15 16:17:57  ts
 * replaced BBIS fkt13 with setIrqHandle
 *
 * Revision 1.7  2006/09/29 14:37:58  DPfeuffer
 * Ident(): string fixed
 *
 * Revision 1.6  2006/08/29 13:25:33  DPfeuffer
 * - D203_A24 variant implemented
 * - standard variant supports no longer A08/D32 space (no support in HW)
 * - trigger devices removed
 * - trigger feature now supported by descriptor entries
 *
 * Revision 1.5  2005/01/19 16:22:25  dpfeuffer
 * - D203_BrdInfo(BBIS_BRDINFO_DEVBUSTYPE): returns now OSS_BUSTYPE_NONE for
 * onboard trigger devices
 * - CfgInfoSlot():
 * - debuf print changed
 * - bugfix: error handling was wrong for trigger devices
 *
 * Revision 1.4  2004/06/30 10:20:55  cs
 * bugfix: returned error when M-Module requested 8bit data bus
 * now just returns 16bit bus.
 *
 * Revision 1.3  2004/06/21 12:20:52  dpfeuffer
 * BBIS_BRDINFO_BRDNAME changed
 *
 * Revision 1.2  2004/03/11 14:58:25  dpfeuffer
 * cosmetics
 *
 * Revision 1.1  2003/01/28 16:17:16  dschmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: bb_d203.c,v 1.18 2013/11/28 17:00:24 ts Exp $";

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

#ifndef D203_VARIANT
#	error "D203_VARIANT must be set"
#endif

#include "brd.h"		/* board specific defines */ 

/*-----------------------------------------+
  |  DEFINES                                 |
  +-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL		brdHdl->dbgLev
#define DBH             brdHdl->dbgHdl

#define MAX_PCI_PATH				16		/* max number of bridges to devices */
#define PCI_SECONDARY_BUS_NUMBER	0x19	/* PCI bridge config */
#define MOD_ID_MAGIC				0x5346  /* M-Module id prom magic word */
#define MOD_ID_MS_MASK				0x5300	/* mask to indicate MS M-Module */

/* enable global interrupt to detect exception (timeout) interrupts */
#define INT_ENABLE( brdHdl )						\
  MSETMASK_D16( brdHdl->virtCtrlBase[0], D203_GCTRL, D203_GCTRL_GIEN );

/* disable global interrupt */
#define INT_DISABLE( brdHdl )						\
  MCLRMASK_D16( brdHdl->virtCtrlBase[0], D203_GCTRL, D203_GCTRL_GIEN );

/*-----------------------------------------+
  |  TYPEDEFS                                |
  +-----------------------------------------*/
typedef struct {
  /* common */
  MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table			*/
  u_int32     ownMemSize;			/* own memory size				*/
  OSS_HANDLE* osHdl;				/* os specific handle			*/
  DESC_HANDLE *descHdl;			/* descriptor handle pointer	*/
  u_int32     dbgLev;				/* debug level for BBIS         */
  DBG_HANDLE  *dbgHdl;			/* debug handle					*/
  u_int32		idCheck;			/* 1: check id					*/
  u_int32		resourcesAssigned;  /* flag resources assigned		*/
  u_int32     expIrqCount;		/* count of exception interrupts*/
  /* PCI specific */
  u_int32     pciDomainNbr;       /* PCI domain number            */
  u_int32		pciBusNbr;			/* PCI bus number				*/
  u_int32     pciDev;				/* PCI device number			*/
  u_int8		pciPath[MAX_PCI_PATH]; /* PCI path from desc		*/
  u_int32		pciPathLen;			/* number of bytes in pciPath	*/
  void*		bar0;				/* base address from BAR0		*/
  /* used resources */
  void*		virtCtrlBase[BRD_MODULE_NBR];	/* virtual ctrl-reg base addr of modules */
  int32		irqLevel;			/* interrupt level						*/
  int32		irqVector;			/* interrupt vector						*/
  /* trigger */
  u_int32		trigPxiSrc;					/* PXI_TRIG_SRC value */
  u_int32		trigPxiDst;					/* PXI_TRIG_DST value */
  u_int32		trigASrc[BRD_MODULE_NBR];	/* MSLOT_n/TRIGA_SRC value */
  u_int32		trigADst[BRD_MODULE_NBR];	/* MSLOT_n/TRIGA_DST value */
  u_int32		trigBSrc[BRD_MODULE_NBR];	/* MSLOT_n/TRIGB_SRC value */
  u_int32		trigBDst[BRD_MODULE_NBR];	/* MSLOT_n/TRIGB_DST value */
} BBIS_HANDLE;

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>	/* bbis jumptable			*/
#include <MEN/bb_d203.h>	/* D203 bbis header file	*/
#include "d203_int.h"		/* D203 specific defines	*/ 

/*-----------------------------------------+
  |  PROTOTYPES                              |
  +-----------------------------------------*/
/* init/exit */
static int32 D203_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 D203_BrdInit(BBIS_HANDLE*);
static int32 D203_BrdExit(BBIS_HANDLE*);
static int32 D203_Exit(BBIS_HANDLE**);
/* info */
static int32 D203_BrdInfo(u_int32, ...);
static int32 D203_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 D203_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 D203_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  D203_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 D203_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 D203_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 D203_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 D203_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 D203_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 D203_SetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64);
static int32 D203_GetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64*);
/* unused */
static int32 D203_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);
static int32 ParsePciPath( BBIS_HANDLE *h, u_int32 *pciBusNbrP );
static int32 PciParseDev( BBIS_HANDLE *h, u_int32 pciBusNbr,
			  u_int32 pciDevNbr, int32 *vendorIDP,
			  int32 *deviceIDP, int32 *headTypeP, int32 *secondBusP);
static int32 PciCfgErr( BBIS_HANDLE *h, char *funcName, int32 error,
			u_int32 pciBusNbr, u_int32 pciDevNbr, u_int32 reg );
static int32 CfgInfoSlot( BBIS_HANDLE *brdHdl, va_list argptr );


/**************************** D203_GetEntry *********************************
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
  extern void __D203_GetEntry( BBIS_ENTRY *bbisP )
#endif
{
  /* init/exit */
  bbisP->init         =   D203_Init;
  bbisP->brdInit      =   D203_BrdInit;
  bbisP->brdExit      =   D203_BrdExit;
  bbisP->exit         =   D203_Exit;
  bbisP->fkt04        =   D203_Unused;
  /* info */
  bbisP->brdInfo      =   D203_BrdInfo;
  bbisP->cfgInfo      =   D203_CfgInfo;
  bbisP->fkt07        =   D203_Unused;
  bbisP->fkt08        =   D203_Unused;
  bbisP->fkt09        =   D203_Unused;
  /* interrupt handling */
  bbisP->irqEnable    =   D203_IrqEnable;
  bbisP->irqSrvInit   =   D203_IrqSrvInit;
  bbisP->irqSrvExit   =   D203_IrqSrvExit;
  bbisP->setIrqHandle =   NULL;
  bbisP->fkt14        =   D203_Unused;
  /* exception handling */
  bbisP->expEnable    =   D203_ExpEnable;
  bbisP->expSrv       =   D203_ExpSrv;
  bbisP->fkt17        =   D203_Unused;
  bbisP->fkt18        =   D203_Unused;
  bbisP->fkt19        =   D203_Unused;
  /* */
  bbisP->fkt20        =   D203_Unused;
  bbisP->fkt21        =   D203_Unused;
  bbisP->fkt22        =   D203_Unused;
  bbisP->fkt23        =   D203_Unused;
  bbisP->fkt24        =   D203_Unused;
  /*  getstat / setstat / address setting */
  bbisP->setStat      =   D203_SetStat;
  bbisP->getStat      =   D203_GetStat;
  bbisP->setMIface    =   D203_SetMIface;
  bbisP->clrMIface    =   D203_ClrMIface;
  bbisP->getMAddr     =   D203_GetMAddr;
  bbisP->fkt30        =   D203_Unused;
  bbisP->fkt31        =   D203_Unused;
}

/****************************** D203_Init ***********************************
 *
 *  Description:  Allocate and return board handle.
 *
 *                - initializes the board handle
 *                - reads and saves board descriptor entries
 *                - assign resources
 *                - map used addresses
 *
 *                The following descriptor keys are used:
 *
 *                Deskriptor key         Default          Range
 *                ---------------------  ---------------  -------------------
 *                DEBUG_LEVEL_DESC       OSS_DBG_DEFAULT  see dbg.h
 *                DEBUG_LEVEL            OSS_DBG_DEFAULT  see dbg.h
 *                PCI_BUS_PATH		     none             see pcibuspath.txt
 *                PCI_BUS_NUMBER	     none             0...255
 *                PCI_BUS_SLOT           none             1...8
 *                PCI_DEVICE_ID          0xFFFF	          0...31
 *                ID_CHECK               1                0, 1
 *
 *                Either PCI_BUS_NUMBER or PCI_BUS_PATH must be specified
 *				  If both are present, PCI_BUS_NUMBER takes precedence.
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
static int32 D203_Init(
		       OSS_HANDLE      *osHdl,
		       DESC_SPEC       *descSpec,
		       BBIS_HANDLE     **brdHdlP )
{
  BBIS_HANDLE		*brdHdl = NULL;
  u_int32			gotsize;
  int32			status, id;
  u_int32			mechSlot, mSlot, value, i;
  OSS_RESOURCES	res[BRD_MODULE_NBR];

  /*-------------------------------+
    | initialize the board structure |
    +-------------------------------*/
  /* get memory for the board structure */
  *brdHdlP = brdHdl = (BBIS_HANDLE*) (OSS_MemGet(
						 osHdl, sizeof(BBIS_HANDLE), &gotsize ));
  if ( brdHdl == NULL )
    return ERR_OSS_MEM_ALLOC;

  /* clear */
  OSS_MemFill(osHdl, gotsize, (char*)brdHdl, 0x00);

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
  status = DESC_Init( descSpec, osHdl, &brdHdl->descHdl );
  if (status)
    return( Cleanup(brdHdl,status) );

  /* get DEBUG_LEVEL_DESC - optional*/
  status = DESC_GetUInt32(brdHdl->descHdl, OSS_DBG_DEFAULT, &value,
			  "DEBUG_LEVEL_DESC");
  if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
    return( Cleanup(brdHdl,status) );

  /* set debug level for DESC module */
  DESC_DbgLevelSet(brdHdl->descHdl, value);

  /* get DEBUG_LEVEL - optional */
  status = DESC_GetUInt32( brdHdl->descHdl, OSS_DBG_DEFAULT, &(brdHdl->dbgLev),
			   "DEBUG_LEVEL");
  if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
    return( Cleanup(brdHdl,status) );

  DBGWRT_1((DBH,"BB - %s_Init\n",BRD_NAME));

  /* PCI_DOMAIN_NUMBER - optional */
  status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->pciDomainNbr, 
			   "PCI_DOMAIN_NUMBER");
							 
  if ( status == ERR_DESC_KEY_NOTFOUND ) {
    /* default pci domain is 0 */
    brdHdl->pciDomainNbr = 0;
  }
	
  /* PCI_BUS_NUMBER - required if PCI_BUS_PATH not given  */
  status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->pciBusNbr,
			   "PCI_BUS_NUMBER");

  if( status == ERR_DESC_KEY_NOTFOUND ){

    /* PCI_BUS_PATH - required if PCI_DEVICE_ID not given */
    brdHdl->pciPathLen = MAX_PCI_PATH;

    status = DESC_GetBinary( brdHdl->descHdl, (u_int8*)"", 0,  
			     brdHdl->pciPath, &brdHdl->pciPathLen,
			     "PCI_BUS_PATH");

    if( status ){
      DBGWRT_ERR((DBH, "*** BB - %s_Init: Desc Key PCI_BUS_PATH "
		  "AND PCI_BUS_NUMBER not found!\n",	BRD_NAME));
      return( Cleanup(brdHdl,status) );
    }

#ifndef VXW_PCI_DOMAIN_SUPPORT 
    /* ts: tweak for F50P + vxW64. TODO: clean up when also supporting F50P on vxW69 !!! */
    DESC_GetUInt32( brdHdl->descHdl, 0, &mechSlot, "PCI_BUS_SLOT");
    brdHdl->pciDomainNbr = 0;
    brdHdl->pciPathLen = 1;
    brdHdl->pciPath[0] = 0x11 - mechSlot;
#endif


    /*--------------------------------------------------------+
      |  parse the PCI_PATH to determine bus number of devices  |
      +--------------------------------------------------------*/
#ifdef DBG	
    DBGWRT_2((DBH, " PCI_PATH="));
    for(i=0; i<brdHdl->pciPathLen; i++){
      DBGWRT_2((DBH, "0x%x", brdHdl->pciPath[i]));
    }
    DBGWRT_2((DBH, "\n"));
#endif
    if( (status = ParsePciPath( brdHdl, &brdHdl->pciBusNbr )) )
      return( Cleanup(brdHdl,status));
	
  } /* if( status == ERR_DESC_KEY_NOTFOUND ) */
  else {
    if( status == ERR_SUCCESS) {
      DBGWRT_1((DBH,"BB - %s: Using main PCI Bus Number from desc %d\n", 
		BRD_NAME, brdHdl->pciBusNbr ));
    }
    else {
      return( Cleanup(brdHdl,status) );
    }
  }

  /* PCI_DEVICE_ID - required if PCI_BUS_SLOT not given  */
  status = DESC_GetUInt32( brdHdl->descHdl, 0xffff, &brdHdl->pciDev, "PCI_DEVICE_ID");
  if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
    return( Cleanup(brdHdl,status) );

  if(status==ERR_DESC_KEY_NOTFOUND){

    /* PCI_BUS_SLOT - required if PCI_DEVICE_ID not given */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &mechSlot, "PCI_BUS_SLOT");
    if( status==ERR_DESC_KEY_NOTFOUND ){
      DBGWRT_ERR((DBH, "*** BB - %s_Init: Desc Key PCI_BUS_SLOT "
		  "AND PCI_DEVICE_ID not found!\n", BRD_NAME));
    }
    if( status )
      return( Cleanup(brdHdl,status) );

    /* convert PCI slot into PCI device id */
    if( (status = OSS_PciSlotToPciDevice( osHdl, 
					  OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr), 
					  mechSlot, (int32*)&brdHdl->pciDev)) )
      return( Cleanup(brdHdl,status) );
  }

  /* ID_CHECK */
  status = DESC_GetUInt32( brdHdl->descHdl, 1, &brdHdl->idCheck, "ID_CHECK");
  if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
    return( Cleanup(brdHdl,status) );

  /*
   * Get Trigger Settings
   */
  /* PXI_TRIG_SRC */
  status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->trigPxiSrc, "PXI_TRIG_SRC");
  if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
    return( Cleanup(brdHdl,status) );

  /* PXI_TRIG_DST */
  status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->trigPxiDst, "PXI_TRIG_DST");
  if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
    return( Cleanup(brdHdl,status) );

  /* prohibit senseless settings */
  if( (brdHdl->trigPxiSrc > 0xff) ||
      (brdHdl->trigPxiDst > 0xff) ||
      (brdHdl->trigPxiSrc & brdHdl->trigPxiDst) ){
    DBGWRT_ERR((DBH, "*** BB - %s_Init: illegal PXI_TRIG_SRC=0x%x"
		"/ PXI_TRIG_DST=0x%x setting\n", BRD_NAME,
		brdHdl->trigPxiSrc, brdHdl->trigPxiDst));
    return( Cleanup(brdHdl,ERR_BBIS_DESC_PARAM) );
  }

  /* module trigger params */
  for( mSlot=0; mSlot<BRD_MODULE_NBR; mSlot++) {

    /* MSLOT_n/TRIGA_SRC */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->trigASrc[mSlot],
			     "MSLOT_%d/TRIGA_SRC", mSlot);
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
      return( Cleanup(brdHdl,status) );

    /* MSLOT_n/TRIGA_DST */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->trigADst[mSlot],
			     "MSLOT_%d/TRIGA_DST", mSlot);
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
      return( Cleanup(brdHdl,status) );

    /* prohibit senseless settings */
    if( (brdHdl->trigASrc[mSlot] > 0xff) ||
	(brdHdl->trigADst[mSlot] > 0xff) ||
	(brdHdl->trigASrc[mSlot] & brdHdl->trigADst[mSlot]) ){
      DBGWRT_ERR((DBH, "*** BB - %s_Init: illegal MSLOT_%d/TRIGA_SRC=0x%x / "
		  "MSLOT_%d/TRIGA_DST=0x%x setting\n", BRD_NAME,
		  mSlot, brdHdl->trigASrc[mSlot],
		  mSlot, brdHdl->trigADst[mSlot]));
      return( Cleanup(brdHdl,ERR_BBIS_DESC_PARAM) );
    }

    /* MSLOT_n/TRIGB_SRC */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->trigBSrc[mSlot],
			     "MSLOT_%d/TRIGB_SRC", mSlot);
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
      return( Cleanup(brdHdl,status) );

    /* MSLOT_n/TRIGB_DST */
    status = DESC_GetUInt32( brdHdl->descHdl, 0, &brdHdl->trigBDst[mSlot],
			     "MSLOT_%d/TRIGB_DST", mSlot);
    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
      return( Cleanup(brdHdl,status) );

    /* prohibit senseless settings */
    if( (brdHdl->trigBSrc[mSlot] > 0xff) ||
	(brdHdl->trigBDst[mSlot] > 0xff) ||
	(brdHdl->trigBSrc[mSlot] & brdHdl->trigBDst[mSlot]) ){
      DBGWRT_ERR((DBH, "*** BB - %s_Init: illegal MSLOT_%d/TRIGB_SRC=0x%x / "
		  "MSLOT_%d/TRIGB_DST=0x%x setting\n", BRD_NAME,
		  mSlot, brdHdl->trigBSrc[mSlot],
		  mSlot, brdHdl->trigBDst[mSlot]));
      return( Cleanup(brdHdl,ERR_BBIS_DESC_PARAM) );
    }
  }

  /* exit descHdl */
  status = DESC_Exit( &brdHdl->descHdl );
  if (status)
    return( Cleanup(brdHdl,status) );

  if( brdHdl->idCheck ){
    /*------------------------------+
      | check vendor-id and device-id |
      +------------------------------*/
    /* get vendor-id */
    if( (status = OSS_PciGetConfig( osHdl, 
				    OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr), 
				    brdHdl->pciDev, 0, OSS_PCI_VENDOR_ID, &id )) )
      return( Cleanup(brdHdl,status) );

    /* verify vendor-id */
    if( id != BRD_PCI_VEN_ID ){
      DBGWRT_ERR((DBH, "*** BB - %s_Init: illegal vendor-id=0x%4x\n",
		  BRD_NAME, id));
      return( Cleanup(brdHdl,ERR_BBIS_ILL_ID) );
    }

    /* get device-id */
    if( (status = OSS_PciGetConfig( osHdl, 
				    OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr), 
				    brdHdl->pciDev, 0, OSS_PCI_DEVICE_ID, &id )) )
      return( Cleanup(brdHdl,status) );

    /* verify device-id */
    if( id != BRD_PCI_DEV_ID ){
      DBGWRT_ERR((DBH, "*** BB - %s_Init: illegal device-id=0x%4x\n",
		  BRD_NAME, id));
      return( Cleanup(brdHdl,ERR_BBIS_ILL_ID) );
    }
  }

  /*-------------------------------------------+
    | get, assign and map module ctrl-reg spaces |
    +-------------------------------------------*/
  /* get base address from BAR0 */
  if( (status = OSS_BusToPhysAddr( osHdl, OSS_BUSTYPE_PCI, &brdHdl->bar0,
				   OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr), 
				   brdHdl->pciDev, 0, 0)) )
    return( Cleanup(brdHdl,status) );

  /* compute module ctrl-reg spaces */
  for( i=0; i<BRD_MODULE_NBR; i++){
    res[i].type = OSS_RES_MEM;
    res[i].u.mem.physAddr =	(void*)(
					(U_INT32_OR_64)brdHdl->bar0 +
					BRD_MBASE_OFFSET(i) + 
					BRD_CTRLR_OFFSET );
    res[i].u.mem.size = BRD_CTRL_SIZE;
  }

  /* assign the resources */
  if( (status = OSS_AssignResources( osHdl, OSS_BUSTYPE_PCI, 
				     OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr),
				     BRD_MODULE_NBR, res )) )
    return( Cleanup(brdHdl,status) );

  brdHdl->resourcesAssigned = TRUE;

  /* map module ctrl-reg spaces */
  for( i=0; i<BRD_MODULE_NBR; i++ ){
    status = OSS_MapPhysToVirtAddr( osHdl,
				    res[i].u.mem.physAddr, res[i].u.mem.size,
				    OSS_ADDRSPACE_MEM, OSS_BUSTYPE_PCI,
				    OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr),	
				    &brdHdl->virtCtrlBase[i] );
    if( status )
      return( Cleanup(brdHdl,status) );
  }

  /*------------------------------+
    | get interrupt information     |
    +------------------------------*/
  /* get interrupt line */
  status = OSS_PciGetConfig( osHdl, 
			     OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr), 
			     brdHdl->pciDev, 0, OSS_PCI_INTERRUPT_LINE, &brdHdl->irqLevel );
	
  /* no interrupt connected */
  if( status || (brdHdl->irqLevel == 0xff) )
    return( Cleanup(brdHdl,ERR_BBIS_NO_IRQ) );

  /* convert level to vector */
  if( (status = OSS_IrqLevelToVector( brdHdl->osHdl, OSS_BUSTYPE_PCI,
				      brdHdl->irqLevel, &brdHdl->irqVector )) )
    return( Cleanup(brdHdl,status) );

  DBGWRT_2((DBH," IRQ level=0x%x, vector=0x%x\n",
	    brdHdl->irqLevel, brdHdl->irqVector));

  return 0;
}

/****************************** D203_BrdInit ********************************
 *
 *  Description:  Board initialization.
 *
 *				  - enable global interrupt
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure    
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_BrdInit(
			  BBIS_HANDLE     *brdHdl )
{
  u_int32	mSlot;

  DBGWRT_1((DBH, "BB - %s_BrdInit\n",BRD_NAME));
	
  /* enable global interrupt to detect exception (timeout) interrupts */
  MSETMASK_D16( brdHdl->virtCtrlBase[0], D203_GCTRL, D203_GCTRL_GIEN );

  /* configure trigger */
  MWRITE_D16( brdHdl->virtCtrlBase[0], D203_TRIGSRC_GPXI, brdHdl->trigPxiSrc );
  MWRITE_D16( brdHdl->virtCtrlBase[0], D203_TRIGDST_GPXI, brdHdl->trigPxiDst );

  for( mSlot=0; mSlot<BRD_MODULE_NBR; mSlot++) {
    MWRITE_D16( brdHdl->virtCtrlBase[mSlot], D203_TRIGSRC_LMMOD,
		brdHdl->trigASrc[mSlot] | (brdHdl->trigBSrc[mSlot]<<8) );
    MWRITE_D16( brdHdl->virtCtrlBase[mSlot], D203_TRIGDST_LMMOD,
		brdHdl->trigADst[mSlot] | (brdHdl->trigBDst[mSlot]<<8) );
  }

  return 0;
}

/****************************** D203_BrdExit ********************************
 *
 *  Description:  Board deinitialization.
 *
 *				  - disable global interrupt
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_BrdExit(
			  BBIS_HANDLE     *brdHdl )
{
  DBGWRT_1((DBH, "BB - %s_BrdExit\n",BRD_NAME));

  /* disable global interrupt */
  MCLRMASK_D16( brdHdl->virtCtrlBase[0], D203_GCTRL, D203_GCTRL_GIEN );

  return 0;
}

/****************************** D203_Exit ***********************************
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
static int32 D203_Exit(
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

/****************************** D203_BrdInfo ********************************
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
static int32 D203_BrdInfo(
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

        *numSlot = BRD_MODULE_NBR;
        break;
      }
		
      /* bus type */
    case BBIS_BRDINFO_BUSTYPE:
      {
        u_int32 *busType = va_arg( argptr, u_int32* );

	*busType = BRD_BUSTYPE;
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
	 * build hw name (e.g. D203 board)
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

/****************************** D203_CfgInfo ********************************
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
static int32 D203_CfgInfo(
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
      *busNbr = brdHdl->pciBusNbr;
      break;
    }

    /* pci domain number */
  case BBIS_CFGINFO_PCI_DOMAIN:
    {
      /* domain number */
      u_int32 *domainNbr = va_arg( argptr, u_int32* );
      u_int32 mSlot      = va_arg( argptr, u_int32 );
	
      *domainNbr = brdHdl->pciDomainNbr;
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

      /* M-Module */
      if( mSlot < BBIS_SLOTS_ONBOARDDEVICE_START ){
	*mode = BBIS_IRQ_SHARED | BBIS_IRQ_EXCEPTION;
	*level = brdHdl->irqLevel;
	*vector = brdHdl->irqVector;
      }
      /* Trigger device */
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

/****************************** D203_IrqEnable ******************************
 *
 *  Description:  Interrupt enable / disable.
 *
 *				  Enable/disable interrupt for specified module slot
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                enable    interrupt setting                   
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_IrqEnable(
			    BBIS_HANDLE     *brdHdl,
			    u_int32         mSlot,
			    u_int32         enable )
{
  DBGWRT_1((DBH, "BB - %s_IrqEnable: mSlot=0x%04x enable=%d\n",BRD_NAME,mSlot,enable));

  /* Trigger device? */
  if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START ){
    DBGWRT_ERR((DBH,"*** %s_IrqEnable: function for mSlot=0x%x not supported\n"
		,BRD_NAME,mSlot));
    return ERR_BBIS_ILL_FUNC;
  }

  if(enable)
    {
      /* enable */
      MSETMASK_D16( brdHdl->virtCtrlBase[mSlot], D203_LCTRL, D203_LCTRL_IEN );
    }
  else
    {
      /* disable */
      MCLRMASK_D16( brdHdl->virtCtrlBase[mSlot], D203_LCTRL, D203_LCTRL_IEN );
    }

  return 0;
}

/****************************** D203_IrqSrvInit *****************************
 *
 *  Description:  Called at the beginning of an interrupt.
 *
 *                - determine interrupt reason 
 *                
 *                Note: Timeout interrupts will be cleared.
 *                      Module interrupt must be cleared on the module
 *                      itself and by D203_IrqSrvExit().
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *  Output.....:  return    BBIS_IRQ_UNK
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_IrqSrvInit(
			     BBIS_HANDLE     *brdHdl,
			     u_int32         mSlot)
{
  u_int16		ctrlReg;
  int32		irqEvent=0;

  IDBGWRT_1((DBH, "BB - %s_IrqSrvInit: mSlot=0x%04x\n",BRD_NAME,mSlot));

  /*------------------------------+
    | determine interrupt reason    |
    +------------------------------*/
  /* get the interrupt information */
  ctrlReg = MREAD_D16(brdHdl->virtCtrlBase[mSlot], D203_LCTRL);

  /* timeout error ? */
  if(ctrlReg & D203_LCTRL_TOUT){
    IDBGWRT_ERR((DBH,"*** %s_IrqSrvInit: mSlot=%d - interrupt by timeout\n",
		 BRD_NAME, mSlot));

    irqEvent |= BBIS_IRQ_EXP;
    brdHdl->expIrqCount++;	
        
    /* clear timeout interrupt */
    MCLRMASK_D16( brdHdl->virtCtrlBase[mSlot], D203_LCTRL, D203_LCTRL_TOUT );
  }

  /* module interrupt ? */
  if(ctrlReg & D203_LCTRL_IP){
    IDBGWRT_2((DBH," interrupt by module\n"));
    irqEvent |= BBIS_IRQ_YES;

    /* Note: the module interrupt will be cleared
       by the module and D203_IrqSrvExit() */
  }
  else{
    IDBGWRT_2((DBH," not my interrupt\n"));
    irqEvent |= BBIS_IRQ_NO;
  }

  return irqEvent;
}

/****************************** D203_IrqSrvExit *****************************
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
static void D203_IrqSrvExit(
			    BBIS_HANDLE     *brdHdl,
			    u_int32         mSlot )
{
  IDBGWRT_1((DBH, "BB - %s_IrqSrvExit: mSlot=0x%04x\n",BRD_NAME,mSlot));

  /* clear interrupt */
  MCLRMASK_D16(brdHdl->virtCtrlBase[mSlot], D203_LCTRL, D203_LCTRL_IP);
}

/****************************** D203_ExpEnable ******************************
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
static int32 D203_ExpEnable(
			    BBIS_HANDLE     *brdHdl,
			    u_int32         mSlot,
			    u_int32			enable)
{
  IDBGWRT_1((DBH, "BB - %s_ExpEnable: mSlot=0x%04x\n",BRD_NAME,mSlot));

  return 0;
}

/****************************** D203_ExpSrv *********************************
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
static int32 D203_ExpSrv(
			 BBIS_HANDLE     *brdHdl,
			 u_int32         mSlot )
{
  IDBGWRT_1((DBH, "BB - %s_ExpSrv: mSlot=0x%04x\n",BRD_NAME,mSlot));

  return BBIS_IRQ_NO;
}

/****************************** D203_SetMIface ******************************
 *
 *  Description:  Set device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24       
 *                dataMode  MDIS_MODE_D2036 | MDIS_MODE_D32       
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_SetMIface(
			    BBIS_HANDLE     *brdHdl,
			    u_int32         mSlot,
			    u_int32         addrMode,
			    u_int32         dataMode)
{
  DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=0x%04x\n",BRD_NAME,mSlot));

  return 0;
}

/****************************** D203_ClrMIface ******************************
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
static int32 D203_ClrMIface(
			    BBIS_HANDLE     *brdHdl,
			    u_int32         mSlot)
{
  DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=0x%04x\n",BRD_NAME,mSlot));

  return 0;
}

/****************************** D203_GetMAddr *******************************
 *
 *  Description:  Get physical address description.
 *
 *                - check module slot number
 *                - assign address spaces
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                addrMode  MDIS_MA08  
 *                dataMode  MDIS_MD16 | MDIS_MD32      
 *                mAddr     pointer to address space            
 *                mSize     size of address space               
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_GetMAddr(
			   BBIS_HANDLE     *brdHdl,
			   u_int32         mSlot,
			   u_int32         addrMode,
			   u_int32         dataMode,
			   void            **mAddr,
			   u_int32         *mSize )
{
  u_int32 offset;
	
  DBGWRT_1((DBH, "BB - %s_GetMAddr: mSlot=0x%04x\n",BRD_NAME,mSlot));


  /*------------------------------+
    | M-Module                      |
    +------------------------------*/
  if( mSlot < BRD_MODULE_NBR ){
		
    /* A08 */
    if( addrMode == MDIS_MA08 ){
      /* D16 */
      if( (dataMode == MDIS_MD16) || (dataMode == MDIS_MD08) )
	offset = BRD_A08_D16_OFFSET;
#ifdef BRD_A08_D32_OFFSET
      /* D32 */
      else if( dataMode == MDIS_MD32 )
	offset = BRD_A08_D32_OFFSET;
#endif
      /* unsupported data mode */
      else {
	DBGWRT_ERR((DBH,"*** %s_GetMAddr: dataMode=0x%x not supported\n",
		    BRD_NAME, dataMode));
	return ERR_BBIS_ILL_ADDRMODE;
      }

      *mSize = BRD_A08_SIZE;
    }

#ifdef BRD_A24_D32_OFFSET
    /* A24 */
    else if( addrMode == MDIS_MA24 ){
      /* D16 */
      if( (dataMode == MDIS_MD16) || (dataMode == MDIS_MD08) )
	offset = BRD_A24_D16_OFFSET;
      /* D32 */
      else if( dataMode == MDIS_MD32 )
	offset = BRD_A24_D32_OFFSET;
      /* unsupported data mode */
      else {
	DBGWRT_ERR((DBH,"*** %s_GetMAddr: dataMode=0x%x not supported\n",
		    BRD_NAME, dataMode));
	return ERR_BBIS_ILL_ADDRMODE;
      }

      *mSize = BRD_A24_SIZE;
    }
#endif

    /* unsupported address mode */
    else {
      DBGWRT_ERR((DBH,"*** %s_GetMAddr: addrMode=0x%x not supported\n",
		  BRD_NAME, addrMode));
      return ERR_BBIS_ILL_ADDRMODE;
    }
    *mAddr = (void*)((U_INT32_OR_64)(brdHdl->bar0) + BRD_MBASE_OFFSET(mSlot) + offset);
  }
  /*------------------------------+
    | Illegal module slot number    |
    +------------------------------*/
  else{
    DBGWRT_ERR((DBH,"*** %s_GetMAddr: wrong module slot number=%d\n",
		BRD_NAME, mSlot));
    return ERR_BBIS_ILL_SLOT;
  }

  DBGWRT_2((DBH, " mSlot=0x%04x : mem address=%08p, length=0x%x\n",
	    mSlot, *mAddr, *mSize));

  return 0;
}

/****************************** D203_SetStat ********************************
 *
 *  Description:  Set driver status
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
static int32 D203_SetStat(
			  BBIS_HANDLE     *brdHdl,
			  u_int32         mSlot,
			  int32           code,
			  INT32_OR_64     value32_or_64 )
{
  int32		value	= (int32)value32_or_64;		/* 32bit value */
	
  DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=0x%04x code=0x%04x value=0x%x\n",
	    BRD_NAME, mSlot, code, value));

  switch (code) {
        
    /* set debug level */
  case M_BB_DEBUG_LEVEL:
    brdHdl->dbgLev = value;
    break;

    /* set exception counter */
  case M_BB_IRQ_EXP_COUNT:
    brdHdl->expIrqCount = value;
    break;

    /* unknown */
  default:
    return ERR_BBIS_UNK_CODE;
  }

  return 0;
}

/****************************** D203_GetStat ********************************
 *
 *  Description:  Get driver status
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
 *                M_BB_ID_CHECK        board id is checked        0..1
 *                M_BB_ID_SIZE         eeprom id-data size[bytes] 0
 *                M_MK_BLK_REV_ID      ident function table ptr   -
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure           
 *                mSlot     module slot number                          
 *                code      getstat code                                
 *  Output.....:  value32_or_64P    getstat value or ptr to blockgetstat data
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_GetStat(
			  BBIS_HANDLE     *brdHdl,
			  u_int32         mSlot,
			  int32           code,
			  INT32_OR_64     *value32_or_64P )
{
  int32		*valueP = (int32*)value32_or_64P;		/* pointer to 32bit value */
	
  DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=0x%04x code=0x%04x\n",BRD_NAME,mSlot,code));

  switch (code) {
    /* get debug level */
  case M_BB_DEBUG_LEVEL:
    *valueP = brdHdl->dbgLev;
    break;

    /* get exception counter */
  case M_BB_IRQ_EXP_COUNT:
    /* Trigger device? */
    if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START )
      return ERR_BBIS_ILL_FUNC;
    *valueP = brdHdl->expIrqCount;
    break;

    /* get IRQ vector */
  case M_BB_IRQ_VECT:
    /* Trigger device? */
    if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START )
      return ERR_BBIS_ILL_FUNC;
    *valueP = brdHdl->irqVector;
    break;

    /* get IRQ level */
  case M_BB_IRQ_LEVEL:
    /* Trigger device? */
    if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START )
      return ERR_BBIS_ILL_FUNC;
    *valueP = brdHdl->irqLevel;
    break;

    /* get IRQ priority */
  case M_BB_IRQ_PRIORITY:
    /* Trigger device? */
    if( mSlot >= BBIS_SLOTS_ONBOARDDEVICE_START )
      return ERR_BBIS_ILL_FUNC;
    *valueP = 0;
    break;

    /* get board id check state */
  case M_BB_ID_CHECK:
    *valueP = brdHdl->idCheck;
    break;

    /* id prom size */
  case M_BB_ID_SIZE:
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

/****************************** D203_Unused *********************************
 *
 *  Description:  Dummy function for unused jump table entries.
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  return  ERR_BBIS_ILL_FUNC
 *  Globals....:  ---
 ****************************************************************************/
static int32 D203_Unused( void )		/* nodoc */
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
	  BRD_NAME	
	  "  Base Board Handler: $Id: bb_d203.c,v 1.18 2013/11/28 17:00:24 ts Exp $" );
}

/********************************* Cleanup **********************************
 *
 *  Description:  Close all handles, unmap addresses, free memory
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
  if (brdHdl->descHdl)
    DESC_Exit(&brdHdl->descHdl);

  /* cleanup debug */
  DBGEXIT((&DBH));

  /*------------------------------+
    |  unmap addresses              |
    +------------------------------*/
  /* unmap already mapped control register spaces */
  for ( i=0; i<BRD_MODULE_NBR; i++)
    if( brdHdl->virtCtrlBase[i] )
      OSS_UnMapVirtAddr( brdHdl->osHdl, &brdHdl->virtCtrlBase[i],
			 BRD_CTRL_SIZE, OSS_ADDRSPACE_MEM );

  /*------------------------------+
    |  unassign resources           |
    +------------------------------*/
#ifdef OSS_HAS_UNASSIGN_RESOURCES
  if( brdHdl->resourcesAssigned ){
    OSS_RESOURCES	res[BRD_MODULE_NBR];

    /* compute module ctrl-reg spaces */
    for( i=0; i<BRD_MODULE_NBR; i++){
      res[i].type = OSS_RES_MEM;
      res[i].u.mem.physAddr =	(void*)(
					(U_INT32_OR_64)brdHdl->bar0 +
					BRD_MBASE_OFFSET(i) + 
					BRD_CTRLR_OFFSET );
      res[i].u.mem.size = BRD_CTRL_SIZE;
    }

    /* unassign the resources */
    OSS_UnAssignResources( brdHdl->osHdl, OSS_BUSTYPE_PCI, 
			   OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr),
			   BRD_MODULE_NBR, res );
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

/********************************* ParsePciPath *****************************
 *
 *  Description: Parses the specified PCI_BUS_PATH to find out PCI Bus Number
 *			   		   
 *---------------------------------------------------------------------------
 *  Input......: brdHdl			handle
 *  Output.....: returns:	   	error code
 *				 *pciBusNbrP	main PCI bus number of D203
 *  Globals....: -
 ****************************************************************************/
static int32 ParsePciPath( BBIS_HANDLE *brdHdl, u_int32 *pciBusNbrP ) 	/* nodoc */
{
  u_int32 i;
  int32 pciBusNbr=0, pciDevNbr;
  int32 error;
  int32 vendorID, deviceID, headerType, secondBus;

  /* parse whole pci path until our device is reached */
  for (i=0; i<brdHdl->pciPathLen; i++){

    pciDevNbr = brdHdl->pciPath[i];
		
    if ( ( i==0 ) 
#ifdef VXW_PCI_DOMAIN_SUPPORT 
	 && ( 0 != h->pciDomainNbr ) 
#endif
	 ) {
      /* as we do not know the numbering order of busses on pci domains, 
	 try to find the device on all busses instead of looking for the 
	 first bus on the domain                                        */
      for(pciBusNbr=0; pciBusNbr<0xff; pciBusNbr++) {
	error = PciParseDev( brdHdl, OSS_MERGE_BUS_DOMAIN(pciBusNbr, brdHdl->pciDomainNbr), 
			     brdHdl->pciPath[0], &vendorID, &deviceID, &headerType,
			     &secondBus );
	if ( error == ERR_SUCCESS && vendorID != 0xffff && deviceID != 0xffff   ) 
	  break; /* found device */
      }
			
      if ( error != ERR_SUCCESS ) { /* device not found */
	DBGWRT_ERR((DBH,"*** BB - %s: first device 0x%02x in pci bus path "
		    "not found on domain %d!\n",
		    BRD_NAME, brdHdl->pciPath[0], brdHdl->pciDomainNbr ));
	return error;               
      }    
    } else {
      /* parse device only once */
      if( (error = PciParseDev( brdHdl, OSS_MERGE_BUS_DOMAIN(pciBusNbr, brdHdl->pciDomainNbr), 
				pciDevNbr, &vendorID, &deviceID, &headerType,
				&secondBus )))
	return error;
    }

    if( vendorID == 0xffff && deviceID == 0xffff ){
      DBGWRT_ERR((DBH,"*** BB - %s:ParsePciPath: Nonexistant device "
		  "domain %d bus %d dev %d\n", BRD_NAME, brdHdl->pciDomainNbr, 
		  pciBusNbr, pciDevNbr ));
      return ERR_BBIS_NO_CHECKLOC;
    }

#ifdef VXW_PCI_DOMAIN_SUPPORT 
    /*--- device is present, is it a bridge ? ---*/
    if( (headerType & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE ){
      DBGWRT_ERR((DBH,"*** BB - %s:ParsePciPath: Device is not a bridge!"
		  "domain %d bus %d dev %d vend=0x%x devId=0x%x\n", 
		  BRD_NAME, brdHdl->pciDomainNbr, pciBusNbr, pciDevNbr, vendorID, 
		  deviceID ));

      return ERR_BBIS_NO_CHECKLOC;
    }
			
    /*--- it is a bridge, determine its secondary bus number ---*/
    DBGWRT_2((DBH, " domain %d bus %d dev %d: vend=0x%x devId=0x%x second bus %d\n",
	      brdHdl->pciDomainNbr, pciBusNbr, pciDevNbr, vendorID, deviceID, secondBus ));

    /*--- continue with new bus ---*/
    pciBusNbr = secondBus;
#endif
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
		
  DBGWRT_2((DBH, " domain %d bus %d dev %d.%d: vend=0x%x devId=0x%x hdrtype %d\n",
	    OSS_DOMAIN_NBR( pciBusNbr ), OSS_BUS_NBR( pciBusNbr ), pciMainDevNbr, 
	    pciDevFunc, *vendorIDP, *deviceIDP, *headerTypeP ));

  if( ((*headerTypeP) & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE )	
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
 *                  - Onboard-Dev:	"onboard slot 0x1000"
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
 *                  - Onboard-Dev:	"D203_TRIG"
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
  u_int32 mSlot     = va_arg( argptr, u_int32 );
  u_int32 *occupied = va_arg( argptr, u_int32* );
  u_int32 *devId    = va_arg( argptr, u_int32* );
  u_int32 *devRev   = va_arg( argptr, u_int32* );
  char	*slotName = va_arg( argptr, char* );
  char	*devName  = va_arg( argptr, char* );

  /* clear parameters to return (for error case) */
  *occupied = 0;
  *devId    = 0;
  *devRev   = 0;
  *slotName = '\0';

  /* set defaults */
  *devName  = '\0'; /* indicates BBIS_SLOT_STR_UNK */

  /*------------------------------+
    | M-Module                      |
    +------------------------------*/
  if( mSlot < BRD_MODULE_NBR ){

    int32	status, status2;
    u_int32	modAddrSize;
    u_int16	irqSetting;
    void	*physModAddr, *virtModAddr;
    u_int32 modType;

    /* build slot name */
    OSS_Sprintf( brdHdl->osHdl, slotName, "M-Module slot %d", mSlot);

    /*------------------------------+
      | get module information        |
      +------------------------------*/
    INT_DISABLE( brdHdl );	/* disable interrupt */

    /* get physical module addr */
    if( (status = D203_GetMAddr( brdHdl, mSlot, MDIS_MA08, MDIS_MD16,
				 &physModAddr, &modAddrSize )) ){
      INT_ENABLE( brdHdl );	/* enable interrupt */
      return status;
    }
		
    /* init module slot */
    if( (status = D203_SetMIface( brdHdl, mSlot, MDIS_MA08, MDIS_MD16 )) ){
      INT_ENABLE( brdHdl );	/* enable interrupt */
      return status;
    }
		
    /*
     * map module address space
     * (quick and dirty without resource assignment)
     */ 
    if( (status = OSS_MapPhysToVirtAddr( brdHdl->osHdl, physModAddr, modAddrSize,
					 OSS_ADDRSPACE_MEM, OSS_BUSTYPE_PCI,
					 OSS_MERGE_BUS_DOMAIN(brdHdl->pciBusNbr, brdHdl->pciDomainNbr), 
					 &virtModAddr )) ){
      D203_ClrMIface( brdHdl, mSlot );
      INT_ENABLE( brdHdl );	/* enable interrupt */
      return status;
    }

    /*
     * now get the information
     * Note: Returns BBIS_SLOT_NBR_UNK for devId/devRev if unknown.
     */
    m_getmodinfo( (U_INT32_OR_64)virtModAddr, &modType,
		  devId, devRev, devName );

    DBGWRT_2((DBH," modType=%d, devId=0x%08x, devRev=0x%08x, devName=%s\n",
	      modType, *devId, *devRev, devName ));

    /*
     * If a m-module hasn't asserted DTACK# within 16us
     * after the assertion of CS# we get an timeout interrupt.
     * In this case, we asume that no m-module is plugged in the slot. 
     */
    OSS_MikroDelay(brdHdl->osHdl, 50);	/* wait 50us */

    /* get the interrupt information */
    irqSetting = MREAD_D16(brdHdl->virtCtrlBase[mSlot], D203_LCTRL);

    /*------------------------------+
      | slot is not occupied          |
      +------------------------------*/
    if(irqSetting & D203_LCTRL_TOUT){
      DBGWRT_2((DBH," mSlot=%d - interrupt by timeout\n", mSlot));

      /* clear timeout interrupt */
      MCLRMASK_D16( brdHdl->virtCtrlBase[mSlot], D203_LCTRL, D203_LCTRL_TOUT );
			
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
    status2 = D203_ClrMIface( brdHdl, mSlot );

    INT_ENABLE( brdHdl );	/* enable interrupt */

    /* return on error */ 
    if( status )
      return status;
    if( status2 )
      return status2;
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







/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: a12_int.h
 *
 *       Author: kp
 *        $Date: 2001/04/11 11:05:48 $
 *    $Revision: 1.1 $
 *
 *  Description: Internal header file for A12 BBIS driver
 *
 *     Switches: ---
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: a12_int.h,v $
 * Revision 1.1  2001/04/11 11:05:48  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _A12_H
#define _A12_H

#ifdef __cplusplus
    extern "C" {
#endif

/*--------------------------------------------------------------------------+
|    DEFINES                                                                |
+--------------------------------------------------------------------------*/
/* debug settings */
#define BRD_NBR_OF_BRDDEV		4		/* number of total devices		*/

#define A12_NBR_OF_MMODS		3 		/* number of M-module slots */

#define BBNAME "A12"

#define A12_MMOD_BRIDGE_DEV_NO	0x18 /* PCI device number on bus 0 */
#define A12_MMOD_BRIDGE_VEN_ID	0x1172 /* Altera */
#define A12_MMOD_BRIDGE_DEV_ID	0x410c /* A12 */


#define A12_MMOD_SLOT_OFFSET	0x02000000 /* addr off between M-mod slots */

/* offsets relative to each M-module slots base address */
#define A12_MMOD_A24_D32_BASE	0x00000000
#define A12_MMOD_A24_D16_BASE	0x01000000
#define A12_MMOD_A08_D32_BASE	0x01fffd00
#define A12_MMOD_A08_D16_BASE	0x01fffe00
#define A12_MMOD_IACK_BASE		0x01ffff00
#define A12_MMOD_CTRL_BASE		0x01ffff04

#define A12_CTRL_SIZE			0x2
/*---------------------------------------------------------------------------+
|    TYPEDEFS                                                                |
+---------------------------------------------------------------------------*/
/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/

/* struct for each M-module slot */
typedef struct {
	MACCESS		vCtrlBase;		/* control registers virtual base */
} MMOD;

typedef struct {
	MDIS_IDENT_FUNCT_TBL idFuncTbl;				/* id function table		*/
    u_int32     ownMemSize;						/* own memory size			*/
    OSS_HANDLE* osHdl;							/* os specific handle		*/
    DESC_HANDLE *descHdl;						/* descriptor handle pointer*/
    u_int32     debugLevel;						/* debug level for BBIS     */
	DBG_HANDLE  *debugHdl;						/* debug handle				*/
	void		*physBase;						/* phys base of regs (BAR0) */
	MMOD		mmod[A12_NBR_OF_MMODS];			/* m-module control */
    OSS_RESOURCES res[A12_NBR_OF_MMODS]; 		/* resources */
	int32		resourcesAssigned; 				/* flag resources assigned */
	int32		irqLevel;						/* bridge IRQ level */
	int32		irqVector;						/* bridge IRQ vector */
} BBIS_HANDLE;


/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
/* init/exit */
static int32 A12_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 A12_BrdInit(BBIS_HANDLE*);
static int32 A12_BrdExit(BBIS_HANDLE*);
static int32 A12_Exit(BBIS_HANDLE**);
/* info */
static int32 A12_BrdInfo(u_int32, ...);
static int32 A12_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 A12_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 A12_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  A12_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 A12_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 A12_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 A12_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 A12_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 A12_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 A12_SetStat(BBIS_HANDLE*, u_int32, int32, int32);
static int32 A12_GetStat(BBIS_HANDLE*, u_int32, int32, int32*);
/* unused */
static int32 A12_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);

#ifdef __cplusplus
    }
#endif

#endif /* _A12_H */

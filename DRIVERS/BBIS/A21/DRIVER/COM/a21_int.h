/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: a21_int.h
 *
 *       Author: kp
 *        $Date: 2001/04/11 11:05:48 $
 *    $Revision: 1.1 $
 *
 *  Description: Internal header file for A21 BBIS driver
 *
 *     Switches: ---
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: a21_int.h,v $
 * Revision 1.1  2001/04/11 11:05:48  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _A21_H
#define _A21_H

#ifdef __cplusplus
extern "C" {
#endif

  /*--------------------------------------------------------------------------+
    |    DEFINES                                                                |
    +--------------------------------------------------------------------------*/
  /* debug settings */
#define BRD_NBR_OF_BRDDEV		4		/* number of total devices		*/

#define A21_NBR_OF_MMODS		3 		/* number of M-module slots */

#define BBNAME "A21"

#define A21_MMOD_BRIDGE_DEV_NO	0x0 /* PCI device number on bus 0 */
#define A21_MMOD_BRIDGE_DOMAIN_NO	0x2 /* PCI device number on domain 2 */
#define A21_MMOD_BRIDGE_VEN_ID	0x1a88 /* MEN */
#define A21_MMOD_BRIDGE_DEV_ID	0x4d45 /* Chameleon FPGA */


#define A21_MMOD_A08_SLOT_OFFSET	0x00000400 /* addr off between M-mod slots */
#define A21_MMOD_A24_SLOT_OFFSET	0x01000000 /* addr off between M-mod slots */

  /* offsets relative to each M-module slots base address */
#define A21_MMOD_A24_BASE	0x01000000
#define A21_MMOD_A08_BASE	0x00001400
#define A21_MMOD_IACK_BASE		0x00001500
#define A21_MMOD_CTRL_BASE		0x00001701

#define A21_CTRL_SIZE			0x2
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
    MDIS_IDENT_FUNCT_TBL idFuncTbl;		/* id function table		*/
    u_int32       ownMemSize;			/* own memory size		*/
    OSS_HANDLE*   osHdl;				/* os specific handle		*/
    DESC_HANDLE   *descHdl;			/* descriptor handle pointer    */
    u_int32       debugLevel;			/* debug level for BBIS         */
    DBG_HANDLE    *debugHdl;			/* debug handle			*/
    void	  *physBase;			/* phys base of regs (BAR0)     */
    MMOD	  mmod[A21_NBR_OF_MMODS];	/* m-module control */
    OSS_RESOURCES res[A21_NBR_OF_MMODS]; 	/* resources */
    int32	resourcesAssigned; 	/* flag resources assigned */
    int32	irqLevel;		/* bridge IRQ level */
    int32	irqVector;		/* bridge IRQ vector */
  } BBIS_HANDLE;


  /*---------------------------------------------------------------------------+
    |    PROTOTYPES                                                              |
    +---------------------------------------------------------------------------*/
  /* init/exit */
  static int32 A21_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
  static int32 A21_BrdInit(BBIS_HANDLE*);
  static int32 A21_BrdExit(BBIS_HANDLE*);
  static int32 A21_Exit(BBIS_HANDLE**);
  /* info */
  static int32 A21_BrdInfo(u_int32, ...);
  static int32 A21_CfgInfo(BBIS_HANDLE*, u_int32, ...);
  /* interrupt handling */
  static int32 A21_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
  static int32 A21_IrqSrvInit(BBIS_HANDLE*, u_int32);
  static void  A21_IrqSrvExit(BBIS_HANDLE*, u_int32);
  /* exception handling */
  static int32 A21_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
  static int32 A21_ExpSrv(BBIS_HANDLE*,u_int32);
  /* get module address */
  static int32 A21_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
  static int32 A21_ClrMIface(BBIS_HANDLE*,u_int32);
  static int32 A21_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
  /* getstat/setstat */
  static int32 A21_SetStat(BBIS_HANDLE*, u_int32, int32, int32);
  static int32 A21_GetStat(BBIS_HANDLE*, u_int32, int32, int32*);
  /* unused */
  static int32 A21_Unused(void);
  /* miscellaneous */
  static char* Ident( void );
  static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);

#ifdef __cplusplus
}
#endif

#endif /* _A21_H */

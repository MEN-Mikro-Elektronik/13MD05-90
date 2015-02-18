/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  sysparam2.h
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2012/03/16 18:56:42 $
 *    $Revision: 2.44 $
 *
 *  	 \brief  Header file for MEN's sysparam library (second try!)
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: sysparam2.h,v $
 * Revision 2.44  2012/03/16 18:56:42  ts
 * R: compiler warning at EP07 Menmon build: EP05_SysParamInit unknown
 * M: added function declaration to header file
 *
 * Revision 2.43  2010/08/17 11:20:28  rlange
 * R: Support for MPC83XX CPU
 * M: Added MPC83XX clock defines
 *
 * Revision 2.42  2010/07/05 11:26:13  rlange
 * R: Added EP06 Flash Support
 * M: Added EP06 Init routine declaration
 *
 * Revision 2.41  2010/01/15 15:03:22  RLange
 * R: Implementated password handling
 * M: Added enum definition for SYSPARAM_ACC_ENCRYPT
 *
 * Revision 2.40  2009/12/04 13:48:44  RLange
 * R: SC20 Flash Support
 * M: Add SC20 Flash Init declaration
 *
 * Revision 2.39  2009/09/23 09:01:23  ufranke
 * R: improve boot speed
 * M: added SYSP_normalboot
 * R: no common ee-nobanner
 * M: added SYSP_nobanner
 *
 * Revision 2.38  2009/07/28 14:21:33  sy
 * add declaration of D602SysParamInit()
 *
 * Revision 2.37  2008/06/27 15:06:01  ufranke
 * R: MIPIOS needs more random seed
 * M: added SYSP_rand
 *
 * Revision 2.36  2008/06/27 12:06:08  aw
 * R: Sysparam support was needed for F302
 * M: added F302_SysParamInit
 *
 * Revision 2.35  2008/06/11 17:36:54  rt
 * R:1. Print a note that user has to reset board after changing an ee-X
 *      parameter if needed.
 * M:1. Add SYSPARAM_ACC_NEEDRESET parameter to SYSPARAM_ACC_ATTR
 *
 * Revision 2.34  2008/05/29 11:12:09  rt
 * added:
 * - SYSP_rststat_cb
 *
 * Revision 2.33  2008/04/11 13:43:38  rt
 * added:
 * -SYSP_nspeed3
 * -SYSP_nmac3
 *
 * Revision 2.32  2008/02/04 14:44:34  ufranke
 * added
 *  + fpga
 *
 * Revision 2.31  2008/01/09 13:51:11  rt
 * added:
 * SYSP_BuspciName()
 *
 * Revision 2.30  2008/01/09 11:06:53  rt
 * added:
 * -SYSP_buspci...
 *
 * Revision 2.29  2007/09/21 13:44:45  rt
 * added:
 * -SYSP_stdis_usb
 * -SYSP_stdis_ether
 *
 * Revision 2.28  2007/09/20 17:56:33  rt
 * added:
 * - SYSP_fram0
 *
 * Revision 2.27  2007/09/12 14:59:42  rt
 * added:
 * - SYSP_usbdp[]
 *
 * Revision 2.26  2007/07/11 10:18:31  rt
 * added:
 * - SYSP_comModeValue (for RS232, RS422, RS485 mode switch)
 *
 * Revision 2.25  2007/06/08 11:25:54  aw
 * + EM09_SysParamInit
 *
 * Revision 2.24  2007/03/22 11:12:33  ufranke
 * added
 *  + SYSP_stdis_nand
 *
 * Revision 2.23  2007/02/27 20:53:31  rt
 * added:
 * - Ep04SysParamInit
 *
 * Revision 2.22  2006/12/22 10:24:32  rla
 * Added VC01 CPU Board
 *
 * Revision 2.21  2006/10/13 19:48:59  cs
 * added:
 *   + SYSP_gstatshow parameter
 *
 * Revision 2.20  2006/08/18 08:11:47  rla
 * Added Ep01
 *
 * Revision 2.19  2006/08/11 17:41:58  rt
 * + Sc15aSysParamInit
 *
 * Revision 2.18  2006/05/17 13:07:31  rt
 * added
 *  + SysParamUnsignedIntArray
 *  + P504SysParamInit
 *  + F6SysParamInit
 *
 * Revision 2.17  2006/01/05 14:01:18  cs
 * added parameter names esm_cb_partnum and esm_cb_hwflag
 *
 * Revision 2.16  2005/12/22 16:16:50  ag
 * added A12SysParamInit
 *
 * Revision 2.15  2005/06/23 15:54:48  kp
 * Copyright string changed (sbo)
 *
 * Revision 2.14  2005/04/19 10:01:08  kp
 * + PQ3 support
 *
 * Revision 2.13  2005/04/12 16:53:16  kp
 * made SYSP_TusParamGet() available outside SYSPARAM (vxWorks replacement
 * for MMPARASTRING)
 *
 * Revision 2.12  2005/03/09 16:48:31  ufranke
 * added
 *  + SYSP_stdis_wdog
 *
 * Revision 2.11  2005/02/15 12:20:10  ufranke
 * added
 *  + SYSP_NSPEED_VALUES_IDX
 *
 * Revision 2.10  2005/02/08 13:07:27  ufranke
 * added
 *  + KM01/EM01 SysparamInit prototypes
 *
 * Revision 2.9  2005/02/02 16:03:20  kp
 * increase parameter name length from 10 to 16
 * added more protos
 *
 * Revision 2.8  2005/01/18 15:49:41  kp
 * + functions for raw NVS access
 *
 * Revision 2.7  2005/01/14 09:36:24  kp
 * added new protos
 *
 * Revision 2.6  2005/01/12 14:20:04  kp
 * added SysParamHelp. Exported lots of external funcs for MENMON
 *
 * Revision 2.5  2004/12/23 12:47:32  ufranke
 * added
 *  + tags
 *  + PP01_SysParamInit()
 *
 * Revision 2.4  2004/12/20 08:37:43  kp
 * added protos for more API extension funcs.
 * Added tag name protos
 *
 * Revision 2.3  2004/11/29 10:47:13  kp
 * added SysparamGetDefault
 *
 * Revision 2.2  2004/11/04 17:22:01  kp
 * added DracoSysParamInit
 *
 * Revision 2.1  2004/01/08 09:07:00  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003-2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _SYSPARAM2_H
#define _SYSPARAM2_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES /CONST                      |
+--------------------------------------*/
/** a pointer to a const string */
typedef const char *SYSP_CONST_STRING;


/* error codes */
#define SYSPARAM_NOT_INIT 			1	/**< xxxSysParamInit not called */
#define SYSPARAM_INVAL_PARAM		2	/**< invalid/unknown parameter name */
#define SYSPARAM_INVAL_VALUE		3	/**< invalid value */
#define SYSPARAM_EEPROM_ERROR		4	/**< error reading/writing EEPROM */
#define SYSPARAM_READ_ONLY			5	/**< parameter is read only */
#define SYSPARAM_NO_RESOURCE		6 	/**< out of resources */
#define SYSPARAM_VAL_TRUNCATED		7 	/**< value has been truncated */
#define SYSPARAM_NOT_SUPPORTED		8	/**< operation not supported */

#define SYSP_PREFIX_LEN    4   /**< max length of NVS prefix incl. term. 0  */
#define SYSP_PARNAME_LEN   16  /**< max length of parameter name excl. pfx  */
#define SYSP_NVSNAME_LEN   10  /**< max length of NVS section incl. term 0  */

/** attributes for SysParamSetEx (can be binary ORed) */
typedef enum {
	SYSPARAM_SA_NON_PERSISTANT=0x1,	/**< don't immediately write to NVS  */
	SYSPARAM_SA_FROM_FIRMWARE=0x2	/**< attempt to set from firmware  */
} SYSPARAM_SET_ATTR;

/** parameter access attributes
 *
 * Returned in the #SYSPARAM_PARAM_INFO structure by SysParamEnum()
 *
 * (some flags can be binary ORed)
 */
typedef enum {
	SYSPARAM_ACC_RO=1,		/**< read-only for user, writeable from firmware*/
	SYSPARAM_ACC_RW=2,		/**< read/write */
	SYSPARAM_ACC_PW=3,		/**< read-only, writeable with password */
	SYSPARAM_ACC_RW_MASK=3,	/**< mask for above flags */
	SYSPARAM_ACC_ENCRYPT=0x10,   /**< parameter shall be displayed encrypted */
	SYSPARAM_ACC_NEEDRESET=0x20, /**< parameter needs reset to take effect */
	SYSPARAM_ACC_MMPARASTR=0x40, /**< parameter is part of MENMON
									parameter string  */
	SYSPARAM_ACC_PERSISTANT=0x80 /**< parameter can be saved non-volatile  */
} SYSPARAM_ACC_ATTR;

/** fake SMB bus number  for SYSPARAM_MMSMB_INIT.eeXXX functions: use flash */
#define SYSPARAM_SMB_BUS_FLASH		0xff

/* common parameter names, see sysp_tags.c */
extern const char
   SYSP_mem0[],
	SYSP_mem1[],
	SYSP_flash0[],
	SYSP_flash1[],
	SYSP_fram0[],
	SYSP_cpu[],
	SYSP_cpuclkhz[],
	SYSP_memclkhz[],
	SYSP_pciclkhz[],
   SYSP_MPC52XX_inclkhz[],			/* defined in sysp_tags_mpc52xx.c */
   SYSP_MPC52XX_xlbclkhz[],		/* defined in sysp_tags_mpc52xx.c */
   SYSP_MPC52XX_ipbclkhz[],		/* defined in sysp_tags_mpc52xx.c */
	SYSP_MPC85XX_ccbclkhz[],		/* defined in sysp_tags_mpc85xx.c */
   SYSP_MPC85XX_brgclkhz[],
   SYSP_MPC85XX_immr[],
   SYSP_MPC83XX_inclkhz[],			/* defined in sysp_tags_mpc83xx.c */
   SYSP_MPC83XX_csbclkhz[],		/* defined in sysp_tags_mpc83xx.c */
   SYSP_MPC83XX_ceclkhz[],		/* defined in sysp_tags_mpc83xx.c */
   SYSP_clun[],
	SYSP_dlun[],
	SYSP_rststat[],
	SYSP_rststat_pwon[],
	SYSP_rststat_pdrop[],
	SYSP_rststat_temp[],
	SYSP_rststat_sw[],
	SYSP_rststat_wdog[],
	SYSP_rststat_rbut[],
	SYSP_rststat_hrst[],
	SYSP_rststat_srst[],
	SYSP_rststat_cb[],
	SYSP_rststat_unknown[],
	SYSP_mmst[],
	SYSP_mm[],
	SYSP_mm_pmm[],
	SYSP_mm_smm[],
	SYSP_cons[],       /* for backward compatibility -
						  use SYSP_conX in new projects */
	SYSP_brd[],
	SYSP_brdmod[],
	SYSP_brdrev[],
	SYSP_sernbr[],
	SYSP_prodat[],
	SYSP_repdat[],
	SYSP_con0[],
	SYSP_con1[],
	SYSP_con2[],
	SYSP_con3[],
	SYSP_gcon[],
	SYSP_gstatshow[],
	SYSP_vmode[],
	SYSP_cbr[],
	SYSP_baud[],
	SYSP_bsadr[],
	SYSP_bs[],
	SYSP_ecl[],
	SYSP_tto[],
	SYSP_tries[],
	SYSP_tdp[],
	SYSP_hdp[],
	SYSP_wdt[],
	SYSP_stignfault[],
	SYSP_stdis[],
	SYSP_stdis_wdog[],
	SYSP_stdis_nand[],
	SYSP_stdis_usb[],
	SYSP_stdis_ether[],
	SYSP_stwait[],
	SYSP_ldlogodis[],
	SYSP_updcdis[],
	SYSP_mmstartup[],
	/* u0..15 missing */
	SYSP_startup[],
	SYSP_kerpar[],
	SYSP_useflpar[],
	SYSP_bf[],
	SYSP_bootfile[],
	SYSP_netip[],
	SYSP_nethost[],
	SYSP_netgw[],
	SYSP_netname[],
	SYSP_netaddr[],
	SYSP_netsm[],
	SYSP_nspeed0[],
	SYSP_nspeed1[],
	SYSP_nspeed2[],
	SYSP_nspeed3[],
	SYSP_nmac0[],
	SYSP_nmac1[],
	SYSP_nmac2[],
	SYSP_nmac3[],
	SYSP_esm_cb_partnum[],             /* defined in sysp_tags_esm_cb.c */
	SYSP_esm_cb_hwflag[],               /* defined in sysp_tags_esm_cb.c */
	SYSP_usbdp[],
	SYSP_fpga[],
	SYSP_buspci0[],	
	SYSP_buspci1[],	
	SYSP_buspci2[],
	SYSP_rand[],
	SYSP_nobanner[],
	SYSP_normalboot[]
	;

extern const char *SYSP_nspeedValue[];

/** index in SYSP_nspeedValue match SYSP_nspeedValue[] entries */
typedef const enum
{
	SYSP_NSPEED_AUTO = 0,  				/**< auto */
	SYSP_NSPEED_10HD,   				/**< 10MBit/s half duplex */
	SYSP_NSPEED_10FD,  					/**< 10MBit/s full duplex */
	SYSP_NSPEED_100HD,   				/**< 100MBit/s half duplex */
	SYSP_NSPEED_100FD,  				/**< 100MBit/s full duplex */
	SYSP_NSPEED_1000  					/**< 1000MBit/s */
} SYSP_NSPEED_VALUES_IDX;

extern const char *SYSP_comModeValue[];

/** index in SYSP_nspeedValue match SYSP_comModeValue[] entries */
typedef const enum
{
	SYSP_COMMODE_RS422 = 0,  	/**< RS422 (has uncritical voltage level) */
	SYSP_COMMODE_RS232,   		/**< RS232 mode */
	SYSP_COMMODE_RS485			/**< RS485 mode */
} SYSP_COMMODE_VALUES_IDX;


/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/** Parameter info structure
 *
 * To be used with SysParamEnum()
 */
typedef struct {
	/** parameter name including prefix */
	char parName[SYSP_PREFIX_LEN+SYSP_PARNAME_LEN];
	SYSPARAM_ACC_ATTR attr;		/**< parameter access attributes  */
} SYSPARAM_PARAM_INFO;

/** NVS info structure
 *
 * To be used with SYSP_NvsEnum()
 */
typedef struct {
	char prefix[SYSP_PREFIX_LEN]; /**< NVS prefix */
	char name[SYSP_NVSNAME_LEN];  /**< NVS section name */
	u_int32 nvsSize;			  /**< size of section in bytes */
} SYSPARAM_NVS_INFO;


/** Parameter help structure
 *
 * Filled by SysParamHelp()
 *
 *
 */
typedef struct {
	char shortDesc[40];			/**< short description of parameter */

    /** First line (most important part) of longer description
		79 chars max, can be NULL */
	const char *longDescFirstLine;

	/** continued longer description, may consist of multiple lines, each
		line 79 chars max, can be NULL */
	const char *longDescContinuation;

	/** allowed characters for live editing (setupmenu),
		argument to strmatch(), if NULL, all chars allowed */
	const char *allowedChars;

	/** if multiple choice input, the list of choices
	    if not multiple choice, choice[0] is NULL.
		List of choices always NULL terminated
	*/
	const char *choice[10];

} SYSPARAM_PARAM_HELP;

/** structure passed to xxxSysParamInit for MENMON/SMB implementations
 */
typedef struct {
	void *osh;					/**< passed unmodified to eeRead/eeWrite  */

	/** function callback to read eeproms,
	 *
	 * Should read \a bufLen bytes from \a offset of the SMB eeprom
	 * at \a smbBus and \a smbAdr into \a buf.
	 * The data block may cross a 256 byte boundary.
	 *
	 * This function is "missused" also for NVS sections in (boot)
	 * flash. If \a smbBus is #SYSPARAM_SMB_BUS_FLASH, then the NVS
	 * section is in flash. In this case
	 * \a smbAdr is interpreted as a sector identifier, e.g. 0xff for last
	 * sector in flash. \a offset is measured from the beginning of
	 * addresses flash sector.
	 *
	 * \return 0 if ok, a value != 0 on error
	 */
	int32 (*eeRead)(void* osh, int smbBus, int smbAdr, int offset,
					u_int8 *buf, int bufLen );

	/** function callback to write eeproms,
	 *
	 * Should write \a bufLen bytes to \a offset of the SMB eeprom
	 * at \a smbBus and \a smbAdr from \a buf.
	 * The data block may cross a 256 byte boundary.
	 * The function should perform a read-after-write.
	 *
	 * Also used for flash, see note in eeRead().
	 *
	 * \return 0 if ok, a value != 0 on error
	 */
	int32 (*eeWrite)(void* osh, int smbBus, int smbAdr, int offset,
					u_int8 *buf, int bufLen );

    /** MENMON parameter string address
	 *
     * The content of this string is used to initialize the parameters
	 * in the \c mmpara parameter group (cpu=, pciclkhz=...).
	 * It should be therefore  either be
	 * - initialized with parameters or
	 * - an empty string and the parameters updated later by firmware
	 *
	 * This string is then filled by SYSPARAM with most of the parameters
	 * and whenever a SysParamSet() or SysParamSetDefaults() is issued.
	 *
	 */
	char *mmParaStringAdrs;
	int mmParaStringMaxLen;		/**< space in MENMON parameter string */

    /** VxWorks bootline string address
	 *
	 * This string is the immediate buffer for the parameters in the
	 * \c vxbline parameter group.
	 *
	 * When it is an empty string when the xxxSysParamInit() routine
	 * is called, SYSPARAM reads the string from non volatile storage.
	 *
	 * If it is a non-empty string, the string is basically validated.
	 *
	 * Whenever either the NV data or the passed string appears invalid,
	 * a default bootline is setup (CPU dependant).
	 *
	 * This string shall have space for at least 480 bytes.
	 *
	 * \a vxBlineStringAdrs will receive immediately any modification
	 * made through SysParamSet() or SysParamSetDefaults().
	 *
	 * The string can be also modified outside the SYSPARAM library.
	 * The SysParamGet() will fetch the values always directly from
	 * this string.
	 */
	char *vxBlineStringAdrs;	/**< VxWorks bootline string address */

} SYSPARAM_MMSMB_INIT;

/** callback type to post process parameter changes */
typedef	int (*SYSP_POST_PROCESS_CB)( const char *parName, int attr,
									 int errCode);

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

int SysParamGetAll( char *buf, int bufLen, int alias );
int SysParamGet( const char *parName, char *parVal, int parValMaxLen );
int SysParamGetDefault( const char *parName, char *parVal, int parValMaxLen );
int SysParamSet( const char *parName, const char *parVal, const char *passwd);
int SysParamSetEx( const char *parName, const char *parVal,
				 const char *passwd, int attr );
int SysParamSetDefaults( void );
int SysParamMakePersistant( const char *parName );
int SysParamEnum( int idx, int alias, SYSPARAM_PARAM_INFO *info);
int SysParamNumParams( int *numP );

const char *SysParamErrStr( int errCode );
int SysParamSetFmt(
	const char *parName,
	const char *passwd,
	int attr,
	const char *fmt, ... );
int SysParamSignedInt( const char *par, int base, int32 def, int32 *valueP );
int SysParamUnsignedInt( const char *par, int base,
						 u_int32 def, u_int32 *valueP );
int SysParamUnsignedIntArray( const char *par, int base, int no,
	                          u_int32* defP, u_int32* valueP);
int SysParamDefUnsignedInt( const char *par, int base,
							u_int32 def, u_int32 *valueP );
int SysParamDefSignedInt( const char *par, int base,
						  int32 def, int32 *valueP );
int SysParamBool( const char *par, int def);

int SYSP_MenBrdNameWithModel( char prefix, char *buf );
int SYSP_MenBrdProdString( char prefix, char *buf );

int SysParamHelp(
	const char *parName,
	SYSPARAM_PARAM_HELP *parHelp);

char *SYSP_NmacName( char *buf, int ifNum );
char *SYSP_NspeedName( char *buf, int ifNum );
char *SYSP_BuspciName( char *buf, int ifNum );
char *SYSP_PrefixParamName( char prefix, const char *parName, char *buf );
int SYSP_PostProcessingHookInstall( SYSP_POST_PROCESS_CB cb );
int SYSP_NvsEnum( int idx, SYSPARAM_NVS_INFO *info );
int SYSP_NvsReadRaw(
	SYSP_CONST_STRING nvsName,
	int offset,
	int size,
	u_int8 *data);
int SYSP_NvsWriteRaw(
	SYSP_CONST_STRING nvsName,
	int offset,
	int size,
	const u_int8 *data);

/*--- Init routines for the individual CPUs ---*/
extern int Sc15aSysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int P504SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int F6SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int A12SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int Em04SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int Em04ASysParamInit( const SYSPARAM_MMSMB_INIT *initBlk );
extern int EM03_SysParamInit( const SYSPARAM_MMSMB_INIT *initBlk );
extern int EM09_SysParamInit( const SYSPARAM_MMSMB_INIT *initBlk );
extern int PP01_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int KM01_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int EM01_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int EP01_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int EP05_SysParamInit( const SYSPARAM_MMSMB_INIT *initBlk );
extern int EP06_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int VC01_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int Ep04SysParamInit( const SYSPARAM_MMSMB_INIT *initblk  );
extern int F302_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );
extern int D602SysParamInit( const SYSPARAM_MMSMB_INIT *initblk  );
extern int SC20_SysParamInit( const SYSPARAM_MMSMB_INIT *initblk );

/* only available inside MENMON, VxWorks */
extern void SysParamVxBootParamsPrompt(void);
extern int SYSP_MmNetIfParamsScan( void );
extern int SYSP_FromRawEth( char *dest, int destLen, void *src );
extern int SYSP_ToRawEth( void *dest, const char *src );
extern int SYSP_StringArrFindStr( const SYSP_CONST_STRING *arr,
						   SYSP_CONST_STRING str);
extern int SYSP_StringArrayLen( const SYSP_CONST_STRING *arr );
extern int SYSP_StringArrCopy(
	SYSP_CONST_STRING dst[],
	const SYSP_CONST_STRING src[],
	int max );
extern int SYSP_StrSafeCpy( char *dst, const char *src, int destLen );
extern void SYSP_AllocInfo( int *availP, int *minAvailP );
extern int SYSP_TusParamGet(
	const char *tus,
	int tusMaxLen,
	const char *parName,
	char *parVal,
	int parValMaxLen );

#ifdef __cplusplus
	}
#endif

#endif	/* _SYSPARAM2_H */







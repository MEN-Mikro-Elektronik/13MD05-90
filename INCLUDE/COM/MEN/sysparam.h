/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: sysparam.h
 *
 *       Author: kp
 *        $Date: 2004/02/26 16:25:43 $
 *    $Revision: 2.6 $
 *
 *  Description: Header file for MEN's sysparam library
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysparam.h,v $
 * Revision 2.6  2004/02/26 16:25:43  UFranke
 * added
 *  - power and temp fail reset causes
 *
 * Revision 2.5  2003/05/27 13:46:13  UFranke
 * added
 *   - export interface for common tags
 *
 * Revision 2.4  2003/04/29 17:20:06  UFranke
 * added
 *   - PP01 support
 *
 * Revision 2.3  2003/04/24 12:30:31  UFranke
 * removed
 *   - include of MEN/oss.h in this header
 * added
 *   - SysParamInit() prototype - first use in PP01 MENMON
 *
 * Revision 2.2  2002/09/30 13:43:32  ub
 * MM_PARASTR_SIZE now 1024
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _SYSPARAM_H
#define _SYSPARAM_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES /CONST                      |
+--------------------------------------*/

/* error codes */
#define SYSPARAM_NOT_INIT 			1	/* xxxSysParamInit not called */
#define SYSPARAM_INVAL_PARAM		2	/* invalid/unknown parameter name */
#define SYSPARAM_INVAL_VALUE		3	/* invalid value */
#define SYSPARAM_EEPROM_ERROR		4	/* error reading/writing EEPROM */
#define SYSPARAM_READ_ONLY			5	/* parameter is read only */


/* misc */
#define MM_PARASTR_SIZE				1024	/* size of MENMON parastring */


/* common sysparam tags - see SYSPARAM/COM/sysparam_tags.c */
extern const char *SYSPARAM_RST;
extern const char *SYSPARAM_CONSOLE;
extern const char *SYSPARAM_MEM_0;
extern const char *SYSPARAM_MEM_1;
extern const char *SYSPARAM_FLASH_0;
extern const char *SYSPARAM_FLASH_1;
extern const char *SYSPARAM_CPU;
extern const char *SYSPARAM_CPU_CLK;
extern const char *SYSPARAM_MEM_CLK;
extern const char *SYSPARAM_PCI_CLK;
extern const char *SYSPARAM_MENMON;

/* common sysparam entries - see SYSPARAM/COM/sysparam_tags.c */
extern const char *SYSPARAM_RST_SOFTWARE;
extern const char *SYSPARAM_RST_PUSH_BUTTON;
extern const char *SYSPARAM_RST_WDOG;
extern const char *SYSPARAM_RST_HRST;
extern const char *SYSPARAM_RST_SRST;
extern const char *SYSPARAM_RST_POWER_ON;
extern const char *SYSPARAM_RST_POWER_FAIL;
extern const char *SYSPARAM_RST_TEMP_FAIL;
extern const char *SYSPARAM_RST_UNKOWN;

extern const char *SYSPARAM_CONSOLE_COM1;
extern const char *SYSPARAM_CONSOLE_COM2;
extern const char *SYSPARAM_CONSOLE_VGA_KEY;

extern const char *SYSPARAM_MENMON_PMM;
extern const char *SYSPARAM_MENMON_SMM;


/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*
 * structure to be passed to the xxxSysParamInit function
 *
 */
typedef struct {
	OSS_HANDLE *osh;
								/* function callbacks to read/write eeproms */
	int32 (*eeRead)(OSS_HANDLE* osh, int smbBus, int smbAdr, int offset,
					u_int8 *buf, int bufLen );
	int32 (*eeWrite)(OSS_HANDLE* osh, int smbBus, int smbAdr, int offset,
					u_int8 *buf, int bufLen );
	char *mmParaStringAdrs;		/* MENMON parameter string address */
	char *vxBlineStringAdrs;	/* VxWorks bootline string address */

} SYSPARAM_INIT;


/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
int Em04SysParamInit( const SYSPARAM_INIT *initblk, int8 vxBlineFromEeprom );
int SysParamInit   /* CPU specific initialization - equal to Em04SysParamInit() */
(
	const SYSPARAM_INIT *desc,
	int8 vxBlineFromEeprom
);
int PP01SysParam_WrapperInit
(
	const SYSPARAM_INIT *desc,
	int8  vxBlineFromEeprom,
	char  board
);

int SysParamGetAll( char *buf, int bufLen );
int SysParamGet( const char *param, char *buf, int bufLen );
int SysParamSet( const char *param, const char *buf, const char *passwd );
int SysParamSetDefaults( void );
int SysParamSaveVxBootline( void );

/* none */


#ifdef __cplusplus
	}
#endif

#endif	/* _SYSPARAM_H */


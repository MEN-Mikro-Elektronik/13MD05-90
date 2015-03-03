/***********************  I n c l u d e  -  F i l e  ***********************/
/**
 *         \file chameleon.h
 *
 *       \author kp and others
 *
 *        \brief Header file for chameleon (flexible FPGA) library
 *
 *     Switches: CHAMELEON_CODES_ONLY -
 *               if set, don't include typedefs and prototypes
 */
/*-------------------------------[ History ]---------------------------------
 *
 * ts: last CVS log message so the origin of the header file is known...
 *
 * $Log: chameleon.h,v $
 * Revision 3.35  2014/02/19 12:41:26  ts
 * R: minRevision within other CHAMELEONV2_TABLE members broke table parsing
 *    (IC filename) in 10EM10A16 VxWorks when used with prebuilt MDIS object.
 * M: moved minRevision at end of struct, replaced reserved member with it
 *---------------------------------------------------------------------------
 * (c) Copyright 2015 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _CHAMELEON_H
#define _CHAMELEON_H

#ifdef __cplusplus
	extern "C" {
#endif
/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* official module codes */
#define CHAMELEON_16Z000_SYSTEM		0x00
#define CHAMELEON_16Z032_DISP		0x01
#define CHAMELEON_16Z033_DISP		0x02
#define CHAMELEON_16Z031_SPI		0x03
#define CHAMELEON_16Z023_IDE		0x04
#define CHAMELEON_16Z030_UARTHLP	0x05
#define CHAMELEON_16Z024_SRAM		0x06
#define CHAMELEON_16Z025_UART		0x07
#define CHAMELEON_16Z029_CAN		0x08
#define CHAMELEON_16Z022_GPIO		0x09
#define CHAMELEON_16Z035_SYSTEM		0x0a
#define CHAMELEON_PCI_INT		0x0b
#define CHAMELEON_CANGPIO		0x0c
#define CHAMELEON_16Z016_IDE		0x10 	/* IDE UDMA */
#define CHAMELEON_16Z063_NAND_RAW	0x18 	/* raw NAND flash controller */
#define CHAMELEON_16Z034_GPIO		0x19
#define CHAMELEON_16Z001_SMB		0x1b 	/* SMB */
#define CHAMELEON_16Z048_ISA		0x1c 	/* ISA bridge */
#define CHAMELEON_16Z049_TMR		0x1d 	/* 8254 timer */
#define CHAMELEON_16Z050_BIO		0x1e 	/* binary I/O */
#define CHAMELEON_16Z045_FLASH		0x1f 	/* flash I/F */
#define CHAMELEON_16Z052_GIRQ		0x20 	/* global interrupt controller */
#define CHAMELEON_16Z043_SDRAM		0x21 	/* SDRAM */
#define CHAMELEON_16Z053_IDE		0x22 	/* standard IDE controller - may NIOS + NAND */
#define CHAMELEON_16Z054_SYSTEM		0x23 	/* system registers - EM03/EM07 */
#define CHAMELEON_16Z055_HDLC		0x24 	/* HDLC */
#define CHAMELEON_16Z044_DISP	    	0x25    /* Display Controller */
#define CHAMELEON_16Z059_IRQ	  	0x26    /* Interrupt controller */
#define CHAMELEON_16Z051_DAC		0x27 	/* Analog output */
#define CHAMELEON_16Z023_IDENHS		0x29 	/* IDE without hot swap */
#define CHAMELEON_16Z061_PWM		0x2a 	/* PWM unit */
#define CHAMELEON_16Z062_DMA		0x2b 	/* general DMA */
#define CHAMELEON_16Z064_ZOOM		0x2d 	/* Zoom logic 640x480 -> 800x600 */
#define CHAMELEON_16Z065_WBAV		0x2e 	/* wishbone->avalon bridge */
#define CHAMELEON_16Z066_MMOD		0x2f 	/* m-module */
#define CHAMELEON_16Z067_VME		0x30 	/* vme */
#define CHAMELEON_16Z068_IDE		0x31 	/* ide standard */
#define CHAMELEON_16Z069_RST		0x32 	/* rst */
#define CHAMELEON_16Z072_OWB		0x36 	/* One Wire Bus */
#define CHAMELEON_16Z073_QDEC		0x33 	/* Quadrature Decoder */
#define CHAMELEON_16Z074_REGSPI		0x37	/* Analog I/O */
#define CHAMELEON_16Z075_SPEED		0x35 	/* Frequency Counter */
#define CHAMELEON_16Z076_QSPI		0x34 	/* Quad SPI */
#define CHAMELEON_16Z077_ETH		0x38 	/* Ethernet Controller */
#define CHAMELEON_16Z079_ANYBUS		0x39	/* Anybus Module */
#define CHAMELEON_16z082_IMPULSE	0x3a	/* Impulse counter Unit */
#define CHAMELEON_CUSTOMER_MODULE	0x3f	/* Customer specific unit */
#define CHAMELEON_16Z080_ARCNET		0x40	/* Arcnet Core */
#define CHAMELEON_16Z081_FIP		0x41	/* FIP Core */
#define CHAMELEON_16Z125_UART		0x42	/* High Speed Uart */
#define CHAMELEON_16Z056_SPI		0x43	/* programable SPI controller */
#define CHAMELEON_16Z084_IDEEPROM   	0x44    /* ID EEPROM emulation for M199 */
#define CHAMELEON_16Z085_MS09N      	0x45    /* MS09N interrupt logic*/
#define CHAMELEON_16Z087_ETH	    	0x46    /* New MEN Ethernet Core */
#define CHAMELEON_16Z057_UART	    	0x47    /* modified Z025 (IRQ handling) */
#define CHAMELEON_16Z089_HSSL	    	0x48    /* HSSL config unit */
#define CHAMELEON_16Z090_DPRAM	    	0x49    /* 70V27 DPRAM unit */
#define CHAMELEON_16Z094_WB28XX	    	0x5e    /* WB to 8xx Bus Bridge */
#define CHAMELEON_16Z126_SERFLASH   	0x7e    /* Serial Flash interface */

/* old module codes: DONT USE THESE DEFINES IN NEW PROJECTS
ts: old module codes were removed 18.02.2015. If there is any code out there still using
these let compile fail gracefully so D.veloper is forced to update his defines. Ha! */

/* pci FPGA defs */
#define CHAMELEON_PCI_VENID_ALTERA	0x1172 /* Altera PCI Vendor ID */
#define CHAMELEON_PCI_VENID_MEN		0x1A88 /* MEN PCI Vendor ID    */

#define CHAMELEON_PCI_DEV_ID		0x4d45 /* standard chameleon device id */

#define EM04_FPGA_PCI_DEV_ID		0x5104
#define EM04_FPGA_PCI_DEV_NO		0x1D
#define EM04_FPGA_PCI_BUS		0
#define EM04_FPGA_PCI_FUNC		0
#define EM04_FPGA_TYPE 			"EM04"

#define A015_PCI2VME_FPGA_IC		56
#define A015_PCI2VME_FPGA_TYPE		"A015-IC56"

#define A015_PCI2MMOD_FPGA_IC		35
#define A015_PCI2MMOD_FPGA_TYPE		"A015-IC35"

/* error codes */
#define CHAMELEON_LOAD_OK		0
#define CHAMELEON_OK			0
#define CHAMELEON_CODE_ABSENT		0xB0
#define CHAMELEON_CODE_MISMATCH		0xB1
#define CHAMELEON_LOAD_ERR		0xB2
#define CHAMELEON_VERIFY_ERR		0xB3
#define CHAMELEON_INIT_DONE_ERR		0xB4
#define CHAMELEON_CONF_STAT_ERR		0xB5
#define CHAMELEON_CODE_CHKSUM_ERR	0xB6
#define CHAMELEON_DEV_NOT_FOUND		0xB7
#define CHAMELEON_PCI_ERR		0xB7
#define CHAMELEON_OUT_OF_RESOURCE	0xBA
#define CHAMELEON_OUT_OF_RANGE		0xBB
#define CHAMELEON_UNIT_NOT_FOUND	0xBC

/**
 * @defgroup _CHAMV2_ERRCODES Chameleon-V2 Error Codes
 * Returns an error state (0xC0..0xDF) of Chameleon-V2 functions.
 * @{ */
#define CHAMELEONV2_PCIDEV_ILL		0xC0	/*!< pci device illegal */
#define CHAMELEONV2_PCIDEV_ERR		0xC1	/*!< pci device error */
#define CHAMELEONV2_TABLE_NOT_FOUND	0xC2	/*!< chameleon table not found */
#define CHAMELEONV2_TABLE_REV_UNSUP	0xC3	/*!< chameleon table revision unsupported */
#define CHAMELEONV2_TABLE_ERR		0xC4	/*!< chameleon table erroneous */
#define CHAMELEONV2_DTYPE_UNSUP		0xC5	/*!< unknown descriptor type found */
#define CHAMELEONV2_BAR_UNASSIGNED	0xC6	/*!< bar unassigned */
#define CHAMELEONV2_BARDESC_MISSING	0xC7	/*!< bar descriptor missing */
#define CHAMELEONV2_HANDLE_ILL		0xC8	/*!< handle illegal */
#define CHAMELEONV2_OUT_OF_RES 		0xC9	/*!< out of resources */
#define CHAMELEONV2_NO_MORE_ENTRIES	0xCA	/*!< no more entries */
/** @} */

/**
 * @defgroup _CHAMV2_SUCCCODES Chameleon-V2 Success Codes
 * Returns a success (0xE0..0xFF) of Chameleon-V2 functions.
 * @{ */
#define CHAMELEONV2_UNIT_FOUND		0xE0	/*!< unit found */
#define CHAMELEONV2_BRIDGE_FOUND	0xE1	/*!< bridge found */
#define CHAMELEONV2_CPU_FOUND		0xE2	/*!< cpu found */
/** @} */


#define FPGA_HEADER_CODE_ABSENT		CHAMELEON_CODE_ABSENT
#define FPGA_HEADER_BAD_PARAM		0xBD
#define FPGA_DATA_CHKSUM_ERR		CHAMELEON_CODE_CHKSUM_ERR
#define FPGA_CODE_MISMATCH			CHAMELEON_CODE_MISMATCH

/**
 * @defgroup _CHAM_MAGICWORD Chameleon Magic Words
 * Used to detect byte swapping and revision of chameleon tab.
 * @{ */
#define CHAMELEON_MAGIC_00		0xABCD		/*!< chameleon-V0 table */
#define CHAMELEON_MAGIC_01		0xCDEF		/*!< chameleon-V1 table */
#define CHAMELEON_MAGIC_02		0xABCE		/*!< chameleon-V2 table */
/** @} */

/**
 * @defgroup _CHAMV2_BUSTYPE Chameleon-V2 Bus Types
 * Bus type of the bus that the chameleon-V2 table describes.
 * @{ */
#define CHAMELEONV2_BUSTYPE_WB		0x0		/*!< Wishbone Bus */
#define CHAMELEONV2_BUSTYPE_AV		0x1		/*!< Avalon Bus */
#define CHAMELEONV2_BUSTYPE_LPC		0x2		/*!< LPC Bus */
#define CHAMELEONV2_BUSTYPE_ISA		0x3		/*!< ISA Bus */
/** @} */

/**
 * @defgroup _CHAMV2_FF Chameleon-V2 InstanceFind() Flags
 * Flags that modify the behaviour of the InstanceFind() function.
 * @{ */
#define CHAMELEONV2_FF_BRGALL	0x0001 /*!< Find also all units and bridges within the bridged
                                            destination window of a matching bridge. See InstanceFind() */
/** @} */

#define CHAMELEONV2_FILENAME_SIZE	13		/*!< length of FPGA filename including
												 terminating null character */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

#ifndef CHAMELEON_CODES_ONLY

#include <MEN/oss.h>

/**
 * This structure describes a chameleon-V0/V1 unit
 */
typedef struct {
	u_int16 modCode;			/*!< unit module code */
	u_int16 revision;			/*!< revision of that module */
	u_int16 instance;			/*!< unit instance number 0..n */
	u_int16 interrupt;			/*!< chameleon internal interrupt number
								   assigned for unit */
	u_int16	bar;				/*!< unit's address space is in BARx (0..n) */
	u_int16 _pad;
	u_int32 offset;				/*!< unit's address space offset to BARx */
	void	*addr;				/*!< computed CPU address */
} CHAMELEON_UNIT;

/**
 * This structure provides information about the callers BAs
 */
typedef struct {
	u_int32	addr;		/*!< base address */
	u_int32 size;		/*!< size of address windows (0 if unknown) */
	int8 type;			/*!< 0=mem, 1=io, -1=unused */
} CHAMELEONV2_BA;

/**
 * This structure provides information about a chameleon-V2 device
 *
 * For Chameleon-V0/V1 devices, the following parameters are fix:
 * - chaRev    = 0
 * - busId     = 0
 * - tableNbr  = 1
 * - bridgeNbr = 0
 * - cpuNbr    = 0
 */
typedef struct {
	u_int16		chaRev;		/*!<  revision of all chameleon tables (0,1,2) */
	u_int16		busId;		/*!<  bus number of bus where caller resides (always 0x0000) */
	u_int16		tableNbr;	/*!<  number of chameleon tables within the table tree */
	u_int16		unitNbr;	/*!<  number of units of the chameleon device */
	u_int16		bridgeNbr;	/*!<  number of bridges of the chameleon device */
	u_int16		cpuNbr;		/*!<  number of cpus of the chameleon device */
	CHAMELEONV2_BA	ba[6];	/*!<  callers base addresses */
} CHAMELEONV2_INFO;

/**
 * This structure provides information about a chameleon-V2 table
 *
 * For Chameleon-V0/V1 devices, the following parameters are fix:
 * - busType   = CHAMELEONV2_BUSTYPE_WB
 * - busId     = 0
 * - file      = ""
 * - magicWord = 0xABCD
 * - reserved  = 0
 * - minRevision = 0
 */
typedef struct {
	u_int16 busType;	/*!< bus type of the FPGA internal bus
						     (\ref _CHAMV2_BUSTYPE) */
	u_int16 busId;		/*!< bus number, enumerated from library, see \ref _CHAM2_BUSID */
	char	model;		/*!< model of FPGA code (ASCII character) */
	u_int16	revision;	/*!< revision of FPGA code (0x00..0x3F) */
	char	file[CHAMELEONV2_FILENAME_SIZE]; /*!< FPGA filename
							(ASCII code 'A'-'Z', e.g. EM04N02IC002)
							including terminating null character */
	u_int16 magicWord;	/*!< magic-word of the chameleon table
						     (\ref _CHAM_MAGICWORD) */
	u_int16 minRevision;	/*!< minor Revision of FPGA code */
} CHAMELEONV2_TABLE;

/**
 * This structure provides information about a chameleon-V2 unit
 *
 * For Chameleon-V0/V1 devices, the following parameters are fix:
 * - variant  = 0
 * - group    = 0
 * - reserved = 0
 * - size     = 0 (unknown)
 * - busId    = 0
 */
typedef struct {
	/* type */
	u_int16 devId;				/*!< device id of unit (last three digit of the
                                     MEN-article name, e.g. 044 for 16Z044_DISP) */
	u_int16 variant;			/*!< variant of device (0x00..0x3F) */
	u_int16 revision;			/*!< revision of variant (0x00..0x3F) */

	/* location */
	u_int16 busId;				/*!< number of bus where unit resides, see \ref _CHAM2_BUSID */
	u_int16 instance;			/*!< instance number of unit (0..n) */
	u_int16 group;				/*!< group the unit belongs to (0..n) */

	/* resources */
	u_int16 interrupt;			/*!< chameleon internal interrupt number
									assigned to unit (0x00..0x3F) */
	u_int16	bar;				/*!< unit's address space is in BARx (0..7)
								     - caller's view, see \ref _CHAM2_ADDR */
	u_int32 offset;				/*!< unit's address space offset to BARx
								     - caller's view, see \ref _CHAM2_ADDR */
	u_int32 size;				/*!< unit's address space size */
	void	*addr;				/*!< computed unit address
								     - caller's view, see \ref _CHAM2_ADDR */
	u_int32 reserved;			/*!< reserved */
} CHAMELEONV2_UNIT;

/**
 * This structure provides information about a chameleon-V2 bridge
 */
typedef struct {
	/* type */
	u_int16 devId;				/*!< device id of bridge (last three digit of the
	                                 MEN-article name) */
	u_int16 variant;			/*!< variant of device (0x00..0x3F) */
	u_int16 revision;			/*!< revision of variant (0x00..0x3F) */

	/* location */
	u_int16 busId;				/*!< number of bus where bridge resides, see \ref _CHAM2_BUSID */
	u_int16 instance;			/*!< instance number of bridge (0..n) */

	/* connection */
	u_int16 nextBus;			/*!< number of the bus behind the bridge. */

	/* resources */
	u_int16 interrupt;			/*!< chameleon internal interrupt number
									assigned to bridge (0x00..0x3F) */
	u_int16	bar;				/*!< bridge's address space is in BARx (0..7)
								     - caller's view, see \ref _CHAM2_ADDR */
	u_int16	dbar;				/*!< bar-window which is behind the bridge (0..7) */
	u_int16 group;				/*!< group the bridge belongs to (0..n) */
	u_int32 offset;				/*!< bridge's address space offset to BARx
								     - caller's view, see \ref _CHAM2_ADDR */
	u_int32 size;				/*!< bridge's address space size */
	void	*addr;				/*!< computed CPU address
								     - caller's view, see \ref _CHAM2_ADDR */
} CHAMELEONV2_BRIDGE;

/**
 * This structure provides information about a chameleon-V2 CPU
 */
typedef struct {
	/* type */
	u_int16 devId;				/*!< device id of CPU (last three digit of the
	                                 MEN-article name) */
	u_int16 variant;			/*!< variant of device (0x00..0x3F) */
	u_int16 revision;			/*!< revision of variant (0x00..0x3F) */

	/* location */
	u_int16 busId;				/*!< number of bus where CPU resides (always 0x0000) */
	u_int16 instance;			/*!< instance number of CPU (0..n) */
	u_int16 group;				/*!< group the CPU belongs to (0..n) */

	/* resources */
	u_int16 interrupt;			/*!< chameleon internal interrupt number
									assigned to CPU (0x00..0x3F) */
	u_int16 _pad;
	u_int32 bootAddr;			/*!< boot address of CPU */
	u_int32 reserved;			/*!< reserved */
} CHAMELEONV2_CPU;

/**
 * This structure defines the characterstics for the requested modules to find,
 * used by the InstanceFind function
 */
typedef struct {
	int16 devId;		/*!< device id of UNIT/BRIDGE/CPU */
	int16 variant;		/*!< variant of UNIT/BRIDGE/CPU device */
	int16 instance;		/*!< instance number of UNIT/BRIDGE/CPU */
	int16 busId;		/*!< number of bus where UNIT/BRIDGE/CPU resides, see \ref _CHAM2_BUSID */
	int16 group;		/*!< group the UNIT/BRIDGE/CPU belongs to (0 means not grouped) */
	u_int16 flags;		/*!< flags, see \ref _CHAMV2_FF */
	int32 bootAddr;		/*!< boot address of CPU */
} CHAMELEONV2_FIND;

typedef void CHAMELEON_HANDLE;
typedef void CHAMELEONV2_HANDLE;	/*!<< chameleon-V2 handle */


/**
 * structure to get info about FPGA-file header
 */
typedef struct {

	char 	fileName[32];		/* ??? */
	char 	fpgaType[16];		/* identifier for FPGA HW type */
	char 	boardType[16];		/* identifier for board type (CPU/I/O brd)
								   with or without model */
	char 	cBoardType[16];		/* carrier board
								   (old EM04 style filename only) ,
								   without trailing '-'*/
	u_int32 size;				/* size in bytes of FPGA netto data */
	u_int32 totSize;			/* size of header + FPGA netto data */
	u_int32 xorSum;				/* copy of chksum from header */

	int32	icNo;
	int32	revision;
	int32	variant;

	u_int8 *data;				/* pointer to FPGA data */
} FPGA_HEADER_INFO;

#ifndef _FPGA_HEADER_H
typedef void FPGA_HEADER;
#endif

/**
 * Chameleon-V2 function table structure
 *
 * Holds the pointers to the \ref _CHAMV2_FUNC.
 */
typedef struct {
        int32 (*InitPci)(
				OSS_HANDLE *osh,
				u_int32 pciBus,
				u_int32 pciDev,
				u_int32 pciFunc,
				CHAMELEONV2_HANDLE **chahP);/*!< ptr to InitPci func */
        int32 (*InitInside)(
				OSS_HANDLE *osh,
				void *tblAddr,
				CHAMELEONV2_HANDLE **chahP);/*!< ptr to InitInside func */
        int32 (*Info)(
				CHAMELEONV2_HANDLE *chah,
				CHAMELEONV2_INFO *info);	/*!< ptr to Info func */
        int32 (*TableIdent)(
				CHAMELEONV2_HANDLE *chah,
				u_int32 idx,
				CHAMELEONV2_TABLE *table);	/*!< ptr to TableIdent func */
        int32 (*UnitIdent)(
				CHAMELEONV2_HANDLE *chah,
				u_int32 idx,
				CHAMELEONV2_UNIT *unit);	/*!< ptr to UnitIdent func */
        int32 (*BridgeIdent)(
				CHAMELEONV2_HANDLE *chah,
				u_int32 idx,
				CHAMELEONV2_BRIDGE *bridge);/*!< ptr to BridgeIdent func */
        int32 (*CpuIdent)(
				CHAMELEONV2_HANDLE *chah,
				u_int32 idx,
				CHAMELEONV2_CPU *cpu);		/*!< ptr to CpuIdent func */
        int32 (*InstanceFind)(
				CHAMELEONV2_HANDLE *chah,
				int32				idx,
				CHAMELEONV2_FIND	find,
				CHAMELEONV2_UNIT	*unit,
				CHAMELEONV2_BRIDGE	*bridge,
				CHAMELEONV2_CPU		*cpu);	/*!< ptr to InstanceFind func */
        void (*Term)(
				CHAMELEONV2_HANDLE **chahP);/*!< ptr to Term func */
} CHAM_FUNCTBL;

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

#ifndef  CHAM_VARIANT
# define _CHAM_GLOBNAME(var,name) name
#else
# define _CHAM_GLOBNAME(var,name) var##name
#endif

#define CHAM_GLOBNAME(var,name)  _CHAM_GLOBNAME(var,name)

/* Chameleon-V0 functions */
#ifdef CHAMELEONV2_EXPORT_CHAMELEONV0
	#define ChameleonInit              CHAM_GLOBNAME(CHAM_VARIANT,          \
	                                                   ChameleonInit)
	#define ChameleonGlobalIdent       CHAM_GLOBNAME(CHAM_VARIANT,          \
	                                                   ChameleonGlobalIdent)
	#define ChameleonUnitIdent         CHAM_GLOBNAME(CHAM_VARIANT,          \
	                                                   ChameleonUnitIdent)
	#define ChameleonUnitFind          CHAM_GLOBNAME(CHAM_VARIANT,          \
	                                                   ChameleonUnitFind)
	#define ChameleonTerm              CHAM_GLOBNAME(CHAM_VARIANT,          \
	                                                   ChameleonTerm)

	extern int32 ChameleonInit( OSS_HANDLE *, int pciBus, int pciDev, CHAMELEON_HANDLE ** );
	extern int32 ChameleonGlobalIdent( CHAMELEON_HANDLE *, char *variantP,
								int *revisionP );
	extern int32 ChameleonUnitIdent( CHAMELEON_HANDLE *, int idx, CHAMELEON_UNIT *);
	extern int32 ChameleonUnitFind( CHAMELEON_HANDLE *, int modCode, int instance,
							 CHAMELEON_UNIT *);
	extern void  ChameleonTerm( CHAMELEON_HANDLE **);
#endif /* CHAMELEONV2_EXPORT_CHAMELEONV0 */

#define ChameleonLoadEm04          CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   ChameleonLoadEm04)
#define ChameleonLoadEm01          CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   ChameleonLoadEm01)
#define ChameleonLoadEm03          CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   ChameleonLoadEm03)
#define ChameleonFpgaCodeIdentEm04 CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   ChameleonFpgaCodeIdentEm04)
#define ChameleonLoad_A015         CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   ChameleonLoad_A015)

/* Chameleon code functions */
#define ChameleonModName           CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   ChameleonModName)
#define CHAM_DevIdToName		   CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   CHAM_DevIdToName)
#define CHAM_ModCodeToDevId        CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   CHAM_ModCodeToDevId)
#define CHAM_DevIdToModCode        CHAM_GLOBNAME(CHAM_VARIANT,          \
                                                   CHAM_DevIdToModCode)

/* Chameleon-V0 functions */
extern int32 ChameleonLoadEm01(OSS_HANDLE *osh, const FPGA_HEADER *fpgaCode,
							   void *pldRegBase);
extern int32 ChameleonLoadEp01(OSS_HANDLE *osh, const FPGA_HEADER *fpgaCode,
							   void *pldRegBase);
extern int32 ChameleonLoadEm03(OSS_HANDLE *osh, const FPGA_HEADER *fpgaCode,
							   void *pldRegBase );
extern int32 ChameleonFpgaCodeIdentEm03(
	const FPGA_HEADER *fpgaCode,
	int *variantP,
	int *revisionP);


extern int32 ChameleonLoadEm04(OSS_HANDLE *osh, const FPGA_HEADER *fpgaCode);
extern int32 ChameleonFpgaCodeIdentEm04(
	const FPGA_HEADER *fpgaCode,
	int *variantP,
	int *revisionP);

extern int32 ChameleonLoad_A015(
	OSS_HANDLE *osh,
	int        icNumber,
	const FPGA_HEADER *fpgaCode
);

/* Chameleon code functions */
extern const char *ChameleonModName( u_int16 modCode );
extern const char *CHAM_DevIdToName( u_int16 devId );
extern u_int16 CHAM_ModCodeToDevId( u_int16 modCode );
extern u_int16 CHAM_DevIdToModCode( u_int16 devId );

int32 ChameleonFpgaHeaderIdent(
	const void *_hdr,
	u_int32 maxSize,
	FPGA_HEADER_INFO *info);
int32 ChameleonFpgaDataXor( const u_int32 *p, u_int32 nBytes, u_int32 *xorP );
int32 ChameleonFpgaDataCheckXor( FPGA_HEADER_INFO *info);
int32 ChameleonFpgaHeaderOkForBoard(
	const FPGA_HEADER_INFO *info,
	const char *mainProductName,
	const char *carrierName);

#ifndef  CHAMV2_VARIANT
# define _CHAMV2_GLOBNAME(var,name) name
#else
# define _CHAMV2_GLOBNAME(var,name) name##var
#endif

#define CHAMV2_GLOBNAME(var,name)  _CHAMV2_GLOBNAME(var,name)

#ifdef CHAMV2_VARIANT
	#define CHAM_Init              CHAMV2_GLOBNAME(CHAMV2_VARIANT, CHAM_Init)
#else
	#define CHAM_Init              CHAM_InitMem
#endif /* CHAMV2_VARIANT */

/* Chameleon-V2 functions */
extern int32 CHAM_InitIo( CHAM_FUNCTBL *fP );
extern int32 CHAM_InitMem( CHAM_FUNCTBL *fP );
extern int32 CHAM_InitIoSw( CHAM_FUNCTBL *fP );
extern int32 CHAM_InitMemSw( CHAM_FUNCTBL *fP );

#endif /* CHAMELEON_CODES_ONLY */

#ifdef __cplusplus
	}
#endif

#endif	/* _CHAMELEON_H */

#if !defined(_CHAMELEON_H_2ND) && defined(_CHAMELEON_STRINGS_C)
#define _CHAMELEON_H_2ND

#ifdef __cplusplus
	extern "C" {
#endif /* __cplusplus */

	static const struct {
		u_int16		modCode;	/* V0 module-code */
#if defined OS9000
		const char name[20];	/* unit name */
#else
		const char *name;	    /* unit name */
#endif
		u_int16		devId;		/* V2 device Id */
	} G_ChamTbl[] = {
		{ 0x0					  , "not identified", 0xffff },
		{ CHAMELEON_16Z001_SMB    , "16Z001_SMB"	,  1 },
		{ CHAMELEON_16Z016_IDE    , "16Z016_IDE"	, 16 },
		{ CHAMELEON_16Z022_GPIO   , "16Z022_GPIO"	, 22 },
		{ CHAMELEON_16Z023_IDE    , "16Z023_IDE"	, 23 },
		{ CHAMELEON_16Z024_SRAM   , "16Z024_SRAM"	, 24 },
		{ CHAMELEON_16Z025_UART   , "16Z025_UART"	, 25 },
		{ CHAMELEON_16Z029_CAN    , "16Z029_CAN"	, 29 },
		{ CHAMELEON_16Z030_UARTHLP, "16Z030_UARTHLP", 30 },
		{ CHAMELEON_16Z031_SPI    , "16Z031_SPI"	, 31 },
		{ CHAMELEON_16Z032_DISP   , "16Z032_DISP"	, 32 },
		{ CHAMELEON_16Z033_DISP   , "16Z033_DISP"	, 33 },
		{ CHAMELEON_16Z034_GPIO   , "16Z034_GPIO"	, 34 },
		{ CHAMELEON_16Z035_SYSTEM , "16Z035_SYSTEM"	, 35 },
		{ 0x0					  , "16Z037_GPIO"	, 37 },
		{ CHAMELEON_16Z043_SDRAM  , "16Z043_SDRAM"	, 43 },
		{ CHAMELEON_16Z044_DISP	  ,	"16Z044_DISP"	, 44 },
		{ CHAMELEON_16Z045_FLASH  , "16Z045_FLASH"	, 45 },
		{ CHAMELEON_16Z048_ISA    , "16Z048_ISA"	, 48 },
		{ CHAMELEON_16Z049_TMR    , "16Z049_TMR"	, 49 },
		{ CHAMELEON_16Z050_BIO    , "16Z050_BIO"	, 50 },
		{ CHAMELEON_16Z051_DAC    , "16Z051_DAC"	, 51 },
		{ CHAMELEON_16Z052_GIRQ   , "16Z052_GIRQ"	, 52 },
		{ CHAMELEON_16Z053_IDE	  , "16Z053_IDETGT"	, 53 },
		{ CHAMELEON_16Z054_SYSTEM , "16Z054_SYSTEM"	, 54 },
		{ CHAMELEON_16Z055_HDLC   , "16Z055_HDLC"	, 55 },
		{ CHAMELEON_16Z056_SPI    , "16Z056_SPI"	, 56 },
		{ CHAMELEON_16Z057_UART,	"16Z057_UART"	, 57 },
		{ CHAMELEON_16Z059_IRQ    , "16Z059_IRQ"	, 59 },
		{ CHAMELEON_16Z061_PWM , 	"16Z061_PWM"	, 61 },
		{ CHAMELEON_16Z062_DMA , 	"16Z062_DMA"	, 62 },
		{ CHAMELEON_16Z063_NAND_RAW,"16Z063_NAND_RAW",63 },
		{ CHAMELEON_16Z064_ZOOM,	"16Z064_ZOOM"	, 64 },
		{ CHAMELEON_16Z065_WBAV,	"16Z065_WBAV"	, 65 },
		{ CHAMELEON_16Z066_MMOD,	"16Z066_MMOD"	, 66 },
		{ CHAMELEON_16Z067_VME,		"16Z065_VME"	, 67 },
		{ CHAMELEON_16Z068_IDE,		"16Z068_IDE"	, 68 },
		{ CHAMELEON_16Z069_RST,		"16Z069_RST"	, 69 },
		{ CHAMELEON_16Z072_OWB,		"16Z072_OWB"	, 72 },
		{ CHAMELEON_16Z073_QDEC,	"16Z073_QDEC"	, 73 },
		{ CHAMELEON_16Z074_REGSPI,	"16Z074_REGSPI"	, 74 },
		{ CHAMELEON_16Z075_SPEED,	"16Z075_SPEED"	, 75 },
		{ CHAMELEON_16Z076_QSPI,	"16Z076_QSPI"	, 76 },
		{ CHAMELEON_16Z077_ETH,		"16Z077_ETH"	, 77 },
		{ 0x0,  					"16Z078_NIOS" 	, 78 },
		{ CHAMELEON_16Z079_ANYBUS,  "16Z079_ANYBUS"	, 79 },
		{ CHAMELEON_16Z080_ARCNET,  "16Z080_ARCNET"	, 80 },
		{ CHAMELEON_16Z081_FIP,  	"16Z081_FIP"	, 81 },
		{ CHAMELEON_16z082_IMPULSE,	"16Z082_IMPULSE", 82 },
		{ CHAMELEON_16Z084_IDEEPROM,"16Z084_IDEEPROM",84 },
		{ CHAMELEON_16Z085_MS09N,	"16Z085_MS09N"  , 85 },
		{ 0x0,						"16Z086_INTERBUS",86 },
		{ CHAMELEON_16Z087_ETH,		"16Z087_ETH"	, 87 },
		{ 0x0,						"16Z088_PG"		, 88 },
		{ CHAMELEON_16Z089_HSSL,	"16Z089_HSSL"	, 89 },
		{ CHAMELEON_16Z090_DPRAM,	"16Z090_DPRAM"	, 90 },
		{ 0x0,						"16Z093_AFDX"	, 93 },
		{ CHAMELEON_16Z094_WB28XX,	"16Z094_WB28XX"	, 94 },
		{ CHAMELEON_16Z023_IDENHS,  "16Z023_IDENHS"	, 123 },
		{ CHAMELEON_16Z125_UART,  	"16Z125_UART"	, 125 },
        { CHAMELEON_16Z126_SERFLASH,"16Z126_SERFLASH", 126 },
		{ 0x0,  					"16Z127_GPIO"	, 127 },


		{ CHAMELEON_CUSTOMER_MODULE,"CUSTOMER_MODULE",900 },
		{ CHAMELEON_PCI_INT,		"PCI INT"		, 0xffff },
		{ CHAMELEON_CANGPIO,		"CAN GPIO"		, 0xffff },
#if defined OS9000
        { 0xffff, "\0", 0xffff }	/* end of table */
#else
		{ 0xffff, NULL, 0xffff }	/* end of table */
#endif
	};

#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif /* _CHAMELEON_H_2ND && _CHAMELEON_STRINGS_C */












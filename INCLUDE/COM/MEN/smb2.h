/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  smb2.h
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2009/12/22 16:04:18 $
 *    $Revision: 3.23 $
 *
 *     \project  SMB2 Library
 *       \brief  system managment bus routines interface
 *    \switches  SMB_COMPILE - for module compilation
 *				 MAC_IO_MAPPED - only valid for SMB_PORT_xxx
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: smb2.h,v $
 * Revision 3.23  2009/12/22 16:04:18  gvarlet
 * R: False definition of MOFFS
 * M: Remove false definition of MOFFS
 *
 * Revision 3.22  2009/12/10 11:08:02  gvarlet
 * R: Definition for MOFFS missing.
 * M: Add define for MOFFS
 *
 * Revision 3.21  2009/07/28 14:22:05  sy
 * add declaration of SMB_MENZ001_Init_Aligned()
 *
 * Revision 3.20  2009/06/22 11:59:41  dpfeuffer
 * R: MDVE warning
 * M: added __MAPILIB define
 *
 * Revision 3.19  2008/10/20 10:43:00  MSoehnlein
 * added SCH support
 *
 * Revision 3.18  2008/06/30 18:11:02  CSchuster
 * R: Library may handle new error when controller is busy for other SW
 * M: added new error code SMB_ERR_CTRL_BUSY
 *
 * Revision 3.17  2008/05/06 14:21:59  RLange
 * Added Multimaster flag in MPC5200 SMB Descriptor
 *
 * Revision 3.16  2007/11/23 20:19:27  cs
 * added:
 *  + multimasterCap to Z001 descriptor
 *
 * Revision 3.15  2007/10/09 09:33:59  SVogel
 * added busyWait to ICH descriptor
 *
 * Revision 3.14  2007/09/12 12:04:35  ufranke
 * changed
 *  - fill ReservedFctP3() with UseOssDelay()
 *    to switch between OSS_MikroDelay() and OSS_Delay()
 *    at runtime
 *
 * Revision 3.13  2007/03/08 15:20:04  cs
 * added:
 *   + support for PORTCB (here +documentation, +dbgLevel)
 *
 * Revision 3.12  2007/02/27 20:55:16  rt
 * fixed:
 * - MenMon does not know ERR_DEV
 *
 * Revision 3.11  2007/02/20 15:10:36  DPfeuffer
 * - map SMB error codes into MDIS device specific error code range now fix
 * - undo: changed interface of *_Init() and *_Exit functions
 *         (VxW6.3 must consider this by disabling strict aliasing)
 *
 * Revision 3.10  2007/01/05 17:35:29  cs
 * added:
 *   + SMB_ERR_NO_IDLE
 *
 * Revision 3.9  2006/11/27 09:16:49  rt
 * cosmetics
 *
 * Revision 3.8  2006/09/11 11:27:40  cs
 * changed:
 *   - interface of *_Init() and *_Exit functions (avoid warnings for VxW6.3)
 *
 * Revision 3.7  2006/05/26 15:08:37  DPfeuffer
 * error codes for SMB2_API modified
 *
 * Revision 3.6  2006/03/03 10:52:47  DPfeuffer
 * - SMB_ENTRIES sructure: __MAPILIB for function pointer declarations added
 * (required for proper calling convention for SMB2_API under Windows)
 *
 * Revision 3.5  2006/02/27 15:26:50  DPfeuffer
 * - added define SMB_ERR_ADDR_EXCLUDED
 * - changed struct SMB_DESC_ICH
 *
 * Revision 3.4  2006/02/22 16:02:08  DPfeuffer
 * modifications during SMB2_API development
 * SMB_DESC_ICH struct: alertPollFreq added
 * Revision 3.3  2006/02/07 19:23:01  cschuster
 * added
 *    + prototypes and descriptors for MGT5200 and MPC85xx I2C controllers
 *    + SMB_ERR_GENERAL define
 * swapped addr and flags in I2CMESSAGE struct
 * fixed SMB_FUNC_SMBUS_EMUL define
 *
 * Revision 3.2  2005/11/24 10:14:18  SVogel
 * Added ICH support to library.
 *
 * Revision 3.1  2005/11/22 09:22:33  cschuster
 * Initial Revision
 * cloned from sysmanagbus.h
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#ifndef _SMB2_H
#  define _SMB2_H

#  ifdef __cplusplus
      extern "C" {
#  endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/

#ifndef SMB2_API
/********* Descriptors for initializing host controller and library ************/

/** Descriptor for the MEN Z001_SMB FPGA I2C controller */
typedef struct
{
	void	   *baseAddr;		/**< base address of Z001_SMB controller */
	u_int32    sclFreq;			/**< SCL frequency [kHz](37/55/74/111/149/223)*/
	u_int32    dbgLevel;		/**< debug level (see dbg.h) */
	int32	   busyWait;		/**< wait time for blocking semaphore (lib busy)
									 ms - 20 is a good value */
	int32	   alertPollFreq;	/**< frequency lib shall poll for SMB alerts
									 ms - 1000 is a good value,
									 0 = no automatic polling */
	u_int32	   timeOut;			/**< wait time if SMBus is busy
									 ms - 20 is a good value (SMBus busy) */
	u_int32	   mikroDelay;		/**< default 0 - OSS_Delay, 1 - OSS_MikroDelay */
	u_int32    multimasterCap;  /**< multi master capability
									 0 = single master only, 1 = multi master bus */
}SMB_DESC_MENZ001;

/** descriptor for the ALI1543 internal SMB controller */
typedef struct
{
	void       *baseAddr;
}SMB_DESC_ALI1543;

/** descriptor for the PORT emulated SMB controller using callbacks */
typedef struct
{
	u_int8     busClock;    /**< 0   -    max - no internal delay
							  *  1   -   1kHz - OSS_Delay() call
							  *  10  -  10kHz - OSS_MikroDelay() call
							  *  100 - 100kHz - OSS_MikroDelay() call
							  */
	void	   *cbArg;	    /**< argument that is passed to callbacks */
	int 	   (*sclIn)(void *);			/**< read SCL pin         */
	int		   (*sclSet)(void *, int val);	/**< set/verify SCL       */
	int		   (*sdaIn)(void *); 			/**< read SDA pin         */
	int		   (*sdaSet)(void *, int val);	/**< set/verify SDA       */

	u_int32    dbgLevel;	/**< debug level (see dbg.h) */
} SMB_DESC_PORTCB;

/** Descriptor for the Intel ICH controller */
typedef struct
{
	void	   *baseAddr;		/**< base address of Intel ICH controller */
	u_int32    dbgLevel;		/**< see dbg.h */
	int32	   alertPollFreq;	/**< frequency lib shall poll for SMB alerts
									 ms - 1000 is a good value,
									 0 = no automatic polling */
    int32	   busyWait;		/**< wait time for blocking semaphore (lib busy)
									 ms - 20 is a good value */
}SMB_DESC_ICH;

/** Descriptor for the Intel SCH controller */
typedef struct
{
	void	   *baseAddr;		/**< base address of Intel ICH controller */
	u_int32    dbgLevel;		/**< see dbg.h */
	int32	   alertPollFreq;	/**< frequency lib shall poll for SMB alerts
									 ms - 1000 is a good value,
									 0 = no automatic polling */
    int32	   busyWait;		/**< wait time for blocking semaphore (lib busy)
									 ms - 20 is a good value */
}SMB_DESC_SCH;

/** descriptor for the MGT5200 internal I2C controller */
typedef struct
{
	void	   *baseAddr;		/**< base address of MGT5200 I2C controller */
	u_int32    divReg;    		/**< see MGT5200 UM, Frequency divider reg. */
	u_int32    filterReg;    	/**< see MPC5200 UM, digital filter sampling reg. */
	u_int32    dbgLevel;		/**< see dbg.h */
	u_int32	   timeOut;			/**< ms - 20 is a good value */
	u_int32	   mikroDelay;		/**< default 0 - OSS_Delay, 1 - OSS_MikroDelay */
	int32	   alertPollFreq;	/**< frequency lib shall poll for SMB alerts
									 ms - 1000 is a good value,
									 0 = no automatic polling */
	u_int32    multimasterCap;  /**< multi master capability
									 0 = single master only, 0 != multi master bus (slave Address) */
}SMB_DESC_MGT5200;

/** descriptor for the MPC85XX internal I2C controller */
typedef struct
{
	void	   *baseAddr;		/**< base address of MPC85XX I2C controller */
	u_int32    divReg;    		/**< see MPC85XX UM, Frequency divider reg. */
	u_int32    filterReg;    	/**< see MPC85XX UM, digital filter sampling reg. */
	u_int32    dbgLevel;		/**< see dbg.h */
	u_int32	   timeOut;			/**< ms - 20 is a good value */
	u_int32	   mikroDelay;		/**< default 0 - OSS_Delay, 1 - OSS_MikroDelay */
	int32	   alertPollFreq;	/**< frequency lib shall poll for SMB alerts
									 ms - 1000 is a good value,
									 0 = no automatic polling */
}SMB_DESC_MPC85XX;

/******************** Common typedefs for SMB2 library *************************/

/** SMBus Handle used in both, common and controller specific part of library */
typedef struct {
	OSS_HANDLE		*osHdl;			/**< OSS handle   */
	u_int32			dbgLevel;		/**< Debug level  */
	DBG_HANDLE		*dbgHdl;		/**< Debug handle */
	OSS_SEM_HANDLE 	*hostCtrlSem;	/**< Semaphore for controller blocking    */
	int32			busyWait;		/**< OSS_SemWait time (ms) for blocking   */
	OSS_ALARM_HANDLE *alarmHdl;		/**< OSS_ALARM_HANDLE for SMBALARM polling*/
	int32			alertPollFreq;	/**< frequency lib shall poll for SMB alerts
									     ms - 1000 is a good value */
	OSS_DL_LIST 	smbAlertCb; 	/**< Callback functions registered (list) */
	int32 (*ExitCom)( void *smbHdl );/**< unititialize common SMB library */
}SMB_COM_HANDLE;

#endif /* SMB2_API */

/** I2C Message - used for pure i2c transaction, also from /dev interface */
typedef struct I2CMESSAGE
{
	u_int16 addr;					/**< slave address					*/
	u_int32 flags;					/**< flags for communication		*/
#define I2C_M_TEN			0x0010	/**< we have a ten bit chip address	*/
#define I2C_M_WR			0x0000	/**< write access					*/
#define I2C_M_RD			0x0001	/**< read access					*/
#define I2C_M_NOSTART		0x4000	/**< don't send start bit			*/
#define I2C_M_REV_DIR_ADDR	0x2000
	u_int16 len;					/**< msg length						*/
	u_int8 *buf;					/**< pointer to msg data			*/
} SMB_I2CMESSAGE;

#define i2cMessage SMB_I2CMESSAGE	/* for compatibility with older SW */

#ifndef __MAPILIB
#	define __MAPILIB
#endif

/** SMBus Library Functions, this is used to access the SMBus		*/
typedef struct
{
	/** Get revision of library for this controller */
	char* (__MAPILIB *Ident)( void );

	/** Unititialize library */
	int32 (__MAPILIB *Exit)( void **smbHdlP );

	/** Read/write flag is part of the command
	    data=1: read command; data=0: write command*/
	int32 (__MAPILIB *QuickComm)( void *smbHdl, u_int32 flags, u_int16 addr,
									u_int8 read_write );

	/** Write one byte to an SMBus device */
	int32 (__MAPILIB *WriteByte)( void *smbHdl, u_int32 flags, u_int16 addr,
									u_int8 data );

	/** Read one byte from an SMBus device */
	int32 (__MAPILIB *ReadByte)( void *smbHdl, u_int32 flags, u_int16 addr,
									u_int8 *data );

	/** Write command and one byte to an SMBus device */
	int32 (__MAPILIB *WriteByteData)( void *smbHdl, u_int32 flags, u_int16 addr,
									u_int8 cmdAddr, u_int8 data );


	/** Write command and read on byte from SMBus device */
	int32 (__MAPILIB *ReadByteData)	( void *smbHdl, u_int32 flags, u_int16 addr,
										u_int8 cmdAddr, u_int8 *data );

	/** Write command and two bytes to SMBus device */
	int32 (__MAPILIB *WriteWordData)( void *smbHdl, u_int32 flags, u_int16 addr,
										u_int8 cmdAddr, u_int16 data );

	/** Write command and read two bytes from SMBus device */
	int32 (__MAPILIB *ReadWordData)( void *smbHdl, u_int32 flags, u_int16 addr,
										u_int8 cmdAddr, u_int16 *data );

	/** Write command and data block to SMBus device */
	int32 (__MAPILIB *WriteBlockData)( void *smbHdl, u_int32 flags, u_int16 addr,
									u_int8 cmdAddr, u_int8 length, u_int8 *data );

	/** Write command and read data block from SMBus device */
	int32 (__MAPILIB *ReadBlockData)( void *smbHdl, u_int32 flags, u_int16 addr,
									u_int8 cmdAddr,  u_int8 *length, u_int8 *data );

	/** Write command and two bytes to and receive two bytes from an SMBus device*/
	int32 (__MAPILIB *ProcessCall)( void *smbHdl, u_int32 flags, u_int16 addr,
									u_int8 cmdAddr, u_int16 *data );

	/** Write command and data block to and receive data block from an SMBus device*/
	int32 (__MAPILIB *BlockProcessCall)( void *smbHdl, u_int32 flags, u_int16 addr,
							   u_int8 cmdAddr, u_int8 writeDataLen, u_int8 *writeData,
							   u_int8 *readDataLen, u_int8 *readData );

	/** perform an SMBALERT broadcast */
	int32 (__MAPILIB *AlertResponse)( void *smbHdl, u_int32 flags,
									u_int16 addr, u_int16 *alertCnt);

	/** install callback function to be invoked on alert from a specific SMBus address  */
	int32 (__MAPILIB *AlertCbInstall)( void *smbHdl, u_int16 addr,
									void (__MAPILIB *cbFuncP)( void *cbArg ), void *cbArg );

	/** uninstall callback function */
	int32 (__MAPILIB *AlertCbRemove)( void *smbHdl, u_int16 addr, void **cbArgP );

	 /* Reserved function pointers */
	int32 (__MAPILIB *ReservedFctP1)( void );
	int32 (__MAPILIB *ReservedFctP2)( void );
	int32 (__MAPILIB *ReservedFctP3)( void );

	/** setup used OSS_Delay or OSS_MikroDelay */
	int32 (__MAPILIB *UseOssDelay)(  void *smbHdl, int useOssDelay );

	/** Master SMBus transfer function (for SMBus host controllers) */
	int32 (__MAPILIB *SmbXfer)( void *smbHdl, u_int32 flags, u_int16 addr,
								u_int8 read_write, u_int8 cmdAdr,
								u_int8 size, u_int8 *data);

	/** Master I2C bus transfer function (for I2C host controllers ) */
	int32 (__MAPILIB *I2CXfer)( void *smbHdl, SMB_I2CMESSAGE msg[], u_int32 num );

	int32 Reserved1[2];	/*   Reserved */
	u_int32 Capability;	/**< Capability flag (Hardware + SMBus library) */
}SMB_ENTRIES;

#if !defined(SMB_COMPILE) && !defined(SMB2_API_COMPILE)
	typedef SMB_ENTRIES SMB_HANDLE;
#endif /*SMB_COMPILE*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+-----------------------------------------*/
/** Predefined SMBus addresses */
/** only bits 7-1 are relevant, bit 0 will be replaced by read/write flag */
#define SMB_ADDR_GENERAL_CALL	0x00
#define SMB_ADDR_10BIT_SLAVE	0xF0 	/**< first 5 Bits of 10bit SMB/I2C address */
										/*!< for I2C only, currently SMB does
										 *   not support 10-bit addressing
										 *   this represents only bits 7-3,
										 *   bit 2-1 are bit 9-8 of 10 bit addr
										 */
#define SMB_ADDR_ALERT_RESPONSE	0x18	/**< SMBus Alert Response Address (ARA) */

/** SMBus_access read or write markers */
#define SMB_READ	1	/**< 8th bit of address, read access */
#define SMB_WRITE	0	/**< 8th bit of address, write access */

/** Flags what functionality is present from controller (Capability flag)*/
#define SMB_FUNC_I2C					0x00000001 /**< This is an I2C host controller */
#define SMB_FUNC_10BIT_ADDR				0x00000002 /**< 10-bit addressing mode*/
#define SMB_FUNC_PROTOCOL_MANGLING		0x00000004 /* I2C_M_{REV_DIR_ADDR,NOSTART} */
#define SMB_FUNC_SMBUS_HWPEC_CALC		0x00000008 /**< Packet error checking done in HW */
#define SMB_FUNC_SMBUS_QUICK			0x00010000
#define SMB_FUNC_SMBUS_READ_BYTE		0x00020000
#define SMB_FUNC_SMBUS_WRITE_BYTE		0x00040000
#define SMB_FUNC_SMBUS_READ_BYTE_DATA	0x00080000
#define SMB_FUNC_SMBUS_WRITE_BYTE_DATA	0x00100000
#define SMB_FUNC_SMBUS_READ_WORD_DATA	0x00200000
#define SMB_FUNC_SMBUS_WRITE_WORD_DATA	0x00400000
#define SMB_FUNC_SMBUS_PROC_CALL		0x00800000
#define SMB_FUNC_SMBUS_READ_BLOCK_DATA	0x01000000
#define SMB_FUNC_SMBUS_WRITE_BLOCK_DATA 0x02000000
#define SMB_FUNC_SMBUS_BLOCK_PROC_CALL	0x04000000
#define SMB_FUNC_SMBUS_READ_I2C_BLOCK	0x10000000 /**< New I2C-like block transfer */
#define SMB_FUNC_SMBUS_WRITE_I2C_BLOCK	0x20000000 /**< New I2C-like block transfer */

#define SMB_FUNC_SMBUS_BYTE 	  SMB_FUNC_SMBUS_READ_BYTE | \
								  SMB_FUNC_SMBUS_WRITE_BYTE
#define SMB_FUNC_SMBUS_BYTE_DATA  SMB_FUNC_SMBUS_READ_BYTE_DATA | \
								  SMB_FUNC_SMBUS_WRITE_BYTE_DATA
#define SMB_FUNC_SMBUS_WORD_DATA  SMB_FUNC_SMBUS_READ_WORD_DATA | \
								  SMB_FUNC_SMBUS_WRITE_WORD_DATA
#define SMB_FUNC_SMBUS_BLOCK_DATA SMB_FUNC_SMBUS_READ_BLOCK_DATA | \
								  SMB_FUNC_SMBUS_WRITE_BLOCK_DATA
#define SMB_FUNC_SMBUS_I2C_BLOCK  SMB_FUNC_SMBUS_READ_I2C_BLOCK | \
								  SMB_FUNC_SMBUS_WRITE_I2C_BLOCK

#define SMB_FUNC_SMBUS_EMUL SMB_FUNC_SMBUS_QUICK | \
							SMB_FUNC_SMBUS_BYTE | \
							SMB_FUNC_SMBUS_BYTE_DATA | \
							SMB_FUNC_SMBUS_WORD_DATA | \
							SMB_FUNC_SMBUS_PROC_CALL | \
							SMB_FUNC_SMBUS_WRITE_BLOCK_DATA

/**********************************************************
 * Note: Any changes on SMB_ERR_ codes must be considered
 *       in SMB2_API/smb2_api.c/errStrTable
 *********************************************************/
/**
 * @defgroup _SMB2_ERR SMB_ERR_X Error Codes
 * Error codes used by the SMB2 and SMB2_API functions.
 * @{ */

/* map SMB error codes into MDIS device specific error code range */
#ifdef ERR_DEV
# define MOFFS (ERR_DEV+0x80)
#else
# define MOFFS 0
#endif /*ERR_DEV*/

#define SMB_ERR_NO           		(0x00)		   /**< No Error */
#define SMB_ERR_DESCRIPTOR   		(0x01 + MOFFS) /**< Initial data missing/wrong */
#define SMB_ERR_NO_MEM       		(0x02 + MOFFS) /**< Could not allocate ressources */
#define SMB_ERR_ADDR    	 		(0x03 + MOFFS) /**< Address not present or wrong */
#define SMB_ERR_BUSY    	 		(0x04 + MOFFS) /**< Bus is busy */
#define SMB_ERR_COLL    	 		(0x05 + MOFFS) /**< Bus collision */
#define SMB_ERR_NO_DEVICE	 		(0x06 + MOFFS) /**< No device found */
#define SMB_ERR_PARAM		 		(0x07 + MOFFS) /**< Wrong parameters passed  */
#define SMB_ERR_PEC		 			(0x08 + MOFFS) /**< PEC error detected */
#define SMB_ERR_NOT_SUPPORTED 		(0x09 + MOFFS) /**< Function/Access size not supported */
#define SMB_ERR_GENERAL			    (0x0a + MOFFS) /**< General Error (timeout, ...) */
#define SMB_ERR_ALERT_INSTALL 		(0x0b + MOFFS) /**< Alert callback installation failed */
#define SMB_ERR_ALERT_NOSIG			(0x0c + MOFFS) /**< No free signal for alert */
#define SMB_ERR_ADDR_EXCLUDED 		(0x0d + MOFFS) /**< Address is excluded */
#define SMB_ERR_NO_IDLE		 		(0x0e + MOFFS) /**< Bus did not get idle after STOP */
#define SMB_ERR_CTRL_BUSY			(0x0f + MOFFS) /**< Controller is busy */
#define SMB_ERR_LAST 				(0x10 + MOFFS) /**< Last SMB_ERR code (don't use it as errror code) */
/** @} */

/**
 * @defgroup _SMB2_FLAG SMB_FLAG_X flags
 * Flags for SMB2_API functions.
 * @{ */
#define SMB_FLAG_TENBIT			0x00000001 /**< access device with 10-bit address*/
#define SMB_FLAG_PEC			0x00000002 /**< add PEC to data */
/** @} */

#define SMB_DESC_PORT_FLAG_SIZE_MASK		0x07
#define SMB_DESC_PORT_FLAG_SIZE_8			0x01
#define SMB_DESC_PORT_FLAG_SIZE_16			0x02
#define SMB_DESC_PORT_FLAG_SIZE_32			0x04

#define SMB_DESC_PORT_FLAG_POLARITY_HIGH	0x10
#define SMB_DESC_PORT_FLAG_READABLE_REG		0x20

/** maximum packet size */
#define SMB_BLOCK_MAX_BYTES 32	/**< block size limit for SMBus accesses */
								/*!< buffers should be allocated to at least
								*    SMB_BLOCK_MAX_BYTES + 2 bytes
								*    (include length + maybe PEC)  */
#define I2C_BLOCK_MAX_BYTES 128 /**< block size limit for I2C accesses */
								/*!< Not specified, but some limit needed */

/** SMBus transaction types (size parameter in the *Xfer function) */
#define SMB_ACC_QUICK			0
#define SMB_ACC_BYTE			1
#define SMB_ACC_BYTE_DATA		2
#define SMB_ACC_WORD_DATA		3
#define SMB_ACC_PROC_CALL		4
#define SMB_ACC_BLOCK_DATA		5
#define SMB_ACC_BLOCK_PROC_CALL	6
#define SMB_ACC_I2C_BLOCK_DATA	7

#ifndef SMB2_API
/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/** Init function for common SMB2 library,
	to be called on Init of controller specific library only (from there) */
u_int32 SMB_COM_Init( void *smbHdl );

/** Init function for MEN 16Z001_SMB host controller */
extern u_int32 SMB_MENZ001_Init(    SMB_DESC_MENZ001 *descP, OSS_HANDLE *osHdl, void **smbHdlP );
extern u_int32 SMB_MENZ001_IO_Init( SMB_DESC_MENZ001 *descP, OSS_HANDLE *osHdl, void **smbHdlP );
extern u_int32 SMB_MENZ001_Init_Aligned(SMB_DESC_MENZ001 *descP, OSS_HANDLE *osHdl, void **smbHdlP );

/** Init function for ICH SMB host controller */
extern u_int32 SMB_ICH_Init(        SMB_DESC_ICH     *descP, OSS_HANDLE *osHdl, void **smbHdlP);
extern u_int32 SMB_ICH_IO_Init(     SMB_DESC_ICH     *descP, OSS_HANDLE *osHdl, void **smbHdlP);

/** Init function for SCH SMB host controller */
extern u_int32 SMB_SCH_Init(        SMB_DESC_SCH     *descP, OSS_HANDLE *osHdl, void **smbHdlP);
extern u_int32 SMB_SCH_IO_Init(     SMB_DESC_SCH     *descP, OSS_HANDLE *osHdl, void **smbHdlP);

extern u_int32 SMB_MGT5200_Init(    SMB_DESC_MGT5200 *descP, OSS_HANDLE *osHdl, void **smbHdlP );

/* exported by smb2_mgt5200.c: */
extern u_int32 SMB_MPC85XX_Init(    SMB_DESC_MPC85XX *descP, OSS_HANDLE *osHdl, void **smbHdlP);

u_int32 SMB_ALI1543_Init( SMB_DESC_ALI1543 *descP, OSS_HANDLE *osHdl, void **smbHdlP );

u_int32 SMB_PORTCB_Init(  SMB_DESC_PORTCB    *descP, OSS_HANDLE *osHdl, void **smbHdlP );
u_int32 SMB_PORTCB_HandleSize(void);
#endif /* SMB2_API */

#  ifdef __cplusplus
      }
#  endif

#endif/*_SMB2_H*/










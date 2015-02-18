/***********************  I n c l u d e  -  F i l e  ***********************/
/*!
 *        \file  fpga_load.h
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2014/01/17 17:18:24 $
 *    $Revision: 2.19 $
 *
 *       \brief  Header file for FPGA_LOAD tool containing:
 *               specific function prototypes,
 *               specific type definitions
 *
 *    \switches
 *
 *
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: fpga_load.h,v $
 * Revision 2.19  2014/01/17 17:18:24  awerner
 * R: 1. Merge error 2.17->2.18
 *    2. F75P uses 16MB SPI Flash
 * M: 1. Manuell merge 2.17->2.18
 *    2. expanded sector max. define to hold 256 sectors
 *
 * Revision 2.18  2013/06/21 16:43:14  MRoth
 * R: no support for Windows7 64bit
 * M: added Global Handle
 *
 * Revision 2.16  2009/01/12 16:58:31  CKauntz
 * R: 32 PCI Deviced are too few
 * M: Changed to 128 to show more than 32 PCI Devices
 *
 * Revision 2.15  2008/07/30 15:53:58  gvarlet
 * R: No support for QNX
 * M: Support for QNX added
 *
 * Revision 2.14  2008/05/09 19:26:43  CKauntz
 * Added interfacespi to switch between parallel and serial flash types
 *
 * Revision 2.13  2008/03/19 16:10:52  CKauntz
 * added:
 *   + ERR_FLASH_ERASE_FAILED
 *   + WB2SPI_FLASH_INTERFACE_ID
 * fixed: MAX_SECT for the AMD29GL064
 *
 * Revision 2.12  2007/08/07 14:01:55  JWu
 * changed:
 *   type of arguments in Z100_Mwrite*, Z100_*_sw
 *
 * Revision 2.11  2007/07/13 17:27:51  cs
 * cosmetics
 *
 * Revision 2.10  2007/07/09 20:10:42  CKauntz
 *  + changed: MAX_SECT, FILE_SIZE for new flash type
 *  + added :  Z100_IO function for swapped version
 *             Function pointer to Read/Write function in DEV_HDL
 *             Makros for Read/Write functions
 *
 * Revision 2.9  2006/02/08 12:07:18  cschuster
 * adde A500* macros (Bus switch setting)
 *
 * Revision 2.8  2005/12/12 11:04:47  cschuster
 * added structure _SMB_LOC_HDL
 *
 * Revision 2.7  2005/12/09 17:28:15  cschuster
 * Copyright string changed
 *
 * Revision 2.6  2005/11/24 18:24:36  cschuster
 * changed FLASH_READ/WRITE macros to Z100_FLASH_READ/WRITE functions
 *     added prototypes and swapped variant defines here
 * added support for am29lvxxx_smb (A404/A500)
 * added string.h to include files
 *
 * Revision 2.5  2005/07/08 17:49:40  cs
 * use MACCESS macros and chameleon (+oss_usr, +dbg_usr) library
 * add VME bus support
 * cosmetics
 *
 * Revision 2.4  2005/01/21 13:36:42  cs
 * added structure CHAM_ENT to device handle
 * added some error defines
 *
 * Revision 2.3  2004/12/23 15:10:36  cs
 * moved to TOOLS/FPGA_LOAD/COM directory
 * split into general, flash specific and native header file
 *
 * Revision 2.2  2004/12/13 18:03:41  cs
 * added DBGOUT define
 * cosmetics
 *
 * Revision 2.1  2004/11/30 18:05:02  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _FPGA_LOAD_H
#define _FPGA_LOAD_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  INCLUDES                                |
+-----------------------------------------*/
/**@{*/
/** OS Specific includes */
#if OS9
#	undef NULL
#	include <setjmp.h>
#	include <setsys.h>
#	include <stdio.h>
#	include <ctype.h>
#endif /* OS9 */

#if LINUX
#	undef NULL
#	include <stdio.h>
#	include <stdlib.h>
#	include <stdarg.h>
#	include <ctype.h>
#	include <string.h>
#endif /* LINUX */

#if VXWORKS
#	include <stdio.h>
#   include <stdlib.h>
#   include <stdarg.h>
#   include <ctype.h>
#	include <string.h>
#endif /* VXWORKS */

#if __QNX__
#	include <stdio.h>
#	include <stdlib.h>
#	include <stdarg.h>
#	include <ctype.h>
#	include <string.h>
#undef MAC_USERSPACE
#endif /* __QNX__ */

#if WINNT
#	include <windows.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <winioctl.h>
#	include <string.h>
#	include <setupapi.h>
#endif /* WINNT */

/**@}*/

#include <MEN/men_typs.h>
#include <MEN/mdis_err.h>
#include <MEN/fpga_header.h>
#include <MEN/dbg.h>
#include <MEN/chameleon.h>
#include <MEN/oss.h>
#include <MEN/smb2.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/usr_err.h>


/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/** \name  Defines */

#ifdef DBG
#define DBGOUT(_x_) printf _x_
#else
#define DBGOUT(_x_)
#endif /* DBG */


#define WB2FLASH_INTERFACE_ID 0x2D	  /* since we use CHAM_V2 functions we have
									   * to use this ID, until now (CHAM_V0/1) 0x1f */
#define WB2SPI_FLASH_INTERFACE_ID 0x7E /* Flash interface for serial flashes with SPI protocol */
#define WB2FLASH_INTERFACE_GROUP 0x00 /* for chamRev < 2 this value is
									   * returned fix as 0 and first interface is used
									   * for chamRev >= 2 only Group 0 is to be used
									   * to be found in chameleon table */
#define Z100_MAX_SECT		257 /* 136 *//* maximum numbers of sectors in flash device */
#define Z100_MAX_DEVICES 	128		/* maximum number of matching PCI devices */
#define Z100_MAX_FILE_SIZE 	0x800000 /* currently set to 8MB */

#define Z045_FLASH_ADDR_REG_OFFSET 0x00
#define Z045_FLASH_DATA_REG_OFFSET 0x04

/*--- boot sector position --*/
#define BOOT_SECT_NONE		0
#define BOOT_SECT_TOP       1
#define BOOT_SECT_BOTTOM	2

#define Z100_OS_ACCESS_READ  1
#define Z100_OS_ACCESS_WRITE 0

#define Z100_FLASH_ACCESS_16BIT 1
#define Z100_FLASH_ACCESS_8BIT  0

/*----- A500 Bus switches --- */
#define A500_BUS_SWITCH_OFFS  			  0x00000000 /* offset of bus switch byte
														in Flash (bytes) */
#define A500_RELEVANT_FLASH_CONTENT_SIZE  0x00040000 /* no sector erase possible!
														always read and restore
														all relevant data when
														setting bus switches
														has to be <= MAX_FILE_SIZE */

/* some macros */
#define LOG_TO_PHYS_ADDRREG(h) (h->pciDev.bar[h->bar] + h->addr_reg_offset)
#define LOG_TO_PHYS_DATAREG(h) (h->pciDev.bar[h->bar] + h->data_reg_offset)

#define MS2TICKS(ms)        (UOS_MsecTimerResolution() * ms)
#define MSECDIFF(basemsec)  (UOS_MsecTimerGet() - basemsec)

#define Z100_MREAD_D8 		h->Mread_D8
#define Z100_MREAD_D16 		h->Mread_D16
#define Z100_MREAD_D32		h->Mread_D32
#define Z100_MWRITE_D8 		h->Mwrite_D8
#define Z100_MWRITE_D16 	h->Mwrite_D16
#define Z100_MWRITE_D32 	h->Mwrite_D32
#define Z100_FLASH_WRITE	h->Write
#define Z100_FLASH_READ		h->Read

/*-----------------------------------------------------------------------
 * Macros to convert from big endian format 16/32 bit values
 *----------------------------------------------------------------------*/
#if defined(_LITTLE_ENDIAN_)
# define Z100_SWAP_BE32(dword)	OSS_SWAP32(dword)
# define Z100_SWAP_BE16(word)	OSS_SWAP16(word)
#elif defined(_BIG_ENDIAN_)

#define Z100_SWAP_BE32(dword) (dword)
#define Z100_SWAP_BE16(word) (word)

#else
# error "Define _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif /* _BIG/_LITTLE_ENDIAN_ */


/* error numbers */
#define ERR_FLASH_NOT_SUPPORTED				(ERR_UOS+0x10)
#define ERR_FLASH_VERIFICATION 				(ERR_UOS+0x11)
#define ERR_NO_SUPPORTED_FLASH_DEVICE_FOUND	(ERR_UOS+0x12)
#define ERR_FLASH_WRITE_FAILED				(ERR_UOS+0x13)
#define ERR_TRIED_TO_DELETE_BOOTSECTOR		(ERR_UOS+0x14)
#define ERR_Z100_ILL_CONFIG_FILE			(ERR_UOS+0x15)
#define ERR_Z100_CORRUPT_CONFIG_FILE		(ERR_UOS+0x16)
#define ERR_Z100_CHAMELEON_TABLE_NOT_FOUND	(ERR_UOS+0x17)
#define ERR_Z100_CHAMELEON_DEVICE_NOT_FOUND	(ERR_UOS+0x18)
#define ERR_FLASH_ERASE_FAILED				(ERR_UOS+0x19)


/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/**@{*/
/** Structure for holding flash memory device specific info */
typedef struct _FLASH_DEVS {
	u_int8 name[256];		/**< name of device */
	int32 manId;			/**< manufacturer Id */
	int32 devId;			/**< device Id */
	u_int32 sectSize;		/**< sector size */
	u_int32 nSectors;		/**< number of sectors in device */
	u_int8	bootSect;		/**<0: no bootsector,\n
								1: boot sector is top sector \n
								2: boot sector is bottom sector */
	u_int32 sectAddr[Z100_MAX_SECT]; /* sector addresses */
	struct _DEV_HDL *devHdl;	/**< pointer to parent device handle */
	u_int8 isInit;			/**< flag if flash info is read and verified */
} FLASH_DEVS;

/** Structure for holding pci device info */
typedef struct _PCI_DEVS {
	u_int8  bus;			/**< bus number */
	u_int8  dev;			/**< number of device on bus */
	u_int8  fun;			/**< number of function in device */
	u_int32 venId;			/**< vendor Id */
	u_int32 devId;			/**< device Id */
	u_int32 subSysVenId;	/**< sub system vendor Id */
	u_int32 bar[6];			/**< BAR0 - BAR5 */
	u_int8  comRegChanged;	/**< flag: command reg of device was changed */
	u_int16 origComReg;		/**< original command register */
} PCI_DEVS;

/** Structure for holding flash type specific function pointers */
typedef struct _FLASH_ENTRY {
	int32 (*Exit)(FLASH_DEVS*);
							/**< Standard Exit for FLASH */
	void  (*Reset)(FLASH_DEVS*);
							/**< Reset device */
	int32 (*EraseChip)(FLASH_DEVS*);
							/**< Erase entire device */
	int32 (*EraseSectors)(FLASH_DEVS*, u_int32, u_int32);
							/**< Erase sectors specified by len,
							 *   starting with sector containing startOffset */
	int32 (*WriteBlock)(FLASH_DEVS*, u_int32, u_int32, u_int8*);
							/**< Write block of data to flash */
	int32 (*ReadBlock)(FLASH_DEVS*, u_int32, u_int32, u_int8*);
							/**< Standard Exit for FLASH */
} FLASH_ENTRY;

/** structure for holding information about used SMBus */
typedef struct _SMB_LOC_HDL {
	char 	smbCtlName[16];		/** if given, this smb controller is
								 *  used for directly accessing the SMB
								 *  controller using the smb2 lib */
	u_int32 smbCtlNum;			/** if given, the SMB_HANDLE structure
								 *  fetched using OSS_GetSmbHdl() */
	void 	*smbHdl;			/** SMB_HANDLE structure to access the SMBus */
} SMB_LOC_HDL;

/** Structure for holding all device info */
typedef struct _DEV_HDL {
	struct _PCI_DEVS	pciDev;			/**< struct with PCI device info */
	CHAMELEONV2_INFO	chamInfo;		/**< chameleon table read from device */
	CHAMELEONV2_UNIT	chamUnit;		/**< chameleon table read from device */
	struct _FLASH_DEVS	flashDev;		/**< struct with flash info */
	int32				busType;		/**< OSS_BUSTYPE_NONE/VME/PCI/ISA */
	int32				mapType;		/**< OSS_ADDRSPACE_MEM/IO */
	void 				*physAddr;		/**< physical address */
	u_int8				*mappedAddr;	/**< physical address mapped to user space */
	u_int32				mappedSize;		/**< size of space mapped to user space*/
	struct _SMB_LOC_HDL smbLocHdl;		/**< evtl. info to access SMBus */
	u_int8 flash_acc_size;				/**< size of flash access data:
										     0:  8 bit data,
										     1: 16 bit data*/
	struct _FLASH_ENTRY	flash_entry;	/**< struct with flash type specific
										 function pointers */
 	u_int8  (*Mread_D8)(	void *ma,	u_int32 offs);	/**< Read 8Bit */
 	u_int16 (*Mread_D16)(	void *ma,	u_int32 offs);	/**< Read 16Bit */
	u_int32 (*Mread_D32)(	void *ma,	u_int32 offs);	/**< Read 32Bit */
	void (*Mwrite_D8)(	void *ma,	u_int32 offs,	u_int8 val);	/**< Write 8Bit */
 	void (*Mwrite_D16)(	void *ma,	u_int32 offs,	u_int16 val);	/**< Write 16Bit */
	void (*Mwrite_D32)(	void *ma,	u_int32 offs,	u_int32 val);	/**< Write 32Bit */

	u_int32 (*Read)( struct _DEV_HDL *h, u_int32 offs);  			/**< Read Value from Flash */
	void (*Write)( struct _DEV_HDL *h, u_int32 offs, u_int32 val);	/**< Writes Value to Flash */
	u_int8	            dbgLevel;       /**< debug level */
	u_int8              interfacespi;   /**< Serial flash interface */
} DEV_HDL;
/**@}*/
/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
#if WINNT
	extern HANDLE	*G_pHga;
#endif /* WINNT */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/

#ifdef Z100_IO_ACCESS_ENABLE
extern u_int8  Z100_Mread_Io_D8(  void *ma, u_int32 offs);
extern u_int16 Z100_Mread_Io_D16( void *ma, u_int32 offs);
extern u_int32 Z100_Mread_Io_D32( void *ma, u_int32 offs);

extern void Z100_Mwrite_Io_D8(  void *ma, u_int32 offs, u_int8 val);
extern void Z100_Mwrite_Io_D16( void *ma, u_int32 offs, u_int16 val);
extern void Z100_Mwrite_Io_D32( void *ma, u_int32 offs, u_int32 val);

/* Swapped IO functions */
extern u_int8  Z100_Mread_Io_D8_SW(  void *ma, u_int32 offs);
extern u_int16 Z100_Mread_Io_D16_SW( void *ma, u_int32 offs);
extern u_int32 Z100_Mread_Io_D32_SW( void *ma, u_int32 offs);

extern void Z100_Mwrite_Io_D8_SW(  void *ma, u_int32 offs, u_int8 val);
extern void Z100_Mwrite_Io_D16_SW( void *ma, u_int32 offs, u_int16 val);
extern void Z100_Mwrite_Io_D32_SW( void *ma, u_int32 offs, u_int32 val);
#endif /* Z100_IO_ACCESS_ENABLE */


#ifdef __cplusplus
      }
#endif

#endif /* _FPGA_LOAD_H */




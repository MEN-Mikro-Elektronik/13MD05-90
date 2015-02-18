/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  am29lvxxx.c
 *
 *       \author  Christian.Schuster@men.de
 *        $Date: 2009/01/12 16:58:33 $
 *    $Revision: 1.12 $
 *
 *        \brief  command set for AMD 29lv160 and compatible flashes
 *
 *
 *     Required: -
 *    \switches  (none)
 */
 /*---------------------------[ Public Functions ]----------------------------
 *
 *  FLASH_Try                Check if this command set can handle the device
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: am29lvxxx.c,v $
 * Revision 1.12  2009/01/12 16:58:33  CKauntz
 * R: 1. In non debugged version no flash name visible
 *    2. Write behind the flash size
 * M: 1.a: changed to printf to show name in verbose mode
 *      b: changed flash name to show size in MB
 *    2. No write or delete when address greater than flash size
 *       No write but delete of sectors when start within but end behind the size
 *
 * Revision 1.11  2008/12/19 15:19:40  CKauntz
 * Added support for AMD and MXIC 29LV320A/D Flash
 *
 * Revision 1.10  2008/03/19 16:11:03  CKauntz
 * added: support for the AMD29GL064 Flash
 * fixed: delete next sector of the end of the sector
 *
 * Revision 1.9  2007/07/09 20:10:58  CKauntz
 * added: function pointer to Read / Write function
 *        decision for 8Bit or 16Bit
 *
 * Revision 1.8  2006/02/07 19:34:43  cschuster
 * cosmetics
 *
 * Revision 1.7  2005/12/09 17:21:34  cschuster
 * Copyright string changed
 *
 * Revision 1.6  2005/11/24 18:20:51  cschuster
 * changed FLASH_READ/FLASH_WRITE macros to function calls Z100_FLASH_READ/WRITE
 * added casts to avoid warnings whith VxWorks compiler
 * cosmetics
 *
 * Revision 1.5  2005/07/08 17:49:48  cs
 * changed flash access size to the smallest possible value
 * (VME for example, would make 2 16bit reads out of 1 32 bit read)
 * cosmetics
 *
 * Revision 1.4  2005/01/31 13:58:16  cs
 * added casts to printf to avoid warnings when compiling VxWorks tool
 * changed verbose printf to DBGOUT
 *
 * Revision 1.3  2004/12/23 15:10:46  cs
 * moved prototypes to include file
 * cosmetics
 *
 * Revision 1.2  2004/12/13 18:03:33  cs
 * cosmetics for documentation and debug messages
 *
 * Revision 1.1  2004/11/30 18:04:57  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include "fpga_load.h"
#include "fpga_load_flash.h"
/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--- command codes ---*/
#define C_RESET				0xF0	/* read/reset command */
#define C_AUTOSELECT		0x90	/* autoselect */
#define C_PROGRAM			0xA0	/* byte program */
#define C_ERASE1			0x80	/* first command for
									   erase chip / sector erase */
#define C_ERASE2_CHIP		0x10	/* second command for erase chip */
#define C_ERASE2_SECTOR		0x30	/* second command for sector erase */

/*--- known manufacturer codes/device codes ---*/
#define MANUFACT_AMD		0x01
#define DEVCODE_29F010		0x20
#define DEVCODE_29F040		0xA4
#define DEVCODE_29F016		0xAD
#define DEVCODE_29LV040		0x4F
#define DEVCODE_29F080		0xD5
#define DEVCODE_29LV160DT	0x22C4
#define DEVCODE_29LV160DB	0x2249
#define DEVCODE_29LV008BT	0x3E
#define DEVCODE_29LV008BB	0x37
#define DEVCODE_29GL064A    0x227E    /* Model Number R4 Bottom Boot Sector
                                    has the same devID as R3 Top Boot Sector */
#define DEVCODE_29LV320DT   0x22F6
#define DEVCODE_29LV320DB   0x22F9

#define MANUFACT_ST 		0x20
#define DEVCODE_ST_29F040	0xE2

#define MANUFACT_MXIC       0xC2
#define DEVCODE_29LV320AT   0x22A7
#define DEVCODE_29LV320AB   0x22A8

/*--- erase states ---*/
#define STATE_IDLE			0
#define STATE_ERASING		1
#define STATE_ERASED		2

#if defined(_LITTLE_ENDIAN_)
# define SW16(dword) 	(dword)
#elif defined(_BIG_ENDIAN_)
# define SW16(dword) OSS_SWAP16(dword)
#else
# error "Define _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif /* _BIG/_LITTLE_ENDIAN_ */


/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/*
 * NOTE: addresses and sizes in the following struct are half the real size
 *       (everything is sized in words ( 16bit )).
 *
 *	{ name, manId, devId, sectSize, nSectors, bootSect, sectAddr[] }
 */
static FLASH_DEVS const G_Devs[] = {
	{ "AMD29LV160DT 2M",	MANUFACT_AMD,	DEVCODE_29LV160DT,	0x8000, 	35,	BOOT_SECT_TOP,
			{
				0x000000, 0x008000, 0x010000, 0x018000, 0x020000, 0x028000,
				0x030000, 0x038000, 0x040000, 0x048000, 0x050000, 0x058000,
				0x060000, 0x068000, 0x070000, 0x078000, 0x080000, 0x088000,
				0x090000, 0x098000, 0x0a0000, 0x0a8000, 0x0b0000, 0x0b8000,
				0x0c0000, 0x0c8000, 0x0d0000, 0x0d8000, 0x0e0000, 0x0e8000,
				0x0f0000, 0x0f8000, 0x0fc000, 0x0fd000, 0x0fe000, 0x100000
		  }
	},
	{ "AMD29LV160DB 2M",	MANUFACT_AMD,	DEVCODE_29LV160DB,	0x8000, 	35,	BOOT_SECT_BOTTOM,
			{
				0x000000, 0x002000, 0x003000, 0x004000, 0x008000, 0x010000,
				0x018000, 0x020000, 0x028000, 0x030000, 0x038000, 0x040000,
				0x048000, 0x050000, 0x058000, 0x060000, 0x068000, 0x070000,
				0x078000, 0x080000, 0x088000, 0x090000, 0x098000, 0x0a0000,
				0x0a8000, 0x0b0000, 0x0b8000, 0x0c0000, 0x0c8000, 0x0d0000,
				0x0d8000, 0x0e0000, 0x0e8000, 0x0f0000, 0x0f8000, 0x100000
			}
	},
	{ "AMD29LV008BT 1M",	MANUFACT_AMD,	DEVCODE_29LV008BT,	0x8000, 	19,	BOOT_SECT_TOP,
			{
				0x000000, 0x008000, 0x010000, 0x018000, 0x020000, 0x028000,
				0x030000, 0x038000, 0x040000, 0x048000, 0x050000, 0x058000,
				0x060000, 0x068000, 0x070000, 0x078000, 0x07C000, 0x07D000,
				0x07E000, 0x080000
		  }
	 },
	{ "AMD29LV008BB 1M",	MANUFACT_AMD,	DEVCODE_29LV008BB,	0x8000, 	19,	BOOT_SECT_BOTTOM,
			{
				0x000000, 0x002000, 0x003000, 0x004000, 0x008000, 0x010000,
				0x018000, 0x020000, 0x028000, 0x030000, 0x038000, 0x040000,
				0x048000, 0x050000, 0x058000, 0x060000, 0x068000, 0x070000,
				0x078000, 0x080000
			}
     },
     { "AMD29GL064A 8M",	MANUFACT_AMD,	DEVCODE_29GL064A,	0x10000, 	135, BOOT_SECT_BOTTOM,
			{ /* MODEL R4 Bottom Boot Sector */
                /*16 bit addressrange */
                0x00000000, 0x00001000, 0x00002000, 0x00003000, 0x00004000,
                0x00005000, 0x00006000, 0x00007000, 0x00008000, 0x00010000,
                0x00018000, 0x00020000, 0x00028000, 0x00030000, 0x00038000,
                0x00040000, 0x00048000, 0x00050000, 0x00058000, 0x00060000,
                0x00068000, 0x00070000, 0x00078000, 0x00080000, 0x00088000,
                0x00090000, 0x00098000, 0x000A0000, 0x000A8000, 0x000B0000,
                0x000B8000, 0x000C0000, 0x000C8000, 0x000D0000, 0x000D8000,
                0x000E0000, 0x000E8000, 0x000F0000, 0x000F8000, 0x00100000,
                0x00108000, 0x00110000, 0x00118000, 0x00120000, 0x00128000,
                0x00130000, 0x00138000, 0x00140000, 0x00148000, 0x00150000,
                0x00158000, 0x00160000, 0x00168000, 0x00170000, 0x00178000,
                0x00180000, 0x00188000, 0x00190000, 0x00198000, 0x001A0000,
                0x001A8000, 0x001B0000, 0x001B8000, 0x001C0000, 0x001C8000,
                0x001D0000, 0x001D8000, 0x001E0000, 0x001E8000, 0x001F0000,
                0x001F8000, 0x00200000, 0x00208000, 0x00210000, 0x00218000,
                0x00220000, 0x00228000, 0x00230000, 0x00238000, 0x00140000,
                0x00248000, 0x00250000, 0x00258000, 0x00260000, 0x00268000,
                0x00270000, 0x00278000, 0x00280000, 0x00288000, 0x00290000,
                0x00298000, 0x002A0000, 0x002A8000, 0x002B0000, 0x002B8000,
                0x002C0000, 0x002C8000, 0x002D0000, 0x002D8000, 0x002E0000,
                0x002E8000, 0x002F0000, 0x002F8000, 0x00300000, 0x00308000,
                0x00310000, 0x00318000, 0x00320000, 0x00328000, 0x00330000,
                0x00338000, 0x00340000, 0x00348000, 0x00350000, 0x00358000,
                0x00360000, 0x00368000, 0x00370000, 0x00378000, 0x00380000,
                0x00388000, 0x00390000, 0x00398000, 0x003A0000, 0x003A8000,
                0x003B0000, 0x003B8000, 0x003C0000, 0x003C8000, 0x003D0000,
                0x003D8000, 0x003E0000, 0x003E8000, 0x003F0000, 0x003F8000,
                0x00400000
            }
     },
     { "AMD29LV320DT 4M",	MANUFACT_AMD,	DEVCODE_29LV320DT,	0x8000, 	71, BOOT_SECT_TOP,
			{   /*16 bit addressrange */
                0x00000000, 0x00008000, 0x00010000, 0x00018000, 0x00020000,
                0x00028000, 0x00030000, 0x00038000, 0x00040000, 0x00048000,
                0x00050000, 0x00058000, 0x00060000, 0x00068000, 0x00070000,
                0x00078000, 0x00080000, 0x00088000, 0x00090000, 0x00098000,
                0x000A0000, 0x000A8000, 0x000B0000, 0x000B8000, 0x000C0000,
                0x000C8000, 0x000D0000, 0x000D8000, 0x000E0000, 0x000E8000,
                0x000F0000, 0x000F8000, 0x00100000, 0x00108000, 0x00110000,
                0x00118000, 0x00120000, 0x00128000, 0x00130000, 0x00138000,
                0x00140000, 0x00148000, 0x00150000, 0x00158000, 0x00160000,
                0x00168000, 0x00170000, 0x00178000, 0x00180000, 0x00188000,
                0x00190000, 0x00198000, 0x001A0000, 0x001A8000, 0x001B0000,
                0x001B8000, 0x001C0000, 0x001C8000, 0x001D0000, 0x001D8000,
                0x001E0000, 0x001E8000, 0x001F0000, 0x001F8000, 0x001F9000,
                0x001FA000, 0x001FB000, 0x001FC000, 0x001FD000, 0x001FE000,
                0x001FF000, 0x00200000
            }
     },
      { "AMD29LV320DB 4M",	MANUFACT_AMD,	DEVCODE_29LV320DB,	0x8000, 	71, BOOT_SECT_BOTTOM,
			{   /*16 bit addressrange */
                0x00000000, 0x00001000, 0x00002000, 0x00003000, 0x00004000,
                0x00005000, 0x00006000, 0x00007000, 0x00008000, 0x00010000,
                0x00018000, 0x00020000, 0x00028000, 0x00030000, 0x00038000,
                0x00040000, 0x00048000, 0x00050000, 0x00058000, 0x00060000,
                0x00068000, 0x00070000, 0x00078000, 0x00080000, 0x00088000,
                0x00090000, 0x00098000, 0x000A0000, 0x000A8000, 0x000B0000,
                0x000B8000, 0x000C0000, 0x000C8000, 0x000D0000, 0x000D8000,
                0x000E0000, 0x000E8000, 0x000F0000, 0x000F8000, 0x00100000,
                0x00108000, 0x00110000, 0x00118000, 0x00120000, 0x00128000,
                0x00130000, 0x00138000, 0x00140000, 0x00148000, 0x00150000,
                0x00158000, 0x00160000, 0x00168000, 0x00170000, 0x00178000,
                0x00180000, 0x00188000, 0x00190000, 0x00198000, 0x001A0000,
                0x001A8000, 0x001B0000, 0x001B8000, 0x001C0000, 0x001C8000,
                0x001D0000, 0x001D8000, 0x001E0000, 0x001E8000, 0x001F0000,
                0x001F8000, 0x00200000
            }
     },
      { "MXIC29LV320AT 4M",	MANUFACT_MXIC,	DEVCODE_29LV320AT,	0x8000, 	71, BOOT_SECT_TOP,
			{   /*16 bit addressrange */
                0x00000000, 0x00008000, 0x00010000, 0x00018000, 0x00020000,
                0x00028000, 0x00030000, 0x00038000, 0x00040000, 0x00048000,
                0x00050000, 0x00058000, 0x00060000, 0x00068000, 0x00070000,
                0x00078000, 0x00080000, 0x00088000, 0x00090000, 0x00098000,
                0x000A0000, 0x000A8000, 0x000B0000, 0x000B8000, 0x000C0000,
                0x000C8000, 0x000D0000, 0x000D8000, 0x000E0000, 0x000E8000,
                0x000F0000, 0x000F8000, 0x00100000, 0x00108000, 0x00110000,
                0x00118000, 0x00120000, 0x00128000, 0x00130000, 0x00138000,
                0x00140000, 0x00148000, 0x00150000, 0x00158000, 0x00160000,
                0x00168000, 0x00170000, 0x00178000, 0x00180000, 0x00188000,
                0x00190000, 0x00198000, 0x001A0000, 0x001A8000, 0x001B0000,
                0x001B8000, 0x001C0000, 0x001C8000, 0x001D0000, 0x001D8000,
                0x001E0000, 0x001E8000, 0x001F0000, 0x001F8000, 0x001F9000,
                0x001FA000, 0x001FB000, 0x001FC000, 0x001FD000, 0x001FE000,
                0x001FF000, 0x00200000
            }
     },
      { "MXIC29LV320AB 4M",	MANUFACT_MXIC,	DEVCODE_29LV320AB,	0x8000, 	71, BOOT_SECT_BOTTOM,
			{   /*16 bit addressrange */
                0x00000000, 0x00001000, 0x00002000, 0x00003000, 0x00004000,
                0x00005000, 0x00006000, 0x00007000, 0x00008000, 0x00010000,
                0x00018000, 0x00020000, 0x00028000, 0x00030000, 0x00038000,
                0x00040000, 0x00048000, 0x00050000, 0x00058000, 0x00060000,
                0x00068000, 0x00070000, 0x00078000, 0x00080000, 0x00088000,
                0x00090000, 0x00098000, 0x000A0000, 0x000A8000, 0x000B0000,
                0x000B8000, 0x000C0000, 0x000C8000, 0x000D0000, 0x000D8000,
                0x000E0000, 0x000E8000, 0x000F0000, 0x000F8000, 0x00100000,
                0x00108000, 0x00110000, 0x00118000, 0x00120000, 0x00128000,
                0x00130000, 0x00138000, 0x00140000, 0x00148000, 0x00150000,
                0x00158000, 0x00160000, 0x00168000, 0x00170000, 0x00178000,
                0x00180000, 0x00188000, 0x00190000, 0x00198000, 0x001A0000,
                0x001A8000, 0x001B0000, 0x001B8000, 0x001C0000, 0x001C8000,
                0x001D0000, 0x001D8000, 0x001E0000, 0x001E8000, 0x001F0000,
                0x001F8000, 0x00200000
            }
     },
	{ "", 0 }
};

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
/*
static u_int32 DetectSize( MTD_HANDLE *h, u_int32 maxSize );*/
static int32 Init( DEV_HDL *h );
static void Identify( DEV_HDL *h );
static const FLASH_DEVS *CheckId( DEV_HDL *h );
static int32 Exit( FLASH_DEVS *fDev );
static void Reset( FLASH_DEVS *fDev );
static int32 EraseChip( FLASH_DEVS *fDev );
static int32 EraseSectors( FLASH_DEVS *fDev,
						  u_int32 startOffs,
						  u_int32 len );
static int32 WriteBlock( FLASH_DEVS *fDev,
						 u_int32 offset,
						 u_int32 len,
						 u_int8 *buf );
static int32 DataPollingAlg( DEV_HDL *h,
							u_int32 offset,
							u_int32 val,
							u_int32 tout);
static int32 FindSect( FLASH_DEVS *h, u_int32 phyAddr, u_int32 *phySizeP );


static void Command( DEV_HDL *h, int cycles, u_int8 cmd );


/******************************* am29lvxxx_Try ********************************/
/** Check if this Flash command set can handle the device.
 *
 *	This is the only public function of a Flash. It gets called from the
 *  fpga_load::init_flash function.
 *
 *
 *  If the Flash command set supports the device,
 *  it returns a pointer to the init function
 *
 *---------------------------------------------------------------------------
 *  \param	devHdl		\IN		DEV_HDL handle
 *	\param	flash_initP	\OUT	pointer to function ptr where init function
 *								will be stored
 *  \param	dbgLevel	\IN		Debug level to be used in try function
 *  \return	success (0) or error code
 ******************************************************************************/
extern int32 Z100_AM29LVXXX_TRY(DEV_HDL *devHdl,
								FLASH_INITP *flash_initP,
								u_int32 dbgLevel)
{
	int32 error=0;
	FLASH_DEVS fDev;

	DBGOUT(( "FLASH::am29lvxxx::Try\n" ));


	if(devHdl->mapType){
#ifdef Z100_IO_ACCESS_ENABLE
		#ifdef MAC_BYTESWAP
			devHdl->Mread_D8   = Z100_Mread_Io_D8_Sw;
			devHdl->Mread_D16  = Z100_Mread_Io_D16_Sw;
			devHdl->Mread_D32  = Z100_Mread_Io_D32_Sw;
			devHdl->Mwrite_D8  = Z100_Mwrite_Io_D8_Sw;
			devHdl->Mwrite_D16 = Z100_Mwrite_Io_D16_Sw;
			devHdl->Mwrite_D32 = Z100_Mwrite_Io_D32_Sw;
		#else
			devHdl->Mread_D8   = Z100_Mread_Io_D8;
			devHdl->Mread_D16  = Z100_Mread_Io_D16;
			devHdl->Mread_D32  = Z100_Mread_Io_D32;
			devHdl->Mwrite_D8  = Z100_Mwrite_Io_D8;
			devHdl->Mwrite_D16 = Z100_Mwrite_Io_D16;
			devHdl->Mwrite_D32 = Z100_Mwrite_Io_D32;
		#endif
	} else {
#else
		printf( " IO mapped access not supported\n");
	}
#endif /* Z100_IO_ACCESS_ENABLE */

	/*--- get read / write function pointer ---*/
	devHdl->Mread_D8   = Z100_Mread_Mem_D8;
	devHdl->Mread_D16  = Z100_Mread_Mem_D16;
	devHdl->Mread_D32  = Z100_Mread_Mem_D32;
	devHdl->Mwrite_D8  = Z100_Mwrite_Mem_D8;
	devHdl->Mwrite_D16 = Z100_Mwrite_Mem_D16;
	devHdl->Mwrite_D32 = Z100_Mwrite_Mem_D32;
#ifdef Z100_IO_ACCESS_ENABLE
	}
#endif /* Z100_IO_ACCESS_ENABLE */

	devHdl->Read  = Z100_Flash_Read;
	devHdl->Write = Z100_Flash_Write;

	/* get device identification from flash */
	Identify( devHdl );

	if( CheckId( devHdl ) == NULL )
		error = ERR_FLASH_NOT_SUPPORTED;

	DBGOUT(( "FLASH::am29lvxxx::Try\n"
			"    manId = 0x%02x, devId = 0x%04x, "
			"sectS = %d, SectNum = %d, bootSect = %d\n",
			(int)devHdl->flashDev.manId, (int)devHdl->flashDev.devId,
			(int)devHdl->flashDev.sectSize, (int)devHdl->flashDev.nSectors,
			(int)devHdl->flashDev.bootSect));

	if( error )
		fDev.manId = 0;
	*flash_initP = NULL;
	if( error == 0){
		*flash_initP = &Init;
	}

	DBGOUT(("FLASH::a29lvxxx::Try finished 0x%04x\n", (int)error));
	return error;
}



/********************************* Init **************************************/
/** Initialize the function pointers to this Flash's command set.
 *
 *	Called by the Flash_Init function.
 *
 *---------------------------------------------------------------------------
 *  \param	h		\IN		DEV_HDL handle
 *  \return	success (0) or error code
 *  \return	*handleP	filled with handle
 ****************************************************************************/
static int32 Init( DEV_HDL *h )
{
	FLASH_ENTRY *ent = &h->flash_entry;

	ent->Exit 		= &Exit;
	ent->Reset 		= &Reset;
	ent->ReadBlock 	= &Z100_FLASH_READ_BLOCK;
	ent->WriteBlock	= &WriteBlock;
	ent->EraseChip  = &EraseChip;
	ent->EraseSectors= &EraseSectors;
	h->flashDev.isInit = TRUE;

	Reset(&h->flashDev);				/* reset device to read mode */
	return 0;
}

/************************************ Exit **********************************/
/** Standard Exit for Flash.
 *
 *---------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \return	success (0) or error code
 ****************************************************************************/
static int32 Exit( FLASH_DEVS *fDev)
{
	DBGOUT(( "FLASH::am29lvxxx::Exit\n" ));

	Reset(fDev);				/* reset device to read mode */

	fDev->devHdl->flashDev.isInit = FALSE;
	return 0;
}

/*----------------------------------------------------------------------
 * INTERNAL FUNCTIONS
 *---------------------------------------------------------------------*/
/********************************* Identify ***********************************/
/** Issue autoselect command at specified phyAddr.
 *
 *  \remark Device will be kept in READ_ID mode when function finished.
 *
 *---------------------------------------------------------------------------
 *  \param	devHdl		\IN		DEV_HDL handle
 ******************************************************************************/
static void Identify( DEV_HDL *devHdl )
{
	u_int32 retval;
	u_int32 addrreg=0;
	DEV_HDL *h = devHdl;

	/* get data size of flash access operations, (addr reg, bit 30) */
	addrreg = Z100_MREAD_D32( h->mappedAddr, Z045_FLASH_ADDR_REG_OFFSET);

	if(   ((addrreg & 0x40000000) >> 30)  || ((addrreg & 0x00400000) >> 22)
		||((addrreg & 0x00004000) >> 14)  || ((addrreg & 0x00000040) >> 6) ){
		h->flash_acc_size = Z100_FLASH_ACCESS_16BIT;
	}
	else{
		h->flash_acc_size = Z100_FLASH_ACCESS_8BIT;
	}

	DBGOUT(( "FLASH::am29lvxxx::Identify addr_reg = 0x%08x; acc_size: %s\n",
	 (int)addrreg, h->flash_acc_size ? "16bit" : "8bit"));

	Command( devHdl, 3, C_RESET );		/* reset device */

	Command( devHdl, 3, C_AUTOSELECT );	/* issue autoselect command */


	if( devHdl->flash_acc_size == Z100_FLASH_ACCESS_16BIT ) {
		retval = Z100_FLASH_READ( devHdl, 0x0 );
		devHdl->flashDev.manId = retval & 0xff;
		retval = Z100_FLASH_READ( devHdl, 0x1 );
		devHdl->flashDev.devId = retval & 0xffff;
	} else {
		retval = Z100_FLASH_READ( devHdl, 0x0 );
		devHdl->flashDev.manId = retval & 0xff;
		retval = Z100_FLASH_READ( devHdl, 0x2 );
		devHdl->flashDev.devId = retval & 0xff;
	}

    if ((devHdl->flashDev.manId != MANUFACT_ST)  &&
        (devHdl->flashDev.manId != MANUFACT_AMD) &&
        (devHdl->flashDev.manId != MANUFACT_MXIC))
    {
        /* retry with the 8Bit command versions */
        h->flash_acc_size = Z100_FLASH_ACCESS_8BIT;

        Command( devHdl, 3, C_RESET );		/* reset device */
       	Command( devHdl, 3, C_AUTOSELECT );	/* issue autoselect command */

        if( devHdl->flash_acc_size == Z100_FLASH_ACCESS_16BIT ) {
    		retval = Z100_FLASH_READ( devHdl, 0x0 );
    		devHdl->flashDev.manId = retval & 0xff;
    		retval = Z100_FLASH_READ( devHdl, 0x1 );
    		devHdl->flashDev.devId = retval & 0xffff;
    	} else {
			/* set the read macros to 16 bit */
			h->flash_acc_size = Z100_FLASH_ACCESS_16BIT;
    		retval = Z100_FLASH_READ( devHdl, 0x0 );
    		devHdl->flashDev.manId = retval & 0xff;
    		retval = Z100_FLASH_READ( devHdl, 0x2 );
    		devHdl->flashDev.devId = retval & 0xffff;
    	}
    }


	if( devHdl->dbgLevel )
		printf( "FLASH::am29lvxxx::Identify mnf=%02x dev=%02x\n",
				(unsigned int)devHdl->flashDev.manId, (int)devHdl->flashDev.devId);

}

/*********************************** CheckId **********************************/
/** Check if our Flash command set can handle specified Flash.
 *
 * ---------------------------------------------------------------------------
 *  \param	devHdl		\IN		DEV_HDL handle
 *  \return		ptr to entry into FLASH_DEVS or NULL if not found
 *  Globals....: -
 ******************************************************************************/
static const FLASH_DEVS *CheckId( DEV_HDL *devHdl )
{
	DBGOUT(( "FLASH::am29lvxxx::CheckId mnf=%02x dev=%02x\n",
			(unsigned int)devHdl->flashDev.manId,
			(int)devHdl->flashDev.devId) );

	/*--- check if device supported ---*/
	if( devHdl->flashDev.manId == MANUFACT_AMD ||
		devHdl->flashDev.manId == MANUFACT_ST ||
		devHdl->flashDev.manId == MANUFACT_MXIC )
	{
		const FLASH_DEVS *d = G_Devs;
		int32 i;

		while( d->devId ){
			if( ( (devHdl->flash_acc_size == Z100_FLASH_ACCESS_16BIT &&
				   devHdl->flashDev.devId == d->devId) ||
			 	  (devHdl->flash_acc_size == Z100_FLASH_ACCESS_8BIT &&
			 	   devHdl->flashDev.devId == (d->devId & 0xff)) ) &&
			 	(devHdl->flashDev.manId == d->manId) )
			{
				if(devHdl->dbgLevel)
					printf("FLASH::am29lvxxx::CheckId\n"
						 "    Found FLASH device %s\n", d->name );

				devHdl->flashDev.sectSize = d->sectSize;
				devHdl->flashDev.nSectors = d->nSectors;
				devHdl->flashDev.bootSect = d->bootSect;
				for( i=0; i<Z100_MAX_SECT; i++)
					devHdl->flashDev.sectAddr[i] = d->sectAddr[i];
				devHdl->flashDev.devHdl   = devHdl;
				devHdl->flashDev.isInit  = 0;
				return d;
			}
			d++;
		}
	}
	DBGOUT(( "FLASH::am29lvxxx::CheckId finished, no supported dev\n"));
	return NULL;
}

/********************************* EraseChip **********************************/
/** Erase entire Flash device.
 *
 *---------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \return	success (0) or error code
 ******************************************************************************/
static int32 EraseChip( FLASH_DEVS *fDev )
{
	u_int32 offs;
	int32 error = 0;
	DEV_HDL *h = fDev->devHdl;

	DBGOUT(( "FLASH::am29lvxxx::EraseChip\n" ));

	if( fDev->devHdl->flash_acc_size ) { /* access 16 bit data bus */
		offs 	= 0x555;
	} else {
		offs 	= 0xaaa;
	}
	if (fDev->devHdl->flashDev.devId == DEVCODE_29GL064A){
	    offs = 0xaaa;
	}

	/*--- give CHIP ERASE command ---*/
	Command( fDev->devHdl, 5, C_ERASE1 );
	Z100_FLASH_WRITE( fDev->devHdl, offs, C_ERASE2_CHIP);

	error = DataPollingAlg(fDev->devHdl, 0, 0xFF, 50000);

	if( error )
		Reset(fDev);

	DBGOUT(( "FLASH::am29lvxxx::EraseChip finished 0x%04x\n", (int)error ));
	return error;
}

/********************************** EraseSectors ******************************/
/** Erase sectors specified by len, starting with sector containing startOffset.
 *
 *---------------------------------------------------------------------------
 *  \param	fDev		\IN		FLASH_DEVS handle
 *  \param	startOffs	\IN		address in first sector to be erased
 *  \param	len			\IN		length (in bytes) to be erased
 *  \return	success (0) or error code
*******************************************************************************/
static int32 EraseSectors( FLASH_DEVS *fDev, u_int32 startOffs, u_int32 len )
{
	u_int32 phyStart, phyEnd, sectSize;
	u_int32  sect=0;
	int32   error = 0;
	DEV_HDL *h = fDev->devHdl;
	u_int32 topBootSect;

	DBGOUT(( "FLASH::am29lvxxx::EraseSectors\n" ));

	topBootSect = (fDev->bootSect == BOOT_SECT_TOP) ? 1 : 0;
	/*--- check start address ---*/
	if(startOffs >= fDev->sectAddr[fDev->nSectors]*2){
		printf("*** Start address out of range!\n");
		error = ERR_FLASH_ERASE_FAILED;
		goto end;
	}
	/*--- limit to device size ---*/
	if( (startOffs + len) >= fDev->sectAddr[fDev->nSectors]*2 )
		len = fDev->sectAddr[fDev->nSectors]*2 - startOffs - 1;

	phyStart = startOffs;
	phyEnd   = startOffs + len -1;

	/*--- convert to sector boundaries ---*/
	sect = FindSect( fDev, phyStart, &sectSize );
	phyStart &= ~(sectSize-1);

	FindSect( fDev, phyEnd, &sectSize );
	phyEnd &= ~(sectSize-1);

	DBGOUT(( "FLASH::am29lvxxx::EraseSectors\n"
			 "    addr= 0x%06x, len= 0x%06x, startOffs= 0x%06x,"
			 " endOffs= 0x%06x, sSize= 0x%06x\n",
			 (int)startOffs, (int)len, (int)phyStart,
			 (int)phyEnd, (int)sectSize ));

	/* delete sectors */
	do {
		sect = FindSect( fDev, phyStart, &sectSize );
		if( fDev->devHdl->dbgLevel )
			printf( "Erasing sector %2d of %2d, size = 0x%06x\n",
					(int)sect+1, (int)fDev->nSectors, (unsigned int)sectSize);
		/*--- give CHIP ERASE command ---*/
		Command( fDev->devHdl, 5, C_ERASE1 );
		Z100_FLASH_WRITE( fDev->devHdl, phyStart, C_ERASE2_SECTOR);

		error = DataPollingAlg(fDev->devHdl, phyStart, 0xFF, 10000);
		if( error )
			break;

		if( fDev->devHdl->dbgLevel )
			printf("            ----> OK\n");
		if( phyStart == phyEnd )
			break;
		phyStart += sectSize;
	} while(1);
end:
	return error;
}

/*********************************** WriteBlock *******************************/
/** Write block of data to Flash.
 *
 *---------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \param	offs	\IN		offset in flash
 *  \param	len		\IN		length (in bytes) to be written
 *  \param	buf		\IN		buffer with data to be written
 *
 *  \return	success (0) or error code
 ******************************************************************************/
static int32 WriteBlock(
    FLASH_DEVS *fDev,
	u_int32 offs,
    u_int32 len,
    u_int8  *buf)
{
	u_int32 val;
	u_int16 *bufp16 = NULL;
	u_int8  *bufp8 = NULL;
	int32 error=0, nWrite;
	u_int32 offs1, offs2, offset = offs;
	DEV_HDL *h = fDev->devHdl;

	DBGOUT(( "FLASH::am29lvxxx::WriteBlock offs=0x%08x len=%d\n",
			 (int)offs, (int)len ));

    /* check write behind the end of the flash */
    if( ( offs >= fDev->sectAddr[fDev->nSectors]*2) ||
        ((offs + len) > fDev->sectAddr[fDev->nSectors]*2))
    {
        printf( "*** Write behind the flash memory!\n");
        error = ERR_FLASH_WRITE_FAILED;
        goto end;
    }

	if( h->flash_acc_size ) { /* access 16 bit data bus */
		nWrite = len / 2 + len%2;
		bufp16 = (u_int16*)buf;
		offs1 	= 0x555;
		offs2 	= 0x2aa;
	} else {
		nWrite = len;
		bufp8 = (u_int8*)buf;
		offs1 	= 0xaaa;
		offs2 	= 0x555;
	}
	if (fDev->devHdl->flashDev.devId == DEVCODE_29GL064A){
	    nWrite = len / 2 + len%2;
		bufp16 = (u_int16*)buf;
	    offs1 	= 0xaaa;
		offs2 	= 0x555;
	}

	while( nWrite ){

		/*--- issue command cycles ---*/
		Z100_FLASH_WRITE( h, offs1, 0xaa);			/* 1st cycle */
		Z100_FLASH_WRITE( h, offs2, 0x55);			/* 2nd cycle */
		Z100_FLASH_WRITE( h, offs1, C_PROGRAM);		/* 3rd cycle */

		if( h->flash_acc_size ) /* access 16 bit data bus */
		{
			val = *bufp16++;
		} else {
			val = *bufp8++;
		}
		if(fDev->devHdl->flashDev.devId == DEVCODE_29GL064A)
		    val = SW16((u_int16)val);

		Z100_FLASH_WRITE( h, offset, val);		/* 4th cycle */

/*		DBGOUT(( "FLASH::am29lvxxx::WriteBlock\n"
				 "    nWrite = %4d, offset: 0x%08x, written value: 0x%04x\n",
				 (int)nWrite, (int)offset, (int)val ));
*/

		if( (error = DataPollingAlg(h, offset, val, 10000)) )
			goto end;

		if (h->flash_acc_size )/* access 16 bit data bus */
		{
			nWrite--;
			offset+=2;
		} else {
			nWrite--;
			offset++;
		}
	}

end:
	if( error )
		Reset(fDev);

	DBGOUT(( "FLASH::am29lvxxx::WriteBlock finished 0x%04x\n", (int)error ));
	return error;
}


/*********************************** Reset ************************************/
/** Reset Flash device.
 *
 *---------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 ******************************************************************************/
static void Reset( FLASH_DEVS *fDev )
{
		Command( fDev->devHdl, 3, C_RESET );
}


/********************************* Command ************************************/
/** Issue command start pattern.
 *
 *	for a 4-cycle command, issues the first 3 cycles\n
 *  for a 6-cycle command, issues the first 5 cycles
 *
 *---------------------------------------------------------------------------
 *  \param	h		\IN		DEV_HDL handle
 *  \param	cycles	\IN		number of command cycles (3/5)
 *  \param	cmd		\IN		command to be issued
 ******************************************************************************/
static void Command(
    DEV_HDL *h,
    int cycles,
    u_int8 cmd)
{
	u_int32 offs1, offs2;

	if(h->dbgLevel)
	    DBGOUT(( "FLASH::am29lvxxx::Command acc_size= %d\n",
			(int)h->flash_acc_size ));

	if( h->flash_acc_size ) { /* access 16 bit data bus */
		offs1 	= 0x555;
		offs2 	= 0x2aa;
	} else {
		offs1 	= 0xaaa;
		offs2 	= 0x555;
	}
	if (h->flashDev.devId == DEVCODE_29GL064A){
	    offs1   = 0xaaa;
	    offs2 	= 0x555;
	}

	/*--- issue command cycles ---*/
	Z100_FLASH_WRITE( h, offs1, 0xaa);		/* 1st cycle */
	Z100_FLASH_WRITE( h, offs2, 0x55);		/* 2nd cycle */
	Z100_FLASH_WRITE( h, offs1, cmd );		/* 3rd cycle */
	if( cycles > 3 ){
		Z100_FLASH_WRITE( h, offs1, 0xaa); 	/* 4th cycle */
		Z100_FLASH_WRITE( h, offs2, 0x55); 	/* 5th cycle */
	}

}

/********************************* DataPollingAlg *****************************/
/** Perform Data Polling Algorithm (after write/erase commands)
 *
 *---------------------------------------------------------------------------
 *  \param	h		\IN		DEV_HDL handle
 *  \param	offset	\IN		offset of data to check
 *  \param	val		\IN		value programmed to offset
 *  \param	tout	\IN		time out in msec
 *
 *  \return	success (0) or error code
 ******************************************************************************/
static int32 DataPollingAlg(
    DEV_HDL *h,
    u_int32 offset,
    u_int32 val,
    u_int32 tout)
{
	u_int32 retVal = 0;
	u_int32 baseMsec, toutMsecs = tout;
	int32 error = 0;

	baseMsec = UOS_MsecTimerGet();


	while(1){

		if(h->flash_acc_size) /* access 16 bit data bus */
			retVal = Z100_MREAD_D16( h->mappedAddr,
									 Z045_FLASH_DATA_REG_OFFSET);
		else				/* access 8 bit data bus */
			retVal = Z100_MREAD_D8( h->mappedAddr,
									Z045_FLASH_DATA_REG_OFFSET);


		/*--- check state in DQ7 ---*/
		if( (retVal & 0x80) == (val & 0x80)){
			/*--- ready ---*/
			break;
		} else if( retVal & 0x20 ){ /*--- check state in DQ5 (timeout) ---*/
			/*--- recheck DQ7 ---*/
			u_int32 retVal2 = 0x00;
			if(h->flash_acc_size)/* access 16 bit data bus */
			{
			    retVal2 = Z100_MREAD_D16( h->mappedAddr,
										  Z045_FLASH_DATA_REG_OFFSET);
			}
			else				/* access 8 bit data bus */
				retVal2 = Z100_MREAD_D8( h->mappedAddr,
										 Z045_FLASH_DATA_REG_OFFSET);

			if( (retVal2 & 0x80) == (val & 0x80)){
				/*--- ready ---*/
				break;
			} else {
				error = ERR_FLASH_WRITE_FAILED;
				goto end;
		}
		}

		if( MSECDIFF(baseMsec) > toutMsecs){
			printf( "*** FLASH::Program Timeout\n");
			error = ERR_UOS_TIMEOUT;
			goto end;
		}
	}

end:
	return(error);
}

/********************************* FindSect ***********************************/
/** Convert dev address to sector number
 *
 *---------------------------------------------------------------------------
 *  \param	fDev		\IN		FLASH_DEVS handle
 *  \param	phyAddr 	\IN		phys address
 *  \param	phySizeP	\OUT	ptr to var where phys size of sector is stored
 *
 *  \return	sector number or error (-1) if not found
 ******************************************************************************/
static int32 FindSect( FLASH_DEVS *fDev, u_int32 phyAddr, u_int32 *phySizeP )
{
	u_int32 addr = phyAddr;
	u_int32 sect;
	addr &= (fDev->sectAddr[fDev->nSectors]*2 - 1);

	for( sect=0; sect < fDev->nSectors; sect++ ){
		if( addr >= fDev->sectAddr[sect]*2 &&
			addr < fDev->sectAddr[sect+1]*2){
			*phySizeP = (fDev->sectAddr[sect+1] - fDev->sectAddr[sect])*2;
			return sect;
		}
	}
	return -1;
}



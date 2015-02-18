/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  stm25p30.c
 *
 *       \author  Christian.Kauntz@men.de
 *        $Date: 2014/01/17 17:05:30 $
 *    $Revision: 2.12 $
 *
 *        \brief  command set for STMikroelektronics M25P32 flashes
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
 * $Log: stm25p32.c,v $
 * Revision 2.12  2014/01/17 17:05:30  awerner
 * R: Merge error in 2.10 to 2.11 changes lost.
 * M: Re Checkin 2.10
 *
 * Revision 2.10  2013/05/13 11:21:10  ts
 * R: programming Flash on F75P failed at offset 0x80000
 * M: fixed typo in 16M Flash array, caused wrong size/sector to be calculated
 *
 * Revision 2.9  2013/04/17 19:02:07  ts
 * R: fpga_load didnt work on F75P with larger SPI flash
 * M: added support of 128Mbit device SSTF016P
 *
 * Revision 2.8  2012/03/14 13:26:22  dpfeuffer
 * R: Windows compiler warning
 * M: cast added
 *
 * Revision 2.7  2010/08/26 09:57:03  CKauntz
 * R:1. Flash update took very long on LINUX, UOS_MikroDelayInit took 600ms
 *   2. No write status
 * M:1. Moved UOS_MikroDelayIni to the ini routine
 *   2. Added current write status in verbose mode
 *
 * Revision 2.6  2010/05/17 16:55:18  CKauntz
 * R: Last byte in flash not programmed
 * M: Removed redundant check for length against the flash size
 *
 * Revision 2.5  2009/01/12 16:58:39  CKauntz
 * R: 1. In non debugged version no flash name visible
 *    2. Write behind the flash size
 * M: 1.a: changed to printf to show name in verbose mode
 *      b: changed flash name to show size in MB
 *    2. No write or delete when address greater than flash size
 *       No write but delete of sectors when start within but end behind the size
 *
 * Revision 2.4  2008/12/19 15:19:42  CKauntz
 * Added support for STMICROM25P64 Flash
 *
 * Revision 2.3  2008/06/19 17:10:13  CKauntz
 * R: WriteBlock writes too fast at F607
 * M: Delay at DataPolling
 * R: ReadBlock reads wrong byte when length ends with 1, 2 or 3 bytes
 * M: Fixed assignment to buffer
 *
 * Revision 2.2  2008/05/09 19:26:50  CKauntz
 * Fixed: + Flash suppoort up to 4MB
 *        + Read and write at all offsets and length
 *
 * Revision 2.1  2008/03/19 16:11:08  CKauntz
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include "fpga_load.h"
#include "fpga_load_flash.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#if defined(_LITTLE_ENDIAN_)
# define SW16(dword) 	(dword)
# define SW32(lword)    (lword)
#elif defined(_BIG_ENDIAN_)
# define SW16(dword) OSS_SWAP16(dword)
# define SW32(lword) OSS_SWAP32(lword)
#else
# error "Define _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif /* _BIG/_LITTLE_ENDIAN_ */


/*--- bit fields for the commands at address lines > bitnbr 24  ---*/
/*--- for the altasmi_parallel Megafunctions ---*/
#define BIT_BULKERASE       0x20000000  /* bit bulk erase */
#define BIT_READSID         0x02000000  /* bit read serial id */
#define BIT_WRITE           0x40000000  /* bit write */
#define BIT_SECTORERASE     0x10000000  /* bit sectore erase */
#define BIT_READSTATUS      0x08000000  /* bit read status */
#define BIT_SECTORPROTECT   0x04000000  /* bit sectoer protect */
#define BIT_BUSY            0x40000000  /* bit busy */
#define BIT_ILLERASE        0x20000000  /* bit illegal erase */
#define BIT_ILLWRITE        0x10000000  /* bit illegal write */
#define BIT_RELOAD          0x80000000  /* bit reload */

#define PROTECTED_BIT_MASK  0xC1        /* mask of the sector protected bits */

/*--- known manufacturer codes/device codes ---*/
#define MANUFACT_STMICRO    	0x15    /* ID read from the flash */
#define MANUFACT_STMICRO_64    	0x16    /* ID read from the flash */
#define MANUFACT_STMICRO_128    0x18    /* ID read from the flash */
#define DEVCODE_M25P32          0x20    /* device id for the M25P32 Flash */
#define DEVCODE_M25P64          DEVCODE_M25P32



/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
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
static int32 ReadBlock( FLASH_DEVS *fDev,
						u_int32 offs,
    					u_int32 len,
    					u_int8  *buf);
static int32 DataPollingAlg( DEV_HDL *h,
							u_int32 offset,
							u_int32 val,
							u_int32 tout);
static int32 FindSect( FLASH_DEVS *h, u_int32 phyAddr, u_int32 *phySizeP );
static int32 UnlockBlock(FLASH_DEVS *fDev);

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
	{ "STMICROM25P32 4M", MANUFACT_STMICRO,DEVCODE_M25P32,0x08000,64,BOOT_SECT_TOP,
	  {
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
		  0x001E0000, 0x001E8000, 0x001F0000, 0x001F8000, 0x00200000
	  }
	},
	{ "STMICROM25P64 8M", MANUFACT_STMICRO_64,DEVCODE_M25P64,0x08000,128,BOOT_SECT_TOP,
	  {
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
		  0x001E0000, 0x001E8000, 0x001F0000, 0x001F8000, 0x00200000,
		  0x00208000, 0x00210000, 0x00218000, 0x00220000, 0x00228000,
		  0x00230000, 0x00238000, 0x00240000, 0x00248000, 0x00250000,
		  0x00258000, 0x00260000, 0x00268000, 0x00270000, 0x00278000,
		  0x00280000, 0x00288000, 0x00290000, 0x00298000, 0x002A0000,
		  0x002A8000, 0x002B0000, 0x002B8000, 0x002C0000, 0x002C8000,
		  0x002D0000, 0x002D8000, 0x002E0000, 0x002E8000, 0x002F0000,
		  0x002F8000, 0x00300000, 0x00308000, 0x00310000, 0x00318000,
		  0x00320000, 0x00328000, 0x00330000, 0x00338000, 0x00340000,
		  0x00348000, 0x00350000, 0x00358000, 0x00360000, 0x00368000,
		  0x00370000, 0x00378000, 0x00380000, 0x00388000, 0x00390000,
		  0x00398000, 0x003A0000, 0x003A8000, 0x003B0000, 0x003B8000,
		  0x003C0000, 0x003C8000, 0x003D0000, 0x003D8000, 0x003E0000,
		  0x003E8000, 0x003F0000, 0x003F8000, 0x00400000
	  }
	},
	{ "STMICROM25P128 16M", MANUFACT_STMICRO_128,DEVCODE_M25P64,0x08000,256,BOOT_SECT_TOP,
	  {
          /* ts: aligned as 4 columns to easier see accidentally deleted chars */
		  0x00000000, 0x00008000, 0x00010000, 0x00018000,
		  0x00020000, 0x00028000, 0x00030000, 0x00038000,
		  0x00040000, 0x00048000, 0x00050000, 0x00058000,
		  0x00060000, 0x00068000, 0x00070000, 0x00078000,
		  0x00080000, 0x00088000, 0x00090000, 0x00098000,
		  0x000A0000, 0x000A8000, 0x000B0000, 0x000B8000,
		  0x000C0000, 0x000C8000, 0x000D0000, 0x000D8000,
		  0x000E0000, 0x000E8000, 0x000F0000, 0x000F8000,

		  0x00100000, 0x00108000, 0x00110000, 0x00118000,
		  0x00120000, 0x00128000, 0x00130000, 0x00138000,
		  0x00140000, 0x00148000, 0x00150000, 0x00158000,
		  0x00160000, 0x00168000, 0x00170000, 0x00178000,
		  0x00180000, 0x00188000, 0x00190000, 0x00198000,
		  0x001A0000, 0x001A8000, 0x001B0000, 0x001B8000,
		  0x001C0000, 0x001C8000, 0x001D0000, 0x001D8000,
		  0x001E0000, 0x001E8000, 0x001F0000, 0x001F8000,

		  0x00200000, 0x00208000, 0x00210000, 0x00218000,
		  0x00220000, 0x00228000, 0x00230000, 0x00238000,
		  0x00240000, 0x00248000, 0x00250000, 0x00258000,
		  0x00260000, 0x00268000, 0x00270000, 0x00278000,
		  0x00280000, 0x00288000, 0x00290000, 0x00298000,
		  0x002A0000, 0x002A8000, 0x002B0000, 0x002B8000,
		  0x002C0000, 0x002C8000, 0x002D0000, 0x002D8000,
		  0x002E0000, 0x002E8000, 0x002F0000, 0x002F8000,

		  0x00300000, 0x00308000, 0x00310000, 0x00318000,
		  0x00320000, 0x00328000, 0x00330000, 0x00338000,
		  0x00340000, 0x00348000, 0x00350000, 0x00358000,
		  0x00360000, 0x00368000, 0x00370000, 0x00378000,
		  0x00380000, 0x00388000, 0x00390000, 0x00398000,
		  0x003A0000, 0x003A8000, 0x003B0000, 0x003B8000,
		  0x003C0000, 0x003C8000, 0x003D0000, 0x003D8000,
		  0x003E0000, 0x003E8000, 0x003F0000, 0x003F8000,

		  0x00400000, 0x00408000, 0x00410000, 0x00418000,
		  0x00420000, 0x00428000, 0x00430000, 0x00438000,
		  0x00440000, 0x00448000, 0x00450000, 0x00458000,
		  0x00460000, 0x00468000, 0x00470000, 0x00478000,
		  0x00480000, 0x00488000, 0x00490000, 0x00498000,
		  0x004A0000, 0x004A8000, 0x004B0000, 0x004B8000,
		  0x004C0000, 0x004C8000, 0x004D0000, 0x004D8000,
		  0x004E0000, 0x004E8000, 0x004F0000, 0x004F8000,

		  0x00500000, 0x00508000, 0x00510000, 0x00518000,
		  0x00520000, 0x00528000, 0x00530000, 0x00538000,
		  0x00540000, 0x00548000, 0x00550000, 0x00558000,
		  0x00560000, 0x00568000, 0x00570000, 0x00578000,
		  0x00580000, 0x00588000, 0x00590000, 0x00598000,
		  0x005A0000, 0x005A8000, 0x005B0000, 0x005B8000,
		  0x005C0000, 0x005C8000, 0x005D0000, 0x005D8000,
		  0x005E0000, 0x005E8000, 0x005F0000, 0x005F8000,

		  0x00600000, 0x00608000, 0x00610000, 0x00618000,
		  0x00620000, 0x00628000, 0x00630000, 0x00638000,
		  0x00640000, 0x00648000, 0x00650000, 0x00658000,
		  0x00660000, 0x00668000, 0x00670000, 0x00678000,
		  0x00680000, 0x00688000, 0x00690000, 0x00698000,
		  0x006A0000, 0x006A8000, 0x006B0000, 0x006B8000,
		  0x006C0000, 0x006C8000, 0x006D0000, 0x006D8000,
		  0x006E0000, 0x006E8000, 0x006F0000, 0x006F8000,

		  0x00700000, 0x00708000, 0x00710000, 0x00718000,
		  0x00720000, 0x00728000, 0x00730000, 0x00738000,
		  0x00740000, 0x00748000, 0x00750000, 0x00758000,
		  0x00760000, 0x00768000, 0x00770000, 0x00778000,
		  0x00780000, 0x00788000, 0x00790000, 0x00798000,
		  0x007A0000, 0x007A8000, 0x007B0000, 0x007B8000,
		  0x007C0000, 0x007C8000, 0x007D0000, 0x007D8000,
		  0x007E0000, 0x007E8000, 0x007F0000, 0x007F8000,

		  0x00800000
	  }
	},	{ "", 0 }
};


/******************************* istratapc28fxxxp30_Try ***********************/
/** Check if this Flash command set can handle the device.
 *
 *	This is the only public function of a Flash. It gets called from the
 *  fpga_load::init_flash function.
 *
 *
 *  If the Flash command set supports the device,
 *  it returns a pointer to the init function
 *
 *-----------------------------------------------------------------------------
 *  \param	devHdl		\IN		DEV_HDL handle
 *	\param	flash_initP	\OUT	pointer to function ptr where init function
 *								will be stored
 *  \param	dbgLevel	\IN		Debug level to be used in try function
 *  \return	success (0) or error code
 ******************************************************************************/
extern int32 Z100_STM25P32_TRY(DEV_HDL *devHdl,
								FLASH_INITP *flash_initP,
								u_int32 dbgLevel)
{
	int32 error=0;
	FLASH_DEVS fDev;

	DBGOUT(( "FLASH::stm25p32::Try\n" ));

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

	DBGOUT(( "FLASH::stm25p32::Try\n"
			"    manId = 0x%04x, devId = 0x%04x, "
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

	DBGOUT(("FLASH::stm25p32::Try finished 0x%04x\n", (int)error));
	return error;
}

/*----------------------------------------------------------------------
 * INTERNAL FUNCTIONS
 *---------------------------------------------------------------------*/

/********************************* Init ***************************************/
/** Initialize the function pointers to this Flash's command set.
 *
 *	Called by the Flash_Init function.
 *
 *-----------------------------------------------------------------------------
 *  \param	h		\IN		DEV_HDL handle
 *  \return	success (0) or error code
 *  \return	*handleP	filled with handle
 ******************************************************************************/
static int32 Init( DEV_HDL *h )
{
	FLASH_ENTRY *ent = &h->flash_entry;

	ent->Exit 			= &Exit;
	ent->Reset 			= &Reset;
	ent->ReadBlock 		= &ReadBlock;
	ent->WriteBlock		= &WriteBlock;
	ent->EraseChip  	= &EraseChip;
	ent->EraseSectors	= &EraseSectors;
	h->flashDev.isInit	= TRUE;

	Reset(&h->flashDev);		/* reset device to read mode */

    UOS_MikroDelayInit();       /* init delay timer once, takes 600ms on Linux */

	return 0;
}

/************************************ Exit ************************************/
/** Standard Exit for Flash.
 *
 *-----------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \return	success (0) or error code
 ******************************************************************************/
static int32 Exit( FLASH_DEVS *fDev)
{
	DBGOUT(( "FLASH::stm25p32::Exit\n" ));

	Reset(fDev);				/* reset device to read mode */

	fDev->devHdl->flashDev.isInit = FALSE;
	return 0;
}

/********************************* Identify ***********************************/
/** Issue autoselect command at specified phyAddr.
 *
 *  \remark Device will be kept in READ_ID mode when function finished.
 *
 *-----------------------------------------------------------------------------
 *  \param	devHdl		\IN		DEV_HDL handle
 ******************************************************************************/
static void Identify( DEV_HDL *devHdl )
{
	u_int32 retval;
	u_int8  flash_acc;
	DEV_HDL *h = devHdl;

	DBGOUT(( "FLASH::stm25p32::Identify\n" ));

	flash_acc = devHdl->flash_acc_size;
	devHdl->flash_acc_size = Z100_FLASH_ACCESS_8BIT;		/* 8 bit Flash */

    retval = Z100_FLASH_READ(devHdl, BIT_READSID);
	if( ((retval & 0xff) == MANUFACT_STMICRO)    ||
	    ((retval & 0xff) == MANUFACT_STMICRO_64) ||
		((retval & 0xff) == MANUFACT_STMICRO_128))
	{
		devHdl->flashDev.manId = retval & 0xff;
        devHdl->flashDev.devId = DEVCODE_M25P32;
	}

	if( devHdl->dbgLevel )
		printf( "FLASH::stm25p32::Identify mnf=%04x dev=%04x\n",
				(unsigned int)devHdl->flashDev.manId,
				(int)devHdl->flashDev.devId);

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
	DBGOUT(( "FLASH::stm25p32::CheckId mnf=%04x dev=%04x\n",
			(unsigned int)devHdl->flashDev.manId,
			(int)devHdl->flashDev.devId) );

	/*--- check if device is supported ---*/
	if( (devHdl->flashDev.manId == MANUFACT_STMICRO) 	 ||
	    (devHdl->flashDev.manId == MANUFACT_STMICRO_64 ) ||
		(devHdl->flashDev.manId == MANUFACT_STMICRO_128))
	{
		const FLASH_DEVS *d = G_Devs;
		int32 i;

		while( d->devId ){
			if( ( devHdl->flashDev.devId == d->devId ) &&
			 	( devHdl->flashDev.manId == d->manId) )
			{
				if(devHdl->dbgLevel)
					printf( "FLASH::stm25p32::CheckId\n"
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
	DBGOUT(( "FLASH::stm25p32::CheckId finished, no supported dev\n"));
	return NULL;
}

/********************************* EraseChip **********************************/
/** Erase entire Flash device.
 *
 *-----------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \return	success (0) or error code
 ******************************************************************************/
static int32 EraseChip( FLASH_DEVS *fDev )
{
	int32 error = 0;
    DEV_HDL *h = fDev->devHdl;

	DBGOUT(( "FLASH::stm25p32::EraseChip\n" ));

    /* set bulkerase bit enable bit */
    Z100_FLASH_WRITE( fDev->devHdl, BIT_BULKERASE , 0 );

    error = DataPollingAlg(fDev->devHdl, 0, 0, 80000);

	DBGOUT(( "FLASH::stm25p32::EraseChip finished 0x%04x\n", (int)error ));
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
	u_int32 retVal = 0;

	DBGOUT(( "FLASH::stm25p32::EraseSectors\n" ));
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
	FindSect( fDev, phyStart, &sectSize );
	phyStart &= ~(sectSize-1);

	FindSect( fDev, phyEnd, &sectSize );
	phyEnd &= ~(sectSize-1);

	DBGOUT(( "FLASH::stm25p32::EraseSectors\n"
			 "    addr= 0x%06x, len= 0x%06x, startOffs= 0x%06x,"
			 " endOffs= 0x%06x, sSize= 0x%06x\n",
			 (int)startOffs, (int)len, (int)phyStart,
			 (int)phyEnd, (int)sectSize ));

	/*--- read sector protect bits ---*/
	retVal = Z100_FLASH_READ(h, BIT_SECTORPROTECT);
	if(retVal & PROTECTED_BIT_MASK){
	    /*--- Unlock the protected areas ---*/
	    UnlockBlock(fDev);
	}

	/* delete sectors */
	do {
		sect = FindSect( fDev, phyStart, &sectSize );
		if( fDev->devHdl->dbgLevel )
			printf( "Erasing sector %d of %d, size = 0x%x\n",
					(int)sect+1, (int)fDev->nSectors, (unsigned int)sectSize);

        /*--- set BIT_SECTORERASE and address ---*/
		Z100_FLASH_WRITE( fDev->devHdl, BIT_SECTORERASE | (phyStart & 0xFFFFFF), 0);

        error = DataPollingAlg(fDev->devHdl, 0, 0, 3000);
		if( error )
			break;

		if( fDev->devHdl->dbgLevel )
			printf("    Erase Sector ----> OK\n");
		if( phyStart == phyEnd )
			break;
		phyStart += sectSize;
	} while(1);

end:
	DBGOUT(( "FLASH::stm25p32::EraseSectors finished 0x%04x\n",
			(int)error));
	return error;
}

/*********************************** WriteBlock *******************************/
/** Write block of data to Flash.
 *
 *-----------------------------------------------------------------------------
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
    u_int32 *bufp32 = NULL;
	u_int8  *bufp8  = NULL;
	u_int32 val		= 0;
	int32 error		= 0;
	u_int32 nWrite	= 0;
	DEV_HDL *h		= fDev->devHdl;
	u_int32 retVal	= 0;
	u_int32 i		= 0;
	u_int32 startoffs = offs;
	u_int32 blockdif = 0;

	DBGOUT(( "FLASH::stm25p32::WriteBlock offs=0x%08x len=%d\n",
			 (int)offs, (int)len ));

    /* check write behind the end of the flash */
    if( ( offs >= fDev->sectAddr[fDev->nSectors]*2) ||
        ((offs + len) > fDev->sectAddr[fDev->nSectors]*2))
    {
        printf( "*** Write behind the flash memory!\n");
        error = ERR_FLASH_WRITE_FAILED;
        goto end;
    }

	/*--- read sector protect bits ---*/
	retVal = Z100_FLASH_READ(h, BIT_SECTORPROTECT);
	if(retVal & PROTECTED_BIT_MASK){
	    /*--- Unlock the protected areas ---*/
	    UnlockBlock(fDev);
	}

	/* first block of less than 265 bytes to write */
	bufp8 = (u_int8*)buf;
	blockdif = 256 - (startoffs & 0xff);

	for ( i =0 ; i < (startoffs % 4) ; i++){
		/*--- write 8 bit to the data register ---*/
		Z100_MWRITE_D8( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET, (u_int8)0xff);
	}
	/*--- start address is no 256 byte block address ---*/
	for ( i = 0; (i < (blockdif % 256)) && (len > 0) ; i++){
		val = *bufp8++;
		/*--- write 8 bit to the data register ---*/
		Z100_MWRITE_D8( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET, (u_int8)(val & 0xff));
		len--;
    }
	if( blockdif % 256){
		/*--- write the address register and the write bit ---*/
		Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_ADDR_REG_OFFSET, BIT_WRITE | (offs & 0xFFFFFF));
		/*--- wait for reset of data busy ---*/
		if( (error = DataPollingAlg(h, offs, val, 1000)) )
			goto end;
		offs += (blockdif);
	}
	if ( blockdif == 256 ){
		bufp32 = (u_int32*)buf;
	}
	else {
		bufp32 = (u_int32*)(buf + blockdif);

	}

	/*--- calculate the number of 32 bit accesses til the end of the file ---*/
	nWrite = len / 4;
	if (len % 4){
		nWrite++;
	}
	for( i = 0; i < nWrite ; i++){

        if( !(i % 32768 )&& ( i != 0 ) && h->dbgLevel) /* write status all 128KB */
            printf("Current write status: %d bytes written from %d bytes !\n",i*4, len);

		if( (!(i % 64)) && ( i != 0 ) ){
			/*--- write the address register and the write bit every 256 bytes ---*/
			Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_ADDR_REG_OFFSET, BIT_WRITE | (offs & 0xFFFFFF));
			/*--- wait for reset of data busy ---*/
			if( (error = DataPollingAlg(h, offs, val, 1000)) )
				goto end;

			offs += 256;
		}
		val = *bufp32++;
		/* last access */
		if(i == nWrite-1){
			switch(len %4)
			{
			case 0:
				/*--- write 32 bit to the data register ---*/
    			Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET, SW32(val) );
				break;
			case 1:
				/*--- write 8 bit to the data register ---*/
    			Z100_MWRITE_D8( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET,(u_int8)(SW32(val) & 0xff) );
				break;
			case 2:
				/*--- write 16 bit to the data register ---*/
    			Z100_MWRITE_D16( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET, (u_int16)(SW32(val) & 0xffff) );
				break;
			case 3:
				/*--- write 16 bit to the data register ---*/
    			Z100_MWRITE_D16( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET, (u_int16)(SW32(val) & 0xffff) );
				/*--- write 8 bit to the data register offset + 2 ---*/
				Z100_MWRITE_D8( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET + 2,
    				(u_int8)((SW32(val) & 0xff0000) >> 16) );
				break;
			}
		}
		else{
			/*--- write 32 bit to the data register ---*/
    		Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET, SW32(val));
}
	}
	if (len != 0){	/* len < 256 is already written -> len = 0 */
		/*--- write the address register and the write bit every 256 bytes ---*/
		Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_ADDR_REG_OFFSET, BIT_WRITE | (offs & 0xFFFFFF));
		/*--- write data register dummy 0xa5a5a5a5 to enable bit write ---*/
//TODO: Added for testing with the SPI Flash		Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET, 0xa5a5a5a5);
		/*--- wait for reset of data busy ---*/
		if( (error = DataPollingAlg(h, offs, val, 1000)) )
			goto end;
	}


end:
	if( error )
		Reset(fDev);

	DBGOUT(( "FLASH::stm25p32::WriteBlock finished 0x%04x\n",
			(int)error ));
	return error;
}

/*********************************** ReadBlock ********************************/
/** Read block of data from Flash.
 *
 *-----------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \param	offs	\IN		offset in flash
 *  \param	len		\IN		length (in bytes) to be read
 *  \param	buf		\IN		destination buffer
 *
 *  \return	success (0)
 ******************************************************************************/

static int32 ReadBlock(
    FLASH_DEVS *fDev,
	u_int32 offs,
    u_int32 len,
    u_int8  *buf)
{
	u_int32 retVal;
	u_int16 retVal16;
	u_int8  retVal8;
	u_int32 nAccess = len;
	u_int32 *bufp32 = NULL;
	DEV_HDL *h = fDev->devHdl;
	u_int32 offset = offs;
	u_int32 i = 0;

	DBGOUT(( "FLASH::stm25p32::ReadBlock offs=0x%08x len=0x%08x\n",
			 (int)offs, (int)len ));


	/*--- calculate number of 32 bit accesses ---*/
	nAccess = len / 4;
	if(len %4)
	    nAccess++;
	/*--- add one nAccess to a next 32 bit area ---*/
	if( (((offset % 4) + ( len %4 ))> 4 ) || (( offset % 4 ) && ((len % 4)==0))  )
		nAccess++;

	bufp32 = (u_int32*)buf;

	/*--- write the address to read from  ---*/
    Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_ADDR_REG_OFFSET, offset & 0xFFFFFC);

    if ( len > 4){
    switch(offset%4){
		case 0:
		   /*--- read 32 bit data from the data register ---*/
		   retVal = Z100_MREAD_D32(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET);
		   /* access 32 bit data bus */
		   *bufp32++ = SW32(retVal);
		break;
		case 3:
			/*--- read 8 bit data from the data register ---*/
			retVal8 = Z100_MREAD_D8(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET+3);
			/* access 8 bit data bus */
			    *bufp32 = SW32(retVal8);
			bufp32 = (u_int32*)((u_int8*)bufp32 + 1);
		break;
		case 2:
			/*--- read 16 bit data from the data register ---*/
			retVal16 = Z100_MREAD_D16(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET+2);
			/* access 16 bit data bus */
			    *bufp32 = SW32(retVal16);
			bufp32  = (u_int32*)((u_int8*)bufp32 + 2);
		break;
		case 1:
			/*--- read 32 bit data from the data register ---*/
			retVal = Z100_MREAD_D32(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET);
			/* access 24 bit data bus */
			    *bufp32 = SW32((u_int8)((retVal & 0xFF00) >> 8));
			bufp32 = (u_int32*)((u_int8*) bufp32 + 1);
			    *bufp32 = SW32((u_int16)((retVal & 0xFFFF0000) >> 16));
			bufp32 = (u_int32*)((u_int8*) bufp32 + 2);
		break;
	}

        len -= (4 - (offset % 4));
	offset -= offset % 4 ;
	offset += 4;
	nAccess--;
    }
    else {
        for ( i  = 0; i < len ; i++) {
            /*--- write the address to read from  ---*/
            Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_ADDR_REG_OFFSET, offset & 0xFFFFFC);
            /*--- read 8 bit data from the data register ---*/
			retVal8 = Z100_MREAD_D8(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET+(offset%4));
			/* access 8 bit data bus */
    	    *bufp32 = SW32(retVal8);
			bufp32 = (u_int32*)((u_int8*) bufp32 + 1 );
			offset++;
			nAccess = 0;
    	}
    }

	while(nAccess>0){
        /* write the address to read from  */
        Z100_MWRITE_D32( h->mappedAddr, Z045_FLASH_ADDR_REG_OFFSET, offset & 0xFFFFFF);
        if(nAccess == 1){
            switch(len %4){
            case 0:
			   /*--- read 32 bit data from the data register ---*/
			   retVal = Z100_MREAD_D32(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET);
			   /* access 32 bit data bus */
               *bufp32++ = SW32(retVal);

            break;
            case 1:
            {
                u_int8* bufp8 = (u_int8*)bufp32;
                /*--- read 8 bit data from the data register ---*/
                retVal8 = Z100_MREAD_D8(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET);
                /* access 8 bit data bus */
                *bufp8 = (u_int8)(retVal8);
            break;
            }
            case 2:
            {
                u_int16 * bufp16 = (u_int16*)bufp32;
                /*--- read 16 bit data from the data register ---*/
                retVal16 = Z100_MREAD_D16(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET);
                /* access 16 bit data bus */
                *bufp16 = (u_int16)SW16(retVal16);
            break;
            }
            case 3:
            {
                u_int16 * bufp16 = (u_int16*)bufp32;
                u_int8* bufp8 = NULL;
                /*--- read 32 bit data from the data register ---*/
                retVal = Z100_MREAD_D32(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET);

                /* access 24 bit data bus */
                *bufp16++ = (u_int16)SW16((retVal) & 0xFFFF);
				bufp8 = (u_int8*)bufp16;
				*bufp8 = (u_int8)(((retVal) & 0xFF0000) >> 16);
            break;
            }
            default:
                /* 32bit access */
            break;
            }
	    }else {
            /*--- read 32 bit data from the data register ---*/
            retVal = Z100_MREAD_D32(h->mappedAddr, Z045_FLASH_DATA_REG_OFFSET);
			/* access 32 bit data bus */
            *bufp32++ = SW32(retVal);

            offset+=4;

       }
		nAccess--;
       }

	DBGOUT(( "FLASH END OF READBLOCK\n "));
	return 0;
}

/*********************************** Reset ************************************/
/** Reset Flash device.
 *
 *---------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 ******************************************************************************/
static void Reset( FLASH_DEVS *fDev )
{
	DEV_HDL *h = fDev->devHdl;
	/* write disable */
	Z100_FLASH_WRITE( fDev->devHdl,0,0);
}


/********************************* DataPollingAlg *****************************/
/** Perform Data Polling Algorithm (after write/erase commands)
 *
 *-----------------------------------------------------------------------------
 *  \param	h		\IN		DEV_HDL handle
 *  \param	offset	\IN		offset of data to check
 *  \param	val		\IN		bit set to wait for
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

    if ((h->flashDev.manId == MANUFACT_STMICRO_64) || (h->flashDev.manId == MANUFACT_STMICRO_128))
        UOS_MikroDelay(500);
    else
    UOS_MikroDelay(100);

	baseMsec = UOS_MsecTimerGet();


	while(1){

        retVal = Z100_MREAD_D32 (h->mappedAddr,Z045_FLASH_ADDR_REG_OFFSET);

		/*--- check the busy flag  ---*/
		if ( retVal  ){
		    if((retVal & BIT_ILLERASE )) /*--- illegal erase active ---*/
            {
                /*error = ERR_FLASH_ERASE_FAILED;*/
                DBGOUT(("Illegal Erase Bit is set ! 0x%0x\n", retVal));
				error = ERR_FLASH_WRITE_FAILED;
				goto end;
            }

		    if((retVal & BIT_ILLWRITE )) /*--- illegal write active ---*/
            {
                DBGOUT(("Illegal Write Bit is set ! 0x%0x\n", retVal));
				error = ERR_FLASH_WRITE_FAILED;
                goto end;
            }
		}
		if(!(retVal & BIT_BUSY ) )  /*--- busy flag not active ---*/
        	break;

		if( MSECDIFF(baseMsec) > toutMsecs){
			printf( "*** FLASH::Program Timeout\n");
			error = ERR_UOS_TIMEOUT;
			goto end;
		}

	} /* while (1)*/

end:
	if(error)
	{
	    /*--- read the status ---*/
		DBGOUT(( "FLASH::stm25p32::DataPollingAlg ReadStatus : 0x%08x\n",
		    SW32((unsigned int)Z100_MREAD_D32( h->mappedAddr ,Z045_FLASH_DATA_REG_OFFSET )) ));
	}
	return(error);
}

/********************************* FindSect ***********************************/
/** Convert dev address to sector number
 *
 *-----------------------------------------------------------------------------
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
		if( (addr >= fDev->sectAddr[sect]*2) && (addr < fDev->sectAddr[sect+1]*2))
		{
			*phySizeP = (fDev->sectAddr[sect+1] - fDev->sectAddr[sect])*2;
			/* printf("FindSect: addr = %x -> sect = %d *phySizeP = %x \n", addr, sect, *phySizeP ); */
			return sect;
		}
	}

	return -1;
}


/********************************* UnlockBlock ********************************/
/** Unlocks the block to erase or write data
 *
 *-----------------------------------------------------------------------------
 *  \param	fDev		\IN		FLASH_DEVS handle
 *  \return	success (0) or error code
 ******************************************************************************/
static int32 UnlockBlock(FLASH_DEVS *fDev)
{
	int32   error = 0;
	DEV_HDL *h = fDev->devHdl;

	/* unlock sectors */
	if( h->dbgLevel )
		printf( "Unlocking sector\n");

	/*--- give UNLOCK BLOCK command ---*/
	Z100_FLASH_WRITE( fDev->devHdl, BIT_SECTORPROTECT, 0  );

	error = DataPollingAlg(fDev->devHdl, 0, 0, 20);

	return error;
}




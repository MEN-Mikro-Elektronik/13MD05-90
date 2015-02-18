/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  am29lvxxx_smb.c
 *
 *       \author  Christian.Schuster@men.de
 *        $Date: 2009/01/12 16:58:35 $
 *    $Revision: 2.5 $
 *
 *        \brief  command set for AMD 29lv160 and compatible flashes
 *                this version is used for accessing the flash over I2C through
 *                an PLD on boards A500, A404, ...
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
 * $Log: am29lvxxx_smb.c,v $
 * Revision 2.5  2009/01/12 16:58:35  CKauntz
 * R: 1. In non debugged version no flash name visible
 *    2. Write behind the flash size
 * M: 1.a: changed to printf to show name in verbose mode
 *      b: changed flash name to show size in MB
 *    2. No write or delete when address greater than flash size
 *       No write but delete of sectors when start within but end behind the size
 *
 * Revision 2.4  2007/10/17 17:27:35  CKauntz
 * Added Switch NO_IDPROM_CHECK for the A404 without assembled EEPROM
 *
 * Revision 2.3  2007/07/09 20:12:53  CKauntz
 * added switch Z100_CONFIG_SMB
 *
 * Revision 2.2  2006/02/08 12:07:51  cschuster
 * added swap macros
 * fixed: don't touch out bits in COMMAND register
 *
 * Revision 2.1  2005/12/12 11:05:20  cschuster
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#ifdef Z100_CONFIG_SMB


#include "fpga_load.h"
#include "fpga_load_flash.h"
#include "MEN/eeprod.h"
#include "MEN/smb2.h"
#include "MEN/oss.h"
#define MAC_MEM_MAPPED
#include "MEN/maccess.h"
/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define ID_PROM_ADDR				0xAC
#define ID_PROM_IDSTART				0x00

#define PLD_SMB_ADDR				0x40
#define PLD_CMD_FLEXEN				0x08
#define PLD_CMD_CLEAR_ADDR			(0x80 | PLD_CMD_FLEXEN)
#define PLD_CMD_LOAD_FPGA			(0x40 | PLD_CMD_FLEXEN)
#define PLD_CMD_ERASE_FLASH			(0x20 | PLD_CMD_FLEXEN)
#define PLD_CMD_READ				(0x10 | PLD_CMD_FLEXEN)
#define PLD_CMD_WRITE				(0x00 | PLD_CMD_FLEXEN)

#define PLD_CMD_BS_MASK				0x03
#define PLD_CMD_BS_OUT0				0x01
#define PLD_CMD_BS_OUT1				0x02

#define PLD_STAT_FL_READY			0x40

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

#define MANUFACT_ST 		0x20
#define DEVCODE_ST_29F040	0xE2

/*--- erase states ---*/
#define STATE_IDLE			0
#define STATE_ERASING		1
#define STATE_ERASED		2

#define DO_BIT_REVERSE		1
#define NO_BIT_REVERSE		0
/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef struct {
	char	hwName[6];
	int32	manId;
	int32	devId;
	u_int8	doBitReverse;
} BOARD_DATA;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/


static BOARD_DATA const G_SupBoards[] = {
	{ "A404-", MANUFACT_AMD, DEVCODE_29LV160DT, DO_BIT_REVERSE },
	{ "A500-", MANUFACT_AMD, DEVCODE_29LV160DT, DO_BIT_REVERSE },
	{ "", 0, 0, 0 }
};

static u_int8 G_DoBitReverse;
/*
 * NOTE: addresses and sizes in the following struct are half the real size
 *       (everything is sized in words ( 16bit )).
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
static int32 ReadBlock( FLASH_DEVS *fDev,
						u_int32 offs,
						u_int32 len,
						u_int8  *buf);
static u_int16 ReverseBitsInBytesOfWord(u_int16 inWord);
#if 0  /* erase sectors not supported by PLD */
static int32 FindSect( FLASH_DEVS *h, u_int32 phyAddr, u_int32 *phySizeP );
#endif

/******************************* am29lvxxx_smb_Try ********************************/
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
extern int32 Z100_AM29LVXXX_SMB_TRY(DEV_HDL *devHdl,
								FLASH_INITP *flash_initP,
								u_int32 dbgLevel)
{
	int32 error=0;
	FLASH_DEVS fDev;
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)devHdl->smbLocHdl.smbHdl;

	DBGOUT(( "FLASH::am29lvxxx_smb::Try\n" ));

	if( !smbHdl ) {
		error = ERR_FLASH_NOT_SUPPORTED;
		goto end;
	}
	/* get device identification from flash */
	Identify( devHdl );

	if( CheckId( devHdl ) == NULL )
		error = ERR_FLASH_NOT_SUPPORTED;

	DBGOUT(( "FLASH::am29lvxxx_smb::Try\n"
			"    manId = 0x%02x, devId = 0x%04x, "
			"sectS = %d, SectNum = %d, bootSect = %d\n",
			(int)devHdl->flashDev.manId, (int)devHdl->flashDev.devId,
			(int)devHdl->flashDev.sectSize, (int)devHdl->flashDev.nSectors,
			(int)devHdl->flashDev.bootSect));

end:
	if( error )
		fDev.manId = 0;
	*flash_initP = NULL;
	if( error == 0){
		*flash_initP = &Init;
	}

	DBGOUT(("FLASH::am29lvxxx_smb::Try finished 0x%04x\n", (int)error));
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
	ent->ReadBlock 	= &ReadBlock;
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
	DBGOUT(( "FLASH::am29lvxxx_smb::Exit\n" ));

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
	u_int32 i;
	int32 error = 0;
	u_int8 boardIdRead[16];
	const BOARD_DATA *boardData = G_SupBoards;
	char *hwName;
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)devHdl->smbLocHdl.smbHdl;

	DBGOUT(( "FLASH::am29lvxxx_smb::Identify\n" ));

#ifndef NO_IDPROM_CHECK
	/* set addr in ID PROM and read magic word */
	if( (error = smbHdl->ReadByteData( smbHdl, 0,
									  ID_PROM_ADDR,
									  ID_PROM_IDSTART,
									  &boardIdRead[0])) )
	{
		printf( "FLASH::am29lvxxx_smb::Identify FAILED to read ID PROM Magic\n"
				"                              (ERROR = 0x%02X \n",
				(int)error);
		goto end;
	}

	/* get ID content from ID PROM */
	for( i=1; i< 16; i++ )
	{
		if( (error = smbHdl->ReadByte( smbHdl, 0,
									   ID_PROM_ADDR,
									   &boardIdRead[i] )) )
		{
			printf( "FLASH::am29lvxxx_smb::Identify FAILED to read ID PROM\n"
					"                              (ERROR = 0x%02X \n",
					(int)error);
			goto end;
		}
	}

	if( (boardIdRead[0]>>4) == EEID_PD )
	{
		EEPROD *eeprod = (EEPROD *)&boardIdRead[0];
		hwName = &eeprod->pd_hwName[0];
	} else if ((boardIdRead[0]>>4) == EEID_PD2)
	{
		EEPROD2 *eeprod2 = (EEPROD2 *)&boardIdRead[0];
		hwName = &eeprod2->pd_hwName[0];
	} else
	{
			printf( "FLASH::am29lvxxx_smb::Identify FAILED: "
					"IDPROM structure not known\n");
			error = 1;
			goto end;
	}
#endif
#ifdef NO_IDPROM_CHECK
    hwName = boardData->hwName; /* IDPROM CHECK disabled for the A404 */
#endif
	while( boardData->devId )
	{
		DBGOUT(( "FLASH::am29lvxxx_smb::Identify: try board %s\n",
				boardData->hwName));
		if( !strncmp(hwName, boardData->hwName, 4) )
		{
			devHdl->flashDev.manId = boardData->manId;
			devHdl->flashDev.devId = boardData->devId;
			G_DoBitReverse = boardData->doBitReverse;

			break;
		}
		boardData++;
	}

	if( !boardData->devId )
	{
		DBGOUT(( "FLASH::am29lvxxx_smb::Identify: no supported board found\n"));
		error = ERR_NO_SUPPORTED_FLASH_DEVICE_FOUND;
		goto end;
	}


end:
	if( error )
	{
		devHdl->flashDev.manId = 0xff;
		devHdl->flashDev.devId = 0xff;
	}

	if( devHdl->dbgLevel )
		printf( "FLASH::am29lvxxx_smb::Identify mnf=%02x dev=%02x\n",
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
	DBGOUT(( "FLASH::am29lvxxx_smb::CheckId\n"));

	/*--- check if device supported ---*/
	if( devHdl->flashDev.manId == MANUFACT_AMD ||
		devHdl->flashDev.manId == MANUFACT_ST )
	{
		const FLASH_DEVS *d = G_Devs;
		int32 i;

        DBGOUT(( "FLASH::am29lvxxx_smb::CheckId: "
						 "FLASH dev: %08x, Man %08x acc_size %d\n",
				  devHdl->flashDev.devId,
			 	  devHdl->flashDev.manId,
			 	  devHdl->flash_acc_size));

		while( d->devId ){
			if( ((devHdl->flash_acc_size &&
				  devHdl->flashDev.devId == d->devId) ||
			 	 ((devHdl->flashDev.devId & 0xff) == (d->devId & 0xff))) &&
			 	(devHdl->flashDev.manId == d->manId) )
			{
				if(devHdl->dbgLevel)
					printf("FLASH::am29lvxxx_smb::CheckId: "
						 "Found FLASH device %s\n", d->name );

				devHdl->flashDev.sectSize = d->sectSize;
				devHdl->flashDev.nSectors = d->nSectors;
				devHdl->flashDev.bootSect = d->bootSect;
				for( i=0; i<Z100_MAX_SECT; i++)
					devHdl->flashDev.sectAddr[i] = d->sectAddr[i];
				devHdl->flashDev.devHdl   = devHdl;
				devHdl->flashDev.isInit  = 0;
				return d;
			}
			DBGOUT(( "FLASH::am29lvxxx_smb::CheckId: "
					 "Missed match at FLASH device %s\n", d->name ));
			d++;
		}
	}
	DBGOUT(( "FLASH::am29lvxxx_smb::CheckId finished, no supported dev\n"));
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
	int32 error = 0;
	u_int8 status = 0, outSave = 0;
	/* u_int8 status = 0; */
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)fDev->devHdl->smbLocHdl.smbHdl;
	static const char functionName[]= "FLASH::am29lvxxx_smb::EraseChip";

	DBGOUT(( "%s entered\n", functionName ));

	/* read the status of cmd-register */
	if( (error = smbHdl->ReadByte( smbHdl, 0,
								  PLD_SMB_ADDR,
								  &status)) )
	{
		printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
				functionName, (int)error);
		goto end;
	}
	outSave = status & PLD_CMD_BS_MASK;

	if( (error = smbHdl->WriteByte(  smbHdl, 0,
									PLD_SMB_ADDR,
									PLD_CMD_ERASE_FLASH | outSave)) )
	{
		printf( "%s FAILED (ERROR = 0x%02X \n", functionName, (int)error);
		goto end;
	}

	/* wait until the flash rdy bit is set */
	/* read the status of cmd-register */
	if( (error = smbHdl->ReadByte( smbHdl, 0,
								  PLD_SMB_ADDR,
								  &status)) )
	{
		printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
				functionName, (int)error);
		goto end;
	}

	/* test the bit flash-rdy, and read again if not set */
	while( !(status & PLD_STAT_FL_READY) ) {
		if( (error = smbHdl->ReadByte( smbHdl, 0,
									  PLD_SMB_ADDR,
									  &status)) )
		{
			printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
					functionName, (int)error);
			goto end;
		}
	}

end:
	DBGOUT(( "%s finished 0x%04x\n", functionName, (int)error ));
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
	int32   error = 0;

	DBGOUT(( "FLASH::am29lvxxx_smb::EraseSectors\n"
			 " erasing sectors not supported for this device,"
			 " instead erasing chip\n" ));
	error = EraseChip( fDev );
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
	u_int16 val;
	/* u_int8 status = 0; */
	u_int16 *bufp16 = NULL;
	int32 error=0, nWrite;
	u_int32 i;
	u_int8 status = 0, outSave = 0;
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)fDev->devHdl->smbLocHdl.smbHdl;
	static const char functionName[]= "FLASH::am29lvxxx_smb::WriteBlock";

	DBGOUT(( "%s: offs=0x%08x len=%d\n",
			 functionName, (int)offs, (int)len ));

    /* check write behind the end of the flash */
    if( ( offs >= fDev->sectAddr[fDev->nSectors]*2) ||
        ((offs + len) > fDev->sectAddr[fDev->nSectors]*2))
    {
        printf( "*** Write behind the flash memory!\n");
        error = ERR_FLASH_WRITE_FAILED;
        goto end;
    }

	/* read the status of cmd-register */
	if( (error = smbHdl->ReadByte( smbHdl, 0,
								  PLD_SMB_ADDR,
								  &status)) )
	{
		printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
				functionName, (int)error);
		goto end;
	}
	outSave = status & PLD_CMD_BS_MASK;

	/* Reset Address Counter */
	if( (error = smbHdl->WriteByte(  smbHdl, 0,
									PLD_SMB_ADDR,
									PLD_CMD_CLEAR_ADDR | outSave)) )
	{
		printf( "%s FAILED (Reset AddrCounter, ERROR = 0x%02X \n",
				functionName, (int)error);
		goto end;
	}

	/* dummy reads to set addrCounter to desired position */
	i = offs / 2;
	while( i ) {
		if( (error = smbHdl->ReadWordData( smbHdl, 0,
										  PLD_SMB_ADDR,
										  PLD_CMD_READ | outSave,
										  &val)) )
		{
			printf( "%s FAILED (Dummy Read %d, ERROR = 0x%02X \n",
					functionName, (int)(offs/2 - i), (int)error);
			goto end;
		}
		i--;
	}

	nWrite = len / 2 + len%2;
	bufp16 = (u_int16*)buf;

	while( nWrite ){
		val = *bufp16;
		if( G_DoBitReverse ){
			val = ReverseBitsInBytesOfWord( *bufp16 );
		} else {
			val = *bufp16;
		}

		if( (error = smbHdl->WriteWordData(  smbHdl, 0,
											PLD_SMB_ADDR,
											PLD_CMD_WRITE | outSave,
											WSWAP16( val ))) )
		{
			printf( "%s FAILED (WriteWordData 0x%06x : 0x%02x, ERROR=0x%02X\n",
					functionName, (int)*bufp16,
					(int)(offs+((len/2+len%2)-nWrite)* 2),	(int)error);
			goto end;
		}

		/* DBGOUT(( "%s: nWrite = %4d, offset: 0x%08x, written value: 0x%04x\n",
				 functionName, (int)nWrite,
				 (int)(offs + ((len/2+len%2)-nWrite)*2), (int)*bufp16 ));
		*/


#if 0   /* not neccessary, flash is way faster than the i2c bus */

		/* wait until the flash rdy bit is set */
		/* read the status of cmd-register */
		if( (error = smbHdl->ReadByte( smbHdl, 0,
									  PLD_SMB_ADDR,
									  &status)) )
		{
			printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
					functionName, (int)error);
			goto end;
		}

		/* test the bit flash-rdy, and read again if not set */
		while( !(status & PLD_STAT_FL_READY) ) {
			if( (error = smbHdl->ReadByte( smbHdl, 0,
										  PLD_SMB_ADDR,
										  &status)) )
			{
				printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
						functionName, (int)error);
				goto end;
			}
		}
#endif /* #if 0 */

		bufp16++;
		nWrite--;
	}

end:

	DBGOUT(( "%s finished 0x%04x\n", functionName, (int)error ));
	return error;
}

/********************************* ReadBlock ******************************/
/** Read block of data from Flash
 *
 *---------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \param	offs	\IN		start offset within device, always word aligned
 *  \param	len		\IN		length (in bytes) to be read
 *  \param	buf		\IN		destination buffer
 *
 *  \return	success (0)
 ****************************************************************************/
static int32 ReadBlock(
    FLASH_DEVS *fDev,
	u_int32 offs,
    u_int32 len,
    u_int8  *buf)
{
	int32 error = 0;
	u_int32 i;
	u_int32 nAccess; /* number of accesses necessary */
	u_int16 retVal = 0;
	u_int16 *bufp16 = (u_int16*)buf;
	u_int8 status = 0, outSave = 0;
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)fDev->devHdl->smbLocHdl.smbHdl;
	static const char functionName[]= "FLASH::am29lvxxx_smb::ReadBlock";

	DBGOUT(( "%s: offs=0x%08x len=0x%08x\n",
			 functionName, (int)offs, (int)len ));

	/* read the status of cmd-register */
	if( (error = smbHdl->ReadByte( smbHdl, 0,
								  PLD_SMB_ADDR,
								  &status)) )
	{
		printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
				functionName, (int)error);
		goto end;
	}
	outSave = status & PLD_CMD_BS_MASK;

	/* Reset Address Counter */
	if( (error = smbHdl->WriteByte(  smbHdl, 0,
									PLD_SMB_ADDR,
									PLD_CMD_CLEAR_ADDR | outSave)) )
	{
		printf( "%s FAILED (Reset AddrCounter, ERROR = 0x%02X \n",
				functionName, (int)error);
		goto end;
	}

	/* dummy reads to set addrCounter to desired position */
	i = offs / 2;
	while( i ) {
		if( (error = smbHdl->ReadWordData( smbHdl, 0,
										  PLD_SMB_ADDR,
										  PLD_CMD_READ | outSave,
										  &retVal)) )
		{
			printf( "%s FAILED (Dummy Read %d, ERROR = 0x%02X \n",
					functionName, (int)(offs/2 - i), (int)error);
			goto end;
		}
		i--;
	}

	nAccess = len / 2 + len%2;

	while(nAccess>0){
		if( (error = smbHdl->ReadWordData( smbHdl, 0,
										  PLD_SMB_ADDR,
										  PLD_CMD_READ | outSave,
										  &retVal)) )
		{
			printf( "%s FAILED (Read %d, ERROR = 0x%02X \n",
					functionName, (int)(offs/2 - i), (int)error);
			goto end;
		}

		/* swap? */
		*bufp16= WSWAP16( retVal );

		if( G_DoBitReverse ){
			*bufp16 = ReverseBitsInBytesOfWord( *bufp16 );
		}

		/*DBGOUT(( "%s naccess = %4d, offset: 0x%08x, read value: 0x%08x\n",
				functionName, (int)(len / 2 + len%2 - nAccess),
				(int)(offs + (len / 2 + len%2 - nAccess)*2), (u_int16)*bufp16));
		*/
		bufp16++;
		nAccess--;
	}
end:
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
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)fDev->devHdl->smbLocHdl.smbHdl;
	int32 error = 0;
	u_int8 status = 0;
	static const char functionName[]= "FLASH::am29lvxxx_smb::Reset";

	/* read the status of cmd-register */
	if( (error = smbHdl->ReadByte( smbHdl, 0,
								  PLD_SMB_ADDR,
								  &status)) )
	{
		printf( "%s: Read Status FAILED ERROR = 0x%02X\n",
				functionName, (int)error);
		return;
	}

	/* Reset Address Counter */
	if( (error = smbHdl->WriteByte(  smbHdl, 0,
									PLD_SMB_ADDR,
									PLD_CMD_CLEAR_ADDR |
									(status & PLD_CMD_BS_MASK))) )
	{
		printf( "%s FAILED (Reset AddrCounter, ERROR = 0x%02X \n",
				functionName, (int)error);
	}
}

/*************************** ReverseBitsInBytesOfWord *************************/
/** Reverse bits in the two bytes of an 16bit word
 *  This needs to be done only on data words when updating A404 and A500 FPGAs
 *
 *---------------------------------------------------------------------------
 *  \param	inWord		\IN		word to be processed
 *
 *  \return	word with bits reversed in each byte
 ******************************************************************************/
static u_int16 ReverseBitsInBytesOfWord(u_int16 inWord)
{
	u_int16 temp =  ((inWord & 0x0001) << 7) |
					((inWord & 0x0002) << 5) |
					((inWord & 0x0004) << 3) |
					((inWord & 0x0008) << 1) |
					((inWord & 0x0010) >> 1) |
					((inWord & 0x0020) >> 3) |
					((inWord & 0x0040) >> 5) |
					((inWord & 0x0080) >> 7) |
					((inWord & 0x0100) << 7) |
					((inWord & 0x0200) << 5) |
					((inWord & 0x0400) << 3) |
					((inWord & 0x0800) << 1) |
					((inWord & 0x1000) >> 1) |
					((inWord & 0x2000) >> 3) |
					((inWord & 0x4000) >> 5) |
					((inWord & 0x8000) >> 7);

	return( temp );
}

#if 0  /* erase sectors currently not supported by PLD */
/********************************* FindSect ***********************************/
/** Convert dev address to sector number
 *
 *---------------------------------------------------------------------------
 *  \param	fDev		\IN		FLASH_DEVSL handle
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

#endif /* #if 0 */



#endif /* Z100_CONFIG_SMB */

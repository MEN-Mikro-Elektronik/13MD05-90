/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  istratapc28fxxxp30.c
 *
 *       \author  Christian.Kauntz@men.de
 *        $Date: 2012/03/14 13:26:20 $
 *    $Revision: 2.5 $
 *
 *        \brief  command set for IntelStrata PC28F640P30 and compatible flashes
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
 * $Log: istratapc28fxxxp30.c,v $
 * Revision 2.5  2012/03/14 13:26:20  dpfeuffer
 * R: Windows compiler warning
 * M: cast added
 *
 * Revision 2.4  2009/01/12 16:58:37  CKauntz
 * R: 1. In non debugged version no flash name visible
 *    2. Write behind the flash size
 * M: 1.a: changed to printf to show name in verbose mode
 *      b: changed flash name to show size in MB
 *    2. No write or delete when address greater than flash size
 *       No write but delete of sectors when start within but end behind the size
 *
 * Revision 2.3  2008/03/19 16:11:05  CKauntz
 * fixed: delete next sector of the end of the sector
 *
 * Revision 2.2  2007/07/13 17:28:13  cs
 * added newline at end of file
 *
 * Revision 2.1  2007/07/09 20:12:56  CKauntz
 * Initial Revision
 *
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2007 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include "fpga_load.h"
#include "fpga_load_flash.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#if defined(_LITTLE_ENDIAN_)
# define SW16(dword) 	(dword)
#elif defined(_BIG_ENDIAN_)
# define SW16(dword) OSS_SWAP16(dword)
#else
# error "Define _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif /* _BIG/_LITTLE_ENDIAN_ */

/*--- command codes for INTEL ---*/
#define C_READARRAY        	0xFF	/* read data */
#define C_READDEVID			0x90	/* read device identifier */
#define C_READCFIQUERY      0x98	/* read CFI Query */
#define C_READSTATREG       0x70	/* read satus register */
#define C_CLEARSTATREG  	0x50	/* clear status register */
#define C_PROGRAMWORD		0x40	/* word program */
#define C_PROGRAMBUFFERD	0xE8	/* program buffer */
#define C_PROGRAMBEFP1		0x80	/* Buffered Enhanced Factory Programm */
#define C_PROGRAMBEFP2		0xD0	/* BEFP */
#define C_SUSPEND			0xB0	/* suspend program/erase */
#define C_RESUME			0xD0	/* resueme program/erase */
#define C_LOCKUNLOCK		0x60	/* Lock Block first cycle */
#define C_LOCKBLOCK			0x01	/* Lock Block second cycle */
#define C_UNLOCKBLOCK		0xD0	/* Unlock Block second cycle */
#define C_LOCKDOWNBLOCK		0xF2	/* Lock-Down Block second cycle */
#define C_PROGPROTLOCREG	0xC0	/* Program Protection/Lock Register */
#define	C_CONFIGRDREG1		0x60	/* Program Read Configuration Register */
#define	C_CONFIGRDREG2		0x03	/* Program Read Configuration Register */
#define C_BLOCKERASE1		0x20	/* first command for block erase */
#define C_BLOCKERASE2		0xD0	/* second command for block erase */

#define INTELMAXBYTES		0x800000	/* maximum bytes to be erased */


/*--- known manufacturer codes/device codes ---*/
#define MANUFACT_INTEL 			0x0089
#define DEVCODE_PC28F640P30T	0x8817
#define DEVCODE_PC28F640P30B	0x881A
#define DEVCODE_PC28F128P30T	0x8818
#define DEVCODE_PC28F128P30B	0x881B
#define DEVCODE_PC28F256P30T	0x8919
#define DEVCODE_PC28F256P30B	0x891C


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
static int32 UnlockBlock(FLASH_DEVS *fDev, u_int32 startOffs, u_int32 len);

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
	{ "INTELSTRATAPC28F640P30 8M", MANUFACT_INTEL,DEVCODE_PC28F640P30T,0x10000,67,BOOT_SECT_TOP,
			{
				0x000000, 0x010000, 0x020000, 0x030000, 0x040000, 0x050000,
				0x060000, 0x070000, 0x080000, 0x090000, 0x0A0000, 0x0B0000,
				0x0C0000, 0x0D0000, 0x0E0000, 0x0F0000, 0x100000, 0x110000,
				0x120000, 0x130000, 0x140000, 0x150000, 0x160000, 0x170000,
				0x180000, 0x190000, 0x1A0000, 0x1B0000, 0x1C0000, 0x1D0000,
				0x1E0000, 0x1F0000, 0x200000, 0x210000, 0x220000, 0x230000,
				0x240000, 0x250000, 0x260000, 0x270000, 0x280000, 0x290000,
				0x2A0000, 0x2B0000, 0x2C0000, 0x2D0000, 0x2E0000, 0x2F0000,
				0x300000, 0x310000, 0x320000, 0x330000, 0x340000, 0x350000,
				0x360000, 0x370000, 0x380000, 0x390000, 0x3A0000, 0x3B0000,
				0x3C0000, 0x3D0000, 0x3E0000, 0x3F0000, 0x3F4000, 0x3F8000,
				0x3FC000, 0x400000
			}
     },
	 { "INTELSTRATAPC28F640P30 8M", MANUFACT_INTEL,DEVCODE_PC28F640P30B,0x10000,67,BOOT_SECT_BOTTOM,
			{
				0x000000, 0x004000, 0x008000, 0x00C000, 0x010000, 0x020000,
				0x030000, 0x040000, 0x050000, 0x060000, 0x070000, 0x080000,
				0x090000, 0x0A0000, 0x0B0000, 0x0C0000, 0x0D0000, 0x0E0000,
				0x0F0000, 0x100000, 0x110000, 0x120000,	0x130000, 0x140000,
				0x150000, 0x160000, 0x170000, 0x180000, 0x190000, 0x1A0000,
				0x1B0000, 0x1C0000, 0x1D0000, 0x1E0000,	0x1F0000, 0x200000,
				0x210000, 0x220000,	0x230000, 0x240000, 0x250000, 0x260000,
				0x270000, 0x280000,	0x290000, 0x2A0000, 0x2B0000, 0x2C0000,
				0x2D0000, 0x2E0000,	0x2F0000, 0x300000, 0x310000, 0x320000,
				0x330000, 0x340000, 0x350000, 0x360000, 0x370000, 0x380000,
				0x390000, 0x3A0000, 0x3B0000, 0x3C0000, 0x3D0000, 0x3E0000,
				0x3F0000, 0x400000
			}
     },
	{ "", 0 }
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
extern int32 Z100_ISTRATAPC28FXXXP30_TRY(DEV_HDL *devHdl,
								FLASH_INITP *flash_initP,
								u_int32 dbgLevel)
{
	int32 error=0;
	FLASH_DEVS fDev;

	DBGOUT(( "FLASH::istratapc28fxxxp30::Try\n" ));

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

	DBGOUT(( "FLASH::istratapc28fxxxp30::Try\n"
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

	DBGOUT(("FLASH::istratapc28fxxxp30::Try finished 0x%04x\n", (int)error));
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
	DBGOUT(( "FLASH::istratapc28fxxxp30::Exit\n" ));

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
	const FLASH_DEVS *devs = G_Devs;
	DEV_HDL *h = devHdl;

	DBGOUT(( "FLASH::istratapc28fxxxp30::Identify\n" ));

	flash_acc = devHdl->flash_acc_size;
	devHdl->flash_acc_size = Z100_FLASH_ACCESS_16BIT;		/* 16 bit Flash */

	/*--- write devId command ---*/
	Z100_FLASH_WRITE( devHdl, (0x0),(C_READDEVID));


	if( (retval = (Z100_FLASH_READ(devHdl, 0x0)) & 0xffff) == MANUFACT_INTEL)
	{
		devHdl->flashDev.manId = retval & 0xffff;

		/*--- read devId ---*/
		retval = Z100_FLASH_READ( devHdl, 0x2 );

		while (devs->devId){

			/*--- check if devId equal to one of IntelDevIds ---*/
			if((u_int32)devs->devId ==  retval)
			{
				devHdl->flashDev.devId = retval;
				break;
			}
			devs++;
		}
	}

	if( devHdl->dbgLevel )
		printf( "FLASH::istratapc28fxxxp30::Identify mnf=%04x dev=%04x\n",
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
	DBGOUT(( "FLASH::istratapc28fxxxp30::CheckId mnf=%04x dev=%04x\n",
			(unsigned int)devHdl->flashDev.manId,
			(int)devHdl->flashDev.devId) );

	/*--- check if device is supported ---*/
	if( devHdl->flashDev.manId == MANUFACT_INTEL )
	{
		const FLASH_DEVS *d = G_Devs;
		int32 i;

		while( d->devId ){
			if( ( devHdl->flashDev.devId == d->devId ) &&
			 	( devHdl->flashDev.manId == d->manId) )
			{
				if(devHdl->dbgLevel)
					printf( "FLASH::istratapc28fxxxp30::CheckId\n"
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
	DBGOUT(( "FLASH::istratapc28fxxxp30::CheckId finished, no supported dev\n"));
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

	DBGOUT(( "FLASH::istratapc28fxxxp30::EraseChip\n" ));

	error = EraseSectors(fDev, 0x000000, INTELMAXBYTES );

	if( error )
		Reset(fDev);

	DBGOUT(( "FLASH::istratapc28fxxxp30::EraseChip finished 0x%04x\n", (int)error ));
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

	DBGOUT(( "FLASH::istratapc28fxxxp30::EraseSectors\n" ));

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

	DBGOUT(( "FLASH::istratapc28fxxxp30::EraseSectors\n"
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

		/*--- clear status register ---*/
		Z100_FLASH_WRITE( fDev->devHdl, phyStart, C_CLEARSTATREG);

		/*--- unlock blocked sector ---*/
		UnlockBlock(fDev, phyStart, len);

		/*--- give CHIP ERASE commands ---*/
		Z100_FLASH_WRITE( fDev->devHdl, phyStart, C_BLOCKERASE1);
		Z100_FLASH_WRITE( fDev->devHdl, phyStart, C_BLOCKERASE2);

		error = DataPollingAlg(fDev->devHdl, phyStart, 0xFF, 10000);
		if( error )
			break;

		if( fDev->devHdl->dbgLevel )
			printf("    Erase Sector ----> OK\n");
		if( phyStart == phyEnd )
			break;
		phyStart += sectSize;
	} while(1);

end:
	DBGOUT(( "FLASH::istratapc28fxxxp30::EraseSectors finished 0x%04x\n",
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
	u_int32 val;
	u_int16 *bufp16 = NULL;
	int32 error=0, nWrite;
	u_int32 offset = offs;
	DEV_HDL *h = fDev->devHdl;

	DBGOUT(( "FLASH::istratapc28fxxxp30::WriteBlock offs=0x%08x len=%d\n",
			 (int)offs, (int)len ));

	/* check write behind the end of the flash */
    if( ( offs >= fDev->sectAddr[fDev->nSectors]*2) ||
        ((offs + len) > fDev->sectAddr[fDev->nSectors]*2))
    {
        printf( "*** Write behind the flash memory!\n");
        error = ERR_FLASH_WRITE_FAILED;
        goto end;
    }

	/* access 16 bit data bus */
	nWrite = len / 2 + len%2;
	bufp16 = (u_int16*)buf;

	while( nWrite ){
		/*--- clear status register ---*/
		Z100_FLASH_WRITE( h, offset, C_CLEARSTATREG);

		/*--- issue command cycles ---*/
		Z100_FLASH_WRITE( h, offset, C_PROGRAMWORD);	/* 1st cycle */
		val = *(u_int16*)bufp16++;
		Z100_FLASH_WRITE( h, offset, SW16(val));		/* 2nd cycle */

		if( (error = DataPollingAlg(h, offset, val, 10000)) )
			goto end;

		/* access 16 bit data bus */
		nWrite--;
		offset+=2;
	}

end:
	if( error )
		Reset(fDev);

	DBGOUT(( "FLASH::istratapc28fxxxp30::WriteBlock finished 0x%04x\n",
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
	u_int32 retVal, offset = offs; /* offs always a multiple of 2 */
	u_int32 nAccess; /* number of accesses necessary */
	u_int16 *bufp16 = NULL;
	DEV_HDL *h = fDev->devHdl;

	DBGOUT(( "FLASH::istratapc28fxxxp30::ReadBlock offs=0x%08x len=0x%08x\n",
			 (int)offs, (int)len ));

	/* access 16 bit data bus */
	nAccess = len / 2 + len%2;
	bufp16 = (u_int16*)buf;

	while(nAccess>0){

		retVal = Z100_FLASH_READ( fDev->devHdl, offset);
		/* access 16 bit data bus */
		*bufp16++ = SW16((u_int16)retVal);
		nAccess--;
		offset+=2;
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
	Z100_FLASH_WRITE( fDev->devHdl, 0x00, C_READARRAY);
}


/********************************* DataPollingAlg *****************************/
/** Perform Data Polling Algorithm (after write/erase commands)
 *
 *-----------------------------------------------------------------------------
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

		Z100_FLASH_WRITE( h, 0x0, C_READSTATREG );
		retVal = Z100_FLASH_READ( h, 0x0 );
		retVal = (retVal);

		/*--- check state SR7 ---*/
		if ( retVal & 0x80 ){	/*--- WSM ready ---*/

			if (( retVal & 0x08 ) 	|| 	/* SR3 VPP ERROR */
				( retVal & 0x10 ) 	||	/* SR4 DATA PROGRAM ERROR */
				( retVal & 0x20 ) 	||	/* SR5 BLOCK ERASE ERROR */
				( retVal & 0x02 ) ){	/* SR1 DEVICE PROTECT ERROR */
				error = ERR_FLASH_WRITE_FAILED;
			}

			/*--- clear status register ---*/
			Z100_FLASH_WRITE( h, 0x0, C_CLEARSTATREG);
			/*--- set read array mode ---*/
			Z100_FLASH_WRITE( h, 0x0, C_READARRAY);
			break;
		}

		if( MSECDIFF(baseMsec) > toutMsecs){
			printf( "*** FLASH::Program Timeout\n");
			error = ERR_UOS_TIMEOUT;
			goto end;
		}

	} /* while (1)*/

end:
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
		if( addr >= fDev->sectAddr[sect]*2 &&
			addr < fDev->sectAddr[sect+1]*2){
			*phySizeP = (fDev->sectAddr[sect+1] - fDev->sectAddr[sect])*2;
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
 *  \param	startOffs	\IN		address in first sector to be erased
 *  \param	len			\IN		length (in bytes) to be erased
 *  \return	success (0) or error code
 ******************************************************************************/
static int32 UnlockBlock(FLASH_DEVS *fDev, u_int32 startOffs, u_int32 len)
{
	int32   error = 0;
	DEV_HDL *h = fDev->devHdl;

	/* unlock sectors */
	if( fDev->devHdl->dbgLevel )
		printf( "Unlocking sectoraddr %06x \n",(int)startOffs);

	/*--- give UNLOCK BLOCK command ---*/
	if ( fDev->manId == MANUFACT_INTEL ) {
		Z100_FLASH_WRITE( fDev->devHdl, startOffs, C_LOCKUNLOCK  );
		Z100_FLASH_WRITE( fDev->devHdl, startOffs, C_UNLOCKBLOCK );
	}
	error = DataPollingAlg(fDev->devHdl, startOffs, 0xFF, 10000);
	if ((!error) && ( fDev->devHdl->dbgLevel ))
		printf("    Unlock Block ----> OK\n");

	return error;
}



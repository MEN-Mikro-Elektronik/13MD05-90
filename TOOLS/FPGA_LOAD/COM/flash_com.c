/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  flash_com.c
 *
 *       \author  Christian.Schuster@men.de
 *        $Date: 2009/03/05 09:52:48 $
 *    $Revision: 1.9 $
 *
 *        \brief  common command set for all flash memory devices
 *
 *
 *     Required: -
 *     \switches (none)
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 *  Z100_FLASH_READ_BLOCK       read specified number of bytes from device
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: flash_com.c,v $
 * Revision 1.9  2009/03/05 09:52:48  CKauntz
 * R: 1. Compiler warning internFunc declared but not defined
 *    2. Copyright not up to date
 * M: 1. Added undef to reduce compiler warning
 *    2. Updated copyright
 *
 * Revision 1.8  2008/03/19 16:11:01  CKauntz
 * added:
 *  + swapped version for BIG and LITTLE endian for the AMD29GL064 flash
 *
 * Revision 1.7  2007/07/09 20:10:55  CKauntz
 * added: devHdl for Makro
 *
 * Revision 1.6  2005/12/09 17:21:48  cschuster
 * Copyright string changed
 *
 * Revision 1.5  2005/11/24 18:19:33  cschuster
 * changed FLASH_READ macro to function call Z100_FLASH_READ
 *
 * Revision 1.4  2005/07/08 17:49:46  cs
 * cosmetics
 *
 * Revision 1.3  2005/01/31 13:58:14  cs
 * added casts to printf to avoid warnings when compiling VxWorks tool
 * changed verbose printf to DBGOUT
 *
 * Revision 1.2  2004/12/23 15:10:43  cs
 * moved prototypes to include file
 *
 * Revision 1.1  2004/11/30 18:04:56  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004-2009 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#undef PROG_FILE_NAME
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

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

/********************************* Z100_ReadBlocks ****************************/
/** Standard Read routine for all flash devices
 *
 *---------------------------------------------------------------------------
 *  \param	fDev	\IN		FLASH_DEVS handle
 *  \param	offs	\IN		start offset within device
 *  \param	len		\IN		length (in bytes) to be read
 *  \param	buf		\IN		destination buffer
 *
 *  \return	success (0)
 ****************************************************************************/
extern int32 Z100_FLASH_READ_BLOCK(
    FLASH_DEVS *fDev,
	u_int32 offs,
    u_int32 len,
    u_int8  *buf)
{
	u_int32 retVal, offset = offs; /* offs always a multiple of 2 */
	u_int32 nAccess; /* number of accesses necessary */
	u_int8  *bufp8 = NULL;
	u_int16 *bufp16 = NULL;
	DEV_HDL *h = fDev->devHdl;

	DBGOUT(( "FLASH::COM::ReadBlock offs=0x%08x len=0x%08x\n",
			 (int)offs, (int)len ));

	if( fDev->devHdl->flash_acc_size )/* access 16 bit data bus */
	{
		nAccess = len / 2 + len%2;
		bufp16 = (u_int16*)buf;
	} else {
		nAccess = len;
		bufp8 = (u_int8*)buf;
	}

	while(nAccess>0){

		/*--- read word by word or byte by byte, extracting valid bytes ---*/
		retVal = Z100_FLASH_READ( fDev->devHdl, offset);
		/*printf( "%s(%d) naccess = %4d, offset: 0x%08x, read value: 0x%08x fl_acc_s: 0x%02x\n",
				__FUNCTION__, __LINE__, nAccess, offset, retVal,fDev->devHdl->flash_acc_size);
		*/
		if( fDev->devHdl->flash_acc_size )/* access 16 bit data bus */
		{
			if (fDev->devHdl->flashDev.devId == 0x227E){
			    retVal = SW16((u_int16)retVal);
			}
			*bufp16++ = (u_int16)retVal;
			nAccess--;
			offset+=2;
		} else {
			*bufp8++ = (u_int8)retVal;
			nAccess--;
			offset++;
		}
	}
	return 0;
}


/************************  F P G A _ L O A D  *********************************/
/*!
 *
 *        \file  hw_acc.c
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2013/06/21 16:45:57 $
 *    $Revision: 2.7 $
 *
 *       \brief  HW access functions, \n
 *				 To be included by each stub and set switches for the swapped versions
 *
 *    \switches FL_SWAP_SW       - for swapped Version\n
 *              FL_SWAP_MMOD_SW  - for swapped M-Module\n
 *              FL_SWAP_MMOD     - for M-Module\n
 *
 *
 *
 */
/*---------------------------[ Public Functions ]-------------------------------
 * none
 *
 *---------------------------------[ History ]----------------------------------
 *
 * $Log: hw_acc.c,v $
 * Revision 2.7  2013/06/21 16:45:57  MRoth
 * R: include path for fpga_load header not in default paths
 * M: change include to current path
 *
 * Revision 2.5  2008/03/19 16:10:57  CKauntz
 * cosmetics
 *
 * Revision 2.4  2007/08/07 14:06:58  JWu
 * changed:
 *   type of arguments in Z100_Mwrite_MEN_D8, Z100_Mwrite_MEN_D16
 *
 * Revision 2.3  2007/07/09 20:10:49  CKauntz
 * changes: all functions to static cause of including in the stubs
 *          Mread / Mwrite adapted to swapped and m-module swapped versions
 *          Decision for IO or Mem version moved to Identyfy functions
 *
 * Revision 2.2  2006/03/16 12:32:50  cs
 * fixed: D16 and D32 read access
 *
 * Revision 2.1  2005/12/12 11:05:22  cschuster
 * Initial Revision
 *
 *
 *
 *------------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*-----------------------------------------+
 |  GLOBALS                                |
 +-----------------------------------------*/

/*-----------------------------------------+
 |  DEFINES                                |
 +-----------------------------------------*/
#ifndef MAC_MEM_MAPPED
#	define MAC_MEM_MAPPED
#endif

#ifdef MAC_IO_MAPPED
#	undef MAC_IO_MAPPED
#endif

#include <MEN/men_typs.h>
#include <MEN/maccess.h>
#include <fpga_load.h>
/*-----------------------------------------+
 |  TYPEDEFS                               |
 +-----------------------------------------*/
/*-----------------------------------------+
 |  PROTOTYPES                             |
 +-----------------------------------------*/

#if (  defined FL_SWAP_SW || defined FL_SWAP_MMOD_SW )
#define SWAP_VALUE { temp = OSS_SWAP16(temp);	}
#define SWAP_OFFS { temp = OSS_SWAP32(temp);	}
#else
#define SWAP_OFFS { temp = temp; }
#define SWAP_VALUE { temp = temp; }
#endif /* FL_SWAP_SW|FL_SWAP_MMOD_SW */

#if (  defined FL_SWAP_MMOD_SW || defined FL_SWAP_MMOD )

	static u_int32 ChWordsInLW(u_int32 dword)
	{
		return (u_int32)(((dword >> 16 ) & 0x0000ffff) | ((dword << 16 ) & 0xffff0000));
	}
#define SWAP_WORDS { temp = ChWordsInLW(temp); }
#else
#define SWAP_WORDS { temp = temp; }
#endif /* FL_SWAP_MMOD_SW|FL_SWAP_MMOD */



static u_int8  Z100_Mread_Mem_D8(
	void *ma,
	u_int32 offs
)
{
	u_int8 temp = MREAD_D8( (MACCESS)ma, offs );
	return( temp );
}
static u_int16 Z100_Mread_Mem_D16(
	void *ma,
	u_int32 offs
)
{
	u_int16 temp;
	temp = MREAD_D16( (MACCESS)ma, offs );
	SWAP_VALUE
	return( temp );
}
static u_int32 Z100_Mread_Mem_D32(
	void *ma,
	u_int32 offs
)
{
	u_int32 temp;
	temp = MREAD_D32( (MACCESS)ma, offs );
	SWAP_WORDS
	SWAP_OFFS
	return( temp );
}

static void Z100_Mwrite_Mem_D8(
	void *ma,
	u_int32 offs,
	u_int8 val
)
{
	MWRITE_D8( (MACCESS)ma, offs, val );
	return;
}
static void Z100_Mwrite_Mem_D16(
	void *ma,
	u_int32 offs,
	u_int16 val
)
{
	u_int16 temp;
	temp = val;
	SWAP_VALUE
	MWRITE_D16( (MACCESS)ma, offs, temp );
	return;
}
static void Z100_Mwrite_Mem_D32(
	void *ma,
	u_int32 offs,
	u_int32 val
)
{
	u_int32 temp;
	temp = val;
	SWAP_WORDS
	SWAP_OFFS
	MWRITE_D32( (MACCESS)ma, offs, temp );
	return;
}

static void Z100_Flash_Write( DEV_HDL *h, u_int32 offs, u_int32 val )
{
	if( !h->smbLocHdl.smbHdl )
	{
		Z100_MWRITE_D32( h->mappedAddr,
						 Z045_FLASH_ADDR_REG_OFFSET,
						 offs);
		if(h->flash_acc_size) /* access 16 bit data bus */
			Z100_MWRITE_D16( h->mappedAddr,
							 Z045_FLASH_DATA_REG_OFFSET,
							 (u_int16)val);
		else				/* access 8 bit data bus */
			Z100_MWRITE_D8( h->mappedAddr,
							Z045_FLASH_DATA_REG_OFFSET,
							(u_int8)val);
		/* printf( "FLASH_WRITE MEM addr=0x%08x offs=0x%08x; val=0x%08x\n",
				(unsigned int)h->mappedAddr, (unsigned int)offs, (unsigned int)val); */
	} else { /* access over smb */
		printf( "Z100_FLASH_WRITE: SMB access not implemented here\n");
	}
}

static u_int32 Z100_Flash_Read( DEV_HDL *h, u_int32 offs )
{
	u_int32 retVal = 0;
	if( !h->smbLocHdl.smbHdl )
	{
		Z100_MWRITE_D32( h->mappedAddr,
						 Z045_FLASH_ADDR_REG_OFFSET,
						 offs);
		if(h->flash_acc_size) /* access 16 bit data bus */
			retVal = Z100_MREAD_D16( h->mappedAddr,
									 Z045_FLASH_DATA_REG_OFFSET);
		else				/* access 8 bit data bus */
			retVal = Z100_MREAD_D8( h->mappedAddr,
									Z045_FLASH_DATA_REG_OFFSET);
		/* printf("FLASH_READ MEM addr=0x%08x; offs=0x%08x; val = 0x%08x\n",
				 (unsigned int)h->mappedAddr, (unsigned int)offs, (unsigned int)retVal); */
	} else
	{
		printf( "Z100_FLASH_READ: SMB access not implemented here\n");
		retVal = -1;
	}
	return retVal;
}

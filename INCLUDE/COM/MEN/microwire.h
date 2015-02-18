/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: microwire.h
 *
 *       Author: uf
 *        $Date: 2000/03/08 14:01:54 $
 *    $Revision: 2.3 $
 *
 *  Description: microwire bus protocol interface
 *     Switches: MCRW_COMPILE - for module compilation
 *				 ID_SW		  - for swapped access
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: microwire.h,v $
 * Revision 2.3  2000/03/08 14:01:54  kp
 * support swapped access
 *
 * Revision 2.2  1999/07/30 14:32:48  Franke
 * cosmetics
 *
 * Revision 2.1  1999/07/08 11:15:33  Franke
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
#ifndef _MCRW_H
#  define _MCRW_H

#  ifdef __cplusplus
      extern "C" {
#  endif

/*----------------+
| Variant defines |
+----------------*/
#ifdef ID_SW
# define MCRW_PORT_Init	MCRW_SW_PORT_Init	
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/* descriptor for the PORT emulated MCRW controller */
typedef struct
{
	u_int8     busClock;    /* 0  -   max - no internal delay
							 * 1  -  1kHz - OSS_Delay() call
							 * 10 - 10kHz - OSS_MikroDelay() call
							 */
	u_int8     addrLength;  /* depend on device type , e.g.NM93C46 has 6 bit addresses */

	void       *addrDataIn;
	u_int8     flagsDataIn;   /* access size, polarity */
	u_int32    maskDataIn;

	void       *addrDataOut;
	u_int8     flagsDataOut;  /* access size, polarity, readable register */
	u_int32    maskDataOut;
	u_int32    notReadBackDefaultsDataOut;
	u_int32    notReadBackMaskDataOut;

	void       *addrClockOut;
	u_int8     flagsClockOut; /* access size, polarity, readable register */
	u_int32    maskClockOut;
	u_int32    notReadBackDefaultsClockOut;
	u_int32    notReadBackMaskClockOut;

	void       *addrCsOut;
	u_int8     flagsCsOut;    /* access size, polarity, readable register */
	u_int32    maskCsOut;
	u_int32    notReadBackDefaultsCsOut;
	u_int32    notReadBackMaskCsOut;

	u_int8     flagsOut;      /* all in one */
}MCRW_DESC_PORT;


typedef struct
{
    char* (*Ident)       ( void );
    int32 (*Exit)        ( void **mcrwHdlP );
    int32 (*WriteEeprom) ( void *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size ); /* write a block, size in byte */
    int32 (*ReadEeprom)  ( void *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size ); /* read a block, size in byte */
    int32 (*SetStat)	 ( void *mcrwHdl, int32 code,  int32 data   );
    int32 (*GetStat)	 ( void *mcrwHdl, int32 code,  int32 *dataP );
}MCRW_ENTRIES;

#ifndef MCRW_COMPILE
	typedef MCRW_ENTRIES MCRW_HANDLE;
#endif /*MCRW_COMPILE*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define MCRW_ERR_NO           		0   /* OK */
#define MCRW_ERR_DESCRIPTOR   		1   /* descriptor is wrong */
#define MCRW_ERR_NO_MEM       		2   /* no memory */
#define MCRW_ERR_UNK_CODE      		3   /* unknown status code */
#define MCRW_ERR_ERASE      		4   /* erase */
#define MCRW_ERR_WRITE      		5   /* write */
#define MCRW_ERR_WRITE_VERIFY  		6   /* verify failed */
#define MCRW_ERR_ADDR      		    7   /* address in EEPROM */
#define MCRW_ERR_BUF 		    	8   /* buffer not aligned */
#define MCRW_ERR_BUF_SIZE 		    9   /* buffer size */

#define MCRW_DESC_PORT_FLAG_SIZE_MASK		0x07
#define MCRW_DESC_PORT_FLAG_SIZE_8			0x01
#define MCRW_DESC_PORT_FLAG_SIZE_16			0x02
#define MCRW_DESC_PORT_FLAG_SIZE_32			0x04

#define MCRW_DESC_PORT_FLAG_POLARITY_HIGH	0x10
#define MCRW_DESC_PORT_FLAG_READABLE_REG	0x20

#define MCRW_DESC_PORT_FLAG_OUT_IN_ONE_REG  0x01  /* all out bits are in one register */

#define MCRW_IOCTL_BUS_CLOCK				0x01  /* set/getstat to change the bus clock */
#define MCRW_IOCTL_ADDR_LENGTH				0x02  /* set/getstat to change the address length */


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
u_int32 MCRW_PORT_Init( MCRW_DESC_PORT *descP, void *osHdl, void **mcrwHdlP );

#  ifdef __cplusplus
      }
#  endif

#endif/*_MCRW_H*/


/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysmanagbus_port.c
 *      Project: SMB_PORT library
 *
 *       Author: uf
 *        $Date: 2004/10/14 16:09:12 $
 *    $Revision: 1.9 $
 *
 *  Description: system managment bus driver for a port
 *				 SMB/IIC protocol emulation.
 *				 Single master is implemented only.
 *
 *
 *     Required: -
 *     Switches: -
 *
 *		   Note: D16/D32 access,
 *				 !SMB_DESC_PORT_FLAG_POLARITY_HIGH
 *				 and !SMB_DESC_PORT_FLAG_READABLE_REG   not tested
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysmanagbus_port.c,v $
 * Revision 1.9  2004/10/14 16:09:12  dpfeuffer
 * casts added
 *
 * Revision 1.8  2003/06/06 09:40:20  kp
 * fixed compiler warnings
 *
 * Revision 1.7  2001/09/18 15:22:50  kp
 * use different names for SMB_PORT_Init depending in access mode IO/MEM
 *
 * Revision 1.6  2000/09/08 10:26:35  Schmidt
 * 1) SMB_PORT_Init : cast for WriteReadTwoByte was wrong
 * 2) nodoc to non-public functions added
 *
 * Revision 1.5  2000/08/03 19:28:44  Franke
 * added WriteReadTwoByte as NULL pointer
 *
 * Revision 1.3  2000/06/09 09:48:59  Schmidt
 * 1) include <MEN/dbg.h> added
 * 2) __inline__ define for VC added
 * 3) smbIdent function added / SMB_PORT_Init sets entry for Ident function
 *
 * Revision 1.2  1999/03/04 08:35:53  kp
 * 1) dummy define for __inline__ in case of Ultra-C
 * 2) replaced all NULL args to MREAD/MWRITE macros with 0
 * 3) fixed bad casts for entries
 *
 * Revision 1.1  1999/03/03 13:38:21  Franke
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/maccess.h>

#define SMB_COMPILE
#include <MEN/sysmanagbus.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
typedef struct
{
	/* function entries */
	SMB_ENTRIES entries;

	/* data */
	u_int32       ownSize;
	OSS_HANDLE 	  *osHdl;
	SMB_DESC_PORT desc;
}SMB_HANDLE;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define SMB_READ           0x01
#define SMB_WRITE          0x00

#ifdef _UCC
/* Ultra-C has no inline funcs */
# define __inline__
#endif

#ifdef _MSC_VER
/* Visual C++ needs inline for inline funcs */
# define __inline__ inline
#endif

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static u_int32 smbExit(         SMB_HANDLE  **smbHdlP );
static u_int32 smbWriteByte(    SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 data );
static u_int32 smbReadByte(     SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 *dataP );
static u_int32 smbWriteTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 data );
#if 0
static u_int32 smbWriteReadBlock(	SMB_HANDLE  *smbHdl, u_int8 addr, u_int32 size,	u_int8 *buf, u_int8 cmdOrIndex );
static u_int32 smbWriteBlock    (	SMB_HANDLE  *smbHdl, u_int8 addr, u_int32 size,	u_int8 *buf );
#endif
static char* smbIdent( void );

/************************************* delay ********************************
 *  Description:  Delay	descriptor dependend time.
 *
 *		   Note:  busClock descriptor value
 *					 0 - max speed - no delay
 *					 1 -  1kHz  OSS_Delay()
 *					10 - 10kHz  OSS_MikroDelay()
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
__inline__ static void delay	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	switch( smbHdl->desc.busClock )
	{
		case 1:  /* 1kHz max */
			OSS_Delay( smbHdl->osHdl, 1 );
			break;
		case 10: /* 10kHz max */
			OSS_MikroDelay( smbHdl->osHdl, 100 );
			break;
		default: /* max speed */
			break;
	}/*switch*/
}/*delay*/

/******************************** sclIn *************************************
 *  Description:  Read the SCL line.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return 0 - low, 1 -high
 *  Globals....:  -
 ****************************************************************************/
static int sclIn	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
u_int32 val;
	/* wait a little before read */
	delay( smbHdl );
	/* read with correct access size */
	switch( smbHdl->desc.flagsClockIn & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			val = MREAD_D32( smbHdl->desc.addrClockIn, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			val = MREAD_D16( smbHdl->desc.addrClockIn, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			val = MREAD_D8(  smbHdl->desc.addrClockIn, 0 );
	}/*switch*/

	/* mask bit / polarity */
	val &= smbHdl->desc.maskClockIn;
	
	if( val ) if( smbHdl->desc.flagsClockIn & SMB_DESC_PORT_FLAG_POLARITY_HIGH )return( 1 );
			  else return( 0 );
	else	  if( smbHdl->desc.flagsClockIn & SMB_DESC_PORT_FLAG_POLARITY_HIGH )return( 0 );
			  else return( 1 );
}/*sclIn*/

/******************************** sclHigh ***********************************
 *  Description:  Release SCL line.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl pointer to smb handle
 *  Output.....:  return 0 - OK | 1 if stuck at low
 *  Globals....:  -
 ****************************************************************************/
static int sclHigh	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
u_int32 val;
	/* read with correct access size */
	switch( smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			val = MREAD_D32( smbHdl->desc.addrClockOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			val = MREAD_D16( smbHdl->desc.addrClockOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			val = MREAD_D8(  smbHdl->desc.addrClockOut, 0 );
	}/*switch*/

	/* not read back bits vernibbling */
	if( !(smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_READABLE_REG) )
	{
		val &= ~smbHdl->desc.notReadBackMaskClockOut;
		val |= smbHdl->desc.notReadBackDefaultsDataOut;
	}/*if*/

	val &= ~smbHdl->desc.maskClockOut;
	if( smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_POLARITY_HIGH )
		val |= smbHdl->desc.maskClockOut;

	/* write with correct access size */
	switch( smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			MWRITE_D32( smbHdl->desc.addrClockOut, 0, val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			MWRITE_D16( smbHdl->desc.addrClockOut, 0, (u_int16)val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			MWRITE_D8(  smbHdl->desc.addrClockOut, 0, (u_int8)val );
	}/*switch*/

	/* check SCL is high */
	if( sclIn( smbHdl ) )
		return( 0 );
	else
		return( 1 );
}/*sclHigh*/

/****************************** sclLow *****************************
 *  Description:  Drive SCL line to low.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl pointer to smb handle
 *  Output.....:  return 0 | 1 if stuck at high
 *  Globals....:  -
 ****************************************************************************/
static int sclLow	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
u_int32 val;
	/* read with correct access size */
	switch( smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			val = MREAD_D32( smbHdl->desc.addrClockOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			val = MREAD_D16( smbHdl->desc.addrClockOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			val = MREAD_D8(  smbHdl->desc.addrClockOut, 0 );
	}/*switch*/

	/* not read back bits vernibbling */
	if( !(smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_READABLE_REG) )
	{
		val &= ~smbHdl->desc.notReadBackMaskClockOut;
		val |= smbHdl->desc.notReadBackDefaultsDataOut;
	}/*if*/

	val |= smbHdl->desc.maskClockOut;
	if( smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_POLARITY_HIGH )
		val &= ~smbHdl->desc.maskClockOut;

	/* write with correct access size */
	switch( smbHdl->desc.flagsClockOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			MWRITE_D32( smbHdl->desc.addrClockOut, 0, val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			MWRITE_D16( smbHdl->desc.addrClockOut, 0, (u_int16)val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			MWRITE_D8(  smbHdl->desc.addrClockOut, 0, (u_int8)val );
	}/*switch*/

	/* check SCL is low */
	if( sclIn( smbHdl ) )
		return( 1 );
	else
		return( 0 );
}/*sclLow*/

/******************************** sdaIn *************************************
 *  Description:  Read the SDA line.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return 0 - low, 1 -high
 *  Globals....:  -
 ****************************************************************************/
static int sdaIn	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
u_int32 val;
	/* wait a little before read */
	delay( smbHdl );
	/* read with correct access size */
	switch( smbHdl->desc.flagsDataIn & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			val = MREAD_D32( smbHdl->desc.addrDataIn, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			val = MREAD_D16( smbHdl->desc.addrDataIn, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			val = MREAD_D8(  smbHdl->desc.addrDataIn, 0 );
	}/*switch*/

	/* mask bit / polarity */
	val &= smbHdl->desc.maskDataIn;
	if( val ) if( smbHdl->desc.flagsDataIn & SMB_DESC_PORT_FLAG_POLARITY_HIGH )return( 1 );
			  else return( 0 );
	else	  if( smbHdl->desc.flagsDataIn & SMB_DESC_PORT_FLAG_POLARITY_HIGH )return( 0 );
			  else return( 1 );
}/*sdaIn*/

/****************************** sdaHigh *****************************
 *  Description:  Release SDA line.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl pointer to smb handle
 *  Output.....:  return 0 - OK | 1 if stuck at low
 *  Globals....:  -
 ****************************************************************************/
static int sdaHigh	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
u_int32 val;
	/* read with correct access size */
	switch( smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			val = MREAD_D32( smbHdl->desc.addrDataOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			val = MREAD_D16( smbHdl->desc.addrDataOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			val = MREAD_D8(  smbHdl->desc.addrDataOut, 0 );
	}/*switch*/

	/* not read back bits vernibbling */
	if( !(smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_READABLE_REG) )
	{
		val &= ~smbHdl->desc.notReadBackMaskDataOut;
		val |= smbHdl->desc.notReadBackDefaultsDataOut;
	}/*if*/

	val &= ~smbHdl->desc.maskDataOut;
	if( smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_POLARITY_HIGH )
		val |= smbHdl->desc.maskDataOut;

	/* write with correct access size */
	switch( smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			MWRITE_D32( smbHdl->desc.addrDataOut, 0, val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			MWRITE_D16( smbHdl->desc.addrDataOut, 0, (u_int16)val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			MWRITE_D8(  smbHdl->desc.addrDataOut, 0, (u_int8)val );
	}/*switch*/

	/* check SCL is high */
	if( sdaIn( smbHdl ) )
		return( 0 );
	else
		return( 1 );
}/*sdaHigh*/

/****************************** sdaLow *****************************
 *  Description:  Drive SDA line to low.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl pointer to smb handle
 *  Output.....:  return 0 | 1 if stuck at high
 *  Globals....:  -
 ****************************************************************************/
static int sdaLow	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
u_int32 val;
	/* read with correct access size */
	switch( smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			val = MREAD_D32( smbHdl->desc.addrDataOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			val = MREAD_D16( smbHdl->desc.addrDataOut, 0 );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			val = MREAD_D8(  smbHdl->desc.addrDataOut, 0 );
	}/*switch*/

	/* not read back bits vernibbling */
	if( !(smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_READABLE_REG) )
	{
		val &= ~smbHdl->desc.notReadBackMaskDataOut;
		val |= smbHdl->desc.notReadBackDefaultsDataOut;
	}/*if*/

	val |= smbHdl->desc.maskDataOut;
	if( smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_POLARITY_HIGH )
		val &= ~smbHdl->desc.maskDataOut;

	/* write with correct access size */
	switch( smbHdl->desc.flagsDataOut & SMB_DESC_PORT_FLAG_SIZE_MASK )
	{
		case SMB_DESC_PORT_FLAG_SIZE_32:
			MWRITE_D32( smbHdl->desc.addrDataOut, 0, val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_16:
			MWRITE_D16( smbHdl->desc.addrDataOut, 0, (u_int16)val );
			break;
		case SMB_DESC_PORT_FLAG_SIZE_8:
		default:
			MWRITE_D8(  smbHdl->desc.addrDataOut, 0, (u_int8)val );
	}/*switch*/

	/* check SDA is low */
	if( sdaIn( smbHdl ) )
		return( 1 );
	else
		return( 0 );
}/*sdaLow*/


/****************************** freeBus ***************************************
 *  Description:  Free the bus from busy devices by setting SDA high
 *                and clock 20 times.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
static void freeBus	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
int i;

	sdaHigh( smbHdl );
	for( i=0; i<20; i++ )
	{
		sclHigh( smbHdl );
		sclLow(  smbHdl );
	}/*for*/
	sclHigh( smbHdl );
}/*freeBus*/

/****************************** start ***************************************
 *  Description:  Set START condition.
 *				  _
 *		 SDA	   |_
 *				  ___
 *		 SCL	
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int start	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	/* check SCL is high */
	if( !sclIn( smbHdl ) )
		return( SMB_ERR_BUSY );

	/* check SDA is high */
	if( !sdaIn( smbHdl ) )
	{
		freeBus( smbHdl );
		if( !sdaIn( smbHdl ) )
			return( SMB_ERR_BUSY );
	}/*if*/

	if( sdaLow( smbHdl ) )
		return( SMB_ERR_COLL );
	else
		return( 0 );
}/*start*/

/****************************** stop ***************************************
 *  Description:  Set STOP condition.
 *				   	  _
 *		 SDA	  X__|
 *				     _
 *		 SCL	  X_|
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int stop	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	sdaLow( smbHdl );
	sclLow( smbHdl );

	if( sclHigh( smbHdl ) )
		return( SMB_ERR_COLL );

	if( sdaHigh( smbHdl ) )
		return( SMB_ERR_BUSY );
	else
		return( 0 );
}/*stop*/

/****************************** checkAckn ***********************************
 *  Description:  Check the 9th bit for acknowledge.
 *				 _ 	  _
 *		 SDA	  |__| 	 expected
 *				    _
 *		 SCL	  _|
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int checkAckn	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	sclLow(  smbHdl );
	sdaHigh( smbHdl );

	if( sclHigh( smbHdl ) )
		return( SMB_ERR_COLL );

	if( sdaIn( smbHdl ) )
		return( SMB_ERR_NO_DEVICE );
	else
		return( 0 );
}/*checkAckn*/

/****************************** sendByte *************************************
 *  Description:  Send a byte.
 *				   __
 *		 SDA	  X__X
 *					_
 *		 SCL	  _|
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int sendByte	/* nodoc */
(
	SMB_HANDLE  *smbHdl,
	u_int8 val
)
{
u_int8 mask;

	mask = 0x80;
	while( mask )
	{
		if( sclLow( smbHdl ) ) return( SMB_ERR_COLL );
		if( mask & val )
		{
			if( sdaHigh( smbHdl ) ) return( SMB_ERR_BUSY );
		}
		else
		{
			if( sdaLow( smbHdl ) ) return( SMB_ERR_BUSY );
		}
		if( sclHigh( smbHdl ) ) return( SMB_ERR_COLL );

		mask >>= 1;
	}/*while*/

	return( 0 );
}/*sendByte*/

/****************************** readByte *************************************
 *  Description:  Read a byte.
 *				   __
 *		 SDA	  X__X
 *					_
 *		 SCL	  _|
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int readByte	/* nodoc */
(
	SMB_HANDLE  *smbHdl,
	u_int8 		*valP
)
{
u_int8 mask;

	*valP = 0xFF;
	mask = 0x80;
	while( mask )
	{
		if( sclLow( smbHdl ) ) return( SMB_ERR_COLL );
		if( !sdaIn( smbHdl ) )
			*valP &= ~mask;
		if( sclHigh( smbHdl ) ) return( SMB_ERR_COLL );

		mask >>= 1;
	}/*while*/

	return( 0 );
}/*readByte*/



/****************************** SMB_PORT_Init *****************************
 *
 *  Description:  Initializes this library and check's the SMB host.
 *
 *---------------------------------------------------------------------------
 *  Input......:  descP 	pointer to SMB descriptor
 *                osHdl     OS specific handle
 *                smbHdlP	pointer to variable where the handle will be stored
 *
 *  Output.....:  return    0 | error code
 *				  *smbHdlP	valid handle | NULL
 *
 *  Globals....:  -
 ****************************************************************************/
#ifdef MAC_IO_MAPPED
u_int32 SMB_PORT_IO_Init
#else
u_int32 SMB_PORT_Init
#endif
(
    SMB_DESC_PORT	*descP,
    OSS_HANDLE	 	*osHdl,
	void		 	**smbHdlP
)
{
u_int32     error  = 0;
SMB_HANDLE  *smbHdl;
u_int32		gotSize;

	*smbHdlP = NULL;

	/*---------------------+
	|  check descriptor	   |
	+---------------------*/
	if(		/* check access size */
		   ( descP->busClock != 0 )
		&& ( descP->busClock != 1 )
		&& ( descP->busClock != 10 )
	  )
	{
		error = SMB_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if(		/* check access size */
		   !( descP->flagsDataIn	& SMB_DESC_PORT_FLAG_SIZE_MASK )
		|| !( descP->flagsDataOut 	& SMB_DESC_PORT_FLAG_SIZE_MASK )
		|| !( descP->flagsClockIn 	& SMB_DESC_PORT_FLAG_SIZE_MASK )
		|| !( descP->flagsClockOut	& SMB_DESC_PORT_FLAG_SIZE_MASK )
	  )
	{
		error = SMB_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if(		/* write registers are readable */
		   !( descP->flagsDataOut 	& SMB_DESC_PORT_FLAG_READABLE_REG )
		|| !( descP->flagsClockOut	& SMB_DESC_PORT_FLAG_READABLE_REG )
	  )
	{
		error = SMB_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if(		/* mask bits is set only one */
		   !(descP->maskDataIn  )
		|| !(descP->maskDataOut	)
		|| !(descP->maskClockIn	)
		|| !(descP->maskClockOut)
	  )
	{
		error = SMB_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	/*---------------------+
	|  alloc structure	   |
	+---------------------*/
	smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
	if( smbHdl == NULL )
	{
	    error = SMB_ERR_NO_MEM;
		goto CLEANUP;
	}/*if*/

	/*---------------------+
	|  init the structure  |
	+---------------------*/
	OSS_MemFill( osHdl, gotSize, (char*)smbHdl, 0 );

	smbHdl->desc 			    = *descP;
	smbHdl->ownSize  			= gotSize;
	smbHdl->osHdl    			= (OSS_HANDLE*) osHdl;

	smbHdl->entries.Ident		= smbIdent;

	smbHdl->entries.Exit		= (int32 (*)(void **))smbExit;

	smbHdl->entries.WriteByte =
		(int32 (*)(void *,u_int8,u_int8))smbWriteByte;

	smbHdl->entries.ReadByte =
		(int32 (*)(void *,u_int8,u_int8*))smbReadByte;

	smbHdl->entries.WriteTwoByte =
		(int32 (*)(void *,u_int8,u_int8,u_int8))smbWriteTwoByte;

	smbHdl->entries.WriteReadTwoByte= (int32 (*)(void *,u_int8,u_int8,u_int8*,u_int8*))NULL;

	/* clear status first */
	*smbHdlP = (void*) smbHdl;

CLEANUP:
	return( error );
}/*SMB_PORT_Init*/

/*******************************  smbExit  *********************************
 *
 *  Description:  Deinitializes this library and SMB controller.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdlP	pointer to variable where the handle is stored
 *
 *  Output.....:  return    always 0
 *				  *smbHdlP	NULL
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbExit
(
	SMB_HANDLE **smbHdlP
)
{
u_int32 error  = 0;
SMB_HANDLE  *smbHdl;

	smbHdl = *smbHdlP;
	*smbHdlP = NULL;

	/* clear status first */

    OSS_MemFree( smbHdl->osHdl, smbHdl, smbHdl->ownSize );

	return( error );
}/*smbExit*/

/******************************** smbReadByte ******************************
 *
 *  Description:  Read a byte from a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr		device address
 *				  dataP     pointer to variable where value will be stored
 *
 *  Output.....:  return    0 | error code
 *				  *dataP	read value
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbReadByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      *dataP
)
{
u_int32 error1 = 0;
u_int32 error2 = 0;
u_int32 acknErr = 0;
u_int32 stopErr = 0;

	*dataP = 0xFF;

	if( (error1 = start( smbHdl )) )
		goto CLEANUP;

	if( (error1 = sendByte( smbHdl, addr | SMB_READ ) ) )
		goto CLEANUP;

	if( !(acknErr = checkAckn( smbHdl )) )
	{
		error2  = readByte( smbHdl, dataP );
		checkAckn( smbHdl ); /* stop here - no ackn from master */
	}/*if*/

CLEANUP:
	stopErr = stop( smbHdl );

	if( error1 )
	    return( error1 );
	if( error2 )
	    return( error2 );
	if( acknErr )
	    return( acknErr );
    return( stopErr );
}/*smbReadByte*/

/******************************** smbWriteByte ******************************
 *
 *  Description:  Write a byte to a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr		device address
 *				  data      value to write
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbWriteByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      data
)
{
u_int32 error1 = 0;
u_int32 error2 = 0;
u_int32 acknErr = 0;
u_int32 stopErr = 0;

	if( (error1 = start( smbHdl )) )
		goto CLEANUP;

	if( (error1 = sendByte( smbHdl, addr | SMB_WRITE ) ) )
		goto CLEANUP;

	if( !(acknErr = checkAckn( smbHdl )) )
	{
		error2  = sendByte( smbHdl, data );
		acknErr = checkAckn( smbHdl );
	}/*if*/

CLEANUP:
	stopErr = stop( smbHdl );

	if( error1 )
	    return( error1 );
	if( error2 )
	    return( error2 );
	if( acknErr )
	    return( acknErr );
    return( stopErr );
}/*smbWriteByte*/

/******************************** smbWriteTwoByte ******************************
 *
 *  Description:  Writes two bytes to a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr		device address
 *				  cmdAddr   device command or index value
 *				  data      value to write
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbWriteTwoByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      cmdAddr,
	u_int8      data
)
{
u_int32 error1 = 0;
u_int32 error2 = 0;
u_int32 error3 = 0;
u_int32 acknErr = 0;
u_int32 stopErr = 0;

	if( (error1 = start( smbHdl )) )
		goto CLEANUP;

	if( (error1 = sendByte( smbHdl, addr | SMB_WRITE ) ) )
		goto CLEANUP;

	if( !(acknErr = checkAckn( smbHdl )) )
	{
		error2  = sendByte( smbHdl, cmdAddr );
		if( !(acknErr = checkAckn( smbHdl )) )
		{
			error3  = sendByte( smbHdl, data );
			acknErr = checkAckn( smbHdl );
		}/*if*/
	}/*if*/

CLEANUP:
	stopErr = stop( smbHdl );

	if( error1 )
	    return( error1 );
	if( error2 )
	    return( error2 );
	if( error3 )
	    return( error3 );
	if( acknErr )
	    return( acknErr );
    return( stopErr );
}/*smbWriteTwoByte*/

/*******************************  smbIdent  *********************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char *smbIdent( void )
{
    return( "SMB - SMB library: $Id: sysmanagbus_port.c,v 1.9 2004/10/14 16:09:12 dpfeuffer Exp $" );
}/*smbIdent*/

#if 0 /* BLK functions not implemented */

/****************************** smbWriteReadBlock ****************************
 *
 *  Description:  Send a command or index byte to the device first and
 *                read than a block from the device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl		valid SMB handle
 *				  addr			device address
 *				  size			nbr of bytes to read
 *				  buf       	buffer location
 *				  cmdOrIndex    byte, that will be send to the target
 *                              comand or index
 *
 *  Output.....:  return    0 | error code
 *				  *dataP	read value
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbWriteReadBlock	/* nodoc */
(
	SMB_HANDLE	*smbHdl,
	u_int8 		addr,
	u_int32		size,
	u_int8		*buf,
	u_int8 		cmdOrIndex
)
{
	return( SMB_ERR_DESCRIPTOR );
}/*smbWriteReadBlock*/

/******************************** smbWriteBlock ******************************
 *
 *  Description:  Writes a block to a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr		device address
 *				  size      buffer size
 *				  buf       buffer location
 *
 *  Output.....:  return    0 | error code
 *				  *dataP	read value
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbWriteBlock	/* nodoc */
(
	SMB_HANDLE	*smbHdl,
	u_int8 		addr,
	u_int32		size,
	u_int8		*buf
)
{
	return( SMB_ERR_DESCRIPTOR );
}/*smbWriteBlock*/

#endif /* BLK functions not implemented */




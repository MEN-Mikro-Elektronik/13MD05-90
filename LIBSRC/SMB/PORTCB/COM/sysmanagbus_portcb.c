/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysmanagbus_portcb.c
 *      Project: SMB library
 *
 *       Author: kp
 *        $Date: 2008/06/18 16:39:39 $
 *    $Revision: 1.13 $
 *
 *  Description: system managment bus driver for a port
 *				 same as sysmanagbus_port but using callbacks to write/read
 *				 pins.
 *
 *				 SMB/IIC protocol emulation.
 *				 Single master is implemented only.
 *
 *     Required: -
 *     Switches: SMB_FIXED_HANDLE - don't allocate handle dynamically
 *									instead, use handle from caller
 *				 SMB_PIC16F914_SUPPORT	- Support SMB interface of PIC16F914
 *               MEN_EP02      - to provide additional routines for the EP02
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysmanagbus_portcb.c,v $
 * Revision 1.13  2008/06/18 16:39:39  AWanka
 * R: Outputs wasn't correct
 * M: Deleted outputs
 *
 * Revision 1.12  2007/09/20 16:58:50  ufranke
 * added
 *  - switch MEN_EP02
 *
 * Revision 1.11  2007/07/25 09:31:17  JWu
 * ign
 *
 * Revision 1.10  2007/07/09 10:18:57  JWu
 * changed argument type of smbWriteAddrByte()
 *
 * Revision 1.9 20.06.2007 09:41	jwu
 * +: routines for devices with 16-bit address
 *
 * Revision 1.8  2006/10/25 15:57:45  rla
 * Added Support for 16F914 PIC SMB
 *
 * Revision 1.7  2005/06/23 16:11:56  kp
 * Copyright line changed (sbo)
 *
 * Revision 1.6  2005/02/15 12:53:35  rlange
 * adapted for max. 100 kHz busclock
 *
 * Revision 1.5  2003/06/05 15:28:54  UFranke
 * changed
 *  - removed DIAB compiler warnings
 *
 * Revision 1.4  2001/07/09 16:43:17  kp
 * added smbWriteReadTwoByte (rla)
 *
 * Revision 1.3  2001/02/08 13:54:13  kp
 * added SetBucClock (SMS24/B11)
 *
 * Revision 1.2  2001/01/23 11:38:34  kp
 * bug fix: Bad parameters passed to sdaIn()
 *
 * Revision 1.1  2000/12/01 13:34:07  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>

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
	SMB_DESC_PORTCB desc;
}SMB_HANDLE;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define SMB_READ           0x01
#define SMB_WRITE          0x00

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
static u_int32 smbWriteReadTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 *data1P, u_int8 *data2P );
static char* smbIdent( void );

#ifdef MEN_EP02
static u_int32 smbReadAddrByte( SMB_HANDLE *smbHdl, u_int8 addr, u_int8 byteAddrH, u_int8 byteAddrL, u_int8 *dataP);
static u_int32 smbWriteAddrByte( SMB_HANDLE *smbHdl, u_int8 smbaddr, u_int16 addr, u_int8 data );
static u_int32 smbWritePageByte( SMB_HANDLE *smbHdl, u_int8 smbaddr, u_int16 addr, unsigned int n, u_int8 *dataP );
#endif

/************************************* delay ********************************
 *  Description:  Delay	descriptor dependend time.
 *
 *		   Note:  busClock descriptor value
 *					 0 - max speed - no delay
 *					 1 -  1kHz  OSS_Delay()
 *					10 - 10kHz  OSS_MikroDelay()
 *					100 - 100kHz  OSS_MikroDelay()
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
static void delay	/* nodoc */
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
		case 100: /* 100kHz max */
			OSS_MikroDelay( smbHdl->osHdl, 10 );
			break;
		default: /* max speed */
			break;
	}/*switch*/
}/*delay*/

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

	smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 1 );
	delay(smbHdl);
	for( i=0; i<20; i++ )
	{
		smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 );
		delay(smbHdl);
		smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
		delay(smbHdl);
	}/*for*/
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 );
	delay(smbHdl);
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
	delay(smbHdl);
	if( !smbHdl->desc.sclIn( smbHdl->desc.cbArg ) )
		return( SMB_ERR_BUSY );

	/* check SDA is high */
	if( !smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
	{
		freeBus( smbHdl );
		if( !smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
			return( SMB_ERR_BUSY );
	}/*if*/

	delay(smbHdl);
	if( smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 0 ) )
		return( SMB_ERR_COLL );
	else
		return( 0 );
}/*start*/

#ifdef SMB_PIC16F914_SUPPORT
/****************************** restart ***************************************
 *  Description:  Set RESTART condition.
 *				  _
 *		 SDA	   |_
 *				  ___
 *		 SCL
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int restart	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
	delay(smbHdl);
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 );

	/* check SCL is high */
	delay(smbHdl);
	if( !smbHdl->desc.sclIn( smbHdl->desc.cbArg ) )
		return( SMB_ERR_BUSY );

	/* check SDA is high */
	if( !smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
	{
		freeBus( smbHdl );
		if( !smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
			return( SMB_ERR_BUSY );
	}/*if*/

	delay(smbHdl);
	if( smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 0 ) )
		return( SMB_ERR_COLL );
	else
		return( 0 );
}/*start*/
#endif

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
	smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 0 );
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
	delay(smbHdl);

	if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
		return( SMB_ERR_COLL );

	delay(smbHdl);
	if( smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 1 ) )
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
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
	smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 1 );
	delay(smbHdl);

	if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
		return( SMB_ERR_COLL );

	delay(smbHdl);
	if( smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
		return( SMB_ERR_NO_DEVICE );
	else
		return( 0 );
}/*checkAckn*/

#ifdef SMB_PIC16F914_SUPPORT
/****************************** checkAckn2 ***********************************
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
static int checkAckn2	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
	smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 1 );
	delay(smbHdl);

	if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
		return( SMB_ERR_COLL );

	delay(smbHdl);
	if( smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
		return( SMB_ERR_NO_DEVICE );
	else{
		smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
		smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 );
		return( 0 );
	}
}/*checkAckn*/
#endif

/****************************** setAckn ***********************************
 *  Description:  Set the 9th bit for acknowledge.
 *				 _ 	  _
 *		 SDA	  |__| 	 set
 *				    _
 *		 SCL	  _|
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int setAckn	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
	smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 0 );
	delay(smbHdl);

	if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
		return( SMB_ERR_COLL );

	delay(smbHdl);
	return( 0 );
}/*setAckn*/

#ifdef SMB_PIC16F914_SUPPORT
/****************************** notAckn ***********************************
 *  Description:  Set the 9th bit for acknowledge.
 *				 ______
 *		 SDA	      	 set
 *				    _
 *		 SCL	  _|
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  return	0 | error code
 *  Globals....:  -
 ****************************************************************************/
static int notAckn	/* nodoc */
(
	SMB_HANDLE  *smbHdl
)
{
	smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 );
	smbHdl->desc.sdaSet( smbHdl->desc.cbArg, 1 );
	delay(smbHdl);

	if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
		return( SMB_ERR_COLL );

	delay(smbHdl);
	return( 0 );
}/*setAckn*/
#endif

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
		if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 ) )
			return( SMB_ERR_COLL );
		delay(smbHdl);

		if( smbHdl->desc.sdaSet( smbHdl->desc.cbArg, !!(mask & val) ) )
			return( SMB_ERR_BUSY );
		delay(smbHdl);

		if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
			return( SMB_ERR_COLL );
		delay(smbHdl);

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
		if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 ))
			return( SMB_ERR_COLL );

		delay(smbHdl);
		if( !smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
			*valP &= ~mask;

		delay(smbHdl);
		if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
			return( SMB_ERR_COLL );

		delay(smbHdl);
		mask >>= 1;
	}/*while*/

	return( 0 );
}/*readByte*/

#ifdef SMB_PIC16F914_SUPPORT
/****************************** readByte2 *************************************
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
static int readByte2	/* nodoc */
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
		if( mask != 0x80 ){
			if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 0 ))
				return( SMB_ERR_COLL );
		}

		delay(smbHdl);
		if( !smbHdl->desc.sdaIn( smbHdl->desc.cbArg ) )
			*valP &= ~mask;

		delay(smbHdl);
		if( smbHdl->desc.sclSet( smbHdl->desc.cbArg, 1 ) )
			return( SMB_ERR_COLL );

		delay(smbHdl);
		mask >>= 1;
	}/*while*/

	return( 0 );
}/*readByte*/
#endif

/****************************** SMB_PORTCB_Init *****************************
 *
 *  Description:  Initializes this library
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
u_int32 SMB_PORTCB_Init
(
    SMB_DESC_PORTCB	*descP,
    OSS_HANDLE	 	*osHdl,
	void		 	**smbHdlP
)
{
	u_int32     error  = 0;
	SMB_HANDLE  *smbHdl;
	u_int32		gotSize=0;

#ifndef	SMB_FIXED_HANDLE
	*smbHdlP = NULL;
#endif
	/*---------------------+
	|  check descriptor	   |
	+---------------------*/
	if(
		   ( descP->busClock != 0 )
		&& ( descP->busClock != 1 )
		&& ( descP->busClock != 10 )
		&& ( descP->busClock != 100 )
	  )
	{
		error = SMB_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	/*---------------------+
	|  alloc structure	   |
	+---------------------*/
#ifndef	SMB_FIXED_HANDLE
	smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
	if( smbHdl == NULL )
	{
	    error = SMB_ERR_NO_MEM;
		goto CLEANUP;
	}/*if*/
#else
	smbHdl = *smbHdlP;
#endif
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
		
	smbHdl->entries.WriteReadTwoByte = (int32 (*)(void *,u_int8,u_int8,
		u_int8*,u_int8*))smbWriteReadTwoByte;

#ifdef MEN_EP02
	smbHdl->entries.ReadAddrByte = (int32 (*)(void *, u_int8, u_int8, u_int8, u_int8*))smbReadAddrByte;

	smbHdl->entries.WriteAddrByte =(int32 (*)(void *, u_int8, u_int16, u_int8))smbWriteAddrByte;
		
	smbHdl->entries.WritePageByte =(int32 (*)(void*, u_int8, u_int16, unsigned int, u_int8*))smbWritePageByte;
#endif /*MEN_EP02*/

	*smbHdlP = (void*) smbHdl;

CLEANUP:
	return( error );
}/*SMB_PORT_Init*/

#ifdef	SMB_FIXED_HANDLE
u_int32 SMB_PORTCB_HandleSize(void)
{
	return sizeof( SMB_HANDLE );
}
#endif

u_int32 SMB_PORTCB_SetBusClock( SMB_HANDLE *smbHdl, u_int32 busClock )
{
	u_int32 oldClock = smbHdl->desc.busClock;
	smbHdl->desc.busClock = busClock;
	return oldClock;
}

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

	error1 = start( smbHdl );
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, addr | SMB_READ );
	if( error1 )
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

#ifdef MEN_EP02
/******************************** smbReadAddrByte ******************************
 *
 *  Description:  Read a byte from a device from random 16-bit address.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr			device address
 *					byteAddrH	MSB of byte address to be read
 *					byteAddrL	LSB of byte address to be read
 *				  dataP     pointer to variable where value will be stored
 *
 *  Output.....:  return    0 | error code
 *				  *dataP	read value
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbReadAddrByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8		byteAddrH,
	u_int8		byteAddrL,
	u_int8      *dataP
)
{
	u_int32 error1 = 0;
	u_int32 error2 = 0;
	u_int32 error3 = 0;
	u_int32 error4 = 0;
	u_int32 error5 = 0;
	u_int32 acknErr1 = 0;
	u_int32 acknErr2 = 0;
	u_int32 acknErr3 = 0;
	u_int32 acknErr4 = 0;
	u_int32 stopErr = 0;

	*dataP = 0xFF;

	error1 = start( smbHdl );
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, addr | SMB_WRITE );
	if( error1 )
		goto CLEANUP;

	if( !(acknErr1 = checkAckn( smbHdl )))
	{
		error2 = sendByte( smbHdl, byteAddrH );
		if( !(acknErr2 = checkAckn( smbHdl )))
		{
			error2 = sendByte( smbHdl, byteAddrL );
			if( !(acknErr3 = checkAckn( smbHdl )))
			{
				error3 = restart( smbHdl );
				if( error3 )
					goto CLEANUP;
				else
				{
					error4 = sendByte( smbHdl, addr | SMB_READ );
					/*printf(" *** acknErr3, error4 %ld,%ld\n ***", acknErr3, error4); */
					if( error4 )
						goto CLEANUP;
					
					if(!(acknErr4 = checkAckn( smbHdl )))
					{
						error5 = readByte( smbHdl, dataP );
						checkAckn( smbHdl ); /* stop here - no ackn from master */
					}/*if*/
				}/*else*/
			}/*if*/
		}/*if*/

	}/*if*/			
CLEANUP:
	stopErr = stop( smbHdl );

	if( error1 )
	{
		printf(" start error.\n");
		return error1;
	  }
	if( error2 )
	{
		printf(" read address error.\n");
	  return error2;
	}
	if( error3 )
	{
		printf("restart error.\n");
		return error3;
	}
	if( error4 )
	{
		printf("read byte error.\n");
		return error4;
	}

	if( error5)
		return error5;
		
	if( acknErr1 )
	    return acknErr1;
	
	if( acknErr2 )
		return acknErr2;
		
	if( acknErr3 )
		return acknErr3;
		
	if( acknErr4 )
		return acknErr4;

    if( stopErr )
    	printf("*** stopErr\n");
    /*return( stopErr ); */
    return stopErr;
}/*smbReadAddrByte*/
#endif

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

	error1 = start( smbHdl );
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, addr | SMB_WRITE );
	if( error1 )
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

	error1 = start( smbHdl );
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, addr | SMB_WRITE );
	if( error1 )
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

#ifdef MEN_EP02
/******************************** smbWriteAddrByte ******************************
 *
 *  Description:  Write a byte to a device of 16-bit address
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  smbaddr	device address
 *				  addr		address 	
 *				  data      value to write
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
 static u_int32 smbWriteAddrByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      smbAddr,
	u_int16		addr,	/* word address */
	u_int8      data
)
{
	u_int32 error1 = 0;
	u_int32 error2 = 0;
	u_int32 error3 = 0;
	u_int32 error4 = 0;
	u_int32 acknErr = 0;
	u_int32 stopErr = 0;

	u_int8 cmdAddrH;
	u_int8 cmdAddrL;
	
	cmdAddrH = (u_int8)((addr & 0xff00) >> 8);
	cmdAddrL = (u_int8)((addr & 0x00ff));
	
	error1 = start( smbHdl );
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, smbAddr | SMB_WRITE );
	if( error1 )
		goto CLEANUP;

	if( !(acknErr = checkAckn( smbHdl )) )
	{	
		/*printf("*** acknErr %ld%\n", acknErr);*/
		error2  = sendByte( smbHdl, cmdAddrH );
		if( !(acknErr = checkAckn( smbHdl )) )
		{	
			/*printf("*** acknErr %ld%\n", acknErr);*/
			error3  = sendByte( smbHdl, cmdAddrL );
			if( !(acknErr = checkAckn( smbHdl )))
			{
				/*printf("*** acknErr %ld%\n", acknErr);*/
				error4 = sendByte( smbHdl, data );
				acknErr = checkAckn( smbHdl );
			}/*if*/
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
	if( error4 )
		return( error4 );
		
	if( acknErr )
	{
		/*printf(" ack Error.\n");*/
	    return( acknErr );
	}
    return( stopErr );
}/*smbWriteAddrByte*/

/******************************** smbWritePageByte ******************************
 *
 *  Description:  Page Write mode, write multiple bytes
 *								to a device of 16-bit address
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr			device address
 *				  cmdAddrH		MSB of address b15-b8
 *				  cmdAddrL		LSB of address b7-b0
 *				  n				number of bytes
 *				  dataP      	pointer to value to write
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
 static u_int32 smbWritePageByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      smbaddr,
	u_int16		addr,
	unsigned int n,
	u_int8      *dataP
)
{
	u_int32 error1 = 0;
	u_int32 error2 = 0;
	u_int32 error3 = 0;
	u_int32 error4 = 0;
	u_int32 acknErr = 0;
	u_int32 stopErr = 0;

	u_int8 cmdAddrH;
	u_int8 cmdAddrL;
	
	cmdAddrH = (u_int8)((addr & 0xff00) >> 8);
	cmdAddrL = (u_int8)((addr & 0x00ff));

	error1 = start( smbHdl );
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, addr | SMB_WRITE );
	if( error1 )
		goto CLEANUP;

	if( !(acknErr = checkAckn( smbHdl )) )
	{
		error2  = sendByte( smbHdl, cmdAddrH );
		if( !(acknErr = checkAckn( smbHdl )) )
		{
			error3  = sendByte( smbHdl, cmdAddrL );
			if( !(acknErr = checkAckn( smbHdl )))
			{
				while(!n)
				{
					error4 != sendByte( smbHdl, *dataP );
					dataP++;
					n--;
					acknErr = checkAckn( smbHdl );	
				}
			}/*if*/
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
	if( error4 )
		return( error4 );
	if( acknErr )
	    return( acknErr );
    return( stopErr );
}/*smbWritePageByte*/
#endif

/******************************** smbWriteReadTwoByte ******************************
 *
 *  Description:  Writes one Byte to and reads two bytes from a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr		device address
 *				  cmdAddr   device command or index value
 *				  data1P    pointer of data1
 *				  data2P	pointer of data2
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbWriteReadTwoByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8		cmdaddr,
	u_int8      *data1P,
	u_int8		*data2P
)
{
	u_int32 error1 = 0;
	u_int32 error2 = 0;
	u_int32 error3 = 0;
	u_int32 acknErr = 0;
	u_int32 stopErr = 0;

	*data1P = 0xFF;
	*data2P = 0xFF;

	/* write 1 byte : means send addr-byte send reg-byte */
	error1 = start( smbHdl );
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, addr | SMB_WRITE );
	if( error1 )
		goto CLEANUP;

	if( !(acknErr = checkAckn( smbHdl )) )
	{
		error2  = sendByte( smbHdl, cmdaddr );
		acknErr = checkAckn( smbHdl );
	}

	if( error2 )
	    return( error2 );

	if( acknErr )
	    return( acknErr );

#ifndef SMB_PIC16F914_SUPPORT
	error1 = start( smbHdl );
#else
	/* read 2 bytes : means send addr byte receive 2 data bytes */
	error1 = restart( smbHdl );
#endif
	if( error1 )
		goto CLEANUP;

	error1 = sendByte( smbHdl, addr | SMB_READ );
	if( error1 )
		goto CLEANUP;

#ifndef SMB_PIC16F914_SUPPORT
	if( !(acknErr = checkAckn( smbHdl )) )
	{
		error2  = readByte( smbHdl, data1P );

#else
	if( !(acknErr = checkAckn2( smbHdl )) )
	{
		u_int32 timeout = 0;

		do{
			delay( smbHdl );
			switch( smbHdl->desc.busClock ){
				case 1: 	timeout += 1000;	break;
				case 10:	timeout += 100;		break;
				case 100:	timeout += 10;		break;
				default:	timeout += 1000;	break;
			}
		} while( !smbHdl->desc.sclIn( smbHdl->desc.cbArg ) && timeout < 1000 );

		error2  = readByte2( smbHdl, data1P );
#endif

#ifndef SMB_PIC16F914_SUPPORT
		if( !(setAckn( smbHdl )) )
		{
			error3 = readByte( smbHdl, data2P);
			checkAckn(smbHdl); /* stop here - no ackn from master */
		}
#else
		notAckn( smbHdl );
#endif
	}

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
}/*smbWriteReadTwoByte*/


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
    return( "SMB - SMB library: $Id: sysmanagbus_portcb.c,v 1.13 2008/06/18 16:39:39 AWanka Exp $" );
}/*smbIdent*/







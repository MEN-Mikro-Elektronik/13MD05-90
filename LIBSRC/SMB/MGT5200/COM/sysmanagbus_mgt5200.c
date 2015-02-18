/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysmanagbus.c
 *      Project: MENMON MGT5200
 *
 *       Author: kp
 *        $Date: 2006/01/27 17:34:49 $
 *    $Revision: 1.5 $
 *
 *  Description: system managment bus driver for the MGT5200 I2C controller
 *				 and MPC85XX I2C controller which is (almost) register
 *				 compatible
 *
 * This code exports two init functions:
 * - SMB_MGT5200_Init - for 5200
 * - SMB_MPC85XX_Init - for 85xx
 *
 *     Required: -
 *     Switches: SMB_FIXED_HANDLE - don't allocate handle dynamically
 *									instead, use handle from caller	
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysmanagbus_mgt5200.c,v $
 * Revision 1.5  2006/01/27 17:34:49  UFRANKE
 * added
 *  + IIC glitch filter register setup
 *    if set in MPC5200 SMB descriptor
 *
 * Revision 1.4  2005/06/24 08:19:38  kp
 * Copyright line changed (sbo)
 *
 * Revision 1.3  2005/04/12 16:56:21  kp
 * cosmetic
 *
 * Revision 1.2  2004/11/04 17:30:27  kp
 * - support MPC85XX
 * - use 32 bit accesses to I2C controller regs
 * - added important dummy read after starting transfer for MPC85xx
 *
 * Revision 1.1  2003/07/31 14:21:35  UFranke
 * Initial Revision
 *
 * Revision 1.6  2003/04/29 17:27:45  UFranke
 * changed
 *   - clear ARBLOST after writeRead2Byte
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/maccess.h>
#include <MEN/mgt5200.h>

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
	u_int32    ownSize;
	OSS_HANDLE *osHdl;
	u_int32	   dbgLevel;
	DBG_HANDLE *dbgHdl;
	MACCESS    i2cBase;		/* base of I2C controller regs */
	u_int32	   timeOut;		
	u_int32	   mikroDelay;	
}SMB_HANDLE;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/

#define I2C_REG_WR( _base, _reg, _val ) \
 do { MWRITE_D32( _base, _reg, ((_val) & 0xff) << 24); } while(0)
#define I2C_REG_RD( _base, _reg ) \
 ((MREAD_D32( _base, _reg ) >> 24) & 0xff)

#define I2C_REG_CLRMASK( _base, _reg, _val ) \
 I2C_REG_WR( _base, _reg, I2C_REG_RD( _base, _reg ) & ~(_val) )

#define I2C_REG_SETMASK( _base, _reg, _val ) \
 I2C_REG_WR( _base, _reg, I2C_REG_RD( _base, _reg ) | (_val) )

#define SMB_READ           0x01
#define SMB_WRITE          0x00

#define	DBG_MYLEVEL		smbHdl->dbgLevel
#define DBH				smbHdl->dbgHdl

#define MPC85XX_I2C_DFSRR 0x14	/* digital filter reg */

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

static char* smbIdent( void );
static u_int32 smbExit(         SMB_HANDLE  **smbHdlP );
static u_int32 smbWriteByte(    SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 data );
static u_int32 smbReadByte(     SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 *dataP );
static u_int32 smbWriteTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 data );
static int32 smbWriteReadTwoByte( void	    *hdl, u_int8 addr, u_int8 cmdAddr, u_int8 *data1P, u_int8 *data2P );


static void delay( SMB_HANDLE  *smbHdl )
{
	if( smbHdl->mikroDelay )
		OSS_MikroDelay( smbHdl->osHdl, 1 );
	else
		OSS_Delay( smbHdl->osHdl, 1 );
	
}/*delay*/

static void freeBus( SMB_HANDLE  *smbHdl )
{
    int tries = 64;

    DBGWRT_ERR((DBH,"*** Freebus\n"));
    do
    {
		/* stop */
		I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, 0x00 );
		OSS_MikroDelay( smbHdl->osHdl, 100 );

		/* ctrl start + transmit + acknoledge */
		I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL,
		   MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_START | MGT5200_I2C_CTRL_TX );
		OSS_MikroDelay( smbHdl->osHdl, 100 );
    } while(tries--);

    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_STAT, 0x00 );
}/*freeBus*/



#define I2C_ERROR	0
#define I2C_OK		(!I2C_ERROR)

extern void *MAIN_MenmonGlobalsP;

#define I2C_MDR_RX		0x01

#define u_int8 unsigned char
#define uint8 u_int8

#if 0
#define MBAR_I2C 0x3D40

typedef struct
{
    volatile u_int8 MADR;
    u_int8	res1[3];
    volatile u_int8 MFDR;
    u_int8	res2[3];
    volatile u_int8 MCR;
    u_int8	res3[3];
    volatile u_int8 STAT;
    u_int8	res4[3];
    volatile u_int8 MDR;
    u_int8	res5[3];
}i2c_regs;
#endif


/* i2cWaitWhileBusy halts the program flow until the I2C bus is not
   busy. If a timeout occurs while waiting for bus to become idle
   I2C_ERROR will be returned. If the bus is idle I2C_OK will be returned.
*/
static int i2cWaitWhileBusy
(
	SMB_HANDLE  *smbHdl
)
{
	int n;

	/* wait while busy */
	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000 : 1 );
	while (n-- && (I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) & MGT5200_I2C_STAT_BUSY))
		delay( smbHdl );

	if (!n)
	{
		return I2C_ERROR;			/* timed out */
	}
	return I2C_OK;
}/*i2cWaitWhileBusy*/

/*
 *	Waits until rising and falling edge of CF bit detected.
 *
 */
static int waitTranfserProgressComplete
(
	SMB_HANDLE  *smbHdl
)
{
	int n;

	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000 : 1 );
	/* Wait till transfer in progress */
	while (n-- && (I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) & MGT5200_I2C_STAT_COMPL))
		delay( smbHdl );
	if( n == 0 ){
		DBGWRT_ERR((DBH,"*** i2c: transfer did not begin\n"));
	}

	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000 : 1 );
	/* Wait till transfer complete */
	while (n-- && !(I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) & MGT5200_I2C_STAT_COMPL))
		delay( smbHdl );

	if( n == 0 ){
		DBGWRT_ERR((DBH,"*** i2c: transfer did not finish\n"));
	}

	return( n );
}/*waitTranfserProgressComplete*/

/* Transmit one byte on the I2C bus.
*/
static int i2cTx
(
	SMB_HANDLE  *smbHdl,
	uint8 Data
)
{

	I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_DATA, Data );

	waitTranfserProgressComplete( smbHdl );

	/* Ack received? */
	if (!(I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) & MGT5200_I2C_STAT_RXAK))
	{
		return I2C_OK;
	}
	DBGWRT_ERR((DBH,"*** i2cTx: no ack, stat=0x%02x\n",I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) ));
	return I2C_ERROR;
}/*i2cTx*/

/* Receive one byte from the I2C bus.
*/
static int i2cRx
(
	SMB_HANDLE  *smbHdl,
	uint8 * Data,
	int Stop,
	int Nack
)
{
	int n;

	n = waitTranfserProgressComplete( smbHdl );

	if (!n)
	{
		return I2C_ERROR;
	}

	if (Stop)
	{
		I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );	/* Generate stop signal */
	}

	if (Nack)
	{
		I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TXAK );	/* Disable ack */
	}
	/*
	 * Do a dummy read to control reg to ensure that
	 * write to control reg completed. A "eieio" would have
	 * the same effect.
	 * If this is not done, I2C does not operate correctly
	 * on MPC85XX
	 */
	n = I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_CTRL );

	*Data = I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_DATA );			/* Read data */

	return I2C_OK;
}/*i2cRx*/

/* Write Len bytes pointed to by *Data to the device with address
   Addr.
*/
static int i2cWrite
(
	SMB_HANDLE  *smbHdl,
	int Addr,
	uint8 * Data,
	int Len
)
{
	int error = SMB_ERR_NO;

	/* Check if arbitration lost */
	if (I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) & MGT5200_I2C_STAT_ARBLOST)
	{
		DBGWRT_ERR((DBH,"*** i2cWrite: arbitration lost\n"));
		I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_STAT, MGT5200_I2C_STAT_ARBLOST);	/* Clear the condition */
		freeBus( smbHdl );
	}

	/* wait until bus free */
	if (!i2cWaitWhileBusy (smbHdl))
	{
		DBGWRT_ERR((DBH,"*** i2cWrite: timeout waiting while busy (Addr=0x%02X, Data=0x%02X, Len=%d)\n", Addr, (int)*Data, Len));
		freeBus( smbHdl );
		return SMB_ERR_BUSY;
	}

	/* TX Enable */
	I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TX );
	/* START */
	I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );

	/* Output Address (R/W = 0) */
	if (!i2cTx(smbHdl,Addr))
	{
		DBGWRT_ERR((DBH,"*** i2cWrite: failed writing address (Addr=0x%02X, Data=0x%02X, Len=%d)\n", Addr, (int)*Data, Len));
		error = SMB_ERR_NO_DEVICE;			/* No ack - fail */
		goto CLEANUP;
	}

	while (Len--)
	{
		if (!i2cTx (smbHdl,*Data++))
		{
			DBGWRT_ERR((DBH,"*** i2cWrite: failed writing data (Addr=0x%02X, Data=0x%02X, Len=%d)\n", Addr, (int)*Data, Len));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/
	}/*while*/

CLEANUP:
	/* STOP */
	I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );

	return error;
}/*i2cWrite*/



/* Read Len bytes to the location pointed to by *Data from the device
   with address Addr.
*/
static int i2cRead
(
	SMB_HANDLE  *smbHdl,
	int Addr,
	uint8 * Data,
	int Len
)
{
	int error = SMB_ERR_NO;
	volatile u_int8 dummy;

	/* Check if arbitration lost */
	if (I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) & MGT5200_I2C_STAT_ARBLOST)
	{
		DBGWRT_ERR((DBH,"*** i2cRead: arbitration lost\n"));
		I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_STAT, MGT5200_I2C_STAT_ARBLOST);	/* Clear the condition */
		freeBus( smbHdl );
	}

	/* wait until bus free */
	if (!i2cWaitWhileBusy(smbHdl))
	{
		DBGWRT_ERR((DBH,"*** i2cRead: timeout waiting while busy (Addr=0x%02X, Len=%d)\n", Addr, Len));
		freeBus( smbHdl );
		return SMB_ERR_BUSY;			/* Timed out */
	}

	/* Enable the I2C for Tx, Ack Enabled */
	I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_TX );
	/* START */
	I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );

	/* Output Address (R/W = 1) */
	if (!i2cTx(smbHdl, Addr | 0x01))
	{
		DBGWRT_ERR((DBH,"*** i2cRead: failed writing address (Addr=0x%02X, Len=%d)\n", Addr, Len));
		I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );	/* Generate stop signal */
		error = SMB_ERR_NO_DEVICE;			/* Timed out - no ack */
		goto CLEANUP;
	}

	I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TX );	/* Set receive mode */
	/* Only 1 byte receive? */
	if( Len == 1 )
	{
		I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TXAK );	/* then Disable ack */
	}

	if( Len )
	{
		dummy = I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_DATA );		/* Dummy read to start transfer */
	}

	while (Len)
	{
		if (!i2cRx (smbHdl, Data++, Len == 1, Len == 2))
		{
			DBGWRT_ERR((DBH,"*** i2cRead: failed reading data (Addr=0x%02X, Len=%d)\n", Addr, Len));
			error = SMB_ERR_COLL;
			goto CLEANUP;
		}/*if*/
		Len--;					/* Decrement count */
	}/*while*/

CLEANUP:
	/* STOP */
	I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );

	return error;
}/*i2cRead*/

/* Write LenOut bytes pointed to by *DataOut, send no STOP and then
   read LenIn bytes to the location pointed to by *DataIn from the
   device with address Addr.
*/
static int i2cWriteRead
(
	SMB_HANDLE  *smbHdl,
	int Addr,
	uint8 * DataOut,
	int LenOut,
	uint8 * DataIn,
	int LenIn
)
{
	int error = SMB_ERR_NO;

	/* Check if arbitration lost */
	if (I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_STAT ) & MGT5200_I2C_STAT_ARBLOST)
	{	
		DBGWRT_ERR((DBH,"*** i2cWriteRead: arbitration lost\n"));
		I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_STAT, MGT5200_I2C_STAT_ARBLOST);	/* Clear the condition */
		freeBus( smbHdl );
	}

	/* wait until bus free */
	if (!i2cWaitWhileBusy(smbHdl))
	{
		freeBus( smbHdl );
		return SMB_ERR_BUSY;
	}

	/* TX ENABLE */
	I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TX );
	/* START */
	I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );

	/* Output Address (R/W = 0) */
	if (!i2cTx(smbHdl, Addr))
	{
		DBGWRT_ERR((DBH,"*** i2cWriteRead: failed writing address Addr=0x%02X\n",
					Addr));
		error = SMB_ERR_NO_DEVICE;			/* No ack - fail */
		goto CLEANUP;
	}

	while (LenOut--)
	{
		if (!i2cTx (smbHdl, *DataOut++))
		{
			DBGWRT_ERR((DBH,"*** i2cWriteRead: failed writing data Addr=0x%02X\n",
					Addr));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/
	}/*while*/

	I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_RSTART );	/* Generate restart signal */

	/* Output Address (R/W = 1) */
	if (!i2cTx(smbHdl, Addr | 0x01))
	{
		DBGWRT_ERR((DBH,"*** i2cWriteRead: failed restart Addr=0x%02X\n",
					Addr));
		I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );	/* Generate stop signal */
		error = SMB_ERR_NO_DEVICE;			/* Timed out - no ack */
		goto CLEANUP;
	}

	I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TX );	/* Set receive mode */
	/* Only 1 byte receive? */
	if (LenIn == 1)
	{
		I2C_REG_SETMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TXAK );	/* then Disable ack */
	}

	if (LenIn)
	{
		*DataIn = I2C_REG_RD( smbHdl->i2cBase, MGT5200_I2C_DATA );	/* Dummy read to start transfer */
	}

	while (LenIn)
	{
		if (!i2cRx(smbHdl, DataIn++, LenIn == 1, LenIn == 2))
		{
			DBGWRT_ERR((DBH,"*** i2cWriteRead: failed read data Addr=0x%02X\n",
					Addr));
			error = SMB_ERR_COLL;
			goto CLEANUP;
		}/*if*/
		LenIn--;				/* Decrement count */
	}/*while*/

CLEANUP:
	/*STOP*/
	I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START );
	delay( smbHdl );
	delay( smbHdl );
	delay( smbHdl );
	I2C_REG_CLRMASK( smbHdl->i2cBase, MGT5200_I2C_STAT, MGT5200_I2C_STAT_ARBLOST);	/* Clear the condition */

	return error;
}/*i2cWriteRead*/


/*******************************  smbIdent  *********************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char *smbIdent( void )	/* nodoc */
{
    return( "SMB - SMB library: $Id: sysmanagbus_mgt5200.c,v 1.5 2006/01/27 17:34:49 UFRANKE Exp $" );
}/*smbIdent*/



#ifdef	SMB_FIXED_HANDLE
u_int32 SMB_MGT5200_HandleSize(void)
{
	return sizeof( SMB_HANDLE );
}
u_int32 SMB_MPC85XX_HandleSize(void)
{
	return sizeof( SMB_HANDLE );
}
#endif

static void EntriesInit( SMB_HANDLE *smbHdl )
{
    smbHdl->entries.Exit        = (int32 (*)(void**))smbExit;
    smbHdl->entries.WriteByte	= (int32 (*)(void*,u_int8,u_int8))
	smbWriteByte;
    smbHdl->entries.ReadByte= (int32 (*)(void*,u_int8,u_int8*))
	smbReadByte;
    smbHdl->entries.WriteTwoByte= (int32 (*)(void*,u_int8,u_int8,u_int8))
	smbWriteTwoByte;

    smbHdl->entries.Ident		= smbIdent;
    smbHdl->entries.WriteReadTwoByte = smbWriteReadTwoByte;

}

static void CtrlCommonInit( SMB_HANDLE *smbHdl )
{
    /*
     * we're never slave, so clear slave address reg
     */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_ADDR, 0x00 );	


	/* this sequence should clear the bus after i.e. watchdog reset
	 * borrowed by Motorola i2cexample.c
	 */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN );	
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_TX );	
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL,
    	MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_TX | MGT5200_I2C_CTRL_START );	/* START */
	i2cTx( smbHdl, 0 );					/* Output Address (R/W = 0) */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_TX ); /*STOP*/

    /* disable I2C module */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, 0x00 );	

    /* enable I2C module */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN );	

    /* clear status and IRQ */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_STAT, 0x00 );
}

/****************************** SMB_MGT5200_Init *****************************
 *
 *  Description:  Initializes the MGT5200 I2C controller lib
 *
 *  Note: I2C pins must have been assigned by caller
 *
 *---------------------------------------------------------------------------
 *  Input......:  desc		descriptor
 *                osHdl     OS specific handle
 *                smbHdlP	pointer to variable where the handle will be stored
 *
 *  Output.....:  return    0 | error code
 *				  *smbHdlP	valid handle | NULL
 *
 *  Globals....:  -
 ****************************************************************************/
u_int32 SMB_MGT5200_Init
(
    SMB_DESC_MGT5200	*desc,
    OSS_HANDLE			*osHdl,
	void				**smbHdlP
)
{
    u_int32     error  = 0;
    SMB_HANDLE  *smbHdl;
    u_int32		gotSize=0;

#ifndef	SMB_FIXED_HANDLE
    *smbHdlP = NULL;
    smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
    if( smbHdl == NULL )
    {
		error = SMB_ERR_NO_MEM;
		goto CLEANUP;
    }/*if*/
	*smbHdlP = smbHdl;

#else
    smbHdl = *smbHdlP;
#endif

    /* init the structure */
    OSS_MemFill( osHdl, gotSize, (char*)smbHdl, 0 );

    DBGINIT((NULL,&smbHdl->dbgHdl));
    smbHdl->dbgLevel		= desc->dbgLevel;

    smbHdl->i2cBase 		= desc->i2cBase;
    smbHdl->timeOut		  	= desc->timeOut;
    smbHdl->mikroDelay		= desc->mikroDelay;
    smbHdl->ownSize  		= gotSize;
    smbHdl->osHdl    		= (OSS_HANDLE*) osHdl;

	EntriesInit( smbHdl );		/* initialize method pointers */

    /* disable I2C module */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, 0x00 );	

    /* set divider register */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_DIV, desc->divReg );


    /* set digital filter register - only if desc->filterReg != 0
       - register not implemented in MPC5200 < B1 */
    if( desc->filterReg )
    	MWRITE_D32( smbHdl->i2cBase, MGT5200_I2C_MIFR, desc->filterReg );

	/* perform common controller init */
	CtrlCommonInit( smbHdl );
#ifndef	SMB_FIXED_HANDLE
  CLEANUP:
#endif
    return( error );
}/*SMB_MGT5200_Init*/


/****************************** SMB_MPC85XX_Init *****************************
 *
 *  Description:  Initializes the MPC85XX I2C controller lib
 *
 *  Note: I2C pins must have been assigned by caller
 *
 *---------------------------------------------------------------------------
 *  Input......:  desc		descriptor
 *                osHdl     OS specific handle
 *                smbHdlP	pointer to variable where the handle will be stored
 *
 *  Output.....:  return    0 | error code
 *				  *smbHdlP	valid handle | NULL
 *
 *  Globals....:  -
 ****************************************************************************/
u_int32 SMB_MPC85XX_Init
(
    SMB_DESC_MPC85XX	*desc,
    OSS_HANDLE			*osHdl,
	void				**smbHdlP
)
{
    u_int32     error  = 0;
    SMB_HANDLE  *smbHdl;
    u_int32		gotSize=0;

#ifndef	SMB_FIXED_HANDLE
    *smbHdlP = NULL;
    smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
    if( smbHdl == NULL )
    {
		error = SMB_ERR_NO_MEM;
		goto CLEANUP;
    }/*if*/
	*smbHdlP = smbHdl;

#else
    smbHdl = *smbHdlP;
#endif

    /* init the structure */
    OSS_MemFill( osHdl, gotSize, (char*)smbHdl, 0 );

    DBGINIT((NULL,&smbHdl->dbgHdl));
    smbHdl->dbgLevel		= desc->dbgLevel;

    smbHdl->i2cBase 		= desc->i2cBase;
    smbHdl->timeOut		  	= desc->timeOut;
    smbHdl->mikroDelay		= desc->mikroDelay;
    smbHdl->ownSize  		= gotSize;
    smbHdl->osHdl    		= (OSS_HANDLE*) osHdl;

	EntriesInit( smbHdl );		/* initialize method pointers */

    /* disable I2C module */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_CTRL, 0x00 );	

    /* set divider register */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_DIV, desc->divReg );

    /* set digital filter register */
    I2C_REG_WR( smbHdl->i2cBase, MPC85XX_I2C_DFSRR, desc->filterReg );

	/* perform common controller init */
	CtrlCommonInit( smbHdl );
#ifndef	SMB_FIXED_HANDLE
  CLEANUP:
#endif
    return( error );
}/*SMB_MPC85XX_Init*/


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

    /* disable I2C module */
    I2C_REG_WR( smbHdl->i2cBase, MGT5200_I2C_ADDR, 0x00 );	

    OSS_MemFree( smbHdl->osHdl, smbHdl, smbHdl->ownSize );

    return( error );
}/*smbExit*/

/******************************** smbReadByte ******************************
 *
 *  Description:  Read a byte from a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl  valid SMB handle
 *			  addr		device address
 *			  dataP     pointer to variable where value will be stored
 *
 *  Output.....:  return  0 | error code
 *			  *dataP	read value
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
    u_int32 error = 0;

    DBGWRT_1((DBH,"smbReadByte addr=%x\n", addr));

    /* set to default */
    *dataP = 0xFF;

    /* sanity check */
    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	error = i2cRead( smbHdl, addr, dataP, 1 );

    return error;
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
	u_int32 error = 0;

	DBGWRT_1((DBH,"smbWriteByte addr=%x data=%x\n", addr, data));

    /* sanity check */
    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	error = i2cWrite( smbHdl, addr, &data, 1 );

    return error;
}/*smbWriteByte*/


/******************************** smbWriteTwoByte ****************************
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
    u_int32 error = 0;
    u_int8  buf[2];

    DBGWRT_1((DBH,"smbWriteTwo\n"));
    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	buf[0] = cmdAddr;
	buf[1] = data;
	
	error = i2cWrite( smbHdl, addr, buf, 2 );

    return error;
}/*smbWriteTwoByte*/


/***************************** smbWriteReadTwoByte **************************
 *
 *  Description:  Writes a command or index byte to
 *                and read two bytes from a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr		device address
 *				  cmdAddr   device command or index value to write
 *				  data1P    pointer for value1
 *				  data2P    pointer for value2
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbWriteReadTwoByte
(
	void	    *hdl,
	u_int8      addr,
	u_int8      cmdAddr,
	u_int8      *data1P,
	u_int8      *data2P
)
{
    int32 error = 0;
    u_int8  buf[2];
	SMB_HANDLE  *smbHdl = hdl;
	
	buf[0] = 0xFF;
	buf[1] = 0xFF;
	
	error = i2cWriteRead( smbHdl, addr, &cmdAddr, 1, buf, 2 );

	*data1P = buf[0];
	*data2P = buf[1];

	return error;
}/*smbWriteReadTwoByte*/


/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: smb2_mgt5200.c
 *      Project: MENMON MGT5200
 *
 *       Author: cs
 *        $Date: 2010/06/14 13:29:36 $
 *    $Revision: 1.10 $
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
 * $Log: smb2_mgt5200.c,v $
 * Revision 1.10  2010/06/14 13:29:36  dpfeuffer
 * R: Windows PREfast warnings
 * M: debug prints modified
 *
 * Revision 1.9  2009/03/17 13:59:35  dpfeuffer
 * R: compiler warnings wit VC2008 64bit compiler
 * M: debug prints with pointers changed to %p
 *
 * Revision 1.8  2008/09/15 16:45:01  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.7  2007/09/12 12:19:32  ufranke
 * fixed
 * - set smbHdl->smbComHdl.busyWait at init
 * - mgt5200_waitTranfserProgressComplete() unsave detection of transfer start edge
 * replaced by detection of IRQ flag due to completed transfer
 * - timeout counter compare in mgt5200_i2cWaitWhileBusy() and mgt5200_waitTranfserProgressComplete()
 * added
 * + mgt5200_smbUseOssDelay()
 *
 * Revision 1.6  2007/02/27 20:37:52  rt
 * fixed:
 * - wrong Register at Master Enable
 *
 * Revision 1.5  2007/02/20 15:10:34  DPfeuffer
 * - undo: changed interface of *_Init() and *_Exit functions
 *         (VxW6.3 must consider this by disabling strict aliasing)
 *
 * Revision 1.4  2006/09/11 11:26:41  cs
 * changed:
 *   - interface of *_Init() and *_Exit functions (avoid warnings for VxW6.3)
 * fixed:
 *   - don't free handle when compiled with SMB_FIXED_HANDLE
 *
 * Revision 1.3  2006/04/27 14:01:49  DPfeuffer
 * mgt5200_i2cXfer(): return value fixed for error case
 *
 * Revision 1.2  2006/02/09 15:54:01  DPfeuffer
 * some casts added to compile with VC
 *
 * Revision 1.1  2006/02/07 19:13:41  cschuster
 * Initial Revision
 *
 *
 * cloned from SMB/MGT5200/sysmanagbus_mgt5200.c Rev. 1.5
 *---------------------------------------------------------------------------
 * (c) Copyright 2006..2007 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

/* #define DBG */

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/maccess.h>
#include <MEN/mgt5200.h>
#include <MEN/mdis_err.h>

#define SMB_COMPILE
#include <MEN/smb2.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
typedef struct
{
	SMB_ENTRIES entries; 		/**< function entries */


	SMB_COM_HANDLE smbComHdl; 	/**< Common handle entries */

	/* data */
	u_int32    ownSize;			/**< size of memory allocated for this handle */
	MACCESS    baseAddr;			/**< base of I2C controller regs */
	u_int32	   timeOut;			/**< wait time if SMBus is busy */
	u_int32	   mikroDelay;		/**< default 0-OSS_Delay, 1-OSS_MikroDelay */
}SMB_HANDLE;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define	DBG_MYLEVEL		smbHdl->smbComHdl.dbgLevel
#define DBH				smbHdl->smbComHdl.dbgHdl

#define OSSH			smbHdl->smbComHdl.osHdl

#define MPC85XX_I2C_DFSRR 0x14	/* digital filter reg */

#define I2C_MDR_RX		0x01

int _dbgRd( SMB_HANDLE *smbHdl, MACCESS base, u_int32 reg, u_int32 val, int line )
{
	DBGWRT_1((DBH,"I2C_REG_RD @%08p+0x%08x %08x line %d\n", (void*)base, reg, val, line ));
	return( val );
}

void _dbgWr( SMB_HANDLE *smbHdl, MACCESS base, u_int32 reg, u_int32 val, int line )
{
	DBGWRT_1((DBH,"I2C_REG_WR >> @%08p+0x%08x %08x line %d\n", (void*)base, reg, val, line ));
}

#ifdef _DIAB_TOOL
	#define _MEN_DO_
	#define _MEN_WHILE(_x_)
#else
	#define _MEN_DO_    do
	#define _MEN_WHILE(_x_)	while(_x_)
#endif


#define I2C_REG_WR( smbHdl, _base, _reg, _val, _line ) \
 _MEN_DO_ { MWRITE_D32( _base, _reg, (u_int32)((_val) & 0xff) << 24); _dbgWr( smbHdl, _base, _reg, _val, _line ); } _MEN_WHILE(0)
#define I2C_REG_RD( smbHdl ,_base, _reg, _line ) \
 (_dbgRd( smbHdl, _base, _reg, ((MREAD_D32( _base, _reg ) >> 24) & 0xff), _line ) )

#define I2C_REG_CLRMASK( smbHdl, _base, _reg, _val, _line ) \
 I2C_REG_WR( smbHdl, _base, _reg, I2C_REG_RD( smbHdl, _base, _reg, _line ) & ~(_val), _line )

#define I2C_REG_SETMASK( smbHdl, _base, _reg, _val, _line ) \
 I2C_REG_WR( smbHdl, _base, _reg, I2C_REG_RD( smbHdl, _base, _reg, _line ) | (_val), _line )


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

static char* mgt5200_smbIdent( void );
static u_int32 mgt5200_smbExit( SMB_HANDLE  **smbHdlP );
static int32 mgt5200_i2cXfer( SMB_HANDLE *smbHdl,
							  struct I2CMESSAGE msg[],
							  u_int32 num );

static void delay( SMB_HANDLE  *smbHdl )
{
	if( smbHdl->mikroDelay )
		OSS_MikroDelay( OSSH, 1 );
	else
		OSS_Delay( OSSH, 1 );

}/*delay*/

static void mgt5200_freeBus( SMB_HANDLE  *smbHdl )
{
    int tries = 64;

    DBGWRT_ERR((DBH,"*** mgt5200_freebus\n"));
    do
    {
		/* stop */
		I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, 0x00, __LINE__ );
		OSS_MikroDelay( OSSH, 100 );

		/* ctrl start + transmit + acknoledge */
		I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL,
		   MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_START | MGT5200_I2C_CTRL_TX, __LINE__ );
		OSS_MikroDelay( OSSH, 100 );
    } while(tries--);

    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, 0x00, __LINE__ );
}/* mgt5200_freeBus */



extern void *MAIN_MenmonGlobalsP;

/* mgt5200_i2cWaitWhileBusy halts the program flow until the I2C bus is not
   busy. If a timeout occurs while waiting for bus to become idle
   I2C_ERROR will be returned. If the bus is idle will be returned.
*/
static int mgt5200_i2cWaitWhileBusy
(
	SMB_HANDLE  *smbHdl
)
{
	int error = SMB_ERR_NO;
	int n;

	/* wait while busy */
	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000 : 1 );
	while( n-- &&
		   ( I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, __LINE__ ) &
		     MGT5200_I2C_STAT_BUSY ) )
		delay( smbHdl );

	if ( n <= 0 )
	{
		DBGWRT_ERR((DBH,"*** %s: BUSY missing\n", __FUNCTION__));
		error = SMB_ERR_BUSY;			/* timed out */
		goto CLEANUP;
	}

CLEANUP:
	return( error );
}/* mgt5200_i2cWaitWhileBusy */

/*
 *	Waits until rising and falling edge of CF bit detected.
 *
 */
static int mgt5200_waitTranfserProgressComplete
(
	SMB_HANDLE  *smbHdl,
	u_int8      *statP
)
{
	int error = SMB_ERR_NO;
	int n;
	u_int8 stat = 0x00;

	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000 : 1 );
	if( n== 0 )
		n++;

	/* Wait till INTERRUPT */
	while( n-- )
	{
		delay( smbHdl );
		stat = (u_int8)(I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, __LINE__ ));
		if( stat & MGT5200_I2C_STAT_IRQ )
			break;
	}/*while*/

	if( n <= 0 )
	{
		DBGWRT_ERR((DBH,"*** %s: warning interrupt edge missing\n", __FUNCTION__));
	}

	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000 : 1 );
	if( n== 0 )
		n++;
		
	/* Wait till transfer complete */
	while( n-- )
	{
		if( stat & MGT5200_I2C_STAT_COMPL )
			break;
		delay( smbHdl );
		stat = (u_int8)(I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, __LINE__ ));
	}

	if( n <= 0 )
	{
		DBGWRT_ERR((DBH,"*** %s: transfer did not finish\n", __FUNCTION__ ));
		error = SMB_ERR_GENERAL;
		goto CLEANUP;
	}

CLEANUP:
	*statP = stat;
	return( error );
}/* mgt5200_waitTranfserProgressComplete */

/* Transmit one byte on the I2C bus.
*/
static int mgt5200_i2cTx
(
	SMB_HANDLE  *smbHdl,
	u_int8 Data
)
{
	int error;
	u_int8 stat = 0xff;

	I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, 0x00, __LINE__ ); /* clear status register */
	I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_DATA, Data, __LINE__ );

	error = mgt5200_waitTranfserProgressComplete( smbHdl, &stat );
	if( error )
	{
		DBGWRT_ERR((DBH,"*** %s: mgt5200_waitTranfserProgressComplete stat %02x\n", __FUNCTION__, stat));
		return( error );
	}

	/* Ack received? */
	if( !( stat & MGT5200_I2C_STAT_RXAK ) )
	{
		return SMB_ERR_NO;
	}
	DBGWRT_ERR((DBH,"*** mgt5200_i2cTx: no ack, stat=0x%02x\n",
					I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, __LINE__ ) ));

	return SMB_ERR_NO_DEVICE;
}/* mgt5200_i2cTx */

/* Receive one byte from the I2C bus.
*/
static int mgt5200_i2cRx
(
	SMB_HANDLE  *smbHdl,
	u_int8 * Data,
	int Stop,
	int Nack
)
{
	int n;
	u_int8 stat;

	if( mgt5200_waitTranfserProgressComplete( smbHdl, &stat ) )
	{
		return SMB_ERR_GENERAL;
	}

	if( Stop )
	{
		I2C_REG_CLRMASK( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_START, __LINE__ );	/* Generate stop signal */
	}

	if( Nack )
	{
		I2C_REG_SETMASK( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_TXAK, __LINE__ );	/* Disable ack */
	}
	/*
	 * Do a dummy read to control reg to ensure that
	 * write to control reg completed. A "eieio" would have
	 * the same effect.
	 * If this is not done, I2C does not operate correctly
	 * on MPC85XX
	 */
	n = I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, __LINE__ );

	*Data = (u_int8)I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_DATA, __LINE__ );			/* Read data */

	return SMB_ERR_NO;
}/* mgt5200_i2cRx */

/* Write Len bytes pointed to by *Data to the device with address
   Addr.
*/
static int mgt5200_i2cWrite
(
	SMB_HANDLE  *smbHdl,
	int Addr,
	u_int8 * Data,
	int Len,
	int Restart
)
{
	int error = SMB_ERR_NO;
	u_int32 ctrl_add = Restart ? MGT5200_I2C_CTRL_RSTART : 0x00;

	DBGWRT_1((DBH,"%s addr %02x len %d restart %d\n", __FUNCTION__, Addr, Len, Restart ));

	if( !Restart )
		/* start with Master Enable */
		I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN, __LINE__ );

	/* TX Enable - START - RESTART? */
	I2C_REG_SETMASK( smbHdl, smbHdl->baseAddr,
					 MGT5200_I2C_CTRL,
					 MGT5200_I2C_CTRL_TX | MGT5200_I2C_CTRL_START | ctrl_add, __LINE__);

	/* Output Address (R/W = 0) */
	if (mgt5200_i2cTx(smbHdl,(u_int8)Addr))
	{
		DBGWRT_ERR((DBH,"*** i2cWrite: line %d failed writing address (Addr=0x%02X, Data=0x%02X, Len=%d)\n", __LINE__, Addr, (int)*Data, Len));
		error = SMB_ERR_NO_DEVICE;			/* No ack - fail */
		goto CLEANUP;
	}

	while (Len--)
	{
		if (mgt5200_i2cTx (smbHdl,*Data++))
		{
			DBGWRT_ERR((DBH,"*** i2cWrite: line %d failed writing data (Addr=0x%02X, Data=0x%02X, Len=%d)\n", __LINE__, Addr, (int)*Data, Len));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/
	}/*while*/

CLEANUP:
	return error;
}/* mgt5200_i2cWrite */



/* Read Len bytes to the location pointed to by *Data from the device
   with address Addr.
*/
static int mgt5200_i2cRead
(
	SMB_HANDLE  *smbHdl,
	int Addr,
	u_int8 * Data,
	int Len,
	int Restart
)
{
	int error = SMB_ERR_NO;
	volatile u_int32 dummy;
	u_int32 ctrl_add = Restart ? MGT5200_I2C_CTRL_RSTART : 0x00;

	DBGWRT_1((DBH,"%s addr %02x len %d restart %d\n", __FUNCTION__, Addr, Len, Restart ));

	if( !Restart )
		/* start with Master Enable */
		I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN, __LINE__ );

	/* TX Enable - START - RESTART? */
	I2C_REG_SETMASK( smbHdl, smbHdl->baseAddr,
					 MGT5200_I2C_CTRL,
					 MGT5200_I2C_CTRL_TX | MGT5200_I2C_CTRL_START | ctrl_add, __LINE__);

	/* Output Address (R/W = 1) */
	if (mgt5200_i2cTx(smbHdl, (u_int8)(Addr | 0x01) ))
	{
		DBGWRT_ERR((DBH,"*** i2cRead: failed writing address "
						"(Addr=0x%02X, Len=%d)\n", Addr, Len));

		error = SMB_ERR_NO_DEVICE;			/* Timed out - no ack */
		goto CLEANUP;
	}

	/* Set receive mode */
	I2C_REG_CLRMASK( smbHdl, smbHdl->baseAddr,
					 MGT5200_I2C_CTRL,
					 MGT5200_I2C_CTRL_TX, __LINE__ );

	/* Only 1 byte receive? */
	if( Len == 1 )
	{
		/* then Disable ack */
		I2C_REG_SETMASK( smbHdl, smbHdl->baseAddr,
						 MGT5200_I2C_CTRL,
						 MGT5200_I2C_CTRL_TXAK, __LINE__ );
	}

	if( Len )
	{
		I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, 0x00, __LINE__ ); /* clear status register */
		dummy = I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_DATA, __LINE__ );		/* Dummy read to start transfer */
	}

	while (Len)
	{
		if( mgt5200_i2cRx (smbHdl, Data++, Len == 1, Len == 2) )
		{
			DBGWRT_ERR((DBH,"*** i2cRead: failed Reading data (Addr=0x%02X, Len=%d)\n", Addr, Len));
			error = SMB_ERR_COLL;
			goto CLEANUP;
		}/*if*/
		I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, 0x00, __LINE__ ); /* clear status register */
		Len--;					/* Decrement count */
	}/*while*/

CLEANUP:
	return error;
}/* mgt5200_i2cRead */

/*******************************  mgt5200_smbIdent  *************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char *mgt5200_smbIdent( void )	/* nodoc */
{
    return( "SMB - SMB library: $Id: smb2_mgt5200.c,v 1.10 2010/06/14 13:29:36 dpfeuffer Exp $" );
}/* mgt5200_smbIdent */



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

static void CtrlCommonInit( SMB_HANDLE *smbHdl )
{
    /*
     * we're never slave, so clear slave address reg
     */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_ADDR, 0x00, __LINE__ );


	/* this sequence should clear the bus after i.e. watchdog reset
	 * borrowed by Motorola i2cexample.c
	 */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN, __LINE__ );
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_TX, __LINE__ );
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL,
    	MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_TX | MGT5200_I2C_CTRL_START, __LINE__ );	/* START */
	mgt5200_i2cTx( smbHdl, 0 );					/* Output Address (R/W = 0) */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN | MGT5200_I2C_CTRL_TX, __LINE__ ); /*STOP*/

    /* disable I2C module */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, 0x00, __LINE__ );

    /* enable I2C module */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, MGT5200_I2C_CTRL_EN, __LINE__ );

    /* clear status and IRQ */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, 0x00, __LINE__ );
}

/******************************** mgt5200_smbUseOssDelay *****************************/
/** Setup used OSS_Delay or OSS_MikroDelay.
 *
 *  \param   smbHdl     	valid SMB handle
 *	\param   useOssDelay    0 | 1
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 mgt5200_smbUseOssDelay( SMB_HANDLE *smbHdl, int useOssDelay )
{
	smbHdl->mikroDelay = !useOssDelay;
    return( SMB_ERR_NO );
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
    u_int32     error  = SMB_ERR_NO;
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

    DBGINIT((NULL,&smbHdl->smbComHdl.dbgHdl));
    smbHdl->smbComHdl.dbgLevel		= desc->dbgLevel;

    smbHdl->baseAddr 		= desc->baseAddr;
    smbHdl->timeOut		  	= desc->timeOut;
    smbHdl->mikroDelay		= desc->mikroDelay;
    smbHdl->ownSize  		= gotSize;
    smbHdl->smbComHdl.osHdl 	= (OSS_HANDLE*) osHdl;
	smbHdl->smbComHdl.busyWait 	= smbHdl->timeOut;

	smbHdl->entries.Capability = SMB_FUNC_I2C |
								 SMB_FUNC_SMBUS_EMUL ;




	DBGWRT_1((DBH,"%s baseAddr %08p smbHdl %08p\n", __FUNCTION__, smbHdl->baseAddr, smbHdl ));
	if( (error = SMB_COM_Init(smbHdl)) )
	{
		goto CLEANUP;
	}

	smbHdl->entries.Exit		= (int32 (*)(void**))mgt5200_smbExit;
	smbHdl->entries.SmbXfer		= NULL;

	smbHdl->entries.I2CXfer		= (int32 (*)( void *smbHdl,
											  struct I2CMESSAGE msg[],
											  u_int32 num))mgt5200_i2cXfer;
	smbHdl->entries.Ident		= (char* (*)(void))mgt5200_smbIdent;
	smbHdl->entries.UseOssDelay	= (int32 (*)(void *,int))mgt5200_smbUseOssDelay;

    /* disable I2C module */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, 0x00, __LINE__ );

    /* set divider register */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_DIV, desc->divReg, __LINE__ );

    /* set digital filter register - only if desc->filterReg != 0
       - register not implemented in MPC5200 < B1 */
    if( desc->filterReg )
    	MWRITE_D32( smbHdl->baseAddr, MGT5200_I2C_MIFR, desc->filterReg );

	/* perform common controller init */
	CtrlCommonInit( smbHdl );
#ifndef	SMB_FIXED_HANDLE
  CLEANUP:
#endif

	DBGWRT_1((DBH,"SMB_MGT5200_Init exit (error=0x%08x)\n", error));

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
    u_int32     error  = SMB_ERR_NO;
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

    DBGINIT((NULL,&smbHdl->smbComHdl.dbgHdl));
    smbHdl->smbComHdl.dbgLevel		= desc->dbgLevel;

    smbHdl->baseAddr 		= desc->baseAddr;
    smbHdl->timeOut		  	= desc->timeOut;
    smbHdl->mikroDelay		= desc->mikroDelay;
    smbHdl->ownSize  		= gotSize;
    smbHdl->smbComHdl.osHdl = (OSS_HANDLE*) osHdl;
	smbHdl->smbComHdl.busyWait 	= smbHdl->timeOut;

	smbHdl->entries.Capability = SMB_FUNC_I2C |
								 SMB_FUNC_SMBUS_EMUL ;


	if( (error = SMB_COM_Init(smbHdl)) )
	{
		goto CLEANUP;
	}

	smbHdl->entries.Exit		= (int32 (*)(void**))mgt5200_smbExit;
	smbHdl->entries.SmbXfer		= NULL;

	smbHdl->entries.I2CXfer		= (int32 (*)( void *smbHdl,
											  struct I2CMESSAGE msg[],
											  u_int32 num))mgt5200_i2cXfer;
	smbHdl->entries.Ident		= (char* (*)(void))mgt5200_smbIdent;

    /* disable I2C module */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_CTRL, 0x00, __LINE__ );

    /* set divider register */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_DIV, desc->divReg, __LINE__ );

    /* set digital filter register */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MPC85XX_I2C_DFSRR, desc->filterReg, __LINE__ );

	/* perform common controller init */
	CtrlCommonInit( smbHdl );
#ifndef	SMB_FIXED_HANDLE
  CLEANUP:
#endif

	DBGWRT_1((DBH,"SMB_MPC85XX_Init exit (error=0x%08x)\n", error));

    return( error );
}/*SMB_MPC85XX_Init*/


/*******************************  mgt5200_smbExit ***************************
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
static u_int32 mgt5200_smbExit
(
	SMB_HANDLE **smbHdlP
)
{
    u_int32 error  = 0;
    SMB_HANDLE  *smbHdl;

    smbHdl = *smbHdlP;

    /* disable I2C module */
    I2C_REG_WR( smbHdl, smbHdl->baseAddr, MGT5200_I2C_ADDR, 0x00, __LINE__ );

	/* deinitialize common interface */
	if(smbHdl->smbComHdl.ExitCom)
		smbHdl->smbComHdl.ExitCom(smbHdl);

#ifndef	SMB_FIXED_HANDLE
    OSS_MemFree( OSSH, smbHdl, smbHdl->ownSize );
    *smbHdlP = NULL;
#endif

    return( error );
}/* mgt5200_smbExit */

/******************************** mgt5200_i2cXfer *****************************/
/** Read/Write data from a device using the I2C protocol
 *
 *  \param   smbHdl     valid SMB handle
 *	\param   msg[]      array of i2c messages to handle
 *	\param   num        number of messages in msg []
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 mgt5200_i2cXfer
(
	SMB_HANDLE *smbHdl,
	struct I2CMESSAGE msg[],
	u_int32 num
)
{
	struct I2CMESSAGE *pmsg;
	u_int32 i;
	int32 error = SMB_ERR_NO;

	DBGWRT_1((DBH,">>> %s: smbHdl %08p\n", __FUNCTION__, smbHdl ));

	/* Check if arbitration lost */
	if( I2C_REG_RD( smbHdl, smbHdl->baseAddr, MGT5200_I2C_STAT, __LINE__ ) &
		MGT5200_I2C_STAT_ARBLOST )
	{
		DBGWRT_ERR((DBH, "*** i2cWrite: arbitration lost\n"));
		/* Clear the condition */
		I2C_REG_CLRMASK( smbHdl, smbHdl->baseAddr,
						 MGT5200_I2C_STAT,
						 MGT5200_I2C_STAT_ARBLOST, __LINE__);
		mgt5200_freeBus( smbHdl );
	}

	/* wait until bus free */
	if( mgt5200_i2cWaitWhileBusy( smbHdl ) )
	{
		DBGWRT_ERR((DBH,"*** %s: timeout waiting while busy\n", __FUNCTION__ ));
		mgt5200_freeBus( smbHdl );
		error = SMB_ERR_BUSY;
		goto CLEANUP;
	}

	for( i = 0; i < num; i++ ) {
		pmsg = &msg[i];
		DBGWRT_3((DBH,"Doing %s %d bytes to 0x%02x - %d of %d messages\n",
				 pmsg->flags & I2C_M_RD ? "read" : "write",
				 pmsg->len, pmsg->addr, i + 1, num));

		if( pmsg->flags & I2C_M_RD )
		{		
			error = mgt5200_i2cRead(smbHdl, pmsg->addr, pmsg->buf, pmsg->len, i);
			if( error )
			{
				DBGWRT_ERR((DBH,"*** %s: mgt5200_i2cRead\n", __FUNCTION__ ));
				break;
			}
		}
		else
		{
			error = mgt5200_i2cWrite(smbHdl, pmsg->addr, pmsg->buf, pmsg->len, i);
			if( error )
			{
				DBGWRT_ERR((DBH,"*** %s: mgt5200_i2cWrite\n", __FUNCTION__ ));
				break;
			}
		}
	}

	/* STOP */
	I2C_REG_CLRMASK( smbHdl, smbHdl->baseAddr,
					 MGT5200_I2C_CTRL,
					 MGT5200_I2C_CTRL_START, __LINE__ );

CLEANUP:
	DBGWRT_1((DBH,"<<< %s: smbHdl %08p error %08x\n", __FUNCTION__, smbHdl, error ));
	return error;
} /* mgt5200_i2cXfer */






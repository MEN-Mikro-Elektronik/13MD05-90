/*********************  P r o g r a m  -  M o d u l e **********************/
/*!
 *        \name  smb_menz001.c
 *     \project  SMB2 library
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2012/01/30 11:50:04 $
 *    $Revision: 1.19 $
 *
 *       \brief  system managment bus driver for the MEN 16Z001_SMB FPGA
 *				 SMB controller
 *
 *     Required: -
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: smb2_menz001.c,v $
 * Revision 1.19  2012/01/30 11:50:04  dpfeuffer
 * R: Windows compiler warning
 * M: grabHostCtrl(): extra argument passed to debug print removed
 *
 * Revision 1.18  2010/06/14 13:29:34  dpfeuffer
 * R: Windows PREfast warnings
 * M: debug prints modified
 *
 * Revision 1.17  2010/03/08 15:03:07  sy
 * R: 1. Terminate bit could be set if status is polled before an abort operation finishes
 * M: 1. add max. 500 ms wait time after starting an abort operation
 *
 * Revision 1.16  2009/03/17 14:45:08  dpfeuffer
 * R: doxygen warning
 * M: cosmetics
 *
 * Revision 1.15  2009/03/17 13:59:33  dpfeuffer
 * R: compiler warnings wit VC2008 64bit compiler
 * M: debug prints with pointers changed to %p
 *
 * Revision 1.14  2008/09/15 16:23:47  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.13  2008/06/30 18:11:31  CSchuster
 * R: 1. when controller was busy through any other SW released it anyway
 *    2. freeBus did not affect I2C devices when this where stuck
 *    3. freeBus always did maximum number of tries
 * M: 1. only release controller when this SW actually allocated it
 *    2. when normal free (TOUT) doesn't work, try to free bus using clk toggle
 *    3. abort freeBus once successful
 *
 * Revision 1.12  2007/11/15 18:15:55  cs
 * added:
 *   + optional support for multimaster (has to be configured/enabled on init)
 * fixed:
 *   - read word data: return data nonswapped
 *     reason: written and read values did not match (bytes swapped)
 * changed:
 *   - initClrChkStatus() less aggressive!
 *     first do a waitBusyReady before resetting the bus
 *
 * Revision 1.11  2007/08/08 12:57:01  rlange
 * Removed Return Error code SMB_ERR_BUSY if Busy Flag is missed in
 * waitBusyReady(). Command might be already finished.
 * Cosmetic for DBG Output in waitBusyReady()
 *
 * Revision 1.10  2007/04/05 20:02:13  cs
 * fixed:
 *    - transfer complete block at SMB_ACC_BLOCK_DATA (last Byte was missing)
 * changed:
 *    - level of some DBG messages
 *
 * Revision 1.9  2007/02/20 15:10:31  DPfeuffer
 * - undo: changed interface of *_Init() and *_Exit functions
 *         (VxW6.3 must consider this by disabling strict aliasing)
 *
 * Revision 1.8  2007/01/05 18:01:02  cs
 * added:
 *   + support for I2C messages
 *   + consider STS_INUSE bit
 * cosmetics
 *
 * Revision 1.7  2006/11/21 13:59:15  cs
 * fixed:
 *   - using OSS_Delay(): don't wait for busy bit (already passed)
 *   - error handling in waitBusyReady()
 *
 * Revision 1.6  2006/09/11 11:24:51  cs
 * changed interface of *_Init() and *_Exit functions (avoid warnings for VxW6.3)
 * fixed SMB_ACC_BLOCK_DATA (writeCmdForHost())
 *
 * Revision 1.5  2006/03/07 09:36:54  cs
 * fixed: don't enable interrupt of Z001_SMB (lib does not support interrupts)
 *
 * Revision 1.4  2006/02/07 19:13:14  cschuster
 * added SMB_FUNC_SMBUS_QUICK to capabilities
 *
 * Revision 1.3  2005/12/12 10:52:50  cschuster
 * moved assignment of smbHdl->ownSize after zeroing of smbHdl
 *
 * Revision 1.2  2005/11/25 11:32:18  cschuster
 * moved assignment of smbHdl->osHdl before SMB_COM_INIT
 * added error check for SMB_COM_INIT
 * when using MikroDelay, reduce granularity (SMBus not that fast)
 *
 * Revision 1.1  2005/11/22 08:45:16  cschuster
 * Initial Revision
 *
 *
 * based on sysmanagbus_ali1543.c, Revision 1.9  (2000/08/08)
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

/* #define DBG */
#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/maccess.h>
#include <MEN/16z001_smb.h>
#include <MEN/mdis_err.h>

#define SMB_COMPILE
#include <MEN/smb2.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/** Local structure for SMB_HANDLE */
typedef struct
{
	SMB_ENTRIES entries; 		/**< function entries */


	SMB_COM_HANDLE smbComHdl; 	/**< Common handle entries */

	/* data */
	u_int32    ownSize;			/**< size of memory allocated for this handle */
	MACCESS    baseAddr;		/**< base of I2C controller regs */
	u_int32	   timeOut;			/**< wait time if SMBus is busy */
	u_int32	   mikroDelay;		/**< default 0-OSS_Delay, 1-OSS_MikroDelay */
	u_int32    multimaster;		/**< controller in multimaster mode */
}SMB_HANDLE;

/* Other settings */
#define MAX_TIMEOUT 		 300	/* times 1/100 sec for sleep*/

#define	DBG_MYLEVEL		smbHdl->smbComHdl.dbgLevel
#define	DBH				smbHdl->smbComHdl.dbgHdl

#define OSSH			smbHdl->smbComHdl.osHdl
/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+-----------------------------------------*/
DBGCMD(	static const char errorStartStr[] =	"*** ERROR - ";	)
DBGCMD(	static const char errorLineStr[]  =	" (line	";		)
DBGCMD(	static const char errorEndStr[]	  =	")***\n";		)
/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/

static char* Z001_smbIdent( void );
static u_int32 Z001_smbExit( SMB_HANDLE  **smbHdlP );
static int32 Z001_SmbXfer( SMB_HANDLE *smbHdl, u_int32 flags, u_int16 addr,
							u_int8 read_write, u_int8 cmdAddr,
							u_int8 size, u_int8 *data );
static int32 Z001_I2cXfer( SMB_HANDLE *smbHdl,
							struct I2CMESSAGE msg[],
							u_int32 num );

/*****************************  Z001_smbIdent  *****************************/
/** Return ident string
 *
 * \return  pointer to ident string
 *
 ****************************************************************************/
static char *Z001_smbIdent( void )
{
    return( "SMB - MEN Z001_SMB library: $Id: smb2_menz001.c,v 1.19 2012/01/30 11:50:04 dpfeuffer Exp $\n");
}/*Z001_smbIdent*/



/* helper functions */
static void delay( SMB_HANDLE  *smbHdl )
{
	if( smbHdl->mikroDelay )
		OSS_MikroDelay( OSSH, 5 );
	else
		OSS_Delay( OSSH, 1 );

}/*delay*/

static void writeStatus( MACCESS baseAddr, u_int8 data )
{
	MWRITE_D8(baseAddr, Z001_SMB_STS, data );
}/*writeStatus*/

static u_int8 readStatus( MACCESS baseAddr )
{
    return( MREAD_D8( baseAddr, Z001_SMB_STS ) );
}/*readStatus*/

/* index: DATA_A=0, DATA_B=1 */
static u_int8 readData( MACCESS baseAddr, int index )
{
    return( MREAD_D8( baseAddr, (Z001_SMB_DATA_A + index) ) );
}/*readData*/

/* index: DATA_A=0, DATA_B=1 */
static void writeData( MACCESS baseAddr, int index, u_int8 data )
{
    MWRITE_D8( baseAddr, (Z001_SMB_DATA_A + index), data );
}/*writeData*/

static u_int8 readBlock( MACCESS baseAddr )
{
    return( MREAD_D8( baseAddr, Z001_SMB_BLK ) );
}/*readBlock*/

static void writeBlock( MACCESS baseAddr, u_int8 data )
{
    MWRITE_D8( baseAddr, Z001_SMB_BLK, data );
}/*writeBlock*/

static void writeCmd( MACCESS baseAddr, u_int8 data )
{
    MWRITE_D8(baseAddr, Z001_SMB_CMD, data);
}/*writeCmd*/

static u_int8 readCmd( MACCESS baseAddr )
{
    return( MREAD_D8( baseAddr, Z001_SMB_CMD ) );
}/*readCmd*/

static void writeCmdForHost( MACCESS baseAddr, u_int8 data )
{
    MWRITE_D8(baseAddr, Z001_SMB_HSTCOM, data);
}/*writeCmdForHost*/

static void writeStart( MACCESS baseAddr )
{
    MWRITE_D8(baseAddr, Z001_SMB_START_PRT, 0xff);
}/*writeStart*/

static void writeAddr( MACCESS baseAddr, u_int8 data )
{
    MWRITE_D8(baseAddr, Z001_SMB_ADDR, data);
}/*writeAddr*/

/*******************************  waitBusyReady  ***************************/
/** Waits to BUSY first and then leaving BUSY and bus idle.
 *  Returns imediatly if DEV_ERR detected.
 *
 *---------------------------------------------------------------------------
 *  \param   smbHdl     valid SMB handle
 *  \param   waitBusy   wait for busy before waiting for free
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static u_int32 waitBusyReady( SMB_HANDLE *smbHdl, int waitBusy )
{
u_int8 status;
int32 n;
	u_int32 error = SMB_ERR_NO;

	DBGWRT_2((DBH,"Z001 waitBusyReady\n"));
	/* wait for transfer in progress */
	if( waitBusy && smbHdl->mikroDelay ) {
		/* Mikrodelay: delay() does 5µs delay at a time */
		n = smbHdl->timeOut * 1000/5 ;
	    do {
			status = readStatus(smbHdl->baseAddr);
			if( status & Z001_SMB_STS_ERR ){
				error = SMB_ERR_ADDR;
				goto WAITBUSY_ERR_EXIT;
			}
			if( status & Z001_SMB_STS_COLL ) {
				error = SMB_ERR_COLL;
				goto WAITBUSY_ERR_EXIT;
			}
			if( status & Z001_SMB_STS_BUSY )
				break;
			delay( smbHdl );
		} while( --n > 0 );
		DBGWRT_4((DBH,"Z001 waitBusyReady: waiting for busy n=%d\n", n));

		if( n == 0 )
		{
			DBGWRT_ERR((DBH,"*** Z001 waitBusyReady: transfer did not begin\n"));
			/* don't exit, maybe busy status was missed, just continue? */
		}
	} else if( waitBusy ){
		/* one delay is definitely sufficient for bus to become busy!!! */
		delay( smbHdl );
	}

	/* Mikrodelay: delay() does 5µs delay at a time */
	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000/5 : 1 );
	/* wait for transfer to finish */
    do{
		status = readStatus( smbHdl->baseAddr );
		DBGWRT_4((DBH,"Z001 waitBusyReady: waiting to finish status=0x%x\n", status));
		if( status & Z001_SMB_STS_ERR ){
			error = SMB_ERR_ADDR;
			goto WAITBUSY_ERR_EXIT;
		}
		if( status & Z001_SMB_STS_COLL ) {
			error = SMB_ERR_COLL;
			goto WAITBUSY_ERR_EXIT;
		}
		if( !(status & Z001_SMB_STS_BUSY) ) /* wait for busy bit to be cleared*/
		{
			/* when transferring I2C message and "don't send STOP" is set: return */
			u_int8 cmd = readCmd( smbHdl->baseAddr );
			if( (cmd & 0x70) == Z001_SMB_CMD_I2C &&
				(cmd & Z001_SMB_CMD_I2C_STPDIS) )
			{
				DBGWRT_4((DBH,"Z001 waitBusyReady: I2C message, no STOP, no Idle\n"));
				return( SMB_ERR_NO );
			}

			/* wait for idle bit to be set */
	    	do{
				status = readStatus( smbHdl->baseAddr );
				if( status & Z001_SMB_STS_IDLE )
				    break;
				delay( smbHdl );
			} while( --n > 0 );

			if( !(status & Z001_SMB_STS_IDLE) ) { /* timeout? */
				DBGWRT_ERR((DBH,"*** Z001 waitBusyReady: wait for idle timeout\n"));
				error = SMB_ERR_NO_IDLE;
				goto WAITBUSY_ERR_EXIT;
			} else
				return( SMB_ERR_NO );
		}
		delay( smbHdl );
	} while ( --n > 0 );
	error = SMB_ERR_BUSY;
	/* DBGWRT_ERR((DBH,"Z001 waitBusyReady: waiting for idle n=%d\n", n)); */

WAITBUSY_ERR_EXIT:
	DBGWRT_ERR((DBH,"*** Z001 waitBusyReady: transfer failed to finish correctly (status = 0x%02x)\n", status));
	return( error );
}/*waitBusyReady*/

/*********************************  freeBus  ********************************/
/** Disconnect host and all devices from IICbus.
 *
 *  \param     smbHdl     valid SMB handle
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static void freeBus( SMB_HANDLE *smbHdl )
{
    int tries = 10;
	int numFree = 0;

	DBGWRT_ERR((DBH,"*** Z001 freeBus enter\n"));
	/* clear status first */
	writeStatus( smbHdl->baseAddr, Z001_SMB_STS_CLEAR );

    do
    {
    	if( numFree++ < (tries/2) ) {
		   	writeCmd( smbHdl->baseAddr, Z001_SMB_CMD_TOUT );
		} else {
			/* didn't success to free bus, stuck I2C bus device? */

			/* try to toggle CLOCK signal using the bypass reg   */
		    /* when data is held low */
			if( !(MREAD_D8( smbHdl->baseAddr, Z001_SMB_BYP ) & Z001_SMB_BYP_DATA_EX) ) {
				int i = 0;
				DBGWRT_ERR((DBH,"*** Z001 freeBus: CMD_TOUT not enough, SDA low, toggle clk to free\n"));
				for( i = 0; i < 9; i++ ){
					/* !SWITCH, DATA_EX, !CLK_EX */
					MWRITE_D8( smbHdl->baseAddr, Z001_SMB_BYP, Z001_SMB_BYP_DATA_EX );
					delay( smbHdl );
					/* !SWITCH, DATA_EX, CLK_EX */
					MWRITE_D8( smbHdl->baseAddr, Z001_SMB_BYP, Z001_SMB_BYP_CLK_EX | Z001_SMB_BYP_DATA_EX );
					delay( smbHdl );
				}

				/* back to normal mode:  SWITCH, DATA_EX, CLK_EX */
				MWRITE_D8( smbHdl->baseAddr, Z001_SMB_BYP, Z001_SMB_BYP_CLK_EX | Z001_SMB_BYP_DATA_EX | Z001_SMB_BYP_SWITCH  ); /* switch back to normal mode */
			}
		}

		if( waitBusyReady( smbHdl, 0) == SMB_ERR_NO ) {
			break;
		}
	} while(--tries);

	writeStatus( smbHdl->baseAddr, Z001_SMB_STS_CLEAR );
	OSS_Delay( OSSH, 200 );
}/*freeBus*/

/***************************  grabHostCtrl  *********************************/
/** initially read INUSE_ST bit and grab host controller when free.
 *
 *---------------------------------------------------------------------------
 *  \param   smbHdl     valid SMB handle
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static u_int32 grabHostCtrl( SMB_HANDLE *smbHdl )
{
	u_int8  status;
	int32 n;

	/* Mikrodelay: delay() does 5µs delay at a time */
	n = smbHdl->timeOut * ( smbHdl->mikroDelay ? 1000/5 : 1 );
	status = readStatus( (MACCESS)smbHdl->baseAddr );

	/* wait for INUSE to clear */
    while( (status & Z001_SMB_STS_INUSE) && (n-- > 0) ){
		status = readStatus( (MACCESS)smbHdl->baseAddr );
    }

    if( status & Z001_SMB_STS_INUSE ) { /* timeout? */
		DBGWRT_ERR((DBH,"*** Z001 grabHostCtrl: timeout\n"));
	    return( SMB_ERR_BUSY );
    }

    return( SMB_ERR_NO );
}

/***************************  releaseHostCtrl  ******************************/
/** release host controller (release INUSE_ST bit).
 *
 *---------------------------------------------------------------------------
 *  \param   smbHdl     valid SMB handle
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static void releaseHostCtrl( SMB_HANDLE *smbHdl )
{
	writeStatus( (MACCESS)smbHdl->baseAddr, Z001_SMB_STS_INUSE );
}
/****************************  initClrChkStatus  ****************************/
/** initially clear status and check for clean start.
 *
 *---------------------------------------------------------------------------
 *  \param   smbHdl     valid SMB handle
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static u_int32 initClrChkStatus( SMB_HANDLE *smbHdl )
{
	u_int8  status;
	u_int32 error  = SMB_ERR_NO;

	DBGWRT_2((DBH,"Z001 initClrChkStatus entry MultiM=%d\n",smbHdl->multimaster));
	/* host controller in use */
    if( (error = grabHostCtrl( smbHdl )) != SMB_ERR_NO ) {
		DBGWRT_ERR((DBH,"*** Z001 initClrChkStatus: Controller in use \n"));
		error = SMB_ERR_CTRL_BUSY;
		goto INITCLRSTS_ERR_EXIT;
    }

	/* clear and check status first */
	writeStatus( (MACCESS)smbHdl->baseAddr, Z001_SMB_STS_CLEAR );

	/* wait when bus busy, free when not in multi master mode */
	if( (error = waitBusyReady(smbHdl, 0)) != SMB_ERR_NO &&
		!smbHdl->multimaster )
	{
		if( error == SMB_ERR_ADDR || error == SMB_ERR_COLL || error == SMB_ERR_BUSY ){
			freeBus( smbHdl );
		}

		status = readStatus( (MACCESS)smbHdl->baseAddr );
		if( status & Z001_SMB_STS_ERR ){
			error = SMB_ERR_ADDR;
			goto INITCLRSTS_ERR_EXIT;
		}
		if( status & Z001_SMB_STS_COLL ) {
			error = SMB_ERR_COLL;
			goto INITCLRSTS_ERR_EXIT;
		}
		if( !(status & Z001_SMB_STS_IDLE) ) {
			error = SMB_ERR_BUSY;
			goto INITCLRSTS_ERR_EXIT;
		}
	}/*if*/

INITCLRSTS_ERR_EXIT:
	DBGWRT_2((DBH,"Z001 initClrChkStatus exit error=0x%04x\n", error));
	return( error );
}

/* Write Len bytes pointed to by *Data to the device with address
   Addr.
*/
static int Z001_i2cWrite
(
	SMB_HANDLE  *smbHdl,
	struct I2CMESSAGE *msg,
	int stop
)
{
	int error = SMB_ERR_NO;
	u_int8 cmd = Z001_SMB_CMD_I2C | Z001_SMB_CMD_BLOCK_CLR;
	u_int32 i;
	DBGCMD(	static const char functionName[] = "SMB2 - Z001_i2cWrite:"; )

	if( msg->flags&I2C_M_NOSTART ) {
		/* disable send of (re)start */
		/* tbd: future use! */
	}

	if( !stop ){
		cmd |= Z001_SMB_CMD_I2C_STPDIS;
	}

	/* clear Block Register & set Command to read/write i2c block */
	writeCmd( (MACCESS)smbHdl->baseAddr, cmd );

	/* write the up to 32 byte to block data register */
	for (i = 0; i < msg->len; i++) {
		writeBlock( (MACCESS)smbHdl->baseAddr, msg->buf[i] );
	}

	/* Data Register A contains length of transfer */
	writeData( (MACCESS)smbHdl->baseAddr, 0, (u_int8)msg->len );

	/* put device address and set read_write bit appropriatly */
	writeAddr( (MACCESS)smbHdl->baseAddr, (u_int8)(msg->addr &  ~0x01) ) ;
    writeStart( (MACCESS)smbHdl->baseAddr );

	if( (error = waitBusyReady( smbHdl, 1 )) )
	{
		DBGWRT_ERR( (DBH, "%s ERROR: waiting for RxReady!!!\n",
					 functionName) );
		goto ERR_EXIT;
	}


ERR_EXIT:
	return error;
}/* mgt5200_i2cWrite */



/* Read Len bytes to the location pointed to by *Data from the device
   with address Addr.
*/
static int Z001_i2cRead
(
	SMB_HANDLE  *smbHdl,
	struct I2CMESSAGE *msg,
	int stop
)
{
	int error = SMB_ERR_NO;
	u_int8 cmd = Z001_SMB_CMD_I2C | Z001_SMB_CMD_BLOCK_CLR;
	u_int32 i;
	DBGCMD(	static const char functionName[] = "SMB2 - Z001_i2cRead:"; )

	if( msg->flags&I2C_M_NOSTART ) {
		/* disable send of (re)start */
		/* tbd: future use! */
	}

	if( !stop ){
		cmd |= Z001_SMB_CMD_I2C_STPDIS;
	}

	/* clear Block Register & set Command to read/write i2c block */
    writeCmd( (MACCESS)smbHdl->baseAddr, cmd );

	writeAddr( (MACCESS)smbHdl->baseAddr, (u_int8)(msg->addr |  0x01) ) ;

	/* Data Register A contains length of transfer */
	writeData( (MACCESS)smbHdl->baseAddr, 0, (u_int8)msg->len );
    writeStart( (MACCESS)smbHdl->baseAddr );

	if( (error = waitBusyReady( smbHdl, 1 )) )
	{
		DBGWRT_ERR( (DBH, "%s ERROR: waiting for RxReady!!!\n",
					 functionName) );
		goto ERR_EXIT;
	}

	/* get actual read bytes from DataA register */
	msg->len = readData( smbHdl->baseAddr, 0 ); /* get length of block */

	/* get the data */
    writeCmd( (MACCESS)smbHdl->baseAddr, readCmd( smbHdl->baseAddr ) | Z001_SMB_CMD_BLOCK_CLR );
	for (i = 0; i < msg->len; i++) {
		msg->buf[i] = readBlock(smbHdl->baseAddr);
		DBGWRT_3( (DBH, "%s BLOCK_DATA len=%d, i=%d, data=%02x\n",
			 	   functionName, msg->len, i, msg->buf[i]) );
	}

ERR_EXIT:
	return error;
}/* Z001_i2cRead */


/***************************** SMB_MENZ001_Init ****************************/
/** Initializes this library and check's the SMB host.
 *
 * \param  descP    \IN  descriptor
 * \param  osHdl    \IN  OS specific handle
 * \param  smbHdlP  \IN  pointer to variable where the handle will be stored
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
#ifdef _Z001_ADDR_DWORD_ALINGED_
u_int32 SMB_MENZ001_Init_Aligned
#elif MAC_IO_MAPPED
u_int32 SMB_MENZ001_IO_Init
#else
u_int32 SMB_MENZ001_Init
#endif
(
    SMB_DESC_MENZ001	*descP,
    OSS_HANDLE			*osHdl,
	void				**smbHdlP
)
{
	u_int32     error  = 0;
	SMB_HANDLE  *smbHdl;
	u_int32		gotSize = 0;

	*smbHdlP = NULL;
	smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
	if( smbHdl == NULL )
	{
	    error = SMB_ERR_NO_MEM;
		goto CLEANUP;
	}/*if*/

	*smbHdlP = smbHdl;

	/* init the structure */
	OSS_MemFill( osHdl, gotSize, (char*)smbHdl, 0 ); /* zero memory */

	smbHdl->ownSize  			= gotSize;

	DBGINIT(( NULL, &smbHdl->smbComHdl.dbgHdl ));
	smbHdl->smbComHdl.dbgLevel	= descP->dbgLevel;

	DBGWRT_1( (DBH, "SMB_MENZ001_Init smbHdl=%08p\n", smbHdl) );

	smbHdl->entries.Capability = SMB_FUNC_SMBUS_BYTE |
								 SMB_FUNC_SMBUS_BYTE_DATA |
								 SMB_FUNC_SMBUS_WORD_DATA |
								 SMB_FUNC_SMBUS_BLOCK_DATA |
								 SMB_FUNC_SMBUS_QUICK |
								 SMB_FUNC_I2C;

	smbHdl->smbComHdl.busyWait = descP->busyWait;
	smbHdl->smbComHdl.alertPollFreq = descP->alertPollFreq;
	smbHdl->smbComHdl.osHdl    	= (OSS_HANDLE*) osHdl;

	if( (error = SMB_COM_Init(smbHdl)) )
	{
		goto CLEANUP;
	}

	smbHdl->baseAddr 			= descP->baseAddr;
	if( smbHdl->timeOut > MAX_TIMEOUT )
	{
		smbHdl->timeOut 		= MAX_TIMEOUT;
	}else
	{
		smbHdl->timeOut 		= descP->timeOut;
	}
	smbHdl->mikroDelay 			= descP->mikroDelay;
	smbHdl->multimaster			= descP->multimasterCap;

	smbHdl->entries.Exit		= (int32 (*)(void**))Z001_smbExit;
	smbHdl->entries.SmbXfer		= (int32 (*)(void *, u_int32, u_int16,
							  				u_int8, u_int8, u_int8, u_int8 *))
											Z001_SmbXfer;

	smbHdl->entries.I2CXfer		= (int32 (*)( void *smbHdl,
											  struct I2CMESSAGE msg[],
											  u_int32 num))Z001_I2cXfer;
	smbHdl->entries.Ident		= (char* (*)(void))Z001_smbIdent;

	/* setup frequency: */
	switch( descP->sclFreq )
	{
	case 37:
		MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_37KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 55:
		MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_55KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 111:
		MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_111KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 149:
		MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_149KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 223:
		MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_223KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	default:
		MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_74KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	}

	/* activate and reset host controller */
	MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HSI, Z001_SMB_HSI_HCIE );

	if( !smbHdl->multimaster ){	
		/* wait for max. 500ms  */
		u_int32 n = 500;
		u_int8 status;
		
		writeCmd( (MACCESS)smbHdl->baseAddr, Z001_SMB_CMD_BLOCK_CLR | Z001_SMB_CMD_ABORT );
		/*wait till abort operation finishes. otherwise the terminate bit can
		be set when read/write program checks the status bit and eventually lead
		to an error*/
		while(n--){
			status = readStatus( (MACCESS)smbHdl->baseAddr );
			if( (status & Z001_SMB_STS_TERM) && (!(status & Z001_SMB_STS_BUSY) )){				
				break;
			}
			OSS_Delay(osHdl, 1);
		}		
		if( n == 0)		
			error = SMB_ERR_BUSY;
	}	
	else
		writeCmd( (MACCESS)smbHdl->baseAddr, Z001_SMB_CMD_BLOCK_CLR );

	/* delete pending SMI_I_STS irq bit & Inuse bit, is cleared by writing a '1' */
	writeStatus( (MACCESS)smbHdl->baseAddr, Z001_SMB_STS_TERM |
											Z001_SMB_STS_DONE | Z001_SMB_STS_INUSE );

	goto END;
CLEANUP:
	if( smbHdl )
	{
		OSS_MemFree( OSSH, smbHdl, gotSize );
		*smbHdlP = NULL;
	}
END:
	return( error );
}/*SMB_Z001_Init*/

/*******************************  Z001_smbExit  *****************************/
/** Deinitializes this library and SMB controller.
 *
 * \param smbHdlP	\INOUT  pointer to variable where the handle is stored,
 *                          set to 0
 * \return    always 0
 *
 ****************************************************************************/
static u_int32 Z001_smbExit
(
	SMB_HANDLE **smbHdlP
)
{
	u_int32 error  = 0;
	SMB_HANDLE  *smbHdl;

	smbHdl = *smbHdlP;
	*smbHdlP = NULL;

	DBGWRT_1( (DBH, "SMB_MENZ001_Exit smbHdl=%08p\n", smbHdl) );

	/* deinitialize common interface */
	if(smbHdl->smbComHdl.ExitCom)
		smbHdl->smbComHdl.ExitCom(smbHdl);

	/* disable Host Controller and interrupts */
	MWRITE_D8( smbHdl->baseAddr, Z001_SMB_HSI, 0x00);

	/* clear status first */
	writeStatus( (MACCESS)smbHdl->baseAddr, Z001_SMB_STS_CLEAR );

    OSS_MemFree( OSSH, smbHdl, smbHdl->ownSize );

	return( error );
}/*Z001_smbExit*/



/******************************** Z001_SmbXfer *****************************/
/** Read/Write data from a device using the SMBus protocol
 *
 *  \param   smbHdl     valid SMB handle
 *	\param   addr       device address
 *	\param   flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param   read_write flag indicating read(=1) or write (=0) access
 *	\param   cmdAdr     data to be sent in command field
 *	\param   size       size of data access (Quick/Byte/Word/Block/(Blk-)Proc
 *	\param   data       pointer to variable with Tx data and for Rx data
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 Z001_SmbXfer
(
	SMB_HANDLE *smbHdl,
	u_int32 flags,
	u_int16 addr,
	u_int8 read_write,
	u_int8 cmdAddr,
	u_int8 size,
	u_int8 *data
)
{
	u_int32 error = 0, i;
	u_int8 len = 0;
	DBGCMD(	static const char functionName[] = "SMB2 - Z001_SmbXfer:"; )

	DBGWRT_1( (DBH, "%s %s addr: 0x%02x flags: 0x%02x size: %d\n",
			   functionName,
			   (read_write ? "read" : "write"),
			   addr, flags, size) );

    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	if( (error = initClrChkStatus( smbHdl )) ){
		goto ERR_EXIT;
	}

	switch(size)
	{
		case SMB_ACC_QUICK:
			DBGWRT_3( (DBH, "%s SMB_ACC_QUICK\n", functionName) );
			size = Z001_SMB_CMD_QUICK;
			break;
		case SMB_ACC_BYTE:
			DBGWRT_3( (DBH, "%s SMB_ACC_BYTE cmd = 0x%02x\n",
					   functionName, cmdAddr) );

			if ( read_write == SMB_WRITE )
			{
		   		writeCmdForHost( (MACCESS)smbHdl->baseAddr, cmdAddr );
		   	}

			size = Z001_SMB_CMD_BYTE;
			break;
		case SMB_ACC_BYTE_DATA:
			DBGWRT_3( (DBH, "%s SMB_ACC_BYTE_DATA cmd: 0x%02x\n",
					   functionName, cmdAddr) );

			writeCmdForHost( (MACCESS)smbHdl->baseAddr, cmdAddr );
			if ( read_write == SMB_WRITE )
			{
				DBGWRT_3( (DBH, "%s SMB_ACC_BYTE_DATA write data: 0x%02x\n",
					 	   functionName, *data) );
				writeData( (MACCESS)smbHdl->baseAddr, 0, *data );
			}

			size = Z001_SMB_CMD_BYTE_DATA;
			break;
		case SMB_ACC_WORD_DATA:
			DBGWRT_3( (DBH, "%s SMB_ACC_WORD_DATA cmd: 0x%02x\n",
					   functionName, cmdAddr) );
			writeCmdForHost( (MACCESS)smbHdl->baseAddr, cmdAddr );
			if ( read_write == SMB_WRITE )
			{
				DBGWRT_3( (DBH, "%s SMB_ACC_WORD_DATA write data: 0x%04x\n",
					 	   functionName, *(u_int16*)data) );
				for(i=0; i<3000; i++)
					;

				writeData( (MACCESS)smbHdl->baseAddr, 0, data[0] );
				writeData( (MACCESS)smbHdl->baseAddr, 1, data[1] );
			}

			size = Z001_SMB_CMD_WORD_DATA;
			break;
		case SMB_ACC_BLOCK_DATA:
			DBGWRT_3( (DBH, "%s SMB_ACC_BLOCK_DATA cmd: 0x%02x len: 0x%02x\n",
							functionName, cmdAddr, data[0] ) );
			writeCmdForHost( (MACCESS)smbHdl->baseAddr, cmdAddr );
			if (read_write == SMB_WRITE) {
				len = data[0];
				if (len > SMB_BLOCK_MAX_BYTES) {
					len = SMB_BLOCK_MAX_BYTES;
					data[0] = len;
				}
				/* clear Block Register & set Command to read/write block */
				writeCmd( (MACCESS)smbHdl->baseAddr, Z001_SMB_CMD_BLOCK_DATA |
													Z001_SMB_CMD_BLOCK_CLR );

				/* Data Register A contains length of transfer */
				writeData( (MACCESS)smbHdl->baseAddr, 0, len );

				/* write the up to 32 byte to block Data Register */
				for (i = 1; i <= len; i++)
					writeBlock( (MACCESS)smbHdl->baseAddr, data[i] );
			}

			size = Z001_SMB_CMD_BLOCK_DATA;
			break;
		default:
			DBGWRT_ERR( (DBH, "%s ERROR: access size not known: %d!!!\n",
						 functionName, size) );

			error = SMB_ERR_NOT_SUPPORTED;
			goto ERR_EXIT;
	}

    writeAddr( (MACCESS)smbHdl->baseAddr, (u_int8)(addr | (read_write & 0x01)) );
    writeCmd( (MACCESS)smbHdl->baseAddr, size );
    writeStart( (MACCESS)smbHdl->baseAddr );

	if( (error = waitBusyReady( smbHdl, 1 )) )
	{
		DBGWRT_ERR( (DBH, "%s ERROR: waiting for TxReady!!!\n",
					 functionName) );
		goto ERR_EXIT;
	}

	/* if we made a write or quick access, we are finished */
	if ((read_write == SMB_WRITE) || (size == Z001_SMB_CMD_QUICK))
		goto ERR_EXIT;

	/* it is a read access, get the data */
	switch( size )
	{
		case Z001_SMB_CMD_BYTE: /* Result put in SMB_DATA_A */
		case Z001_SMB_CMD_BYTE_DATA:
			*data = readData( smbHdl->baseAddr, 0 );
			DBGWRT_3( (DBH, "%s BYTE(_DATA) data=%04x\n",
				 	   functionName, *data) );
			break;
		case Z001_SMB_CMD_WORD_DATA:
			*(u_int16*)data = (u_int16)((readData( smbHdl->baseAddr, 1 ) << 8 ) |
					 		  readData( smbHdl->baseAddr, 0 ));
			DBGWRT_3( (DBH, "%s WORD_DATA wData=%04x\n",
				 	   functionName, *(u_int16*)data) );
			break;
		case Z001_SMB_CMD_BLOCK_DATA:
			len = readData( smbHdl->baseAddr, 0 ); /* get length of block */
			if( len > SMB_BLOCK_MAX_BYTES )
				len = SMB_BLOCK_MAX_BYTES;

			data[0] = len;
			/* clear Block Register & get read block */
			writeCmd( smbHdl->baseAddr, readCmd( smbHdl->baseAddr ) |
										Z001_SMB_CMD_BLOCK_CLR );

			/* get the data */
			for (i = 1; i <= len; i++) {
				data[i] = readBlock(smbHdl->baseAddr);
				DBGWRT_3( (DBH, "%s BLOCK_DATA len=%d, i=%d, data=%02x\n",
					 	   functionName, len, i, data[i]) );
			}

			break;
	}

ERR_EXIT:
	if( error != SMB_ERR_CTRL_BUSY )
		releaseHostCtrl( smbHdl );
    return( error );
}


/********************************** Z001_i2cXfer ****************************/
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
static int32 Z001_I2cXfer
(
	SMB_HANDLE *smbHdl,
	struct I2CMESSAGE msg[],
	u_int32 num
)
{
	struct I2CMESSAGE *pmsg;
	u_int32 i = 0;
	int32 error = SMB_ERR_NO;
	int32 doStop = 0, didStop = 0;
	DBGCMD(	static const char functionName[] = "SMB2 - Z001_I2cXfer:"; )

	if( !msg ){
			error = SMB_ERR_PARAM;
			goto ERR_EXIT;
	}
	DBGWRT_1( (DBH, "%s numMsg: %d\n",
			   functionName, num) );

	if( (error = initClrChkStatus( smbHdl )) ){
		DBGWRT_ERR( (DBH, "*** %s ERROR: failed to get control of Z001_SMB controller or bus\n",
					 functionName) );
		goto ERR_EXIT;
	}

	/* tingle through messages */
	for( i = 0; error == SMB_ERR_NO && i < num; i++ ){
		pmsg = &msg[i];

		/* sanity checks */
		if( !pmsg ||
			pmsg->flags & I2C_M_TEN ||
			pmsg->len > SMB_BLOCK_MAX_BYTES ){
			DBGWRT_ERR( (DBH, "*** %s ERROR: illegal parameter!!!\n",
						 functionName) );
			error = SMB_ERR_PARAM;
			goto ERR_EXIT;
		}
		if(i == (num - 1)) /* send stop on last packet */
			doStop = 1;

		if( pmsg->flags & I2C_M_RD )
			error = Z001_i2cRead( smbHdl, pmsg, doStop );
		else
			error = Z001_i2cWrite( smbHdl, pmsg, doStop );

		if( doStop && !error )
			didStop = 1;
	}
ERR_EXIT:

	/* terminate accesses with stop condition if not already done. */
	/* use last sent SMBus address as device address */
	if( i != 0 && !didStop ) {
		struct I2CMESSAGE tmpMsg;

		tmpMsg.buf = 0;
		tmpMsg.len = 0;
		tmpMsg.addr = msg[i-1].addr;
		tmpMsg.flags = msg[i-1].flags & I2C_M_TEN ;

		Z001_i2cRead( smbHdl, &tmpMsg, 1 );
	}

	releaseHostCtrl( smbHdl );
	return error;
}



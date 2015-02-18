/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysmanagbus.c
 *      Project: MENMON A11
 *
 *       Author: uf
 *        $Date: 2007/07/26 15:17:29 $
 *    $Revision: 1.10 $
 *
 *  Description: system managment bus driver for the ALI1543C
 *				 SMB controller
 *
 *     Required: -
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysmanagbus_ali1543.c,v $
 * Revision 1.10  2007/07/26 15:17:29  cs
 * added:
 *   + workaround for ALI SMB controller hangup
 *     (would hang without error signaling, SW Interface works fine
 *      no I2C bus activity)
 *     now: do SMB controller Reset (ABORT) after every finished SMB cycle
 * cosmetics
 *
 * Revision 1.9  2000/08/08 11:00:59  kp
 * fixed casts to entry functions
 *
 * Revision 1.8  2000/08/03 19:28:19  Franke
 * added WriteReadTwoByte
 *
 * Revision 1.7  2000/08/01 09:45:44  Franke
 * added ReadBlock/WriteBlock empty functions only
 * added ident function
 *
 * Revision 1.6  2000/06/27 13:02:08  Franke
 * changed SMB_ALI1543_Init type of osHdl now OSS_HANDLE instead of void*
 *
 * Revision 1.5  1999/03/01 16:27:34  Franke
 * change SMB_HANDLE, function names and interfaces
 *
 * Revision 1.4  1999/03/01 12:22:07  Franke
 * added SMB_ReadByte() set read byte to 0xFF at error
 *
 * Revision 1.3  1999/01/28 16:26:17  kp
 * fixed pointer arithmetic
 *
 * Revision 1.2  1998/12/08 17:29:19  Franke
 * replaced delay loops with OSS_Delay()
 *
 * Revision 1.1  1998/12/01 17:38:49  Franke
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
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
	u_int32    ownSize;
	OSS_HANDLE *osHdl;
	u_int32	   dbgLevel;
	DBG_HANDLE *dbgHdl;
	void       *baseAddr;
}SMB_HANDLE;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define SMB_HOST_STATUS			0x00
#define SMB_HOST_SLV_CMD   		0x01
#define SMB_HOST_START_PRT 	 	0x02
#define SMB_HOST_ADDR   		0x03
#define SMB_HOST_DATA_A  		0x04
#define SMB_HOST_DATA_B			0x05
#define SMB_HOST_BLK   			0x06
#define SMB_BLK_CMD_FOR_HOST	0x07


#define SMB_CLEAR_STATUS      0xFF

#define SMB_HOST_STATUS_TERM	0x80
#define SMB_HOST_STATUS_COLL	0x40
#define SMB_HOST_STATUS_ERR		0x20
#define SMB_HOST_STATUS_INT		0x10
#define SMB_HOST_STATUS_BUSY	0x08   /*RO*/
#define SMB_HOST_STATUS_IDLE	0x04   /*RO*/
#define SMB_HOST_STATUS_SLV_INT	0x02
#define SMB_HOST_STATUS_SLV_BSY	0x01   /*RO*/



#define SMB_READ           0x01
#define SMB_WRITE          0x00


#define SMB_CMD_QUICK         0x00
#define SMB_CMD_SEND_REC_BYTE 0x10
#define SMB_CMD_WR_RD_BYTE    0x20
#define SMB_CMD_WR_RD_WORD    0x30
#define SMB_CMD_WR_RD_BLOCK   0x40
#define SMB_CMD_BLOCK_CLEAR   0x80

#define SMB_CMD_T_ABORT		  0x04	/* ABORT /TERMINATE */
#define SMB_CMD_T_OUT		  0x08	/* T_OUT_CMD */

#define	DBG_MYLEVEL		smbHdl->dbgLevel
/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/
DBGCMD(	static const char errorStartStr[] =	"*** ERROR - ";	)
DBGCMD(	static const char errorLineStr[]  =	" (line	";		)
DBGCMD(	static const char errorEndStr[]	  =	")***\n";		)
/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

static char* smbIdent( void );
static u_int32 smbExit(         SMB_HANDLE  **smbHdlP );
static u_int32 smbWriteByte(    SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 data );
static u_int32 smbReadByte(     SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 *dataP );
static u_int32 smbWriteTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 data );
static u_int32 smbWriteReadTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 *data1P, u_int8 *data2P );
/*static u_int32 smbWriteReadBlock(	SMB_HANDLE  *smbHdl, u_int8 addr, u_int32 size,	u_int8 *buf, u_int8 cmdOrIndex );
static u_int32 smbWriteBlock(	    SMB_HANDLE  *smbHdl, u_int8 addr, u_int32 size,	u_int8 *buf );*/

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
    return( "SMB - SMB library: $Id: sysmanagbus_ali1543.c,v 1.10 2007/07/26 15:17:29 cs Exp $" );
}/*smbIdent*/


static void writeStatus( void* baseAddr, u_int8 data )
{
	MWRITE_D8(baseAddr,SMB_HOST_STATUS, data );
}/*writeStatus*/

static u_int8 readStatus( void* baseAddr )
{
    return( MREAD_D8( baseAddr,SMB_HOST_STATUS ) );
}/*readStatus*/

static u_int8 readData( void* baseAddr, int index )
{
    return( MREAD_D8( baseAddr, (SMB_HOST_DATA_A + index) ) );
}/*readData*/

/* index DATA_A=0, DATA_B=1 */
static void writeData( void* baseAddr, int index, u_int8 data )
{
    MWRITE_D8( baseAddr,(SMB_HOST_DATA_A + index), data );
}/*writeData*/

static void writeCmd( void* baseAddr, u_int8 data )
{
    MWRITE_D8(baseAddr,SMB_HOST_SLV_CMD,data);
}/*writeCmd*/

static void writeCmdForHost( void* baseAddr, u_int8 data )
{
    MWRITE_D8(baseAddr,SMB_BLK_CMD_FOR_HOST,data);
}/*writeCmdForHost*/

static void writeStart( void* baseAddr )
{
    MWRITE_D8(baseAddr,SMB_HOST_START_PRT,0xff);

}/*writeStart*/

static void writeAddr( void* baseAddr, u_int8 data )
{
    MWRITE_D8(baseAddr,SMB_HOST_ADDR,data);
}/*writeAddr*/

static void resetController( void* baseAddr )
{
	writeCmd( baseAddr, MREAD_D8( baseAddr, SMB_HOST_SLV_CMD ) | SMB_CMD_T_ABORT );

}/*resetController*/

/*******************************  waitBusyReady  *********************************
 *
 *  Description:  Waits to BUSY first and then leaving BUSY and bus idle.
 *				  Returns imediatly if DEV_ERR detected.
 *
 *---------------------------------------------------------------------------
 *  Input......:  baseAddr 	base address of the SMB controller
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 waitBusyReady( SMB_HANDLE  *smbHdl )
{
	void* baseAddr = smbHdl->baseAddr;
	int    loopy;
	u_int8 status;

	/* wait to get busy (in case we ask too fast */
    for( loopy = 0; loopy<2000; loopy++ )
	{
		status = readStatus(baseAddr);
		if( status & SMB_HOST_STATUS_ERR )
			return( SMB_ERR_ADDR );
		if( status & SMB_HOST_STATUS_COLL )
			return( SMB_ERR_COLL );
		if( status & SMB_HOST_STATUS_BUSY )
			break;
	}/**/

	/* wait for not busy */
    for( loopy = 1; loopy<20000; loopy++ )
	{
		status = readStatus(baseAddr);
		if( status & SMB_HOST_STATUS_ERR )
			return( SMB_ERR_ADDR );
		if( status & SMB_HOST_STATUS_COLL )
			return( SMB_ERR_COLL );
		if( !(status & SMB_HOST_STATUS_BUSY) )
		{
			/* wait for idle */
	    	for( loopy = 0; loopy<20000; loopy++ )
			{
				status = readStatus(baseAddr);
				if( status & SMB_HOST_STATUS_IDLE )
				    break;
			}/*for*/
			return( SMB_ERR_NO );
		}
		if( !(loopy % 2000) )
			OSS_Delay( smbHdl->osHdl, 1 ); /* be nice to other tasks when it takes longer */
	}/**/
	return( SMB_ERR_BUSY );
}/*waitReady*/

/*********************************  freeBus  *********************************
 *
 *  Description:  Disconnect host and all devices from IICbus.
 *
 *---------------------------------------------------------------------------
 *  Input......:  baseAddr 	base address of the SMB controller
 *                osHdl     OS specific handle
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static void freeBus( SMB_HANDLE  *smbHdl )
{

	void* baseAddr = smbHdl->baseAddr;
	int    loopy;
	u_int8 status;

	/* first give it a chance to complete old commands or recover from Reset */
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
    for( loopy = 1; loopy<2000; loopy++ )
	{
		if( !(loopy % 500) )
			OSS_Delay( smbHdl->osHdl, 1 ); /* be nice to other tasks in case it takes longer */

		status = readStatus(baseAddr);

		if( !( status & (SMB_HOST_STATUS_BUSY|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_COLL)) &&
			(status & SMB_HOST_STATUS_IDLE) )
		{
		    goto ALISMB_EXIT;
		}
		writeStatus( baseAddr, SMB_CLEAR_STATUS );
	}

	/* clear status first */
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	waitBusyReady(baseAddr);
   	writeCmd( baseAddr, SMB_CMD_T_OUT );
	waitBusyReady(baseAddr);
	writeStatus( baseAddr, SMB_CLEAR_STATUS );
	OSS_Delay( smbHdl->osHdl, 200 );
ALISMB_EXIT:
}/*freeBus*/

/****************************** SMB_ALI1543_Init *****************************
 *
 *  Description:  Initializes this library and check's the SMB host.
 *
 *---------------------------------------------------------------------------
 *  Input......:  baseAddr 	base address of the SMB controller
 *                osHdl     OS specific handle
 *                smbHdlP	pointer to variable where the handle will be stored
 *
 *  Output.....:  return    0 | error code
 *				  *smbHdlP	valid handle | NULL
 *
 *  Globals....:  -
 ****************************************************************************/
u_int32 SMB_ALI1543_Init
(
    SMB_DESC_ALI1543	*descP,
    OSS_HANDLE			*osHdl,
	void				**smbHdlP
)
{
	u_int32     error  = 0;
	SMB_HANDLE  *smbHdl;
	u_int32		gotSize;

	*smbHdlP = NULL;
	smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
	if( smbHdl == NULL )
	{
	    error = SMB_ERR_NO_MEM;
		goto CLEANUP;
	}/*if*/

	/* init the structure */
	OSS_MemFill( osHdl, gotSize, (char*)smbHdl, 0 );

	DBGINIT((NULL,&smbHdl->dbgHdl));
	smbHdl->dbgLevel			= (DBG_NORM | DBG_LEVERR);

	smbHdl->baseAddr 			= descP->baseAddr;
	smbHdl->ownSize  			= gotSize;
	smbHdl->osHdl    			= (OSS_HANDLE*) osHdl;

	smbHdl->entries.Exit			= (int32 (*)(void**))smbExit;
	smbHdl->entries.WriteByte		= (int32 (*)(void*,u_int8,u_int8))
		                                smbWriteByte;
	smbHdl->entries.ReadByte		= (int32 (*)(void*,u_int8,u_int8*))
									    smbReadByte;
	smbHdl->entries.WriteTwoByte	= (int32 (*)(void*,u_int8,u_int8,u_int8))
		                               smbWriteTwoByte;

	smbHdl->entries.Ident		= smbIdent;
	smbHdl->entries.WriteReadTwoByte= (int32 (*)(void *,u_int8,u_int8,u_int8*,u_int8*))smbWriteReadTwoByte;

	/* clear status first */
	writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );

	DBGWRT_1((smbHdl->dbgHdl, "SMB: SMB_ALI1543_Init \n" ) );
	*smbHdlP = (void*) smbHdl;

CLEANUP:
	return( error );
}/*SMB_ALI1543_Init*/

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

	/* reset controller first */
	resetController(smbHdl->baseAddr);

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
	u_int8  status;
	u_int32 error = 0;
	DBGCMD(	static const char functionName[] = "SMB - smbReadByte:"; )

    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	DBGWRT_1((smbHdl->dbgHdl, "%s\n", functionName ) );
	/* set to default */
	*dataP = 0xFF;

	/* clear and check status first */
	writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
	status = readStatus(smbHdl->baseAddr);
	if( !(status & SMB_HOST_STATUS_IDLE) ||
    	( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
	  )
	{
		if( !(status & SMB_HOST_STATUS_IDLE) )
	        freeBus( smbHdl );

		writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
		status = readStatus(smbHdl->baseAddr);
		if( !(status & SMB_HOST_STATUS_IDLE) ||
    		( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
		  )
		{
			DBGWRT_ERR((smbHdl->dbgHdl, "*** %s bus busy sts=0x%04x!!\n", functionName, status ) );
			error = SMB_ERR_BUSY;
			goto ALISMB_EXIT;
		}/*if*/
	}/*if*/

    writeAddr(smbHdl->baseAddr, addr | SMB_READ );
    writeCmd(smbHdl->baseAddr, SMB_CMD_SEND_REC_BYTE );
    writeStart(smbHdl->baseAddr);

	if( (error = waitBusyReady(smbHdl)))
			goto ALISMB_EXIT;

    *dataP = readData(smbHdl->baseAddr, 0 );
	DBGWRT_3((smbHdl->dbgHdl, "\t\t%02x\n", *dataP ) );

ALISMB_EXIT:
	resetController(smbHdl->baseAddr);
    return( error );
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
	u_int8  status;
	u_int32 error = 0;
	DBGCMD(	static const char functionName[] = "SMB - smbWriteByte:"; )

    if(	addr & 0x01 )
		return( SMB_ERR_ADDR );

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x  data %02x\n", functionName, addr, data ) );

	/* clear and check status first */
	writeStatus(smbHdl->baseAddr, SMB_CLEAR_STATUS );
	status = readStatus(smbHdl->baseAddr);
	if( !(status & SMB_HOST_STATUS_IDLE) ||
    	( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
	  )
	{
		if( !(status & SMB_HOST_STATUS_IDLE) ){
	        freeBus( smbHdl );
	    }
		writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
		status = readStatus( smbHdl->baseAddr );
		if( !(status & SMB_HOST_STATUS_IDLE) ||
    		( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
		  )
		{
			DBGWRT_ERR((smbHdl->dbgHdl, "*** %s bus still busy sts=0x%04x!!\n", functionName, status ) );
			error = SMB_ERR_BUSY;
			goto ALISMB_EXIT;
		}/*if*/
	}/*if*/

    writeCmdForHost(smbHdl->baseAddr, data );
    writeAddr(smbHdl->baseAddr, addr | SMB_WRITE );
    writeCmd(smbHdl->baseAddr, SMB_CMD_SEND_REC_BYTE );
    writeStart(smbHdl->baseAddr);

	if( (error = waitBusyReady(smbHdl)))
	    goto ALISMB_EXIT;


ALISMB_EXIT:
	resetController(smbHdl->baseAddr);
    return( error );
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
	u_int8  status;
	u_int32 error = 0;
	DBGCMD(	static const char functionName[] = "SMB - smbWriteTwoByte:"; )

    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	DBGWRT_1((smbHdl->dbgHdl, "smbWriteTwoByte addr %02x  cmdAddr %02x data %02x\n", functionName, addr, cmdAddr, data ) );

	/* clear and check status first */
	writeStatus(smbHdl->baseAddr, SMB_CLEAR_STATUS );
	status = readStatus(smbHdl->baseAddr);
	if( !(status & SMB_HOST_STATUS_IDLE) ||
    	( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
	  )
	{
		if( !(status & SMB_HOST_STATUS_IDLE) )
	        freeBus( smbHdl );
		writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
		status = readStatus( smbHdl->baseAddr );
		if( !(status & SMB_HOST_STATUS_IDLE) ||
    		( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
		  )
		{
			error = SMB_ERR_BUSY;
			goto ALISMB_EXIT;
		}/*if*/
	}/*if*/

    writeAddr( smbHdl->baseAddr, addr | SMB_WRITE );
    writeCmd( smbHdl->baseAddr, SMB_CMD_WR_RD_BYTE );
    writeData( smbHdl->baseAddr, 0, data );
    writeCmdForHost( smbHdl->baseAddr, cmdAddr );
    writeStart(smbHdl->baseAddr);

	if( (error = waitBusyReady(smbHdl)))
	    goto ALISMB_EXIT;


ALISMB_EXIT:
	resetController(smbHdl->baseAddr);
    return( error );
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
static u_int32 smbWriteReadTwoByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      cmdAddr,
	u_int8      *data1P,
	u_int8      *data2P
)
{
	u_int8  status;
	u_int32 error = 0;
	DBGCMD(	static const char functionName[] = "SMB - smbWriteReadTwoByte:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x  cmd %d\n", functionName, addr, cmdAddr ) );

    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	/* clear and check status first */
	writeStatus(smbHdl->baseAddr, SMB_CLEAR_STATUS );
	status = readStatus(smbHdl->baseAddr);
	if( !(status & SMB_HOST_STATUS_IDLE) ||
    	( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
	  )
	{
		if( !(status & SMB_HOST_STATUS_IDLE) )
	        freeBus( smbHdl );
		writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
		status = readStatus( smbHdl->baseAddr );
		if( !(status & SMB_HOST_STATUS_IDLE) ||
    		( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
		  )
		{
	    	error = SMB_ERR_BUSY;
	    	goto ALISMB_EXIT;
		}/*if*/
	}/*if*/

    writeAddr( smbHdl->baseAddr, addr | SMB_READ );
    writeCmd( smbHdl->baseAddr, SMB_CMD_WR_RD_WORD );
    writeCmdForHost( smbHdl->baseAddr, cmdAddr );
    writeStart(smbHdl->baseAddr);

	if( (error = waitBusyReady(smbHdl)))
		goto ALISMB_EXIT;

    *data1P = readData(smbHdl->baseAddr, 0 );
    *data2P = readData(smbHdl->baseAddr, 1 );

	DBGWRT_3((smbHdl->dbgHdl, "\t\t%02x %02x\n", *data1P, *data2P ) );

ALISMB_EXIT:
	resetController(smbHdl->baseAddr);
    return( error );
}/*smbWriteReadTwoByte*/

#if 0  /* ALI - write read block does not work like HW description - disabled uf 3.8.2000*/
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
static u_int32 smbWriteReadBlock
(
	SMB_HANDLE	*smbHdl,
	u_int8 		addr,
	u_int32		size,
	u_int8		*buf,
	u_int8 		cmdOrIndex
)
{
	u_int8  status;
	u_int32 error = 0;
	u_int8  data;
	int i;

    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	/* clear and check status first */
	writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
	status = readStatus(smbHdl->baseAddr);
	if( !(status & SMB_HOST_STATUS_IDLE) ||
    	( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
	  )
	{
		if( !(status & SMB_HOST_STATUS_IDLE) )
	        freeBus( smbHdl );
		writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
		status = readStatus(smbHdl->baseAddr);
		if( !(status & SMB_HOST_STATUS_IDLE) ||
    		( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
		  )
		{
	    	return( SMB_ERR_BUSY );
		}/*if*/
	}/*if*/

    writeAddr(smbHdl->baseAddr, addr | SMB_READ );
    writeCmd(smbHdl->baseAddr, SMB_CMD_WR_RD_BLOCK | SMB_CMD_BLOCK_CLEAR ); /* BLK cmd + CLR ALI internal buffer index */
	writeData( smbHdl->baseAddr, 0, (u_int8)size );
	writeCmdForHost( smbHdl->baseAddr, cmdOrIndex );
    writeStart(smbHdl->baseAddr);

	/* OSS_Delay( smbHdl->osHdl, 500 );*/

	if( (error = waitBusyReady(smbHdl)))
    	return( error );

	for( i=0;i<32;i++ )
	{
		data = *((u_int8*)smbHdl->baseAddr+SMB_HOST_BLK);
		printf(" %02x", data );
	}/*for*/
	printf("\n");

    /**buf++ = readData(smbHdl->baseAddr, 0 );*/

    return( 0 );
}/*smbWriteReadBlock*/

/******************************** smbWriteBlock ******************************
 *
 *  Description:  Writes a block to a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	valid SMB handle
 *				  addr		device address
 *				  size      buffer size  2..33
 *				  buf       buffer location
 *
 *  Output.....:  return    0 | error code
 *				  *dataP	read value
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbWriteBlock
(
	SMB_HANDLE	*smbHdl,
	u_int8 		addr,
	u_int32		size,
	u_int8		*buf
)
{
	u_int8  status;
	u_int8  data;
	u_int32 error = 0;
	u_int32	currSize = size;
	int i;

	DBGCMD(	static const char functionName[] = "SMB - smbWriteBlock:"; )

	DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TODO Workaround for Silicon bug %s%d%s",
				  errorStartStr, functionName,
				  error,
				  errorLineStr, __LINE__, errorEndStr ));
   	return( SMB_ERR_PARAM );

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x  size %d\n", functionName, addr, currSize) );

    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	if( size < 2 || size > 33 ) /* ALI internal buffer size == 32 byte */
	{
    	return( SMB_ERR_PARAM );
	}/*if*/

	/* clear and check status first */
	writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
	status = readStatus(smbHdl->baseAddr);
	if( !(status & SMB_HOST_STATUS_IDLE) ||
    	( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
	  )
	{
		if( !(status & SMB_HOST_STATUS_IDLE) )
	        freeBus( smbHdl );
		writeStatus( smbHdl->baseAddr, SMB_CLEAR_STATUS );
		status = readStatus(smbHdl->baseAddr);
		if( !(status & SMB_HOST_STATUS_IDLE) ||
    		( status & ( SMB_HOST_STATUS_COLL|SMB_HOST_STATUS_ERR|SMB_HOST_STATUS_BUSY ) )
		  )
		{
	    	return( SMB_ERR_BUSY );
		}/*if*/
	}/*if*/

    writeAddr(smbHdl->baseAddr, addr | SMB_WRITE );
    writeCmd(smbHdl->baseAddr, SMB_CMD_WR_RD_BLOCK | SMB_CMD_BLOCK_CLEAR ); /* BLK cmd + CLR ALI internal buffer index */
	DBGWRT_3((smbHdl->dbgHdl, "cmd %02x\n", SMB_CMD_WR_RD_BLOCK | SMB_CMD_BLOCK_CLEAR) );


	/* like example in manual */
    MWRITE_D8(smbHdl->baseAddr, 3, addr); /* adr */
    MWRITE_D8(smbHdl->baseAddr, 1, 0xc0); /* BLK RW */
    MWRITE_D8(smbHdl->baseAddr, 4, 6); /* DATA size */
    MWRITE_D8(smbHdl->baseAddr, 6, 0x07); /* BLK data */
    MWRITE_D8(smbHdl->baseAddr, 6, 0x2a); /* BLK data */
    MWRITE_D8(smbHdl->baseAddr, 6, 0x51); /* BLK data */
    MWRITE_D8(smbHdl->baseAddr, 6, 0xd0); /* BLK data */
    MWRITE_D8(smbHdl->baseAddr, 6, 0x46); /* BLK data */
    MWRITE_D8(smbHdl->baseAddr, 6, 0x38); /* BLK data */
    MWRITE_D8(smbHdl->baseAddr, 7, 0x77); /*host cmd 77*/
    MWRITE_D8(smbHdl->baseAddr, 2, 0x12); /*START*/

#if 0
	data = *buf++;
	currSize--;
	DBGWRT_3((smbHdl->dbgHdl, "currSize=%d buf %02x", currSize, data) );
	writeCmdForHost( smbHdl->baseAddr, data );  /* first byte to send */


	writeData( smbHdl->baseAddr, 0, (u_int8)(currSize) );   /* nbr of bytes to send */

	for( i=0;i<32;i++ )
		*((u_int8*)smbHdl->baseAddr+SMB_HOST_BLK) = i+1;

	currSize--;
	while( currSize-- )
	{
		data = *buf++;
		DBGWRT_3((smbHdl->dbgHdl, " %02x", data) );
		*((u_int8*)smbHdl->baseAddr+SMB_HOST_BLK) = data;
	}/*while*/
#endif
	DBGWRT_3((smbHdl->dbgHdl, "\n") );

    writeStart(smbHdl->baseAddr);

	if( (error = waitBusyReady(smbHdl)))
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: waitBusyReady() error 0x%x %s%d%s",
					  errorStartStr, functionName,
					  error,
					  errorLineStr, __LINE__, errorEndStr ));
    	return( error );
    }/*if*/

    return( 0 );
}/*smbWriteBlock*/

#endif   /* ALI - write read block does not work like HW description */

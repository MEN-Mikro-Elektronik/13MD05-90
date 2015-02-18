/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysmanagbus_mpc8240.c
 *      Project: MENMON F001
 *
 *       Author: uf
 *        $Date: 2000/12/01 12:03:10 $
 *    $Revision: 2.0 $
 *
 *  Description: system managment bus/I2C driver for the
 *				 MPC8240 I2C controller.
 *				 Single master is implemented only.
 *
 *
 *     Required: -
 *     Switches: -
 *         Note: Error recovery is not perfect.
 *               If the system restart within a EEPROM access, it
 *               may happens, that the EEPROM hold SDA line low and
 *               the IIC bus blocks still the next power up.
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysmanagbus_mpc8240.c,v $
 * Revision 2.0  2000/12/01 12:03:10  kp
 * Many changes to get it running.
 * Tested due to problems with B11 RTC
 *
 * Revision 1.2  2000/08/03 19:28:06  Franke
 * added WriteReadTwoByte is a NULL pointer
 *
 * Revision 1.1  2000/08/01 09:47:23  Franke
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/maccess.h>
#include <MEN/dbg.h>

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
	u_int32       	 ownSize;
	OSS_HANDLE 	  	 *osHdl;
	u_int32			 dbgLevel;
	DBG_HANDLE		 *dbgHdl;
	SMB_DESC_MPC8240 desc;
}SMB_HANDLE;


/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define SMB_READ           0x01
#define SMB_WRITE          0x00

#define OWN_SLAVE_ADDR		0x00
#define DONT_USE_IRQ		0


/* register addresses in big endian mode for long access */
#define I2CREG_OWN_ADDR								0x00
#define I2CREG_FREQUENCE_DIVIDER_DIGITAL_FILTERFREQ	0x04
#define I2CREG_CONTROL								0x08
#define I2CREG_STATUS								0x0C
#define I2CREG_DATA									0x10


#define I2CREG_CONTROL_MEN			0x80000000
#define I2CREG_CONTROL_MEIN			0x40000000
#define I2CREG_CONTROL_MSTA			0x20000000
#define I2CREG_CONTROL_MTX			0x10000000
#define I2CREG_CONTROL_TXAK			0x08000000
#define I2CREG_CONTROL_RSTA			0x04000000	/* W */
#define I2CREG_CONTROL_RESV_1		0x02000000
#define I2CREG_CONTROL_RESV_0		0x01000000

#define I2CREG_STATUS_MCF			0x80000000
#define I2CREG_STATUS_MAAS			0x40000000
#define I2CREG_STATUS_MBB			0x20000000
#define I2CREG_STATUS_MAL			0x10000000	/* R/W */
#define I2CREG_STATUS_RESV			0x08000000
#define I2CREG_STATUS_SRW			0x04000000
#define I2CREG_STATUS_MIF			0x02000000	/* R/W */
#define I2CREG_STATUS_RXAK			0x01000000

#define	DBG_MYLEVEL		smbHdl->dbgLevel


#ifdef _UCC
/* Ultra-C has no inline funcs */
# define __inline__
#endif

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
static u_int32 smbExit(         SMB_HANDLE  **smbHdlP );
static u_int32 smbWriteByte(    SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 data );
static u_int32 smbReadByte(     SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 *dataP );
static u_int32 smbWriteTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 data );
static u_int32 smbWriteReadBlock( SMB_HANDLE  *smbHdl, u_int8 addr, u_int32 size,	u_int8 *buf, u_int8 cmdOrIndex );
static u_int32 smbWriteBlock	( SMB_HANDLE  *smbHdl, u_int8 addr, u_int32 size,	u_int8 *buf );


/*******************************  smbIdent  *********************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char* smbIdent( void )	/* nodoc */
{
    return( "SMB - SMB library: $Id: sysmanagbus_mpc8240.c,v 2.0 2000/12/01 12:03:10 kp Exp $" );
}/*smbIdent*/

/************************************* delay ********************************
 *  Description:  Delay	descriptor dependend time.
 *
 *		   Note:  pollTimeUs descriptor value
 *                 < 100 OSS_MikroDelay
 *                 else  OSS_Delay(pollTimeUs/1000)
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
__inline__ static void delay
(
	SMB_HANDLE  *smbHdl
)
{
DBGCMD(	static const char functionName[] = "SMB - delay:"; )

	DBGWRT_3((smbHdl->dbgHdl, "%s\n", functionName) );

	if( smbHdl->desc.pollTimeUs > 100 )
		OSS_Delay( smbHdl->osHdl, (smbHdl->desc.pollTimeUs/1000) );
	else
		OSS_MikroDelay( smbHdl->osHdl, smbHdl->desc.pollTimeUs );
}/*delay*/

/******************************* smbErrorRecover *****************************
 *  Description:  Try to recover from error.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
void smbErrorRecover
(
	SMB_HANDLE  *smbHdl
)
{
DBGCMD(	static const char functionName[] = "SMB - smbErrorRecover:"; )
int i;

	DBGWRT_1((smbHdl->dbgHdl, "%s\n", functionName) );

	/* reset the I2C controller */
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, 0x00 );
	delay( smbHdl->osHdl );
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MEN );
	delay( smbHdl->osHdl );
	
	/* try to free the bus by clocking */
	for( i=0; i<20; i++ )
	{

		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
		MSETMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MSTA | I2CREG_CONTROL_RSTA );
		delay( smbHdl->osHdl );
	}/*for*/
	
	/* reset the I2C controller */
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, 0x00 );
	delay( smbHdl->osHdl );
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MEN );
	delay( smbHdl->osHdl );
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
}/*smbErrorRecover*/


/******************************* smbPrintStat ****************************
 *  Description:  Status register debugs.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	pointer to smb handle
 *                stat      contains status register contents
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
static void smbPrintStat
(
	SMB_HANDLE  *smbHdl,
	u_int32 stat
)
{
	DBGWRT_2((smbHdl->dbgHdl,"  STAT %08x\n", stat ));
	DBGWRT_3((smbHdl->dbgHdl,"  MCF  MAAS MBB  MAL  ---- SRW  MIF  RXAK\n "));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_MCF	 ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_MAAS  ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_MBB	 ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_MAL	 ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_RESV  ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_SRW	 ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_MIF	 ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl," %s", stat & I2CREG_STATUS_RXAK  ?" x  " : "    " ));
	DBGWRT_3((smbHdl->dbgHdl,"\n" ));
}/*smbPrintStat*/


/******************************* waitForEvent ****************************
 *  Description:  Wait for event.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl	  pointer to smb handle
 *				  eventMask   event mask e.g. MIF bit
 *				  errorMask   error mask e.g. MAL bit
 *				  currStatP   pointer to variable where status will be stored
 *  Output.....:  *currStatP  current status
 *  Globals....:  -
 ****************************************************************************/
static unsigned char waitForEvent
(
	SMB_HANDLE  *smbHdl,
	unsigned long eventMask,
	unsigned long errorMask,
	unsigned long *currStatP
)
{
int i;
unsigned long oldStat = 0xFF;
DBGCMD(	static const char functionName[] = "SMB - waitForEvent:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s\n", functionName) );

	for( i=0;i<2000;i++)
	{
		*currStatP = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_STATUS);
		if( oldStat != *currStatP )
		{
			oldStat = *currStatP;
			/*DBGWRT_ERR((smbHdl->dbgHdl,"stat=%x\n", *currStatP));*/
			smbPrintStat( smbHdl, oldStat );
		}/*if*/

		if( *currStatP & I2CREG_STATUS_MIF ){
			/*--- clear MIF ---*/
			MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, *currStatP & ~I2CREG_STATUS_MIF);
			/**currStatP = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_STATUS) | I2CREG_STATUS_MIF;  ??? */

			/* ERROR */
			if( *currStatP & errorMask )
			{
				DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: hit errorMask 0x%x %s%d%s",
							  errorStartStr, functionName, *currStatP,
							  errorLineStr, __LINE__, errorEndStr ));
				
				/*--- reset smb controller ---*/
				/* disable the I2C module  */
				MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, 0 );
				
				OSS_Delay( smbHdl->osHdl, 10 );
				/* enable the I2C module */
				MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL,
							I2CREG_CONTROL_MEN );

				/* clear status register */
				MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS,
							  I2CREG_STATUS_MAL );
				return( 1 );
			}/*if*/

			/* OK */
			if( *currStatP & eventMask )
			{
				DBGWRT_2((smbHdl->dbgHdl, "  hit eventMask \n") );
				return( 0 );
			}/*if*/
		}
		delay( smbHdl );
	}/*for*/

	/* timeout */
	DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TIMEOUT %s%d%s",
				  errorStartStr, functionName,
				  errorLineStr, __LINE__, errorEndStr ));
	return( 1 );
}/*waitForEvent*/


/************************************* start ********************************
 *  Description:  Generate START condition and send slave address.
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl		pointer to smb handle
 *				  addr			slave address
 *				  readMode		SMB_READ | SMB_WRITE
 *  Output.....:  return  0 - OK or 1 - ERROR
 *  Globals....:  -
 ****************************************************************************/
static int start
(
	SMB_HANDLE  *smbHdl,
    unsigned char addr, /* address of the receiver */
    int			readMode
)
{
	u_int32 status;
	u_int32 ctrl;
	u_int32 data;
	int32 timeout;
	DBGCMD(	static const char functionName[] = "SMB - start:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s\n", functionName) );

	/*--- wait for bus idle ---*/
	for( timeout=100000; timeout>0; timeout-=smbHdl->desc.pollTimeUs ){

		status = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_STATUS);
	
		if( (status & I2CREG_STATUS_MBB) == 0)
			break;

		delay(smbHdl);
	}

	if( (status & I2CREG_STATUS_MBB) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
		return( SMB_ERR_BUSY );
	}

	ctrl   = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL);
    /* OK, we have the bus */
    /* prepare to write the slave address */
    ctrl |= I2CREG_CONTROL_MSTA | I2CREG_CONTROL_MTX ;
    ctrl &= ~( I2CREG_CONTROL_RSTA | I2CREG_CONTROL_TXAK);
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, ctrl);

    /* write the slave address and xmit/rcv mode bit */
    data = (addr | readMode ) << 24;
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_DATA, data );

	return( SMB_ERR_NO );	
}/*start*/

/****************************** SMB_MPC8240_Init *****************************
 *
 *  Description:  Initializes this library and init the SMB host.
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
u_int32 SMB_MPC8240_Init
(
    SMB_DESC_MPC8240	*descP,
    OSS_HANDLE 			*osHdl,
	void		 		**smbHdlP
)
{
u_int32     error  = 0;
SMB_HANDLE  *smbHdl = NULL;
u_int32		gotSize;
u_int32		value;

	*smbHdlP = NULL;

	/*---------------------+
	|  check descriptor	   |
	+---------------------*/
	if( descP->busClockDivider > 0x3f )
	{
		error = SMB_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if( descP->baseAddr == 0 )
	{
		error = SMB_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	OSS_MikroDelayInit( osHdl );
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

	DBGINIT((NULL,&smbHdl->dbgHdl));
	smbHdl->dbgLevel			= descP->dbgLevel;

	smbHdl->entries.Ident		= smbIdent;
	smbHdl->entries.Exit		= (int32 (*)(void **))smbExit;
	smbHdl->entries.WriteByte   = (int32 (*)(void *,u_int8,u_int8))smbWriteByte;
	smbHdl->entries.ReadByte    = (int32 (*)(void *,u_int8,u_int8*))smbReadByte;
	smbHdl->entries.WriteTwoByte= (int32 (*)(void *,u_int8,u_int8,u_int8))smbWriteTwoByte;
	smbHdl->entries.WriteReadTwoByte= (int32 (*)(void *,u_int8,u_int8,u_int8,u_int8))NULL;

	/* disable the I2C module - disable interrupt */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, (I2CREG_CONTROL_MEN|I2CREG_CONTROL_MEIN) );

	/* disable MULTIMASTER */
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_OWN_ADDR, 0x00 );

	/* set digital filter - default
	   set frequence divider - 0x3F => 32768 => 100MHz memory bus clk / divider = 3kHz */
	value = (descP->busClockDivider << 24) | 0x00100000;
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_FREQUENCE_DIVIDER_DIGITAL_FILTERFREQ, value );

	/* enable the I2C module */
	MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MEN );

	/* clear status register */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MAL );

	*smbHdlP = (void*) smbHdl;
	return( error );
	
CLEANUP:
	/* cleanup debug */
	DBGEXIT((&smbHdl->dbgHdl));

	if( smbHdl != NULL )
    	OSS_MemFree( smbHdl->osHdl, smbHdl, smbHdl->ownSize );
	return( error );
}/*SMB_MPC8240_Init*/

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

	/* disable the I2C module */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, (I2CREG_CONTROL_MEN|I2CREG_CONTROL_MEIN) );
	/* clear interrupts */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );

	/* cleanup debug */
	DBGEXIT((&smbHdl->dbgHdl));

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
u_int32 status;
u_int32 data;
u_int32 error = SMB_ERR_NO;
u_int32 ctrl;
DBGCMD(	static const char functionName[] = "SMB - smbReadByte:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x\n", functionName,addr) );
	*dataP = 0xFF;

	error = start( smbHdl, addr, SMB_READ);
	if( error )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: start() error %x %s%d%s",
					  errorStartStr, functionName,
					  error,
					  errorLineStr, __LINE__, errorEndStr ));
		return error;
	}
	
	if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_BUSY;
		goto CLEANUP;
	}/*if*/

	ctrl   = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL);

	if( status & I2CREG_STATUS_MCF )
	{
		if( status & I2CREG_STATUS_RXAK )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/

		ctrl &= ~I2CREG_CONTROL_MTX;
		ctrl |= I2CREG_CONTROL_TXAK;
		MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, ctrl );

		/* dummy read */
		data = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_DATA );

		if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_BUSY;
		}

		/* STOP */
		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MSTA );
		data = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_DATA );
		*dataP = (u_int8)(data>>24);
		DBGWRT_1((smbHdl->dbgHdl, "  read val %02x\n", *dataP) );
	}/*if*/

CLEANUP:
	/* STOP */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MSTA );

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
u_int32 status;
u_int32 error = SMB_ERR_NO;
u_int32 ctrl;
DBGCMD(	static const char functionName[] = "SMB - smbWriteByte:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x data %02x\n", functionName,addr,data) );

	/* START - send address */
	error = start( smbHdl, addr, SMB_WRITE );
	if( error )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: start() error %x %s%d%s",
					  errorStartStr, functionName,
					  error,
					  errorLineStr, __LINE__, errorEndStr ));
		return error;
	}

	if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_BUSY;
		goto CLEANUP;
	}/*if*/

	ctrl   = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL);
	/*MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );*/
	if( status & I2CREG_STATUS_MCF )
	{
		if( status & I2CREG_STATUS_RXAK )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/

		/* send data */
		DBGWRT_2((smbHdl->dbgHdl, "  write data byte\n", functionName ) );
		MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_DATA, data<<24 );

		if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_BUSY;
		}
		if( status & I2CREG_STATUS_RXAK )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/

		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
	}
	else
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TRANSFER not complete %s%d%s",
				  errorStartStr, functionName,
				  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_COLL;
		goto CLEANUP;
	}/*if*/

CLEANUP:
	/* STOP */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MSTA | I2CREG_CONTROL_MTX );

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
u_int32 status;
u_int32 error = SMB_ERR_NO;
DBGCMD(	static const char functionName[] = "SMB - smbWriteTwoByte:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x data %02x data %02x\n", functionName,addr,cmdAddr,data) );

	/* START - send address */
	error = start( smbHdl, addr, SMB_WRITE );
	if( error )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: start() error %x %s%d%s",
					  errorStartStr, functionName,
					  error,
					  errorLineStr, __LINE__, errorEndStr ));
		return error;
	}

	if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_BUSY;
		goto CLEANUP;
	}/*if*/

	if( status & I2CREG_STATUS_MCF )
	{
		if( status & I2CREG_STATUS_RXAK )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/

		/* send data byte 1 */
		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MAL );
		DBGWRT_2((smbHdl->dbgHdl, "  write data byte #1\n" ) );
		MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_DATA, cmdAddr<<24 );
		if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_BUSY;
		}
		if( status & I2CREG_STATUS_RXAK )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/
		/*MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );*/

		/* send data byte 2 */
		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MAL );
		DBGWRT_2((smbHdl->dbgHdl, "  write data byte #2\n" ) );
		MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_DATA, data<<24 );
		if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_BUSY;
		}
		if( status & I2CREG_STATUS_RXAK )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_NO_DEVICE;
			goto CLEANUP;
		}/*if*/
		/*MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );*/
	}
	else
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TRANSFER not complete %s%d%s",
				  errorStartStr, functionName,
				  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_COLL;
		goto CLEANUP;
	}/*if*/

CLEANUP:
	/* STOP */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MSTA | I2CREG_CONTROL_MTX );

	return( error );
}/*smbWriteTwoByte*/

#if 0 /* block functions not full implemented */
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
u_int32 ctrl;
u_int32 status;
u_int32 error = SMB_ERR_NO;
int     retry;
u_int32 i;
u_int32 data;
DBGCMD(	static const char functionName[] = "SMB - smbReadBlock:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x\n", functionName,addr) );

			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TODO - send byte to target !!! - uf 03.08.2000 %s%d%s",
					  errorStartStr, functionName,
					  error, retry,
					  errorLineStr, __LINE__, errorEndStr ));
	error = SMB_ERR_PARAM; /* TODO - send byte to target !!! - uf 03.08.2000 */


	/* START - send address */
	for( retry=0; retry<4; retry++ )
	{
		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
		error = start( smbHdl, addr, SMB_READ, retry );
		if( error )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: start() error %x retry %d %s%d%s",
					  errorStartStr, functionName,
					  error, retry,
					  errorLineStr, __LINE__, errorEndStr ));
		}
		else
		{
			DBGWRT_2((smbHdl->dbgHdl, "%s start() error %x retry %d\n", functionName, error, retry ) );
			break;
		}/*if*/
	}/*for*/
	if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_BUSY;
		goto CLEANUP;
	}/*if*/
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
	if( status & I2CREG_STATUS_RXAK )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
				  errorStartStr, functionName,
				  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_NO_DEVICE;
		goto CLEANUP;
	}/*if*/
	if( !(status & I2CREG_STATUS_MCF) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TRANSFER not complete %s%d%s",
				  errorStartStr, functionName,
				  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_COLL;
		goto CLEANUP;
	}/*if*/


	ctrl   = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL);
	/* byte 0..(size-1) */
	for( i=0; i<size; i++ )
	{
		if( i==0 )
		{
			ctrl &= ~I2CREG_CONTROL_MTX; /* master receive */
			MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, ctrl );

			/* dummy read */
			data = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_DATA );
		}/*if*/

		/* read data */
		if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_BUSY;
		}
		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
		if( !(status & I2CREG_STATUS_MCF) )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TRANSFER not complete %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_COLL;
			goto CLEANUP;
		}/*if*/

		if( i==(size-1) )
		{
			ctrl |= I2CREG_CONTROL_TXAK; /* no ackn at last received byte */
/*			ctrl &= ~I2CREG_CONTROL_MSTA; /* STOP */
			MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, ctrl );
		}/*if*/

		data = MREAD_D32( smbHdl->desc.baseAddr, I2CREG_DATA );
		*buf = (u_int8)(data>>24);
		DBGWRT_1((smbHdl->dbgHdl, "  read val#%d %02x\n", i, *buf) );
		buf++;
	}/*for*/

	if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
				  errorStartStr, functionName,
				  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_BUSY;
	}

CLEANUP:
	/* STOP */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MSTA );

	return( error );
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
static u_int32 smbWriteBlock
(
	SMB_HANDLE	*smbHdl,
	u_int8 		addr,
	u_int32		size,
	u_int8		*buf
)
{
u_int32 status;
u_int32 error = SMB_ERR_NO;
int     retry;
u_int32 i;
u_int32 data;
DBGCMD(	static const char functionName[] = "SMB - smbWriteBlock:"; )

	DBGWRT_1((smbHdl->dbgHdl, "%s addr %02x\n", functionName,addr) );

	/* START - send address */
	for( retry=0; retry<4; retry++ )
	{
		MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
		error = start( smbHdl, addr, SMB_WRITE, retry );
		if( error )
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: start() error %x retry %d %s%d%s",
					  errorStartStr, functionName,
					  error, retry,
					  errorLineStr, __LINE__, errorEndStr ));
		}
		else
		{
			DBGWRT_2((smbHdl->dbgHdl, "%s start() error %x retry %d\n", functionName, error, retry ) );
			break;
		}/*if*/
	}/*for*/
	if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
	{
		DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
		error = SMB_ERR_BUSY;
		goto CLEANUP;
	}/*if*/
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );

	for( i=1; i<=size; i++ )
	{
		if( status & I2CREG_STATUS_MCF )
		{
			if( status & I2CREG_STATUS_RXAK )
			{
				DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
						  errorStartStr, functionName,
						  errorLineStr, __LINE__, errorEndStr ));
				error = SMB_ERR_NO_DEVICE;
				goto CLEANUP;
			}/*if*/

			/* send data */
			DBGWRT_2((smbHdl->dbgHdl, "  write data byte #%d\n", i ) );
			data = (*buf++) << 24;
			MWRITE_D32( smbHdl->desc.baseAddr, I2CREG_DATA, data );
			if( waitForEvent( smbHdl, I2CREG_STATUS_MIF, I2CREG_STATUS_MAL, &status ) )
			{
				DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_BUSY %s%d%s",
						  errorStartStr, functionName,
						  errorLineStr, __LINE__, errorEndStr ));
				error = SMB_ERR_BUSY;
			}
			if( status & I2CREG_STATUS_RXAK )
			{
				DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: SMB_ERR_NO_DEVICE %s%d%s",
						  errorStartStr, functionName,
						  errorLineStr, __LINE__, errorEndStr ));
				error = SMB_ERR_NO_DEVICE;
				goto CLEANUP;
			}/*if*/
			MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
		}
		else
		{
			DBGWRT_ERR( ( smbHdl->dbgHdl, "%s%s: TRANSFER not complete %s%d%s",
					  errorStartStr, functionName,
					  errorLineStr, __LINE__, errorEndStr ));
			error = SMB_ERR_COLL;
			goto CLEANUP;
		}/*if*/
	}/*for*/

CLEANUP:
	/* STOP */
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_STATUS, I2CREG_STATUS_MIF | I2CREG_STATUS_MAL );
	MCLRMASK_D32( smbHdl->desc.baseAddr, I2CREG_CONTROL, I2CREG_CONTROL_MSTA | I2CREG_CONTROL_MTX );

	return( error );
}/*smbWriteBlock*/

#endif /* block functions not full implemented */


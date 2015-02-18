/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysmanagbus_ich.c
 *      Project: SC14
 *
 *       Author: sv
 *        $Date: 2005/10/12 09:33:47 $
 *    $Revision: 1.2 $
 *
 *  Description: system managment bus driver for the ICH6
 *				 SMB controller
 *
 *     Required: -
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * nativ stuff replaced due to compatibility
 * $Log: sysmanagbus_ich.c,v $
 * Revision 1.2  2005/10/12 09:33:47  dpfeuffer
 * nativ stuff replaced due to compatibility
 *
 * Revision 1.1  2005/10/06 08:43:06  SVogel
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN mikro elektronik GmbH, Nuernberg, Germany
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
/* SMBus I/O Register Address Map */
#define SMB_HST_STS                 0x00 /* host status */
#define SMB_HST_CNT                 0x02 /* host control */
#define SMB_HST_CMD                 0x03 /* host command */
#define SMB_XMIT_SLVA               0x04 /* transmitt slave address*/
#define SMB_HST_D0                  0x05 /* host data 0*/
#define SMB_HST_D1                  0x06 /* host data 1*/
#define SMB_HOST_BLOCK_DB           0x07 /* host block data byte */
#define SMB_PEC                     0x08 /* packet error check */
#define SMB_RCV_SLVA                0x09 /* receive slave address */
#define SMB_SLV_DATA_0              0x0A /* receive slave data 0 */
#define SMB_SLV_DATA_1              0x0B /* receive slave data 1 */
#define SMB_AUX_STS                 0x0C /* auxiliary status */
#define SMB_AUX_CTL                 0x0D /* auxiliary control */
#define SMB_SMLINK_PIN_CTL          0x0E /* SMLink pin control */
#define SMB_SMBUS_PIN_CTL           0x0F /* SMBus pin control */
#define SMB_SLV_STS                 0x10 /* slave status */
#define SMB_SLV_CMD                 0x11 /* slave command */
#define SMB_NOTIFY_DADDR            0x14 /* notify device address */
#define SMB_NOTIFY_DLOW             0x16 /* notify data low byte */
#define SMB_NOTIFY_DHIGH            0x17 /* notify data high byte */

/* register SMB_HST_STS (default: 0x00) */
#define HOST_BUSY                  0x01
#define INTR                       0x02
#define DEV_ERR                    0x04
#define BUS_ERR                    0x08
#define FAILED                     0x10
#define SMBALERT_STS               0x20
#define INUSE_STS                  0x40
#define BYTE_DONE_STATUS           0x80

#define SMB_CLEAR_STATUS           (INTR+DEV_ERR+BUS_ERR+FAILED)

/* register SMB_HST_CNT        (default: 0x00) */
#define INTREN                     0x01
#define KILL                       0x02
#define QUICK                      (0x00 << 2)
#define BYTE                       (0x01 << 2)
#define BYTE_DATA                  (0x02 << 2)
#define WORD_DATA                  (0x03 << 2)
#define PROCESS_CALL               (0x04 << 2)
#define BLOCK                      (0x05 << 2)
#define I2C_READ                   (0x06 << 2)
#define BLOCK_PROCESS              (0x07 << 2)
#define LAST_BYTE                  0x30
#define START                      0x40
#define PEC_EN                     0x80
/* register SMB_HST_CMD        (default: 0x00) */
/* host status command register bit 0..7 */

/* register SMB_XMIT_SLVA      (default: 0x00) */
/* transmitt slave register bit 7..1 address, bit 0 0=write, 1=read */

/* register SMB_HST_D0         (default: 0x00) */
/* host data0 register for block write commands */

/* register SMB_HST_D1         (default: 0x00) */
/* host data1 register for, used during execution of any command */

/* register SMB_HOST_BLOCK_DB  (default: 0x00) */
/* host block data byte register */

/* register SMB_PEC            (default: 0x00) */
/* packet error check register */

/* register SMB_RCV_SLVA       (default: 0x44) */
/* receive slave address register bit 6..0 = slave address */

/* register SMB_SLV_DATA_0     (default: 0x00) */
/* receive slave data0 register */

/* register SMB_SLV_DATA_1     (default: 0x00) */
/* receive slave data1 register */

/* register SMB_AUX_STS        (default: 0x00) */
#define CRC_ERROR                   0x01
#define SMBUS_TCO_MODE              0x02

/* register SMB_AUX_CTL        (default: 0x00) */
#define AUTO_APPEND_CRC             0x01
#define EN_32BYTE_BUFFER            0x02

/* register SMB_SMLINK_PIN_CTL (default: 0x??) */
#define SMLINK0_CUR_STS             0x01
#define SMLINK1_CUR_STS             0x02
#define SMLINK_CLK_CTL              0x04

/* register SMB_SMBUS_PIN_CTL  (default: 0x??) */
#define SMBCLK_CUR_STS              0x01
#define SMBDATA_CUR_STS             0x02
#define SMBCLK_CTL                  0x04

/* register SMB_SLV_STS        (default: 0x00) */
#define HOST_NOTIFY_STS             0x01

/* register SMB_SLV_CMD        (default: 0x00) */
#define HOST_NOTIFY_INTREN          0x01
#define HOST_NOTIFY_WKEN            0x02
#define SMBALERT_DIS                0x04

/* register SMB_NOTIFY_DADDR   (default: 0x00) */
/* notify device address register bit 7..1 device address */

/* register SMB_NOTIFY_DLOW    (default: 0x) */
/* notify data low byte register */

/* register SMB_NOTIFY_DHIGH   (default: 0x) */
/* notify data high byte register */
#define WAIT_BUSY_TIME              8000
#define WRITE_ENABLE                0x00
#define READ_ENABLE                 0x01

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

static u_int32 LocWaitBusyReady( void * baseAddr);

static u_int32 LocSmbExit( SMB_HANDLE  **smbHdlP );
static u_int32 LocSmbWriteByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 data );
static u_int32 LocSmbReadByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 *dataP );
static u_int32 LocSmbWriteTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 data );
static u_int32 LocSmbWriteReadTwoByte( SMB_HANDLE  *smbHdl, u_int8 addr, u_int8 cmdAddr, u_int8 *data1P, u_int8 *data2P );

static char* LocSmbIdent( void )
{
    return( "ICH SMB - SMB library: $Id: sysmanagbus_ich.c,v 1.2 2005/10/12 09:33:47 dpfeuffer Exp $" );
}/*LocSmbIdent*/

/*******************************  LocWaitBusyReady  *************************
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
static u_int32 LocWaitBusyReady( void * baseAddr )
{
    u_int32 i=0;
    u_int8 status=0;

    for( i=0; i<WAIT_BUSY_TIME; i++ )
	{
		status = MREAD_D8(baseAddr, SMB_HST_STS);
		
		if( status & DEV_ERR )
		{
			return( SMB_ERR_ADDR );
	    }
		
		if( status & BUS_ERR )
		{
			return( SMB_ERR_COLL );
	    }
		
		if( status & HOST_BUSY )
		{
			break;
	    }
	
	    MWRITE_D8(baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	}
	
    for( i=0; i<WAIT_BUSY_TIME; i++ )
	{
		status = MREAD_D8(baseAddr, SMB_HST_STS);
		
		if( status & DEV_ERR )
		{
			return( SMB_ERR_ADDR );
		}
		
		if( status & BUS_ERR )
		{
			return( SMB_ERR_COLL );
		}
		
		if( !(status & HOST_BUSY) )
		{		
	    	for( i=0; i<WAIT_BUSY_TIME; i++ )
			{			
				status = MREAD_D8(baseAddr, SMB_HST_STS);
				
				if( status & BYTE_DONE_STATUS )
				{
				    break;
				}
				
				MWRITE_D8(baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);
			}
			return( SMB_ERR_NO );
		}		
	}
	return( SMB_ERR_BUSY );	
}/*LocWaitBusyReady*/

/*******************************  LocSmbExit  *******************************
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
static u_int32 LocSmbExit
(
	SMB_HANDLE **smbHdlP
)
{
    u_int32 error  = 0;
    SMB_HANDLE  *smbHdl;

	smbHdl = *smbHdlP;
	*smbHdlP = NULL;

	/* clear status first */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	

    OSS_MemFree( smbHdl->osHdl, smbHdl, smbHdl->ownSize );

	return( error );
}/*LocSmbExit*/

/******************************** LocSmbReadByte ****************************
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
static u_int32 LocSmbReadByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      *dataP
)
{
    u_int8  status = 0;
    u_int32 error = 0;

    if(	addr & READ_ENABLE )
    {
    	return( SMB_ERR_ADDR );
    }

	/* set to default */
	*dataP = 0xFF;	

	/* clear and check status first */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);
	status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);

	if( !(status & BYTE_DONE_STATUS) ||
    	 (status & (DEV_ERR | BUS_ERR | HOST_BUSY)) )
	{
		MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	    status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);	    	
	}		

    MWRITE_D8(smbHdl->baseAddr,SMB_XMIT_SLVA, (addr | READ_ENABLE));
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CNT, (BYTE+START));

	if( (error = LocWaitBusyReady(smbHdl->baseAddr)) )
	{
	    return(error);
	}
	
    *dataP = MREAD_D8(smbHdl->baseAddr, SMB_HST_D0);

    return( 0 );
}/*LocSmbReadByte*/

/******************************** LocSmbWriteByte ***************************
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
static u_int32 LocSmbWriteByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      data
)
{
    u_int8  status = 0;
    u_int32 error = 0;

    if(	addr & READ_ENABLE )
    {
    	return( SMB_ERR_ADDR );
    }

	/* clear and check status first */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);

	if( !(status & BYTE_DONE_STATUS) ||
    	 (status & (DEV_ERR | BUS_ERR | HOST_BUSY)) )
	{
		MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	    status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);	
	}	

    MWRITE_D8(smbHdl->baseAddr,SMB_XMIT_SLVA, addr | WRITE_ENABLE);
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, data);
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CNT, (BYTE+START));


	if( (error = LocWaitBusyReady(smbHdl->baseAddr)) )
	{	
	    return( error );
	}

    return( 0 );
}/*LocSmbWriteByte*/

/******************************** LocSmbWriteTwoByte ************************
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
static u_int32 LocSmbWriteTwoByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      cmdAddr,
	u_int8      data
)
{
    u_int8  status = 0;
    u_int32 error = 0;

    if(	addr & READ_ENABLE )
    {
    	return( SMB_ERR_ADDR );
    }

	/* clear and check status first */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);

	if( !(status & BYTE_DONE_STATUS) ||
    	 (status & (DEV_ERR | BUS_ERR | HOST_BUSY)) )
	{
		MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	    status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);	
	}	
	
    MWRITE_D8(smbHdl->baseAddr, SMB_XMIT_SLVA, addr | WRITE_ENABLE);
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_D0, data);
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CNT, (BYTE_DATA+START));

	if( (error = LocWaitBusyReady(smbHdl->baseAddr)) )
	{	
	    return( error );
	}

    return( 0 );
}/*LocSmbWriteTwoByte*/

/*************************** LocSmbWriteReadTwoByte *************************
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
static u_int32 LocSmbWriteReadTwoByte
(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      cmdAddr,
	u_int8      *data1P,
	u_int8      *data2P
)
{
    u_int8  status = 0;
    u_int32 error = 0;

    if(	addr & READ_ENABLE )
    {
    	return( SMB_ERR_ADDR );
    }

	/* clear and check status first */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);

	if( !(status & BYTE_DONE_STATUS) ||
    	 (status & (DEV_ERR | BUS_ERR | HOST_BUSY)) )
	{
		MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
	    status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);	
	}

    MWRITE_D8(smbHdl->baseAddr,SMB_XMIT_SLVA, addr | READ_ENABLE);
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);
    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CNT, (WORD_DATA+START));

	if( (error = LocWaitBusyReady(smbHdl->baseAddr)) )
	{   	
	    return( error );
	}

    *data1P = MREAD_D8(smbHdl->baseAddr, SMB_HST_D0);
    *data2P = MREAD_D8(smbHdl->baseAddr, SMB_HST_D1);

    return( 0 );
}/*LocSmbWriteReadTwoByte*/

/****************************** SMB_ICH_Init *****************************
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
 *************************************************************************/
u_int32 SMB_ICH_Init(
    SMB_DESC_ICH    	*descP,
    OSS_HANDLE			*osHdl,
	void				**smbHdlP
)
{
    SMB_HANDLE  *smbHdl;
    u_int32     error  = 0;
    u_int32		gotSize = 0;

	*smbHdlP = NULL;
	smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
	
	if( smbHdl == NULL )
	{
	    error = SMB_ERR_NO_MEM;
		return error;
	}/*if*/

	/* init the structure */
	OSS_MemFill( osHdl, gotSize, (char*)smbHdl, 0 );

	smbHdl->dbgLevel			= 0xFFFFFFFF;

	smbHdl->baseAddr 			= descP->baseAddr;
	smbHdl->ownSize  			= gotSize;
	smbHdl->osHdl    			= (OSS_HANDLE*) osHdl;

    smbHdl->entries.Ident		= LocSmbIdent;
	smbHdl->entries.Exit		= (int32 (*)(void**))LocSmbExit;
	smbHdl->entries.WriteByte	= (int32 (*)(void*,u_int8,u_int8))
		                                LocSmbWriteByte;
	smbHdl->entries.ReadByte	= (int32 (*)(void*,u_int8, u_int8*))
									    LocSmbReadByte;
	smbHdl->entries.WriteTwoByte= (int32 (*)(void*,u_int8,u_int8,u_int8))
		                                LocSmbWriteTwoByte;		
	smbHdl->entries.WriteReadTwoByte= (int32 (*)(void *,u_int8,u_int8,u_int8 *,u_int8 *))
	                                    LocSmbWriteReadTwoByte;

	/* clear status first */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);		

	*smbHdlP = (void*) smbHdl;

    return 0;	
}/*SMB_Ich_Init*/

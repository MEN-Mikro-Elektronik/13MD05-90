/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: sysmanagbus_16z001.c
 *      Project: MENMON 16Z001
 *
 *       Author: kp/rla
 *        $Date: 2009/01/21 17:58:16 $
 *    $Revision: 1.6 $
 *
 *  Description: system managment bus driver for the 16Z001 I2C controller
 *
 * This code exports one init functions:
 * - SMB_16Z001_Init	16Z001 Init routine
 *
 *     Required: -
 *     Switches: SMB_FIXED_HANDLE - don't allocate handle dynamically
 *									instead, use handle from caller
 *
 *				 MENMON - for MENMON support
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: sysmanagbus_16z001.c,v $
 * Revision 1.6  2009/01/21 17:58:16  AWanka
 * R: SMB Address is used in internal format (7 Bits) and with read/write
 *    Bit (8 Bits)
 * M: vxWorks uses only the 8 Bit format.
 *
 * Revision 1.5  2008/08/12 14:56:23  aw
 * R: SMB Address is used in internal format (7 Bits) and with read/write
 *    Bit (8 Bits)
 * M: OS9 uses now only the 8 Bit format.
 *
 * Revision 1.4  2007/12/04 17:12:13  rt
 * fixed:
 * -error in deinit (delete pending status bits)
 *
 * Revision 1.3  2007/09/19 16:01:22  rt
 * fixed:
 * -return error if smbRead/WriteByte failed
 *
 * Revision 1.2  2006/08/18 08:18:22  rla
 * Cosmetics: Delete unused variables
 *
 * Revision 1.1  2006/07/11 16:11:35  rla
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2006 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/maccess.h>

#define SMB_COMPILE
#include <MEN/sysmanagbus.h>
#include <MEN/16z001_smb.h>

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#ifdef MENMON
#define	DBG_MYLEVEL		MMBIOS_dbgLevel
#else
#define	DBG_MYLEVEL		smbHdl->dbgLevel
#endif

#define DBH				smbHdl->dbgHdl

#if (defined(MENMON) || defined(OS9000) || defined(VXWORKS))
#define ADDSHIFT
#else
#define ADDSHIFT(addr) ((addr & 0x7f) << 1)
#endif

#define MAX_TIMEOUT 		300		/* times 1/100 sec for sleep*/

/*
 * Data for SMBus Messages
 */
#define I2C_SMBUS_BLOCK_MAX	32		/* As specified in SMBus standard */
#define I2C_SMBUS_I2C_BLOCK_MAX	32	/* Not specified but we use
									 * same structure */


/* smbus_access read or write markers */
#define I2C_SMBUS_READ		1
#define I2C_SMBUS_WRITE		0

/* SMBus transaction types (size parameter in the above functions)
   Note: these no longer correspond to the (arbitrary) PIIX4 internal codes! */
#define I2C_SMBUS_QUICK		    		0
#define I2C_SMBUS_BYTE		    		1
#define I2C_SMBUS_BYTE_DATA	    		2
#define I2C_SMBUS_WORD_DATA	    		3
#define I2C_SMBUS_PROC_CALL	    		4
#define I2C_SMBUS_BLOCK_DATA	    	5
#define I2C_SMBUS_I2C_BLOCK_DATA    	6
#define I2C_SMBUS_BLOCK_PROC_CALL   	7		/* SMBus 2.0 */
#define I2C_SMBUS_BLOCK_DATA_PEC    	8		/* SMBus 2.0 */
#define I2C_SMBUS_PROC_CALL_PEC     	9		/* SMBus 2.0 */
#define I2C_SMBUS_BLOCK_PROC_CALL_PEC  	10		/* SMBus 2.0 */
#define I2C_SMBUS_WORD_DATA_PEC	   		11		/* SMBus 2.0 */

#define I2C_ERROR	0
#define I2C_OK		(!I2C_ERROR)

#define u_int8 unsigned char
#define uint8 u_int8

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
	u_int32    sclFreq;
	u_int32	   timeOut;		/**< wait time if SMBus is busy */
	u_int32	   mikroDelay;	/**< default 0-OSS_Delay, 1-OSS_MikroDelay */
}SMB_HANDLE;

union i2c_smbus_data {
	u_int8 byte;
	u_int16 word;
	u_int8 block[I2C_SMBUS_BLOCK_MAX + 3]; /* block[0] is used for length */
                          /* one more for read length in block process call */
	                                            /* and one more for PEC */
};

#ifdef MENMON
extern u_int32 MMBIOS_dbgLevel;
#endif


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

static char* smbIdent( void );
static u_int32 smbExit( SMB_HANDLE  **smbHdlP );
static u_int32 smbWriteByte( SMB_HANDLE  *smbHdl,
							 u_int8 addr,
							 u_int8 data );

static u_int32 smbReadByte( SMB_HANDLE  *smbHdl,
							u_int8 addr,
							u_int8 *dataP );

static u_int32 smbWriteTwoByte( SMB_HANDLE  *smbHdl,
								u_int8 addr,
								u_int8 cmdAddr,
								u_int8 data );

static int32 smbWriteReadTwoByte( void *hdl,
								  u_int8 addr,
								  u_int8 cmdAddr,
								  u_int8 *data1P,
								  u_int8 *data2P );


/* helper functions */
static void delay( SMB_HANDLE  *smbHdl )
{
	if( smbHdl->mikroDelay )
		OSS_MikroDelay( smbHdl->osHdl, 5 );
	else
		OSS_Delay( smbHdl->osHdl, 1 );

}/*delay*/

/*******************************  waitBusyReady  ***************************/
/** Waits to BUSY first and then leaving BUSY and bus idle.
 *  Returns imediatly if DEV_ERR detected.
 *
 *---------------------------------------------------------------------------
 *  \param   smbHdl     valid SMB handle
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static u_int32 waitBusyReady( SMB_HANDLE *smbHdl )
{
u_int8 status;
u_int32 n;

	DBGWRT_ERR((DBH,"*** Z001 waitBusyReady: timeout %d, mikroDelay %d\n",
					smbHdl->timeOut, smbHdl->mikroDelay));
	/* Mikrodelay: do 5µs delay at a time */
	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 200 : 1 );
	/* wait for transfer in progress */
    while(n--)
	{
		status = MREAD_D8(smbHdl->i2cBase, Z001_SMB_STS );
		if( status & Z001_SMB_STS_ERR )
			return( SMB_ERR_ADDR );
		if( status & Z001_SMB_STS_COLL )
			return( SMB_ERR_COLL );
		if( status & Z001_SMB_STS_BUSY )
			break;
		delay( smbHdl );
	}/**/
	if( n == 0 )
	{
		DBGWRT_ERR((DBH,"*** Z001 waitBusyReady: transfer did not begin\n"));
	return( SMB_ERR_BUSY );
	}

	n = smbHdl->timeOut * (smbHdl->mikroDelay ? 1000 : 1 );
	/* wait for transfer to finish */
    while(n--)
	{
		status = MREAD_D8( smbHdl->i2cBase, Z001_SMB_STS );
		if( status & Z001_SMB_STS_ERR )
			return( SMB_ERR_ADDR );
		if( status & Z001_SMB_STS_COLL )
			return( SMB_ERR_COLL );
		if( !(status & Z001_SMB_STS_BUSY) ) /* wait for busy bit to be cleared*/
		{
	    	while(n--)						/* wait for idle bit to be set */
			{
				status = MREAD_D8( smbHdl->i2cBase, Z001_SMB_STS );
				if( status & Z001_SMB_STS_IDLE )
				    break;
				delay( smbHdl );
			}/*while*/
			return( SMB_ERR_NO );
		}
		delay( smbHdl );
	}/*while*/
	DBGWRT_ERR((DBH,"*** Z001 waitBusyReady: transfer did not finish\n"));
	return( SMB_ERR_BUSY );
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

    do
    {
		/* clear status first */
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_STS, Z001_SMB_STS_CLEAR );
		waitBusyReady( smbHdl );
	   	MWRITE_D8( smbHdl->i2cBase, Z001_SMB_CMD, Z001_SMB_CMD_TOUT );
		waitBusyReady( smbHdl );
	} while(tries--);

	MWRITE_D8( smbHdl->i2cBase, Z001_SMB_STS, Z001_SMB_STS_CLEAR );
	OSS_Delay( smbHdl->osHdl, 200 );
}/*freeBus*/


/************************** smbAccess *********************************/
/** Low-level hardware access function
 *
 *  \param 	smbHdl		Smb Handle
 *  \param 	addr		I2C address - in internal format (>>1 )
 *  \param  read_write	Write: 0, Read: 1 last bit in address byte
 *  \param  command 	Single/block accesses,
 *  \param  size		Number of bytes to transfer
 *  \param  data		Where to store data or retrieve it from
 *
 *  \return         \c -
 */
int smbAccess(	SMB_HANDLE  *smbHdl,
			  	u_int16 	 addr,
			  	char 		 read_write,
			  	u_int8 		 command,
			  	int 		 size,
			  	union 		 i2c_smbus_data * data )
{

	int i,len;
	u_int32 error = 0;
	u_int8 status = 0;
	DBGCMD(	static const char functionName[] = "SMB - smbAccess:"; )

	/* clear all the bits (clear-on-write) */
	MWRITE_D8( smbHdl->i2cBase, Z001_SMB_STS, 0xff );

	/* clear and check status first */
	MWRITE_D8( smbHdl->i2cBase, Z001_SMB_STS, Z001_SMB_STS_CLEAR );
	status = MREAD_D8( smbHdl->i2cBase, Z001_SMB_STS );
	if( !(status & Z001_SMB_STS_IDLE) ||
    	( status & ( Z001_SMB_STS_COLL | Z001_SMB_STS_ERR | Z001_SMB_STS_BUSY ) )
	  )
	{
		if( !(status & Z001_SMB_STS_IDLE) )
	        freeBus( smbHdl );
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_STS, Z001_SMB_STS_CLEAR );
		status = MREAD_D8( smbHdl->i2cBase, Z001_SMB_STS );
		if( !(status & Z001_SMB_STS_IDLE) ||
    		( status & ( Z001_SMB_STS_COLL | Z001_SMB_STS_ERR | Z001_SMB_STS_BUSY ) )
		  )
		{
	    	return( SMB_ERR_BUSY );
		}/*if*/
	}/*if*/


	DBGWRT_1((DBH,"..smbAccess: r/w=%d command=0x%x addr=0x%02x size=%d\n",
			read_write, command, ADDSHIFT(addr), size ));

/* --- write address and commands --- */
	switch( size ) {
    case I2C_SMBUS_PROC_CALL:
		DBGWRT_1((DBH,"..I2C_SMBUS_PROC_CALL (unsupported)\n" ));
		break;

    case I2C_SMBUS_QUICK:
		DBGWRT_1((DBH, "..I2C_SMBUS_QUICK\n" ));
		MWRITE_D8(smbHdl->i2cBase, Z001_SMB_ADDR, ADDSHIFT(addr) | (read_write & 1));
		size = Z001_SMB_CMD_QUICK;
		break;

    case I2C_SMBUS_BYTE:
		DBGWRT_1((DBH, "..I2C_SMBUS_BYTE\n..addr:0x%02x byte1:0x%02x cmd: 0x%02x\n",
			ADDSHIFT(addr),
			data->byte,
			command ));
        MWRITE_D8(smbHdl->i2cBase, Z001_SMB_ADDR, ADDSHIFT(addr) | (read_write & 1));
		if ( read_write == I2C_SMBUS_WRITE ){		
			MWRITE_D8( smbHdl->i2cBase, Z001_SMB_HSTCOM, command );
		}

		/* The command is: read/write byte */
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_CMD, Z001_SMB_CMD_BYTE );

		size = Z001_SMB_CMD_BYTE;
		break;

    case I2C_SMBUS_BYTE_DATA:
		DBGWRT_1((DBH, "..I2C_SMBUS_BYTE_DATA\n..addr:0x%02x byte:0x%02x cmd: 0x%02x\n",
				ADDSHIFT(addr),
				data->byte,
				command ));

		/* SMB_address from which we read/write */
		MWRITE_D8(smbHdl->i2cBase, Z001_SMB_ADDR, ADDSHIFT(addr) | (read_write & 1));

		/* The command is: read/write byte */
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_CMD, Z001_SMB_CMD_BYTE_DATA );

		/* Parameter command contains the actual address in the device */
		MWRITE_D8(smbHdl->i2cBase, Z001_SMB_HSTCOM, command );

		if (read_write == I2C_SMBUS_WRITE) {
			MWRITE_D8( smbHdl->i2cBase, Z001_SMB_DATA_A, data->byte);
		}

		size = Z001_SMB_CMD_BYTE_DATA;
		break;


    case I2C_SMBUS_WORD_DATA:
	    DBGWRT_1((DBH, "..I2C_SMBUS_WORD_DATA\n..addr:0x%02x byte1:0x%02x"
	    		" byte2:0x%02x cmd: 0x%02x\n",
				ADDSHIFT(addr),
				(data->word & 0xFF),
				((data->word & 0xFF00) >> 8),
				command ));

		/* SMB address of the device */
		MWRITE_D8(smbHdl->i2cBase, Z001_SMB_ADDR, ADDSHIFT(addr) | (read_write & 1));

		/* The command is: read/write word */
		MWRITE_D8(smbHdl->i2cBase, Z001_SMB_CMD, Z001_SMB_CMD_WORD_DATA );

		/* Parameter command contains the actual address in the device */
		MWRITE_D8(smbHdl->i2cBase, Z001_SMB_HSTCOM, command );

		/* If we do writing, provide the word in DataA and DataB.
		   N.B.: Refer to Devices Manual for byte order!*/
		if (read_write == I2C_SMBUS_WRITE) {
			MWRITE_D8(smbHdl->i2cBase, Z001_SMB_DATA_A, data->word & 0xff);
			MWRITE_D8(smbHdl->i2cBase, Z001_SMB_DATA_B, (data->word & 0xff00) >> 8);
		}

		size = Z001_SMB_CMD_WORD_DATA;

		break;


    case I2C_SMBUS_BLOCK_DATA:
		DBGWRT_1((DBH, "..I2C_SMBUS_BLOCK_DATA\n" ));
		/* SMB address of the device */
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_ADDR, ADDSHIFT(addr) | (read_write & 1));

		/* Actual address we want to make the block transfer to&from */
		MWRITE_D8(smbHdl->i2cBase, Z001_SMB_HSTCOM, command);

		if (read_write == I2C_SMBUS_WRITE) {
			len = data->block[0];
			if (len < 0) {
				len = 0;
				data->block[0] = len;
			}
			if (len > 32) {
				len = 32;
				data->block[0] = len;
			}

			/* Data Register A contains length of transfer */
			MWRITE_D8(smbHdl->i2cBase, Z001_SMB_DATA_A, len );

			/* Clear Block Register & set command to read/write block */
			MWRITE_D8( smbHdl->i2cBase,
					   Z001_SMB_CMD,
					   Z001_SMB_CMD_BLOCK_DATA | Z001_SMB_CMD_BLOCK_CLR );

			/* Write up to 32 bytes to block Data Register */
			for (i = 1; i <= len; i++)
				MWRITE_D8(smbHdl->i2cBase, Z001_SMB_BLK, data->block[i]);
		}

		size = Z001_SMB_CMD_BLOCK_DATA;
		break;
	}

	DBGWRT_1(( DBH, "..SMBHSI(0x%x)    =0x%x\n", smbHdl->i2cBase + Z001_SMB_HSI,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_HSI) ));

	DBGWRT_1(( DBH, "..SMBHCBC(0x%x)   =0x%x\n", smbHdl->i2cBase + Z001_SMB_HCBC,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_HCBC) ));

	DBGWRT_1(( DBH, "..SMBSTS(0x%x)    =0x%x\n", smbHdl->i2cBase + Z001_SMB_STS,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_STS) ));

	DBGWRT_1(( DBH, "..SMBCMD(0x%x)    =0x%x\n", smbHdl->i2cBase + Z001_SMB_CMD,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_CMD) ));

	DBGWRT_1(( DBH, "..SMBADDR(0x%x)   =0x%x\n", smbHdl->i2cBase + Z001_SMB_ADDR,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_ADDR) ));

	DBGWRT_1(( DBH, "..SMBDATA_A(0x%x) =0x%x\n", smbHdl->i2cBase + Z001_SMB_DATA_A,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_DATA_A) ));

	DBGWRT_1(( DBH, "..SMBDATA_B(0x%x) =0x%x\n", smbHdl->i2cBase + Z001_SMB_DATA_B,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_DATA_B) ));

	DBGWRT_1(( DBH, "..SMBHOSTCOM(0x%x)=0x%x\n", smbHdl->i2cBase + Z001_SMB_HSTCOM,
		MREAD_D8( smbHdl->i2cBase, Z001_SMB_HSTCOM) ));

	/* Kick off the actual transaction by writing to SMBSTART */
	MWRITE_D8(smbHdl->i2cBase, Z001_SMB_START_PRT, 0x01);

	if( (error = waitBusyReady( smbHdl )) )
	{
		DBGWRT_ERR( (DBH, "%s ERROR: waiting for TxReady!!!\n",
					 functionName) );
		goto ERR_EXIT;
	}

	/* If we made a write or quick access we are finished */
	if ((read_write == I2C_SMBUS_WRITE) || (size == Z001_SMB_CMD_QUICK)){
		return 0;
	}

	/* --- If we made a read, read data according to command type ----*/
	switch(size) {

    case Z001_SMB_CMD_BYTE: /* Result put in SMB_DATA_A */
    case Z001_SMB_CMD_BYTE_DATA:
		data->byte = MREAD_D8( smbHdl->i2cBase, Z001_SMB_DATA_A );
		DBGWRT_1((DBH, "..read: 0x%02x\n ", data->byte ));

		break;

    case Z001_SMB_CMD_WORD_DATA:
		data->word = MREAD_D8( smbHdl->i2cBase, Z001_SMB_DATA_A ) +
						( MREAD_D8(smbHdl->i2cBase, Z001_SMB_DATA_B )<< 8);
		break;

    case Z001_SMB_CMD_BLOCK_DATA:
		/* Number of bytes for block transfer */
		len = MREAD_D8( smbHdl->i2cBase, Z001_SMB_DATA_A );
		if(len > 32)
			len = 32;
		data->block[0] = len;
		/* Clear Block Register & set command to read/write block */
		MWRITE_D8( smbHdl->i2cBase,
				   Z001_SMB_CMD,
				    Z001_SMB_CMD_BLOCK_DATA | Z001_SMB_CMD_BLOCK_CLR );

		for (i = 1; i <= data->block[0]; i++) {
			data->block[i] = MREAD_D8( smbHdl->i2cBase, Z001_SMB_BLK );
		}
		break;
	}

ERR_EXIT:
	return( error );
}


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
    return( "SMB - SMB library: $Id: sysmanagbus_16z001.c,v 1.6 2009/01/21 17:58:16 AWanka Exp $" );
}/*smbIdent*/



#ifdef	SMB_FIXED_HANDLE
u_int32 SMB_16Z001_HandleSize(void)
{
	return sizeof( SMB_HANDLE );
}
#endif

static void EntriesInit( SMB_HANDLE *smbHdl )
{
    smbHdl->entries.Exit        = (int32 (*)(void**))smbExit;
    smbHdl->entries.WriteByte	= (int32 (*)(void*,u_int8,u_int8))
									smbWriteByte;
    smbHdl->entries.ReadByte	= (int32 (*)(void*,u_int8,u_int8*))
									smbReadByte;
    smbHdl->entries.WriteTwoByte= (int32 (*)(void*,u_int8,u_int8,u_int8))
									smbWriteTwoByte;

    smbHdl->entries.Ident		= smbIdent;
    smbHdl->entries.WriteReadTwoByte = (int32 (*)(void*,u_int8,u_int8,u_int8*,
    									u_int8*))smbWriteReadTwoByte;

}

static void CtrlCommonInit( SMB_HANDLE *smbHdl )
{
	/* setup frequency: */
	switch( smbHdl->sclFreq )
	{
	case 37:
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_37KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 55:
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_55KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 111:
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_111KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 149:
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_149KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	case 223:
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_223KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	default:
		MWRITE_D8( smbHdl->i2cBase, Z001_SMB_HCBC,
				   Z001_SMB_HCBC_BASECLK_74KHZ | Z001_SMB_HCBC_IDLE_BASECLK32);
		break;
	}

	/* Activate and reset Host controller */
	MWRITE_D8( smbHdl->i2cBase,
			   Z001_SMB_HSI,
			   Z001_SMB_HSI_HCIE
#ifndef MENMON
			   + Z001_SMB_HSI_IE		/* enable interrupts */
#endif
			   );

	MWRITE_D8( smbHdl->i2cBase,
			   Z001_SMB_CMD,
			   Z001_SMB_CMD_BLOCK_CLR | Z001_SMB_CMD_ABORT );

	/* Delete pending SMBSTS_I_STS irq bit, is CLEARed by writing a '1' */
	MWRITE_D8( smbHdl->i2cBase, Z001_SMB_STS, (Z001_SMB_STS_TERM |
										       Z001_SMB_STS_DONE) );
}

/****************************** SMB_16Z001_Init *****************************
 *
 *  Description:  Initializes the 16Z001 I2C controller lib
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
u_int32 SMB_16Z001_Init(
    SMB_DESC_MENZ001	*desc,
    OSS_HANDLE			*osHdl,
	void				**smbHdlP
)
{
    SMB_HANDLE  *smbHdl;
    u_int32		gotSize=0;
	u_int32 error = 0;

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
    smbHdl->sclFreq 		= desc->sclFreq;
    smbHdl->ownSize  		= gotSize;
    smbHdl->mikroDelay 		= desc->mikroDelay;
    smbHdl->timeOut 		= desc->timeOut;
    smbHdl->osHdl    		= (OSS_HANDLE*) osHdl;

	EntriesInit( smbHdl );		/* initialize method pointers */

	/* perform common controller init */
	CtrlCommonInit( smbHdl );

#ifndef	SMB_FIXED_HANDLE
  CLEANUP:
#endif
    return( error );
}/*SMB_16Z001_Init*/




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
static u_int32 smbExit(
	SMB_HANDLE **smbHdlP
)
{
    u_int32 error  = 0;
    SMB_HANDLE  *smbHdl;

    smbHdl = *smbHdlP;
    *smbHdlP = NULL;

    /* disable I2C module */

	/* Delete pending status bits (CLEARed by writing a '1') */
	MWRITE_D8( smbHdl->i2cBase, Z001_SMB_STS, ( Z001_SMB_STS_CLEAR
											  | Z001_SMB_STS_INUSE ) );

    OSS_MemFree( smbHdl->osHdl, smbHdl, smbHdl->ownSize );

    return( error );
}/*smbExit*/

/******************************** smbReadByte ******************************
 *
 *  Description:  Read a byte from a device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  smbHdl  valid SMB handle
 *			  	  addr	  device address
 *			  	  dataP   pointer to variable where value will be stored
 *
 *  Output.....:  return  0 | error code
 *			  	  *dataP  read value
 *
 *  Globals....:  -
 ****************************************************************************/
static u_int32 smbReadByte(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      *dataP
)
{
    u_int32 error = 0;

    DBGWRT_1((DBH,".smbReadByte addr=%x\n", addr));

    /* set to default */
    *dataP = 0xFF;

    /* sanity check */
    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	/* error = i2cRead( smbHdl, addr, dataP, 1 ); */

	error = smbAccess( 	smbHdl,
					addr,
					I2C_SMBUS_READ,
					0,
					I2C_SMBUS_BYTE,
					(union i2c_smbus_data*)dataP );

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
static u_int32 smbWriteByte(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      data
)
{
	u_int32 error = 0;

	DBGWRT_1((DBH,".smbWriteByte addr=%x data=%x\n", addr, data));

    /* sanity check */
    if(	addr & 0x01 ){
        return( SMB_ERR_ADDR );
    }

	/* error = i2cWrite( smbHdl, addr, &data, 1 ); */

	error = smbAccess( 	smbHdl,
						addr,
						I2C_SMBUS_WRITE,
						data,
						I2C_SMBUS_BYTE,
						(union i2c_smbus_data*)&data );

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
static u_int32 smbWriteTwoByte(
	SMB_HANDLE  *smbHdl,
	u_int8      addr,
	u_int8      cmdAddr,
	u_int8      data
)
{
    u_int32 error = 0;

    DBGWRT_1((DBH,".smbWriteTwo addr=%x cmdAddr=%x data=%x\n", addr, cmdAddr, data));
    if(	addr & 0x01 )
    	return( SMB_ERR_ADDR );

	/* error = i2cWrite( smbHdl, addr, buf, 2 ); */

	smbAccess( 	smbHdl,
				addr,
				I2C_SMBUS_WRITE,
				cmdAddr,
				I2C_SMBUS_BYTE_DATA,
				(union i2c_smbus_data*)&data );

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
static int32 smbWriteReadTwoByte(
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

	DBGWRT_1((DBH,".smbWriteReadTwoByte addr=%x cmdAddr=%x\n", addr, cmdAddr));

	buf[0] = 0xFF;
	buf[1] = 0xFF;

	/* error = i2cWriteRead( smbHdl, addr, &cmdAddr, 1, buf, 2 ); */

	smbAccess( 	smbHdl,
				addr,
				I2C_SMBUS_READ,
				cmdAddr,
				I2C_SMBUS_WORD_DATA,
				(union i2c_smbus_data*)buf );


	*data1P = buf[0];
	*data2P = buf[1];

	return error;
}/*smbWriteReadTwoByte*/

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  smb2_ich.c
 *      \project  SMB2 library
 *
 *       Author  sv
 *        $Date: 2013/07/15 18:05:34 $
 *    $Revision: 1.16 $
 *
 *      \brief  system managment bus driver for the ICH
 *				SMB controller
 *
 *     Required: -
 *     \switches: MAC_IO_MAPPED
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: smb2_ich.c,v $
 * Revision 1.16  2013/07/15 18:05:34  channoyer
 * R: Not easy to build the BSP from IDE
 * M: Update MEN specific files accordingly
 *
 * Revision 1.15  2012/12/17 14:24:01  dpfeuffer
 * R: block r/w and PEC support requested from customer (for F18E)
 * M: block r/w and PEC support implemented
 *
 * Revision 1.14  2012/02/17 08:18:01  dpfeuffer
 * R: AMD FCH SMB controller support
 * M: LocWaitBusyReady(): delay added for FCH
 *
 * Revision 1.13  2010/06/11 09:04:37  ufranke
 * R: sometimes wasting time during transfer
 * M: LocWaitBusyReady() fixed
 *
 * Revision 1.12  2009/03/17 14:45:03  dpfeuffer
 * R: doxygen warning
 * M: cosmetics
 *
 * Revision 1.11  2009/03/17 13:59:28  dpfeuffer
 * R: compiler warnings wit VC2008 64bit compiler
 * M: debug prints with pointers changed to %p
 *
 * Revision 1.10  2007/10/09 09:34:01  SVogel
 * added busyWait initialization
 *
 * Revision 1.9  2007/07/19 15:13:55  DPfeuffer
 * cast for Windows compiler was necessary because MACCESS modification
 *
 * Revision 1.8  2007/02/20 15:10:29  DPfeuffer
 * - undo: changed interface of *_Init() and *_Exit functions
 *         (VxW6.3 must consider this by disabling strict aliasing)
 *
 * Revision 1.7  2006/11/15 08:07:58  svogel
 * adaptions due to interface change of common library
 * added
 * -----
 * + smb quick command support
 *
 * Revision 1.6  2006/02/27 15:26:48  DPfeuffer
 * SMB_ICH_Init(): alertPollFreq parameter passing added
 *
 * Revision 1.5  2006/02/22 16:02:06  DPfeuffer
 * LocSmbXfer(): SMB_ACC_BYTE_DATA, SMB_ACC_WORD_DATA fixed
 *
 * Revision 1.4  2006/02/10 15:44:12  SVogel
 * bugfix in SMB_ICH_init
 *
 * Revision 1.3  2005/11/28 09:35:27  SVogel
 * Added doxygen documentation.
 *
 * Revision 1.2  2005/11/24 14:45:23  SVogel
 * Removed define OSSH.
 *
 * Revision 1.1  2005/11/24 10:15:12  SVogel
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
  *  (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
/*#define DBG*/
#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/maccess.h>
#include <MEN/mdis_err.h>

#define SMB_COMPILE
#include <MEN/smb2.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/** structure for SMB_HANDLE */
typedef struct
{
	SMB_ENTRIES entries; 		/**< function entries */	
	SMB_COM_HANDLE smbComHdl; 	/**< Common handle entries */

	u_int32    ownSize;			/**< size of memory allocated for this handle */
	MACCESS    baseAddr;		/**< base address of smb */
}SMB_HANDLE;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/

/* SMBus I/O Register Address Map */
#define SMB_HST_STS                 0x00 /* host status
                                          * register SMB_HST_STS (default: 0x00) */
#define SMB_HST_CNT                 0x02 /* host control
                                          * register SMB_HST_CNT (default: 0x00) */
#define SMB_HST_CMD                 0x03 /* host command 
                                          * register SMB_HST_CMD (default: 0x00)
                                          * host status command register bit 0..7 */
#define SMB_XMIT_SLVA               0x04 /* transmitt slave address
                                          * register SMB_XMIT_SLVA (default: 0x00) 
                                          * transmitt slave register bit 7..1 address, bit 0 0=write, 1=read */
#define SMB_HST_D0                  0x05 /* host data 0
                                          * register SMB_HST_D0 (default: 0x00)
                                          * host data0 register for block write commands */
#define SMB_HST_D1                  0x06 /* host data 1
                                          * register SMB_HST_D1 (default: 0x00) 
                                          * host data1 register for, used during execution of any command */ 
#define SMB_HOST_BLOCK_DB           0x07 /* host block data byte
                                          * register SMB_HOST_BLOCK_DB (default: 0x00)
                                          * host block data byte register */
#define SMB_PEC                     0x08 /* packet error check
                                          * register SMB_PEC (default: 0x00)
                                          * packet error check register */
#define SMB_RCV_SLVA                0x09 /* receive slave address
                                          * register SMB_RCV_SLVA (default: 0x44)
                                          * receive slave address register bit 6..0 = slave address */
#define SMB_SLV_DATA_0              0x0A /* receive slave data 0
                                          * register SMB_SLV_DATA_0 (default: 0x00)
                                          * receive slave data0 register */
#define SMB_SLV_DATA_1              0x0B /* receive slave data 1
                                          * register SMB_SLV_DATA_1 (default: 0x00)
                                          * receive slave data1 register */
#define SMB_AUX_STS                 0x0C /* auxiliary status
                                          * register SMB_AUX_STS (default: 0x00) */
#define SMB_AUX_CTL                 0x0D /* auxiliary control
                                          * register SMB_AUX_CTL (default: 0x00) */
#define SMB_SMLINK_PIN_CTL          0x0E /* SMLink pin control
                                          * register SMB_SMLINK_PIN_CTL (default: 0x04) */
#define SMB_SMBUS_PIN_CTL           0x0F /* SMBus pin control
                                          * register SMB_SMBUS_PIN_CTL (default: 0x00) */ 
#define SMB_SLV_STS                 0x10 /* slave status 
                                          * register SMB_SLV_STS (default: 0x00) */
#define SMB_SLV_CMD                 0x11 /* slave command
                                          * register SMB_SLV_CMD (default: 0x00) */
#define SMB_NOTIFY_DADDR            0x14 /* notify device address
                                          * register SMB_NOTIFY_DADDR (default: 0x00)
                                          *  notify device address register bit 7..1 device address */
#define SMB_NOTIFY_DLOW             0x16 /* notify data low byte
                                          * register SMB_NOTIFY_DLOW (default: 0x00)
                                          *   notify data low byte register */
#define SMB_NOTIFY_DHIGH            0x17 /* notify data high byte 
                                          * register SMB_NOTIFY_DHIGH   (default: 0x00)
                                          * notify data high byte register */

/* register SMB_HST_STS (default: 0x00) */
#define HOST_BUSY                  0x01
#define INTR                       0x02
#define DEV_ERR                    0x04
#define BUS_ERR                    0x08
#define FAILED                     0x10
#define SMBALERT_STS               0x20
#define INUSE_STS                  0x40
#define BYTE_DONE_STATUS           0x80

/*#define SMB_CLEAR_STATUS           (INTR+DEV_ERR+BUS_ERR+FAILED)*/
#define SMB_CLEAR_STATUS           (0xFF) /* clear all */

/* register SMB_HST_CNT (default: 0x00) */
#define INTREN                     0x01
#define KILL                       0x02
#define QUICK                      (0x00)
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

/* register SMB_AUX_STS (default: 0x00) */
#define CRC_ERROR                   0x01
#define SMBUS_TCO_MODE              0x02

/* register SMB_AUX_CTL (default: 0x00) */
#define AUTO_APPEND_CRC             0x01
#define EN_32BYTE_BUFFER            0x02

/* register SMB_SMLINK_PIN_CTL (default: 0x04) */
#define SMLINK0_CUR_STS             0x01
#define SMLINK1_CUR_STS             0x02
#define SMLINK_CLK_CTL              0x04

/* register SMB_SMBUS_PIN_CTL (default: 0x00) */
#define SMBCLK_CUR_STS              0x01
#define SMBDATA_CUR_STS             0x02
#define SMBCLK_CTL                  0x04

/* register SMB_SLV_STS (default: 0x00) */
#define HOST_NOTIFY_STS             0x01

/* register SMB_SLV_CMD (default: 0x00) */
#define HOST_NOTIFY_INTREN          0x01
#define HOST_NOTIFY_WKEN            0x02
#define SMBALERT_DIS                0x04
/* end of SMBus I/O Register Address Map */

#define WAIT_BUSY_TIME              8000
#define WRITE_ENABLE                0x00
#define READ_ENABLE                 0x01

#define	DBG_MYLEVEL		smbHdl->smbComHdl.dbgLevel
#define	DBH				smbHdl->smbComHdl.dbgHdl

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

static char * LocSmbIdent( void );
static int32 LocWaitBusyReady( SMB_HANDLE *smbHdl, u_int32 flags );
static int32 LocSmbExit( SMB_HANDLE  **smbHdlP );
static int32 LocSmbXfer( SMB_HANDLE *smbHdl, u_int32 flags, u_int16 addr,
						 u_int8 read_write, u_int8 cmdAddr,
						 u_int8 size, u_int8 *dataP );

/*******************************  LocSmbIdent  *************************
 *
 *  Description:  Returns the identification string of the module.
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  identification string
 *
 *  Globals....:  -
 ****************************************************************************/
static char *LocSmbIdent( void )
{
    return( "ICH SMB - SMB2 library: $Id: smb2_ich.c,v 1.16 2013/07/15 18:05:34 channoyer Exp $" );
}/*LocSmbIdent*/

/*******************************  LocWaitBusyReady  *****************************/
/** Waits to BUSY first and then leaving BUSY and bus idle.
 *				  Returns imediatly if DEV_ERR detected.
 *
 * \param   smbHdl     valid SMB handle
 * \param   flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *
 * \return    smb error
 *
 ****************************************************************************/
static int32 LocWaitBusyReady( SMB_HANDLE *smbHdl, u_int32 flags )
{
    u_int32 i=0;
    u_int8 status=0;
	void* baseAddr = (void*)smbHdl->baseAddr;

	/* wait for busy */
    for( i=0; i<WAIT_BUSY_TIME; i++ )
	{
		status = MREAD_D8(baseAddr, SMB_HST_STS);
		
		if( status & DEV_ERR )
		{
			/* Parity Error Correction enabled ? */
			if( flags & SMB_FLAG_PEC )
			{
				/* check for PEC error (and clear it) */
				if( MREAD_D8(baseAddr, SMB_AUX_STS) & CRC_ERROR )
				{
					MWRITE_D8(baseAddr, SMB_AUX_STS, CRC_ERROR);
					DBGWRT_ERR((DBH, "*** LocWaitBusyReady: PEC error\n"));
					return( SMB_ERR_PEC );
				}
			}
			return( SMB_ERR_ADDR );
	    }
		
		if( status & BUS_ERR )
		{
			return( SMB_ERR_COLL );
	    }
		
		if( status & HOST_BUSY )
		{
			/*DBGWRT_1((DBH,"%s: line %d   - i %d - status %02x\n", __FUNCTION__, __LINE__, i, status ));*/
			break;
	    }
	}

	/*DBGWRT_1((DBH,"%s: line %d   - i %d\n", __FUNCTION__, __LINE__, i ));*/

	/* wait for !busy && INTR */
    for( i=0; i<WAIT_BUSY_TIME; i++ )
	{
		/* delay required for AMD FCH SMB controller */
		OSS_Delay( smbHdl->smbComHdl.osHdl, 1 );

		status = MREAD_D8(baseAddr, SMB_HST_STS);
		
		if( status & DEV_ERR )
		{
			/* Parity Error Correction enabled ? */
			if( flags & SMB_FLAG_PEC )
			{
				/* check for PEC error (and clear it) */
				if( MREAD_D8(baseAddr, SMB_AUX_STS) & CRC_ERROR )
				{
					MWRITE_D8(baseAddr, SMB_AUX_STS, CRC_ERROR);
					DBGWRT_ERR((DBH, "*** LocWaitBusyReady: PEC error\n"));
					return( SMB_ERR_PEC );
				}
			}

			return( SMB_ERR_ADDR );
		}
		
		if( status & BUS_ERR )
		{
			return( SMB_ERR_COLL );
		}

		/*DBGWRT_1((DBH,"%s: line %d   - i %d - status %02x\n", __FUNCTION__, __LINE__, i, status ));*/
		
		if( !(status & HOST_BUSY) /* busy is gone */
			&& ( status & INTR )  /* intr say complete */
		  )
		{		    
			/*DBGWRT_1((DBH,"%s: line %d   - i %d - status %02x\n", __FUNCTION__, __LINE__, i, status ));*/
			return( SMB_ERR_NO );
		}		
	}
	return( SMB_ERR_BUSY );	
}/* LocWaitBusyReady */

 /*******************************  LocSmbExit  *****************************/
/** Deinitializes this library and SMB controller.
 *
 * \param smbHdlP	\INOUT  pointer to variable where the handle is stored,
 *                          set to 0
 * \return    always 0
 *
 ****************************************************************************/
static int32 LocSmbExit
(
	SMB_HANDLE **smbHdlP
)
{
	int32 error  = 0;
	SMB_HANDLE  *smbHdl;

	smbHdl = *smbHdlP;
	*smbHdlP = NULL;

	/* deinitialize common interface */
	if(smbHdl->smbComHdl.ExitCom)
	{
	    smbHdl->smbComHdl.ExitCom(smbHdl);
    }

	/* clear status first */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	

    OSS_MemFree( smbHdl->smbComHdl.osHdl, smbHdl, smbHdl->ownSize );

	return( error );
}/* LocSmbExit */

 /******************************** LocSmbXfer *****************************/
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
static int32 LocSmbXfer
(
    SMB_HANDLE *smbHdl,
	u_int32 flags,
	u_int16 addr,
	u_int8 read_write,
	u_int8 cmdAddr,
	u_int8 size,
	u_int8 *dataP
)
{
	u_int8  status = 0;
	u_int8 len = 0;
	u_int32 error = SMB_ERR_NO, i;
	u_int16 *wDataP = (u_int16*)dataP;
	DBGCMD(	static const char functionName[] = "SMB2_ICH - LocSmbXfer:"; )

	if(	addr & READ_ENABLE )
    {
    	return( SMB_ERR_ADDR );
    }

    DBGWRT_1( (DBH, "%s: smbHdl = %08p\n", functionName, smbHdl) );

	/* check for PEC error (and clear it) */
	if( MREAD_D8(smbHdl->baseAddr, SMB_AUX_STS) & CRC_ERROR )
	{
		MWRITE_D8(smbHdl->baseAddr, SMB_AUX_STS, CRC_ERROR);
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

    MWRITE_D8(smbHdl->baseAddr, SMB_XMIT_SLVA, addr | read_write);
       
	switch(size)
	{
		case SMB_ACC_QUICK:		    
		    DBGWRT_2( (DBH, "%s SMB_ACC_QUICK\n", functionName) );
			size = QUICK + START;

			/* PEC for quick command not possible */
			if( flags & SMB_FLAG_PEC )
				flags = flags & ~SMB_FLAG_PEC;

			break;

		case SMB_ACC_BYTE:	
		    if( read_write == SMB_WRITE )
            {
                MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);
            }	    
		    size = BYTE + START;
		    break;
		    
		case SMB_ACC_BYTE_DATA:
		    DBGWRT_2( (DBH, "%s SMB_ACC_BYTE_DATA\n", functionName) );		    
		    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);
		    
		   	if( read_write == SMB_WRITE )
            {
                MWRITE_D8(smbHdl->baseAddr, SMB_HST_D0, *dataP);
            }

		   	size = BYTE_DATA + START;
			break;

		case SMB_ACC_WORD_DATA:
		    DBGWRT_2( (DBH, "%s SMB_ACC_WORD_DATA\n", functionName) );
		    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);
            
            if( read_write == SMB_WRITE )
            {                
                MWRITE_D8(smbHdl->baseAddr, SMB_HST_D0, *dataP);
				MWRITE_D8(smbHdl->baseAddr, SMB_HST_D1, *(dataP+1));  
            }

            size = WORD_DATA + START;
			break;

		case SMB_ACC_BLOCK_DATA:
			DBGWRT_2( (DBH, "%s SMB_ACC_BLOCK_DATA cmd: 0x%02x len: 0x%02x\n",
							functionName, cmdAddr, dataP[0] ) );
		    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);

			if (read_write == SMB_WRITE) {
				
				len = dataP[0];
				if (len > SMB_BLOCK_MAX_BYTES) {
					len = SMB_BLOCK_MAX_BYTES;
					dataP[0] = len;
				}

				/* Data Register 0 contains length of transfer */
				MWRITE_D8(smbHdl->baseAddr, SMB_HST_D0, len);

				/* reset block register buffer pointer */
				MREAD_D8(smbHdl->baseAddr, SMB_HST_CNT);

				/* write the up to 32 byte to block Data Register */
				for (i = 1; i <= len; i++)
					MWRITE_D8(smbHdl->baseAddr, SMB_HOST_BLOCK_DB, dataP[i]);
			}

			size = BLOCK + START;
			break;

		default:
			error = SMB_ERR_NOT_SUPPORTED;
			goto ERR_EXIT;
	}

	/* Parity Error Correction demanded? */
    if( flags & SMB_FLAG_PEC ){
		/* enable automatic CRC apposition */
		MSETMASK_D8(smbHdl->baseAddr, SMB_AUX_CTL, AUTO_APPEND_CRC);
	    DBGWRT_2( (DBH, "%s set AUTO_APPEND_CRC for PEC\n", functionName) );
    }
	else{
		/* disable automatic CRC apposition */
		MCLRMASK_D8(smbHdl->baseAddr, SMB_AUX_CTL, AUTO_APPEND_CRC);
	}

	MWRITE_D8(smbHdl->baseAddr, SMB_HST_CNT, size);

	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, (char)SMB_CLEAR_STATUS);
	
	if( (error = LocWaitBusyReady( smbHdl, flags ) ))
	{   	    	    
	    goto ERR_EXIT;
	}
		
	/* initialize and perform a read opereation */	
	size &= ~START;

	if(	read_write == SMB_READ )
    {           
	    switch( size )
	    {
	        case BYTE:
	        case BYTE_DATA:
	            *dataP = MREAD_D8(smbHdl->baseAddr, SMB_HST_D0);
	            DBGWRT_3( (DBH, "%s BYTE(_DATA) data=%04x\n",
				 	   functionName, *dataP) );
	            break;
	            
	        case WORD_DATA:
	            *wDataP = MREAD_D8(smbHdl->baseAddr, SMB_HST_D0) +
                         (MREAD_D8(smbHdl->baseAddr, SMB_HST_D1) << 8);
                DBGWRT_3( (DBH, "%s WORD_DATA data=%04x\n",
				 	   functionName, *wDataP) );
	            break;

		case BLOCK:
			/* get length of block */
			len = MREAD_D8(smbHdl->baseAddr, SMB_HST_D0); 
			if( len > SMB_BLOCK_MAX_BYTES )
				len = SMB_BLOCK_MAX_BYTES;
			dataP[0] = len;

			/* reset block register buffer pointer */
			MREAD_D8(smbHdl->baseAddr, SMB_HST_CNT);

			/* get the data */
			for (i = 1; i <= len; i++) {
				dataP[i] = MREAD_D8(smbHdl->baseAddr, SMB_HOST_BLOCK_DB);
				DBGWRT_3( (DBH, "%s BLOCK_DATA len=%d, i=%d, data=%02x\n",
					 	   functionName, len, i, dataP[i]) );
			}

			break;

	        default:
	            error = SMB_ERR_NOT_SUPPORTED;
			    goto ERR_EXIT;
	            break;
	    }
    }
    
ERR_EXIT:
    return( error );
}/* LocSmbXfer */

 /***************************** SMB_ICH_Init ****************************/
/** Initializes this library and check's the SMB host.
 *
 * \param  descP    \IN  descriptor
 * \param  osHdl    \IN  OS specific handle
 * \param  smbHdlP  \IN  pointer to variable where the handle will be stored
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
#ifdef MAC_IO_MAPPED 
    u_int32 SMB_ICH_IO_Init 
 #else 
    u_int32 SMB_ICH_Init
 #endif
 ( SMB_DESC_ICH	*descP, OSS_HANDLE		*osHdl, void			**smbHdlP ) 
 { 
    u_int32     error  = 0; 
    SMB_HANDLE  *smbHdl = NULL; 
    u_int32		gotSize = 0;
#ifdef MAC_IO_MAPPED    
	DBGCMD(	static const char functionName[] = "SMB2 - SMB_ICH_IO_Init:"; )
#else
    DBGCMD(	static const char functionName[] = "SMB2 - SMB_ICH_Init:"; )
#endif
			
	smbHdl   = (SMB_HANDLE*) OSS_MemGet( osHdl, sizeof(SMB_HANDLE), &gotSize );
	if( smbHdl == NULL )
	{
	    error = SMB_ERR_NO_MEM;
		goto CLEANUP;
	}/*if*/

	*smbHdlP = smbHdl;

	/* init the structure */
	OSS_MemFill( osHdl, gotSize, (char*)smbHdl, 0 ); /* zero memory */

   	smbHdl->smbComHdl.osHdl = osHdl;

	DBGINIT(( NULL, &smbHdl->smbComHdl.dbgHdl ));

	smbHdl->smbComHdl.dbgLevel		= descP->dbgLevel;
	smbHdl->smbComHdl.alertPollFreq = descP->alertPollFreq;

    DBGWRT_1( (DBH, "%s: smbHdl = %08p\n", functionName, smbHdl) );

	smbHdl->entries.Capability = SMB_FUNC_SMBUS_QUICK |
	                             SMB_FUNC_SMBUS_BYTE |
								 SMB_FUNC_SMBUS_BYTE_DATA |
								 SMB_FUNC_SMBUS_WORD_DATA |
								 SMB_FUNC_SMBUS_BLOCK_DATA | 
								 SMB_FUNC_SMBUS_HWPEC_CALC;	/* HW PEC supported */

    smbHdl->smbComHdl.busyWait  = descP->busyWait;

	smbHdl->baseAddr 			= descP->baseAddr;

	smbHdl->ownSize  			= gotSize;

	smbHdl->entries.Exit		= (int32 (*)(void**))LocSmbExit;
	smbHdl->entries.SmbXfer		= (int32 (*)(void *, u_int32, u_int16,
							  				u_int8, u_int8, u_int8, u_int8 *))
											LocSmbXfer;

	smbHdl->entries.I2CXfer		= NULL;
	smbHdl->entries.Ident		= (char* (*)(void))LocSmbIdent;
		
	error = SMB_COM_Init(smbHdl);
    if( error )
    {
   		DBGWRT_ERR((DBH, "*** %s: SMB_COM_Init\n", functionName ));
        goto CLEANUP;        
    }
    
    /* clear status first */
	MWRITE_D8(smbHdl->baseAddr, SMB_HST_STS, SMB_CLEAR_STATUS);

    /*
	 * set E32B bit for block access
	 * This enables the block commands to transfer or receive up to 32-bytes. 
	 */
	MWRITE_D8(smbHdl->baseAddr, SMB_AUX_CTL, EN_32BYTE_BUFFER);

CLEANUP:
	return( error );
}/* SMB_ICH_Init */





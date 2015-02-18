/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  smb2_sch.c
 *      \project  SMB2 library
 *
 *       Author  MSoehnlein
 *        $Date: 2012/01/30 11:50:01 $
 *    $Revision: 1.4 $
 *
 *      \brief  system managment bus driver for the SCH
 *				SMB controller
 *
 *     Required: -
 *     \switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: smb2_sch.c,v $
 * Revision 1.4  2012/01/30 11:50:01  dpfeuffer
 * R: unused define
 * M: INTR define removed
 *
 * Revision 1.3  2009/03/17 14:45:06  dpfeuffer
 * R: doxygen warning
 * M: cosmetics
 *
 * Revision 1.2  2009/03/17 13:59:30  dpfeuffer
 * R: compiler warnings wit VC2008 64bit compiler
 * M: debug prints with pointers changed to %p
 *
 * Revision 1.1  2008/10/20 10:42:58  MSoehnlein
 * Initial Revision
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
#define SMB_HST_CNT                 0x00 /* host control register */
#define SMB_HST_STS                 0x01 /* host status register */
#define SMB_XMIT_SLVA               0x04 /* transmitt slave address register */
                                         /* bit0: 0=write, 1=read */
#define SMB_HST_CMD                 0x05 /* host command/status register */
#define SMB_HST_D0                  0x06 /* host data 0 register */
#define SMB_HST_D1                  0x07 /* host data 1 register */


/* Register SMB_HST_STS */
#define COMPLETION_STATUS          0x01
#define DEV_ERR                    0x02
#define BUS_ERR                    0x04
#define HOST_BUSY                  0x08

#define SMB_CLEAR_STATUS           (COMPLETION_STATUS | DEV_ERR | BUS_ERR)
#define SMB_ABORT_TRANSF           0x00

/* register SMB_HST_CNT */
#define QUICK                      0x00
#define BYTE                       0x01
#define BYTE_DATA                  0x02
#define WORD_DATA                  0x03
#define BLOCK                      0x05
#define START                      0x10


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
static int32 LocWaitBusyReady( void * baseAddr);
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
    return( "SCH SMB - SMB2 library: $Id: smb2_sch.c,v 1.4 2012/01/30 11:50:01 dpfeuffer Exp $" );
}/*LocSmbIdent*/

  /*******************************  LocWaitBusyReady  *****************************/
/** Waits to BUSY first and then leaving BUSY.
 *				  Returns imediatly if DEV_ERR or BUS_ERR detected.
 *
 * \param baseAddr	\INOUT  pointer to base address of the SMB controller
 *
 * \return    smb error
 *
 ****************************************************************************/
static int32 LocWaitBusyReady( void * baseAddr )
{
    u_int32 i = 0;
    u_int8 status = 0;

    for( i=0; i<WAIT_BUSY_TIME; i++ )
	{
		status = MREAD_D8(baseAddr, SMB_HST_STS);
		
		/* Abort if device error detected */
		if( status & DEV_ERR )
			return( SMB_ERR_ADDR );
		
		/* Abort if bus error detected */
		if( status & BUS_ERR )
			return( SMB_ERR_COLL );
		
		/* Abort if host busy or completion detected */
		if( status & (HOST_BUSY | COMPLETION_STATUS) )
			break;
	}
	

    for( i=0; i<WAIT_BUSY_TIME; i++ )
	{
		status = MREAD_D8(baseAddr, SMB_HST_STS);
		
		/* Abort if device error detected */
		if( status & DEV_ERR )
			return( SMB_ERR_ADDR );
		
		/* Abort if bus error detected */
		if( status & BUS_ERR )
			return( SMB_ERR_COLL );
		
		/* Return if completion detected */
		if( status == COMPLETION_STATUS )
			return( SMB_ERR_NO );
	}

	/* Transfer not completed */
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
	u_int8	status  = 0;
	u_int8	control = 0;
	u_int32	error = SMB_ERR_NO;
	u_int16	*wDataP = (u_int16*)dataP;
	DBGCMD(	static const char functionName[] = "SMB2_SCH - LocSmbXfer:"; )

	/* Abort of client address is not 7-bit */
	if(	addr & READ_ENABLE )
    	return( SMB_ERR_ADDR );

    DBGWRT_1( (DBH, "%s: smbHdl = %08p\n", functionName, smbHdl) );

	/* Abort any transfer in progress*/
    control = MREAD_D8(smbHdl->baseAddr, SMB_HST_CNT);	
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_CNT, SMB_ABORT_TRANSF);	

	/* Clear all error bits */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
    status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);	

	/* If status has not been cleared */
	if( status & (COMPLETION_STATUS | DEV_ERR | BUS_ERR | HOST_BUSY) )
	return( SMB_ERR_BUSY );	


    MWRITE_D8(smbHdl->baseAddr, SMB_XMIT_SLVA, addr | read_write);

	switch(size)
	{
		case SMB_ACC_QUICK:		
		    DBGWRT_2( (DBH, "%s SMB_ACC_QUICK\n", functionName) );
			size = QUICK;
			break;

		case SMB_ACC_BYTE:	
		    if( read_write == SMB_WRITE )
            {
                MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);
            }	
		    size = BYTE;
		    break;
		
		case SMB_ACC_BYTE_DATA:
		    DBGWRT_2( (DBH, "%s SMB_ACC_BYTE_DATA\n", functionName) );		
		    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);
		
		   	if( read_write == SMB_WRITE )
            {
                MWRITE_D8(smbHdl->baseAddr, SMB_HST_D0, *dataP);
            }

		   	size = BYTE_DATA;
			break;

		case SMB_ACC_WORD_DATA:
		    DBGWRT_2( (DBH, "%s SMB_ACC_WORD_DATA\n", functionName) );
		    MWRITE_D8(smbHdl->baseAddr, SMB_HST_CMD, cmdAddr);

            if( read_write == SMB_WRITE )
            {
                MWRITE_D8(smbHdl->baseAddr, SMB_HST_D0, *dataP);
				MWRITE_D8(smbHdl->baseAddr, SMB_HST_D1, *(dataP+1));
            }

            size = WORD_DATA;
			break;

		default:
			error = SMB_ERR_NOT_SUPPORTED;
			goto ERR_EXIT;
	}

	control = size | START;
	MWRITE_D8(smbHdl->baseAddr, SMB_HST_CNT, control);
	
	if( (error = LocWaitBusyReady((void*)smbHdl->baseAddr)) )
	{   	    	
	    goto ERR_EXIT;
	}
		
	/* initialize and perform a read opereation */	

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
	
	        default:
	            error = SMB_ERR_NOT_SUPPORTED;
			    goto ERR_EXIT;
	            break;
	    }
    }

ERR_EXIT:
    return( error );
}/* LocSmbXfer */

 /***************************** SMB_SCH_Init ****************************/
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
    u_int32 SMB_SCH_IO_Init
 #else
    u_int32 SMB_SCH_Init
 #endif
 ( SMB_DESC_SCH	*descP, OSS_HANDLE *osHdl, void **smbHdlP )
 {
	u_int8		status  = 0;
	u_int8		control = 0;
    u_int32     error   = 0;
    u_int32		gotSize = 0;
    SMB_HANDLE  *smbHdl = NULL;

#ifdef MAC_IO_MAPPED
	DBGCMD(	static const char functionName[] = "SMB2 - SMB_SCH_IO_Init:"; )
#else
    DBGCMD(	static const char functionName[] = "SMB2 - SMB_SCH_Init:"; )
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
								 SMB_FUNC_SMBUS_WORD_DATA;
								
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

	/* Abort any transfer in progress*/
    control = MREAD_D8(smbHdl->baseAddr, SMB_HST_CNT);	
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_CNT, SMB_ABORT_TRANSF);	

	/* Clear all error bits */
	MWRITE_D8(smbHdl->baseAddr,SMB_HST_STS, SMB_CLEAR_STATUS);	
    status = MREAD_D8(smbHdl->baseAddr, SMB_HST_STS);	

	/* If status has not been cleared */
	if( status & (COMPLETION_STATUS | DEV_ERR | BUS_ERR | HOST_BUSY) )
	error = SMB_ERR_BUSY;	

CLEANUP:
	return( error );
}/* SMB_SCH_Init */




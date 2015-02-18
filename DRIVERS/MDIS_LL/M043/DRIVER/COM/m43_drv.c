/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m43_drv.c
 *      Project: M43 module driver (MDIS V4.x)
 *
 *       Author: ds
 *        $Date: 2013/07/11 14:32:34 $
 *    $Revision: 1.11 $
 *
 *  Description: Low level driver for M43 modules
 *
 *     Required:  -
 *     Switches:  _ONE_NAMESPACE_PER_DRIVER_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m43_drv.c,v $
 * Revision 1.11  2013/07/11 14:32:34  gv
 * R: Porting to MDIS5
 * M: Changed according to MDIS Porting Guide 0.9
 *
 * Revision 1.10  2004/04/08 13:05:46  cs
 * additions for MDIS4/2004 conformity
 *   changed M43_Info addrSize
 *   added   M43_Info lockmode processing
 *
 * Revision 1.9  2004/04/07 15:08:19  cs
 * cosmetics to reach MDIS4/2004 conformity
 *   added static prototypes
 *
 * Revision 1.8  2001/08/16 10:09:05  kp
 * M43_GetEntry renamed now in m43_drv.h according to variant
 *
 * Revision 1.7  1998/08/03 16:34:28  Schmidt
 * idFuncTbl is now located in LL_HANDLE
 * idFuncTbl is now initialized in Init
 * IdFuncTbl() removed
 * error messages are now with prefix LL
 *
 * Revision 1.6  1998/07/27 11:49:21  see
 * wrong static const char *IdentString type changed
 * M43_Init: ID_CHECK default is now 1
 * M43_Init: error message was wrong
 * M43_Getstat: M_LL_ID_SIZE returned wrong size=32
 * M43_Getstat: M_LL_BLK_ID_DATA changed (template style)
 * LL_HANDLE: unused irqHdl removed
 * LL_HANDLE: base renamed to ma (template style)
 * some defs renamed  (template style)
 * some defs added (template style)
 *
 * Revision 1.5  1998/07/20 09:22:26  Schmidt
 * DBGEXIT added
 *
 * Revision 1.4  1998/07/17 12:12:16  Franke
 * cosmetics
 *
 * Revision 1.3  1998/07/16 16:03:03  Schmidt
 * update to MDIS 4.1, new function IdFuncTbl(),
 * Set/Getstat: M_BUF_WR_MODE removed / M_LL_CH_DIR is M_CH_INOUT
 *
 * Revision 1.2  1998/03/13 17:09:47  Schmidt
 * repair ident string
 * remove permanent debug infos
 * use MD16 instead of MD08
 * remove #include <stdarg.h>
 *
 * Revision 1.1  1998/03/09 09:38:39  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
static const char IdentString[]="M43 - m43 low level driver: $Id: m43_drv.c,v 1.11 2013/07/11 14:32:34 gv Exp $";

#include <MEN/men_typs.h>   /* system dependend definitions   */
#include <MEN/maccess.h>    /* hw access macros and types     */
#include <MEN/dbg.h>		/* debug functions                */
#include <MEN/oss.h>        /* oss functions                  */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/modcom.h>     /* id prom functions              */
#include <MEN/mdis_api.h>   /* MDIS global defs               */
#include <MEN/mdis_com.h>   /* MDIS common defs               */
#include <MEN/mdis_err.h>   /* MDIS error codes               */
#include <MEN/ll_defs.h>    /* low level driver definitions   */

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* general */
#define CH_NUMBER			8			/* nr of device channels */
#define ADDRSPACE_COUNT		1			/* nr of required address spaces */
#define MOD_ID_MAGIC		0x5346      /* eeprom identification (magic) */
#define MOD_ID_SIZE			128			/* eeprom size */
#define MOD_ID				43			/* module id */

/* debug settings */
#define DBG_MYLEVEL		m43Hdl->dbgLevel
#define DBH             m43Hdl->dbgHdl

/* register offsets */
#define M43_CTRL_REG		0x00

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* ll handle */
typedef struct
{
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table */
    u_int32         ownMemSize;
    OSS_HANDLE		*osHdl;
	MACCESS			ma;
	OSS_SEM_HANDLE	*devSem;
	u_int32			useId;
	/* debug */
    u_int32         dbgLevel;		/* debug level */
	DBG_HANDLE      *dbgHdl;		/* debug handle */
} M43_HANDLE;

/* include files which need LL_HANDLE */
#include <MEN/ll_entry.h>		/* low level driver jumptable  */
#include <MEN/m43_drv.h>		/* M43 driver header file */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
static int32 M43_Init( DESC_SPEC *descSpec, OSS_HANDLE *OsHdl, MACCESS *ma,
		  OSS_SEM_HANDLE *DevSem, OSS_IRQ_HANDLE *irqHandle,
		  LL_HANDLE **LLDat );
static int32 M43_Exit( LL_HANDLE **LLDat );
static int32 M43_Read( LL_HANDLE *LLDat, int32 ch, int32 *value );
static int32 M43_Write( LL_HANDLE *LLDat, int32 ch, int32 value );
static int32 M43_SetStat( LL_HANDLE *LLDat, int32 ch, int32 code, INT32_OR_64 value32_or_64 );
static int32 M43_GetStat( LL_HANDLE *LLDat, int32 ch, int32 code, INT32_OR_64 *value32_or_64P );
static int32 M43_BlockRead( LL_HANDLE *LLDat, int32 ch, void *buf, int32 size, int32 *nbrRdBytesP );
static int32 M43_BlockWrite( LL_HANDLE *LLDat, int32 ch, void *buf, int32 size, int32 *nbrWrBytesP );
static int32 M43_Irq( LL_HANDLE *LLDat );
static int32 M43_Info( int32 infoType, ... );

static char* Ident( void );


/**************************** M43_GetEntry **********************************
 *
 *  Description:  Gets the entry points of the low-level driver functions.
 *
 *         Note:  Is used by MDIS kernel only.
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *
 *  Output.....:  drvP  pointer to the initialized structure
 *
 *  Globals....:  ---
 ****************************************************************************/
void M43_GetEntry( LL_ENTRY* drvP )
{
    drvP->init			= M43_Init;
    drvP->exit			= M43_Exit;
    drvP->read			= M43_Read;
    drvP->write			= M43_Write;
    drvP->blockRead		= M43_BlockRead;
    drvP->blockWrite	= M43_BlockWrite;
    drvP->setStat		= M43_SetStat;
    drvP->getStat		= M43_GetStat;
    drvP->irq			= M43_Irq;
    drvP->info			= M43_Info;
}


/******************************** M43_Init ***********************************
 *
 *  Description:  Allocates and initialize the LL structure
 *				  Decodes descriptor
 *				  Reads and checks the ID if in descriptor enabled
 *				  Initialize the hardware
 *					- reset module (switch the relays off)
 *                The driver supports 8 digital outputs (relays),
 *				  the relay status can be read out
 *
 *                Deskriptor key		Default			Range/Unit
 *				  ------------------------------------------------------------
 *                DEBUG_LEVEL           OSS_DBG_DEFAULT   see dbg.h
 *                DEBUG_LEVEL_DESC      OSS_DBG_DEFAULT   see dbg.h
 *                ID_CHECK              1                 0..1
 *
 *---------------------------------------------------------------------------
 *  Input......:  descSpec		descriptor specifier
 *                osHdl			pointer to the os specific structure
 *                maHdl			access handle
 *								(in simplest case module base address)
 *                devSemHdl		device semaphore for unblocking in wait
 *                irqHdl		irq handle for mask and unmask interrupts
 *                llHdlP 		pointer to the variable where low level driver
 *								handle will be stored
 *
 *  Output.....:  llHdlP		pointer to low level driver handle
 *                return		0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_Init(
    DESC_SPEC		*descSpec,
    OSS_HANDLE      *osHdl,
    MACCESS			*maHdl,
    OSS_SEM_HANDLE  *devSem,
    OSS_IRQ_HANDLE  *irqHdl,
    LL_HANDLE		**llHdlP)
{
    M43_HANDLE		*m43Hdl;
    int32			status;
    u_int32			gotsize;
	DESC_HANDLE		*descHdlP;
    int32			modIdMagic;
    int32			modId;
    u_int32			value;

	/*----------------------------+
	| initialize the LL structure |
	+-----------------------------*/
	/* get memory for the LL structure */
    m43Hdl = (M43_HANDLE*) OSS_MemGet(
					osHdl, sizeof(M43_HANDLE), &gotsize );
	if (m43Hdl == NULL)	return ERR_OSS_MEM_ALLOC;

	/* set LL handle to the LL structure */
	*llHdlP = (LL_HANDLE*)m43Hdl;

	/* store data into the LL structure */
    m43Hdl->ownMemSize	= gotsize;
    m43Hdl->osHdl		= osHdl;
    m43Hdl->ma			= *maHdl;
    m43Hdl->devSem		= devSem;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	 /* drivers ident function */
	 m43Hdl->idFuncTbl.idCall[0].identCall = Ident;
	 /* libraries ident functions */
	 m43Hdl->idFuncTbl.idCall[1].identCall = DESC_Ident;
	 m43Hdl->idFuncTbl.idCall[2].identCall = OSS_Ident;
	 /* terminator */
	 m43Hdl->idFuncTbl.idCall[3].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

	/*--------------------------------+
	| get data from the LL descriptor |
	+--------------------------------*/
	/* init descHdl */
	status = DESC_Init(	descSpec, osHdl, &descHdlP );
	if (status) {
		DBGEXIT((&DBH));
		OSS_MemFree( osHdl, (char*)m43Hdl, gotsize);
		return status;
	}

    /* get DEBUG_LEVEL_DESC */
    status = DESC_GetUInt32( descHdlP, OSS_DBG_DEFAULT, &value,
				"DEBUG_LEVEL_DESC");
    if ( (status!=0) && (status!=ERR_DESC_KEY_NOTFOUND) ) {
        DESC_Exit( &descHdlP );
		DBGEXIT((&DBH));
		OSS_MemFree( osHdl, (char*)m43Hdl, gotsize);
        return status;
	}
	DESC_DbgLevelSet(descHdlP, value);	/* set level */

	/* get DEBUG_LEVEL */
    status = DESC_GetUInt32( descHdlP, OSS_DBG_DEFAULT, &(m43Hdl->dbgLevel),
                "DEBUG_LEVEL");
    if ( (status!=0) && (status!=ERR_DESC_KEY_NOTFOUND) ) {
        DESC_Exit( &descHdlP );
		DBGEXIT((&DBH));
		OSS_MemFree( osHdl, (char*)m43Hdl, gotsize);
        return status;
    }

    DBGWRT_1((DBH, "LL - M43_Init\n"));

	/* get ID_CHECK  */
    status = DESC_GetUInt32( descHdlP, 1, &(m43Hdl->useId), "ID_CHECK");
    if ( (status!=0) && (status!=ERR_DESC_KEY_NOTFOUND) ) {
        DESC_Exit( &descHdlP );
		DBGEXIT((&DBH));
		OSS_MemFree( osHdl, (char*)m43Hdl, gotsize);
        return status;
    }

	/* exit descHdl */
	status = DESC_Exit( &descHdlP );
	if (status) {
		DBGEXIT((&DBH));
		OSS_MemFree( osHdl, (char*)m43Hdl, gotsize);
		return status;
	}

	if(m43Hdl->useId){
		/*--------------------------------+
		| check module Id				  |
		+--------------------------------*/
		modIdMagic = m_read((U_INT32_OR_64)m43Hdl->ma, 0);
		modId      = m_read((U_INT32_OR_64)m43Hdl->ma, 1);

		if( modIdMagic != MOD_ID_MAGIC ) {
			DBGWRT_ERR((DBH,"*** LL - M43_Init: illegal magic=0x%04x\n", modIdMagic));
			status = ERR_LL_ILL_ID;
			DBGEXIT((&DBH));
			OSS_MemFree( osHdl, (char*)m43Hdl, gotsize);
			return status;
		}

		if( modId != MOD_ID ) {
			DBGWRT_ERR((DBH,"*** LL - M43_Init: illegal id=%d\n", modId));
			status = ERR_LL_ILL_ID;
			DBGEXIT((&DBH));
			OSS_MemFree( osHdl, (char*)m43Hdl, gotsize);
			return status;
		}
	}

	/*--------------------------------+
	| initialize module				  |
	+--------------------------------*/
	/* reset all channels */
	MWRITE_D16(m43Hdl->ma, M43_CTRL_REG, 0);

    return 0;

}/*M43_init*/

/****************************** M43_Exit *************************************
 *
 *  Description:  Deinitializes the hardware (switch the relays off)
 *				  Frees allocated memory
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdlP		pointer to low level driver handle
 *
 *  Output.....:  llHdlP		NULL
 *                return		0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_Exit(
	LL_HANDLE	**llHdlP)
{
    M43_HANDLE*     m43Hdl = (M43_HANDLE*) *llHdlP;
	int32			status;

    DBGWRT_1((DBH, "LL - M43_Exit\n"));

	/* reset all channels */
	MWRITE_D16(m43Hdl->ma, M43_CTRL_REG, 0);

	DBGEXIT((&DBH));
	/* free memory for the LL structure */
    status = OSS_MemFree( m43Hdl->osHdl, (int8*) m43Hdl, m43Hdl->ownMemSize );

    return(status);

}/*M43_exit*/


/****************************** M43_Read *************************************
 *
 *  Description:  Reads input state from current channel.
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl		pointer to low-level driver data structure
 *                ch		current channel
 *
 *  Output.....:  valueP	read value
 *                return	0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_Read(
	LL_HANDLE	*llHdl,
    int32		ch,
    int32		*valueP)
{
    M43_HANDLE*       m43Hdl = (M43_HANDLE*) llHdl;

    DBGWRT_1((DBH, "LL - M43_Read: ch=%d\n",ch));

	*valueP = ( (MREAD_D16(m43Hdl->ma, M43_CTRL_REG)) >> ch ) & 0x01;

    return(0);

}/*M43_read*/


/****************************** M43_Write ************************************
 *
 *  Description:  Write ouput state to current channel.
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl		pointer to ll-drv data structure
 *                ch		current channel
 *                value		switch on/off
 *
 *  Output.....:  return	0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_Write(
	LL_HANDLE	*llHdl,
	int32		ch,
	int32		value)
{
    M43_HANDLE*       m43Hdl = (M43_HANDLE*) llHdl;

    DBGWRT_1((DBH, "LL - M43_Write: ch=%d\n",ch));

	if (value)
		MSETMASK_D16(m43Hdl->ma, M43_CTRL_REG, (1 << ch) );
	else
		MCLRMASK_D16(m43Hdl->ma, M43_CTRL_REG, (1 << ch) );

    return(0);

}/*M43_write*/


/******************************** M43_SetStat *******************************
 *
 *  Description:  Changes the device state.
 *
 *		common codes			values			meaning
 *		---------------------------------------------------------------------
 *		M_LL_DEBUG_LEVEL        see dbg.h       enable/disable debug output
 *      M_LL_CH_DIR             M_CH_INOUT      channel direction
 *
 *
 *		M43 specific codes		values          meaning
 *		---------------------------------------------------------------------
 *		-						-				-
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl			pointer to low-level driver data structure
 *                code			setstat code
 *                ch			current channel (0..8)
 *                value32_or_64	setstat value or pointer to blocksetstat data
 *
 *  Output.....:  return	0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_SetStat(
	LL_HANDLE	*llHdl,
	int32		code,
	int32		ch,
	INT32_OR_64 value32_or_64 )
{
	int32			value	= (int32)value32_or_64;	/* 32bit value     */
	INT32_OR_64		valueP  = value32_or_64;		/* stores 32/64bit pointer */

    M43_HANDLE*		m43Hdl = (M43_HANDLE*) llHdl;

    DBGWRT_1((DBH, "LL - M43_SetStat: ch=%d code=0x%04x value=0x%x\n",
			  ch,code,value));

    switch(code)
    {
		/* set debug level */
        case M_LL_DEBUG_LEVEL:
            m43Hdl->dbgLevel = value;
            break;

		/* set channel direction */
		case M_LL_CH_DIR:
			if( value != M_CH_INOUT )
				return ERR_LL_ILL_DIR;
			break;

		/* unknown setstat */
        default:
            return ERR_LL_UNK_CODE;
    }

    return(0);

}/*M43_setstat*/


/******************************** M43_GetStat *******************************
 *
 *  Description:  Gets the device state.
 *
 *		common codes			values			meaning
 *		---------------------------------------------------------------------
 *		M_LL_CH_NUMBER			8				number of channels
 *
 *		M_LL_CH_DIR				M_CH_INOUT		direction of curr ch
 *                                               always inout
 *
 *		M_LL_CH_LEN				1				channel length in bit
 *
 *		M_LL_CH_TYP				M_CH_BINARY		channel type
 *
 *		M_LL_ID_CHECK			0 or 1			check EEPROM-Id from module
 *
 *		M_LL_DEBUG_LEVEL        see dbg.h       current debug level
 *
 *      M_LL_ID_SIZE            128             eeprom size [bytes]
 *
 *		M_LL_BLK_ID_DATA		-               eeprom raw data
 *
 *		M_MK_BLK_REV_ID			pointer to the ident function table
 *
 *
 *		M43 specific codes		values          meaning
 *		---------------------------------------------------------------------
 *		-						-				-
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl				pointer to low-level driver data structure
 *                code				getstat code
 *                ch				current channel (0..7)
 *
 *
 *  Output.....:  value32_or_64P	pointer to getstat value or
 *							 		pointer to blocksetstat data
 *                return		0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_GetStat(
	LL_HANDLE	*llHdl,
	int32		code,
	int32		ch,
	INT32_OR_64 *value32_or_64P)
{
    M43_HANDLE*	m43Hdl		= (M43_HANDLE*)llHdl;
    int32		*valueP		= (int32*)value32_or_64P;		/* pointer to 32bit value  */
	INT32_OR_64	*value64P	= value32_or_64P;		 		/* stores 32/64bit pointer  */
	M_SG_BLOCK	*blk 		= (M_SG_BLOCK*)value32_or_64P; 	/* stores block struct pointer */


    DBGWRT_1((DBH, "LL - M43_GetStat: ch=%d code=0x%04x\n",
			  ch,code));

    switch(code)
    {
		/* get number of channels */
		case M_LL_CH_NUMBER:
			*valueP = CH_NUMBER;
			break;

		/* get channel direction */
		case M_LL_CH_DIR:
			*valueP = M_CH_INOUT;
			break;

		/* get channel length in bit */
		case M_LL_CH_LEN:
			*valueP = 1;
			break;

		/* get channel type */
		case M_LL_CH_TYP:
			*valueP = M_CH_BINARY;
			break;

		/* get setting of descriptor key 'ID_CHECK' */
        case M_LL_ID_CHECK:
            *valueP = m43Hdl->useId;
            break;

		/* get debug level */
        case M_LL_DEBUG_LEVEL:
            *valueP = m43Hdl->dbgLevel;
            break;

		/* get IRQ counter */
		case M_LL_IRQ_COUNT:
			return ERR_LL_UNK_CODE;
			break;

		/* id prom size */
        case M_LL_ID_SIZE:
            *valueP = MOD_ID_SIZE;
            break;

		/* get ID data */
		case M_LL_BLK_ID_DATA:
		{
			u_int32 n;
			u_int16 *dataP = (u_int16*)blk->data;

			if (blk->size < MOD_ID_SIZE)		/* check buf size */
				return(ERR_LL_USERBUF);

			for (n=0; n<MOD_ID_SIZE/2; n++)		/* read MOD_ID_SIZE/2 words */
				*dataP++ = (int16)m_read((U_INT32_OR_64)m43Hdl->ma, (int8)n);

			break;
		}

		/* get revision ID */
		case M_MK_BLK_REV_ID:
			*value64P = (INT32_OR_64)&m43Hdl->idFuncTbl;
			break;

		/* unknown getstat */
        default:
            return ERR_LL_UNK_CODE;
    }

    return(0);

}/*M43_getstat*/


/***************************** M43_BlockRead ********************************
 *
 *  Description:  Read channels 0..('size'-1) of module into 'buf'.
 *                Channels will be read in rising order (0..size-1).
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl			pointer to low-level driver data structure
 *                ch			current channel (always ignored)
 *                buf			buffer to store read values
 *                size			number of bytes to read (1..8)
 *
 *  Output.....:  nbrRdBytesP	number of read bytes (0..8)
 *                return		0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_BlockRead(
     LL_HANDLE	*llHdl,
     int32		ch,
     void		*buf,
     int32		size,
	 int32		*nbrRdBytesP)
{
    M43_HANDLE*			m43Hdl = (M43_HANDLE*)llHdl;
 	register u_int8		*bufptr = (u_int8*)buf;
    register u_int16	value;
	int32				i;

    DBGWRT_1((DBH, "LL - M43_BlockRead: ch=%d, size=%d\n",ch,size));

	*nbrRdBytesP = 0;

 	/* read all channels */
	value = MREAD_D16(m43Hdl->ma, M43_CTRL_REG);

	/* error if size > CH_NUMBER */
	if(size > CH_NUMBER)
		return ERR_LL_ILL_PARAM;

	/* expand bits -> 'size' bytes */
	for(i=0; i<size; i++){
		*(bufptr + i) = value & 0x01;
		value >>= 1;
	}

	*nbrRdBytesP = size;

	return 0;

}/*M43_block_read*/


/***************************** M43_BlockWrite *******************************
 *
 *  Description:  Output values in 'buf' to channels 0..('size'-1) of module.
 *                Channels will be written in rising order (0..size-1).
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl			pointer to low-level driver data structure
 *                ch			current channel (always ignored)
 *                buf			buffer where output values are stored
 *				  size			number of bytes to write (1..8)
 *
 *  Output.....:  nbrWrBytesP	number of written bytes (0..8)
 *				  return		0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_BlockWrite(
     LL_HANDLE	*llHdl,
     int32		ch,
     void		*buf,
     int32		size,
 	 int32		*nbrWrBytesP
)
{
    M43_HANDLE*  m43Hdl = (M43_HANDLE*)llHdl;
 	register u_int8		*bufptr = (u_int8*)buf;
    register u_int8		value = 0;
	int32				i;

    DBGWRT_1((DBH, "LL - M43_BlockWrite: ch=%d, size=%d\n",ch,size));

	*nbrWrBytesP = 0;

	/* error if size > CH_NUMBER */
	if(size > CH_NUMBER)
		return ERR_LL_ILL_PARAM;

	/* compress size bytes -> 'size' bits */
	for(i=0; i<size; i++){
		value |= ( (*(bufptr+i)) & 0x01 ) << i;
	}

	/* write all channels */
	MWRITE_D16(m43Hdl->ma, M43_CTRL_REG, value);

	*nbrWrBytesP = size;

	return 0;

}/*M43_block_write*/


/******************************** M43_Irq ***********************************
 *
 *  Description:  (unused - the module have no interrupt )
 *
 *---------------------------------------------------------------------------
 *  Input......:  llHdl  pointer to ll-drv data structure
 *
 *  Output.....:  return LL_IRQ_DEV_NOT
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 M43_Irq(
   LL_HANDLE	*llHdl)
{
    DBGCMD( M43_HANDLE     *m43Hdl = (M43_HANDLE*) llHdl; )

    IDBGWRT_1((DBH, "LL - M43_Irq:\n"));

	/* not my interrupt */
	return LL_IRQ_DEV_NOT;

}/*M43_irq_c*/


/****************************** M43_Info ************************************
 *
 *  Description:  Gets low level driver info.
 *
 *                NOTE: can be called before M33_Init().
 *
 *  supported info type codes		value          meaning
 *  ------------------------------------------------------------------------
 *  LL_INFO_HW_CHARACTER
 *   arg2  u_int32 *addrModeChar	MDIS_MA08		M-Module characteristic
 *   arg3  u_int32 *dataModeChar	MDIS_MD08		M-Module characteristic
 *
 *  LL_INFO_ADDRSPACE_COUNT
 *     arg2  u_int32 *nbrOfMaddr	1				number of address spaces
 *
 *  LL_INFO_ADDRSPACE
 *     arg2  u_int32 addrNr			0				current address space
 *     arg3  u_int32 *addrMode		MDIS_MA08		used address mode
 *     arg4  u_int32 *dataMode		MDIS_MD16		used data mode
 *     arg5  u_int32 *addrSize		0x100			needed size
 *
 *  LL_INFO_IRQ
 *     arg2  u_int32 *useIrq		0				module use no interrupt
 *
 *  LL_INFO_LOCKMODE
 *     arg2  u_int32 *lockModeP		LL_LOCK_CALL		used lock mode
 *
 *---------------------------------------------------------------------------
 *  Input......:  infoType			desired information
 *                ...				variable argument list
 *
 *  Output.....:  return			0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
int32 M43_Info
(
   int32  infoType,
   ...
)
{
	va_list		argptr;

	va_start(argptr,infoType);

	switch( infoType )
	{
		/* module characteristic */
		case LL_INFO_HW_CHARACTER:
		{
			u_int32 *addrModeChar = va_arg( argptr, u_int32* );
			u_int32 *dataModeChar = va_arg( argptr, u_int32* );

			*addrModeChar = MDIS_MA08;
			*dataModeChar = MDIS_MD08;

			break;
		}

		/* number of address spaces */
		case LL_INFO_ADDRSPACE_COUNT:
		{
			u_int32 *nbrOfMaddr = va_arg( argptr, u_int32* );

			*nbrOfMaddr = ADDRSPACE_COUNT;

			break;
		}

		/* info about address space */
		case LL_INFO_ADDRSPACE:
		{
			u_int32 addrNr = va_arg( argptr, u_int32 );
			u_int32 *addrMode = va_arg( argptr, u_int32* );
			u_int32 *dataMode = va_arg( argptr, u_int32* );
			u_int32 *addrSize = va_arg( argptr, u_int32* );

			if( addrNr == 0 ){
				*addrMode = MDIS_MA08;
				*dataMode = MDIS_MD16;
				*addrSize = 0x100;
			}
			else {
				va_end( argptr );
				return ERR_LL_ILL_PARAM;
			}

			break;
		}

		/* use no interrupt */
		case LL_INFO_IRQ:
		{
			u_int32 *useIrq = va_arg( argptr, u_int32* );

			*useIrq = 0;
			break;
		}
                /* process lock mode */
                case LL_INFO_LOCKMODE:
                {
                        u_int32 *lockModeP = va_arg(argptr, u_int32*);

                        *lockModeP = LL_LOCK_CALL;
                        break;
                }

		/* error */
		default:
			va_end( argptr );
			return ERR_LL_UNK_CODE;
	}

	/* all was ok */
	va_end( argptr );
	return 0;

}/*M43_info*/

/*******************************  Ident  ************************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  return  ptr to ident string
 *
 *  Globals....:  -
 ****************************************************************************/
static char* Ident( void )
{
    return( (char*)IdentString );
}





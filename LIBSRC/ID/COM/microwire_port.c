/*********************  P r o g r a m  -  M o d u l e **********************/
/*!
 *         \file microwire_port.c
 *      Project: ID LIB
 *
 *       \author uf
 *        $Date: 2008/09/18 17:28:57 $
 *    $Revision: 1.10 $
 *
 *        \brief Microwire bus protocol library for a port emulation.
 *
 *				 This libary don't exclude multiple access.
 *
 *     Required: oss
 *     Switches: none
 *
 *		   Note: Only D16 access implemented.
 *               Only all control/data bits in one register implemented.
 */
 /*-------------------------------[ History ]---------------------------------
 *
 * $Log: microwire_port.c,v $
 * Revision 1.10  2008/09/18 17:28:57  CKauntz
 * R1: No doxygen documentation
 * R2: No support for 64 bit OS
 * R3: m_read and m_write double in documentation
 * M1: Changed comments for doxygen documentation
 * M2: Changed base to U_INT32_OR_64 to support 64 bit OS
 * M3: Renamed m_read to m_read_loc and m_write to m_write_loc
 *
 * Revision 1.9  2008/09/09 15:46:14  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 1.8  2003/01/13 11:15:22  dschmidt
 * _clock(): cast added
 *
 * Revision 1.7  2000/09/07 15:39:37  Schmidt
 * nodoc for non-public functions added
 *
 * Revision 1.6  2000/04/04 13:29:37  Schmidt
 * casts added
 *
 * Revision 1.5  2000/03/16 16:10:09  kp
 * now including "id_var.h" for swapped variant
 *
 * Revision 1.4  1999/12/02 13:44:24  Schmidt
 * _MSC_VER switch: inline replaced with __inline
 *
 * Revision 1.3  1999/08/06 09:37:01  kp
 * _READ and _WRITE macros caused name clashes with OS-9's stdio.h,
 * renamed to _READ_ and _WRITE_
 *
 * Revision 1.2  1999/08/02 10:47:28  Schoberl
 * added casts
 *
 * Revision 1.1  1999/07/30 14:23:16  Franke
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999..2008 by MEN mikro elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
static const char RCSid[]="MCRW - MicroWire Port Lib $Id: microwire_port.c,v 1.10 2008/09/18 17:28:57 CKauntz Exp $";

#include "id_var.h"
#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/maccess.h>

#define MCRW_COMPILE
#include <MEN/microwire.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
typedef struct
{
	/* function entries */
	MCRW_ENTRIES entries;

	/* data */
	u_int32        ownSize;
	OSS_HANDLE 	   *osHdl;
	MCRW_DESC_PORT desc;
	u_int32		   outDefault; /* if all DATA out in one register */
}MCRW_HANDLE;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+-----------------------------------------*/
/*--- instructions for serial EEPROM ---*/
#define     _READ_   0x80    /* read data */
#define     EWEN    0x30    /* enable erase/write state */
#define     ERASE   0xc0    /* erase cell */
#define     _WRITE_  0x40    /* write data */
#define     ERAL    0x20    /* chip erase */
#define     WRAL    0x10    /* chip write */
#define     EWDS    0x00    /* disable erase/write state */

#define     T_WP    10000   /* max. time required for write/erase (us) */

/* bit definition */
#define B_DAT	0x01				/* data in-;output		*/
#define B_CLK	0x02				/* clock				*/
#define B_SEL	0x04				/* chip-select			*/


#ifdef _UCC
/* Ultra-C has no inline funcs */
# define __inline__
#endif
#ifdef _MSC_VER
/* VC has no __inline__ funcs */
# define __inline__  __inline
#endif

/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
/* none */
/*-----------------------------------------+
|  STATICS                                 |
+-----------------------------------------*/
/* none */
/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
static char* mcrwIdent       ( void );
static int32 mcrwExit        ( MCRW_HANDLE **mcrwHdlP );
static int32 mcrwWriteEeprom ( MCRW_HANDLE *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size );
static int32 mcrwReadEeprom  ( MCRW_HANDLE *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size );
static int32 mcrwSetStat	 ( MCRW_HANDLE *mcrwHdl, int32 code,  int32 data   );
static int32 mcrwGetStat	 ( MCRW_HANDLE *mcrwHdl, int32 code,  int32 *dataP );
static u_int16 m_read_loc    ( MCRW_HANDLE *mcrwHdl, void *base, u_int8 index );
static int m_write_loc       ( MCRW_HANDLE *mcrwHdl, void *base, u_int8  index, u_int16 data );

/*****************************  mcrwIdent  *********************************/
/** Gets the pointer to ident string.
 *
 *
 *---------------------------------------------------------------------------
 * \return  pointer to ident string
 *
 ****************************************************************************/
static char* mcrwIdent( void )
{
	return( (char*)RCSid );
}/*mcrwIdent*/

/************************************* delay *******************************/
/** Delay	descriptor dependend time.
 *
 *		   Note:  busClock descriptor value \n
 *					 0 - max speed - no delay\n
 *					 1 -  1kHz  OSS_Delay()\n
 *					10 - 10kHz  OSS_MikroDelay()
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl	\IN pointer to mcrw handle
 *
 ****************************************************************************/
__inline__ static void delay	
(
	MCRW_HANDLE  *mcrwHdl
)
{
	switch( mcrwHdl->desc.busClock )
	{
		case 1:  /* 1kHz max */
			OSS_Delay( mcrwHdl->osHdl, 1 );
			break;
		case 10: /* 10kHz max */
			OSS_MikroDelay( mcrwHdl->osHdl, 100 );
			break;
		default: /* max speed */
			break;
	}/*switch*/
}/*delay*/


/*----------------------------------------------------------------------
 * LOW-LEVEL ROUTINES FOR SERIAL EEPROM
 *--------------------------------------------------------------------*/

/******************************* _select ***********************************/
/** Select EEPROM:
 *                 output DI/CLK/CS low
 *                 delay
 *                 output CS high
 *                 delay
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN handle pointer
 *  \param base			\IN base address
 *
 ***************************************************************************/
static void _select(MCRW_HANDLE  *mcrwHdl, void *base )	
{
    MWRITE_D16( base, 0, (0 | mcrwHdl->outDefault) );			/* everything inactive */
    delay(mcrwHdl);
    MWRITE_D16( base, 0, (B_SEL | mcrwHdl->outDefault) );		/* select high */
    delay(mcrwHdl);
}

/******************************* _deselect *********************************/
/** Deselect EEPROM
 *                 output CS low
 *---------------------------------------------------------------------------
 *	\param mcrwHdl		\IN MCRW handle pointer
 *	\param base			\IN base address pointer
 ***************************************************************************/
static void _deselect(MCRW_HANDLE  *mcrwHdl, void *base )	
{
    MWRITE_D16( base, 0, (0 | mcrwHdl->outDefault) );			/* everything inactive */
}


/******************************* _clock ***********************************/
/**   Output data bit:
 *                 output clock low
 *                 output data bit
 *                 delay
 *                 output clock high
 *                 delay
 *                 return state of data serial eeprom's DO - line
 *                 (Note: keep CS asserted)
 *---------------------------------------------------------------------------
 *	\param mcrwHdl		\IN MCRW handle pointer
 *	\param base			\IN base address pointer
 *	\param dbs			\IN data bit set
 *	\return state of DO line
 *
 ***************************************************************************/
static int _clock(MCRW_HANDLE  *mcrwHdl, void *base, u_int8 dbs )	
{
    MWRITE_D16( base, 0, dbs|B_SEL| (u_int16)mcrwHdl->outDefault );  /* output clock low */
                                            /* output data high/low */
    delay(mcrwHdl);                               /* delay    */

    MWRITE_D16( base, 0, dbs|B_CLK|B_SEL| (u_int16)mcrwHdl->outDefault );  /* output clock high */
    delay(mcrwHdl);                               /* delay    */

    return( MREAD_D16( base, 0) & B_DAT );  /* get data */
}


/*----------------------------------------------------------------------
 * HIGH-LEVEL ROUTINES FOR SERIAL EEPROM
 *--------------------------------------------------------------------*/
/******************************* _opcode ***********************************/
/**   Output opcode with leading startbit
 *
 *---------------------------------------------------------------------------
 *	\param mcrwHdl		\IN MCRW handle pointer
 *	\param base			\IN base address pointer
 *	\param code			\IN operation code
 *
 ***************************************************************************/
static void _opcode(MCRW_HANDLE  *mcrwHdl, void *base, u_int8 code )	
{
    register int i;

    _select(mcrwHdl, base);
    _clock(mcrwHdl, base,1);                         /* output start bit */

    for(i=7; i>=0; i--)
        _clock(mcrwHdl, base,(u_int8)((code>>i)&0x01));        /* output instruction code  */
}

/******************************* _write ***********************************/
/**   Write a specified word into EEPROM at 'base'.
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\param base			\IN base address pointer
 *	\param index		\IN index to write (0..63)
 *	\param data			\IN word to write
 *	\return 0=ok 1=write err 2=verify err
 *
 ***************************************************************************/
static int _write(MCRW_HANDLE  *mcrwHdl, void *base, u_int8 index, u_int16 data )	
{
    register int    i,j;                    /* counters     */

    _opcode(mcrwHdl, base,EWEN);                     /* write enable */
    _deselect(mcrwHdl, base);                        /* deselect     */

    _opcode(mcrwHdl, base, (u_int8)(_WRITE_+index) );             /* select write */
    for(i=15; i>=0; i--)
        _clock(mcrwHdl, base,(u_int8)((data>>i)&0x01));        /* write data   */
    _deselect(mcrwHdl, base);                        /* deselect     */

    _select(mcrwHdl, base);
    for(i=T_WP; i>0; i--)                   /* wait for low */
    {   if(!_clock(mcrwHdl, base,0))
            break;
        delay(mcrwHdl);
    }
    for(j=T_WP; j>0; j--)                   /* wait for high*/
    {   if(_clock(mcrwHdl, base,0))
            break;
        delay(mcrwHdl);
    }

    _opcode(mcrwHdl, base, EWDS);                    /* write disable*/
    _deselect(mcrwHdl, base);                        /* disable      */

    if((i==0) || (j==0))                    /* error ?      */
        return MCRW_ERR_WRITE;                           /* ..yes */

    if( data != m_read_loc(mcrwHdl, base,index) )        /* verify data  */
        return MCRW_ERR_WRITE_VERIFY;                           /* ..error      */

    return 0;                               /* ..no         */
}

/******************************* _erase ***********************************/
/**   Erase a specified word into EEPROM
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\param base			\IN base address pointer
 *	\param index		\IN index to erase (0..15)
 *  \return   0=ok 1=error
 *
 ***************************************************************************/
static int _erase(MCRW_HANDLE  *mcrwHdl, void *base, u_int8 index )	
{
    register int    i,j;                    /* counters     */

    _opcode(mcrwHdl, base,EWEN);                     /* erase enable */
    for(i=0;i<4;i++) _clock(mcrwHdl, base,0);
    _deselect(mcrwHdl, base);                        /* deselect     */

    _opcode(mcrwHdl, base,(u_int8)(ERASE+index) );              /* select erase */
    _deselect(mcrwHdl, base);                        /* deselect     */

    _select(mcrwHdl, base);
    for(i=T_WP; i>0; i--)                   /* wait for low */
    {   if(!_clock(mcrwHdl, base,0))
            break;
        delay(mcrwHdl);
    }

    for(j=T_WP; j>0; j--)                   /* wait for high*/
    {   if(_clock(mcrwHdl, base,0))
            break;
        delay(mcrwHdl);
    }

    _opcode(mcrwHdl, base,EWDS);                     /* erase disable*/
    _deselect(mcrwHdl, base);                        /* disable      */

    if((i==0) || (j==0))                    /* error ?      */
        return MCRW_ERR_ERASE;
    return 0;
}

/******************************* m_read_loc ********************************/
/**   Read a specified word from EEPROM at 'base'.
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\param base			\IN base address pointer
 *	\param index		\IN index to read
 *  \return   read word
 *
 ****************************************************************************/
static u_int16 m_read_loc(MCRW_HANDLE  *mcrwHdl, void *base, u_int8 index )	
{
    register u_int16    wx;                 /* data word    */
    register int        i;                  /* counter      */

    _opcode(mcrwHdl,base, (u_int8)(_READ_+index) );
    for(wx=0, i=0; i<16; i++)
        wx = (u_int16)((wx<<1)+_clock(mcrwHdl,base,0));
    _deselect(mcrwHdl,base);

    return(wx);
}

/******************************* m_write_loc *******************************/
/**   Write a specified word into EEPROM at 'base'.
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\param base			\IN base address pointer
 *	\param index		\IN index to write
 *  \param data			\IN word to write
 *  \return   readed word
 *
 ***************************************************************************/
static int m_write_loc(MCRW_HANDLE  *mcrwHdl, void *base, u_int8  index, u_int16 data )	
{
    if( _erase(mcrwHdl, base, index ))              /* erase cell first */
        return( MCRW_ERR_ERASE );

    return _write(mcrwHdl, base, index, data );
}

/*****************************  mcrwWriteEeprom  ********************************/
/**   Not implemented - because not tested.
 *
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\pram addr			\IN byte address 0..0xFE (must be word aligned)
 *	\param buf			\IN write buffer
 *  \param size			\IN in byte must be multiple of 2
 *  \return   0 or error code
 *	
 ****************************************************************************/
static int32 mcrwWriteEeprom( MCRW_HANDLE *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size )
{
int32 error;
int   wordCount;

	/*--------------------+
	| parameter checking  |
	+--------------------*/
	/* check buffer is word aligned */
	if( (INT32_OR_64)buf%2 )
		return( MCRW_ERR_BUF );
	/* check addr is multiple of 2 and not to big */
	if( addr%2 || addr > 0xFE )
		return( MCRW_ERR_ADDR );
	/* check size is multiple of 2 and not to big */
	if( size%2 || size > 0x100 || (addr+size) > 0x100 )
		return( MCRW_ERR_BUF_SIZE );

	addr = addr/2;

	/*-----------+
	| read loop  |
	+-----------*/
	for( wordCount=0; wordCount<(size/2); wordCount++ )
	{
		 error = m_write_loc( mcrwHdl, mcrwHdl->desc.addrDataIn,
						  (u_int8)(addr+wordCount), *buf++ );
		 if( error )
		 	return( error );
	}/*for*/

	return( MCRW_ERR_NO );
}/*mcrwWriteEeprom*/

/*****************************  mcrwReadEeprom  ********************************/
/**   Reads <size>/2 words from EEPROM.
 *
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\param addr			\IN byte address 0..0xFE (must be multiple of 2)
 *	\param buf			\IN read buffer (must be word aligned)
 *  \param size			\IN in byte 0..0x100 (must be multiple of 2)
 *                         ( the maximum buffer size is depend on the
 *                           EEPROM type )
 *  \return   0 or error code
 *	
 ****************************************************************************/
static int32 mcrwReadEeprom( MCRW_HANDLE *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size )
{
int wordCount;

	/*--------------------+
	| parameter checking  |
	+--------------------*/
	/* check buffer is word aligned */
	if( (INT32_OR_64)buf%2 )
		return( MCRW_ERR_BUF );
	/* check addr is multiple of 2 and not to big */
	if( addr%2 || addr > 0xFE )
		return( MCRW_ERR_ADDR );
	/* check size is multiple of 2 and not to big */
	if( size%2 || size > 0x100 || (addr+size) > 0x100 )
		return( MCRW_ERR_BUF_SIZE );

	addr = addr/2;

	/*-----------+
	| read loop  |
	+-----------*/
	for( wordCount=0; wordCount<(size/2); wordCount++ )
	{
		*buf++ = m_read_loc( mcrwHdl, mcrwHdl->desc.addrDataIn,
						(u_int8)(addr+wordCount) );
	}/*for*/

	return( MCRW_ERR_NO );
}/*mcrwReadEeprom*/


/*****************************  mcrwGetStat  ********************************/
/**   Getstat - not supported.
 *
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\param code			\IN getstat code
 *	\param dataP		\IN pointer to variable where value will be stored
 *
 *  \return   0 or error code
 *
 ****************************************************************************/
static int32 mcrwGetStat( MCRW_HANDLE *mcrwHdl, int32 code,  int32 *dataP )
{
	return( MCRW_ERR_UNK_CODE );
}/*mcrwGetStat*/
/*****************************  mcrwSetStat  ********************************/
/**   Setstat - not supported.
 *
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdl		\IN MCRW handle pointer
 *	\param code			\IN setstat code
 *	\param data			\IN setstat value
 *	\return   0 or error code
 *
 ****************************************************************************/
static int32 mcrwSetStat( MCRW_HANDLE *mcrwHdl, int32 code,  int32 data   )
{
	return( MCRW_ERR_UNK_CODE );
}/*mcrwSetStat*/

/****************************** MCRW_PORT_Init ****************************/
/**   Initializes this library and check's the MCRW host.
 *
 *---------------------------------------------------------------------------
 *  \param descP		\IN pointer to MCRW descriptor
 *  \param osHdl		\IN OS specific handle
 *  \param mcrwHdlP		\IN pointer to variable where the handle will be stored
 *	\return    0 | error code
 *
 ****************************************************************************/
u_int32 MCRW_PORT_Init
(
    MCRW_DESC_PORT	*descP,
    void		 	*osHdl,
	void		 	**mcrwHdlP
)
{
u_int32     error  = 0;
MCRW_HANDLE  *mcrwHdl;
u_int32		gotSize;

	*mcrwHdlP = NULL;

	/*---------------------+
	|  check descriptor	   |
	+---------------------*/
	if(		/* check access size */
		   ( descP->busClock != 0 )
		&& ( descP->busClock != 1 )
		&& ( descP->busClock != 10 )
	  )
	{
		error = MCRW_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if(		/* check access size */
		   ( descP->addrLength < 6 )
		|| ( descP->addrLength > 8 )
	  )
	{
		error = MCRW_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if(		/* check access size */
		   !( descP->flagsDataIn	& MCRW_DESC_PORT_FLAG_SIZE_MASK )
		|| !( descP->flagsDataOut 	& MCRW_DESC_PORT_FLAG_SIZE_MASK )
		|| !( descP->flagsClockOut	& MCRW_DESC_PORT_FLAG_SIZE_MASK )
		|| !( descP->flagsCsOut	    & MCRW_DESC_PORT_FLAG_SIZE_MASK )
	  )
	{
		error = MCRW_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if(		/* read registers should be readable */
		   !( descP->flagsDataIn 	& MCRW_DESC_PORT_FLAG_READABLE_REG )
	  )
	{
		error = MCRW_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	if(	/* write registers are in one register ? */
		( descP->flagsOut & MCRW_DESC_PORT_FLAG_OUT_IN_ONE_REG )
	  )
	{
		
		/* then the addresses must be equal */
		if( descP->addrDataIn != descP->addrDataOut
			|| descP->addrDataIn != descP->addrCsOut
		  )
		{
			error = MCRW_ERR_DESCRIPTOR;
			goto CLEANUP;
		}/*if*/
	}
	else
	{
		/* on M-Modules the bits are in one register
		   the lib is not prepared for non M-Module EEPROM interface
		   due a lack of suitable test hardware
		   -> the error case can be removed after preparing test */
		error = MCRW_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/


	if(		/* mask bits has to be set */
		   !(descP->maskDataIn  )
		|| !(descP->maskDataOut	)
		|| !(descP->maskClockOut)
		|| !(descP->maskCsOut   )
	  )
	{
		error = MCRW_ERR_DESCRIPTOR;
		goto CLEANUP;
	}/*if*/

	/*---------------------+
	|  alloc structure	   |
	+---------------------*/
	mcrwHdl   = (MCRW_HANDLE*) OSS_MemGet( (OSS_HANDLE*) osHdl, sizeof(MCRW_HANDLE), &gotSize );
	if( mcrwHdl == NULL )
	{
	    error = MCRW_ERR_NO_MEM;
		goto CLEANUP;
	}/*if*/

	/*---------------------+
	|  init the structure  |
	+---------------------*/
	OSS_MemFill( (OSS_HANDLE*) osHdl, gotSize, (char*)mcrwHdl, 0 );

	mcrwHdl->desc 			    = *descP;
	mcrwHdl->ownSize  			= gotSize;
	mcrwHdl->osHdl    			= (OSS_HANDLE*) osHdl;

	if(	/* write registers are in one register ? */
		( descP->flagsOut & MCRW_DESC_PORT_FLAG_OUT_IN_ONE_REG )
	  )
	{
		/* merge the out defaults, if all outs in one register */
		mcrwHdl->desc.notReadBackDefaultsDataOut  &= mcrwHdl->desc.notReadBackDefaultsClockOut;
		mcrwHdl->desc.notReadBackDefaultsDataOut  &= mcrwHdl->desc.notReadBackDefaultsCsOut;
		mcrwHdl->desc.notReadBackDefaultsClockOut = mcrwHdl->desc.notReadBackDefaultsDataOut;
		mcrwHdl->desc.notReadBackDefaultsCsOut    = mcrwHdl->desc.notReadBackDefaultsDataOut;
		mcrwHdl->outDefault   = mcrwHdl->desc.notReadBackDefaultsDataOut;
	}/*if*/

	mcrwHdl->entries.Ident		= mcrwIdent;
	mcrwHdl->entries.Exit		= (int32 (*)(void **))mcrwExit;
	mcrwHdl->entries.WriteEeprom =
		(int32 (*)(void *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size)) mcrwWriteEeprom;
	mcrwHdl->entries.ReadEeprom =
		(int32 (*)(void *mcrwHdl, u_int8 addr, u_int16 *buf, u_int16 size)) mcrwReadEeprom;
	mcrwHdl->entries.SetStat =
		(int32 (*)(void *mcrwHdl, int32 code,  int32 data)) mcrwSetStat;
	mcrwHdl->entries.GetStat =
		(int32 (*)(void *mcrwHdl, int32 code,  int32 *dataP)) mcrwGetStat;


	/* set the handle */
	*mcrwHdlP = (void*) mcrwHdl;

CLEANUP:
	return( error );
}/*MCRW_PORT_Init*/

/*******************************  mcrwExit  ********************************/
/**   Deinitializes this library and MCRW controller.
 *
 *---------------------------------------------------------------------------
 *  \param mcrwHdlP		\IN	pointer to variable where the handle is stored
 *	\return    always 0
 *
 ****************************************************************************/
static int32 mcrwExit
(
	MCRW_HANDLE **mcrwHdlP
)
{
u_int32 error  = 0;
MCRW_HANDLE  *mcrwHdl;

	mcrwHdl = *mcrwHdlP;

	*mcrwHdlP = NULL;

    OSS_MemFree( mcrwHdl->osHdl, mcrwHdl, mcrwHdl->ownSize );

	return( error );
}/*mcrwExit*/





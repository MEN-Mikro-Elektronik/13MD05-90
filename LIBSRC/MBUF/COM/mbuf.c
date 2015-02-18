/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: mbuf.c
 *      Project: MDIS 4.0 / LL Driver
 *
 *       Author: uf
 *        $Date: 2010/04/21 16:51:11 $
 *    $Revision: 1.18 $
 *
 *  Description: read/write buffer handling and
 *               signaling to application and
 *               internal synchronisation
 *
 *     Required:
 *     Switches:
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *  MBUF_Ident             Gets the pointer to ident string.
 *  MBUF_Create            Allocates memory for the buffer structure and
 *  MBUF_Remove            Frees the allocated memory MBUF_HANDLE and buffer.
 *  MBUF_Write             Writes size of bytes to a buffer in LL-Drv setblock.
 *  MBUF_Read              Reads size of bytes from a buffer in LL-Drv
 *  MBUF_GetNextBuf        Gets the in or out buffer in LL-drv ISR.
 *  MBUF_ReadyBuf          Signals end of block rd or wr access to buffer in
 *  MBUF_EventHdlrInst     Installs buffer event handler
 *  MBUF_SetStat           Handles set and blocksetstats to the read or write
 *  MBUF_GetStat           Handles get and blockgetstats to the read or write
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mbuf.c,v $
 * Revision 1.18  2010/04/21 16:51:11  CRuff
 * R:1. Porting to MDIS5 (according to porting guide rev. 0.9)
 * M:1. convert format of pointer debug prints to 0x08p
 *
 * Revision 1.17  2008/12/18 10:27:03  CKauntz
 * R: MBUF_Remove() wrong error handling at bufHdl->buf
 * M: fixed invalid freeing of bufHdl->buf
 *
 * Revision 1.16  2008/09/19 09:50:22  ufranke
 * R: MBUF_Read() wrong return value introduced in revision 1.15
 * M: fixed
 *
 * Revision 1.15  2008/09/09 15:17:02  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 1.14  2006/12/15 14:11:54  ts
 * Cosmetics
 *
 * Revision 1.13  2006/05/26 13:29:17  DPfeuffer
 * OSS_IrqMask/Unmask() replaced by OSS_IrqMaskR/Restore (due to W2k SMP support)
 *
 * Revision 1.12  2001/01/19 14:48:41  kp
 * cosmetics
 *
 * Revision 1.11  1999/05/04 14:36:43  Schoberl
 * MBUF_SetStat/GetStat : BUGFIX uses now the right buffer handle
 * : checks status code for buffer direction
 *
 * Revision 1.10  1999/04/27 14:26:13  kp
 * mark all static functions with nodoc
 *
 * Revision 1.9  1999/03/29 16:22:14  kp
 * Allow device semaphore to be NULL (this is the case for spMDIS for OS-9)
 * Only release and claim devSem if it's valid
 *
 * Revision 1.8  1998/11/26 16:15:02  Schmidt
 * MBUF_GetNextBuf : *gotsizeP was 1 to high on return
 * for(gotsize=1; gotsize<=size; gotsize++) replaced with
 * for(gotsize=0; gotsize<size;)
 *
 * Revision 1.7  1998/11/17 11:38:23  Schmidt
 * MBUF_SetStat : M_BUF_xx_SIGxxx_xxx : OSS_IrqMask / OSS_IrqUnMask removed
 *
 * Revision 1.6  1998/10/02 17:19:55  see
 * MBUF_Read/Write: renamed remainder to error
 * MBUF_RdWrSemWait header: wrong return values fixed
 * some headers: wrong MBUF_xxx names changed to M_BUF_xxx
 * bug fixed: MBUF_SetTimeout: allow timeout=0
 * MBUF_RdWrSemWait: long type changed to int32
 * MBUF_Create, MBUF_SetTimeout: set timeout to OSS_SEM_WAITFOREVER if msec=0
 * MBUF_Create, MBUF_SetTimeout: return ERR_MBUF_ILL_PARAM if msec<0
 * MBUF_SetStat, MBUF_GetStat: check code via M_BUF_CODE macro
 * bugs fixed: all functions: DBGWRT was done before handle check
 *
 * Revision 1.5  1998/06/29 11:00:34  Schmidt
 * include <MEN/dbg.h> moved before oss.h
 *
 * Revision 1.4  1998/06/15 17:44:52  Franke
 * changed DBG_ALL => OSS_DBG_DEFAULT
 *
 * Revision 1.3  1998/05/29 16:59:06  see
 * replaced "..." with "\n" in some LEV1 messages
 *
 * Revision 1.2  1998/04/17 15:42:49  see
 * header: public functions added
 * global IdentString removed, is now returned directly in MBUF_Ident
 * DBG macros updated, include dbg.h
 * MBUF_HANDLE: moved from mbuf.h to here, dbgHdl added
 * MBUF_HANDLE: debug handle dbgHdl added
 * MBUF_RdWrSemWait: passed bufHdl instead dbg_level for DBG macros usage
 * DBGWRT: all: prefix 'MBUF' added
 * DBGWRT: wrong prefixes 'LL' and 'CL' renamed to 'MBUF'
 * RCSid must be a const char array
 *
 * Revision 1.1  1998/03/10 12:21:22  franke
 * Added by mcvs
 *
 * Revision 1.6  1998/02/04 13:55:07  ds
 * add #include <MEN/...>
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: mbuf.c,v 1.18 2010/04/21 16:51:11 CRuff Exp $";

#include "MEN/men_typs.h"
#include <MEN/dbg.h>
#include "MEN/oss.h"
#include "MEN/mdis_err.h"
#include "MEN/mdis_api.h"

/* pass debug definitions to dbg.h */
#define DBG_MYLEVEL		bufHdl->dbgLevel

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
typedef struct {              /* i/o buffer data:              */
	int32   ownMemSize;
	u_int32 dbgLevel;
	int32   bufferMode;
	int32   direction;
	u_int32 lowHighWater;
	u_int8  *buf;             /* buffer location               */
	u_int32 size;             /* buffer size [bytes]           */
	u_int32 gotSize;          /* real buffer size [bytes]      */
	u_int32 width;            /* buffer width [bytes]          */
	u_int8  *fill;            /* next write pointer            */
	u_int8  *empty;           /* next read pointer             */
	u_int8  *end;             /* end of buffer                 */
	u_int32 timeout;          /* sleep timeout [msec] (0=endless) */
	u_int32 errorEnabled;
	u_int32 errorOccured;
	u_int32 errorCount;
	u_int32 count;            /* entries in buffer [entries]   */
	u_int32 ev_flags;         /* event handler flags           */
	                          /* event handler function        */
	int32   (*ev_handler)( void* lldrvHdl, u_int32 ev_flags);
	OSS_SIG_HANDLE  *sigHdl;
	void*           lldrvHdl;
	OSS_SEM_HANDLE  *bufSem;
	OSS_SEM_HANDLE  *devSem;        /* for unblocking device in wait */
	OSS_IRQ_HANDLE  *irqHdl;
	OSS_HANDLE*     osHdl;         /* for complicated os */
	DBG_HANDLE*     dbgHdl;        /* for complicated os */
} MBUF_HANDLE;

/* say: don't use MBUF_HANDLE type */
#define _NO_MBUF_HANDLE
/* MBUF_HANDLE needs to be declared BEFORE including mbuf.h */
#include "MEN/mbuf.h"

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/* debug handle */
#define DBH		bufHdl->dbgHdl

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int32 MBUF_BufReset( MBUF_HANDLE    *bufHdl,
                            OSS_IRQ_HANDLE *irqHdl,
                            int32          clear );


static int32 MBUF_RdWrSemWait(
   OSS_HANDLE         *osHdl,
   OSS_SEM_HANDLE*    RdOrWrSemDat,
   int32               msec,
   OSS_SEM_HANDLE*    devSemDat,
   OSS_IRQ_HANDLE*    irqHdl,
   MBUF_HANDLE*       bufHdl,
   OSS_IRQ_STATE	 *oldIrqStateP);


/*****************************  MBUF_Ident  *********************************
 *
 *  Description:  Gets the pointer to ident string.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  return  pointer to ident string
 *
 *  Globals....:  -
 ****************************************************************************/
char* MBUF_Ident( void )
{
	return("MBUF - Buffer Manager: $Id: mbuf.c,v 1.18 2010/04/21 16:51:11 CRuff Exp $");
}/*MBUF_Ident*/


/****************************  MBUF_Create **********************************
 *
 *  Description:  Allocates memory for the buffer structure and initializes
 *                the structure. Allocates also a buffer with size bytes and
 *                stores the osHdl and the devSem.
 *
 *                system requirements:
 *                   one semaphore per buffer
 *
 *                NOTE: size should be multiple of width
 *
 *---------------------------------------------------------------------------
 *  Input......:  osHdl        pointer to a os specific struct
 *                devSem       device semaphore handle for blocking
 *                size         whole buffer size [bytes]
 *                width        buffer width [bytes]
 *                mode         M_BUF_CURRBUF | M_BUF_RINGBUF
 *                             | M_BUF_RINGBUF_OVERWR (only read direction)
 *                direction    buffer to create MBUF_RD | MBUF_WR
 *                lowHighWater buffer low/highwater mark (0..size)
 *                timeout      buffer timeout [msec] (0=endless)
 *
 *
 *  Output.....:  bufHdlP      pointer to created buffer handle
 *                return       0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
int32 MBUF_Create
(
    OSS_HANDLE      *osHdl,
    OSS_SEM_HANDLE  *devSem,
    void*           lldrvHdl,
    int32           size,
    int32           width,
    int32           mode,
    int32           direction,
    int32           lowHighWater,
    int32           timeout,
    OSS_IRQ_HANDLE  *irqHdl,
    MBUF_HANDLE     **bufHdlP
)
{
    int32        retCode;
    int32        fktRetCode;
    u_int32      gotSize;
    u_int32      realBufSize;
    MBUF_HANDLE* bufHdl = 0;
    int32        depth;

    retCode = 0;
    bufHdl = 0;
    *bufHdlP = 0;

	/* check params */
	if (timeout < 0)
        return( ERR_MBUF_ILL_PARAM );

    if( size < 1 )
    {
        return( ERR_MBUF_ILL_SIZE );
    }/*if*/

    depth = size / width;

    /* round up */
    size = (int32) (width * depth);

    if( mode == M_BUF_RINGBUF_OVERWR && direction == MBUF_WR )
    {
       return( ERR_MBUF_ILL_DIR );
    }/*if*/

	/* endless timeout: prepare value for SemWait */
	if (timeout == 0)
        timeout = OSS_SEM_WAITFOREVER;

    bufHdl = (MBUF_HANDLE*) OSS_MemGet( osHdl, sizeof(MBUF_HANDLE), &gotSize );
    *bufHdlP = bufHdl;


    if( bufHdl != NULL )
    {
        /* fill turkey with 0 */
        OSS_MemFill( osHdl, gotSize, (char*) bufHdl, 0 );

        bufHdl->ownMemSize   = gotSize;
        bufHdl->size         = size;
        bufHdl->width        = width;
        bufHdl->timeout      = timeout;
        bufHdl->errorEnabled = 0;
        bufHdl->errorOccured = 0;
        bufHdl->errorCount   = 0;
        bufHdl->count        = 0;
        bufHdl->ev_flags     = 0;
        bufHdl->ev_handler   = 0;
        bufHdl->bufferMode   = mode;
        bufHdl->direction    = direction;
        bufHdl->lowHighWater = lowHighWater;
        bufHdl->sigHdl    = 0;
        bufHdl->lldrvHdl  = lldrvHdl;
        bufHdl->devSem       = devSem;
        bufHdl->irqHdl    = irqHdl;
        bufHdl->osHdl        = osHdl;

		/* prepare debugging */
		DBG_MYLEVEL = OSS_DBG_DEFAULT;
		DBGINIT((NULL,&DBH));

		DBGWRT_1((DBH,"MBUF - MBUF_Create: mode=%d dir=%s size=%d\n",
				  mode, direction==MBUF_RD ? "RD":"WR",size));

        bufHdl->buf          = (u_int8*) OSS_MemGet( osHdl, size, &realBufSize );
        if( bufHdl->buf != NULL )
        {

            bufHdl->gotSize     = realBufSize;
            bufHdl->fill        = bufHdl->buf;
            bufHdl->empty       = bufHdl->buf;
            bufHdl->end         = bufHdl->buf + (size-1);

            fktRetCode = OSS_SemCreate( osHdl,
                                        OSS_SEM_BIN,
                                        0,                     /* is empty */
                                        &bufHdl->bufSem );
            if( fktRetCode != 0 )
            {
                MBUF_Remove( &bufHdl );
                retCode = fktRetCode;
            }/*if*/
        }
        else
        {
            MBUF_Remove( &bufHdl );
            retCode = ERR_OSS_MEM_ALLOC;
        }/*if*/
    }
    else
    {
        retCode = ERR_OSS_MEM_ALLOC;
    }/*if*/

    return( retCode );
}/*MBUF_Create*/

/****************************  MBUF_Remove  *********************************
 *
 *  Description:  Frees the allocated memory MBUF_HANDLE and buffer.
 *                Remove the created buffer semaphore and installed signal.
 *                Set the handle pointer to 0.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdlP   pointer to created buffer handle
 *
 *  Output.....:  bufHdlP   0
 *                return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
int32 MBUF_Remove
(
    MBUF_HANDLE** bufHdlP
)
{
    int32        retCode;
    int32        fktRetCode;
    MBUF_HANDLE* bufHdl;

    bufHdl = *bufHdlP;

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_Remove\n") );
	DBGEXIT((&DBH));

    retCode = OSS_SemRemove( bufHdl->osHdl, &bufHdl->bufSem );

    if( bufHdl->sigHdl )
    {
        fktRetCode = OSS_SigRemove( bufHdl->osHdl,
                                    &bufHdl->sigHdl );
        if( !retCode && fktRetCode )
        {
           /* no error before but now */
           retCode = fktRetCode;
        }/*if*/
    }/*if*/

	if( bufHdl->buf )
	{
		fktRetCode = OSS_MemFree( bufHdl->osHdl, bufHdl->buf, bufHdl->gotSize );

		if( !retCode && fktRetCode )
		{
		   /* no error before but now */
		   retCode = fktRetCode;
		}/*if*/
	}/*if*/

    fktRetCode = OSS_MemFree( bufHdl->osHdl,
                              (u_int8*)bufHdl,
                              bufHdl->ownMemSize );
    if( !retCode && fktRetCode )
    {
       /* no error before but now */
       retCode = fktRetCode;
    }/*if*/

    return( retCode );
}/*MBUF_Remove*/

/****************************  MBUF_Write  ************************************
 *
 *  Description:  Writes size of bytes to a buffer in LL-Drv setblock.
 *
 *                In M_BUF_RINGBUF mode the fill pointer and count counters
 *                are incremented.
 *                Checks the underrun error, if error enabled clear it and
 *                return with error code.
 *                If buffer full in M_BUF_RINGBUF mode it gives the isr a
 *                chance to make space.
 *                   unmask IRQ
 *                   release devSem (other routines then M_setblock can work)
 *                   claim   bufSem  (is empty first, IRQ release or timeout)
 *                   claim   devSem
 *                   mask   IRQ
 *                   check  timeout or signal occured -> error
 *                   check  is space now, if not do it again
 *
 *                NOTE: Size should be multiple of width in M_BUF_RINGBUF mode
 *                      or equal width in M_BUF_CURRBUF mode.
 *                      Buffer direction should be MBUF_DIR_OUT.
 *                      BufferMode should be M_BUF_CURRBUF | M_BUF_RINGBUF.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                buffer      source buffer
 *                size        nbr of bytes to write
 *
 *  Output.....:  nbrWrBytesP nbr of written bytes
 *                return      0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
int32 MBUF_Write
(
    MBUF_HANDLE*    bufHdl,
    u_int8*         buffer,
    u_int32         size,
    int32*          nbrWrBytesP
)
{
    register u_int8* usrbuf = buffer;   /* point to user buffer */
    int32            request;
    int32            error;
	OSS_IRQ_STATE	 oldIrqState;

    *nbrWrBytesP = 0;

	error = ERR_MBUF_ILL_PARAM;
    if( bufHdl == NULL )
    {
		error = ERR_MBUF_NO_BUF;
    	goto CLEANUP;
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_Write\n") );

    if( bufHdl->direction == MBUF_RD )
    {
       return( ERR_MBUF_ILL_DIR );
    }/*if*/

    switch( bufHdl->bufferMode )
    {
		case M_BUF_CURRBUF:

         if( bufHdl->width > size )            /* buf too small ? */
             return(ERR_MBUF_USERBUF);

         oldIrqState = OSS_IrqMaskR( bufHdl->osHdl, bufHdl->irqHdl ); /* DISABLE irqs */
         OSS_MemCopy( bufHdl->osHdl, bufHdl->width, (char*) usrbuf, (char*) bufHdl->buf );/* put data */
         OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState ); /* ENABLE irqs */
         *nbrWrBytesP = bufHdl->width;
         return( 0 );
         break;

       case M_BUF_RINGBUF:
       case M_BUF_RINGBUF_OVERWR:
         if( bufHdl->width > size )            /* buf too small ? */
             return(ERR_MBUF_USERBUF);

         request = size / bufHdl->width;      /* user requested entrys */

         DBGWRT_1((DBH," ----- buf=%08p end=%08p, usrbuf=%08p:\n",
              bufHdl->buf, bufHdl->end, usrbuf ) );

         while(request)                      /* while entries requested */
         {
             oldIrqState = OSS_IrqMaskR( bufHdl->osHdl, bufHdl->irqHdl ); /* DISABLE irqs */

             DBGWRT_1((DBH," req=%ld, fill=%08p empty=%08p cnt=%ld\n",
                  request,bufHdl->fill,bufHdl->empty,bufHdl->count) );

             if( bufHdl->errorOccured )        /* underrun occured ? */
                 if( bufHdl->errorEnabled )
                 {  /* handling enabled ? */
                     bufHdl->errorOccured = 0;  /* clear flag */
					 /* ENABLE irqs */
                     OSS_IrqRestore( bufHdl->osHdl,
									 bufHdl->irqHdl, oldIrqState );
					
                     return(ERR_MBUF_UNDERRUN);
                 }/*if*/

             /*--------------------------+
             | any space in the buffer ? |
             +--------------------------*/
             if( bufHdl->count < bufHdl->size )
             {
                 OSS_MemCopy( bufHdl->osHdl, bufHdl->width, (char*) usrbuf, (char*)bufHdl->fill);
                 bufHdl->fill += bufHdl->width;    /* put data */
                 bufHdl->count+= bufHdl->width;
                 OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState ); /* ENABLE irqs */

                 if (bufHdl->fill > bufHdl->end)   /* at end of buf ? */
                     bufHdl->fill = bufHdl->buf;   /* wrap around */

                 usrbuf += bufHdl->width;           /* took data */
                 request--;

                 /*-------------------+
                 | new buffer data ?  |
                 +-------------------*/
                 if (bufHdl->count== bufHdl->width && /* new data ? */
                     (bufHdl->ev_flags & MBUF_EV_newdata))    /* MBUF_EV_newdata ? */
                     bufHdl->ev_handler( bufHdl->lldrvHdl, MBUF_EV_newdata );
             }
             /*--------------------------+
             | buffer is full ?          |
             +--------------------------*/
             else
             {
                 /* ENABLE irqs, sleep .., DISABLE irqs */
                 error = MBUF_RdWrSemWait(  bufHdl->osHdl,
                                                bufHdl->bufSem,
                                                bufHdl->timeout,
                                                bufHdl->devSem,
                                                bufHdl->irqHdl,
                                                bufHdl,
												&oldIrqState
                                             );

                 OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState ); /* ENABLE irqs */

                 if( error )    /* error ? */
                     return( error ); /* timeout */
             }/*if*/
         } /* while request */

         *nbrWrBytesP = (u_int32)(usrbuf-buffer);
         return( 0 );
         break;

       default:
         return( ERR_MBUF_ILL_PARAM );
    }/*if*/

CLEANUP:
    return( error );
}/*MBUF_Write*/

/*****************************  MBUF_Read  **********************************
 *
 *  Description:  Reads size of bytes from a buffer in LL-Drv getblock.
 *
 *                In M_BUF_RINGBUF mode the empty pointer is incremented and
 *                count counter is decremented.
 *                Checks the overflow error, if error enabled clear it and
 *                return with error code.
 *                If buffer empty in M_BUF_RINGBUF mode it gives the isr a
 *                chance to fill the buffer.
 *                   unmask IRQ
 *                   release devSem (other routines then M_setblock can work)
 *                   claim   bufSem  (is empty first, IRQ release or timeout)
 *                   claim   devSem
 *                   mask   IRQ
 *                   check  timeout or signal occured -> error
 *                   check  there are data now, if not do it again
 *
 *                NOTE: Size should be multiple of width.
 *                      Buffer direction should be MBUF_DIR_IN.
 *                      Buffer mode can be M_BUF_CURRBUF | M_BUF_RINGBUF
 *                      | M_BUF_RINGBUF_OVERWR.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                buffer      source buffer
 *                size        nbr of bytes to write
 *
 *
 *  Output.....:  nbrRdBytesP  nbr of read bytes
 *                return       0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
int32 MBUF_Read
(
    MBUF_HANDLE*    bufHdl,
    u_int8*         buffer,
    u_int32         size,
    int32*          nbrRdBytesP
)
{
    register u_int8* usrbuf = buffer;   /* point to user buffer */
    int32            request;
    int32            error;
	OSS_IRQ_STATE	 oldIrqState;

    *nbrRdBytesP = 0;

	error = ERR_MBUF_ILL_PARAM;
    if( bufHdl == NULL )
    {
       error = ERR_MBUF_NO_BUF;
       goto CLEANUP;
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_Read\n") );

    if( bufHdl->direction == MBUF_WR  )
    {
       return( ERR_MBUF_ILL_DIR );
    }/*if*/

    switch( bufHdl->bufferMode )
    {
       case M_BUF_CURRBUF:
         if( bufHdl->width > size )             /* buf too small ? */
             return(ERR_MBUF_USERBUF);
		 /* DISABLE irqs */	
         oldIrqState = OSS_IrqMaskR( bufHdl->osHdl, bufHdl->irqHdl );
         OSS_MemCopy( bufHdl->osHdl,
					  bufHdl->width,
					  (char*)bufHdl->buf, (char*)usrbuf);  /* took data */
		 /* ENABLE irqs */
         OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState );
         *nbrRdBytesP = bufHdl->width;
         return( 0 );
         break;

       case M_BUF_RINGBUF:
       case M_BUF_RINGBUF_OVERWR:
         if( bufHdl->width > size )            /* buf too small ? */
             return(ERR_MBUF_USERBUF);

         request = size / bufHdl->width;      /* user requested entrys */
         DBGWRT_1((DBH," ----- buf=%08p end=%08p, usrbuf=%08p:\n",
				   bufHdl->buf, bufHdl->end, usrbuf ) );

         while( request )  /* while entries requested */
         {
			 /* DISABLE irqs */
             oldIrqState = OSS_IrqMaskR( bufHdl->osHdl,
										 bufHdl->irqHdl );

             DBGWRT_1((DBH," req=%ld, fill=%08p empty=%08p cnt=%ld\n",
                  request,bufHdl->fill,bufHdl->empty,bufHdl->count) );

             if( bufHdl->errorOccured )         /* overflow occured ? */
                 if( bufHdl->errorEnabled )
                 {   /* handling enabled ? */
                     bufHdl->errorOccured = 0;   /* clear flag */
					 OSS_IrqRestore( bufHdl->osHdl,
									 bufHdl->irqHdl,
									 oldIrqState ); /* ENABLE irqs */
                     return(ERR_MBUF_OVERFLOW);
                 }

             /*--------------------------+
            | any data in the buffer ?  |
             +--------------------------*/
             if( bufHdl->count > 0 )
             {
                 OSS_MemCopy(bufHdl->osHdl,
							 bufHdl->width,
							 (char*)bufHdl->empty, (char*)usrbuf);
                 bufHdl->empty += bufHdl->width;     /* took data */
                 bufHdl->count -= bufHdl->width;
                 OSS_IrqRestore( bufHdl->osHdl,
								 bufHdl->irqHdl,
								 oldIrqState ); /* ENABLE irqs */

                 if (bufHdl->empty > bufHdl->end)    /* at end of buf ? */
                     bufHdl->empty = bufHdl->buf;    /* wrap around */

                 usrbuf += bufHdl->width;            /* put data */
                 request--;

                 /*----------------------+
                 | new buffer space ?    |
                 +-----------------------*/
                 /* new space ? */

				 /* MBUF_EV_newspace ? */
                 if( bufHdl->count == (bufHdl->size-bufHdl->width) &&
                     (bufHdl->ev_flags & MBUF_EV_newspace))
                 {
                     bufHdl->ev_handler( bufHdl->lldrvHdl, MBUF_EV_newspace );
                 }/*if*/
             }
             /*--------------------------+
             | buffer is empty ?         |
             +--------------------------*/
             else
             {
                 /* ENABLE irqs, sleep ..., DISABLE irqs */
                 error = MBUF_RdWrSemWait(  bufHdl->osHdl,
                                                bufHdl->bufSem,
                                                bufHdl->timeout,
                                                bufHdl->devSem,
                                                bufHdl->irqHdl,
                                                bufHdl,
												&oldIrqState
                                             );
				 /* ENABLE irqs */
                 OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState );

                 if( error )    /* error ? */
                     return( error ); /* timeout */
             }/*if*/
         } /* while request */

		 *nbrRdBytesP = (u_int32)(usrbuf-buffer);
         return( 0 );
         break;

       default:
         return( ERR_MBUF_ILL_PARAM );
    }/*if*/

CLEANUP:
    return( error );
}/*MBUF_Read*/

/*************************  MBUF_SetTimeout  *********************************
 *
 *  Description:  Set a new timeout value.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                timeout                            IN
 *
 *  Output.....:  return  0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_SetTimeout	/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32        timeout
)
{
	/* check params */
    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_SetTimeout\n") );

	if (timeout < 0)
        return( ERR_MBUF_ILL_PARAM );

	/* endless timeout: prepare value for SemWait */
	if (timeout == 0)
        timeout = OSS_SEM_WAITFOREVER;

	bufHdl->timeout      = timeout;

    return( ERR_SUCCESS );
}/*MBUF_SetTimeout*/

/*************************  MBUF_GetTimeout  *********************************
 *
 *  Description:  Get the timeout value.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl    buffer handle
 *
 *
 *  Output.....:  timeoutP  timeout in ms
 *                return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetTimeout	/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32*       timeoutP
)
{

    if( bufHdl == NULL )
    {
       *timeoutP = 0;
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetTimeout\n") );

	/* endless timeout: re-convert value */
	if (bufHdl->timeout == OSS_SEM_WAITFOREVER)
        *timeoutP = 0;
	else
		*timeoutP = bufHdl->timeout;

    return( 0 );
}/*MBUF_GetTimeout*/

/************************  MBUF_SetBufferMode  ******************************
 *
 *  Description:  Changes the buffer mode.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                mode   M_BUF_USRCTRL
 *                       | M_BUF_CURRBUF
 *                       | M_BUF_RINGBUF
 *                       | M_BUF_RINGBUF_OVERWR ( only direction == MBUF_RD )
 *
 *  Output.....:  0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_SetBufferMode	/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    OSS_IRQ_HANDLE *irqHdl,
    int32        mode
)
{
    int32 retCode;
	OSS_IRQ_STATE	oldIrqState;

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_SetBufferMode\n") );

    oldIrqState = OSS_IrqMaskR( bufHdl->osHdl, bufHdl->irqHdl ); /* DISABLE irqs */

    switch( mode )
    {
        case M_BUF_USRCTRL:
        case M_BUF_CURRBUF:
        case M_BUF_RINGBUF:
          bufHdl->bufferMode = mode;
          MBUF_BufReset( bufHdl, irqHdl, 0);
          retCode = 0;
          break;

        case M_BUF_RINGBUF_OVERWR:
          if( bufHdl->direction == MBUF_WR )
          {
               return( ERR_MBUF_ILL_DIR) ;
          }/*if*/
          bufHdl->bufferMode = mode;
          MBUF_BufReset( bufHdl, irqHdl, 0);
          retCode = 0;
          break;

        default:
          retCode = ERR_MBUF_ILL_PARAM;

    }/*switch*/
    OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState ); /* ENABLE irqs */

    return( retCode );
}/*MBUF_SetBufferMode*/

/***********************  MBUF_GetBufferMode  ****************************
 *
 *  Description:  Gets the buffer mode.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl    buffer handle
 *
 *
 *  Output.....:  modeP     buffer mode
 *                return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
int32 MBUF_GetBufferMode
(
    MBUF_HANDLE* bufHdl,
    int32*       modeP
)
{

    if( bufHdl == NULL )
    {
       *modeP = M_BUF_USRCTRL;           /* no buffer -> user ctrl */
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetBufferMode\n") );

    *modeP = bufHdl->bufferMode;

    return( 0 );
}/*MBUF_GetBufferMode*/

/***********************  MBUF_SetLowHighWater  ****************************
 *
 *  Description:  Sets the low or high water mark.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                lowHighWater                       IN
 *
 *  Output.....:  0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_SetLowHighWater /* nodoc */
(
    MBUF_HANDLE *bufHdl,
    u_int32     lowHighWater
)
{

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_SetLowHighWater\n") );

    if( bufHdl->size < lowHighWater )
    {
       return( ERR_MBUF_ILL_PARAM );
    }/*if*/

    bufHdl->lowHighWater = lowHighWater;

    return( 0 );
}/*MBUF_SetLowHighWater*/

/***********************  MBUF_GetLowHighWater  ****************************
 *
 *  Description:  Gets the low or high water mark.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *
 *
 *  Output.....:  lowHighWaterP  mark in byte
 *                return         0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetLowHighWater /* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32*       lowHighWaterP
)
{

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetLowHighWater\n") );

    *lowHighWaterP = bufHdl->lowHighWater;

    return( 0 );
}/*MBUF_GetLowHighWater*/

/*****************************  MBUF_GetNextBuf  ****************************
 *
 *  Description:  Gets the in or out buffer in LL-drv ISR.
 *
 *                for direction MBUF_RD in M_BUF_RINGBUF mode
 *                - if no space and error is enabled
 *                  sets the error flag and increments the error count
 *                - increment fill pointer + width
 *                - increment buffer count + width
 *
 *                for direction MBUF_RD in M_BUF_RINGBUF_OVERWR mode
 *                - if no space increment fill and empty pointer
 *                - increment fill pointer + width
 *                - increment buffer count + width
 *
 *                for direction MBUF_WR  in M_BUF_RINGBUF mode
 *                - if count <= 0 and error is enabled
 *                  sets the error flag and increments the error count
 *                - increment empty pointer + width
 *                - decrement buffer count - width
 *
 *                NOTE: -should only be called within irq service !
 *                      -*gotsizeP may be lesser then size if the end
 *                       of ring buffer is reached, then call again
 *                      -unit of size IS NOT BYTE!
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                size      in width
 *
 *
 *  Output.....:  gotsizeP  in width
 *                return
 *                   0 no buffer availible
 *                   start of buffer      in M_BUF_CURRBUF mode
 *                   next buffer          in M_BUF_RINGBUF mode
 *                   next buffer          in M_BUF_RINGBUF_OVERWR mode
 *
 *  Globals....:  -
 ****************************************************************************/
void* MBUF_GetNextBuf
(
    MBUF_HANDLE  *bufHdl,
    int32        size,
    int32        *gotsizeP
)
{
    void* nextBuf;
    int32 gotsize;

    nextBuf = NULL;
    *gotsizeP = 0;

    if( bufHdl==NULL || bufHdl->buf==NULL )
    {
       return( 0 );
    }/*if*/

    IDBGWRT_1((DBH,"MBUF - MBUF_GetNextBuf: \n") );

    switch( bufHdl->bufferMode )
    {
        /*------------------------------------------------------------------+
        | M_BUF_CURRBUF - current buffer                                       |
        +------------------------------------------------------------------*/
        case M_BUF_CURRBUF:
            nextBuf = bufHdl->buf;                         /* start adr */
            break;
        /*------------------------------------------------------------------+
        | M_BUF_RINGBUF - ring buffer                                          |
        +------------------------------------------------------------------*/
        case M_BUF_RINGBUF:
        case M_BUF_RINGBUF_OVERWR:
            if( bufHdl->direction == MBUF_RD )
            {
                /*--------------------------+
                | buffer full - overrun ?   |
                +--------------------------*/
                if (bufHdl->count >= bufHdl->size)
                {
                    if( bufHdl->bufferMode != M_BUF_RINGBUF_OVERWR )
                    {
                        if (bufHdl->errorEnabled )     /* error enable ? */
                        {
                            bufHdl->errorOccured = MBUF_ERROR_OCCURED;   /* set overflow flag */
                            bufHdl->errorCount++;
                        }/*if*/
                    }
                    else
                    {
                        /* overwrite mode */
                        /* count is equal size */
                        /* the empty pointer is also incremented */

                        nextBuf = (void*)bufHdl->fill;              /* current entry */

                        for( gotsize = 0; gotsize < size; )
                        {
                            bufHdl->fill += bufHdl->width;          /* put data */
                            bufHdl->empty += bufHdl->width;
							gotsize++;

                            if( bufHdl->empty > bufHdl->end )        /* at end of buf ? */
                            {
                                bufHdl->empty = bufHdl->buf;         /* wrap around */
                            }/*if*/

                            if( bufHdl->fill > bufHdl->end )        /* at end of buf ? */
                            {
                                bufHdl->fill = bufHdl->buf;         /* wrap around */
                                break;
                            }/*if*/
                        }/*for*/

                        *gotsizeP = gotsize;

                        return( nextBuf );
                    }/*if*/
                    return(NULL);
                }/*if*/

                nextBuf = (void*)bufHdl->fill;              /* current entry */

                for( gotsize = 0; gotsize < size; )
                {
                    bufHdl->fill += bufHdl->width;          /* put data */
                    bufHdl->count+= bufHdl->width;
					gotsize++;

                    if( bufHdl->fill > bufHdl->end )        /* at end of buf ? */
                    {
                        bufHdl->fill = bufHdl->buf;         /* wrap around */
                        break;
                    }/*if*/
                }/*for*/

                *gotsizeP = gotsize;
            }/*if*/

            if( bufHdl->direction == MBUF_WR  )
            {
                /*--------------------------+
                | buffer empty - underrun ? |
                +--------------------------*/
                if( bufHdl->count <= 0 )
                {
                    if (bufHdl->errorEnabled )     /* error enable ? */
                    {
                        bufHdl->errorOccured = MBUF_ERROR_OCCURED;   /* set overflow flag */
                        bufHdl->errorCount++;
                    }/*if*/
                    return(NULL);
                }/*if*/

                nextBuf = (void*)bufHdl->empty;            /* current entry */

                for( gotsize = 0; gotsize < size; )
                {
                    bufHdl->empty += bufHdl->width;       /* took data */
                    bufHdl->count -= bufHdl->width;
					gotsize++;

                    if( bufHdl->empty > bufHdl->end )     /* at end of buf ? */
                    {
                        bufHdl->empty = bufHdl->buf;      /* wrap around */
                        break;
                    }
                }/*for*/

                *gotsizeP = gotsize;
            }/*if*/
            break;
        /*------------------------------------------------------------------+
        | (unknown modes)                                                   |
        +------------------------------------------------------------------*/
        default:
            nextBuf = NULL;
    }

    return(nextBuf);
}/*MBUF_GetNextBuf*/

/******************************  MBUF_ReadyBuf  ********************************
 *
 *  Description:  Signals end of block rd or wr access to buffer in LL-drv ISR.
 *                Ever:
 *                   - the bufSem semaphore is released (signaled)
 *                If lowHighWater is reached:
 *                   - a signal is sended if installed
 *                   - the event handler is called if installed
 *
 *                NOTE: should only be called within irq service !
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *
 *  Output.....:  0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
int32 MBUF_ReadyBuf(   MBUF_HANDLE* bufHdl )
{
    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    IDBGWRT_1((DBH,"MBUF - MBUF_ReadyBuf: \n") );

    if( bufHdl->direction == MBUF_RD )
    {
        /*------------------------------+
        | buffer is full now ?          |
        +------------------------------*/
        if( bufHdl->count >= bufHdl->lowHighWater )
        {
            if( bufHdl->sigHdl )                    /* SIG_infull ? */
            {
                OSS_SigSend( bufHdl->osHdl, bufHdl->sigHdl );  /* send signal  */
            }/*if*/

            if( bufHdl->ev_flags & MBUF_EV_full )       /* MBUF_EV_full ?    */
                bufHdl->ev_handler( bufHdl->lldrvHdl, MBUF_EV_full );
        }/*if*/
    }
    else
    {
        /* direction => out */

        if( bufHdl->count <= bufHdl->lowHighWater )
        {
            if( bufHdl->sigHdl )                    /* SIG_empty ? */
            {
                OSS_SigSend( bufHdl->osHdl, bufHdl->sigHdl );  /* send signal  */
            }/*if*/

            if( bufHdl->ev_flags & MBUF_EV_empty)          /* MBUF_EV_empty ?  */
                bufHdl->ev_handler( bufHdl->lldrvHdl, MBUF_EV_empty);
        }/*if*/
    }/*if*/

    /*------------------------------+
    | somebody waiting for data ?   |
    +------------------------------*/
    OSS_SemSignal( bufHdl->osHdl, bufHdl->bufSem );  /* wake it up */

    return(0);
}/*MBUF_ReadyBuf*/

/**************************** MBUF_EventHdlrInst **************************
 *
 *  Description:  Installs buffer event handler
 *                (already installed handler will be overwritten)
 *                Masks IRQ first and unmask after installing.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                handler    event handler function
 *                flags      event flags to service (MBUF_EV_empty,
 *                                                   MBUF_EV_full,
 *                                                   MBUF_EV_newspace,
 *                                                   MBUF_EV_newdata )
 *  Output.....:  0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
int32 MBUF_EventHdlrInst
(
    MBUF_HANDLE    *bufHdl,
    int32          (*handler)( void*    lldrvHdl
                              ,u_int32 ev_flags),
    int32          flags
)
{
	OSS_IRQ_STATE	oldIrqState;

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_EventHdlrInst : flags=%x \n",flags) );

    if( handler )
    {
		oldIrqState = OSS_IrqMaskR( bufHdl->osHdl, bufHdl->irqHdl ); /* DISABLE irqs */
        bufHdl->ev_flags   = flags;
        bufHdl->ev_handler = handler;
		OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState ); /* ENABLE irqs */
    }/*if*/

    return(0);
}/*MBUF_EventHdlrInst*/

/*************************** MBUF_SigInstRemove *****************************
 *
 *  Description:  Installs or removes signals for reaching lowHighWater mark.
 *                If install and mark is reached, a signal is sended.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                setstatCode   SIG_set_infull..SIG_clr_outempty
 *                signalNbr     signal nbr to install
 *
 *  Output.....:  0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
static int32 MBUF_SigInstRemove	/* nodoc */
(
    MBUF_HANDLE *bufHdl,
    int32       setstatCode,
    int32       signalNbr
)
{
    int32 fktRetCode;

    DBGWRT_1((DBH,"MBUF_SigInstRemove: \n") );
    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    switch( setstatCode )
    {
        case M_BUF_RD_SIGSET_HIGH:
           if( bufHdl->direction == MBUF_WR  )
           {
               return( ERR_MBUF_ILL_DIR ) ;
           }/*if*/
           if( bufHdl->sigHdl != 0 )
           {
               return( ERR_MBUF_ILL_PARAM );
           }/*if*/
           fktRetCode = OSS_SigCreate( bufHdl->osHdl,
                                       signalNbr,
                                       &bufHdl->sigHdl );

           if( bufHdl->count >= bufHdl->lowHighWater )
               OSS_SigSend( bufHdl->osHdl, bufHdl->sigHdl );

           if( fktRetCode != 0 )
               return( fktRetCode );
           break;

        case M_BUF_RD_SIGCLR_HIGH:
           if( bufHdl->direction == MBUF_WR  )
           {
               return( ERR_MBUF_ILL_DIR) ;
           }/*if*/
           fktRetCode = OSS_SigRemove( bufHdl->osHdl,
                                       &bufHdl->sigHdl );
           if( fktRetCode != 0 )
               return( fktRetCode );
           break;

        case M_BUF_WR_SIGSET_LOW:
           if( bufHdl->direction == MBUF_RD )
           {
               return( ERR_MBUF_ILL_DIR) ;
           }/*if*/
           if( bufHdl->sigHdl != 0 )
           {
               return( ERR_MBUF_ILL_PARAM );
           }/*if*/
           fktRetCode = OSS_SigCreate( bufHdl->osHdl,
                                       signalNbr,
                                       &bufHdl->sigHdl );

           if( bufHdl->count <= bufHdl->lowHighWater )
               OSS_SigSend( bufHdl->osHdl, bufHdl->sigHdl );

           if( fktRetCode != 0 )
               return( fktRetCode );
           break;

        case M_BUF_WR_SIGCLR_LOW:
           if( bufHdl->direction == MBUF_RD )
           {
               return( ERR_MBUF_ILL_DIR) ;
           }/*if*/
           fktRetCode = OSS_SigRemove( bufHdl->osHdl,
                                       &bufHdl->sigHdl );
           if( fktRetCode != 0 )
               return( fktRetCode );
           break;

        default:
           return( ERR_MBUF_UNK_CODE );
    }/*switch*/

    return( 0 );
}/*MBUF_SigInstRemove*/

/****************************** MBUF_BufReset *********************************
 *
 *  Description:  Resets and optionally clear specified buffer.
 *                Masks IRQ first and unmask after reseting.
 *
 *                in M_BUF_USRCTRL
 *                   M_BUF_RINGBUF
 *                   M_BUF_RINGBUF_OVERWR mode
 *                   count is set to 0
 *
 *                in M_BUF_CURRBUF
 *                   count is set to witdh
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                irqHdl                                 IN
 *                clear     clear buffer if 1               IN
 *
 *  Output.....:  0 | error code
 *
 *  Globals....:  ---
 ****************************************************************************/
static int32 MBUF_BufReset		/* nodoc */
(
    MBUF_HANDLE    *bufHdl,
    OSS_IRQ_HANDLE *irqHdl,
    int32          clear
)
{
	OSS_IRQ_STATE	oldIrqState;

    if( bufHdl==NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_BufReset buf=%08p \n", bufHdl->buf) );

    oldIrqState = OSS_IrqMaskR( bufHdl->osHdl, bufHdl->irqHdl ); /* DISABLE irqs */

    if (clear)
        OSS_MemFill( bufHdl->osHdl, bufHdl->size,
                     (char*) bufHdl->buf, 0x0);    /* clear physically */

    bufHdl->empty  = bufHdl->buf;
    bufHdl->fill   = bufHdl->buf;
    switch( bufHdl->bufferMode )
    {
        case M_BUF_USRCTRL:
        case M_BUF_RINGBUF:
        case M_BUF_RINGBUF_OVERWR:
          bufHdl->count  = 0;
          break;

        case M_BUF_CURRBUF:
        default:
          bufHdl->count  = bufHdl->width;

    }/*switch*/

    OSS_IrqRestore( bufHdl->osHdl, bufHdl->irqHdl, oldIrqState ); /* ENABLE irqs */

    return(0);
}/*MBUF_BufReset*/




/*********************  MBUF_EnableDisableError  ****************************
 *
 *  Description:  enable or disable underrun or overflow errors.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *                enable                                 IN
 *
 *  Output.....:  0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_EnableDisableError /* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32        enable
)
{

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_SetFlags\n") );

    if( enable )
        bufHdl->errorEnabled = MBUF_ERROR_ENABLED;
    else
        bufHdl->errorEnabled = MBUF_ERROR_DISABLED;

    return( 0 );
}/*MBUF_EnableDisableError*/

/**************************  MBUF_GetErrorFlag ******************************
 *
 *  Description:  Show if error occured.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl      buffer handle
 *
 *
 *  Output.....:  errorFlagP  error flag
 *                return      0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetErrorFlag	/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32*       errorFlagP
)
{

    if( bufHdl == NULL )
    {
       *errorFlagP = 0;
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetErrorFlag\n") );

    *errorFlagP = bufHdl->errorOccured;

    return( 0 );
}/*MBUF_GetError*/


/****************************  MBUF_GetErrorCount ****************************
 *
 *  Description:  Get the error count.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl       buffer handle
 *
 *
 *  Output.....:  errorCountP  error count
 *                return       0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetErrorCount	/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32*       errorCountP
)
{

    if( bufHdl == NULL )
    {
       *errorCountP = 0;
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetErrorCount\n") );

    *errorCountP = bufHdl->errorCount;

    return( 0 );
}/*MBUF_GetErrorCount*/


/*************************  MBUF_GetSize  *********************************
 *
 *  Description:  Get the size.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl    buffer handle
 *
 *
 *  Output.....:  sizeP     buffer size in byte
 *                return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetSize		/* nodoc */
(
    MBUF_HANDLE  *bufHdl,
    u_int32      *sizeP
)
{

    if( bufHdl == NULL )
    {
       *sizeP = 0;
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetSize\n") );

    *sizeP = bufHdl->size;

    return( 0 );
}/*MBUF_GetSize*/


/*************************  MBUF_GetCount  *********************************
 *
 *  Description:  Get the count value.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl    buffer handle
 *
 *
 *  Output.....:  countP    count
 *                return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetCount		/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32*       countP
)
{

    if( bufHdl == NULL )
    {
       *countP = 0;
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetCount\n") );

    *countP = bufHdl->count;

    return( 0 );
}/*MBUF_GetCount*/


/*************************  MBUF_GetWidth  *********************************
 *
 *  Description:  Get the width.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl    buffer handle
 *
 *
 *  Output.....:  widthP    width in byte
 *                return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetWidth		/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int32*       widthP
)
{

    if( bufHdl == NULL )
    {
       *widthP = 0;
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetWidth\n") );

    *widthP = bufHdl->width;

    return( 0 );
}/*MBUF_GetWidth*/


/*************************  MBUF_GetData  *********************************
 *
 *  Description:  Get the whole buffer data ( bufHdl->size bytes ).
 *
 *---------------------------------------------------------------------------
 *  Input......:  bufHdl    buffer handle
 *
 *
 *  Output.....:  dataP     buffer data
 *                return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 MBUF_GetData		/* nodoc */
(
    MBUF_HANDLE* bufHdl,
    int8*        dataP
)
{


    if( bufHdl == NULL )
    {
       *dataP = 0;
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH,"MBUF - MBUF_GetData\n") );

    OSS_MemCopy( bufHdl->osHdl, bufHdl->size, (char*) bufHdl->buf, (char*) dataP );

    return( 0 );
}/*MBUF_GetData*/

/*************************** MBUF_RdWrSemWait *********************************
 *
 *  Description:  Wait for specified semaphore(MDIS Read/Write) with timeout
 *                Interrupts must be masked before calling SemWait.
 *                After return, Interrupts are masked again.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  OSS_SEM_HANDLE*  RdOrWrSemDat,
 *                int32            msec,
 *                OSS_SEM_HANDLE*  devSemDat,
 *                OSS_IRQ_HANDLE*  irqHdl
 *                MBUF_HANDLE*     bufHdl
 *				  OSS_IRQ_STATE*   oldIrqStateP
 *
 *  Output.....:  ---
 *
 *  Return.....:  0 | error code
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int32 MBUF_RdWrSemWait	/* nodoc */
(
   OSS_HANDLE         *osHdl,
   OSS_SEM_HANDLE     *RdOrWrSemDat,
   int32              msec,
   OSS_SEM_HANDLE     *devSemDat,
   OSS_IRQ_HANDLE     *irqHdl,
   MBUF_HANDLE        *bufHdl,
   OSS_IRQ_STATE	  *oldIrqStateP
)
{
    u_int32       SemRet;
    u_int32       RetCode;

    DBGWRT_1((DBH,"MBUF - MBUF_RdWrSemWait\n" ) );

	OSS_IrqRestore( osHdl, irqHdl, *oldIrqStateP ); /* ENABLE irqs */

	if( devSemDat != NULL ){
		RetCode = OSS_SemSignal( osHdl, devSemDat ); /* release the device semaphore */
		if( RetCode )
		{
			DBGWRT_1((DBH," *** MBUF_RdWrSemWait: OSS_SemSignal() failed\n") );
		}/*if*/
	}

    SemRet = OSS_SemWait( osHdl, RdOrWrSemDat, msec );  /* sleep until tout or sig */
	if (SemRet)
	{
		DBGWRT_1((DBH," *** MBUF_RdWrSemWait: OSS_SemWait() error=%ld\n", SemRet));
	}

	if( devSemDat != NULL ){
		RetCode = OSS_SemWait( osHdl, devSemDat, OSS_SEM_WAITFOREVER );     /* gain access to device */
		if( RetCode )
		{
			DBGWRT_1((DBH," *** MBUF_RdWrSemWait: OSS_SemWait() failed\n") );
		}/*if*/
	}


	*oldIrqStateP = OSS_IrqMaskR( osHdl, irqHdl ); /* DISABLE irqs */

    return(SemRet);
}/*MBUF_RdWrSemWait*/


/***************************** MBUF_SetStat *********************************
 *
 *  Description:  Handles set and blocksetstats to the read or write buffer.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  rdBufHdl  read  buffer handle or NULL
 *                wrBufHdl  write buffer handle or NULL
 *                code      setstat code
 *                value     setstat value
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
int32 MBUF_SetStat
(
    MBUF_HANDLE *rdBufHdl,
    MBUF_HANDLE *wrBufHdl,
    int32       code,
    int32       value
)
{
    MBUF_HANDLE  *bufHdl=NULL;
    int32        retCode;

	/* read buffer mode ? */
    if ((((code) >= M_RDBUF_OF) && ((code) <= M_RDBUF_LAST) )  || \
	 (((code) >= M_RDBUF_BLK_OF) && ((code) <= M_RDBUF_BLK_LAST)))
	{
		if (rdBufHdl){
			if (rdBufHdl->direction == MBUF_RD)
				bufHdl = rdBufHdl;
			else return( ERR_MBUF_ILL_DIR );
		}
	}

	/* write buffer mode ? */
    if ((((code) >= M_WRBUF_OF) && ((code) <= M_WRBUF_LAST) )  || \
	 (((code) >= M_WRBUF_BLK_OF) && ((code) <= M_WRBUF_BLK_LAST)))
	{
		if (wrBufHdl){
			if (wrBufHdl->direction == MBUF_WR)
				bufHdl = wrBufHdl;
			else return( ERR_MBUF_ILL_DIR );
		}
	}

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH, "MBUF - MBUF_SetStat\n" )  );
    DBGWRT_2((DBH, "     code=$%04x\n",code )  );

    switch(code)
    {
        /*--------------------------+
        |  buffer mode              |
        +--------------------------*/
        case M_BUF_RD_MODE:
        case M_BUF_WR_MODE:
           return( MBUF_SetBufferMode( bufHdl,
                                       bufHdl->irqHdl,
                                       value )
                 );
           break;

        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_BUF_RD_DEBUG_LEVEL:
        case M_BUF_WR_DEBUG_LEVEL:
            bufHdl->dbgLevel = value;
            break;

        /*--------------------------+
        |   set/clr  sig in full    |
        +--------------------------*/
        case M_BUF_RD_SIGSET_HIGH:
        case M_BUF_RD_SIGCLR_HIGH:
           retCode =  MBUF_SigInstRemove( bufHdl, code, value );
           return( retCode );
           break;


        /*--------------------------+
        |   set/clr  sig out empty  |
        +--------------------------*/
        case M_BUF_WR_SIGSET_LOW:
        case M_BUF_WR_SIGCLR_LOW:
           retCode =  MBUF_SigInstRemove( bufHdl, code, value );
           return( retCode );
           break;


        case M_BUF_RD_ERR:
            return( MBUF_EnableDisableError( bufHdl, value ) );
        case M_BUF_RD_CLEAR:
            return( MBUF_BufReset( bufHdl, bufHdl->irqHdl, 1) );
        case M_BUF_RD_RESET:
            return( MBUF_BufReset( bufHdl, bufHdl->irqHdl, 0) );
        case M_BUF_RD_TIMEOUT:
            return( MBUF_SetTimeout( bufHdl, value ) );
        case M_BUF_RD_HIGHWATER:
            return( MBUF_SetLowHighWater( bufHdl, value ) );

        case M_BUF_WR_ERR:
            return( MBUF_EnableDisableError( bufHdl, value ) );
        case M_BUF_WR_CLEAR:
            return( MBUF_BufReset( bufHdl, bufHdl->irqHdl, 1) );
        case M_BUF_WR_RESET:
            return( MBUF_BufReset( bufHdl, bufHdl->irqHdl, 0) );
        case M_BUF_WR_TIMEOUT:
            return( MBUF_SetTimeout( bufHdl, value ) );
        case M_BUF_WR_LOWWATER:
            return( MBUF_SetLowHighWater( bufHdl, value ) );

        /*--------------------------+
        |  (unknown)                |
        +--------------------------*/
        default:

            return( ERR_MBUF_UNK_CODE );
    }/*switch*/

    return(0);
}/*MBUF_SetStat*/



/************************* MBUF_GetStat *************************************
 *
 *  Description:  Handles get and blockgetstats to the read or write buffer.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  rdBufHdl  read  buffer handle or NULL
 *                wrBufHdl  write buffer handle or NULL
 *                code      getstat code
 *                value     getstat value
 *
 *  Output.....:  return    0 | error code
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
int32 MBUF_GetStat
(
    MBUF_HANDLE  *rdBufHdl,
    MBUF_HANDLE  *wrBufHdl,
    int32        code,
    int32        *valueP
)
{
    MBUF_HANDLE  *bufHdl=NULL;
    u_int32      size;
    int32        processId;

	/* read buffer mode ? */
    if ((((code) >= M_RDBUF_OF) && ((code) <= M_RDBUF_LAST) )  || \
	 (((code) >= M_RDBUF_BLK_OF) && ((code) <= M_RDBUF_BLK_LAST)))
	{
		if (rdBufHdl){
			if (rdBufHdl->direction == MBUF_RD)
				bufHdl = rdBufHdl;
			else return( ERR_MBUF_ILL_DIR );
		}
	}

	/* write buffer mode ? */
    if ((((code) >= M_WRBUF_OF) && ((code) <= M_WRBUF_LAST) )  || \
	 (((code) >= M_WRBUF_BLK_OF) && ((code) <= M_WRBUF_BLK_LAST)))
	{
		if (wrBufHdl){
			if (wrBufHdl->direction == MBUF_WR)
				bufHdl = wrBufHdl;
			else return( ERR_MBUF_ILL_DIR );
		}
	}

    if( bufHdl == NULL )
    {
       return( ERR_MBUF_NO_BUF );
    }/*if*/

    DBGWRT_1((DBH, "MBUF - MBUF_GetStat\n" )  );
    DBGWRT_2((DBH, "     code=$%04x\n",code )  );

    switch(code)
    {
        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_BUF_RD_DEBUG_LEVEL:
        case M_BUF_WR_DEBUG_LEVEL:
            *valueP = bufHdl->dbgLevel;
            break;

        case M_BUF_RD_MODE:
        case M_BUF_WR_MODE:
            return( MBUF_GetBufferMode( bufHdl, valueP) );
            break;

        case M_BUF_BLK_RD_DATA:
            /* if( !bufHdl )  - already checked above
                return(ERR_MBUF_NO_BUF); */

            MBUF_GetSize( bufHdl, &size );

            if (OSS_MemChk(bufHdl->osHdl,(int8*)valueP, size,
                          OSS_WRITE) != 0) /* check perm */
                return(ERR_OSS_NO_PERM);

            MBUF_GetData( bufHdl, (int8*)valueP );
            break;

        case M_BUF_BLK_WR_DATA:
            /* if( !bufHdl )  - already checked above
                return(ERR_MBUF_NO_BUF); */

            MBUF_GetSize( bufHdl, &size );

            if (OSS_MemChk(bufHdl->osHdl,(int8*)valueP, size,
                          OSS_WRITE) != 0) /* check perm */
                return(ERR_OSS_NO_PERM);

            MBUF_GetData( bufHdl, (int8*)valueP );
            break;

        case M_BUF_RD_ERR:
            MBUF_GetErrorFlag( bufHdl, valueP ); break;
        case M_BUF_WR_ERR:
            MBUF_GetErrorFlag( bufHdl, valueP ); break;
        case M_BUF_RD_ERR_COUNT:
            MBUF_GetErrorCount( bufHdl, valueP ); break;
        case M_BUF_WR_ERR_COUNT:
            MBUF_GetErrorCount( bufHdl, valueP ); break;
        case M_BUF_RD_BUFSIZE: MBUF_GetSize( bufHdl, (u_int32*)valueP );  break;
        case M_BUF_RD_WIDTH:   MBUF_GetWidth( bufHdl, valueP ); break;
        case M_BUF_RD_COUNT:   MBUF_GetCount( bufHdl, valueP ); break;
        case M_BUF_RD_TIMEOUT: MBUF_GetTimeout( bufHdl, valueP );  break;
        case M_BUF_RD_HIGHWATER: MBUF_GetLowHighWater( bufHdl, valueP );  break;

        case M_BUF_WR_BUFSIZE: MBUF_GetSize( bufHdl, (u_int32*)valueP );  break;
        case M_BUF_WR_WIDTH:   MBUF_GetWidth( bufHdl, valueP ); break;
        case M_BUF_WR_COUNT:   MBUF_GetCount( bufHdl, valueP ); break;
        case M_BUF_WR_TIMEOUT: MBUF_GetTimeout( bufHdl, valueP );  break;
        case M_BUF_WR_LOWWATER: MBUF_GetLowHighWater( bufHdl, valueP );  break;

        /*--------------------------+
        |   sig info                |
        +--------------------------*/
        case M_BUF_RD_SIGSET_HIGH:
        case M_BUF_WR_SIGSET_LOW:
           OSS_SigInfo( bufHdl->osHdl, bufHdl->sigHdl,
                        valueP, &processId );
           break;

        /*--------------------------+
        |  (unknown)                |
        +--------------------------*/
        default:
           return( ERR_MBUF_UNK_CODE );
    }/*switch*/

    return(0);
}/*MBUF_GetStat*/












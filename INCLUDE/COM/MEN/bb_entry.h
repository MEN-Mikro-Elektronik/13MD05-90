/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_entry.h
 *
 *       Author: ds
 *        $Date: 2008/08/29 08:28:47 $
 *    $Revision: 1.6 $
 *
 *  Description: BBIS board handler jump table structure
 *
 *     Switches:
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_entry.h,v $
 * Revision 1.6  2008/08/29 08:28:47  dpfeuffer
 * R:1. setStat/getStat protptype doesn't support 64-bit
 * M:1. setStat/getStat protptyp changed to INT32_OR_64
 *
 * Revision 1.5  2006/12/11 17:11:19  ufranke
 * changed
 * - fkt13() is now setIrqHandle() for CHAMELEON GIRQ support
 *   because GIRQ is not designed for BBIS ( seperate register for each IRQ enable )
 *   we must be able to lock/unlock IRQ in BBIS itself
 *
 * Revision 1.4  1998/07/31 14:53:50  see
 * structure BBIS_ENTRY changed: fkt29() is now clrMIface()
 *
 * Revision 1.3  1998/06/02 11:00:11  see
 * cosmetics
 *
 * Revision 1.2  1998/02/25 12:19:29  franke
 * unused fkt now with parameter void
 *
 * Revision 1.1  1998/02/19 09:13:14  franke
 * initial
 *
 * Revision 1.1  1997/12/04 09:56:40  ds
 * initial revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BB_ENTRY_H_
#define _BB_ENTRY_H_

#ifdef __cplusplus
    extern "C" {
#endif


/*---------------------------------------------------------------------------+
|    JUMP TABLE STRUCTURE                                                    |
+---------------------------------------------------------------------------*/
typedef struct _BBIS_ENTRY {
    /* init/exit */
    int32 (*init)       (OSS_HANDLE*,DESC_SPEC*,BBIS_HANDLE**);     /* 00 */
    int32 (*brdInit)    (BBIS_HANDLE*);                             /* 01 */
    int32 (*brdExit)    (BBIS_HANDLE*);                             /* 02 */
    int32 (*exit)       (BBIS_HANDLE**);                            /* 03 */
    int32 (*fkt04)      (void);                                     /* 04 */

    /* info */
    int32 (*brdInfo)    (u_int32,...);                              /* 05 */
    int32 (*cfgInfo)    (BBIS_HANDLE*,u_int32,...);                 /* 06 */
    int32 (*fkt07)      (void);                                     /* 07 */
    int32 (*fkt08)      (void);                                     /* 08 */
    int32 (*fkt09)      (void);                                     /* 09 */

    /* interrupt handling */
    int32 (*irqEnable)  (BBIS_HANDLE*,u_int32,u_int32);             /* 10 */
    int32 (*irqSrvInit) (BBIS_HANDLE*,u_int32);                     /* 11 */
    void  (*irqSrvExit) (BBIS_HANDLE*,u_int32);                     /* 12 */
    int32 (*setIrqHandle)( BBIS_HANDLE *, OSS_IRQ_HANDLE * );       /* 13 */
    int32 (*fkt14)      (void);                                     /* 14 */

    /* exception handling */
    int32 (*expEnable)  (BBIS_HANDLE*,u_int32,u_int32);             /* 15 */
    int32 (*expSrv)     (BBIS_HANDLE*,u_int32);                     /* 16 */
    int32 (*fkt17)      (void);                                     /* 17 */
    int32 (*fkt18)      (void);                                     /* 18 */
    int32 (*fkt19)      (void);                                     /* 19 */

    /* */
    int32 (*fkt20)      (void);                                     /* 20 */
    int32 (*fkt21)      (void);                                     /* 21 */
    int32 (*fkt22)      (void);                                     /* 22 */
    int32 (*fkt23)      (void);                                     /* 23 */
    int32 (*fkt24)      (void);                                     /* 24 */

    /*  getstat / setstat / address setting */
    int32 (*setStat)    (BBIS_HANDLE*,u_int32,int32,INT32_OR_64);   /* 25 */
    int32 (*getStat)    (BBIS_HANDLE*,u_int32,int32,INT32_OR_64*);  /* 26 */
    int32 (*setMIface)  (BBIS_HANDLE*,u_int32,u_int32,u_int32);     /* 27 */
    int32 (*getMAddr)   (BBIS_HANDLE*, u_int32, u_int32, u_int32, 	/* 28 */
						 void**, u_int32*);
    int32 (*clrMIface)  (BBIS_HANDLE*,u_int32);                     /* 29 */
    int32 (*fkt30)      (void);                                     /* 30 */
    int32 (*fkt31)      (void);                                     /* 31 */

} BBIS_ENTRY;


/*---------------------------------------------------------------------------+
|    FUNCTION DECLARATION                                                    |
+---------------------------------------------------------------------------*/
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    extern void BBIS_GetEntry( BBIS_ENTRY* bbisP );
#else
    extern int32 BBIS_FindEntry( char* boardName, BBIS_ENTRY* bbisP );
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */

#ifdef __cplusplus
    }
#endif

#endif /* _BB_ENTRY_H_ */




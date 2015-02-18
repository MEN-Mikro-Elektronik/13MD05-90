/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: desc.h
 *
 *       Author: uf
 *        $Date: 1998/05/29 16:49:07 $
 *    $Revision: 1.3 $
 *
 *  Description: interface to desc module
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: desc.h,v $
 * Revision 1.3  1998/05/29 16:49:07  see
 * descriptor types added (defs)
 *
 * Revision 1.2  1998/04/17 15:38:44  see
 * DESC_MAX_KEYLEN added (resized from 4096 to 256)
 * missing prototypes for DESC_DbgLevelGet/Set added
 *
 * Revision 1.1  1998/02/23 09:41:18  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _DESC_H
#  define _DESC_H

#  ifdef __cplusplus
      extern "C" {
#  endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/* descriptor data ptr */
typedef void* DESC_SPEC;

/* descriptor handle */
typedef void* DESC_HANDLE;

/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
/* limits */
#define DESC_MAX_KEYLEN     256		/* max key string length */

/* descriptor types */
#define DESC_TYPE_DEVICE	0x1
#define DESC_TYPE_BOARD		0x2

/* check return value for fatal errors */
#define DESC_FATAL(error)	((error) && (error) != ERR_DESC_KEY_NOTFOUND)

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
extern char* DESC_Ident( void );
extern int32 DESC_Init( DESC_SPEC   *descSpec,
                        OSS_HANDLE  *osHdl,
                        DESC_HANDLE **descHandleP );

extern int32 DESC_GetUInt32( DESC_HANDLE *descHandle,
                             u_int32      defVal,
                             u_int32     *valueP,
                             char        *keyFmt,
                             ...);

extern int32 DESC_GetBinary( DESC_HANDLE *descHandle,
                             u_int8      *defVal,
                             u_int32     defValLen,
                             u_int8      *bufP,
                             u_int32     *lenP,
                             char        *keyFmt,
                             ...);

extern int32 DESC_GetString( DESC_HANDLE *descHandle,
                             char        *defVal,
                             char        *bufP,
                             u_int32     *lenP,
                             char        *keyFmt,
                             ...);

extern void DESC_DbgLevelSet( DESC_HANDLE *descHandle,
							  u_int32 dbgLevel);

extern void DESC_DbgLevelGet( DESC_HANDLE *descHandle,
							  u_int32 *dbgLevelP);

extern int32 DESC_Exit( DESC_HANDLE **descHandleP );

#  ifdef __cplusplus
   }
#  endif

#endif/*_DESC_H*/



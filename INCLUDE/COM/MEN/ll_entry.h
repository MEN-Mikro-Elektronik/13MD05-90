/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: ll_entry.h
 *
 *      $Author: CKauntz $
 *        $Date: 2008/08/22 10:22:39 $
 *    $Revision: 1.4 $
 *
 *  Description: MDIS LL-driver jump table definitions
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: ll_entry.h,v $
 * Revision 1.4  2008/08/22 10:22:39  CKauntz
 * changed getstat arg INT32_OR_64 for 64BIT
 *
 * Revision 1.3  2008/01/18 14:24:43  ufranke
 * changed
 *  - prepared for 64bit OS
 *  - setStat arg INT32_OR_64 now
 *
 * Revision 1.2  1998/06/02 11:00:17  see
 * cosmetics (headers)
 *
 * Revision 1.1  1998/02/19 09:12:23  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997..2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _LL_ENTRY_H
#define _LL_ENTRY_H

#ifdef __cplusplus
    extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* jump table structure */
typedef struct {
        int32 (*init)(DESC_SPEC*, OSS_HANDLE*, MACCESS*,
					  OSS_SEM_HANDLE*, OSS_IRQ_HANDLE*, LL_HANDLE**);
        int32 (*exit)(LL_HANDLE**);
        int32 (*read)(LL_HANDLE*, int32, int32*);
        int32 (*write)(LL_HANDLE*,int32, int32);
        int32 (*blockRead)( LL_HANDLE*, int32, void*, int32, int32* );
        int32 (*blockWrite)(LL_HANDLE*, int32, void*, int32, int32* );
        int32 (*setStat)(LL_HANDLE*, int32, int32, INT32_OR_64);
        int32 (*getStat)(LL_HANDLE*, int32, int32, INT32_OR_64* );
        int32 (*irq)(LL_HANDLE*);
        int32 (*info)(int32, ...);
} LL_ENTRY;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    extern void LL_GetEntry( LL_ENTRY* drvP );
#else
    extern int32 LL_FindEntry( char* name, LL_ENTRY* drvP );
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */

#ifdef __cplusplus
    }
#endif

#endif /* _LL_ENTRY_H */





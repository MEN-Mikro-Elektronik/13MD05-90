/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: ll_defs.h
 *
 *      $Author: see $
 *        $Date: 1998/06/02 11:00:14 $
 *    $Revision: 1.2 $
 *
 *  Description: LL-Drv. defines
 *
 *     Switches: _NO_LL_HANDLE	exclude LL_HANDLE declaration
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: ll_defs.h,v $
 * Revision 1.2  1998/06/02 11:00:14  see
 * ifdef _NO_LL_HANDLE added to exclude LL_HANDLE declaration
 * LL_INFO_LOCKMODE defines added
 *
 * Revision 1.1  1998/02/19 09:12:37  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _LL_DEFS_H
#define _LL_DEFS_H

#ifdef __cplusplus
    extern "C" {
#endif

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/* ll-drv info function codes */
#define LL_INFO_ADDRSPACE_COUNT         0x01
#define LL_INFO_ADDRSPACE               0x02
#define LL_INFO_HW_CHARACTER            0x03
#define LL_INFO_IRQ                     0x04
#define LL_INFO_LOCKMODE                0x05

/* LL_INFO_LOCKMODE values */
#define LL_LOCK_NONE	0x01
#define LL_LOCK_CALL	0x02
#define LL_LOCK_CHAN	0x03

/* ll-drv irq return codes */
#define LL_IRQ_DEV_NOT  0
#define LL_IRQ_DEVICE   1
#define LL_IRQ_UNKNOWN  2

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
#ifndef _NO_LL_HANDLE
   typedef void *LL_HANDLE;
#endif

#ifdef __cplusplus
    }
#endif

#endif /* _LL_DEFS_H */


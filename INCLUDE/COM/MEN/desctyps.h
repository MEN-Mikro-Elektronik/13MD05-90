/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: desctyps.h
 *
 *       Author: see
 *        $Date: 1998/05/29 16:49:04 $
 *    $Revision: 1.2 $
 *
 *  Description: Descriptor key types
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: desctyps.h,v $
 * Revision 1.2  1998/05/29 16:49:04  see
 * missing header/ifdefs added
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _DESCTYPS_H
#define _DESCTYPS_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* keys: all keys begin with 0x6d */
#define DESC_BINARY			0x6d01
#define DESC_U_INT32		0x6d02
#define DESC_STRING			0x6d03
#define DESC_DIR			0x6d64
#define DESC_END			0x6d65

#ifdef __cplusplus
	}
#endif

#endif	/* _DESCTYPS_H */


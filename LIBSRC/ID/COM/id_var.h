/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: id_var.h
 *
 *       Author: ds
 *        $Date: 1999/04/26 14:40:21 $
 *    $Revision: 1.1 $
 *
 *  Description: ID library defines for different variants
 *
 *     Switches: ID_SW - swapped access
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: id_var.h,v $
 * Revision 1.1  1999/04/26 14:40:21  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _ID_VAR_H
#define _ID_VAR_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* swapped access */
#ifdef ID_SW
#	define MAC_MEM_MAPPED
#	define MAC_BYTESWAP
#endif

#ifdef __cplusplus
	}
#endif

#endif	/* _ID_VAR_H */

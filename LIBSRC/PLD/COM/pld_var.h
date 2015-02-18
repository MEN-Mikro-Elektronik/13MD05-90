/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: pld_var.h
 *
 *       Author: ds
 *        $Date: 1999/04/26 15:14:53 $
 *    $Revision: 1.1 $
 *
 *  Description: PLD loader library defines for different variants
 *
 *     Switches: PLD_SW - swapped access
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: pld_var.h,v $
 * Revision 1.1  1999/04/26 15:14:53  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _PLD_VAR_H
#define _PLD_VAR_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* swapped access */
#ifdef PLD_SW
#	define MAC_MEM_MAPPED
#	define MAC_BYTESWAP
#endif

#ifdef __cplusplus
	}
#endif

#endif	/* _PLD_VAR_H */

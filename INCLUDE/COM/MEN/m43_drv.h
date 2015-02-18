/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: m43_ll.h
 *
 *       Author: ds
 *        $Date: 2013/07/11 14:32:42 $
 *    $Revision: 1.4 $
 *
 *  Description: M43 specific set-/getstat codes and LL prototypes
 *
 *     Switches:  _ONE_NAMESPACE_PER_DRIVER_
 *				  _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m43_drv.h,v $
 * Revision 1.4  2013/07/11 14:32:42  gv
 * R: Porting to MDIS5
 * M: Changed according to MDIS Porting Guide 0.9
 *
 * Revision 1.3  2004/04/07 15:08:37  cs
 * cosmetics for MDIS4/2004 conformity
 *   removed global prototypes
 *   (changed to static and moved to m43_drv.c)
 *
 * Revision 1.2  2001/08/16 10:09:07  kp
 * support swapped access variant
 *
 * Revision 1.1  1998/03/09 09:38:56  Schmidt
 * Added by mcvs
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _M43_LL_H_
#define _M43_LL_H_

#ifdef __cplusplus
	extern "C" {
#endif


/*---------------------------------------------------------------------------+
|    DEFINES                                                                 |
+---------------------------------------------------------------------------*/

/*-- M43 specific status codes ( M_DEV_OF / M_DEV_BLK_OF + 0x00...0xff ) --*/
/*											S,G: S=setstat, G=getstat code */


/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
#ifdef _LL_DRV_

# ifdef _ONE_NAMESPACE_PER_DRIVER_
#	define M43_GetEntry		LL_GetEntry
# else
	/* variant for swapped access */
#	ifdef M43_SW
#		define M43_GetEntry		M43_SW_GetEntry
#	endif
	extern void M43_GetEntry(LL_ENTRY* drvP);
# endif
#endif /* _LL_DRV_ */

/*-----------------------------------------+
|  BACKWARD COMPATIBILITY TO MDIS4         |
+-----------------------------------------*/
#ifndef U_INT32_OR_64
 /* we have an MDIS4 men_types.h and mdis_api.h included */
 /* only 32bit compatibility needed!                     */
 #define INT32_OR_64  int32
 #define U_INT32_OR_64 u_int32
 typedef INT32_OR_64  MDIS_PATH;
#endif /* U_INT32_OR_64 */

#ifdef __cplusplus
	}
#endif

#endif /* _M43_LL_H_ */

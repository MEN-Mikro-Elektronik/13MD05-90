/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_a201.h
 *
 *      $Author: ufranke $
 *        $Date: 2004/07/30 11:36:56 $
 *    $Revision: 1.7 $
 *
 *  Description: A201/B201/B202 specific set-/getstat codes and BBIS prototypes
 *
 *     Switches:  _ONE_NAMESPACE_PER_DRIVER_
 *                _LL_DRV_
 *				  B201
 *				  B202	
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_a201.h,v $
 * Revision 1.7  2004/07/30 11:36:56  ufranke
 * fixed
 *  - function name GetEntry
 *
 * Revision 1.6  2004/06/21 16:32:49  dpfeuffer
 * changes for swapped variants
 *
 * Revision 1.5  1999/07/07 17:02:36  Franke
 * added   prototypes B201_GetEntry, B202_GetEntry
 * changed only A201/B201/B202_GetEntry are extern now
 *
 * Revision 1.4  1999/05/25 11:00:01  kp
 * added B201/B202 support
 *
 * Revision 1.3  1999/03/31 13:07:33  Schmidt
 * prototype A201_ExpEnable added
 * prototype A201_ExpSrv added
 *
 * Revision 1.2  1998/07/31 15:54:32  see
 * A201_ClrMIface prototype added
 *
 * Revision 1.1  1998/02/19 13:28:56  franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BB_A201_H_
#define _BB_A201_H_

#ifdef __cplusplus
    extern "C" {
#endif


/*---------------------------------------------------------------------------+
|    DEFINES                                                                 |
+---------------------------------------------------------------------------*/

/*-- A201 specific status codes ( M_BRD_OF / M_BRD_BLK_OF + 0x00...0xff ) --*/
/*                                          S,G: S=setstat, G=getstat code */

#ifndef A201_GLOBNAME
#define _A201_GLOBNAME(var,name) var##_##name
# ifndef _ONE_NAMESPACE_PER_DRIVER_
#  define A201_GLOBNAME(var,name) _A201_GLOBNAME(var,name)
# else
#  define A201_GLOBNAME(var,name) _A201_GLOBNAME(A201,name)
# endif
#endif

/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
#define __A201_GetEntry	A201_GLOBNAME(A201_VARIANT,GetEntry)

#ifdef __cplusplus
    }
#endif

#endif /* _BB_A201_H_ */





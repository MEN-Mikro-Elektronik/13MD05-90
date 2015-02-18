/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_chameleon.h
 *
 *       Author: kp
 *        $Date: 2011/01/19 11:03:01 $
 *    $Revision: 2.4 $
 *
 *  Description: Header file for CHAMELEON BBIS driver
 *               - CHAMELEON function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_chameleon.h,v $
 * Revision 2.4  2011/01/19 11:03:01  dpfeuffer
 * R: chameleon BBIS shall support mem and io mapped chameleon units
 * M: __BB_CHAMELEON_IoRead/WriteD32 defines and prototypes added
 *
 * Revision 2.3  2009/05/13 13:21:54  cs
 * R:1. Windows failed to find GetEntry function (expects to be called BBIS_*)
 *   2. no default prefix for chameleon variant GetEntry was defined
 * M:1. when ONE_NAMESPACE_PER_DRIVER is set:
 *      GetEntry funciton is BBIS_GetEntry
 *   2. when nothing else set use CHAMELEON_ as variant prefix
 *
 * Revision 2.2  2008/08/08 09:48:07  aw
 * R: Project GCCS needs two chameleon bbis drivers with different names. One
 *    with interrupts from chameleon table(EM09) and one with interrupts over PCI
 *    (F206).
 * M: Added variant support for function prototyp xxx_GetEntry.
 *
 * Revision 2.1  2003/02/03 10:44:39  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BB_CHAMELEON_H_
#define _BB_CHAMELEON_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*---------------------------------------------------------------------------+
|    DEFINES                                                                 |
+---------------------------------------------------------------------------*/
/* defines for variant support */
#ifndef  BB_CHAMELEON_VARIANT
# define BB_CHAMELEON_VARIANT CHAMELEON
#endif

# define _BB_CHAMELEON_GLOBNAME(var,name) var##_##name

#ifndef _ONE_NAMESPACE_PER_DRIVER_
# define BB_CHAMELEON_GLOBNAME(var,name) _BB_CHAMELEON_GLOBNAME(var,name)
#else
# define BB_CHAMELEON_GLOBNAME(var,name) _BB_CHAMELEON_GLOBNAME(BBIS,name)
#endif

#define __BB_CHAMELEON_GetEntry			BB_CHAMELEON_GLOBNAME(BB_CHAMELEON_VARIANT,GetEntry)
#define __BB_CHAMELEON_IoReadD32		BB_CHAMELEON_GLOBNAME(BB_CHAMELEON_VARIANT,IoReadD32)
#define __BB_CHAMELEON_IoWriteD32		BB_CHAMELEON_GLOBNAME(BB_CHAMELEON_VARIANT,IoWriteD32)

/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/

extern u_int32 __BB_CHAMELEON_IoReadD32( MACCESS ma, u_int32 offs );
extern void __BB_CHAMELEON_IoWriteD32( MACCESS ma, u_int32 offs, u_int32 val );

#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void __BB_CHAMELEON_GetEntry( BBIS_ENTRY* bbisP );
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */

#ifdef __cplusplus
	}
#endif

#endif /* _BB_CHAMELEON_H_ */


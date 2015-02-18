/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: d302_int.h
 *
 *       Author: ds
 *        $Date: 2001/11/12 15:48:15 $
 *    $Revision: 1.1 $
 *
 *  Description: Internal header file for D302 BBIS
 *
 *     Switches: ---
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: d302_int.h,v $
 * Revision 1.1  2001/11/12 15:48:15  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _D302_H
#define _D302_H

#ifdef __cplusplus
    extern "C" {
#endif


/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
#define D302_PCI_VEN_ID			0x1172	/* pci vendor-id				*/
#define D302_PCI_DEV_ID			0xD302	/* pci device-id				*/

#if (defined(_BIG_ENDIAN_)    && !defined(MAC_BYTESWAP)) \
 || (defined(_LITTLE_ENDIAN_) &&  defined(MAC_BYTESWAP))
# define D302_LCTRL				0x02	/* offset to local ctrl-reg		*/
# define D302_GCTRL				0x00	/* offset to global ctrl-reg	*/
#else
# define D302_LCTRL				0x00	/* offset to local ctrl-reg		*/
# define D302_GCTRL				0x02	/* offset to global ctrl-reg	*/
#endif

/* local ctrl-reg */
#define D302_LCTRL_IP			0x01	/* module interrupt pending		*/
#define D302_LCTRL_IEN			0x02	/* module interrupt enable		*/
#define D302_LCTRL_FAST			0x04	/* module fast PCI access		*/
#define D302_LCTRL_TOUT			0x08	/* module timeout occured		*/

/* global ctrl-reg */
#define D302_GCTRL_GIP			0x01	/* global interrupt pending		*/
#define D302_GCTRL_GIEN			0x02	/* global interrupt enable		*/


#ifdef __cplusplus
    }
#endif

#endif /* _D302_H */

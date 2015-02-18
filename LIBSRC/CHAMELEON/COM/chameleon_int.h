/***********************  I n c l u d e  -  F i l e  ***********************/
/**
 *         \file chameleon_int.h
 *
 *       \author dieter.pfeuffer@men.de
 *        $Date: 2009/03/17 15:21:11 $
 *    $Revision: 2.9 $
 *
 *        \brief Internal header file for chameleon library
 *
 *     Switches: _LITTLE_ENDIAN_ / _BIG_ENDIAN_
 *               _DOXYGEN_ - to build proper doxygen documentation
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: chameleon_int.h,v $
 * Revision 2.9  2009/03/17 15:21:11  dpfeuffer
 * R: compiler error with VC2008 64bit compiler
 * M: (U_INT32_OR_64) cast added
 *
 * Revision 2.8  2008/03/31 11:31:40  DPfeuffer
 * - BIT4BAR(bar) macro added
 *
 * Revision 2.7  2007/07/11 09:15:13  CKauntz
 * added Swapping for Big and Little Endian
 *
 * Revision 2.6  2007/05/18 11:19:57  cs
 * removed CHAV2_PCI_VENID/..DEVID (defined in MEN/chameleon.h)
 *
 * Revision 2.5  2006/03/24 14:56:04  cschuster
 * changed:
 *     - access IO-mapped addresses with VXWORKS + PPC just like memory-mapped
 *
 * Revision 2.4  2005/07/08 14:42:15  ub
 * redeclaration of MACCESS removed
 *
 * Revision 2.3  2005/06/23 16:12:03  kp
 * Copyright line changed (sbo)
 *
 * Revision 2.2  2005/05/04 08:59:40  kp
 * moved static prototypes into C files
 * don't re-typedef MACCESS if mac_mem.h used
 *
 * Revision 2.1  2005/04/29 14:45:25  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _CHAMELEON_INT_H
#define _CHAMELEON_INT_H

#ifdef __cplusplus
	extern "C" {
#endif


/*--------------------------------------*/
/*    INCLUDES                          */
/*--------------------------------------*/
#include <MEN/men_typs.h>
#include <MEN/chameleon.h>

#if defined(VXWORKS) && defined(MAC_IO_MAPPED) && CPU_FAMILY==PPC
# undef MAC_IO_MAPPED
# define MAC_MEM_MAPPED
# include <MEN/maccess.h>
# undef MAC_MEM_MAPPED
# define MAC_IO_MAPPED
#else
#include <MEN/maccess.h>
#endif

#include <MEN/dbg.h>

/*--------------------------------------*/
/*	DEFINES			            		*/
/*--------------------------------------*/
/* Layout of the shared RAM (offsets) */
#if ((defined (_LITTLE_ENDIAN_) && !(defined (MAC_BYTESWAP))) || \
    (defined (_BIG_ENDIAN_) && defined (MAC_BYTESWAP)))

# define SWAPWORD(w) (w)
# define SWAPLONG(l) (l)

#elif ((defined (_BIG_ENDIAN_) && !(defined (MAC_BYTESWAP))) || \
      (defined (_LITTLE_ENDIAN_) && defined (MAC_BYTESWAP)) )

# define SWAPWORD(w) ((((w)&0xff)<<8) + (((w)&0xff00)>>8))
# define SWAPLONG(l) ((((l)&0xff)<<24) + (((l)&0xff00)<<8) + \
					 (((l)&0xff0000)>>8) + (((l)&0xff000000)>>24))

#else
#error "Invalid combination of _LITTLE_ENDIAN_, _BIG_ENDIAN_ and MAC_BYTESWAP !"
#endif

#define CFGTABLE_SIZE 0x80
#define CFGTABLE_MA(h) 			  ((MACCESS)(U_INT32_OR_64)(h)->bar[0])

#define CFGTABLE_READWORD(ma,idx) SWAPWORD(MREAD_D16(ma,(idx)*4))
#define CFGTABLE_READLONG(ma,idx) SWAPLONG(MREAD_D32(ma,(idx)*4))

#if defined(LINUX) || defined(__QNXNTO__) || defined(WINNT)
#  define MAP_REQUIRED
#endif

#define NBR_OF_BARS 6
#define BIT4BAR(bar) (0x1 << (bar))


/*
 * Chameleon-V2 Table Map
 */		

/* header */
#define CHAV2_HEADER_LSIZE	5	/* nbr of longs */

/* unit/bridge/cpu (0x00) */
#define CHAV2_DTYPE_MASK	0xf0000000
#define CHAV2_DTYPE_SHIFT	28

#define CHAV2_DTYPE_UNIT	0x0
#define CHAV2_DTYPE_BRG		0x1
#define CHAV2_DTYPE_CPU		0x2
#define CHAV2_DTYPE_BAR		0x3
#define CHAV2_DTYPE_END		0xf
		
/* unit */
#define CHAV2_UNIT_LSIZE		4	/* nbr of longs */

/* bridge */
#define CHAV2_BRG_LSIZE			5	/* nbr of longs */

/* cpu */
#define CHAV2_CPU_LSIZE			3	/* nbr of longs */

/* cpu */
#define CHAV2_BA_LSIZE(count)	(1 + ((count)*2))	/* nbr of longs */


#ifdef __cplusplus
	}
#endif

#endif	/* _CHAMELEON_INT_H */





/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: maccess.h
 *
 *       Author: uf
 *        $Date: 2011/05/18 16:57:07 $
 *    $Revision: 1.18 $
 *
 *  Description: including os specific access macros
 *
 *     Switches: MAC_MEM_MAPPED		memory mapped access
 *               MAC_IO_MAPPED		i/o mapped access
 *               MAC_SHARC_TB		access with SHARC toolbox
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: maccess.h,v $
 * Revision 1.18  2011/05/18 16:57:07  CRuff
 * R: 1. support of pci domains
 * M: 1a) new macro MAC_PCI2MAHDL2 added, which includes the pci domain
 *    1b) new macro MAC_MAHDL2PCI_DOM added to extract the pci domain
 *
 * Revision 1.17  2009/08/31 11:41:55  MRoth
 * R: MAC_MAHDL2PCI_* did not compile with 64-bit Windows compiler
 * M: U_INT32_OR_64 cast added to MAC_MAHDL2PCI_*
 *
 * Revision 1.16  2008/08/22 09:54:52  dpfeuffer
 * R: MAC_PCI2MAHDL did not compile with 64-bit Windows compiler
 * M: U_INT32_OR_64 cast added to MAC_PCI2MAHDL
 *
 * Revision 1.15  2007/03/09 13:27:46  ufranke
 * changed
 * + NIOS_II to NIOS_II_ALTERA_IDE to stay mac_mem.h for NIOS MENMON
 *
 * Revision 1.14  2007/02/07 16:01:28  aw
 * added NIOS_II defines
 *
 * Revision 1.13  2005/06/23 16:11:14  kp
 * Copyright line changed (sbo)
 *
 * Revision 1.12  2004/03/19 11:11:03  ub
 * added QNX conditional
 *
 * Revision 1.11  2002/06/14 10:53:11  DSchmidt
 * added VCIRTX conditional for io access
 *
 * Revision 1.10  2001/01/19 14:48:14  kp
 * added LINUX conditional
 *
 * Revision 1.9  1999/07/23 09:49:44  Schmidt
 * MAC_PCI2MAHDL, MAC_MAHDL2PCI_xxx macros added
 *
 * Revision 1.8  1999/07/22 18:07:23  Franke
 * added mac_io_vxworks.h and check MAC_MEM/IO
 *
 * Revision 1.7  1999/04/22 10:35:20  Schmidt
 * swap macros added
 *
 * Revision 1.6  1999/03/26 11:41:22  Schoberl
 * _WINNT_REG_FUNC_ removed
 *
 * Revision 1.5  1999/03/24 13:43:08  Schmidt
 * MAC_IO_MAPPED added
 * _WINNT_REG_FUNC_ defines MAC_IO_MAPPED for compatibility
 *
 * Revision 1.4  1998/10/15 14:37:27  see
 * header: switches explained
 *
 * Revision 1.3  1998/09/25 10:44:16  kp
 * added MAC_SHARC_TB
 *
 * Revision 1.2  1998/06/10 15:11:38  Schmidt
 * changed MEM_MAPPED to MAC_MEM_MAPPED
 *
 * Revision 1.1  1998/02/23 10:08:20  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _MACCESS_H
#define _MACCESS_H


/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
/* macros for PCI-parameter <--> maccess-handle conversion */
#define MAC_PCI2MAHDL(bus, dev, func)						\
	(MACCESS)(U_INT32_OR_64) ( ( (bus)  & 0xff ) << 16 |	\
				( (dev)  & 0x1f ) << 11 |					\
				( (func) & 0x07 ) <<  8 )

#define MAC_PCI2MAHDL2(domain, bus, dev, func)				\
	(MACCESS)(U_INT32_OR_64) ( ( (domain) & 0xff ) << 24 |	\
				( (bus)  & 0xff ) << 16 |					\
				( (dev)  & 0x1f ) << 11 |					\
				( (func) & 0x07 ) <<  8 )

#define MAC_MAHDL2PCI_DOM(ma)	 ( (u_int32)(((U_INT32_OR_64)(ma)>>24) & 0xff) )
#define MAC_MAHDL2PCI_BUS(ma)	 ( (u_int32)(((U_INT32_OR_64)(ma)>>16) & 0xff) )
#define MAC_MAHDL2PCI_DEV(ma)	 ( (u_int32)(((U_INT32_OR_64)(ma)>>11) & 0x1f) )
#define MAC_MAHDL2PCI_FUNC(ma) ( (u_int32)(((U_INT32_OR_64)(ma)>> 8) & 0x07) )

/* swap macros */
#ifdef MAC_BYTESWAP
#	define RSWAP8(a)	(a)
#	define RSWAP16(a)	OSS_Swap16(a)
#	define RSWAP32(a)	OSS_Swap32(a)
#	define WSWAP8(a)	(a)
#	define WSWAP16(a)	OSS_SWAP16(a)
#	define WSWAP32(a)	OSS_SWAP32(a)
#else
#	define RSWAP8(a)	(a)
#	define RSWAP16(a)	(a)
#	define RSWAP32(a)	(a)
#	define WSWAP8(a)	(a)
#	define WSWAP16(a)	(a)
#	define WSWAP32(a)	(a)
#endif


/* sanity check */
#ifdef MAC_MEM_MAPPED
#	ifdef MAC_IO_MAPPED
#		error "Do not define MAC_MEM_MAPPED and MAC_IO_MAPPED together"
#	endif /* MAC_IO_MAPPED */
#endif /* MAC_MEM_MAPPED */
#ifndef MAC_MEM_MAPPED
#	ifndef MAC_IO_MAPPED
#		error "Define MAC_MEM_MAPPED or MAC_IO_MAPPED must be defined"
#	endif /* MAC_IO_MAPPED */
#endif /* MAC_MEM_MAPPED */


/* include files for memory mapped access */
#ifdef MAC_MEM_MAPPED

#	ifdef WINNT
#	define MAC_OS_SPECIFIC
#   	include <MEN/MACCESS/nt_func.h>
#	endif

#	ifdef LINUX
#	define MAC_OS_SPECIFIC
#   	include <MEN/MACCESS/maccess_linux.h>
#	endif

#   ifdef NIOS_II_ALTERA_IDE
#   define MAC_OS_SPECIFIC
#       include <MEN/MACCESS/maccess_nios2.h>
#   endif


#	ifndef MAC_OS_SPECIFIC
#		include <MEN/MACCESS/mac_mem.h>
#	endif

#endif
 /* MAC_MEM_MAPPED */


/* include files for i/o mapped access */
#ifdef MAC_IO_MAPPED

#	ifdef WINNT
#   	include <MEN/MACCESS/nt_func.h>
#	endif

#	ifdef VXWORKS
#   	include <MEN/MACCESS/mac_io_vxworks.h>
#	endif

#	ifdef LINUX
#   	include <MEN/MACCESS/maccess_linux.h>
#	endif

#	ifdef __QNX__
#   	include <MEN/MACCESS/maccess_qnx.h>
#	endif

#	ifdef VCIRTX
#   	include <MEN/MACCESS/mac_io_vcirtx.h>
#	endif

#   ifdef NIOS_II_ALTERA_IDE
#     include <MEN/MACCESS/maccess_nios2.h>
# endif


#endif /* MAC_IO_MAPPED */


/* others */
#ifdef MAC_SHARC_TB
#	include <MEN/MACCESS/mac_sharc.h>
#endif


#endif /* _MACCESS_H */







/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: maccess_linux.h
 *
 *       Author: kp
 *        $Date: 2012/04/16 13:09:15 $
 *    $Revision: 1.5 $
 *
 *  Description: access macros for linux
 *
 *     Switches: MAC_MEM_MAPPED, MAC_IO_MAPPED, MAC_BYTESWAP
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: maccess_linux.h,v $
 * Revision 1.5  2012/04/16 13:09:15  ts
 * R: build of customer specific userland application failed
 * M: make include of io.h dependent on define __KERNEL__ (kernelspace builds)
 *
 * Revision 1.4  2009/03/18 13:12:04  ts
 * R: the wrong native header was included for PowerPCs
 * M: base conditional include on CONFIG_PPC, not _PPC_IO_H
 *
 * Revision 1.3  2007/07/13 17:56:22  cs
 * added:
 *   + support for user space
 *
 * Revision 1.2  2004/06/09 11:12:21  kp
 * added UML support (for testing)
 *
 * Revision 1.1  2001/01/19 14:58:22  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MACCESS_LINUX_H
#define _MACCESS_LINUX_H

#ifdef __KERNEL__
# include <asm/io.h>
#endif


#if defined (__UM_IO_H)
/*
 * User mode linux has no real mem mapped I/O
 */

#define readb(addr) (0)
#define readw(addr) (0)
#define readl(addr) (0)
#define writeb(b,addr)
#define writew(b,addr)
#define writel(b,addr)
#endif

/*
 * Handle PowerPCs differently due to the weird byteswapping in asm-ppc/io.h
 */
#ifndef MAC_USERSPACE
# ifdef CONFIG_PPC
#  include <MEN/MACCESS/mac_linux_ppc.h>
# else
#  include <MEN/MACCESS/mac_linux_generic.h>
# endif
#else /* !MAC_USERSPACE */
# include <MEN/MACCESS/mac_linux_usr.h>
#endif /* !MAC_USERSPACE */

#endif /* _MACCESS_LINUX_H */

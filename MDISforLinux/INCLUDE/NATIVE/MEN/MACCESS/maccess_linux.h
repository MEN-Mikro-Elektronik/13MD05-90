/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: maccess_linux.h
 *
 *       Author: kp
 *
 *  Description: access macros for linux
 *
 *     Switches: MAC_MEM_MAPPED, MAC_IO_MAPPED, MAC_BYTESWAP
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

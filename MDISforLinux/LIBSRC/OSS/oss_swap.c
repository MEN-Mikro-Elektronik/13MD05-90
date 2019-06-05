/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_swap.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Byte swapping routines
 *
 *    \switches  -
 */
/*
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

#include "oss_intern.h"

/**********************************************************************/
/** Swap bytes in 16-bit word.
 * \copydoc oss_specification.c::OSS_Swap16()
 */
u_int16 OSS_Swap16( u_int16 word )
{
    return( (word>>8) | (word<<8) );
}

/**********************************************************************/
/** Swap bytes in 32-bit word.
 * \copydoc oss_specification.c::OSS_Swap32()
 */
u_int32 OSS_Swap32( u_int32 dword )
{
    return(  (dword>>24) | (dword<<24) | \
             ((dword>>8) & 0x0000ff00) | \
             ((dword<<8) & 0x00ff0000)   );
}


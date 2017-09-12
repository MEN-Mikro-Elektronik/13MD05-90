/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_swap.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2005/07/07 17:17:39 $
 *    $Revision: 1.3 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Byte swapping routines
 *
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_swap.c,v $
 * Revision 1.3  2005/07/07 17:17:39  cs
 * Copyright line changed
 *
 * Revision 1.2  2003/04/11 16:13:36  kp
 * Comments changed to Doxygen
 *
 * Revision 1.1  2001/01/19 14:39:17  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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


/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_isa_pnp.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2005/07/07 17:17:20 $
 *    $Revision: 1.3 $
 *
 *	   \project  MDISforLinux
 *  	 \brief  ISA PNP support
 *
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_isa_pnp.c,v $
 * Revision 1.3  2005/07/07 17:17:20  cs
 * Copyright line changed
 *
 * Revision 1.2  2003/04/11 16:13:23  kp
 * Comments changed to Doxygen
 *
 * Revision 1.1  2001/01/19 14:39:10  kp
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
/** Get ISAPNP configuration data.
 *
 * \copydoc oss_specification.c::OSS_IsaGetConfig()
 *
 * \linux not implemented. Returns \c ERR_OSS_ISA_NO_PNPDATA
 */
int32 OSS_IsaGetConfig(
		OSS_HANDLE	*oss,
		int8		*cardVendId,
		int16		cardProdId,
		int8		*devVendId,
		int16		devProdId,
		int32       devNbr,
		int32       resType,
		int32       resNbr,
		u_int32		*valueP
)
{
    DBGWRT_ERR(( DBH, "*** OSS_IsaGetConfig not implemented\n"));

	return( ERR_OSS_ISA_NO_PNPDATA );
}/*OSS_IsaGetConfig*/


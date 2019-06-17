/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_ident.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  OSS user space library version info
 *
 *    \switches
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2007-2019, MEN Mikro Elektronik GmbH
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

static const char IdentString[]=MENT_XSTR(MAK_REVISION);

char* OSS_Ident( void )
{
    return( (char*)IdentString );
}



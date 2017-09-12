/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  mk_nonmdisif.h
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/02/21 13:34:29 $
 *    $Revision: 2.1 $
 *
 *  	 \brief  Export declaration to be included by non-MDIS drivers
 *				 using the MDIS configuration & BBIS support
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mk_nonmdisif.h,v $
 * Revision 2.1  2003/02/21 13:34:29  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#ifndef _MK_NONMDISIF_H
#define _MK_NONMDISIF_H

int mdis_open_external_dev(
	char *devName,
	char *brdName,
	int slotNo,
	int addrMode,
	int dataMode,
	int addrSpaceSize,
	void **mappedAddrP,
	void **ossHandleP,
	void **devP );

int mdis_close_external_dev( void *_dev );

int mdis_install_external_irq(
	void *_dev,
	int (*handler)(void *data),
	void *data );

int mdis_remove_external_irq( void *_dev );

#endif /* _MK_NONMDISIF_H */

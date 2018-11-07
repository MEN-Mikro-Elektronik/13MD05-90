/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  vme4l_api.h
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2004/07/26 16:31:32 $
 *    $Revision: 2.2 $
 *
 *  	 \brief  VME4L_API user interface definitions
 *
 *     Switches: -
 */
/*
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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

#ifndef _VME4L_API_H
#define _VME4L_API_H

#  ifdef __cplusplus
      extern "C" {
#  endif

int VME4L_Open( VME4L_SPACE space );

int VME4L_Close(int spaceFd);

const char *VME4L_SpaceName( VME4L_SPACE space );

int VME4L_RevisionInfo(int spaceFd, char *buf);

int VME4L_SwapModeSet( int spaceFd, int swapMode);

int VME4L_Read(
	int spaceFd,
	vmeaddr_t vmeAddr,
	int accWidth,
	size_t size,
	void *dataP,
	int flags );
int VME4L_Write(
	int spaceFd,
	vmeaddr_t vmeAddr,
	int accWidth,
	size_t size,
	void *dataP,
	int flags );
int VME4L_Map(
	int spaceFd,
	vmeaddr_t vmeAddr,
	size_t size,
	void **mappedAddrP);
int VME4L_UnMap(
	int spaceFd,
	void *mappedAddr,
	size_t size);
int VME4L_SigInstall( int fd, int vector, int level, int signal, int flags );
int VME4L_SigUnInstall( int fd, int vector );
int VME4L_IrqEnable( int fd, int level );
int VME4L_IrqDisable( int fd, int level );
int VME4L_SysCtrlFunctionGet( int fd );
int VME4L_SysCtrlFunctionSet( int fd, int state );
int VME4L_SysReset( int fd );
int VME4L_ArbitrationTimeoutGet( int fd, int clear );
int VME4L_BusErrorGet(int fd, int *attrP, vmeaddr_t *addrP, int clear );
int VME4L_RequesterModeSet( int fd, int state );
int VME4L_RequesterModeGet( int fd );
int VME4L_AddrModifierSet( int spaceFd, char addrModifier);
int VME4L_AddrModifierGet( int spaceFd );
int VME4L_GeoAddrGet( int spaceFd );
int VME4L_RequesterLevelSet( int spaceFd, char level );
int VME4L_RequesterLevelGet( int spaceFd );
int VME4L_PostedWriteModeGet( int fd );
int VME4L_PostedWriteModeSet( int fd, int state );
int VME4L_IrqGenerate( int fd, int level, int vector);
int VME4L_IrqGenAcked( int fd, int id );
int VME4L_IrqGenClear( int fd, int id );
int VME4L_RmwCycle(
	int spaceFd,
	vmeaddr_t vmeAddr,
	int accWidth,
	uint32_t mask,
	uint32_t *rvP);
int VME4L_AOnlyCycle(
	int spaceFd,
	vmeaddr_t vmeAddr);
int VME4L_SlaveWindowCtrl(
	int spaceFd,
	vmeaddr_t vmeAddr,
	size_t size);

int VME4L_MboxRead( int fd,	int mbox, uint32_t *rvP);
int VME4L_MboxWrite( int fd, int mbox, uint32_t val);
int VME4L_LocMonRegRead( int fd, int reg, uint32_t *rvP);
int VME4L_LocMonRegWrite( int fd, int reg, uint32_t val);

#  ifdef __cplusplus
       }
#  endif

#endif /* _VME4L_API_H */

/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mdis_mk.h
 *
 *       Author: kp
 *        $Date: 2010/02/18 15:37:16 $
 *    $Revision: 1.6 $
 *
 *  Description: MDIS driver definitions
 *
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MDIS_MK_H
#define _MDIS_MK_H

#ifdef __cplusplus
      extern "C" {
#endif

#include <linux/ioctl.h>
/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/

/* structure passed via ioctl for MDIS set/getstats */
typedef struct {
	int32 code;
	union {
		INT32_OR_64 value;
		void *data;
	} p;
} MDIS_LINUX_SGSTAT;

/*
 * structure passed via ioctls
 * MDIS_OPEN_DEVICE, MDIS_REMOVE_DEVICE,
 * MDIS_OPEN_BOARD , MDIS_REMOVE_BOARD
 */
typedef struct {
	char devName[40];			/* IN: MDIS device name */
	int32 devDescLen;			/* IN: length of dev. descriptor */
	void *devData;				/* IN: dev. descriptor data */
	char brdName[40];			/* OUT: (MDIS_OPEN_DEVICE first call)
								   board name from device descriptor */
	int32 brdDescLen;			/* IN: length of brd. descriptor */
	void *brdData;				/* IN: brd. descriptor data */
	
} MDIS_OPEN_DEVICE_DATA;

/* structure passed via ioctl MDIS_CREATE_DEVICE */
typedef struct {
	MDIS_OPEN_DEVICE_DATA d;
	int rtMode;					/* if non-zero, create device for RTAI */
	int persist;				/* if non-zero, don't delete device on close */
} MDIS_CREATE_DEVICE_DATA;

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/

#define MDIS_DEV_NAME		"/dev/mdis"	/* global MDIS device name */
#define MDIS_DESCRIPTOR_DIR "/etc/mdis" /* where to find MDIS descriptors */

#define MDIS_IOC_MAGIC		'M'	/* use this as our magic number */

/* ioctls implemented by the global MDIS driver */
#define MDIS_READ			_IOR(MDIS_IOC_MAGIC, 1, int32)
#define MDIS_WRITE			_IO(MDIS_IOC_MAGIC, 2 )
#define MDIS_SETSTAT		_IOWR( MDIS_IOC_MAGIC, 3, MDIS_LINUX_SGSTAT )
#define MDIS_GETSTAT		_IOWR( MDIS_IOC_MAGIC, 4, MDIS_LINUX_SGSTAT )
#define MDIS_OPEN_DEVICE    _IOW( MDIS_IOC_MAGIC, 5, MDIS_OPEN_DEVICE_DATA )
#define MDIS_CREATE_DEVICE  _IOW( MDIS_IOC_MAGIC, 6, MDIS_CREATE_DEVICE_DATA )
#define MDIS_REMOVE_DEVICE	_IOW( MDIS_IOC_MAGIC, 7, MDIS_OPEN_DEVICE_DATA )
#define MDIS_OPEN_BOARD		_IOW( MDIS_IOC_MAGIC, 8, MDIS_OPEN_DEVICE_DATA )
#define MDIS_REMOVE_BOARD	_IOW( MDIS_IOC_MAGIC, 9, MDIS_OPEN_DEVICE_DATA )
#define MDIS_IOC_MAXNR		9

/* table to compress/decompress error numbers on PPC. see mk_module.c */
typedef struct {
	int orgStart, orgEnd, compStart, compEnd;
} MDIS_ERRNO_COMPRESSION_TABLE;

#define MK_ERRNO_COMPRESSION_TABLE \
const MDIS_ERRNO_COMPRESSION_TABLE mdisErrnoCompressionTable[] = { \
	{ ERR_LL,		ERR_LL+0xff,		0x150,	0x15f },	\
	{ ERR_DEV,		ERR_DEV+0xff,		0x160,	0x1ff },	\
	{ ERR_MK, 		ERR_MK+0xff, 		0xa0, 	0xbf },		\
	{ ERR_MBUF,		ERR_MBUF+0xff,		0xc0,	0xcf },		\
	{ ERR_BBIS,		ERR_BBIS+0xff,		0xd0,	0xdf },		\
	{ ERR_OSS,		ERR_OSS+0x7f,		0xe0,	0xff },		\
	{ ERR_OSS_PCI,	ERR_OSS_PCI+0x1f,	0x100,	0x10f },	\
	{ ERR_OSS_VME,	ERR_OSS_VME+0x1f,	0x110,	0x117 },	\
	{ ERR_OSS_ISA,	ERR_OSS_ISA+0x3f,	0x118,	0x11f },	\
	{ ERR_DESC,		ERR_DESC+0x7f,		0x120,	0x12f },	\
	{ ERR_ID,		ERR_ID+0x7f,		0x130,	0x137 },	\
	{ ERR_PLD,		ERR_PLD+0x7f,		0x138,	0x13f },	\
	{ ERR_BK,		ERR_BK+0xff,		0x140,	0x14f },	\
	{ -1 }													\
}														
/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* Linux native extensions to MDIS_API */
int32 MDIS_CreateDevice( char *device, int rtMode, int persist );
int32 MDIS_RemoveDevice( char *device );
int32 MDIS_OpenBoard( char *device );
int32 MDIS_RemoveBoard( char *device );

#ifdef __KERNEL__
extern int mdis_register_ll_driver( char *llName,
									void (*getEntry)(LL_ENTRY *),
									struct module *module);
extern int mdis_unregister_ll_driver( char *llName );
extern int mdis_find_ll_handle( char *devName, LL_HANDLE **hP, LL_ENTRY *entry );
#endif

#ifdef __cplusplus
   }
#endif

#endif  /* _MDIS_MK_H  */


/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: ossu_os.h
 *
 *       Author: christian.schuster@men.de
 *
 *  Description: Linux user space specific data types and defines
 *
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * Copyright 2005-2019, MEN Mikro Elektronik GmbH
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
#ifndef _OSSU_OS_H
#define _OSSU_OS_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <stdarg.h>			/* for variable argument list */
#include <MEN/men_typs.h>
#include <MEN/dbg.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>

#ifdef OSS_CONFIG_PCI
#	include "../../../LIBSRC/pciutils/lib/pci.h"
#endif /* OSS_CONFIG_PCI */
/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define NO_CALLBACK		/* flag for oss.h not to include callback protos */
#define NO_SHARED_MEM	/* flag for oss.h not to include shared mem protos */

#define OSS_HAS_MAP_VME_ADDR 1

#define OSS_DBG_DEFAULT     0xc0008000

/*-------------------------------+
|          PCI specials          |
+--------------------------------*/
#define OSS_PATH_PROC_BUS_PCI	"/proc/bus/pci"

/*-------------------------------+
|          VME specials          |
+--------------------------------*/
/* Access Flags (0x000000xx) */
#define OSS_VME_ACC_MASK		0x000000ff	/* ACC mask */
/** \defgroup _VME_F VME_F_X Access Flags */
/** The following access flags exist. */
/**@{*/
#define OSS_VME_POSTWR		0x00000001	/**< use posted write instead of delayed write */
#define OSS_VME_DMA			0x00000002	/**< use DMA-engine for non-BLT access */
/**@}*/

/** SW Swapping Flags (0x00000x00); currently not supported in OSS_USR Lib */
#define OSS_VME4L_SWAP_MASK	0x00000f00	/* SWSWAP mask */
/** \defgroup _OSS_VME4L_XX_SWAP  Swapping Flags */
/** The following swapping flags exist. */
/**@{*/
#define OSS_VME_SW_SWAP	0x00000100	/**< perform address swapping in software */
#define OSS_VME_HW_SWAP1	0x00000200	/**< use hardware swap mode1 */
/**@}*/

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
typedef struct {
	char devName[40];				/* name of OSS instance */
	int dbgLevel;					/* debug level */
	DBG_HANDLE *dbh;				/* debug handle */

#ifdef OSS_CONFIG_PCI
	struct pci_access *pciAcc;
	struct pci_filter pciFilter;	/* Device filter */
#endif /* OSS_CONFIG_PCI */

	/*
	 * flag that indicates wether OSS has been created for
	 * !only included for compatibility, not used in userspace!
	 */
	int rtMode;
} OSS_HANDLE;

typedef void OSS_ALARM_HANDLE;
typedef void OSS_SIG_HANDLE;
typedef void OSS_SEM_HANDLE;
typedef void OSS_IRQ_HANDLE;
typedef void OSS_SPINL_HANDLE;

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
extern int32  OSS_Init( char *instName, OSS_HANDLE **ossP );
extern int32  OSS_Exit( OSS_HANDLE **ossP );
extern void __attribute__((noreturn)) OSS_DiePci(char *msg, ...);

#ifdef __cplusplus
   }
#endif
#endif /*_OSSU_OS_H*/

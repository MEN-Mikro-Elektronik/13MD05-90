/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu.c
 *
 *      \author  christian.schuster@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Core functions of the OSS module for user space
 *
 *    \switches  MAC_USERSPACE
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2005-2019, MEN Mikro Elektronik GmbH
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
#ifdef MAC_USERSPACE
/*! \mainpage

 This is the documentation of the MEN OSS module (Operating System Services)
 for Linux user mode.

 Refer to the \ref osscommonspec "OSS Common Specification" for the
 plain common specification.

 Under Linux user mode, OSS is implemented as a user space library named
 \b men_oss_usr or \b men_oss_usr_dbg

 \section natusrcalls Special OSS calls under Linux user mode

 - \b Initialisation and termination: OSS_Init(), OSS_Exit()

 \section natlibsused Native libraries and drivers used under Linux user mode

 - \b VME4L_API library, VME4LX driver
 - \b pciutils library (libpci.a)

*/

/*!
 \menimages
*/
/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#include "oss_intern.h"

#include <stdio.h>
#include <stdarg.h> /* for va_list */

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

u_int32 OSS_dbgLev = OSS_DBG_DEFAULT;
DBG_HANDLE *OSS_dbgHdl;

int OSS_ProcBusPCIDev;
int OSS_Memdev;
OSS_DL_LIST OSS_VME_addrWinList;

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/


/**********************************************************************/
/** Initializes the OSS module
 *
 * Creates an instance of OSS and returns a handle for that instance.
 *
 * \param devName		\IN	name of calling module
 * \param ossP			\OUT contains the created handle for that instance
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 * \sa OSS_Exit
 */
int32  OSS_Init( char *devName, OSS_HANDLE **ossP )
{
	OSS_HANDLE *oss = *ossP;
	oss = NULL;

	if( (oss = (OSS_HANDLE *)malloc( sizeof(OSS_HANDLE) )) == NULL )
		return ERR_OSS_MEM_ALLOC;

	if( strlen(devName)+1 > sizeof(oss->devName))
		return ERR_OSS_ILL_PARAM;

	strcpy( oss->devName, devName );
	oss->rtMode	  = 0;


	DBGINIT((NULL,&DBH));		/* init debugging */
	oss->dbgLevel = OSS_DBG_DEFAULT;

	DBGWRT_1((DBH,"OSS_Init %s\n", devName));

	if( (OSS_Memdev = open( "/dev/mem", O_RDWR )) < 0 ){
		printf("OSS_USR: ERROR: can't open /dev/mem");
		return ERR_OSS;
	}


#ifdef OSS_CONFIG_PCI
	oss->pciAcc = pci_alloc();
	oss->pciAcc->error = OSS_DiePci;

	pci_filter_init(oss->pciAcc, &oss->pciFilter);

	pci_init(oss->pciAcc);
	/* scan PCI bus !!! devices are listed here from last to first */
	pci_scan_bus(oss->pciAcc);
		
#endif /* OSS_CONFIG_PCI */
	OSS_DL_NewList( &OSS_VME_addrWinList );

	*ossP = oss;

	return 0;					
}/*OSS_Init*/

/**********************************************************************/
/** Deinitializes the OSS module
 *
 * Destructs an instance of OSS.
 *
 * \param ossP			\IN  contains the handle to destruct\n
 *						\OUT *ossP set to NULL
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 * \sa OSS_Init
 */
int32  OSS_Exit( OSS_HANDLE **ossP )
{
	OSS_HANDLE *oss = *ossP;

	if( oss == NULL )
		return ERR_OSS_ILL_PARAM;

	DBGWRT_1((DBH,"OSS_Exit\n"));
	DBGEXIT((&DBH));

#ifdef OSS_CONFIG_PCI
	pci_cleanup(oss->pciAcc);
#endif /* OSS_CONFIG_PCI */

	if( OSS_Memdev > 0 )
		close( OSS_Memdev );

	free( oss );
	*ossP = NULL;

	return 0;
}/*OSS_Exit*/



/**********************************************************************/
/** Set debug level of OSS instance.
 *
 * \copydoc oss_specification.c::OSS_DbgLevelSet
 * \sa OSS_DbgLevelGet
 */
void OSS_DbgLevelSet( OSS_HANDLE *oss, u_int32 newLevel )
{
    DBG_MYLEVEL = newLevel;
}/*OSS_DbgLevelSet*/

/**********************************************************************/
/** Get debug level of OSS instance.
 *
 * \copydoc oss_specification.c::OSS_DbgLevelGet
 * \sa OSS_DbgLevelSet
 */
u_int32  OSS_DbgLevelGet( OSS_HANDLE *oss )
{
    return( DBG_MYLEVEL );
}/*OSS_DbgLevelGet*/

void __attribute__((noreturn))
OSS_DiePci(char *msg, ...)
{
  va_list args;

  va_start(args, msg);
  fputs("lspci: ", stderr);
  vfprintf(stderr, msg, args);
  fputc('\n', stderr);
  exit(1);
}

#endif /* MAC_USERSPACE */

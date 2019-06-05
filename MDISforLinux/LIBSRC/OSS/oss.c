/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Core functions of the OSS module
 *
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

/*! \mainpage

 This is the documentation of the MEN OSS module (Operating System Services)
 for Linux and/or RTAI kernel mode.

 Refer to the \ref osscommonspec "OSS Common Specification" for the
 plain common specification.

 Under Linux, OSS is implemented as a loadable kernel module,
 named \b men_oss or \b men_oss_dbg.

 \section modparam Module Parameters:

 - #oss_pci_slot_devnbrs

 \section natcalls Special OSS calls under Linux

 - \b Initialisation and termination: OSS_Init(), OSS_Exit()
 - \b RTAI timing mode initialisation: OSS_RtaiTimingInit()
 - \b interface routines to USR_OSS (should not be called by user):
    OSS_RtaiMsToCount(), OSS_TaskToSigEntry(), OSS_RtaiSigTaskRegister(),
	OSS_RtaiSigTaskUnregister()

*/

/*!
 \menimages
*/
/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#include "oss_intern.h"

/*--- symbols exported by OSS module ---*/
EXPORT_SYMBOL(OSS_Init);
EXPORT_SYMBOL(OSS_Exit);
EXPORT_SYMBOL(OSS_Ident);
EXPORT_SYMBOL(OSS_MemGet);
EXPORT_SYMBOL(OSS_MemFree);
EXPORT_SYMBOL(OSS_MemChk);
EXPORT_SYMBOL(OSS_MemCopy);
EXPORT_SYMBOL(OSS_MemFill);
EXPORT_SYMBOL(OSS_StrCpy);
EXPORT_SYMBOL(OSS_StrLen);
EXPORT_SYMBOL(OSS_StrNcmp);
EXPORT_SYMBOL(OSS_StrCmp);
EXPORT_SYMBOL(OSS_StrTok);
EXPORT_SYMBOL(OSS_IrqMask);
EXPORT_SYMBOL(OSS_IrqUnMask);
EXPORT_SYMBOL(OSS_IrqMaskR);
EXPORT_SYMBOL(OSS_IrqRestore);
EXPORT_SYMBOL(OSS_IrqLevelToVector);
EXPORT_SYMBOL(OSS_SigCreate);
EXPORT_SYMBOL(OSS_SigSend);
EXPORT_SYMBOL(OSS_SigRemove);
EXPORT_SYMBOL(OSS_SigInfo);
EXPORT_SYMBOL(OSS_SemCreate);
EXPORT_SYMBOL(OSS_SemRemove);
EXPORT_SYMBOL(OSS_SemWait);
EXPORT_SYMBOL(OSS_SemSignal);
EXPORT_SYMBOL(OSS_DbgLevelSet);
EXPORT_SYMBOL(OSS_DbgLevelGet);
EXPORT_SYMBOL(OSS_Delay);
EXPORT_SYMBOL(OSS_TickRateGet);
EXPORT_SYMBOL(OSS_TickGet);
EXPORT_SYMBOL(OSS_GetPid);
EXPORT_SYMBOL(OSS_MikroDelayInit);
EXPORT_SYMBOL(OSS_MikroDelay);
EXPORT_SYMBOL(OSS_BusToPhysAddr);
EXPORT_SYMBOL(OSS_PciGetConfig);
EXPORT_SYMBOL(OSS_PciSetConfig);
EXPORT_SYMBOL(OSS_PciSlotToPciDevice);
EXPORT_SYMBOL(OSS_IsaGetConfig);
EXPORT_SYMBOL(OSS_AssignResources);
EXPORT_SYMBOL(OSS_UnAssignResources);
EXPORT_SYMBOL(OSS_MapPhysToVirtAddr);
EXPORT_SYMBOL(OSS_UnMapVirtAddr);
EXPORT_SYMBOL(OSS_Sprintf);
EXPORT_SYMBOL(OSS_Vsprintf);
EXPORT_SYMBOL(OSS_AlarmCreate);
EXPORT_SYMBOL(OSS_AlarmRemove);
EXPORT_SYMBOL(OSS_AlarmSet);
EXPORT_SYMBOL(OSS_AlarmClear);
EXPORT_SYMBOL(OSS_Swap16);
EXPORT_SYMBOL(OSS_Swap32);
EXPORT_SYMBOL(OSS_DL_NewList);
EXPORT_SYMBOL(OSS_DL_Remove);
EXPORT_SYMBOL(OSS_DL_RemHead);
EXPORT_SYMBOL(OSS_DL_AddTail);
EXPORT_SYMBOL(OSS_irqLock);
EXPORT_SYMBOL(OSS_SpinLockCreate);
EXPORT_SYMBOL(OSS_SpinLockRemove);
EXPORT_SYMBOL(OSS_SpinLockAcquire);
EXPORT_SYMBOL(OSS_SpinLockRelease);

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*--- Module parameters ---*/

/** geographical slot mapping
 * This array defines the mapping between geographical PCI slot numbers
 * and the corresponding PCI device number. It is used by
 * OSS_PciSlotToPciDevice(). Currently the D201 board driver will call
 * this function when PCI_BUS_SLOT is specified in the descriptor.
 *
 * It defaults to the standard mapping for CompactPci backplanes.
 * It can be changed by the user by specifying oss_pci_slot_devnbrs=x,y,...
 * to the command line of insmod when the OSS module is loaded.
 * A value of -1 indicates that the slot is not available.
 * The index in the array is the geographical slot -1. I.e.
 * oss_pci_slot_devnbrs[1] specifies the device number of slot #2
 */

static int arr_argc=0;
#define MDIS_OSS_BUS_CNT 16
int oss_pci_slot_devnbrs[MDIS_OSS_BUS_CNT] = { -1, 0xf, 0xe, 0xd, 0xc, 0xb,
											   0xa, 0x9, -1, -1, -1, -1, -1,
											   -1, -1, -1 };

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
MODULE_PARM( oss_pci_slot_devnbrs, "8-16i" );
#else
module_param_array(oss_pci_slot_devnbrs, int, &arr_argc, 0664);
#endif



u_int32 OSS_dbgLev = OSS_DBG_DEFAULT;
DBG_HANDLE *OSS_dbgHdl;
sigset_t OSS_allRtSigs;			/* signal mask containing all real-time sigs */
sigset_t OSS_allSigs;			/* signal mask containing all signals */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

/**********************************************************************/
/** Initializes the OSS module
 *
 * Creates an instance of OSS and returns a handle for that instance.
 *
 * \rtai  It is assumed that this function is always in the context of
 *        standard Linux. If called from an RTAI task, this function may
 *		  block and therefore violate realtime behaviour.
 *
 * \param instName		\IN	name of calling module
 * \param ossP			\OUT contains the created handle for that instance
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 * \sa OSS_Exit
 */
int32  OSS_Init( char *instName, OSS_HANDLE **ossP )
{
	OSS_HANDLE *oss;

	*ossP = NULL;

	if( (oss = kmalloc( sizeof(OSS_HANDLE), GFP_KERNEL )) == NULL )
		return ERR_OSS_MEM_ALLOC;

	if( strlen(instName)+1 > sizeof(oss->instName))
		return ERR_OSS_ILL_PARAM;

	strcpy( oss->instName, instName );

	DBGINIT((NULL,&DBH));		/* init debugging */
	oss->dbgLevel = OSS_DBG_DEFAULT;

	DBGWRT_1((DBH,"OSS_Init %s\n", instName));
	*ossP = oss;

	/* init signal masks */
	{
		int sig;

		for( sig=SIGRTMIN; sig<=SIGRTMAX; sig++ ){
			sigaddset( &OSS_allRtSigs, sig );
		}
		sigfillset( &OSS_allSigs );
	}

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

	kfree( oss );
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

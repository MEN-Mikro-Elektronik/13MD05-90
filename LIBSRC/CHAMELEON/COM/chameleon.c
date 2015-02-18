/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  chameleon.c
 *
 *      \author  rla
 *        $Date: 2012/09/10 16:09:04 $
 *    $Revision: 1.26 $
 *
 *        \brief  routines for chameleon library
 *
 *            	  This module includes all important routines
 *				  of the chameleon library
 *
 *     Switches: MAC_IO_MAPPED / MAC_MEM_MAPPED
 *               MAP_REQUIRED - enables mapping/unmapping
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: chameleon.c,v $
 * Revision 1.26  2012/09/10 16:09:04  sy
 * R:1. only single PCI domain supported.
 *      2. in line 380, the complete BAR value, including attribute bits are used to determine if the BAR is set up
 * M: 1. multiple PCI domain supported, introduced new marco OSS_VXBUS_SUPPORT
 *       2. attribute bits are filtered out in determining if BAR is already set up
 *
 * Revision 1.25  2011/04/14 17:40:54  CRuff
 * R: support of pci domains
 * M: changed type of pciBus to u_int32 (from int)
 *
 * Revision 1.24  2010/09/02 15:04:39  UFranke
 * R: common lib
 * M: removed LINUX native stuff
 *
 * Revision 1.23  2010/08/23 17:40:41  CRuff
 * R: 1. chameleon lib is now built as core library (ALL_CORE_LIBS)
 * M: 1. added symbol exports for chameleon methods
 *
 * Revision 1.22  2009/08/19 11:04:56  CRuff
 * R: Unnecessary check which CHAM_INIT is called (done by chameleon.h)
 * M: removed check and use the general version
 *
 * Revision 1.21  2009/03/17 15:21:05  dpfeuffer
 * R: compiler error with VC2008 64bit compiler
 * M: (U_INT32_OR_64) cast added
 *
 * Revision 1.20  2008/11/13 11:24:25  channoyer
 * R:1. Set by error incorrect symbolic revision name
 *   2. Correction language by PU
 * M:1. Back to numeric revision
 *   2. Cosmetics
 *
 * Revision 1.19  2008/09/15 16:02:34  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.18  2008/09/05 15:10:44  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 1.17  2008/03/31 11:31:33  DPfeuffer
 * - documentation updated
 * - modified to run unit test
 * - ChameleonTerm(): error handling for V2 support fixed
 *
 * Revision 1.16  2007/09/11 19:52:06  channoyer
 * - cancel change about CHAM_Init. Add -DCHAMV2_VARIANT=Mem
 *
 * Revision 1.15  2007/09/06 16:29:32  channoyer
 * + add define CHAMELEONV0_STATIC_EXTERN to be able to compil chameleon.c directly (not include from chameleonv2.c)
 * - change CHAM_InitIo and CHAM_InitMem to CHAM_Init to conform to chameleonV2.c 2.14 change
 *
 * Revision 1.14  2007/07/11 09:13:40  CKauntz
 * changed extern to static for chameleonV0
 *
 * Revision 1.13  2007/06/27 12:00:50  cs
 * fix:
 *   - for Chameleon V1: don't mask modCode Bits 7 - 9
 *
 * Revision 1.12  2006/08/02 10:12:18  DPfeuffer
 * - ChameleonInit(): don't abort if OSS_BusToPhysAddr() fails
 *
 * Revision 1.11  2006/03/24 14:58:57  cschuster
 * changed:
 *     - return and access addresses retrieved with OSS_BusToPhysAddr()
 *
 * Revision 1.10  2005/06/23 16:11:59  kp
 * Copyright line changed (sbo)
 *
 * Revision 1.9  2005/04/29 14:45:18  dpfeuffer
 * - Chameleon-V1 device support implemented
 * - supports Chameleon-V2 devices by using Chameleon-V2 functions
 *
 * Revision 1.8  2004/11/22 10:01:49  ub
 * Added: support for IO-mapped access
 *
 * Revision 1.7  2004/05/24 11:31:48  dpfeuffer
 * defined(WINNT) added
 *
 * Revision 1.6  2003/08/06 13:01:11  kp
 * Added doxygen main page
 *
 * Revision 1.5  2003/04/02 10:21:01  kp
 * Linux/QNX: mapped only 4 bytes for config table. Now 0x80
 *
 * Revision 1.4  2003/03/17 12:39:05  ub
 * Fixed: OSS_MapPhysToVirtAddr() now called only at init
 * Fixed: virtual address space now unmapped at exit
 *
 * Revision 1.3  2003/02/13 13:57:59  ub
 * convert physical hardware addresses to virtual (for Linux)
 *
 * Revision 1.2  2002/11/28 13:23:10  kp
 * mask prefetchable bit in BARs
 *
 * Revision 1.1  2002/09/11 10:13:30  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

/*! \mainpage

 The Chameleon library provides functions to get information about the units,
 bus architecture and structure of Chameleon devices implemented within FPGAs.

 This common library is used in MENMON, BSPs and drivers to perform common
 actions on Chameleon devices and is used as an information data base.\n\n

 <b>Chameleon Library Glossary:</b>\n
 A <em>Chameleon FPGA</em> can implement one ore more Chameleon devices.

 A <em>Chameleon Device</em> is the whole chunk of all Chameleon sub-devices
 (e.g. units, bridges, CPUs) connected via internal buses.

 A <em>Chameleon Function</em> is one complex function built with several
 Chameleon units (display controller, bit blitter, SDRAM window).

 A <em>Chameleon Unit</em> is one simple function (e.g. an SDRAM window) within
 a Chameleon device.

 A <em>Chameleon Table Tree</em> consists of all Chameleon cables linked together
 which describes a whole Chameleon device (all internal buses of the Chameleon
 device).

 A <em>Chameleon Table</em> describes all Chameleon sub-devices (e.g. units,
 bridges, CPUs) on one Chameleon device internal bus.\n\n


 <b>Chameleon Library Functions:</b>

 The library supports Chameleon-V0/V1 and Chameleon-V2 tables with
 the following function groups:

 - The \ref _cham0func were implemented for the first Chameleon table
 version (V0) and updated for Chameleon-V1 tables but supports also
 Chameleon-V2 tables (by using internally the Chameleon-V2 functions).

 - The \ref _cham2func were added to provide the extended information
 from Chameleon-V2 tables but supports also Chameleon-V0/V1 tables
 (by using internally the Chameleon-V0 functions).

 - The \ref _CHAM_CODE_FUNC can be used to convert between Chameleon-V0/V1
   module codes and Chameleon-V2 device IDs and to get the name of a
   unit for a module code or device ID.

 - ChameleonLoadxxx() Functions\n
 The board specific ChameleonLoadxxx() (re-)loads the FPGA with the
 specified code. It is a special function in that it doesn't require a
 handle (it can be called without calling V0::ChameleonInit(), V2::InitPci(),
 V2::InitInside() before). fpgaCode must point to the FPGA code's header followed by
 the FPGA code.\n

\section Chameleon Unit Test
For library verification (Chameleon-V0 and Chameleon-V2 functions) in User Mode
without real hardware, the CHA_UT unit test is provided. See \example cha_ut.c \n

<b>The CHA_UT unit test must always be executed with success after each modification
of the library!</b>

*/

/***************************************************************************/
/*! \page _cham0func Chameleon-V0 Functions

 <b>Attention: The \ref _CHAM_FUNC are deprecated and should notbe used for new
 software designs!</b>\n

 V0::ChameleonInit() creates an handle for  a Chameleon device. This handle
 must be passed to all further V0 functions. V0::ChameleonTerm() destroys
 the created handle.\n
 Note: The handle of the V0 functions is different from the handle of the V2 functions.

 V0::ChameleonGlobalIdent() retrieves the global information (from the Chameleon
 table header) of a Chameleon device.\n

 V0::ChameleonUnitIdent() retrieves the information about all Chameleon units of
 the Chameleon device.\n

 V0::ChameleonUnitFind() searches for a Chameleon unit with the specified
 characteristic(s).\n

 */

/*! \page dummy
 \menimages
*/

#ifdef CHAM_UNITTEST
	#include "../UNITTEST/COM/cha_ut.h"
#endif

#ifndef CHAMELEONV0_STATIC_EXTERN
	#define  CHAMELEONV0_STATIC_EXTERN extern
#endif

/*--------------------------------------*/
/*    INCLUDES                          */
/*--------------------------------------*/
#include "chameleon_int.h"

/*--------------------------------------*/
/*	DEFINES			            		*/
/*--------------------------------------*/
#define SUPPORT_CHAM_V2

/*--------------------------------------*/
/*	TYPEDEFS     	                    */
/*--------------------------------------*/
/** chameleon V0 handle */
typedef struct {
	OSS_HANDLE *osh;
	u_int32 size;
	u_int16 pciDevId;
	u_int32 pciBus; /*contains pciDomain in upper 8 bit!*/
	int pciDev;
	u_int32 bar[6];
	int numMods;				/* modules in config table */
	MACCESS ma;
	u_int8 chaRev;				/* chameleon revision */
#ifdef MAP_REQUIRED
	u_int8 mapped;				/* 1: address was mapped */
#endif
#ifdef SUPPORT_CHAM_V2
	/* Chameleon-V2 extension */
	CHAM_FUNCTBL f2;			/* chameleon-V2 function-table */
	CHAMELEONV2_HANDLE *h2;		/* chameleon-V2 handle */
#endif
} CHAMELEON_HDL;

/*--------------------------------------*/
/*	GLOBALS     	                    */
/*--------------------------------------*/

/*--------------------------------------*/
/*	PROTOTYPES     	                    */
/*--------------------------------------*/

/**
 * \defgroup _CHAM_FUNC Chameleon-V0 Functions
 *  @{
 */

/**********************************************************************/
/** routine to teminate Chameleon device
 *  It destroys the handle for that device and free the memory
 *  and sets the contents of *chahP to NULL
 *
 *	\param chahP		pointer to CHAMELEON_HANDLE
 *
 *  \return 0=ok, or error number
 */

CHAMELEONV0_STATIC_EXTERN
void ChameleonTerm( CHAMELEON_HANDLE **chahP)
{
	CHAMELEON_HDL *chah = (CHAMELEON_HDL *)*chahP;

#ifdef SUPPORT_CHAM_V2
	if( chah->chaRev == 2 ){
		if( chah->h2 )
			chah->f2.Term( &chah->h2 );
	}
#endif

#ifdef MAP_REQUIRED
	if( chah->mapped == TRUE ){
		OSS_UnMapVirtAddr( chah->osh, (void**)&(chah->ma), CFGTABLE_SIZE,
			#ifdef MAC_IO_MAPPED
						  OSS_ADDRSPACE_IO );
			#else
						  OSS_ADDRSPACE_MEM );
			#endif
	}
#endif

	OSS_MemFree(chah->osh,(char*)chah,chah->size);
	*chahP = NULL;
}

/**********************************************************************/
/** routine to init Chameleon device
 *
 *  Before calling this routine, the BARs of the PCI device must be filled!
 *
 *  It creates the handle for that device.
 *
 *  This routine
 *  - Checks the presence of the PCI device (not the PCI device/vendor ID!)
 *	- Reads the base address registers
 *	- Checks if the configuration table contains the proper sync word
 *  - Determines the number of units in the configuration table
 * I
 *  \param osh          OSS handle, returned by OSS_Init
 *  \param pciBus		PCI Bus Id (contains pciDomain in upper 8 bit!)
 *  \param pciDev		PCI Device Id
 *	\param chahP		pointer to variable where handle will be stored
 *
 *  \return 0=ok, or error number CHAMELEON_xxx
 *	\return *chahP = handle
 */

CHAMELEONV0_STATIC_EXTERN
int32 ChameleonInit( OSS_HANDLE *osh,
#ifdef OSS_VXBUS_SUPPORT
					 VXB_DEVICE_ID busCtrlID,
#endif
					 int pciBus,
					 int pciDev,
					 CHAMELEON_HANDLE **chahP)

{
	CHAMELEON_HDL *chah;
	u_int32 hGotSize=0;
	int32 val=0, i;
	u_int32 error=0;
	u_int16 magicWord;

	*chahP = NULL;
	
	/*------------------------------+
	|  check if PCI device present  |
	+------------------------------*/	
	if( (error = OSS_PciGetConfig(osh,
#ifdef OSS_VXBUS_SUPPORT			
			busCtrlID,
#endif			
			pciBus,pciDev,0,OSS_PCI_DEVICE_ID,&val)) == 0)
	{
		if ( val == 0xFFFF ){
			return CHAMELEON_DEV_NOT_FOUND;
		}
	}
	else
		return CHAMELEON_PCI_ERR;
	
	/*-----------------------------+
	|  Create and init handle     |
	+-----------------------------*/
	if( (chah = (CHAMELEON_HDL *)
	  OSS_MemGet( osh, sizeof(CHAMELEON_HDL), &hGotSize )) == NULL )
		return CHAMELEON_OUT_OF_RESOURCE;

	OSS_MemFill( osh, sizeof(*chah), (char*)chah, 0 );

	chah->size = hGotSize;
	chah->osh = osh;
	chah->pciDevId = (u_int16)val;
	chah->pciBus = pciBus;
	chah->pciDev = pciDev;

	/*-------------------------+
	|  Read base address regs  |
	+-------------------------*/
	for( i = 0; i < NBR_OF_BARS; i++ ){
		
		OSS_PciGetConfig(osh,
#ifdef OSS_VXBUS_SUPPORT
				busCtrlID,
#endif				
				pciBus, pciDev, 0, OSS_PCI_ADDR_0+i, &val);

		/* bar unassigned? */
		if( ((val & 0xFFFFFFF0) == 0xfffffff0) || ((val & 0xFFFFFFF0) == 0x00000000) ){
			 chah->bar[i] = 0xffffffff;
		}
		/* bar assigned! */
		else{
			/* get translated address of the BAR */
			if( OSS_BusToPhysAddr(osh, OSS_BUSTYPE_PCI,
				(void**)&chah->bar[i], pciBus, pciDev, 0, i ) )
				/* no translated address available */
				chah->bar[i] = 0xffffffff;
		}
	}
	
	/*---------------------------------+
	|  Set & convert hardware address  |
	+----------------------------------*/
	chah->ma = CFGTABLE_MA(chah);
	
#ifdef MAP_REQUIRED

    if( ( OSS_MapPhysToVirtAddr(
			  osh, (void*)chah->ma, CFGTABLE_SIZE,
#ifdef MAC_IO_MAPPED
			  OSS_ADDRSPACE_IO,
#else
			  OSS_ADDRSPACE_MEM,
#endif
			  OSS_BUSTYPE_PCI, pciBus, (void**)&(chah->ma) ) != 0 ) ) {

		ChameleonTerm( (CHAMELEON_HANDLE **)&chah );
        return CHAMELEON_UNIT_NOT_FOUND;
    }
	chah->mapped = TRUE;
#endif

	/*--------------------------------------------+
	|  Check if config table contains sync word   |
	+--------------------------------------------*/
	magicWord = (u_int16)CFGTABLE_READWORD( chah->ma, 1 );
	
	/* determine chameleon table revision */
	switch( magicWord ){
		case CHAMELEON_MAGIC_00:
			chah->chaRev = 0;
			break;
		case CHAMELEON_MAGIC_01:
			chah->chaRev = 1;
			break;
#ifdef SUPPORT_CHAM_V2
		case CHAMELEON_MAGIC_02:
			chah->chaRev = 2;
			break;
#endif
		default:
			error = CHAMELEON_VERIFY_ERR;
			ChameleonTerm( (CHAMELEON_HANDLE **)&chah );
			return error;
	}
	
#ifdef SUPPORT_CHAM_V2
	if( chah->chaRev == 2 ){
		CHAMELEONV2_INFO info;

		/* fill V2 function-table */
		CHAM_Init( &chah->f2 );

		/* V2 Init */
		if( (error=chah->f2.InitPci(
			osh,
#ifdef OSS_VXBUS_SUPPORT
			busCtrlID,
#endif			
			pciBus, pciDev, 0, &chah->h2)) ){
			ChameleonTerm( (CHAMELEON_HANDLE **)&chah );
			return error;
		}
		/* get number of units */
		if( (error=chah->f2.Info( chah->h2, &info )) ){
			chah->f2.Term( &chah->h2 );
			ChameleonTerm( (CHAMELEON_HANDLE **)&chah );
			return error;
		}
		chah->numMods = info.unitNbr;
		*chahP = chah;
		return 0;
	}
#endif

	*chahP = chah;

	/*--------------------------------------------+
	|  Determine number of units in config table  |
	+--------------------------------------------*/
	for( i=1; i<256; i++ ){
		/* check for end marker (device is all 1's) */
		if( (CFGTABLE_READWORD( chah->ma, i*2 ) & 0x3f0 ) == 0x3f0 )
			break;
	}
	chah->numMods = i-1;
	
	return error;
}

/**********************************************************************/
/** routine to get Chameleon global device info
 *
 *  It retrieves the global variant and revision of the FPGA code.
 *
 *	\param h			CHAMELEON_HANDLE returned from ChameleonInit()
 *  \param variantP		pointer to variable where variant of FPGA code will
 *						be stored. (ASCII code 'A'-'Z')
 *  \param revisionP	pointer to variable where revision of FPGA code will
 *						be stored. (one binary byte)
 *
 *  \return 0 or error code
 */

CHAMELEONV0_STATIC_EXTERN
int32 ChameleonGlobalIdent(
	CHAMELEON_HANDLE *h,
	char *variantP,
	int *revisionP)
{
	CHAMELEON_HDL *chah = (CHAMELEON_HDL *)h;
	u_int16 val;

#ifdef SUPPORT_CHAM_V2
	if( chah->chaRev == 2 ){
		CHAMELEONV2_TABLE	table;
		u_int32				error;

		if( (error=chah->f2.TableIdent( chah->h2, 0, &table )) )
			return error;

		*revisionP = table.revision;
		*variantP  = table.model;
		return 0;
	}
#endif

	/* get FPGA revision/variant */
	val = (u_int16)CFGTABLE_READWORD( chah->ma, 0 );

	*revisionP = val & 0xff;
	*variantP  = (char )(val >> 8);

	return 0;
}

/**********************************************************************/
/** Retrieve information about specified unit
 *
 *  Retrieve information about the unit at index \a idx in the FPGA
 *  configuration table. This function is typically used to list all
 *  available units.
 *
 *  \param h   			CHAMELEON_HANDLE returned from ChameleonInit()
 *  \param idx			index of unit to query (0..n)
 *  \param info			ptr to struct where information will be stored
 *
 *  \return 0 or error code
 */

CHAMELEONV0_STATIC_EXTERN
int32 ChameleonUnitIdent(
	CHAMELEON_HANDLE *h,
	int idx,
	CHAMELEON_UNIT *info)
{
	CHAMELEON_HDL *chah = (CHAMELEON_HDL *)h;
	u_int16 val1, val2;

	if( idx >= chah->numMods )
		return CHAMELEON_OUT_OF_RANGE;

#ifdef SUPPORT_CHAM_V2
	if( chah->chaRev == 2 ){
		CHAMELEONV2_UNIT	unit;
		u_int32				error;

		if( (error=chah->f2.UnitIdent( chah->h2, idx, &unit )) )
			return error;

		info->modCode 	= CHAM_DevIdToModCode( unit.devId );
		info->revision  = unit.revision;
		info->instance	= unit.instance;
		info->interrupt	= unit.interrupt;
		info->bar		= unit.bar;
		info->offset	= unit.offset;
		info->addr 		= unit.addr;
		return 0;
	}
#endif

	/* get FPGA revision/variant */
	val1 = (u_int16)CFGTABLE_READWORD( chah->ma, (idx+1)*2 );
	val2 = (u_int16)CFGTABLE_READWORD( chah->ma, (idx+1)*2+1 );

	if( chah->chaRev == 0 )
		info->modCode = (u_int16)((val1 & 0x03f0) >> 4);	/* V0 */
	else
		info->modCode = (u_int16)((val1 & 0x1ff0) >> 4);	/* V1 */

	info->revision  = (u_int16)((val1 & 0x000f));
	info->instance	= (u_int16)((val2 & 0x3c00) >> 10);
	info->interrupt	= (u_int16)((val2 & 0x03c0) >> 6);

	if( chah->chaRev == 0 )
		info->bar	= (u_int16)((val2 & 0xc000) >> 14);	/* V0 */
	else
		info->bar	= (u_int16)(((val1 & 0x2000) >> 11) |
					            ((val2 & 0xc000) >> 14));	/* V1 */

	info->offset	= (val2 & 0x003f) * 0x100;

	info->addr 		= (void *)(U_INT32_OR_64)(chah->bar[info->bar] + info->offset);

	return CHAMELEON_OK;
}
#ifdef CHAMELEONV2_EXPORT_CHAMELEONV0
/**********************************************************************/
/** Search configuration table for unit
 *
 *  Searches the configuration table for the specified module code and
 *  instance number. For example, to search Frodo UART, 2nd instance,
 *  pass \a modCode as 7 and \a instance as 1. If \a modCode and/or
 *  instance is specified as -1, the function ignores that parameter
 *
 *  <b>Compatibility\n</b>
 *  If used with Chameleon-V2 tables, not all instances may be found
 *  because Chameleon-V2 tables may use one and the same instance number
 *  several times within the tables of one chameleon system (because
 *  pluggable busses).
 *
 *	\param h			CHAMELEON_HANDLE returned from ChameleonInit()
 *  \param modCode		module code to search (CHAMELEON_xxx)
 *	\param instance		nth instance to search for (0..n)
 *  \param info			ptr to struct where information will be stored
 *
 *  \return 0 or error code
 */

CHAMELEONV0_STATIC_EXTERN
int32 ChameleonUnitFind(
	CHAMELEON_HANDLE *h,
	int modCode,
	int instance,
	CHAMELEON_UNIT *info)
{
	int idx = 0;

	while( ChameleonUnitIdent( h, idx, info ) == CHAMELEON_OK ){

		if( ((info->modCode == modCode) || (modCode == -1 )) &&
			((info->instance == instance) || (instance == -1)))

			return CHAMELEON_OK;

		idx++;
	}

	return CHAMELEON_UNIT_NOT_FOUND;
}

#endif /* CHAMELEONV2_EXPORT_CHAMELEONV0 */
/*! @} */




/*********************  P r o g r a m  -  M o d u l e **********************/
/*!
 *        \file  smb2_com.c
 *     \project  SMB2 library
 *
 *       Author  Christian.Schuster@men.de
 *        $Date: 2012/02/17 08:17:53 $
 *    $Revision: 1.19 $
 *
 *       \brief  common function of system managment bus drivers
 *
 *     Required: -
 *    \switches  SMB_NO_ALERT  disable alert, no need of oss_dl_list, oss_alarm
 *               MAC_USERSPACE
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: smb2_com.c,v $
 * Revision 1.19  2012/02/17 08:17:53  dpfeuffer
 * R: very long lines
 * M: cosmetics
 *
 * Revision 1.18  2010/09/24 13:37:37  CKauntz
 * R: DBGWRT failed with zero data pointer
 * M: Fixed debug message
 *
 * Revision 1.17  2010/09/10 13:20:15  UFranke
 * R: bootrom size optimization
 * M: added switch SMB_NO_ALERT
 *
 * Revision 1.16  2010/06/14 13:29:32  dpfeuffer
 * R: Windows PREfast warnings
 * M: smbComSmbXferEmulate() and debug prints modified
 *
 * Revision 1.15  2009/08/21 08:02:30  dpfeuffer
 * R: Windows PREfast warning
 * M: smbComSmbXferEmulate(): DBGWRT_ERR fixed
 *
 * Revision 1.14  2008/09/15 16:23:22  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.13  2007/11/15 17:40:30  cs
 * fixed debug output in XferEmulate
 *
 * Revision 1.12  2007/10/09 14:43:15  GLeonhardt
 * Replace implicit initializing of array, because of xcc from OS9.
 *
 * Revision 1.11  2007/09/12 12:06:56  ufranke
 * added
 *  + debugs at error
 *  + smbComUseOssDelay()
 *
 * Revision 1.10  2007/07/13 17:38:53  cs
 * removed SMBALERT support when compiled for user space (no OSS_Alarm* support)
 *
 * Revision 1.9  2007/02/20 15:10:26  DPfeuffer
 * variable type changed for VC compiler
 *
 * Revision 1.8  2006/09/07 22:22:23  cs
 * cosmetics
 *
 * Revision 1.7  2006/02/27 15:26:45  DPfeuffer
 * - smbComExit(): bugfix: semaphore is required for alarm routine
 * - cmbComAlertCbRemove(): bugfix: empty list recognition was wrong
 * - NO_CALLBACK switch removed
 *
 * Revision 1.6  2006/02/22 16:07:41  DPfeuffer
 * cosmetics
 *
 * Revision 1.5  2006/02/22 16:02:04  DPfeuffer
 * cmbComAlertCbRemove(): 'void **cbArgP' parameter added
 *
 * Revision 1.4  2006/02/07 19:12:31  cschuster
 * bugfix: always initialize function pointers
 * added:
 *     + filled smbComSmbXferEmulate()
 *
 * Revision 1.3  2005/12/12 10:59:49  cschuster
 * updated documentation
 *
 * Revision 1.2  2005/11/23 09:19:22  dpfeuffer
 * modifications for windows compiler
 *
 * Revision 1.1  2005/11/22 08:45:13  cschuster
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005..2010 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
/*! \mainpage
    This is the documentation of the SMBus library.
    The library provides functions to access diverse SMBus/I2C controllers,
    depending on the built.

    \section supported_controllers Supported Controllers
    The SMBus library supports currently the following controllers:
    - MEN 16Z001_SMB	FPGA SMB host controller
    - INTEL ICH 		SMB host controller
	- MGT5200/MPC85XX   I2C controller

    \n \section FuncDesc Functional Description

    \n \subsection General General/Initialization
    A controller specific smbInit function is called. This function returns
    an array of function pointers, which provide the SMBus access functions
    specific to the SMBus/I2C controller.
	The library also supports some special SMBus protocols such as the Alert
	Response Protocol.

    \n \section Documents Overview of all Documents

    \subsection smb2_com  Common functions
    smb2_com.c
    This file provides a set of functions that are common to all controllers.
    This functions performs SMB protocol specific tasks such as SMB_ALERT
    handling, software PEC, emulating the SMBus protocol on an I2C bus, ...

	\subsection smb2_specific Controller specific functions
	To every controller supported there exists an own file, smb2_***.c
	which provides the controller specific functions to the library.
*/


/* #define DBG */

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/mdis_err.h>
#include <MEN/smb2.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/** Double linked List node to keep registered callbacks for SMBus device alerts */
#ifndef SMB_NO_ALERT
typedef struct
{
	OSS_DL_NODE node;					/**< Double Linked List node */
	u_int16 addr;						/**< SMBus address this node represents */
	void (*alertCb)( void *cbArg );		/**< callback function registered */
	void *alertCbArg;					/**< argument for callback function */
	u_int32 gotsize;					/**< memory allocated for this node */
}SMB_ALERT_NODE;
#endif
/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
#define	DBG_MYLEVEL		smbComHdl->dbgLevel
#define DBH				smbComHdl->dbgHdl

#define OSSH			smbComHdl->osHdl
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/
DBGCMD(	static const char errorStartStr[] =	"*** ERROR - ";	)
DBGCMD(	static const char errorLineStr[]  =	" (line	";		)
DBGCMD(	static const char errorEndStr[]	  =	")***\n";		)
/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int32 smbComQuickComm( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 read_write );
static int32 smbComWriteByte( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 data );
static int32 smbComReadByte( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 *data );
static int32 smbComWriteByteData( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 cmdAddr, u_int8 data );
static int32 smbComReadByteData( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 cmdAddr, u_int8 *data );
static int32 smbComWriteWordData( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 cmdAddr, u_int16 data );
static int32 smbComReadWordData( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 cmdAddr, u_int16 *data );
static int32 smbComWriteBlockData( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 comAddr,
								u_int8 length, u_int8 *data);
static int32 smbComReadBlockData( SMB_HANDLE *smbHdl, u_int32 flags,
								 u_int16 addr, u_int8 comAddr,
								 u_int8 *length, u_int8 *data);
static int32 smbComProcessCall( SMB_HANDLE *smbHdl, u_int32 flags,
								u_int16 addr, u_int8 cmdAddr, u_int16 *data );
static int32 smbComBlockProcessCall( SMB_HANDLE *smbHdl, u_int32 flags,
									u_int16 addr, u_int8 cmdAddr,
									u_int8 writeDataLen, u_int8 *writeData,
									u_int8 *readDataLen, u_int8 *readData );
static int32 smbComSmbXfer( SMB_HANDLE *smbHdl, u_int32 flags, u_int16 addr,
							u_int8 read_write, u_int8 cmdAddr,
							u_int8 size, u_int8 *data );
static int32 smbComSmbXferEmulate(	SMB_HANDLE *smbHdl,	u_int32 flags,
									u_int16 addr, u_int8 read_write,
									u_int8 cmdAddr, u_int8 size, u_int8 *data );
#ifndef SMB_NO_ALERT
	static void smbComAlertPoll( void *smbHdl );
	static int32 smbComAlertResponse( SMB_HANDLE *smbHdl, u_int32 flags,
										u_int16 addr, u_int16 *alertCnt);
	static int32 smbComAlertCbInstall( SMB_HANDLE *smbHdl, u_int16 addr,
										void (*cbFuncP)( void *cbArg ), void *cbArg );
	static int32 cmbComAlertCbRemove( SMB_HANDLE *smbHdl, u_int16 addr, void **cbArgP );
#endif /* SMB_NO_ALERT */

static int32 smbComAddPec( u_int16 addr, u_int8 cmdAddr, u_int8 size,
							u_int8 *data );
static int32 smbComCheckPec( u_int16 addr, u_int8 cmdAddr, u_int8 size,
							 u_int32 partial, u_int8 *data );

static int32 smbComUseOssDelay( SMB_HANDLE *smbHdl, int useOssDelay );

static u_int32 smbComExit( void *smbHdlP );


/****************************** SMB_COM_Init ********************************/
/** Initializes common part of library.
 *
 *  \param   smbHdlP \IN    valid SMB handle
 *
 *  \return  \c  0 | error code
 *
 ****************************************************************************/
u_int32 SMB_COM_Init
(
	void *smbHdlP
)
{
	u_int32     error  = 0;
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)smbHdlP;
	u_int32 retVal;
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE *)((u_int8*)smbHdlP +
														   sizeof(SMB_ENTRIES));
	DBGCMD(	static const char functionName[] = "SMB2 - SMB_COM_Init"; )

	if( smbHdl == NULL )
	{
		error = SMB_ERR_NO_MEM;
		goto ERR_EXIT;
	}
	DBGWRT_1( (DBH, "%s smbHdlP %08p\n", functionName, smbHdlP ) );

	/* init semaphore for blocking mechanism */
	if( (error = OSS_SemCreate( OSSH, OSS_SEM_BIN, 1,
								&smbComHdl->hostCtrlSem)) )
	{
		DBGWRT_ERR( (DBH, "*** %s: can't create sem: err=%d!!!\n",
					functionName, error) );
		goto ERR_EXIT;
	}

#ifndef SMB_NO_ALERT
	/* create list for SMB alert callbacks */
	OSS_DL_NewList( &smbComHdl->smbAlertCb );
#endif

	smbHdl->QuickComm = (int32 (*)(void*,u_int32,u_int16,u_int8))
		                          smbComQuickComm;

	smbHdl->WriteByte = (int32 (*)(void*,u_int32,u_int16,u_int8))
		                          smbComWriteByte;

	smbHdl->ReadByte = (int32 (*)(void*,u_int32,u_int16,u_int8*))
								 smbComReadByte;

	smbHdl->WriteByteData	= (int32 (*)(void*,u_int32,u_int16,
										 u_int8,u_int8))
									    smbComWriteByteData;

	smbHdl->ReadByteData	= (int32 (*)(void*,u_int32,u_int16,
											 u_int8,u_int8*))
									    smbComReadByteData;

	smbHdl->WriteWordData	= (int32 (*)(void*,u_int32,u_int16,
											 u_int8,u_int16))
									    smbComWriteWordData;

	smbHdl->ReadWordData	= (int32 (*)(void*,u_int32,u_int16,
											 u_int8,u_int16*))
									    smbComReadWordData;

	smbHdl->WriteBlockData	= (int32 (*)(void*,u_int32,u_int16,
											 u_int8,u_int8,u_int8*))
									    smbComWriteBlockData;

	smbHdl->ReadBlockData	= (int32 (*)(void*,u_int32,u_int16,
											 u_int8,u_int8*,u_int8*))
									    smbComReadBlockData;

	smbHdl->ProcessCall		= (int32 (*)(void*,u_int32,u_int16,
											 u_int8,u_int16*))
									    smbComProcessCall;

	smbHdl->BlockProcessCall= (int32 (*)(void *,u_int32,u_int16,
											 u_int8,u_int8,u_int8*,
											 u_int8*,u_int8*))
									    smbComBlockProcessCall;

	smbHdl->UseOssDelay = (int32 (*)(void *,int))smbComUseOssDelay;

#ifndef SMB_NO_ALERT
	smbHdl->AlertResponse = (int32 (*)(void *, u_int32, u_int16,u_int16*))
										smbComAlertResponse;

	smbHdl->AlertCbInstall = (int32 (*)(void *,u_int16, void (*)( void*), void*))
										smbComAlertCbInstall;

	smbHdl->AlertCbRemove = (int32 (*)(void *,u_int16,void**))cmbComAlertCbRemove;

	if( smbComHdl->alertPollFreq )
	{
		#ifndef MAC_USERSPACE
				/* start SMBALERT polling */
				if( (error = OSS_AlarmCreate( OSSH,
											  smbComAlertPoll,
											  (void *)smbHdl,
											  &smbComHdl->alarmHdl)) )
				{
					DBGWRT_ERR( (DBH, "*** %s: can't create alarm: err=%d!!!\n",
								functionName, error) );
					goto ERR_EXIT;
				}
		
				if( (error = OSS_AlarmSet( OSSH,
										   smbComHdl->alarmHdl,
		    							   (u_int32)smbComHdl->alertPollFreq,
		    							   1,
		    							   &retVal)) )
				{
					DBGWRT_ERR( (DBH, "*** %s: can't set alarm to cyclic all %d ms: "
								"err=%d!!!\n",
								functionName, (int)smbComHdl->alertPollFreq, error) );
					goto ERR_EXIT;
				}
		#else
				printf("*** SMB_COM_Init - Error: alertPoll / Alarms not supported in user space\n");
		#endif
	}
#endif /*SMB_NO_ALERT*/


ERR_EXIT:
	smbComHdl->ExitCom = (int32 (*)(void*))smbComExit;
	return(error);
}/*smbComInit*/

/*******************************  smbComExit  *****************************/
/**  Deinitializes this library.
 *
 *  \param     smbHdlP	pointer to SMB handle
 *
 *  \return    always 0
 *
 ****************************************************************************/
static u_int32 smbComExit
(
	void *smbHdlP
)
{
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE*)((u_int8*)smbHdlP + sizeof(SMB_HANDLE));

#ifndef SMB_NO_ALERT
	#ifndef MAC_USERSPACE
		/* remove alarm */
		if( smbComHdl->alarmHdl )
			OSS_AlarmRemove( OSSH, &smbComHdl->alarmHdl );
	#endif /* MAC_USERSPACE */
#endif /*SMB_NO_ALERT*/

	/* destroy semaphore */
	if( smbComHdl->hostCtrlSem )
		OSS_SemRemove( OSSH, &smbComHdl->hostCtrlSem );

#ifndef SMB_NO_ALERT
	{
		SMB_ALERT_NODE* remNode;
		while( (remNode = (SMB_ALERT_NODE*)OSS_DL_RemHead(&smbComHdl->smbAlertCb)) )
			OSS_MemFree(OSSH, (u_int8*)remNode, remNode->gotsize);
	}
#endif /*SMB_NO_ALERT*/

	return( 0 );
}/*smbComExit*/

/****************************** smbComUseOssDelay ********************************/
/** Setup used OSS_Delay or OSS_MikroDelay.
 *
 *  \param   smbHdlP \IN	    valid SMB handle
 *  \param   useOssDelay \IN    1 for OSS_Delay or 0
 *
 *  \return  \c  0 | error code
 *
 ****************************************************************************/
static int32 smbComUseOssDelay( SMB_HANDLE *smbHdl, int useOssDelay )
{
	u_int32 error = SMB_ERR_NOT_SUPPORTED;
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE*)((u_int8*)smbHdl + sizeof(SMB_HANDLE));
	DBGCMD(	static const char functionName[] = "SMB2 - smbComSetupUsedInternalOss_xDelay"; )

	if( smbHdl == NULL || smbHdl->UseOssDelay == NULL )
	{
		goto CLEANUP;
	}

	DBGWRT_1((DBH, "%s: smbHdl %08p useOssDelay %d\n", functionName, smbHdl, useOssDelay ));

	if( (error = OSS_SemWait( OSSH, smbComHdl->hostCtrlSem, smbComHdl->busyWait)) )
	{
		DBGWRT_ERR( (DBH, "*** %s: OSS_SemWait\n", functionName) );
		error = SMB_ERR_BUSY;
		goto CLEANUP;
	}

	error = smbHdl->UseOssDelay(smbHdl, useOssDelay);
	if( error )
	{
		DBGWRT_ERR( (DBH, "*** %s: smbHdl %08p line %d\n",	functionName, smbHdl, __LINE__ ));
	}

	OSS_SemSignal(OSSH, smbComHdl->hostCtrlSem);

CLEANUP:
	return( error );
}


/******************************** smbComQuickComm **************************/
/** Perform a quick access to a device.
 *  The r/w bit of the address is the actual command issued to the device.
 *  This bit is set from the data parameter provided.
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	  valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr	      device address
 *	\param     read_write access to perform ( SMB_READ or SMB_WRITE )
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static int32 smbComQuickComm
(
	SMB_HANDLE  *smbHdl,
	u_int32		flags,
	u_int16		addr,
	u_int8		read_write
)
{
	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_QUICK) )
		return SMB_ERR_NOT_SUPPORTED;

	return(smbComSmbXfer(smbHdl, flags, addr, read_write, 0, SMB_ACC_QUICK, 0));
}/*smbComQuickComm*/

/******************************** smbComReadByte ***************************/
/**  Read a byte from a device.
 *
 *  \param     smbHdl   valid SMB handle
 *	\param     flags    flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr	    device address
 *	\param     *data     pointer to variable where value will be stored
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static int32 smbComReadByte
(
	SMB_HANDLE  *smbHdl,
	u_int32		flags,
	u_int16		addr,
	u_int8		*data
)
{
	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_READ_BYTE) )
		return SMB_ERR_NOT_SUPPORTED;

	return( smbComSmbXfer(smbHdl, flags, addr, SMB_READ,
							0, SMB_ACC_BYTE, data) );
}/*smbComReadByte*/

/******************************** smbComWriteByte **************************/
/**  Write a byte to a device.
 *
 *  \param     smbHdl   valid SMB handle
 *	\param     flags    flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr	    device address
 *	\param     data     value to write
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static int32 smbComWriteByte
(
	SMB_HANDLE	*smbHdl,
	u_int32		flags,
	u_int16		addr,
	u_int8		data
)
{
	int32 error;
	DBGCMD( SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE*)((u_int8*)smbHdl + sizeof(SMB_HANDLE)) );

	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_WRITE_BYTE) )
	{
		error = SMB_ERR_NOT_SUPPORTED;
		goto CLEANUP;
	}

	error = smbComSmbXfer(smbHdl, flags, addr, SMB_WRITE, data, SMB_ACC_BYTE, 0);

CLEANUP:
	if( error )
	{
		DBGWRT_ERR( (DBH, "*** %s: smbHdl %08p flags %08x addr %04x\n",	__FUNCTION__, smbHdl, flags, addr ));
	}
	return( error );
}/*smbComWriteByte*/

/******************************** smbComReadByteData ***********************
 *
 *  Description:  Writes a command or index byte to
 *                and read one byte from a device.
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr		device address
 *	\param     cmdAddr   device command or index value
 *	\param     *data     pointer to variable where value will be stored
 *
 *  Output.....:  return    0 | error code
 *                *data     read byte value
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbComReadByteData
(
	SMB_HANDLE  *smbHdl,
	u_int32		flags,
	u_int16     addr,
	u_int8      cmdAddr,
	u_int8      *data
)
{
	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_READ_BYTE_DATA) )
		return SMB_ERR_NOT_SUPPORTED;

	return( smbComSmbXfer(smbHdl, flags, addr, SMB_READ,
							cmdAddr, SMB_ACC_BYTE_DATA, data) );
}/*smbComReadByteData*/

/******************************** smbComWriteByteData ***********************
 *
 *  Description:  Writes command and 1 data byte to a device.
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr		device address
 *	\param     cmdAddr   device command or index value
 *	\param     data      value to write
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbComWriteByteData
(
	SMB_HANDLE  *smbHdl,
	u_int32		flags,
	u_int16     addr,
	u_int8      cmdAddr,
	u_int8      data
)
{
	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_WRITE_BYTE_DATA) )
		return SMB_ERR_NOT_SUPPORTED;

	return( smbComSmbXfer( smbHdl, flags, addr, SMB_WRITE,
							cmdAddr, SMB_ACC_BYTE_DATA, &data) );
}/*smbComWriteByteData*/

/***************************** smbComReadWordData ***************************
 *
 *  Description:  Writes a command or index byte to
 *                and read two bytes from a device.
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr		device address
 *	\param     cmdAddr   device command or index value to write
 *	\param     *data     pointer for data read
 *
 *  \return    0 | error code
 *                *data     read word value
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbComReadWordData
(
	SMB_HANDLE  *smbHdl,
	u_int32		flags,
	u_int16     addr,
	u_int8      cmdAddr,
	u_int16     *data
)
{
	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_READ_WORD_DATA) )
		return SMB_ERR_NOT_SUPPORTED;

	return( smbComSmbXfer(smbHdl, flags, addr, SMB_READ,
							cmdAddr, SMB_ACC_WORD_DATA, (u_int8*)data) );
}/*smbWriteReadTwoByte*/

/******************************** smbComWriteWordData ***********************
 *
 *  Description:  Writes command and 1 data byte to a device.
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr		device address
 *	\param     cmdAddr   device command or index value
 *	\param     data      value to write
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbComWriteWordData
(
	SMB_HANDLE  *smbHdl,
	u_int32		flags,
	u_int16     addr,
	u_int8      cmdAddr,
	u_int16     data
)
{
	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_WRITE_WORD_DATA) )
		return SMB_ERR_NOT_SUPPORTED;

	return( smbComSmbXfer(smbHdl, flags, addr, SMB_WRITE,
							cmdAddr, SMB_ACC_WORD_DATA, (u_int8*)&data) );
}/*smbComWriteWordData*/

/****************************** smbComProcessCall **************************
 *
 *  Description:  Writes command and 2 data bytes to a device and immediately
 *                reads two bytes from the device.
 *                (No stop condition in between)
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr		device address
 *	\param     cmdAddr   device command or index value
 *	\param     data      pointer to data to write and read data
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbComProcessCall
(
	SMB_HANDLE *smbHdl,
	u_int32		flags,
	u_int16 addr,
	u_int8 cmdAddr,
	u_int16 *data
)
{
	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_PROC_CALL) )
		return SMB_ERR_NOT_SUPPORTED;

	return( smbComSmbXfer(smbHdl, flags, addr, SMB_READ,
						  cmdAddr, SMB_ACC_PROC_CALL, (u_int8*)data) );
}

/****************************** smbComReadBlockData *************************
 *
 *  Description:  Send a command or index byte to the device first and
 *                read then a block from the device.
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl		valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr			device address
 *	\param     cmdAddr    	byte, that will be send to the target
 *                              comand or index
 *  \param     length        pointer where length read is placed
 *	\param     data       	buffer location where data will be placed
 *
 *  \return    0 | error code
 *
 ****************************************************************************/
static int32 smbComReadBlockData
(
	SMB_HANDLE	*smbHdl,
	u_int32		flags,
	u_int16		addr,
	u_int8		cmdAddr,
	u_int8		*length,
	u_int8		*data           /* buffer of at least SMB_BLOCK_MAX_BYTES + 2 */
)
{
	u_int8 retData[SMB_BLOCK_MAX_BYTES + 2];
	int32 retVal = 0;
	u_int32 i;

	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_READ_BLOCK_DATA) )
		return SMB_ERR_NOT_SUPPORTED;


	if( (retVal = smbComSmbXfer(smbHdl, flags, addr, SMB_READ,
								 cmdAddr, SMB_ACC_BLOCK_DATA, retData)) )
	{
		*length = 0;
	} else
	{
		for( i=1; i<=retData[0]; i++)
		{
			data[i-1] = retData[i];
		}
		*length = retData[0];
	}
	return( retVal );
}/*smbComReadBlockData*/

/******************************** smbComWriteBlockData *************************
 *
 *  Description:  Writes a block to a device.
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr		device address
 *	\param     size      buffer size  2..32
 *	\param     *data     buffer location
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbComWriteBlockData
(
	SMB_HANDLE	*smbHdl,
	u_int32		flags,
	u_int16 	addr,
	u_int8		cmdAddr,
	u_int8		length,
	u_int8		*data
)
{
	u_int8 txData[SMB_BLOCK_MAX_BYTES + 2];
	u_int32 i;

	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_WRITE_BLOCK_DATA) )
		return SMB_ERR_NOT_SUPPORTED;

	if( length > SMB_BLOCK_MAX_BYTES )
	{
		length = SMB_BLOCK_MAX_BYTES;
	}
	for( i=0; i < length; i++ )
	{
		txData[i+1] = data[i];
	}
	txData[0] = length;
	return( smbComSmbXfer(smbHdl, flags, addr, SMB_WRITE,
							cmdAddr, SMB_ACC_BLOCK_DATA, txData) );
}/*smbComWriteBlockData*/

/***************************** smbComBlockProcessCall ***********************
 *
 *  Description:  Writes a command and a block of data to a device and
 *                immediately reads a block of data from the device
 *                (No stop bit in between)
 *
 *  Attention:    writeDataLen and readDataLen together may not exceed 32 Bytes!
 *
 *---------------------------------------------------------------------------
 *  \param     smbHdl	   valid SMB handle
 *	\param     flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param     addr		   device address
 *	\param     writeDataLen number of bytes to transmit
 *	\param     *writeData   Tx buffer location
 *  \param     *readDataLen will hold number of bytes received
 *	\param     *readData    Rx buffer location
 *
 *  \return    0 | error code
 *
 *  Globals....:  -
 ****************************************************************************/
static int32 smbComBlockProcessCall
(
	SMB_HANDLE *smbHdl,
	u_int32	flags,
	u_int16 addr,
	u_int8 cmdAddr,
	u_int8 writeDataLen,
	u_int8 *writeData,
	u_int8 *readDataLen,
	u_int8 *readData
)
{
	u_int8 rtxData[SMB_BLOCK_MAX_BYTES + 2];
	int32 retVal = 0;
	u_int32 i;

	if( !(smbHdl->Capability & SMB_FUNC_SMBUS_BLOCK_PROC_CALL) )
		return SMB_ERR_NOT_SUPPORTED;

	/* sanity check */
	if( writeDataLen > SMB_BLOCK_MAX_BYTES )
	{
		writeDataLen = SMB_BLOCK_MAX_BYTES;
	}
	/* write block */
	for( i=0; i < writeDataLen; i++ )
	{
		rtxData[i+1] = writeData[i];
	}
	rtxData[0] = writeDataLen;
	if( (retVal = smbComSmbXfer(smbHdl, flags, addr, SMB_READ,
								  cmdAddr, SMB_ACC_BLOCK_PROC_CALL, rtxData)) )
	{
		*readDataLen = 0;
	} else
	{
		if( rtxData[0] > SMB_BLOCK_MAX_BYTES )
		{
			*readDataLen = SMB_BLOCK_MAX_BYTES;
		}else
		{
			*readDataLen = rtxData[0];
		}

		for( i=1; i<=*readDataLen; i++)
		{
			readData[i-1] = rtxData[i];
		}
	}
	return( retVal );

}

/******************************* smbComSmbXfer *****************************/
/** Read/Write data from a device using the SMBus protocol
 *
 *  \param   smbHdl     valid SMB handle
 *	\param   flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param   addr       device address
 *	\param   read_write flag indicating read(=1) or write (=0) access
 *	\param   cmdAdr     data to be sent in command field
 *	\param   size       size of data access (Quick/Byte/Word/Block/(Blk-)Proc
 *	\param   data       pointer to variable with Tx data and for Rx data
 *
 *  \return  0 | error code
 *
 ****************************************************************************/
static int32 smbComSmbXfer
(
	SMB_HANDLE *smbHdl,
	u_int32 flags,
	u_int16 addr,
	u_int8 read_write,
	u_int8 cmdAddr,
	u_int8 size,
	u_int8 *data
)
{
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE*)((u_int8*)smbHdl + sizeof(SMB_HANDLE));
	int32 error = 0;
	int32 swpec = 0;
	u_int8 partialPec = 0;
	DBGCMD(	static const char functionName[] = "SMB2 - smbComSmbXfer"; )

	DBGWRT_1( (DBH, "%s: %s addr=0x%04x size=%d cmdAddr=0x%02x data=0x%04x\n",
			functionName, read_write ? "read" : "write",
            addr, size, cmdAddr, (data == 0)?0:*(u_int16*)data) );
	if( (flags & SMB_FLAG_PEC) &&
		!(smbHdl->Capability & SMB_FUNC_SMBUS_HWPEC_CALC) )
	{
		swpec = 1;
		if( size == SMB_ACC_BLOCK_PROC_CALL ) {
			smbComAddPec(addr, cmdAddr, size, data);
			partialPec = data[data[0] + 1];
		} else if( (read_write == SMB_WRITE) &&
				   size != SMB_ACC_PROC_CALL &&
		           size != SMB_ACC_QUICK &&
		           size != SMB_ACC_I2C_BLOCK_DATA)
		{
			size = (u_int8)smbComAddPec(addr, cmdAddr, size, data);
		}
	}

	if (smbHdl->SmbXfer) {
		if( (error = OSS_SemWait( OSSH,
								  smbComHdl->hostCtrlSem,
								  smbComHdl->busyWait)) )
		{
			DBGWRT_ERR( (DBH, "*** %s: OSS_SemWait\n", functionName) );
			error = SMB_ERR_BUSY;
			goto ERR_EXIT;
		}

		error = smbHdl->SmbXfer(smbHdl, flags, addr, read_write,
								 cmdAddr, size, data);
		if( error )
		{
			DBGWRT_ERR( (DBH, "*** %s: smbHdl %08p flags %08x addr %04x error 0x%x line %d\n",
				functionName, smbHdl, flags, addr, error, __LINE__ ));
		}

		OSS_SemSignal(OSSH, smbComHdl->hostCtrlSem);
	}
	else
	{
		error = smbComSmbXferEmulate(smbHdl, flags, addr, read_write, cmdAddr,size,data);
		if( error )
		{
			DBGWRT_ERR( (DBH, "*** %s: smbHdl %08p flags %08x addr %04x error 0x%x line %d\n",
				functionName, smbHdl, flags, addr, error, __LINE__ ));
		}
	}

	if( !error &&
		swpec &&
		size != SMB_ACC_QUICK &&
		size != SMB_ACC_I2C_BLOCK_DATA &&
		( read_write == SMB_READ ||
		  ((flags & SMB_FLAG_PEC) && (size == SMB_ACC_PROC_CALL  ||
		  							  size == SMB_ACC_BLOCK_PROC_CALL)) ) )
	{
		if(smbComCheckPec(addr, cmdAddr, size, partialPec, data))
		{
			DBGWRT_ERR( (DBH, "*** %s: smbHdl %08p flags %08x addr %04x line %d\n",
				functionName, smbHdl, flags, addr, __LINE__ ));
			error = SMB_ERR_PEC;
			goto ERR_EXIT;
		}
	}

	/* tbd: add support for SW PEC */
ERR_EXIT:
	return( error );
}

/******************************* smbComI2cXfer *****************************/
/** Read/Write data from a device using the I2C protocol
 *
 *  \param   smbHdl     valid SMB handle
 *	\param   msg      array of I2C messages (packets) to transfer
 *	\param   num       number of messages in msg to transfer
 *
 *  \return  0 | error code
 *
 ****************************************************************************/
static int32 smbComI2cXfer
(
	SMB_HANDLE *smbHdl,
	struct I2CMESSAGE msg[],
	u_int32 num)
{
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE*)((u_int8*)smbHdl + sizeof(SMB_HANDLE));
	int32 error = 0;
	DBGCMD(	static const char functionName[] = "SMB2 - smbComI2cXfer"; )


	if (smbHdl->I2CXfer)
	{
		if( OSS_SemWait(OSSH, smbComHdl->hostCtrlSem, smbComHdl->busyWait) != 0)
		{
			DBGWRT_ERR( (DBH, "*** %s: OSS_SemWait\n", functionName) );
			error = SMB_ERR_BUSY;
			goto ERR_EXIT;
		}

		error = smbHdl->I2CXfer(smbHdl, msg, num);
		if( error )
		{
			DBGWRT_ERR((DBH, "*** %s: smbHdl %08p\n",	functionName, smbHdl ));
		}
		OSS_SemSignal(OSSH, smbComHdl->hostCtrlSem);
	}
	else
	{
		DBGWRT_ERR( (DBH, "*** %s: No I2CXfer function available!!\n",	functionName) );
		error = SMB_ERR_NOT_SUPPORTED;
		goto ERR_EXIT;
	}

ERR_EXIT:
	return( error );
}

/**************************** smbComSmbXferEmulate **************************/
/** Read/Write data from a device emulating the SMBus protocol on an I2C bus
 *
 *  \param   smbHdl     valid SMB handle
 *	\param   flags      flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param   addr       device address
 *	\param   read_write flag indicating read(=1) or write (=0) access
 *	\param   cmdAdr     data to be sent in command field
 *	\param   size       size of data access (Quick/Byte/Word/Block/(Blk-)Proc
 *	\param   data       pointer to variable with Tx data and for Rx data
 *
 *  \return  0 | error code
 *
 ****************************************************************************/
static int32 smbComSmbXferEmulate
(
	SMB_HANDLE *smbHdl,
	u_int32 flags,
	u_int16 addr,
	u_int8 read_write,
	u_int8 cmdAddr,
	u_int8 size,
	u_int8 *data
)
{
	int32 error = SMB_ERR_NO;
	DBGCMD(	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE*)((u_int8*)smbHdl +
														  sizeof(SMB_HANDLE)); )
	DBGCMD(	static const char functionName[] = "SMB2 - smbComSmbXferEmulate"; )
	/* Generate a series of messages. One for writing, two for reading
	  (write addr, read data) */
	unsigned char msgbuf0[34];
	unsigned char msgbuf1[34];
	int num = (read_write == SMB_READ) ? 2 : 1;
    struct I2CMESSAGE msg[2];

	int i;

    msg[0].addr  = addr;
    msg[0].flags = flags;
    msg[0].len   = 1;
    msg[0].buf   = msgbuf0;

    msg[1].addr  = addr;
    msg[1].flags = flags | I2C_M_RD;
    msg[1].len   = 0;
    msg[1].buf   = msgbuf1;

	msgbuf0[0] = cmdAddr;
	switch(size) {
	case SMB_ACC_QUICK:
		msg[0].len = 0;
		/* Special case: The read/write field is used as data */
		msg[0].flags = flags | ((read_write == SMB_READ) ? I2C_M_RD : 0);
		num = 1;
		break;
	case SMB_ACC_BYTE:
		if (read_write == SMB_READ) {
			/* Special case: only a read! */
			msg[0].flags = I2C_M_RD | flags;
			num = 1;
		}
		/* write is covered anyway */
		break;
	case SMB_ACC_BYTE_DATA:
		if (read_write == SMB_READ)
			msg[1].len = 1;
		else {
			msg[0].len = 2;
			msgbuf0[1] = *data;
		}
		break;
	case SMB_ACC_WORD_DATA:
		if (read_write == SMB_READ)
			msg[1].len = 2;
		else {
			msg[0].len=3;
			msgbuf0[1] = data[1];
			msgbuf0[2] = data[0];
		}
		break;
	case SMB_ACC_PROC_CALL:
		num = 2; /* Special case */
		read_write = SMB_READ;
		msg[0].len = 3;
		msg[1].len = 2;
		msgbuf0[1] = data[1];
		msgbuf0[2] = data[0];
		break;
	case SMB_ACC_BLOCK_DATA:
		if (read_write == SMB_READ) {
			/* reads with unknown length not supported by I2C bus */
			DBGWRT_ERR( (DBH, "*** %s: Block read not supported "
				"under I2C emulation!!!\n", functionName) );
			return SMB_ERR_NOT_SUPPORTED;
		} else {
			msg[0].len = data[0] + 2;
			if (msg[0].len > SMB_BLOCK_MAX_BYTES + 2) {
				DBGWRT_ERR( (DBH, "*** %s: Invalid block write size (%d)\n",
					functionName, data[0]) );
				return SMB_ERR_PARAM;
			}
			if( flags & SMB_FLAG_PEC )
				(msg[0].len)++;
			for (i = 1; i <= msg[0].len; i++)
				msgbuf0[i] = data[i-1];
		}
		break;
	case SMB_ACC_BLOCK_PROC_CALL:
		DBGWRT_ERR( (DBH, "*** %s: Block process call not supported "
			"under I2C emulation!\n", functionName) );
		return SMB_ERR_NOT_SUPPORTED;
	case SMB_ACC_I2C_BLOCK_DATA:
		if (read_write == SMB_READ) {
			msg[1].len = SMB_BLOCK_MAX_BYTES;
		} else {
			msg[0].len = data[0] + 1;
			if (msg[0].len > SMB_BLOCK_MAX_BYTES + 1) {
				DBGWRT_ERR( (DBH, "*** %s: Called with "
					"invalid block write size (%d)\n", functionName, data[0]) );
				return SMB_ERR_PARAM;
			}
			for (i = 1; i <= data[0]; i++)
				msgbuf0[i] = data[i];
		}
		break;
	default:
		DBGWRT_ERR( (DBH, "*** %s: Called with invalid size (%d)\n",
			functionName, size) );
		return SMB_ERR_PARAM;
	}

	if( (error = smbComI2cXfer(smbHdl, msg, num)) != SMB_ERR_NO)
	{
		DBGWRT_ERR( (DBH, "*** %s: smbComI2cXfer\n",	functionName) );
		return error;
	}

	if (read_write == SMB_READ)
		switch(size) {
			case SMB_ACC_BYTE:
				*data = msgbuf0[0];
				break;
			case SMB_ACC_BYTE_DATA:
				*data = msgbuf1[0];
				break;
			case SMB_ACC_WORD_DATA:
			case SMB_ACC_PROC_CALL:
				*(u_int16*)data = (u_int16)(msgbuf1[0] | (msgbuf1[1] << 8));
				break;
			case SMB_ACC_I2C_BLOCK_DATA:
				/* currently fixed to 32 */
				data[0] = SMB_BLOCK_MAX_BYTES;
				for (i = 0; i < SMB_BLOCK_MAX_BYTES; i++)
					data[i+1] = msgbuf1[i];
				break;
		}
	return 0;
}

#ifndef SMB_NO_ALERT
/********************************* smbComAlertPoll **************************/
/**  Issue a read byte command to the Alert Response Address
 *   If an alarm is returend and a callback function is installed for the
 *   device that returned the alarm, the CB function is called
 *
 *   \param   smbHdl   \IN  valid SMB handle
 *
 *   \return  void
 *
 ****************************************************************************/
static void smbComAlertPoll
(
	void *smbHdl
)
{
	u_int16 alertCnt;
	smbComAlertResponse( (SMB_HANDLE*)smbHdl, 0, 0, &alertCnt );
}

/****************************** smbComAlertResponse ***********************/
/**  Issue a read byte command to the Alert Response Address
 *
 *   If an alarm is returned by a device and the device address matches
 *   the address passed, the alertCnt parameter returns with an 1 set.
 *
 *   In addition the function will call the callback function installed (if any)
 *   for any device address that returns an alert.
 *
 *   \param   smbHdl   \IN  valid SMB handle
 *   \param   flags    \IN  flags for device access
 *	 \param   addr     \IN  0x00 or device address to be compared with
 *   \param   alertCnt \OUT pointer where number of received alerts is stored
 *
 *   \return  0 | error code
 *
 ****************************************************************************/
static int32 smbComAlertResponse
(
	SMB_HANDLE  *smbHdl,
	u_int32 	flags,
	u_int16		addr,
	u_int16 	*alertCnt

)
{
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE*)((u_int8*)smbHdl + sizeof(SMB_HANDLE));
	int32 error = 0;
	SMB_ALERT_NODE *curNode;
	u_int16 currSrcAddr = 0;

	DBGCMD(	static const char functionName[] = "SMB2 - Z001_SmbAlertResponse"; )

	*alertCnt = 0;

	/* issue alert response to get new alert */
	if( (error = smbComReadByte(smbHdl,
								0,
								SMB_ADDR_ALERT_RESPONSE,
								(u_int8*)&currSrcAddr)) &&
		(error != SMB_ERR_ADDR) )
	{
		DBGWRT_ERR( (DBH, "*** %s: SMB/I2C Transfer failed!!\n",
					functionName) );
	}

	if( error == SMB_ERR_ADDR )
	{
		/* no SMB device acknoledged */
		error = 0;
		*alertCnt = 0;
		goto ERR_EXIT;
	}

	/* see if CB function installed for current alert address */
	for(curNode = (SMB_ALERT_NODE*)smbComHdl->smbAlertCb.head;
		curNode->node.next;
		curNode = (SMB_ALERT_NODE*)curNode->node.next)
	{
		if( curNode->addr == (u_int8)addr )
		{
			/* call CB function */
			curNode->alertCb(curNode->alertCbArg);
		}
	}

	if( (addr != 0) && (addr == currSrcAddr) ) /* was an alert? */
	{
		*alertCnt = 1;
	}

ERR_EXIT:
	return error;
}


/****************************** smbComAlertCbInstall ************************/
/**  Install callback function to be invoked on alert from a specific address
 *
 *   \param   smbHdl   \IN  valid SMB handle
 *	 \param   addr     \IN  SMBus address callback fct. is to be installed for
 *   \param   cbFuncP  \IN  pointer to callback function
 *   \param   cbArg    \IN  argument for callback function
 *
 *   \return  0 | error code
 *
 ****************************************************************************/
static int32 smbComAlertCbInstall
(
	SMB_HANDLE *smbHdl,
	u_int16 addr,
	void (*cbFuncP)( void *cbArg ),
	void *cbArg
)
{
	u_int32 error = 0;
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE *)((u_int8*)smbHdl + sizeof(SMB_ENTRIES));
	DBGCMD(	static const char functionName[] = "SMB2 - smbComAlertCbInstall"; )

	SMB_ALERT_NODE *curNode;
	u_int32 gotsize = 0;

	/* search list for already installed CB func on this address */
	for(curNode = (SMB_ALERT_NODE*)smbComHdl->smbAlertCb.head;
		curNode->node.next;
		curNode = (SMB_ALERT_NODE*)curNode->node.next)
	{
		if( curNode->addr == (u_int8)addr )
		{
			/* list entry available, cb already installed, exit */
			DBGWRT_ERR( (DBH, "*** %s: CB function for this address "
							  "already installed (addr = 0x%04x)!!\n",
				 		functionName, addr) );
			error = SMB_ERR_PARAM;
			goto ERR_EXIT;
		}
	}

	/* attach cb for desired address */
	if( !(curNode = (SMB_ALERT_NODE*)OSS_MemGet(OSSH,
											 sizeof(SMB_ALERT_NODE),
											 &gotsize)) )
	{
		DBGWRT_ERR( (DBH, "*** %s: Can't allocate SMB Alert node!!\n",
			 		functionName) );
		error = SMB_ERR_NO_MEM;
		goto ERR_EXIT;
	}
	curNode->addr 		= addr;
	curNode->alertCb  	= cbFuncP;
	curNode->alertCbArg = cbArg;
	curNode->gotsize 	= gotsize;

	OSS_DL_AddTail( &smbComHdl->smbAlertCb, &curNode->node );

ERR_EXIT:
	return error;
} /* AlertCbInstall */

/****************************** cmbComAlertCbRemove ************************/
/**  Remove callback function for a specific address
 *
 *   \param   smbHdl   \IN  valid SMB handle
 *	 \param   addr     \IN  SMBus address callback fct. is to be removed for
 *   \param   cbArgP   \OUT argument for callback function
 *
 *   \return  0 | error code
 *
 ****************************************************************************/
static int32 cmbComAlertCbRemove
(
	SMB_HANDLE *smbHdl,
	u_int16 addr,
	void **cbArgP
)
{
	SMB_COM_HANDLE *smbComHdl = (SMB_COM_HANDLE *)((u_int8*)smbHdl + sizeof(SMB_ENTRIES));
	SMB_ALERT_NODE *curNode;
	u_int32 error = 0;

	DBGCMD(	static const char functionName[] = "SMB2 - cmbComAlertCbRemove:"; )

	/* search list for installed CB func on this address */
	for(curNode = (SMB_ALERT_NODE*)smbComHdl->smbAlertCb.head;
		curNode->node.next;
		curNode = (SMB_ALERT_NODE*)curNode->node.next)
	{
		if( curNode->addr == (u_int8)addr )
		{
			/* list entry available, cb already installed, exit */
			OSS_DL_Remove( (OSS_DL_NODE*)curNode );
			*cbArgP = curNode->alertCbArg;
			OSS_MemFree( OSSH, (u_int8*)curNode, curNode->gotsize );
			goto ERR_EXIT;
		}
	}
	if( !curNode->node.next )
	{
		/* searched to the end without finding installed cb */
		DBGWRT_ERR( (DBH, "*** %s: no installed CB function for this address",
			 		functionName) );
		error = SMB_ERR_PARAM;

	}
ERR_EXIT:

	return error;
} /* cmbComAlertCbRemove */

#endif /* SMB_NO_ALERT */

static int32 smbComAddPec(
	u_int16 addr,
	u_int8 cmdAddr,
	u_int8 size,
	u_int8 *data
)
{
	/* tbd all */
	return 0;
}

static int32 smbComCheckPec(
	u_int16 addr,
	u_int8 cmdAddr,
	u_int8 size,
	u_int32 partial,
	u_int8 *data
)
{
	/* tbd all */
	return 0;
}



/** \page dummy
  \menimages
*/









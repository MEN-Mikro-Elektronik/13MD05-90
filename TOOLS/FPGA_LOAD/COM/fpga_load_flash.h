/***********************  I n c l u d e  -  F i l e  ***********************/
/*!
 *        \file  fpga_load_flash.h
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2008/03/19 16:10:59 $
 *    $Revision: 2.5 $
 *
 *       \brief  Header file for FPGA_LOAD tool
 *               containing Flash specific functions
 *
 *    \switches
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: fpga_load_flash.h,v $
 * Revision 2.5  2008/03/19 16:10:59  CKauntz
 * added: STM25P32_Trys
 *
 * Revision 2.4  2007/07/09 20:10:53  CKauntz
 * added: Try-functions for the swapped versions
 *
 * Revision 2.3  2005/12/12 11:05:03  cschuster
 * Copyright string changed
 *
 * Revision 2.2  2005/11/24 18:21:42  cschuster
 * added defines for am29lvxxx_smb
 *
 * Revision 2.1  2004/12/23 15:10:42  cs
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _FPGA_LOAD_FLASH_H
#define _FPGA_LOAD_FLASH_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* Flash specific functions */
typedef int32 (*FLASH_READ_BLOCKP)(FLASH_DEVS *, u_int32, u_int32, u_int8 *);
typedef int32 (*FLASH_INITP)(DEV_HDL *h);
typedef int32 (*FLASH_TRYP)(DEV_HDL *, FLASH_INITP *, u_int32);

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/

extern int32 Z100_AM29LVXXX_TRY(DEV_HDL *devHdl,
									FLASH_INITP *flash_initP,
									u_int32 dbgLevel);
extern int32 Z100_AM29LVXXX_TRY_sw(DEV_HDL *devHdl,
									   FLASH_INITP *flash_initP,
									   u_int32 dbgLevel);
extern int32 Z100_AM29LVXXX_TRY_mmod(DEV_HDL *devHdl,
									   FLASH_INITP *flash_initP,
									   u_int32 dbgLevel);
extern int32 Z100_AM29LVXXX_TRY_mmod_sw(DEV_HDL *devHdl,
										  FLASH_INITP *flash_initP,
										  u_int32 dbgLevel);

extern int32 Z100_AM29LVXXX_SMB_TRY(DEV_HDL *devHdl,
									FLASH_INITP *flash_initP,
									u_int32 dbgLevel);
extern int32 Z100_AM29LVXXX_SMB_TRY_sw(DEV_HDL *devHdl,
									   FLASH_INITP *flash_initP,
									   u_int32 dbgLevel);

extern int32 Z100_FLASH_READ_BLOCK( FLASH_DEVS *fDev,
									u_int32 offs,
									u_int32 len,
									u_int8  *buf);

extern int32 Z100_ISTRATAPC28FXXXP30_TRY (DEV_HDL *devHdl,
										 	 FLASH_INITP *flash_initP,
										 	 u_int32 dbgLevel);
extern int32 Z100_ISTRATAPC28FXXXP30_TRY_sw(DEV_HDL *devHdl,
										 		FLASH_INITP *flash_initP,
										 		u_int32 dbgLevel);
extern int32 Z100_ISTRATAPC28FXXXP30_TRY_mmod(DEV_HDL *devHdl,
										 	 	FLASH_INITP *flash_initP,
										 	 	u_int32 dbgLevel);
extern int32 Z100_ISTRATAPC28FXXXP30_TRY_mmod_sw(DEV_HDL *devHdl,
										 		   FLASH_INITP *flash_initP,
										 		   u_int32 dbgLevel);

extern int32 Z100_STM25P32_TRY(DEV_HDL *devHdl,
					 		   FLASH_INITP *flash_initP,
					 		   u_int32 dbgLevel);
extern int32 Z100_STM25P32_TRY_sw(DEV_HDL *devHdl,
					 		      FLASH_INITP *flash_initP,
					 		      u_int32 dbgLevel);

#ifdef __cplusplus
      }
#endif

#endif /* _FPGA_LOAD_FLASH_H */



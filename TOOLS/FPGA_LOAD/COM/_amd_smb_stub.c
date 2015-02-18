/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  _amd_smb_stub.c
 *
 *       \author  Christian.Kauntz@men.de
 *        $Date: 2009/03/05 09:52:50 $
 *    $Revision: 2.2 $
 *
 *        \brief  instance for AMD Flash with SMB normal version
 *
 *
 *     Required: -
 *    \switches  (none)
 */
 /*---------------------------[ Public Functions ]----------------------------
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: _amd_smb_stub.c,v $
 * Revision 2.2  2009/03/05 09:52:50  CKauntz
 * R: Compiler warning internFunc declared but not defined
 * M: Added undef to reduce compiler warning
 *
 * Revision 2.1  2007/07/09 20:12:57  CKauntz
 * Initial Revision
 *
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2007 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#if defined MAC_BYTESWAP
#undef MAC_BYTESWAP
#endif /* MAC_BYTESWAP */

#undef PROG_FILE_NAME
#define Z100_AM29LVXXX_SMB_TRY Z100_AM29LVXXX_SMB_TRY
#include "am29lvxxx_smb.c"


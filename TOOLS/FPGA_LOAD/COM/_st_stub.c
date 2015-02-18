/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *         \file  _st_stub.c
 *
 *       \author  Christian.Kauntz@men.de
 *        $Date: 2009/03/05 09:53:07 $
 *    $Revision: 2.3 $
 *
 *        \brief  instance for STMicroelektronics Flash
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
 * $Log: _st_stub.c,v $
 * Revision 2.3  2009/03/05 09:53:07  CKauntz
 * R: Compiler warning internFunc declared but not defined
 * M: Added undef to reduce compiler warning
 *
 * Revision 2.2  2009/01/22 17:53:44  CKauntz
 * Added fpga_load.h to solve compiler warnings
 *
 * Revision 2.1  2008/03/19 16:11:09  CKauntz
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#ifdef FL_SWAP_SW
#undef FL_SWAP_SW
#endif /* FL_SWAP_SW */
#ifdef FL_SWAP_MMOD_SW
#undef FL_SWAP_MMOD_SW
#endif /* FL_SWAP_MMOD_SW */
#ifdef FL_SWAP_MMOD
#undef FL_SWAP_MMOD
#endif /* FL_SWAP_MMOD */


#define Z100_STM25P32_TRY Z100_STM25P32_TRY

#undef PROG_FILE_NAME
#include "fpga_load.h"
#include "hw_acc.c"
#include "stm25p32.c"


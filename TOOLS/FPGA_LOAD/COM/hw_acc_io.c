/************************  F P G A _ L O A D  *********************************/
/*!
 *
 *        \file  hw_acc_io.c
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2014/01/20 10:17:15 $
 *    $Revision: 2.10 $
 *
 *       \brief  HW access functions for I/O mapped HW
 *
 *
 *     Required:
 *
 *
 */
/*---------------------------[ Public Functions ]-------------------------------
 * none
 *
 *---------------------------------[ History ]----------------------------------
 *
 * $Log: hw_acc_io.c,v $
 * Revision 2.10  2014/01/20 10:17:15  awerner
 * R: <sys/io.h> not available in Windows
 * M: Added ifdef to not include the header in Windows
 *
 * Revision 2.9  2014/01/17 17:27:22  awerner
 * R: 1. Merge error 2.7->2.8
 *    2. compile failed on PPC platform
 * M: 1. Manuell merge 2.7->2.8
 *    2. on PPC platforms io.h doesnt exist, include conditional
 *
 * Revision 2.8  2013/06/21 16:46:04  MRoth
 * R: no support for Windows7 64bit
 * M: 1a) added WIN64 specific men_genacc WRITE/Read macros
 *    1b) changed pcigenacc interface to men_genacc interface
 *
 * Revision 2.6  2009/03/05 09:52:45  CKauntz
 * R: Compiler warning internFunc declared but not defined
 * M: Added undef to reduce compiler warning
 *
 * Revision 2.5  2009/01/22 17:55:59  CKauntz
 * Changed compile order of header files to solve compiler warnings
 *
 * Revision 2.4  2007/08/07 14:10:48  JWu
 * changed:
 *   type of arguments in Z100_Mwrite_Io_D8*, Z100_Mwrite_Io_D16*
 *
 * Revision 2.3  2007/07/09 20:10:51  CKauntz
 * added: swapped versions of the Read / Write functions
 *
 * Revision 2.2  2006/03/24 15:00:54  cschuster
 * access IO-mapped addresses with VXWORKS + PPC just like memory mapped
 *
 * Revision 2.1  2005/12/12 11:05:24  cschuster
 * Initial Revision
 *
 *
 *
 *------------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/

#ifdef Z100_IO_ACCESS_ENABLE

#if !defined PPC && !defined WINNT
# include <sys/io.h>
#endif

#ifndef VXWORKS
# ifdef MAC_MEM_MAPPED
#	undef MAC_MEM_MAPPED
# endif
# ifndef MAC_IO_MAPPED
#	define MAC_IO_MAPPED
# endif
#else
# if CPU_FAMILY==PPC
#   ifndef MAC_MEM_MAPPED
#	  define MAC_MEM_MAPPED
#   endif
#   ifdef MAC_IO_MAPPED
#	  undef MAC_IO_MAPPED
#   endif
# endif
#endif

#undef PROG_FILE_NAME
#include "fpga_load.h"
#include <MEN/maccess.h>
#if defined(WINNT) && defined(_WIN64)
 #include <MEN/genacc.h>
#endif

extern u_int8 Z100_Mread_Io_D8(
	void *ma,
	u_int32 offs
)
{
#if defined(WINNT) && defined(_WIN64)
	return GENACC_IO_READ_D8((*G_pHga), (ma), (offs));
#else
	return MREAD_D8((MACCESS)ma, offs );
#endif
}


extern u_int16 Z100_Mread_Io_D16(
	void *ma,
	u_int32 offs
)
{
#if defined(WINNT) && defined(_WIN64)
	return GENACC_IO_READ_D16((*G_pHga), (ma), (offs));
#else
	return MREAD_D16((MACCESS)ma, offs );
#endif
}


extern u_int32 Z100_Mread_Io_D32(
	void *ma,
	u_int32 offs
)
{
#if defined(WINNT) && defined(_WIN64)
	return GENACC_IO_READ_D32((*G_pHga), (ma), (offs));
#else
	return MREAD_D32((MACCESS)ma, offs );
#endif
}


extern void Z100_Mwrite_Io_D8(
	void *ma,
	u_int32 offs,
	u_int8 val
)
{
#if defined(WINNT) && defined(_WIN64)
	GENACC_IO_WRITE_D8((*G_pHga), (ma), (offs), (val));
#else
	MWRITE_D8((MACCESS)ma, offs, val );
#endif
	return;
}


extern void Z100_Mwrite_Io_D16(
	void *ma,
	u_int32 offs,
	u_int16 val
)
{
#if defined(WINNT) && defined(_WIN64)
	GENACC_IO_WRITE_D16((*G_pHga), (ma), (offs), (val));
#else
	MWRITE_D16((MACCESS)ma, offs, val );
#endif
	return;
}


extern void Z100_Mwrite_Io_D32(
	void *ma,
	u_int32 offs,
	u_int32 val
)
{
#if defined(WINNT) && defined(_WIN64)
	GENACC_IO_WRITE_D32((*G_pHga), (ma), (offs), (val));
#else
	MWRITE_D32((MACCESS)ma, offs, val );
#endif
	return;
}


/* Swapped versions of the IO functions */


extern u_int8 Z100_Mread_Io_D8_SW(
	void *ma,
	u_int32 offs
)
{
#if defined(WINNT) && defined(_WIN64)
	return GENACC_IO_READ_D8((*G_pHga), (ma), (offs));
#else
	return (u_int8)MREAD_D8( (MACCESS)ma, offs );
#endif
}


extern u_int16 Z100_Mread_Io_D16_SW(
	void *ma,
	u_int32 offs
)
{
	u_int16 temp;
#if defined(WINNT) && defined(_WIN64)
	temp = GENACC_IO_READ_D16((*G_pHga), (ma), (offs));
#else
	temp = MREAD_D16((MACCESS)ma, offs );
#endif
	temp = OSS_SWAP16(temp);
	return (temp);
}


extern u_int32 Z100_Mread_Io_D32_SW(
	void *ma,
	u_int32 offs
)
{
	u_int32 temp;
#if defined(WINNT) && defined(_WIN64)
	temp = GENACC_IO_READ_D32((*G_pHga), (ma), (offs));
#else
	temp = MREAD_D32((MACCESS)ma, offs );
#endif
	temp = OSS_SWAP32(temp);
	return (temp);
}


extern void Z100_Mwrite_Io_D8_SW(
	void *ma,
	u_int32 offs,
	u_int8 val
)
{
#if defined(WINNT) && defined(_WIN64)
	GENACC_IO_WRITE_D8((*G_pHga), (ma), (offs), (val));
#else
	MWRITE_D8((MACCESS)ma, offs, val );
#endif
	return;
}


extern void Z100_Mwrite_Io_D16_SW(
	void *ma,
	u_int32 offs,
	u_int16 val
)
{
	u_int16 temp;

	temp = OSS_SWAP16(val);
#if defined(WINNT) && defined(_WIN64)
	GENACC_IO_WRITE_D16((*G_pHga), (ma), (offs), (val));
#else
	MWRITE_D16((MACCESS)ma, offs, temp );
#endif
	return;
}


extern void Z100_Mwrite_Io_D32_SW(
	void *ma,
	u_int32 offs,
	u_int32 val
)
{
	u_int32 temp;
	temp = OSS_SWAP32(val);
#if defined(WINNT) && defined(_WIN64)
	GENACC_IO_WRITE_D32((*G_pHga), (ma), (offs), (val));
#else
	MWRITE_D32((MACCESS)ma, offs, temp );
#endif
	return;
}


#endif /* Z100_IO_ACCESS_ENABLE */



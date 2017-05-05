/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  dbg_os.h
 *
 *      \author  kp
 *        $Date: 2006/09/27 18:38:03 $
 *    $Revision: 1.5 $
 *
 *	   \project  MDIS4Linux
 *       \brief  Linux macros to display driver debug messages
 *
 *    \switches  MAC_USERSPACE
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: dbg_os.h,v $
 * Revision 1.5  2006/09/27 18:38:03  ts
 * Support for MDIS Xenomai added
 *
 * Revision 1.4  2005/07/08 11:40:38  cs
 * added support for userspace
 * changed header to doxygen
 * Copyright line changed
 *
 * Revision 1.3  2003/04/11 16:08:05  kp
 * changed CONFIG_RTHAL switch against MDIS_RTAI_SUPPORT
 *
 * Revision 1.2  2003/02/21 12:18:48  kp
 * use rt_printk if CONFIG_RTHAL defined
 *
 * Revision 1.1  2001/01/19 14:38:30  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
#ifndef _DBG_OS_H
#define _DBG_OS_H

#ifdef __cplusplus
   extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/kernel.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define DBG_Init	men_DBG_Init
#define DBG_Exit	men_DBG_Exit
#define DBG_Memdump men_DBG_Memdump

#define __LDL	KERN_DEBUG  /* define printk level used for debug messages */

#define DBG_WRITE_DEFINED_BY_DBG_OS_H

#define DBG_Write(dbh,fmt,args...) printk( __LDL fmt, ## args )



#else /* __KERNEL__ */
# ifdef MAC_USERSPACE
#	undef DBG_WRITE_DEFINED_BY_DBG_OS_H
#	define DBG_Init	men_usr_DBG_Init
#	define DBG_Exit	men_usr_DBG_Exit
#	define DBG_Write	men_usr_DBG_Write
#	define DBG_Memdump men_usr_DBG_Memdump
# endif /* MAC_USERSPACE */
#endif /* __KERNEL__ */
/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifdef __cplusplus
   }
#endif
#endif /*_DBG_OS_H*/


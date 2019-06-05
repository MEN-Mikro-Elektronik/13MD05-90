/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  dbg_os.h
 *
 *      \author  kp
 *
 *	   \project  MDISforLinux
 *       \brief  Linux macros to display driver debug messages
 *
 *    \switches  MAC_USERSPACE
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


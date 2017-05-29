/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bbis_bk.h
 *
 *       Author: kp
 *        $Date: 2004/06/09 09:07:39 $
 *    $Revision: 1.2 $
 *
 *  Description: BBIS module definitions
 *
 *     Switches: __KERNEL__
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bbis_bk.h,v $
 * Revision 1.2  2004/06/09 09:07:39  kp
 * bbis_register_bb_driver proto changed
 *
 * Revision 1.1  2001/01/19 14:37:56  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#ifndef _BBIS_BK_H
#define _BBIS_BK_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* none */

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
#define BK_MAX_DEVNAME	39		/* maximum length of device name */

/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
#ifdef __KERNEL__
extern int bbis_register_bb_driver( char *bbName,
									void (*getEntry)(BBIS_ENTRY *),
									struct module *module);
extern int bbis_unregister_bb_driver( char *bbName );

extern int bbis_open(
	char *devName,
	DESC_SPEC *bbDesc,
	BBIS_HANDLE **bbHdlP,
	BBIS_ENTRY *bbEntries);

extern int bbis_close(char *devName);

extern char *bbis_ident(void);

#endif

#ifdef __cplusplus
   }
#endif

#endif  /* _BBIS_BK_H  */


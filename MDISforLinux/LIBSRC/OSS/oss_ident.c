/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_ident.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2012/08/07 19:01:19 $
 *    $Revision: 2.21 $
 *
 *	   \project  MDISforLinux
 *  	 \brief  OSS library version info
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_ident.c,v $
 * Revision 2.21  2012/08/07 19:01:19  ts
 * R: new Version of Fileset
 * M: updated Version string to 2.15
 *
 * Revision 2.20  2012/05/14 21:30:21  ts
 * (AUTOCI) Checkin due to new revision 2.14 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.19  2011/05/31 16:29:16  cs
 * (AUTOCI) Checkin due to new revision 2.13 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.18  2011/05/25 15:55:49  CRuff
 * (AUTOCI) Checkin due to new revision 2.12 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.17  2010/08/26 17:13:52  CRuff
 * (AUTOCI) Checkin due to new revision 2.11 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.16  2009/09/25 13:16:57  CRuff
 * (AUTOCI) Checkin due to new revision 2.10 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.15  2009/09/08 16:46:03  CRuff
 * (AUTOCI) Checkin due to new revision 2.9 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.14  2009/02/18 13:40:47  GLeonhardt
 * (AUTOCI) Checkin due to new revision 2.8 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.13  2007/12/10 10:38:42  ts
 * (AUTOCI) Checkin due to new revision 2.7 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.12  2007/03/28 16:23:07  ts
 * (AUTOCI) Checkin due to new revision 2.6 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.11  2006/12/15 14:00:53  ts
 * (AUTOCI) Checkin due to new revision 2.5 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.10  2005/08/11 16:53:35  ts
 * (AUTOCI) Checkin due to new revision 2.4 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.9  2005/07/08 16:51:37  cs
 * (AUTOCI) Checkin due to new revision 1.1 of fileset LINUX/LIBSRC/oss_usr
 *
 * Revision 2.8  2005/07/08 11:34:48  cs
 * (AUTOCI) Checkin due to new revision 1.0 of fileset LINUX/LIBSRC/oss_usr
 *
 * Revision 2.7  2005/07/07 17:17:14  cs
 * Copyright line changed
 *
 * Revision 2.6  2005/01/31 15:20:29  ts
 * (AUTOCI) Checkin due to new revision 2.3 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.5  2004/10/27 15:44:25  kp
 * (AUTOCI) Checkin due to new revision 2.2 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.4  2004/07/27 09:03:34  kp
 * (AUTOCI) Checkin due to new revision 2.1 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.3  2004/06/09 09:27:02  kp
 * (AUTOCI) Checkin due to new revision 2.0 of fileset LINUX/LIBSRC/oss
 *
 * Revision 2.2  2003/11/21 12:50:57  kp
 * Fixed bug: Must include "oss_intern.h" to rename OSS_Ident to men_OSS_Ident
 *
 * Revision 2.1  2003/10/07 11:49:22  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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

#include "oss_intern.h"

static const char *IdentString="OSS - MEN Linux Operating System Services: %FSREV LINUX/LIBSRC/oss 2.15 2012-08-07%";

char* OSS_Ident( void )
{
    return( (char*)IdentString );
}



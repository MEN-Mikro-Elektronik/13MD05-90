/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ident.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2012/08/07 19:11:13 $
 *    $Revision: 1.20 $
 *
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  USR_OSS global edition history
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ident.c,v $
 * Revision 1.20  2012/08/07 19:11:13  ts
 * (AUTOCI) Checkin due to new revision 2.14 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.19  2012/05/23 17:56:26  ts
 * (AUTOCI) Checkin due to new revision 2.13 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.18  2012/05/14 21:34:32  ts
 * R: new Revision of MDIS System package
 *
 * Revision 1.17  2011/05/25 16:00:37  CRuff
 * (AUTOCI) Checkin due to new revision 2.11 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.16  2010/08/26 17:20:17  CRuff
 * (AUTOCI) Checkin due to new revision 2.10 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.15  2009/09/25 19:25:32  CRuff
 * (AUTOCI) Checkin due to new revision 2.9 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.14  2009/09/25 13:31:46  CRuff
 * (AUTOCI) Checkin due to new revision 2.8 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.13  2009/09/08 18:16:54  CRuff
 * (AUTOCI) Checkin due to new revision 2.7 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.12  2009/02/18 13:54:32  GLeonhardt
 * (AUTOCI) Checkin due to new revision 2.6 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.11  2007/12/10 10:44:26  ts
 * (AUTOCI) Checkin due to new revision 2.5 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.10  2007/05/11 15:21:32  ts
 * new Revision 2.4
 *
 * Revision 1.9  2007/03/28 16:32:08  ts
 * (AUTOCI) Checkin due to new revision 2.3 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.8  2005/01/31 15:24:25  ts
 * (AUTOCI) Checkin due to new revision 2.2 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.7  2004/10/27 15:46:09  kp
 * (AUTOCI) Checkin due to new revision 2.1 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.6  2004/06/09 10:28:11  kp
 * (AUTOCI) Checkin due to new revision 2.0 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.5  2003/10/07 12:30:19  kp
 * (AUTOCI) Checkin due to new revision 1.7 of fileset LINUX/LIBSRC/usr_oss
 *
 * Revision 1.4  2003/06/06 09:29:45  kp
 * Changed headers for doxygen
 *
 * Revision 1.3  2002/05/31 15:12:12  kp
 * Ident string changed
 *
 * Revision 1.2  2001/09/18 15:21:14  kp
 * Ident string changed
 *
 * Revision 1.1  2001/01/19 14:39:42  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
static const char RCSid[]="$Header: /dd2/CVSR/LINUX/LIBSRC/USR_OSS/ident.c,v 1.20 2012/08/07 19:11:13 ts Exp $";

#include "uos_int.h"

/*! \mainpage

 This is the documentation of the MEN USR_OSS library (User state
 Operating System Services) for Linux in user land.

 Refer to the \ref usrosscommonspec "USR_OSS Common Specification" for the
 plain common specification.

 For Linux, the USR_OSS module is compiled as a static or shared library
 \c libusr_oss.a or \c libusr_oss.so, that is linked to application.

 The Linux implementation does not add any specific calls to the USR_OSS
 library.
*/

/*! \page dummy
 \menimages
*/

/**********************************************************************/
/** Return ident string of UOS module
 * \copydoc usr_oss_specification.c::UOS_Ident()
 */
char* UOS_Ident( void )
{
	return("UOS - User Operating System Services: %FSREV LINUX/LIBSRC/usr_oss 2.14 2012-08-07%");
}







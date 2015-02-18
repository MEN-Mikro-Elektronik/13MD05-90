/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ident.c
 *
 *      \author  see
 *        $Date: 2013/11/14 17:23:10 $
 *    $Revision: 1.14 $
 *
 *     \project  UTL library
 *  	 \brief  USR_UTL global edition history
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ident.c,v $
 * Revision 1.14  2013/11/14 17:23:10  ts
 * R: new Rev. with 3 more sourcefiles
 *
 * Revision 1.13  2013/11/05 14:38:13  ts
 * (AUTOCI) Checkin due to new revision 1.29 of fileset COM/LIBSRC/usr_utl
 *
 * Revision 1.12  2012/08/07 19:13:37  ts
 * (AUTOCI) Checkin due to new revision 1.28 of fileset COM/LIBSRC/usr_utl
 *
 * Revision 1.11  2012/05/14 21:41:32  ts
 * (AUTOCI) Checkin due to new revision 1.27 of fileset COM/LIBSRC/usr_utl
 *
 * Revision 1.10  2011/05/25 16:12:35  CRuff
 * (AUTOCI) Checkin due to new revision 1.26 of fileset COM/LIBSRC/usr_utl
 *
 * Revision 1.9  2010/08/26 17:24:29  CRuff
 * (AUTOCI) Checkin due to new revision 1.25 of fileset COM/LIBSRC/usr_utl
 *
 * Revision 1.8  2010/06/21 20:08:43  ch
 * (AUTOCI) Checkin due to new revision 1.23 of fileset COM/LIBSRC/usr_utl
 *
 * Revision 1.7  2009/07/22 13:43:50  dpfeuffer
 * (AUTOCI) Checkin due to new revision 1.21 of fileset COM/LIBSRC/usr_utl
 *
 * Revision 1.6  2009/07/22 13:27:03  dpfeuffer
 * R: Generate doxygen documentation for MDIS5
 * M: file and function headers changed for doxygen
 *
 * Revision 1.5  2009/03/31 10:56:10  ufranke
 * cosmetics
 *
 * Revision 1.4  2004/10/14 15:55:52  dpfeuffer
 * new ident string
 *
 * Revision 1.3  1999/02/15 15:13:02  see
 * new ident string
 *
 * Revision 1.2  1998/11/05 09:45:01  see
 * new ident string
 *
 * Revision 1.1  1998/08/13 10:26:03  see
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1998 MEN Mikro Elektronik GmbH. All rights reserved.
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/LIBSRC/USR_UTL/COM/ident.c,v 1.14 2013/11/14 17:23:10 ts Exp $";

#include <MEN/men_typs.h>
#include <stdio.h>
#include <MEN/usr_utl.h>

/**********************************************************************/
/** Return ident string of UTL module
 *
 *  \return			pointer to ident string
 */
char* UTL_Ident( void )
{
	return("UTL - User Utilities: %FSREV COM/LIBSRC/usr_utl 1.30 2013-11-14%");
}

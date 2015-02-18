/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_ident.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2014/01/20 17:44:44 $
 *    $Revision: 2.3 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  OSS user space library version info
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ossu_ident.c,v $
 * Revision 2.3  2014/01/20 17:44:44  awerner
 * (AUTOCI) Checkin due to new revision 1.3 of fileset LINUX/LIBSRC/oss_usr
 *
 * Revision 2.2  2007/07/16 13:33:09  cs
 * (AUTOCI) Checkin due to new revision 1.2 of fileset LINUX/LIBSRC/oss_usr
 *
 * Revision 2.1  2007/07/16 09:55:20  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2007 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include "oss_intern.h"

static const char *IdentString="OSS - MEN Linux Operating System Services for user space: %FSREV LINUX/LIBSRC/oss_usr 1.3 2014-01-20%";

char* OSS_Ident( void )
{
    return( (char*)IdentString );
}



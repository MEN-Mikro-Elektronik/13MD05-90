/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: ident.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2003/02/21 13:34:33 $
 *    $Revision: 2.0 $
 *
 *  Description: Return MK ident string
 *
 *     Required: -
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: ident.c,v $
 * Revision 2.0  2003/02/21 13:34:33  kp
 * Added interface for non-MDIS drivers.
 * Prepare for RTAI
 *
 * Revision 1.3  2003/02/21 10:11:49  kp
 * fixed problem when mapping PCI I/O spaces
 *
 * Revision 1.2  2001/09/18 15:27:50  kp
 * Ident string changed
 *
 * Revision 1.1  2001/01/19 14:58:31  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "mk_intern.h"

/*********************************** MDIS_Ident *******************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
char* MDIS_Ident( void )
{
	return("MK - MDIS Kernel: $Id: ident.c,v 2.0 2003/02/21 13:34:33 kp Exp $");
}



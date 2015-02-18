/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: errmsg.c
 *      Project: PLD loader library
 *
 *       Author: see
 *        $Date: 1999/04/26 15:14:33 $
 *    $Revision: 1.4 $
 *
 *  Description: Return PLD Loader error message string
 *
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 * PLD_ErrorMsg - Return PLD error message string
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: errmsg.c,v $
 * Revision 1.4  1999/04/26 15:14:33  Schmidt
 * includes added to support different variants
 *
 * Revision 1.3  1998/10/08 15:02:26  see
 * PLD_ErrorMsg: return string instead static *msg
 *
 * Revision 1.2  1998/06/02 09:27:22  see
 * RCSid type changed from char* to char[]
 *
 * Revision 1.1  1998/02/25 09:48:00  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/LIBSRC/PLD/COM/errmsg.c,v 1.4 1999/04/26 15:14:33 Schmidt Exp $";

#include "pld_var.h"		/* defines variants */
#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/maccess.h>
#include <MEN/pld_load.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
/* none */

/********************************* PLD_ErrorMsg *****************************
 *
 *  Description: Return PLD error message string
 *			
 *---------------------------------------------------------------------------
 *  Input......: error		PLD_ERR_xxx error number
 *  Output.....: return		pointer to error message
 *  Globals....: -
 ****************************************************************************/
char *PLD_ErrorMsg(int32 error)
{
	switch(error) {
		case 0:					return("PLD successful loaded");
		case PLD_ERR_NOTFOUND:	return("PLD did not respond");
		case PLD_ERR_INIT:		return("PLD initialization failed");
		case PLD_ERR_LOAD:		return("PLD data loading failed");
		case PLD_ERR_TERM:		return("PLD termination failed");
	}

	return("PLD unknown error");
}



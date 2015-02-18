/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: ident.c
 *      Project:
 *
 *       Author: ds
 *        $Date: 2012/05/23 18:09:19 $
 *    $Revision: 1.50 $
 *
 *  Description: Return D201 ident string
 *
 *     Required: -
 *     Switches: DBG	  	enable debugging
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *  D201_Ident              Return ident string
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: ident.c,v $
 * Revision 1.50  2012/05/23 18:09:19  ts
 * (AUTOCI) Checkin due to new revision 1.57 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.49  2011/05/24 16:40:59  CRuff
 * (AUTOCI) Checkin due to new revision 1.56 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.48  2010/06/21 20:49:16  ch
 * (AUTOCI) Checkin due to new revision 1.55 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.47  2010/01/28 15:35:02  ufranke
 * (AUTOCI) Checkin due to new revision 1.54 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.46  2009/11/09 13:43:45  CKauntz
 * (AUTOCI) Checkin due to new revision 1.53 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.45  2009/09/09 13:30:07  dpfeuffer
 * (AUTOCI) Checkin due to new revision 1.52 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.44  2009/08/21 13:38:52  dpfeuffer
 * (AUTOCI) Checkin due to new revision 1.51 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.43  2009/08/06 09:13:27  CRuff
 * (AUTOCI) Checkin due to new revision 1.50 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.42  2009/05/29 13:41:41  ufranke
 * (AUTOCI) Checkin due to new revision 1.49 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.41  2009/04/01 14:21:02  ufranke
 * (AUTOCI) Checkin due to new revision 1.48 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.40  2008/09/26 15:19:08  ufranke
 * (AUTOCI) Checkin due to new revision 1.47 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.39  2008/01/25 13:51:00  ufranke
 * (AUTOCI) Checkin due to new revision 1.46 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.38  2007/12/10 11:53:14  ts
 * (AUTOCI) Checkin due to new revision 1.45 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.37  2007/03/28 16:44:41  ts
 * (AUTOCI) Checkin due to new revision 1.44 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.36  2007/02/23 18:01:03  DPfeuffer
 * (AUTOCI) Checkin due to new revision 1.43 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.35  2006/12/21 11:04:19  ufranke
 * (AUTOCI) Checkin due to new revision 1.42 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.34  2006/12/15 16:25:12  ts
 * (AUTOCI) Checkin due to new revision 1.41 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.33  2006/12/15 14:40:04  ts
 * (AUTOCI) Checkin due to new revision 1.40 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.32  2006/10/19 14:16:35  cs
 * (AUTOCI) Checkin due to new revision 1.39 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.31  2006/07/25 15:32:12  ufranke
 * (AUTOCI) Checkin due to new revision 1.38 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.30  2006/07/17 14:43:19  ufranke
 * (AUTOCI) Checkin due to new revision 1.37 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.29  2006/06/02 11:37:43  ufranke
 * (AUTOCI) Checkin due to new revision 1.36 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.28  2005/12/23 15:37:18  UFRANKE
 * (AUTOCI) Checkin due to new revision 1.35 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.27  2005/08/11 16:58:11  ts
 * (AUTOCI) Checkin due to new revision 1.34 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.26  2005/06/30 11:04:24  UFranke
 * cosmetics
 *
 * Revision 1.25  2005/01/31 15:36:52  ts
 * (AUTOCI) Checkin due to new revision 1.32 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.24  2004/10/15 14:28:54  dpfeuffer
 * (AUTOCI) Checkin due to new revision 1.31 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.23  2004/07/30 12:40:35  ufranke
 * (AUTOCI) Checkin due to new revision 1.30 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.22  2004/06/21 10:59:12  dpfeuffer
 * (AUTOCI) Checkin due to new revision 1.29 of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.21  2004/06/21 10:25:03  dpfeuffer
 * (AUTOCI) Checkin due to new revision Undefined Release of fileset COM/DRIVERS/BBIS/d201
 *
 * Revision 1.20  2004/06/09 10:42:24  kp
 * new rev
 *
 * Revision 1.19  2004/05/14 12:31:10  UFranke
 * cosmetics
 *
 * Revision 1.18  2004/03/24 11:02:24  ub
 * Revision string updated
 *
 * Revision 1.17  2004/03/11 09:16:13  dpfeuffer
 * new rev
 *
 * Revision 1.16  2004/03/10 17:50:51  dpfeuffer
 * use FSREV for ident string
 *
 * Revision 1.15  2001/11/14 16:51:45  Schmidt
 * modified for D201D302 BBIS
 *
 * Revision 1.14  2001/06/20 16:11:04  Schmidt
 * - PLX BAR0 bug workaround modified for F201, C203, C204 carrier board
 * - D201_IrqSrvInit: debug code now only in debug driver
 *
 * Revision 1.13  2001/01/11 12:16:48  Schmidt
 * PLX BAR0 bug workaround added, OSS_UnAssignResources added, now using PLD 1R4
 *
 * Revision 1.12  2000/08/31 14:40:23  Schmidt
 * Bugfix of D201_BrdInfo(BBIS_BRDINFO_FUNCTION), D201_ClrMIface
 *
 * Revision 1.11  2000/06/13 09:49:50  kp
 * Now using PLD 1R3
 *
 * Revision 1.10  2000/03/17 15:14:14  kp
 * Now supporting the PCI_BUS_PATH key
 *
 * Revision 1.9  2000/03/09 12:18:55  kp
 * added "swapped" info in ident function
 *
 * Revision 1.8  2000/03/01 14:56:54  kp
 * EepromReadBuf was a global symbol. Renamed this variant specific
 * Removed all global (static variables). Sustituted with defines
 * Made PLD data const array
 *
 * Revision 1.7  1999/09/08 09:30:09  Schmidt
 * Ident string depends now on the variant
 *
 * Revision 1.6  1998/09/21 10:25:55  Schmidt
 * force revision 1.6
 *
 * Revision 1.1  1998/09/21 10:18:28  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <MEN/men_typs.h>
#include "d201.h"

/*********************************** D201_Ident *****************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
char* Ident( void ) /* nodoc */
{
	return (

#ifdef _D201
		"D201 - D201"
#endif
#ifdef _C203
		"C203 - C203"
#endif
#ifdef _C204
		"C204 - C204"
#endif
#ifdef _F201
		"F201 - F201"
#endif
#ifdef _F202
		"F202 - F202"
#endif
#ifdef MAC_BYTESWAP
		" Swapped"
#endif
		" Base Board Handler: %FSREV COM/DRIVERS/BBIS/d201 1.57 2012-05-23%" );
}










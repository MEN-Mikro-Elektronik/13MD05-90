/****************************************************************************
 ************                                                    ************
 ************               M _ W R I T E                        ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/04/01 14:36:41 $
 *    $Revision: 1.6 $
 *
 *  Description: Testing M_write()
 *
 *     Required: mdis_api.l, usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m_write.c,v $
 * Revision 1.6  2009/04/01 14:36:41  ufranke
 * cosmetics
 *
 * Revision 1.5  2008/01/18 14:37:37  ufranke
 * cosmetics
 *
 * Revision 1.4  2006/10/18 13:46:10  cs
 * added:
 *   + include "string.h", avoid warnings
 *
 * Revision 1.3  1999/04/20 14:55:07  kp
 * declare usage() as static, main() as int
 * eliminate compiler warnings
 *
 * Revision 1.2  1998/10/15 13:56:37  see
 * check for M_close error
 * V1.1
 * exit if M_open failed
 *
 * Revision 1.1  1998/08/11 09:53:23  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: m_write.c,v 1.6 2009/04/01 14:36:41 ufranke Exp $";

#include <MEN/men_typs.h>

#include <stdio.h>
#include <string.h>

#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define REV "V1.1"      /* program revision */

/********************************* usage ************************************
 *
 *  Description: Print program usage and revision
 *
 *---------------------------------------------------------------------------
 *  Input......: -
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void usage(void)
{
	printf("Usage: m_write <device> <value>\n");
	printf("Function: Testing M_write() function\n");
	printf("Options:\n");
	printf("    device       device name           [none]\n");
	printf("    value        value to write (hex)  [none]\n");
	printf("\n");
	printf("(c) 1998 by MEN mikro elektronik GmbH, %s\n\n",REV);
}

/********************************* main *************************************
 *
 *  Description: Program main function
 *
 *---------------------------------------------------------------------------
 *  Input......: argc,argv	argument counter, data ..
 *  Output.....: return		success (0) or error (1)
 *  Globals....: -
 ****************************************************************************/
int main(argc,argv)
int  argc;
char *argv[];
{
	int32 path,data;
	char *device,*errstr,errbuf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("?", errbuf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (argc<3 || UTL_TSTOPT("?")) {			/* help requested ? */
		usage();
		return(1);
	}

	/*--------------------+
    |  get arguments      |
    +--------------------*/
	device = argv[1];
	data = UTL_Atox(argv[2]);

	/*--------------------+
    |  process ..         |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	printf("write: value=0x%lx (%ld) written\n",data,data);

	if (M_write(path,data) < 0) {
		printf("*** can't write: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	abort:
	if (M_close(path) < 0) {
		printf("*** can't close: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	return(0);
}



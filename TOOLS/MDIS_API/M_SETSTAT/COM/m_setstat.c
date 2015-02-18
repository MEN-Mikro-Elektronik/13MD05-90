/****************************************************************************
 ************                                                    ************
 ************               M _ S E T S T A T                    ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/04/01 14:36:36 $
 *    $Revision: 1.6 $
 *
 *  Description: Testing M_setstat()
 *
 *     Required: mdis_api.l, usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m_setstat.c,v $
 * Revision 1.6  2009/04/01 14:36:36  ufranke
 * cosmetics
 *
 * Revision 1.5  2008/09/15 15:03:17  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.4  2006/10/18 13:46:06  cs
 * added:
 *   + include "string.h", avoid warnings
 *
 * Revision 1.3  1999/04/20 14:55:00  kp
 * declare usage() as static, main() as int
 * eliminate compiler warnings
 *
 * Revision 1.2  1998/10/15 13:56:26  see
 * check for M_close error
 * V1.1
 * exit if M_open failed
 *
 * Revision 1.1  1998/08/11 09:53:19  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: m_setstat.c,v 1.6 2009/04/01 14:36:36 ufranke Exp $";

#include <MEN/men_typs.h>

#include <stdio.h>
#include <string.h>

#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

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
	printf("Usage: m_setstat <device> <code> <value>\n");
	printf("Function: Testing M_setstat() function\n");
	printf("Options:\n");
	printf("    device       device name           [none]\n");
	printf("    code         status code (hex)     [none]\n");
	printf("    value        data value (hex)      [none]\n");
	printf("\n");
	printf("(c) 1998 by MEN mikro elektronik GmbH\n %s\n\n",RCSid);
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
	int32 path,code,data;
	char *device,*errstr,errbuf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("?", errbuf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (argc<4 || UTL_TSTOPT("?")) {			/* help requested ? */
		usage();
		return(1);
	}

	/*--------------------+
    |  get arguments      |
    +--------------------*/
	device = argv[1];
	code = UTL_Atox(argv[2]);
	data = UTL_Atox(argv[3]);

	/*--------------------+
    |  process ..         |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	printf("setstat code=0x%04x: value=0x%x (%d) written\n",(int)code,(int)data,(int)data);

	if (M_setstat(path,code,data)) {
		printf("*** can't setstat: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	abort:
	if (M_close(path) < 0) {
		printf("*** can't close: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	return(0);
}



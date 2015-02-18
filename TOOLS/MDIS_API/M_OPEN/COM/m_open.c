/****************************************************************************
 ************                                                    ************
 ************               M _ O P E N                          ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/04/01 14:36:27 $
 *    $Revision: 1.6 $
 *
 *  Description: Testing M_open()
 *
 *     Required: mdis_api.l, usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m_open.c,v $
 * Revision 1.6  2009/04/01 14:36:27  ufranke
 * cosmetics
 *
 * Revision 1.5  2008/09/15 15:00:15  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.4  2006/10/18 13:46:00  cs
 * added:
 *   + include "string.h", avoid warnings
 *
 * Revision 1.3  1999/04/20 14:54:50  kp
 * declare usage() as static, main() as int
 * eliminate compiler warnings
 *
 * Revision 1.2  1998/10/15 13:56:14  see
 * check for M_close error
 * V1.1
 * exit if M_open failed
 *
 * Revision 1.1  1998/08/11 09:53:13  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: m_open.c,v 1.6 2009/04/01 14:36:27 ufranke Exp $";

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
	printf("Usage: m_open [<opts>] <device> [<opts>]\n");
	printf("Function: Testing M_open() function\n");
	printf("Options:\n");
	printf("    device       device name                    [none]\n");
	printf("    -w           wait for signal before close   [OFF]\n");
	printf("\n");
	printf("(c) 1998..2008 by MEN mikro elektronik GmbH\n %s\n\n",RCSid);
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
	int32 path,wait,n;
	u_int32 occured;
	char *device,*errstr,errbuf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("w?", errbuf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (UTL_TSTOPT("?")) {						/* help requested ? */
		usage();
		return(1);
	}

	/*--------------------+
    |  get arguments      |
    +--------------------*/
	for (device=NULL, n=1; n<argc; n++)			/* search for name */
		if (*argv[n] != '-') {
			device = argv[n];
			break;
		}

	if (!device) {					/* not found ? */
		usage();
		return(1);
	}

	wait = (UTL_TSTOPT("w") ? 1 : 0);

	/*--------------------+
    |  process ..         |
    +--------------------*/
	printf("open %s\n",device);

	if ((path = M_open(device)) < 0) {
		printf("*** can't open: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	printf("path=%d opened\n",(int)path);

	if (wait) {
		UOS_SigInit(NULL);
		printf("\nwait for deadly signal ..\n");
		UOS_SigWait(0, &occured);
	}

	printf("close path\n");

	if (M_close(path) < 0) {
		printf("*** can't close: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	return(0);
}




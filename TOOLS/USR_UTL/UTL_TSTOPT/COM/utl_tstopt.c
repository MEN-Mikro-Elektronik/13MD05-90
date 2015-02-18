/****************************************************************************
 ************                                                    ************
 ************              U O S _ T S T O P T                   ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:46:22 $
 *    $Revision: 1.2 $
 *
 *  Description: Test USR_UTL functions UTL_Tstopt/Illiopt
 *
 *     Required: usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: utl_tstopt.c,v $
 * Revision 1.2  1999/04/20 15:46:22  kp
 * cosmetics
 *
 * Revision 1.1  1998/08/10 11:28:52  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_UTL/UTL_TSTOPT/COM/utl_tstopt.c,v 1.2 1999/04/20 15:46:22 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_utl.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define REV "V1.0"      /* program revision */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
void usage(void);

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
	printf("Usage: utl_tstopt [<opts>] <device> [<opts>]\n");
	printf("Function: Testing UTL_Tstopt function\n");
	printf("Options:\n");
	printf("    device       device name           [none]\n");
	printf("    -c=<chan>    channel (1..6)        [1]\n");
	printf("    -v           verbose               [off]\n");
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
	u_int32 n,chan,verbose;
	char *device,*str,*errstr,buf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("c=v?", buf))) {	/* check args */
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
		printf("*** device not specified\n");
		return(1);
	}

	chan      = ((str = UTL_TSTOPT("c=")) ? atoi(str) : 1);
	verbose   = (UTL_TSTOPT("v") ? 1 : 0);

	printf("chan=%d\n", chan);
	printf("verbose=%d\n", verbose);
	printf("device=%s\n", device);

	return(0);
}

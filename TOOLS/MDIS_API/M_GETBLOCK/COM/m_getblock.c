/****************************************************************************
 ************                                                    ************
 ************               M _ G E T B L O C K                  ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/04/01 14:36:18 $
 *    $Revision: 1.6 $
 *
 *  Description: Testing M_getblock()
 *
 *     Required: mdis_api.l, usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m_getblock.c,v $
 * Revision 1.6  2009/04/01 14:36:18  ufranke
 * cosmetics
 *
 * Revision 1.5  2008/09/15 14:56:39  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.4  2006/09/11 10:47:04  cs
 * added include of string.h
 *
 * Revision 1.3  1999/04/20 14:54:40  kp
 * declare usage() as static, main() as int
 * eliminate compiler warnings
 *
 * Revision 1.2  1998/10/15 13:56:02  see
 * bug fixed: blksize was not initialized
 * check for M_close error
 * V1.2
 * exit if M_open failed
 * options -c -b -t added
 * V1.1
 *
 * Revision 1.1  1998/08/11 09:53:06  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: m_getblock.c,v 1.6 2009/04/01 14:36:18 ufranke Exp $";

#include <MEN/men_typs.h>

#include <stdio.h>
#include <stdlib.h>
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
	printf("Usage: m_getblock [<opts>] <device> <size> [<opts>]<\n");
	printf("Function: Testing M_getblock function\n");
	printf("Options:\n");
	printf("    device       device name                          [none]\n");
	printf("    size         block size                           [none]\n");
	printf("    -c=<chan>    channel number (0..n)                [none]\n");
	printf("    -b=<mode>    block i/o mode                       [none]\n");
	printf("                 0 = M_BUF_USRCTRL\n");
	printf("                 1 = M_BUF_CURRBUF\n");
	printf("                 2 = M_BUF_RINGBUF\n");
	printf("                 3 = M_BUF_RINGBUF_OVERWR\n");
	printf("    -t=<msec>    block read timeout [msec] (0=none)   [none]\n");
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
	int32 path=-1,chan,blksize=0,tout,blkmode,gotsize,n;
	char *blkbuf = NULL;
	char *device,*str,*errstr,errbuf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("c=b=t=?", errbuf))) {	/* check args */
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
	for (device=NULL, n=1; n<argc; n++) {
		if (*argv[n] != '-') {
			if (device == NULL)
				device = argv[n];
			else {
				blksize = atoi(argv[n]);
				break;
			}
		}
	}

	if (!device || blksize==0) {
		usage();
		return(1);
	}

	chan      = ((str = UTL_TSTOPT("c=")) ? atoi(str) : -1);
	blkmode   = ((str = UTL_TSTOPT("b=")) ? atoi(str) : -1);
	tout      = ((str = UTL_TSTOPT("t=")) ? atoi(str) : -1);

	/*--------------------+
    |  create buffer      |
    +--------------------*/
	if ((blkbuf = malloc(blksize)) == NULL) {
		printf("*** can't alloc %d bytes\n",(int)blksize);
		goto abort;
	}

	/*--------------------+
    |  open+config        |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		printf("*** can't open: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	/* channel number */
	if (chan != -1) {
		printf("set current channel=%d\n",(int)chan);

		if ((M_setstat(path, M_MK_CH_CURRENT, chan)) < 0) {
			printf("*** can't setstat M_MK_CH_CURRENT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
	}

	/* block i/o mode */
	if (blkmode != -1) {
		printf("set block i/o mode=%d\n",(int)blkmode);

		if ((M_setstat(path, M_BUF_RD_MODE, blkmode)) < 0) {
			printf("*** can't setstat M_BUF_RD_MODE: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
	}

	/* block read timeout */
	if (tout != -1) {
		printf("set block read timeout=%d msec\n",(int)tout);

		if ((M_setstat(path, M_BUF_RD_TIMEOUT, tout)) < 0) {
			printf("*** can't setstat M_BUF_RD_TIMEOUT: %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
	}

	/*--------------------+
    |  write buffer       |
    +--------------------*/
	printf("blockread: reading size=%d bytes\n",(int)blksize);

	if ((gotsize = M_getblock(path,(u_int8*)blkbuf,blksize)) < 0) {
		printf("*** can't getblock: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	UTL_Memdump("\nread data",blkbuf,gotsize,1);

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:

	if (blkbuf)
		free(blkbuf);

	if (M_close(path) < 0) {
		printf("*** can't close: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	return(0);
}



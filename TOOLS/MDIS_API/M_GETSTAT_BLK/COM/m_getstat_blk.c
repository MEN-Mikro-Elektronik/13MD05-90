/****************************************************************************
 ************                                                    ************
 ************               M _ G E T S T A T _ B L K            ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/04/01 14:36:22 $
 *    $Revision: 1.6 $
 *
 *  Description: Testing M_getstat() (blockwise)
 *
 *     Required: mdis_api.l, usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m_getstat_blk.c,v $
 * Revision 1.6  2009/04/01 14:36:22  ufranke
 * cosmetics
 *
 * Revision 1.5  2008/09/15 14:59:14  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.4  2006/10/18 13:45:57  cs
 * added:
 *   + include "string.h", avoid warnings
 *
 * Revision 1.3  1999/04/20 14:54:47  kp
 * declare usage() as static, main() as int
 * eliminate compiler warnings
 *
 * Revision 1.2  1998/10/15 13:56:10  see
 * check for M_close error
 * V1.1
 * exit if M_open failed
 *
 * Revision 1.1  1998/08/11 09:53:11  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: m_getstat_blk.c,v 1.6 2009/04/01 14:36:22 ufranke Exp $";

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
	printf("Usage: m_getstat_blk <device> <code> <size>\n");
	printf("Function: Testing M_getstat() function (blockwise)\n");
	printf("Options:\n");
	printf("    device       device name           [none]\n");
	printf("    code         status code (hex)     [none]\n");
	printf("    size         block size            [none]\n");
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
	int32 path,code,size;
	M_SG_BLOCK blk;
	char *buf = NULL;
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
	size = atoi(argv[3]);

	if ((code & M_OFFS_BLK) == 0) {
		printf("*** not a block code\n");
		return(1);
	}

	if ((path = M_open(device)) < 0) {
		printf("*** can't open: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	if ((buf = malloc(size)) == NULL) {
		printf("*** can't alloc %d bytes\n",(int)size);
		goto abort;
	}

	blk.size = size;
	blk.data = (void*)buf;

	printf("block getstat: code=0x%04x, size=%d\n",(int)code,(int)size);

	if (M_getstat(path,code,(int32*)&blk) < 0) {
		printf("*** can't getstat: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	UTL_Memdump("\nread data",blk.data,blk.size,1);

	abort:

	if (buf)
		free(buf);

	if (M_close(path) < 0) {
		printf("*** can't close: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	return(0);
}



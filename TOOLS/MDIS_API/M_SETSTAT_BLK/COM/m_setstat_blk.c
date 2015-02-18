/****************************************************************************
 ************                                                    ************
 ************               M _ S E T S T A T _ B L K            ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2010/09/02 16:18:02 $
 *    $Revision: 1.9 $
 *
 *  Description: Testing M_setstat() (blockwise)
 *
 *     Required: mdis_api.l, usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m_setstat_blk.c,v $
 * Revision 1.9  2010/09/02 16:18:02  UFranke
 * R: compiler warning under VxWorks GNU
 * M: cosmetics
 *
 * Revision 1.8  2009/09/23 13:02:48  CRuff
 * R: not 64bit compatible
 * M: added type conversions to make 64bit compatible on linux system
 *
 * Revision 1.7  2009/08/05 08:52:36  CRuff
 * R: porting to MDIS5 (64bit)
 * M: adapt printf() format to be able to print 64bit addresses
 *
 * Revision 1.6  2009/04/01 14:36:38  ufranke
 * cosmetics
 *
 * Revision 1.5  2008/09/15 15:03:49  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.4  2006/10/18 13:46:08  cs
 * added:
 *   + include "string.h", avoid warnings
 *
 * Revision 1.3  1999/04/20 14:55:04  kp
 * declare usage() as static, main() as int
 * eliminate compiler warnings
 *
 * Revision 1.2  1998/10/15 13:56:33  see
 * check for M_close error
 * V1.1
 * exit if M_open failed
 *
 * Revision 1.1  1998/08/11 09:53:21  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2010 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: m_setstat_blk.c,v 1.9 2010/09/02 16:18:02 UFranke Exp $";

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
	printf("Usage: m_setstat_blk <device> <code> <<byte0> [<byte1>..]\n");
	printf("Function: Testing M_setstat() function (blockwise)\n");
	printf("Options:\n");
	printf("    device       device name           [none]\n");
	printf("    code         status code (hex)     [none]\n");
	printf("    byte0..n     block data bytes (hex)  [none]\n");
	printf("\n");
	printf("(c) 1998..2010 by MEN mikro elektronik GmbH\n %s\n\n", RCSid);
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
	int32 path,code,size,n;
	M_SG_BLOCK blk;
	char *buf = NULL;
	u_int32 *p = NULL;
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
	size = argc-3;

	/*--------------------+
    |  process ..         |
    +--------------------*/
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
		goto ABORT;
	}
	p = (u_int32*)buf;

	blk.size = size;
	blk.data = (void*)buf;

	printf("size=%d, data=0x%p\n",(int)blk.size,(void*)blk.data);

	for (n=0; n<size; n++)
		*p++ = UTL_Atox(argv[3+n]);

	printf("block setstat: code=0x%04x, size=%d\n",(int)code,(int)size);

	if (M_setstat(path,code,(INT32_OR_64)&blk) < 0) {
		printf("*** can't setstat: %s\n",M_errstring(UOS_ErrnoGet()));
		goto ABORT;
	}

	UTL_Memdump("\nwritten data",blk.data,blk.size,1);

ABORT:
	if (buf)
		free(buf);

	if (M_close(path) < 0) {
		printf("*** can't close: %s\n",M_errstring(UOS_ErrnoGet()));
		return(1);
	}

	return(0);
}




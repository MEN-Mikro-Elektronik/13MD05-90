/****************************************************************************
 ************                                                    ************
 ************               M _ M O D _ I D                      ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 2009/09/23 13:02:45 $
 *    $Revision: 2.4 $
 *
 *  Description: Print module id
 *
 *     Required: mdis_api.l, usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m_mod_id.c,v $
 * Revision 2.4  2009/09/23 13:02:45  CRuff
 * R: not 64bit compatible
 * M: added type conversions to make 64bit compatible on linux system
 *
 * Revision 2.3  2009/04/01 14:36:24  ufranke
 * cosmetics
 *
 * Revision 2.2  2008/09/05 11:06:43  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 2.1  2006/07/20 15:01:52  ufranke
 * cosmetics
 *
 * Revision 2.0  2002/06/06 15:35:55  kp
 * Added programming mode (requires MMODPRG driver)
 *
 * Revision 1.4  2000/03/23 17:08:40  Franke
 * cosmetics
 *
 * Revision 1.3  1999/07/21 14:43:51  Franke
 * cosmetics
 *
 * Revision 1.2  1999/04/20 15:08:18  kp
 * cosmetics
 *
 * Revision 1.1  1998/08/11 10:39:48  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: m_mod_id.c,v 2.4 2009/09/23 13:02:45 CRuff Exp $";

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
#define REV "V2.0"      /* program revision */

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
	printf("Usage: Show:    m_mod_id [<opts>] <device>\n");
	printf("Usage: Program: m_mod_id [<opts>] <device> <prgspec>\n");
	printf("Function: Print or program module id\n");
	printf("Options:\n");
	printf("    device       device name           [none]\n");
	printf("    progspec     programming specification file\n");
	printf("\n");
	printf("(c) 1998-2002 by MEN mikro elektronik GmbH, %s\n\n",REV);
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
	int32 path,size;
	M_SG_BLOCK blk;
	char *buf = NULL;
	char *device,*errstr,strbuf[100], *progfile=NULL;
	FILE *fp=NULL;

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("?", strbuf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (argc<2 || UTL_TSTOPT("?")) {			/* help requested ? */
		usage();
		return(1);
	}

	/*--------------------+
    |  get arguments      |
    +--------------------*/
	device = argv[1];
	if( argc > 2 )
		progfile = argv[2];

	/* open device */
	if ((path = M_open(device)) < 0) {
		printf("*** can't open: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* get required buffer size */
	if (M_getstat(path,M_LL_ID_SIZE,&size) < 0) {
		printf("*** can't getstat: %s\n",M_errstring(UOS_ErrnoGet()));
		goto abort;
	}

	/* alloc buffer */
	if ((buf = malloc(size)) == NULL) {
		printf("*** can't alloc %ld bytes\n",size);
		goto abort;
	}

	blk.size = size;
	blk.data = (void*)buf;

	if( progfile == NULL ){
		/*--------+
		|  Show   |
		+--------*/

		/* get id prom data */
		if (M_getstat(path,M_LL_BLK_ID_DATA,(int32*)&blk) < 0) {
			printf("*** can't getstat: %s\n",M_errstring(UOS_ErrnoGet()));
			goto abort;
		}

		/* print id prom data */
		UTL_Memdump("id prom data",blk.data, blk.size, 2);
	}
	else {
		int nwords=0;
		int val;
		u_int16 *bufp = (u_int16 *)buf;

		/*----------+
		|  Program  |
		+----------*/
		fp = fopen(progfile, "r");
		
		if( fp == NULL ){
			perror("Can't open program file");
			goto abort;
		}

		while( fgets( strbuf, sizeof(strbuf), fp )){

			if( strbuf[0] == '*' ) /* check for comment */
				printf("%s", strbuf );

			else {
				char *s = strbuf;
				
				while( *s ){
					/* skip white space */
					while( *s == ' ' || *s == '\t' || *s == '\n' ||
						   *s == '\r'  )
						s++;

					if( *s == '\n' || *s == '\r' || *s == '\0' )
						break;

					if( sscanf( s, "%x", &val ) != 1){
						printf("Error scanning <%s>\n", s );
						goto abort;
					}
					*bufp++ = (u_int16)val;
					nwords++;

					/* skip until white space */
					while( *s && (*s != ' ') && (*s != '\t') )
						s++;
				}
			}
		}

		printf("\nProgramming %d words into IDPROM...\n", nwords);
		blk.size = nwords * 2;
		/* get id prom data */

		if (M_setstat(path,M_LL_BLK_ID_DATA,(INT32_OR_64)&blk) < 0) {
			printf("*** Error while programming %s\n",
				   M_errstring(UOS_ErrnoGet()));
			goto abort;
		}
		printf("Programming done.\n");
	}
	abort:

	if( fp )
		fclose(fp);

	if (buf)
		free(buf);

	if( path != -1 )
		M_close(path);

	return(0);
}


/****************************************************************************
 ************                                                    ************
 ************              U O S _ R A N D                       ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:16 $
 *    $Revision: 1.3 $
 *
 *  Description: Test USR_OSS function UOS_Random
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_rand.c,v $
 * Revision 1.3  1999/04/20 15:45:16  kp
 * cosmetics
 *
 * Revision 1.2  1998/08/18 14:04:14  Schmidt
 * include stdlib.h added
 *
 * Revision 1.1  1998/08/07 14:15:02  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_RAND/COM/uos_rand.c,v 1.3 1999/04/20 15:45:16 kp Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>


int main(argc,argv)
int  argc;
char **argv;
{
	u_int32 rand, loop=64;

	if (argc<2 || strcmp(argv[1],"-?")==0)  {
		printf("uos_sigrand <startval>\n");
		return(1);
	}

	rand = atoi(argv[1]);

	printf("start with value=%d\n",rand);

	while(loop) {
		rand = UOS_Random(rand);
		printf("0x%08x ",rand);
		fflush(stdout);

		if ((--loop % 4)==0)
			printf("\n");
	}

	return(0);
}


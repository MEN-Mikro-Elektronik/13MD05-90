/****************************************************************************
 ************                                                    ************
 ************              U O S _ R A N D M A P                 ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:18 $
 *    $Revision: 1.2 $
 *
 *  Description: Test USR_OSS function UOS_RandomMap
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_randmap.c,v $
 * Revision 1.2  1999/04/20 15:45:18  kp
 * cosmetics
 *
 * Revision 1.1  1998/08/07 14:15:05  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_RANDMAP/COM/uos_randmap.c,v 1.2 1999/04/20 15:45:18 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>


int main(argc,argv)
int  argc;
char **argv;
{
	u_int32 val,ra,re,map;

	if (argc<4 || strcmp(argv[1],"-?")==0)  {
		printf("uos_randmap <hex_val> <hex_ra> <hex_re>\n");
		return(1);
	}

	sscanf(argv[1],"%x",&val);
	sscanf(argv[2],"%x",&ra);
	sscanf(argv[3],"%x",&re);

	map = UOS_RandomMap(val,ra,re);

	printf("value=0x%08x ra=0x%08x re=0x%08x => mapped=0x%08x\n",val,ra,re,map);

	return(0);
}



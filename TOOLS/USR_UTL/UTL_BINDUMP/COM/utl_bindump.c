/****************************************************************************
 ************                                                    ************
 ************              U O S _ B I N D U M P                 ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:46:20 $
 *    $Revision: 1.2 $
 *
 *  Description: Test USR_UTL function UTL_Bindump
 *
 *     Required: usr_utl.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: utl_bindump.c,v $
 * Revision 1.2  1999/04/20 15:46:20  kp
 * cosmetics
 *
 * Revision 1.1  1998/08/10 11:28:55  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_UTL/UTL_BINDUMP/COM/utl_bindump.c,v 1.2 1999/04/20 15:46:20 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_utl.h>

int main(argc,argv)
int  argc;
char *argv[];
{
	u_int32 bits, val;
	char buf[40];

	if (argc<3 || strcmp(argv[1],"-?")==0)  {
		printf("uos_bindump <hexvalue> <bits>\n");
		return(1);
	}

	sscanf(argv[1],"%x",&val);
	bits = atoi(argv[2]);

	printf("value=0x%x, bits=%d\n",val,bits);
	printf("bindump='%s'\n", UTL_Bindump(val,bits,buf));

	return(0);
}

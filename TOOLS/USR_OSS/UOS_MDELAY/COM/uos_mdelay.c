/****************************************************************************
 ************                                                    ************
 ************              U O S _ M D E L A Y                   ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:14 $
 *    $Revision: 1.3 $
 *
 *  Description: Test USR_OSS functions UOS_MikroDelayXXX
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_mdelay.c,v $
 * Revision 1.3  1999/04/20 15:45:14  kp
 * cosmetics
 *
 * Revision 1.2  1998/08/18 14:03:32  Schmidt
 * unused variable 'i' removed
 *
 * Revision 1.1  1998/08/07 14:14:59  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_MDELAY/COM/uos_mdelay.c,v 1.3 1999/04/20 15:45:14 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>

int main(argc,argv)
int  argc;
char **argv;
{
	u_int32 n, t1 ,t2, wait, res, error;

	if ((error = UOS_MikroDelayInit())) {
		printf("*** can't UOS_MikroDelayInit: error=0x%04x\n",error);
		return(0);
	}

	res = UOS_MsecTimerResolution();
	wait = 1000*res;

	for (n=0; n<4; n++, wait *= 10) {
		printf("\nwait %d usec\n",wait);

		t1 = UOS_MsecTimerGet();

		if ((error = UOS_MikroDelay(wait))) {
			printf("*** can't UOS_MikroDelay: error=0x%04x\n",error);
			break;
		}

		t2 = UOS_MsecTimerGet();
		printf("dt=%d usec elapsed\n",(t2-t1)*1000);
	}

	return(0);
}



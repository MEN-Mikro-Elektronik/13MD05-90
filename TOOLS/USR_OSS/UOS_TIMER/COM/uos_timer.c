/****************************************************************************
 ************                                                    ************
 ************              U O S _ T I M E R                     ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:25 $
 *    $Revision: 1.3 $
 *
 *  Description: Test USR_OSS functions UOS_MsecTimerXXX
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_timer.c,v $
 * Revision 1.3  1999/04/20 15:45:25  kp
 * cosmetics
 *
 * Revision 1.2  1998/08/18 14:04:55  Schmidt
 * check the returned time from UOS_Delay() added,
 * unused variable 'i' removed
 *
 * Revision 1.1  1998/08/07 14:15:14  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_TIMER/COM/uos_timer.c,v 1.3 1999/04/20 15:45:25 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>


int main(argc,argv)
int  argc;
char **argv;
{
	u_int32 n, t1 ,t2, wait=1, realwait, res;

	wait = res = UOS_MsecTimerResolution();
	printf("timer resolution=%d msec\n",res);

	for (n=0; n<4; n++, wait *= 10) {
		printf("\nwait %d msec\n",wait);

		t1 = UOS_MsecTimerGet();

		if ((realwait = UOS_Delay(wait)) > wait+res)
			printf("*** delay time=%d inaccurate\n",realwait);
		else {
			t2 = UOS_MsecTimerGet();
			printf("t1=%d\n",t1);
			printf("t2=%d\n",t2);
			printf("dt=%d msec elapsed\n",t2-t1);
		}
	}

	/* check the returned time from UOS_Delay() */
	for (wait=0; wait<res*3; wait+=res/10) {
		printf("\nwait %d msec\n",wait);

		t1 = UOS_MsecTimerGet();

		realwait = UOS_Delay(wait);
		printf("realwait time=%d\n",realwait);

		t2 = UOS_MsecTimerGet();
		printf("t1=%d\n",t1);
		printf("t2=%d\n",t2);
		printf("dt=%d msec elapsed\n",t2-t1);
		
		if( realwait != t2-t1 )
			printf("*** elapsed time is not the realwait time\n");
	}

	return(0);
}



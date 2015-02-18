/****************************************************************************
 ************                                                    ************
 ************              U O S _ S I G M A S K                 ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: kp
 *        $Date: 2001/01/19 14:57:20 $
 *    $Revision: 1.1 $
 *
 *  Description: Test USR_OSS signal functions
 *               Dispatch signals via signal handler and mask signals
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_sigmask.c,v $
 * Revision 1.1  2001/01/19 14:57:20  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_SIGMASK/COM/uos_sigmask.c,v 1.1 2001/01/19 14:57:20 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>


static void SigHandler(u_int32 sigCode)
{
	switch(sigCode) {
		case UOS_SIG_USR1:	
			printf(">>> SigHandler: signal=UOS_SIG_USR1 occured\n");
			break;
		case UOS_SIG_USR2:
			printf(">>> SigHandler: signal=UOS_SIG_USR2 occured\n");
			break;
		default:
			printf(">>> SigHandler: signal=%d occured\n",sigCode);
	}
}

int main(argc,argv)
int  argc;
char **argv;
{
	int32 error;
	u_int32 signal;

	if (argc<2 || strcmp(argv[1],"-?")==0)  {
		printf("uos_sighand <signal>\n");
		return(1);
	}

	signal  = atoi(argv[1]);

	if (signal==UOS_SIG_USR1 || signal==UOS_SIG_USR2) {
		printf("*** illegal signal=%d: internally used\n",signal);
		return(1);
	}

	/* init signal handling */
	if (error = UOS_SigInit(SigHandler)) {	
		printf("*** can't UOS_SigInit: error=0x%04x\n",error);
		return(1);
	}

	/* install signals */
	if ((error = UOS_SigInstall(UOS_SIG_USR1)) ||
		(error = UOS_SigInstall(UOS_SIG_USR2)) ||
		(error = UOS_SigInstall(signal)) ) {
		printf("*** can't UOS_SigInstall: error=0x%04x\n",error);
		goto abort;
	}

	printf("installed signal=UOS_SIG_USR1=%d\n",UOS_SIG_USR1);
	printf("installed signal=UOS_SIG_USR2=%d\n",UOS_SIG_USR2);
	printf("installed signal=%d\n",signal);

	/* do anything .. */

	while(UOS_KeyPressed() == -1) {
		printf("\nwait for signals (sigs masked)..\n");
		UOS_SigMask();
		UOS_Delay(5000);
		printf("\nwait for signals (sigs unmasked)..\n");
		UOS_SigUnMask();
		UOS_Delay(5000);
	}

	abort:
	UOS_SigExit();						/* terminate signal handling */

	return(0);
}




/****************************************************************************
 ************                                                    ************
 ************              U O S _ S I G H A N D                 ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:21 $
 *    $Revision: 1.3 $
 *
 *  Description: Test USR_OSS signal functions
 *               Dispatch signals via signal handler
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_sighand.c,v $
 * Revision 1.3  1999/04/20 15:45:21  kp
 * cosmetics
 *
 * Revision 1.2  1999/02/15 15:01:23  see
 * don't check for signal range
 * bug fixed: don't mask signals in main()
 *
 * Revision 1.1  1998/08/07 14:15:08  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_SIGHAND/COM/uos_sighand.c,v 1.3 1999/04/20 15:45:21 kp Exp $";

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
	printf("\nwait for signals ..\n");

	while(UOS_KeyPressed() == -1)
		UOS_Delay(10);

	abort:
	UOS_SigExit();						/* terminate signal handling */

	return(0);
}




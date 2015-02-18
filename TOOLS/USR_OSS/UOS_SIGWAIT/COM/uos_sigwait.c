/****************************************************************************
 ************                                                    ************
 ************              U O S _ S I G W A I T                 ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:23 $
 *    $Revision: 1.4 $
 *
 *  Description: Test USR_OSS signal functions
 *               Wait for signals via UOS_SigWait()
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_sigwait.c,v $
 * Revision 1.4  1999/04/20 15:45:23  kp
 * cosmetics
 *
 * Revision 1.3  1999/02/15 15:05:20  see
 * include usr_err.h
 *
 * Revision 1.2  1999/02/15 15:01:25  see
 * don't check for signal range
 * print signal on ERR_UOS_ABORTED
 *
 * Revision 1.1  1998/08/07 14:15:11  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_SIGWAIT/COM/uos_sigwait.c,v 1.4 1999/04/20 15:45:23 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_err.h>

int main(argc,argv)
int  argc;
char **argv;
{
	int32 error;
	u_int32 signal, timeout, occured;

	if (argc<3 || strcmp(argv[1],"-?")==0)  {
		printf("uos_sigwait <signal> <timeout>\n");
		return(1);
	}

	signal  = atoi(argv[1]);
	timeout = atoi(argv[2]);

	if (signal==UOS_SIG_USR1 || signal==UOS_SIG_USR2) {
		printf("*** illegal signal=%d: internally used\n",signal);
		return(1);
	}

	/* init signal handling */
	if (error = UOS_SigInit(NULL)) {	
		printf("*** can't UOS_SigInit: error=0x%04x\n",error);
		return(1);
	}

	/* mask all signals */
	if ((error = UOS_SigMask())) {					
		printf("*** can't UOS_SigMask: error=0x%04x\n",error);
		goto abort;
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

	/* wait for signal .. */
	while(!error) {
		printf("\nwait for signal (timeout=%d) ..\n", timeout);

		if (error = UOS_SigWait(timeout, &occured)) {	/* unmask, wait .. */
			switch (error) {
			case ERR_UOS_TIMEOUT:
				printf("*** timeout\n");	
				break;
			case ERR_UOS_ABORTED:
				printf("*** aborted (signal=%d)\n",occured);
				break;
			default:
				printf("*** error=0x%04x\n",error);
			}
		}
		else
			printf(">>> signal=%d occured\n",occured);
	}

	abort:
	UOS_SigExit();						/* terminate signal handling */

	return(0);
}




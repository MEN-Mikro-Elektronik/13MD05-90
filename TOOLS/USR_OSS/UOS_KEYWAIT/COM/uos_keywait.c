/****************************************************************************
 ************                                                    ************
 ************              U O S _ K E Y W A I T                 ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:12 $
 *    $Revision: 1.2 $
 *
 *  Description: Test USR_OSS function UOS_KeyWait
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_keywait.c,v $
 * Revision 1.2  1999/04/20 15:45:12  kp
 * cosmetics
 *
 * Revision 1.1  1998/08/07 14:14:58  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_KEYWAIT/COM/uos_keywait.c,v 1.2 1999/04/20 15:45:12 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>


int main(argc,argv)
int  argc;
char **argv;
{
	int32 keyCode, done=0;

	printf("Press 'x' for exit\n");

	while(!done) {
		switch((keyCode = UOS_KeyWait())) {
		case -1:
			printf("*** read error\n");
			break;
		case 'x':
			done=1;
			break;
		default:
			printf("\ncode=0x%02x\n",keyCode);
	
		}
	}

	return(0);
}


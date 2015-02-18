/****************************************************************************
 ************                                                    ************
 ************              U O S _ K E Y P R E S S               ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: see
 *        $Date: 1999/04/20 15:45:09 $
 *    $Revision: 1.3 $
 *
 *  Description: Test USR_OSS function UOS_KeyPressed
 *
 *     Required: usr_oss.l
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: uos_keypress.c,v $
 * Revision 1.3  1999/04/20 15:45:09  kp
 * cosmetics
 *
 * Revision 1.2  1998/08/18 14:02:50  Schmidt
 * modified
 *
 * Revision 1.1  1998/08/07 14:14:55  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/USR_OSS/UOS_KEYPRESS/COM/uos_keypress.c,v 1.3 1999/04/20 15:45:09 kp Exp $";

#include <stdio.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>


int main(argc,argv)
int  argc;
char **argv;
{
	int32 keyCode, noKeyCnt=0, done=0;

	printf("Press 'x' for exit\n");

	while(!done) {
		switch((keyCode = UOS_KeyPressed())) {
		case -1:
			if ((noKeyCnt++)==0)
				printf("no key pressed\n");
			if (noKeyCnt==10000){
				printf("10000 X\n");
				noKeyCnt=1;
			}
			break;
		case 'x':
			done=1;
			break;
		default:
			noKeyCnt=0;
			printf("\nkey pressed (code=0x%02x)\n",keyCode);
	
		}
	}

	return(0);
}


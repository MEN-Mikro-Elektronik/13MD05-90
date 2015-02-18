/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_spcycle.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/07/09 11:04:50 $
 *    $Revision: 1.3 $
 *
 *  	 \brief  Example program for special cycles (RMW, AONLY)
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_spcycle.c,v $
 * Revision 1.3  2009/07/09 11:04:50  rt
 * R: 1.) Missing parameter for swap mode.
 * M: 1.) Optional swap parameter added.
 *
 * Revision 1.2  2009/06/03 19:30:58  rt
 * R: 1.) No support for VME addresses > 0xFFFFFFFF.
 * M: 1.) Use strtoull for VME address.
 *
 * Revision 1.1  2003/12/15 15:02:41  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: vme4l_spcycle.c,v 1.3 2009/07/09 11:04:50 rt Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define CHK(expression) \
 if( !(expression)) {\
	 printf("\n*** Error during: %s\nfile %s\nline %d\n", \
      #expression,__FILE__,__LINE__);\
      printf("%s\n",strerror(errno));\
     goto ABORT;\
 }

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

static void usage(void)
{
	printf("Usage: vme4l_spcycle <spc-num> <rmw|aonly> <vmeaddr> [<accWidth>] "
		   "[<swpmode>]\n");
	exit(1);
}


/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd, rv;
	VME4L_SPACE spc;
	vmeaddr_t vmeAddr;
	int accWidth=1;
	int doAonly;
	int swapMode = 0;

	if( argc < 4 )
		usage();

	spc 	= strtol( argv[1], NULL, 10 );
	doAonly	= strcmp( argv[2], "rmw" );
	vmeAddr = strtoull( argv[3], NULL, 16 );
	if( argc > 4 )
		accWidth    = strtoul( argv[4], NULL, 16 );
	if( argc > 5 )
		swapMode = strtoul( argv[5], NULL, 16 );
		
	printf("Open space %s\n", VME4L_SpaceName(spc));

	CHK( (fd = VME4L_Open( spc )) >= 0 );

	CHK( VME4L_SwapModeSet( fd, swapMode ) == 0 );
	
	if( doAonly ){
		printf("Address only cycle\n");
		CHK( (rv = VME4L_AOnlyCycle( fd, vmeAddr )) == 0 );
	}
	else {
		uint32_t readVal;

		printf("RMW cycle\n");
		CHK( (rv = VME4L_RmwCycle( fd, vmeAddr, accWidth, 0x80, &readVal ))
			 == 0 );

		printf("org. Readval was 0x%08x\n", readVal );
	}

	return 0;

 ABORT:
	return 1;
}


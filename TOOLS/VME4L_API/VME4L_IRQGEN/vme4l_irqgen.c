/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_irqgen.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/12/15 15:02:43 $
 *    $Revision: 1.1 $
 *
 *  	 \brief  Example program for VME4L_IrqGenerate() and friends
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_irqgen.c,v $
 * Revision 1.1  2003/12/15 15:02:43  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: vme4l_irqgen.c,v 1.1 2003/12/15 15:02:43 kp Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
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
	printf("Usage: vme4l_irqgen <level> <vector> [<numirqs>]\n");
	exit(1);
}


/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd, rv, vector, level, i, numIrqs=1, interrupterId;
	int timeout;

	if( argc < 3 )
		usage();

	level  = strtol( argv[1], NULL, 10 );
	vector = strtol( argv[2], NULL, 10 );

	if( argc > 3 )
		numIrqs = strtoul( argv[3], NULL, 10 );

	CHK( (fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 );

	printf( "Trigger %d irqs (level %d vector %d)\n", numIrqs, level, vector );

	for( i=0; i<numIrqs; i++ ){

		/* trigger IRQ */
		CHK( ( interrupterId = VME4L_IrqGenerate( fd, level, vector )) >= 0 );

		printf(".");
		fflush(stdout);
		/* wait for ack */

		timeout = 100;

		do {
			rv = VME4L_IrqGenAcked( fd, interrupterId );
			if( rv == 0 ){
				struct timespec request, remain;

				request.tv_sec = 0;
				request.tv_nsec = 10*1E6; /* 10ms */
				nanosleep( &request, &remain );
			}

		} while( rv == 0 && --timeout > 0 );


		if( rv == 0 ){
			printf("Timeout! no IACK!\n");
			/* if not acknowledged, clear irq */
			CHK( VME4L_IrqGenClear( fd, interrupterId ) == 0 );
			goto ABORT;
		}
		CHK( rv >= 0 );
	}
	printf("IRQS SENT OK\n");


	return 0;

 ABORT:
	return 1;
}

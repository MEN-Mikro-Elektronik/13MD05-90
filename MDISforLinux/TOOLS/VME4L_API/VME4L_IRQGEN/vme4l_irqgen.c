/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_irqgen.c
 *
 *      \author  klaus.popp@men.de
 *
 *  	 \brief  Example program for VME4L_IrqGenerate() and friends
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

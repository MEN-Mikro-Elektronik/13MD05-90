/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_irqtest.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/09/24 11:10:10 $
 *    $Revision: 1.4 $
 *
 *  	 \brief  Example program for VME4L_Read/Write
 *
 *	This program shows how to access VME space and VME interrupts. In order
 *  to run, it requires a A201 or A203N VME M-Module carrier board
 *  with an M99 M-module.
 *
 *  The A201/A203N + M99 is programmed to generate fixed number of interrupts
 *  per second. The M99Int() routine will measure the interrupt latency of
 *  the M99Int() handler, by reading the M99's timer.
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 *
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

static const char RCSid[]="$Id: vme4l_irqtest.c,v 1.4 2009/09/24 11:10:10 CRuff Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
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

#define SwapD16(val) (val)

#define MWRITE_D16(ma,offs,val) \
   *(volatile uint16_t*)((uint8_t*)(ma)+(offs)) = SwapD16(val)
#define MREAD_D16(ma,offs) \
   (SwapD16(*(volatile uint16_t*)((uint8_t*)(ma)+(offs))))
/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

int G_fd;
int G_abort;
/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

static void usage(void)
{
	printf("Usage: vme4l_irqtest <level 1-7> <vector 1-255>\n");
	exit(1);
}

static void SigIntHandler( int sigNum )
{
	G_abort = 1;
}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{

	int vector, level;

	if( argc < 3 )
		usage();

	level   = strtoul( argv[1], NULL, 10 );
	vector  = strtoul( argv[2], NULL, 10 );

	printf("vme4l_irqtest: waiting for Level %d vector %d\n", level, vector );
	/* open VME space */
	CHK( (G_fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 );

	signal( SIGUSR1, SigIntHandler );
	CHK( VME4L_SigInstall( G_fd, vector, level, SIGUSR1, VME4L_IRQ_NOFLAGS ) == 0 );

	/* enable VME interrupt level */
	CHK( VME4L_IrqEnable( G_fd, level ) == 0);

	while(!G_abort) {
		sleep(1);
		fprintf(stdout, ".");
		fflush(stdout);
	}

	if ( G_abort )
		printf("\nreceived VME IRQ level %d, OK!\n", level );
	return 0;
ABORT:
	return 1;
}

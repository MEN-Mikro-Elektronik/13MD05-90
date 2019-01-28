/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_pldz002_locmon.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/06/03 19:19:33 $
 *    $Revision: 1.2 $
 *
 *  	 \brief  Example program for location monitor handling
 *
 * Programs one of the two location monitors of PLDZ002 with parameters
 * from command line.
 *
 * Waits for a LM hit and displays the LM register values.
 * Repeated until program is killed.
 *
 * Example: Monitor A24 read or write access to 0xE00000:
 *
 *   # vme4l_pldz002_locmon 0 36 e00000
 *
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN mikro elektronik GmbH, Nuremberg, Germany
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
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#define LOCMON_SIG	SIGUSR1

#define CHK(expression) \
 if( !(expression)) {\
	 printf("\n*** Error during: %s\nfile %s\nline %d\n", \
      #expression,__FILE__,__LINE__);\
      printf("%s\n",strerror(errno));\
     goto ABORT;\
 }


static void usage(void)
{
	printf("Usage: vme4l_pldz002_locmon <mon-num> <ctrl> <addr>\n");
	exit(1);
}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int n=0, monNum, fd;
	sigset_t sigMask;
	uint32_t ctrl, addr;
	int ctrlIdx, addrIdx;
	int abort = 0;

	if( argc < 4 )
		usage();

	monNum = strtol( argv[1], NULL, 10 );
	ctrl   = strtol( argv[2], NULL, 16 );
	addr   = strtol( argv[3], NULL, 16 );

	ctrlIdx = monNum ? 1 : 0;
	addrIdx = monNum ? 3 : 2;
	
	CHK( (fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 );

	/* setup monitor regs */
	CHK( VME4L_LocMonRegWrite( fd, ctrlIdx, ctrl ) == 0 );
	CHK( VME4L_LocMonRegWrite( fd, addrIdx, addr ) == 0 );

	/*
	 * Instruct VME4L to send signal 33
	 * when location monitor triggers
	 */
	CHK( VME4L_SigInstall( fd,
						   VME4L_IRQVEC_LOCMON(monNum),
						   VME4L_IRQLEV_LOCMON(monNum),
						   LOCMON_SIG,
						   VME4L_IRQ_ENBL ) == 0 );


	/* mask signal, wait for signal with sigwaitinfo */
	sigaddset( &sigMask, LOCMON_SIG );
	sigaddset( &sigMask, SIGINT );
	sigprocmask( SIG_BLOCK, &sigMask, NULL );

	while( !abort ){
		
		/* Let signals come in */
		switch (sigwaitinfo( &sigMask, NULL )){
			case LOCMON_SIG:

				CHK( VME4L_LocMonRegRead( fd, ctrlIdx, &ctrl ) == 0 );
				CHK( VME4L_LocMonRegRead( fd, addrIdx, &addr ) == 0 );
				printf("HIT! ctrl=0x%02x addr=0x%08x\n", ctrl, addr );

				n++;
				break;
			case SIGINT:
				abort = 1;
				break;
			default:
				/* ignore */
				break;
		}
	}
	CHK( VME4L_IrqDisable( fd, VME4L_IRQLEV_LOCMON(monNum) ) == 0) ;
	CHK( VME4L_SigUnInstall( fd, VME4L_IRQVEC_LOCMON(monNum) ) == 0 );

	return 0;					/* not reached */

 ABORT:
	return 1;
}


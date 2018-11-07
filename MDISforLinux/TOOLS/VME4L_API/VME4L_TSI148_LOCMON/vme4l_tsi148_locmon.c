/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_tsi148_locmon.c
 *
 *      \author  ralf.truebenbach@men.de
 *        $Date: 2009/06/03 19:43:12 $
 *    $Revision: 1.1 $
 *
 *  	 \brief  Example program for location monitor handling
 *
 * Programs the location monitor of the TSI148 with parameters
 * from command line.
 *
 * Waits for a LM hit and displays the LM register values.
 * Repeated until program is killed.
 *
 * Example: Monitor A24 read or write access to 0xE00000:
 *
 *   # vme4l_tsi148_locman a24 e00000
 *
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*
 *---------------------------------------------------------------------------
 * (c) Copyright 2009 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

static const char RCSid[]="$Id: vme4l_tsi148_locmon.c,v 1.1 2009/06/03 19:43:12 rt Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>
#include <MEN/men_typs.h>
#include <MEN/tsi148.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#define LOCMON_SIG	(SIGRTMIN+1)

#define CHK(expression) \
 if( !(expression)) {\
	 printf("\n*** Error during: %s\nfile %s\nline %d\n", \
      #expression,__FILE__,__LINE__);\
      printf("%s\n",strerror(errno));\
     goto ABORT;\
 }

int G_abort;


static void usage(void)
{
	printf("Usage: vme4l_tsi148_locmon <addrSpc> <addr>\n");
	printf("       (e.g. vme4l_tsi148_locman a24 e00000)\n");
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
	int n=0, fd;
	sigset_t sigMask;
	int lmIrqNo=0;
	uint32_t attr;
	uint32_t addrSpc;
	uint64_t addr;

	/* get parameters */
	if( argc != 3 || argv[1][0] != 'a' )
		usage();

	addrSpc = strtoul( &argv[1][1], NULL, 10 );
	addr    = strtoull( argv[2], NULL, 16 );

	/* convert parameters */
	switch( addrSpc ) {
		case 16: attr = TSI148_LMAT_AS_A16; break;
		case 24: attr = TSI148_LMAT_AS_A24; break;
		case 32: attr = TSI148_LMAT_AS_A32; break;
		case 64: attr = TSI148_LMAT_AS_A64; break;
		default: usage();
	}
	attr |= TSI148_LMAT_EN | TSI148_LMAT_SUPR | TSI148_LMAT_NPRIV |
			TSI148_LMAT_PGM | TSI148_LMAT_DATA;
	
	/* Remember, TSI148 supports four locations (each location is 8 bytes).
	   Address bits 3 and 4 are used to define the specific location. */
	switch( addr & TSI148_LMX_MASK ){
		case TSI148_LM0: lmIrqNo = 0; break;	
		case TSI148_LM1: lmIrqNo = 1; break;
		case TSI148_LM2: lmIrqNo = 2; break;
		case TSI148_LM3: lmIrqNo = 3; break;
	}
	addr &= TSI148_LMBA_MASK;
	
	/* open any address space */
	CHK( (fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 ); /* /dev/vme4l_a24_d16
															must exist */

	/* setup monitor regs */
	CHK( VME4L_LocMonRegWrite( fd, 0, (uint32_t)(addr>>32) ) == 0 );
	CHK( VME4L_LocMonRegWrite( fd, 1, (uint32_t)addr ) == 0 );
	CHK( VME4L_LocMonRegWrite( fd, 2, attr ) == 0 );

	/*
	 * instruct VME4L to send signal
	 * when location monitor triggers
	 */
	CHK( VME4L_SigInstall( fd,
						   VME4L_IRQVEC_LOCMON( lmIrqNo ),
						   VME4L_IRQLEV_LOCMON( lmIrqNo ),
						   LOCMON_SIG,
						   VME4L_IRQ_ENBL ) == 0 );

	signal( SIGINT, SigIntHandler );


	/* mask signal, wait for signal with sigwaitinfo */
	sigaddset( &sigMask, LOCMON_SIG );
	sigprocmask( SIG_BLOCK, &sigMask, NULL );

	while( !G_abort ){
		
		uint32_t addr_h, addr_l;
		
		/* let signals come in */
		if( sigwaitinfo( &sigMask, NULL ) == LOCMON_SIG ){

			CHK( VME4L_LocMonRegRead( fd, 0, &addr_h ) == 0 );
			CHK( VME4L_LocMonRegRead( fd, 1, &addr_l ) == 0 );
			CHK( VME4L_LocMonRegRead( fd, 2, &attr ) == 0 );
			printf( "HIT %d! addr=0x%08x_%08x lmat=0x%08x lmIrqNo=%d\n",
					++n, addr_h, addr_l, attr, lmIrqNo );
		}
	}
	
	/* clean up */
	CHK( VME4L_LocMonRegWrite( fd, 2, 0 ) == 0 ); /* disable */
	CHK( VME4L_IrqDisable( fd, VME4L_IRQLEV_LOCMON( lmIrqNo ) ) == 0) ;
	CHK( VME4L_SigUnInstall( fd, VME4L_IRQVEC_LOCMON( lmIrqNo ) ) == 0 );

	return 0;					/* not reached */

 ABORT:
	return 1;
}


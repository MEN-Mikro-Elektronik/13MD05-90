/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_mbox.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/06/03 19:18:11 $
 *    $Revision: 1.3 $
 *
 *  	 \brief  Example program for mailbox handling
 *
 * For PLDZ002 and TSI148 the A16 slave window must be exported (use
 * vme_slvwin). The vme4l_mbox program must be started first, then the
 * remote program.
 *
 * Requires a program on the remote CPU that does the following:
 *
 *  for(;;)
 *     read mailbox reg until it is read as 0x0000_0000
 *     write a value to mailbox reg
 *		(value increases with each write. first value shall be 0x0000_0001)
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

static const char RCSid[]="$Id: vme4l_mbox.c,v 1.3 2009/06/03 19:18:11 rt Exp $";

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

#define MBOX_SIG	(SIGRTMIN+1)

#define CHK(expression) \
 if( !(expression)) {\
	 printf("\n*** Error during: %s\nfile %s\nline %d\n", \
      #expression,__FILE__,__LINE__);\
      printf("%s\n",strerror(errno));\
     goto ABORT;\
 }


static void usage(void)
{
	printf("Usage: vme4l_mbox <mbox> <numevents> [v]\n");
	exit(1);
}


/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int nEvents, mbox, fd;
	uint32_t mboxValue, n=0;
	sigset_t sigMask;
	int verbose=0;

	if( argc < 3 )
		usage();

	if( argc > 3 )
		verbose++;


	mbox 	= strtol( argv[1], NULL, 10 );
	nEvents = strtoul( argv[2], NULL, 10 );
		
	CHK( (fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 );


	/*
	 * Instruct VME4L to send signal
	 * when mailbox is written from VME
	 */
	CHK( VME4L_SigInstall( fd,
						   VME4L_IRQVEC_MBOXWR(mbox),
						   VME4L_IRQLEV_MBOXWR(mbox),
						   MBOX_SIG,
						   VME4L_IRQ_ENBL ) == 0 );


	/* mask signal, wait for signal with sigwaitinfo */
	sigaddset( &sigMask, MBOX_SIG );
	sigprocmask( SIG_BLOCK, &sigMask, NULL );

	while( n < nEvents ){
		
		/* flag i'm ready */
		CHK( VME4L_MboxWrite( fd, mbox, 0x00000000 ) == 0 );

		/* Let signals come in */
		if( sigwaitinfo( &sigMask, NULL ) == MBOX_SIG ){

			CHK( VME4L_MboxRead( fd, mbox, &mboxValue ) == 0 );
			n++;

			if( verbose )
				printf("got %d\n", mboxValue );

			if( mboxValue != n ){
				printf("Bad mboxvalue 0x%08x sb 0x%08x\n",
					   mboxValue, n );
				goto ABORT;
			}					
		}
	}
	printf("%d mailbox writes recognized\n", n);

	return 0;

 ABORT:
	return 1;
}


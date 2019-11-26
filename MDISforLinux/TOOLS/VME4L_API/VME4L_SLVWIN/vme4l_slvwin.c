/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_slvwin.c
 *
 *      \author  klaus.popp@men.de
 *
 *  	 \brief  Example program to control slave windows
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*
 *---------------------------------------------------------------------------
 * Copyright 2004-2019, MEN Mikro Elektronik GmbH
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
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>

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
	printf("Usage: vme4l_slvwin <spc-num> <vmeaddr> <size> [<r/w>]\n");
	exit(1);
}


/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd, rv, i;
	VME4L_SPACE spc;
	vmeaddr_t vmeAddr;
	size_t size;
	int read = 0;
	int write = 0;
	void *map;

	if( argc < 4 || argc >5 )
		usage();

	/* get arguments */
	spc 	= strtol( argv[1], NULL, 10 );
	vmeAddr = strtoull( argv[2], NULL, 16 );
	size    = strtoul( argv[3], NULL, 16 );
	if( argc > 4 ) {
		read	= (argv[4][0]=='r') ? 1 : 0;
		/* note: write makes only sense for shared RAM! */
		write	= (argv[4][0]=='w') ? 1 : 0;
	}
	
	printf( "Open space %s\n", VME4L_SpaceName(spc) );

	CHK( (fd = VME4L_Open( spc )) >= 0 );

	CHK( (rv = VME4L_SlaveWindowCtrl( fd, vmeAddr, size )) == 0 );
	
	/* map slave window into application space
	   note: the vmeAddr argument of VME4L_Map() is the offset within the shared RAM here! */
	CHK( VME4L_Map( fd,	0,  size, &map) == 0 );

	if( write ) {
		/* write a string to RAM */
		sprintf( map, "\x12\x34\x56\x78 VME4L Slave Window at 0x%llx (spc=%d)",	 vmeAddr, spc );
	}
	
	printf("spc %d opened. Now send from other VME card then press ENTER:\n");
	UOS_KeyWait();

	if( read ) {
		/* dump RAM */
		for( i=0; i<0x100; i++ ) {
			if( !(i%0x10) )
				printf( "\n" );
			printf( "%02x ", ((uint8_t*)map)[i] );
		}
		printf( "\n" );
	}
	
	CHK( VME4L_Close( fd ) >= 0 );

	return 0;

 ABORT:
	return 1;
}


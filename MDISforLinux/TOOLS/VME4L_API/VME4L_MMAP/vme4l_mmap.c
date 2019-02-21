/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_mmap.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/06/03 19:18:59 $
 *    $Revision: 1.2 $
 *
 *  	 \brief  Example program for VME4L_Map
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
static void MemDump(
    char *buf,
    uint32_t n,
    uint32_t fmt
	);


static void usage(void)
{
	printf("Usage: vme4l_mmap <spc-num> <vmeaddr> <size> <r/w> [<swpmode>] "
		   " [<berr>]\n");
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
	size_t size;
	int doRead;
	void *map;
	int swapMode = 0;

	if( argc < 5 )
		usage();

	spc 	= strtol( argv[1], NULL, 10 );
	vmeAddr = strtoull( argv[2], NULL, 16 );
	size    = strtoul( argv[3], NULL, 16 );
	doRead = ( *argv[4] == 'w' ) ? 0:1;
	if( argc > 5 )
		swapMode = strtoul( argv[5], NULL, 16 );

	printf("Open space %s\n", VME4L_SpaceName(spc));

	CHK( (fd = VME4L_Open( spc )) >= 0 );

	CHK( VME4L_SwapModeSet( fd, swapMode ) == 0 );

	if( argc > 6 && (!strcmp( argv[6], "berr")) ){
		printf("Install bus error signal\n");
		/* install SIGBUS for *any* bus error (kills program) */
		CHK( VME4L_SigInstall( fd, VME4L_IRQVEC_BUSERR,
							   VME4L_IRQLEV_BUSERR, SIGBUS,
							   VME4L_IRQ_NOFLAGS) == 0 );
	}


	CHK( (rv = VME4L_Map( fd, vmeAddr, size, &map )) == 0 );

	printf("vaddr=%p rv=%d\n", map, rv );

	if( doRead ){
		MemDump( (char *)map, size, 1 );
	}
	else {
		/* fill VME with some dummy data */
		uint8_t *p = (uint8_t *)map;
		size_t i;

		for( i=0; i<size; i++ )
			*p++ = i;
	}
	CHK( (rv = VME4L_UnMap( fd, map, size )) == 0 );

	return 0;

 ABORT:
	return 1;
}

static void MemDump(
    char *buf,
    uint32_t n,
    uint32_t fmt
)
{
    char *k, *k0, *kmax = buf+n;
    int32_t i;


    for (k=k0=buf; k0<kmax; k0+=16)
    {
        printf("%04x: ", (unsigned int)(k-buf));

        switch(fmt)                                        /* dump hex: */
        {
           case 4 : for (k=k0,i=0; i<16; i+=4, k+=4)       /* long aligned */
                       if (k<kmax)  printf("%08lx ",*(uint32_t*)k);
                       else         printf("         ");
                    break;
           case 2 : for (k=k0,i=0; i<16; i+=2, k+=2)       /* word aligned */
                       if (k<kmax)  printf("%04x ",*(uint16_t*)k & 0xffff);
                       else         printf("     ");
                    break;
           default: for (k=k0,i=0; i<16; i++, k++)         /* byte aligned */
                       if (k<kmax)  printf("%02x ",*k & 0xff);
                       else         printf("   ");
        }

        for (k=k0,i=0; i<16 && k<kmax; i++, k++)           /* dump ascii */
            if ( *(uint8_t*)k>=32 && *(uint8_t*)k<=127 )
               printf("%c", *k);
            else
               printf(".");

        printf("\n");
    }
}

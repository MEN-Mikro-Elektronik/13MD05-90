/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_rwex.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/06/03 19:19:53 $
 *    $Revision: 1.3 $
 *
 *  	 \brief  Example program for VME4L_Read/Write
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_rwex.c,v $
 * Revision 1.3  2009/06/03 19:19:53  rt
 * R: 1.) No support for VME addresses > 0xFFFFFFFF.
 * M: 1.) Use strtoull for VME address.
 *
 * Revision 1.2  2009/04/30 21:18:05  rt
 * R: 1.) Print better error message.
 * M: 1.) Added comment.
 *
 * Revision 1.1  2003/12/15 15:02:28  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

static const char RCSid[]="$Id: vme4l_rwex.c,v 1.3 2009/06/03 19:19:53 rt Exp $";

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
    uint8_t *buf,
    uint32_t n,
    uint32_t fmt
	);


static void usage(void)
{
	printf("Usage: vme4l_rwex <spc-num> <vmeaddr> <size> <accWidth> <r/w> "
		   "[<swpmode>]\n");
	exit(1);
}

static void SigHandler( int sigNum )
{

}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd, rv;
	uint8_t *buf;
	VME4L_SPACE spc;
	vmeaddr_t vmeAddr;
	int accWidth;
	size_t size;
	int doRead;
	int swapMode = 0;
	int opt_mod = 0;

	if( argc < 6 )
		usage();


	signal( SIGUSR1, SigHandler ); /* catch sig 10 (typical) */

	spc 	= strtol( 	argv[1], NULL, 10 );
	vmeAddr = strtoull( argv[2], NULL, 0 );
	size    = strtoul( 	argv[3], NULL, 0 );
	accWidth= strtoul( 	argv[4], NULL, 0 );
	doRead = ( *argv[5] == 'w' ) ? 0:1;
	/* opt_mod = strtol(argv[6], NULL, 0);*/

	if( argc > 6 )
		swapMode = strtoul( argv[6], NULL, 16 );

	CHK( (buf = malloc( size )) != NULL);

	printf("Open space %s, user buffer @ %p \n", VME4L_SpaceName(spc), buf );

	CHK( (fd = VME4L_Open( spc )) >= 0 /*node /dev/vme4l_<spc> must exist*/ );

	if( opt_mod > 0) {
		printf("change AM to 0x%x\n", opt_mod & 0xff );
		CHK( VME4L_AddrModifierSet( fd, (char)(opt_mod & 0xff))==0 );
	}

	CHK( VME4L_SwapModeSet( fd, swapMode ) == 0 );

	if( doRead ){
		CHK( (rv = VME4L_Read( fd, vmeAddr, accWidth, size, buf, VME4L_RW_NOFLAGS )) >=0);

		MemDump( buf, rv, 1 );
	}
	else {
		uint8_t *p = buf;
		int i;

		for( i=0; i<size; i++ )
			*p++ = i;

		CHK( (rv = VME4L_Write( fd, vmeAddr, accWidth, size, buf, VME4L_RW_NOFLAGS )) >=0);
	}

	return 0;

 ABORT:
	return 1;
}

static void MemDump(
    uint8_t *buf,
    uint32_t n,
    uint32_t fmt
)
{
    uint8_t *k, *k0, *kmax = buf+n;
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

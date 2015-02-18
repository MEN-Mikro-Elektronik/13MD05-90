/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_rwspeed.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/06/03 19:30:07 $
 *    $Revision: 1.2 $
 *
 *  	 \brief  Speed measurement for VME4L_Read/Write
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_rwspeed.c,v $
 * Revision 1.2  2009/06/03 19:30:07  rt
 * R: 1.) No support for VME addresses > 0xFFFFFFFF.
 *    2.) Cosmetics.
 * M: 1.) Use strtoull for VME address.
 *    2.a) Avoid compiler warnings.
 *    2.b) Comment added.
 *
 * Revision 1.1  2003/12/15 15:02:30  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: vme4l_rwspeed.c,v 1.2 2009/06/03 19:30:07 rt Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>
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
    int n,
    int fmt
	);


static void usage(void)
{
	printf("Usage: vme4l_rwspeed <spc-num> <vmeaddr> <size-hex> <accWidth> "
		   "<loops> <r/w>\n");
	exit(1);
}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd, rv=0;
	char *buf;
	uint64_t deltaTime, startTime, endTime, totBytes;
	VME4L_SPACE spc;
	vmeaddr_t vmeAddr;
	int accWidth, loops, run;
	size_t size;
	int doRead;
	struct timeval startTv, endTv;
	struct timezone tz;

	if( argc != 7 )
		usage();

	spc 	= strtol( argv[1], NULL, 10 );
	vmeAddr = strtoull( argv[2], NULL, 16 );
	size    = strtoul( argv[3], NULL, 16 );
	accWidth= strtoul( argv[4], NULL, 16 );	
	loops   = strtoul( argv[5], NULL, 10 );	
	doRead = ( *argv[6] == 'w' ) ? 0:1;

	CHK( (buf = malloc( size )) != NULL);

	printf("Open space %s\n", VME4L_SpaceName(spc));

	CHK( (fd = VME4L_Open( spc )) >= 0 /*node /dev/vme4l_<spc> must exist*/ );

	
	if( !doRead ){
		/* fill buffer */
		char *p = buf;
		int i;

		for( i=0; i<size; i++ )
			*p++ = i;
	}

	gettimeofday( &startTv, &tz );

	for( run=0; run<loops; run++ ){
		if( doRead ){
			CHK( (rv = VME4L_Read( fd, vmeAddr, accWidth, size, buf,
								   VME4L_RW_NOFLAGS )) >=0);
		}
		else {
			CHK( (rv = VME4L_Write( fd, vmeAddr, accWidth, size, buf,
									VME4L_RW_NOFLAGS )) >=0);
		}
	}

	gettimeofday( &endTv, &tz );

	startTime = (startTv.tv_sec * 1000000LL) + startTv.tv_usec;
	endTime = (endTv.tv_sec * 1000000LL) + endTv.tv_usec;
	deltaTime = endTime - startTime;


	if( doRead ){
		printf("Last buffer read:\n");
		MemDump( buf, rv > 0x100 ? 0x100 : rv, accWidth );
	}

	totBytes = (loops * size );
	printf("Total bytes : %llx\n", totBytes );
	printf("Elapsed time: %lld usec\n", deltaTime );
	printf("Per transfer: %2.3g usec\n", (double)deltaTime /
		   (totBytes/accWidth) );
	printf("Speed       : %2.3g MB/s\n", ((double)totBytes/(1024*1024)) /
		   ((double)deltaTime/1E6) );


	return 0;

 ABORT:
	return 1;
}

static void MemDump(
    char *buf,
    int n,
    int fmt
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

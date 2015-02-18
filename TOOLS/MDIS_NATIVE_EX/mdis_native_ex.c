/*
 * MDIS linux native example program
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>

char *device = "m43_1";

int main( int argc, char *argv[] )
{
	int32 fd;

	/* open device */
	fd = M_open( device );

	if( fd < 0 ){
		printf("Can't open %s: %s\n", device, M_errstring(errno));
		exit(1);
	}

	/* write 0xff to the device */
	if( M_write( fd, 0xff )){
		printf("Can't write to %s: %s\n", device, M_errstring(errno));
		exit(1);
	}
	sleep(1);

	M_close(fd);
	return 0;
}

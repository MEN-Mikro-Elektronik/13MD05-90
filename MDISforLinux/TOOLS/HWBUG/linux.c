/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: linux.c
 *      Project: HwBug
 *
 *       Author: ub
 *        $Date: 2010/09/24 16:51:56 $
 *    $Revision: 1.6 $
 *
 *  Description: Linux specific functions
 *               Bus errors are not catched and cause program termination.
 *
 *
 *     Required: -
 *     Switches:
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *  void os_usage(argc,argv)
 *  void os_init()
 *  void os_exit()
 *  int  os_get_char()
 *  void os_setterm()
 *  void os_restterm()
 *  u_int32 os_access_address( physadr, size, read, value, be_flag )
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
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

static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/linux.c,v 1.6 2010/09/24 16:51:56 rt Exp $";

#include "hwbug.h"
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <asm/errno.h>
void os_setterm(), os_restterm(), os_exit();
static struct termios MyOpts, OrgOpts;
static int Memdev;
/*----------------------------------------------------------------------
 * os_usage - print OS specific usage, scan parameters
 */
void os_usage(argc,argv)
int argc;
char **argv;
{
    /* include ident string, suppress warning */
    char a = a = RCSid[0];

	if (argc==2 && !strcmp(argv[1],"-?")) {
		printf("Syntax: hwbug or hwbug [command]\n");
		printf("Function: Hardware Debugger for Linux\n");
		printf("Commands:\n");
		printf("    h    list of available commands\"\n");
		printf("\n");
		exit(1);
	}
}

/*----------------------------------------------------------------------
 * os_init - initialize OS specific
 */
void os_init()
{
	extern int assemble(), disassemble(), execute_program();
	void catch_sigint(), catch_sigbus();

	signal( SIGINT, catch_sigint );
	signal( SIGBUS, catch_sigbus );
	signal( SIGSEGV, catch_sigbus );
	os_setterm();					/* setup terminal */

	if( (Memdev = open( "/dev/mem", O_RDWR )) < 0 ){
		perror("can't open /dev/mem");
		os_exit();
	}
}

/*----------------------------------------------------------------------
 * os_exit - OS-specific cleanup
 */
void os_exit(void)
{
	close( Memdev );
	os_restterm();
}

/*----------------------------------------------------------------------
 * os_get_char - wait for input char (unbuffered)
 * returns: char read or -1 if error
 */
int os_get_char(void)
{
	char c;

	if( read( 0, &c, 1 ) == -1 )
		return -1;
	return c;
}

/*----------------------------------------------------------------------
 * catch bus error signal
 */
void catch_sigbus()
{
	printf(" *** bus error\n");
	signal( SIGSEGV, SIG_IGN );
}

void catch_sigint()
{
	printf(" *** sigint\n");
	FLUSH;
	os_exit();
	exit(1);
}

/*----------------------------------------------------------------------
 * os_setterm - setup terminal for RAW input
 */
void os_setterm()
{
    if(tcgetattr( 0 , &OrgOpts ) !=0)     /* save old status */
	{
        perror("tcgetattr( 0 )");
		exit(1);
	}
    MyOpts = OrgOpts;
    MyOpts.c_lflag &= ~(ECHO | ICANON);

    if(tcsetattr(0, TCSAFLUSH, &MyOpts) !=0)
	{
        perror("tcsetattr");
		exit(1);
	}
}

/*----------------------------------------------------------------------
 * os_restterm - restore original TTY settings
 */
void os_restterm()
{
    if(tcsetattr(0, TCSAFLUSH, &OrgOpts) !=0)
	{
        perror("tcsetattr");
		exit(1);
	}
}


/*----------------------------------------------------------------------
 * os_access_address - read or write memory
 * returns: nothing on writes
 *			or read value
 *			be_flag is set if buserror occurred (not yet implemented)
 */
u_int32 os_access_address( physadr, size, read, value, be_flag )
u_int32 physadr;
int size, read;
u_int32 value;
int *be_flag;
{
	void *map_adr, *adr;

#ifdef MAC_IO_MAPPED_EN
	if( G_iomapped ) {
		if( read ){
			switch(size){
				case 1: 	value = inb(physadr); break;
				case 2: 	value = inw(physadr); break;
				case 4: 	value = inl(physadr); break;
			}
		} else {
			switch(size){
				case 1: 	outb((unsigned char)value,      physadr); break;
				case 2: 	outw((unsigned short int)value, physadr); break;
				case 4: 	outl((unsigned int)value,       physadr); break;
			}
		}
	} else
#endif /*MAC_IO_MAPPED_EN*/
	{

//  	printf( "os_access_address: 0x%x  size:%d  read:%d\n", physadr, size, read );

	    /* mmap offset parameter must be a multiple of the page size (0x1000) */
		map_adr = mmap( NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, Memdev,
						 physadr & ~0xfff );

		if( map_adr == MAP_FAILED ){
			perror("Couldn't map physical memory");
			goto cleanup;
		}

	    adr = map_adr + (physadr & 0xfff);

		/*--------------+
		| access memory |
		+--------------*/
		if( read ){
			switch(size){
				case 1: 	value = *(unsigned char *)adr; break;
				case 2: 	value = *(unsigned short *)adr; break;
				case 4: 	value = *(unsigned long *)adr; break;
			}
		} else {
			switch(size){
				case 1: 	*(unsigned char *) adr = value; break;
				case 2: 	*(unsigned short *)adr = value; break;
				case 4: 	*(unsigned long *) adr = value; break;
			}
	 		msync( map_adr, 4, MS_SYNC );
		}

	    munmap( map_adr, 0x1000 );
	}
cleanup:
    *be_flag = 0;
    return value;
}

int32 os_init_io_mapped()
{
#ifdef MAC_IO_MAPPED_EN
	int error = 0;
	error = iopl(3);
	if(error)
		return -error;

	G_iomapped = 1;
	return 0;
#else
	G_iomapped = 0;
	return -EOPNOTSUPP;
#endif
}

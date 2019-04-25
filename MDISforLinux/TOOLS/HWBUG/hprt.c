/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: hprt.c
 *      Project: HwBug
 *
 *      $Author: ub $
 *        $Date: 2003/05/26 10:05:53 $
 *    $Revision: 1.1 $
 *
 *  Description: HP-RT specific functions
 *                      
 *                      
 *     Required: - 
 *     Switches: M68K
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  void os_usage(argc,argv)                      
 *  void os_init()
 *  void os_exit()                      
 *  int  os_get_char()                   
 *  void os_setterm()                   
 *  void os_restterm()                  
 *  u_int32 os_permit_access( adr, size, name )
 *          os_protect_access( virt_adr, size, name )
 *  u_int32 os_access_address( physadr, size, read, value, be_flag )
 *  
 *	static  u_int32 _access_vmebus( physadr, size, read, value, be_flag )
 *	static  u_int32 _access_shmem( physadr, size, read, value, be_flag )
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1995-2019, MEN Mikro Elektronik GmbH
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
 
static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/hprt.c,v 1.1 2003/05/26 10:05:53 ub Exp $";

#include "hwbug.h"
#include <ctype.h>
#include <ioctl.h>
#include <signal.h>
#include <file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shmmap.h>
#include <sys/sysshmem.h>
#include <machine/pa_cache.h>

void os_setterm(), os_restterm();
u_int32 os_permit_access(),_access_vmebus(),_access_shmem();

static struct old_sgttyb MyOpts, OrgOpts;
static int id, space=0, debug;
static char *devname=0, *vmeaccess=0;

/*----------------------------------------------------------------------
 * os_usage - print OS specific usage, scan parameters
 */
void os_usage(argc,argv)
int argc;
char **argv;
{
	if (argc<2 || !strcmp(argv[2],"-?")) {
		printf("Syntax: hwbug <smsf> or <vmeaccess>\n");
		printf("Function: Hardware Debugger for HP-RT\n");
		printf("          Accessing shared memory or VMEbus directly\n");
		printf("Options:\n");
		printf("    <smsf>    	shared memory special file\n");
		printf("    <vmeaccess> VMEbus access (A32/A24/A16)\n");
		printf("\n");
		exit(1);
	}

	if (toupper(*argv[1]) == 'A') {
		vmeaccess = argv[1];

		switch (atoi(argv[1]+1)) {
			case 32: space = A32+DATA_ACCESS; break;
			case 24: space = A24+DATA_ACCESS; break;
			case 16: space = A16+DATA_ACCESS; break;
			default: printf("*** illegal vmeaccess\n"); exit(1);
		}
	}
	else	
		devname = argv[1];

	debug = (argc==3 && strcmp(argv[2],"-d")==0 ? 1 : 0);
}

/*----------------------------------------------------------------------
 * os_init - initialize OS specific
 */
void os_init(argc,argv)
int argc;
char **argv;
{
	extern int assemble(), disassemble(), permit_access(),execute_program();
	void catch_sigint(), catch_sigbus();

	if (devname) {
		printf("ACCESSING: Shared memory (%s)\n\n",devname);

		if ((id = open(devname,O_RDWR,0)) < 0) {
			printf("*** can't open '%s'\n",devname);
			exit(1);
		}
	}
	else
		printf("ACCESSING: VMEbus (%s)\n\n",vmeaccess);

	if (debug)
		printf("** DEBUG mode **\n\n");

	signal( SIGINT, catch_sigint );
	signal( _SIGBUS, catch_sigbus );
	signal( SIGSEGV, catch_sigbus );

	os_setterm();					/* setup terminal */
}

/*----------------------------------------------------------------------
 * os_exit - OS-specific cleanup
 */
void os_exit()
{
	int i;

	os_restterm();
	close(id);
}

/*----------------------------------------------------------------------
 * os_get_char - wait for input char (unbuffered)
 * returns: char read or -1 if error
 */
int os_get_char()
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
}

/*----------------------------------------------------------------------
 * catch keyboard interrupt signal
 */
void catch_sigint()
{
	printf(" ^C");
	FLUSH;
}

/*----------------------------------------------------------------------
 * os_setterm - setup terminal for RAW input
 */
void os_setterm()
{
    if(ioctl(0, OTIOCGETP, &OrgOpts) !=0) {   /* get term params */
        perror("ioctl(TIOCGETP)");
		exit(1);
	}

    MyOpts = OrgOpts;						  /* modify term params */

    MyOpts.sg_flags &= ~(/*CRMOD|*/ECHO|RAW|TANDEM);
    MyOpts.sg_flags |= CBREAK|CRMOD;
    MyOpts.sg_erase = 0;
    MyOpts.sg_kill  = 0;
/*	MyOpts.t_intrc  = -1;		/* interrrupt char (default "^?")	*/
/*	MyOpts.t_quitc  = -1;		/* quit char (default "^\")			*/
	MyOpts.t_startc = -1;		/* start output (default "^Q")		*/
	MyOpts.t_stopc  = -1;		/* stop output (default "^S")		*/
	MyOpts.t_eofc   = -1;		/* end-of-file char (default "^D")	*/
	MyOpts.t_brkc   = -1;		/* input delimiter (like nl), (default 0)	*/
	MyOpts.t_suspc  = -1;		/* stop process signal (default "^Z")		*/
	MyOpts.t_dsuspc = -1;		/* delayed stop process signal (default "^Y")	*/
	MyOpts.t_rprntc = -1;		/* reprint line (default "^R")				*/
	MyOpts.t_flushc = -1;		/* flush output (toggles) (default "^O")	*/
	MyOpts.t_werasc = -1;		/* word erase (default "^W")				*/
	MyOpts.t_lnextc = -1;		/* literal next character (default "^V")	*/

    if(ioctl(0, OTIOCSETP, &MyOpts) !=0) {      /* set term params */
        perror("ioctl(TIOCSETP)");
		exit(1);
	}
} 

/*----------------------------------------------------------------------
 * os_restterm - restore original TTY settings
 */
void os_restterm()
{
    if(ioctl(0, OTIOCSETP, &OrgOpts) !=0) 
	{
        perror("ioctl(TIOCSETP)");
		exit(1);
	}
}

/*----------------------------------------------------------------------
 * os_permit_access - allow access to a mem area
 */	
u_int32 os_permit_access( adr, size, name )
u_int32 adr;
int size;
char *name;		/* (unused) */
{
	return(0); 
}

/*----------------------------------------------------------------------
 * os_protect_access - prevent memory block from accesses
 */
os_protect_access( virt_adr, size, name )
u_int32 virt_adr;
int size;
char *name;   /* (unused) */
{
	return(0); 
}

/*----------------------------------------------------------------------
 * os_access_address - read or write memory (buserror proof)
 * returns: nothing on writes
 *			or read value
 *			be_flag is set if buserror occurred
 */
u_int32 os_access_address( physadr, size, read, value, be_flag )
u_int32 physadr;
int size, read;
u_int32 value;
int *be_flag;
{
	if (devname)
		value = _access_shmem( physadr, size, read, value, be_flag );
	else
		value = _access_vmebus( physadr, size, read, value, be_flag );

	return value;
}	

/*----------------------------------------------------------------------
 * _access_vmebus - read or write vmebus directly (buserror proof)
 * returns: nothing on writes
 *			or read value
 *			be_flag is set if buserror occurred
 */
u_int32 _access_vmebus( physadr, size, read, value, be_flag )
u_int32 physadr;
int size, read;
u_int32 value;
int *be_flag;
{
	errno = 0;

	/*--------------+
	| access VMEbus |
	+--------------*/
	if( read ){
		switch(size){
			case 1: 	value = (u_int8) VME_phys_read1(physadr,space,0); break;
			case 2: 	value = (u_int16)VME_phys_read2(physadr,space,0); break;
			case 4: 	value = (u_int32)VME_phys_read(physadr,space,0);  break;
		}
	} else {
		switch(size){
			case 1: 	VME_phys_write1(physadr,space,value); break;
			case 2: 	VME_phys_write2(physadr,space,value); break;
			case 4: 	VME_phys_write(physadr,space,value);  break;
		}
	}								

	switch (errno) {
		case 0     : *be_flag=0;                                  break;
		case EFAULT: *be_flag=1; printf(" *** bus error\n");      break;
		case ENOSYS: *be_flag=1; printf(" *** no VMEbus\n");      break;
		default    : *be_flag=1; printf(" *** error %d\n",errno); break;
	}

	return value;
}	

/*----------------------------------------------------------------------
 * _access_shmem - read or write shared memory (buserror proof)
 * returns: nothing on writes
 *			or read value
 *			be_flag is set if buserror occurred
 */
u_int32 _access_shmem( offs, access, read, value, be_flag )
u_int32 offs;	/* offset in swsm */
int access, read;
u_int32 value;
int *be_flag;
{
	u_int32 virtadr,adr,size;
	
	*be_flag = 0;		/* say: no buserror */

	/*----------------------+
	| map swsm 				|
	+----------------------*/
	size = (offs & ~0xfff) + 0x1000;		/* round up to 4k page boundry */

	if ((virtadr = (u_int32)shmmap(id,NULL,size,0,SHM_WRITE|SHM_READ)) == 0) {
		printf("*** can't map addr=%08x size=%d\n",offs,size);
		return(0);
	}

	if (debug)
		printf("\nshmmap: offs=%08x, size=%d: virtadr=%08x+%04x\n",
			   offs,size,virtadr,offs);

	adr = virtadr+offs;				/* calc access address */

	/*--------------+
	| access memory |
	+--------------*/
	if( read ){
		pdcache((char*)virtadr,size);		/* invalidate data cache before read */

		switch(access){
			case 1: 	value = *(unsigned char *)adr; break;
			case 2: 	value = *(unsigned short *)adr; break;
			case 4: 	value = *(unsigned long *)adr; break;
		}
	} else {
		switch(access){
			case 1: 	*(unsigned char *) adr = value; break;
			case 2: 	*(unsigned short *)adr = value; break;
			case 4: 	*(unsigned long *) adr = value; break;
		}

		fdcache((char*)virtadr,size);		/* flush data cache after write */
	}								

	/*----------------------+
	| unmap swsm 			|
	+----------------------*/
	if (shmunmap((void*)virtadr,0) == -1)
		printf("*** can't unmap addr=%08x\n",virtadr);

	return value;
}	


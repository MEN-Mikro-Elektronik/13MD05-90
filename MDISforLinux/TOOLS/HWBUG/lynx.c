/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: lynx.c
 *      Project: HwBug
 *
 *      $Author: ub $
 *        $Date: 2003/05/26 10:06:09 $
 *    $Revision: 1.1 $
 *
 *  Description: LynxOS specific functions
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
 *---------------------------------------------------------------------------
 * Copyright (c) 1993-2019, MEN Mikro Elektronik GmbH
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
 
static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/lynx.c,v 1.1 2003/05/26 10:06:09 ub Exp $";


#include "hwbug.h"
#include <ctype.h>
#include <ioctl.h>
#include <signal.h>
#include <smem.h>

static char 
	*h_pe = " <adr> <size>             enable access to physical memory",
	*h_as = " <adr>|T                  line-by-line assembler  (T)emp Buffer",
	*h_di = " [<adr>|T]                disassemble             (T)emp Buffer",
	*h_go = " <adr>|T                  execute program         (T)emp Buffer";

void os_setterm(), os_restterm();
u_int32 os_permit_access();
static struct old_sgttyb MyOpts, OrgOpts;
static u_int32 pe_area[10];

/*----------------------------------------------------------------------
 * os_usage - print OS specific usage, scan parameters
 */
void os_usage(argc,argv)
int argc;
char **argv;
{
	if (argc==2 && !strcmp(argv[1],"-?")) {
		printf("Syntax: hwbug \n");
		printf("Function: Hardware Debugger for LynxOS\n");
		printf("Options:\n");
		printf("    (none)\n");
		printf("\n");
		exit(1);
	}
}

/*----------------------------------------------------------------------
 * os_init - initialize OS specific
 */
void os_init()
{
	extern int assemble(), disassemble(), permit_access(),execute_program();
	void catch_sigint(), catch_sigbus();
	
	signal( SIGINT, catch_sigint );
	signal( SIGBUS, catch_sigbus );
	signal( SIGSEGV, catch_sigbus );
	os_setterm();					/* setup terminal */

	add_command( "PE", permit_access, h_pe, 0 );	
#if 0
	add_command( "AS", assemble, h_as, 0 );
	add_command( "DI", disassemble, h_di, 1 );
/*	add_command( "GO", execute_program, h_go, 0 );*/
#endif
	smem_remove("hwbug_acc");

}

/*----------------------------------------------------------------------
 * os_exit - OS-specific cleanup
 */
void os_exit()
{
	int i;
	os_restterm();

	/*-- free all areas alloated by PE cmd ---*/
	for(i=0; i<10; i++){
		char smem_name[16];
		if( pe_area[i] ){
			sprintf( smem_name, "hwbug%d", i );
			os_protect_access( pe_area[i], 0, smem_name );
		}
	}
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
	signal( SIGSEGV, catch_sigbus );
}

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
    if(ioctl(0, OTIOCGETP, &OrgOpts) !=0)     /* save old status */
	{
        perror("ioctl(TIOCGETP)");
		exit(1);
	}
    MyOpts = OrgOpts;
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

    if(ioctl(0, OTIOCSETP, &MyOpts) !=0) 
	{
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
 * permit_access - user function to allow access to a memory area
 */
permit_access( argc, argv )
int argc;
char **argv;
{
	u_int32 adr, count, virt;
	char smem_name[16];
	int i;
	
	if( argc < 3 ) return 1;
	make_hex( argv[1], &adr );
	make_hex( argv[2], &count );

	for( i=0; i<10; i++ ){
		sprintf( smem_name, "hwbug%d", i );
		virt = os_permit_access( adr, count, smem_name );
		if( virt ) break;
	}
	if( virt ){
		printf("The virtual address of the memory is $%08x\n", virt );
		pe_area[i] = virt;
	}
	else
		printf("Cannot access this area!\n");	
}
	
/*----------------------------------------------------------------------
 * os_permit_access - allow access to a mem area
 */	
u_int32 os_permit_access( adr, size, name )
u_int32 adr;
int size;
char *name;
{
	u_int32 virt, offs=0;

	if( adr & 0xfff ){
		offs = adr & 0xfff;
		adr &= ~0xfff;
		size += offs;
	}
	virt = (u_int32)smem_create(
	 name, (char *)adr, size, SM_READ|SM_WRITE);

	if( virt == 0 ) return 0; 
	return virt+offs; 
}

/*----------------------------------------------------------------------
 * os_protect_access - prevent memory block from accesses
 */
os_protect_access( virt_adr, size, name )
u_int32 virt_adr;
int size;
char *name;
{
	if( virt_adr & 0xfff ){
		virt_adr &= ~0xfff;
	}
	smem_create(name, (char *)virt_adr, 0, SM_DETACH);
	smem_remove(name);
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
	int setjmp_val;
	u_int32 adr;
	
	*be_flag = 0;
	/*----------------------+
	| allow access 			|
	+----------------------*/
	adr = os_permit_access( physadr, size, "hwbug_acc" );
/*printf("virt adr=%x\n", adr );*/
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
	}								
	os_protect_access( adr, 0, "hwbug_acc" );
	
	return value;
}	
#if 0
/*----------------------------------------------------------------------
 * execute_program - execute a program at any address
 * It's buserror-proof and can be aborted by ^C
 * Other exceptions are not handled and will cause HwBug to terminate
 */
execute_program( argc, argv )
int argc;
char **argv;
{
	extern u_int8 assembler_buffer[];
	u_int32 adr, prot_adr;
	int setjmp_val;
	
	if( argc < 2 ) return 1;

	if( *argv[1] == 'T' )
		adr = (u_int32)assembler_buffer;
	else {
		if( make_hex( argv[1], &adr ) == -1 )
			return 1;
	}		
	prot_adr = adr;

	os_permit_access( prot_adr, 0x1000 );
	program_running = 1;
	
	if( (setjmp_val = setjmp( buserr_recover )) == 0 ){		
		patch_berr();

		/*--- execute program ---*/
		(*((void (*) ())adr))();
	}	
	else if(setjmp_val == 1 ){
		printf(" *** bus error\n");
	}
	else if(setjmp_val == 2 ){
		printf(" ^C\n");
	}
	program_running = 0;

	return 0;
}	

#endif

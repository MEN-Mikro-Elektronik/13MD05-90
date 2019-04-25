/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: os9.c
 *      Project: HwBug
 *
 *      $Author: ub $
 *        $Date: 2003/05/26 10:06:11 $
 *    $Revision: 1.1 $
 *
 *  Description: OS-9 specific functions
 *                      
 *                      
 *     Required: - 
 *     Switches: - 
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
 
static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/os9.c,v 1.1 2003/05/26 10:06:11 ub Exp $";


#include "hwbug.h"
#include <sgstat.h>

static char 
	*h_pe = " <adr> <size>               Permit access to memory",
	*h_as = " <adr>|T                    Line-by-line assembler  (T)emp Buffer",
	*h_di = " [<adr>|T]                  Disassemble             (T)emp Buffer",
	*h_go = " <adr>|T                    Execute program         (T)emp Buffer";

void os_setterm(), os_restterm();
static int ssm_installed;
static u_int32 ExcJmp;
static int program_running;
jmp_buf buserr_recover;
static u_int32 CPUType;
static u_int32 old_berr;
static struct sgbuf termpd, oldpd;

/*----------------------------------------------------------------------
 * os_usage - print OS specific usage, scan parameters
 */
void os_usage(argc,argv)
int argc;
char **argv;
{
	if (argc==2 && !strcmp(argv[1],"-?")) {
		printf("Syntax: hwbug \n");
		printf("Function: Hardware Debugger for OS-9\n");
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
	void signal_catch();
	void *mod, *modlink();
	
	intercept( signal_catch );		/* setup signal handler */
	os_setterm();					/* setup path options */

	/*--------------------------+
	| check if SSM installed    |
	+--------------------------*/
	if( (mod = modlink( "ssm", 0 )) != (void *)-1){
		munlink(mod);
		ssm_installed = 1;
	}
	/*--------------------------+
	| add OS specific commands  |
	+--------------------------*/
	if( ssm_installed )
		add_command( "PE", permit_access, h_pe, 0 );	

	add_command( "AS", assemble, h_as, 0 );
	add_command( "DI", disassemble, h_di, 1 );
	add_command( "GO", execute_program, h_go, 0 );

	/*--- get some globals for bus error catching ---*/
	ExcJmp = _getsys( D_ExcJmp, sizeof(ExcJmp));
	CPUType = _getsys( D_MPUTyp, sizeof(long));
}

/*----------------------------------------------------------------------
 * os_exit - OS-specific cleanup
 */
void os_exit()
{
	os_restterm();
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
 * signal_catch - catch signals
 */
void signal_catch(signum)
int signum;
{
	extern int control_c;
	
	switch(signum){
	case 2:
		os_exit();
		exit(2);
	case 3:
		control_c = 1;
		if( program_running ){
			sigmask(0);
			longjmp( buserr_recover, 2 );
		}
		break;
	}	
}

/*----------------------------------------------------------------------
 * os_setterm - setup terminal for RAW input
 */
void os_setterm()
{
	_gs_opt(0,&termpd);
	oldpd = termpd;
	termpd.sg_echo = 0;
	termpd.sg_psch = 0;	/* because its usually ^W */
	_ss_opt(0,&termpd);
} 

/*----------------------------------------------------------------------
 * os_restterm - restore original TTY settings
 */
void os_restterm()
{
	_ss_opt(0,&oldpd);
}

/*----------------------------------------------------------------------
 * permit_access - user function to allow access to a memory area
 */
permit_access( argc, argv )
int argc;
char **argv;
{
	u_int32 adr, count;
	
	if( argc < 3 ) return 1;

	make_hex( argv[1], &adr );
	make_hex( argv[2], &count );
	os_permit_access( adr, count );
}
	
/*----------------------------------------------------------------------
 * os_permit_access - allow access to a mem area
 * This is only done if the SSM module is installed
 */	
os_permit_access( adr, size )
char *adr;
int size;
{
	if( ssm_installed )
		_f_permit(adr, size );
}

/*----------------------------------------------------------------------
 * os_protect_access - prevent memory block from accesses
 */
os_protect_access( adr, size )
char *adr;
int size;
{
	if( ssm_installed )
		_f_protect(adr, size );
}

/*----------------------------------------------------------------------
 * patch_berr - patch buserror vector in the OS-9 jump table
 */
patch_berr()
{
	extern void buserror_catch();
	u_int32 buserr_func;

	buserr_func = (u_int32)buserror_catch;

	/*--- get old buserror routine ---*/
	_cpymem( 0, sizeof(u_int32), ExcJmp+6, &old_berr); 		

	/*--- patch new one ---*/
	os_permit_access( (char*)ExcJmp, 100 );
	*(u_int32 *)(ExcJmp+6) = buserr_func;
}

/*----------------------------------------------------------------------
 * restore_berr - restore original OS-9 buserror routine
 */
restore_berr()
{
	/*--- restore old function ---*/
	*(u_int32 *)(ExcJmp+6) = old_berr;
	os_protect_access((char*)(ExcJmp+6), 100);
}

/*----------------------------------------------------------------------
 * os_access_address - read or write memory (buserror proof)
 * returns: nothing on writes
 *			or read value
 *			be_flag is set if buserror occurred
 */
u_int32 os_access_address( adr, size, read, value, be_flag )
u_int32 adr;
int size, read;
u_int32 value;
int *be_flag;
{
	int setjmp_val;
	
	if( (setjmp_val = setjmp( buserr_recover )) == 0 ){		
		/*----------------------+
		| allow SSM access      |
		+----------------------*/
		os_permit_access( (char*)adr, size );

		/*----------------------+
		| patch buserror vector |
		+----------------------*/
		patch_berr();
		
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
		*be_flag = 0;
	}
	else if(setjmp_val == 1 ){
		printf(" *** bus error\n");
		*be_flag = 1;
	}
	/*--- restore old function ---*/
	restore_berr();
	os_protect_access( (char*)adr, size );
	
	return value;
}	

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

	os_permit_access( (char*)prot_adr, 0x1000 );
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

/*----------------------------------------------------------------------
 * os_get_frame_len - get frame length of bus error exception
 */
os_get_frame_len()
{
	int len;
	
	switch(CPUType){
	case 68000:
	case 68008:	len = 0x0E; break;
	case 68010: len = 0x3a; break;
	case 68020:
	case 68030:	len = 0x5c; break;
	case 68040: len = 0x3c; break;
	case 68060: len = 0x10; break;
	case 68300: len = 0x18; break;
	}
	len += 4;
	return len;
}

_asm("
buserror_catch:
	bsr		os_get_frame_len
	adda.l	d0,sp				adjust supervisor stack
	andi.w #^$2000,sr			back to user mode
	
	lea.l	buserr_recover(a6),a0
	move.l	a0,d0
	moveq	#1,d1
	bsr		longjmp
");


/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_mtest.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2013/10/24 09:56:13 $
 *    $Revision: 1.5 $
 *
 *  	 \brief  Memory test for VME4L master spaces
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_mtest.c,v $
 * Revision 1.5  2013/10/24 09:56:13  ts
 * R: gcc 4.6 under Ubuntu 12.10 suggested parentheses around if statement
 * M: added parentheses
 *
 * Revision 1.4  2013/09/02 13:50:58  ts
 * R: 1. address modifiers on A21 VME bridge should be user programmable
 *    2. test memory range for DMA tests on A21 VME spaces was too small
 * M: 1. add option -m to set privileged or non privileged accesses
 *    2. enlarged test memory array
 *
 * Revision 1.3  2009/09/24 11:14:50  CRuff
 * R: compiler warnings
 * M: fix compiler warnings
 *
 * Revision 1.2  2004/07/26 16:31:41  kp
 * added -x option
 *
 * Revision 1.1  2003/12/15 15:02:34  kp
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

static const char RCSid[]="$Id: vme4l_mtest.c,v 1.5 2013/10/24 09:56:13 ts Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>
#include <MEN/men_typs.h>
#include <MEN/usr_utl.h>
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

#define SWAP16(word)	( (((word)>>8) & 0xff) | (((word)<<8)&0xff00) )

#define SWAP32(dword)	( ((dword)>>24) | ((dword)<<24) | \
							  (((dword)>>8) & 0x0000ff00)   | \
							  (((dword)<<8) & 0x00ff0000)     )

#define ERR_READ	1
#define ERR_WRITE	2
#define ERR_BERR	3
#define ERR_MIRR	4
#define ERR_RW		5

#define MSGWDW_WIDTH 39
#define ERRWDW_WIDTH 40

#define INFOSTEP	262144
/* 32768 */

typedef struct {
    char test_id;
	char *action_line;
	int errcnt;
} test_descr;

test_descr test[] = {
	{ 'b', "Byte access, random pattern" },
	{ 'w', "Word access, random pattern" },
	{ 'l', "Long access, random pattern" },
	{ 'B', "Byte access, linear pattern" },
	{ 'W', "Word access, linear pattern" },
	{ 'L', "Long access, linear pattern" },
	{ 'v', "VME4L_Read/Write block xfer, random pattern" },
	{ 'V', "VME4L_Read/Write block xfer, linear pattern" },
	{ 0 }
};

/*------------------+
| Global variables  |
+------------------*/
u_int8 mode;						/* test mode, MD_xx */
u_int32 startadr, endadr;			/* start and end address of VME memory */
int pass;							/* test pass */
long just_verify;					/* don't write RAM, just verify */
int max_errors=(u_int32)(1<<31)-1;	/* max. errorcount until program abort */
int tot_errors=0;					/* total errors occured */
int accWidth=4;
int spaceNum=4;
int opt_pause=0;
int spaceFd;
int fillInfo=1;
char *mapStart, *mapEnd;			/* mapped startAddr/end */
int busErrCnt=0;
long swSwap = 0;
char G_buf[INFOSTEP<<2];

char *usage_str = "\
Syntax:   vme4l_mtest [<opts>] <startaddr> <endaddr> [<opts>]\n\
Function: VME4L memory test\n\
Options:\n\
    -s=<spc>   VME4L space number [4=a24d16]\n\
    -a=<w>     access width for v/V tests in bytes [4]\n\
    -w=<swp>   swap mode to use [0]\n\
    -n=<num>   number of runs\n\
    -q=<num>   abort after <num> errors\n\
    -m=<mod>   set address modifier (AM) for space <spc> (A21 only!) [0]\n\
                0 (00b) non-privileged data access (default)\n\
                1 (01b) non-privileged program access\n\
                2 (10b) supervisory data access\n\
                3 (11b) supervisory program access\n\
               (BLT: only 0 and 2 supported (supervisory=bit[1])\n\
    -v         don't write to RAM, just verify\n\
    -f         no fill info\n\
    -p         1s delay between read & verify\n\
    -x         do SW swapping (only for \"l\" test)\n\
    -t=<lst>   list of tests to execute:\n"
"\n\
";

void version(void);

int usage( int no )
{
	test_descr *test_p = test;
	fprintf( stderr, "%s", usage_str );

	while(test_p->test_id){
		fprintf(stderr, "       %c: %s\n", test_p->test_id,
				test_p->action_line);
		test_p++;
	}
	version();
	exit( no );
	return 0;					/* not reached */
}

void version(void)
{
	char *rev = "$Revision: 1.5 $";
	char *p = rev+strlen("$Revision: ");

	fprintf(stderr, "\nV ");
	while( *p != '$' ) fprintf(stderr, "%c", *p++);
	fprintf(stderr, " (c) Copyright 1995-2013 by MEN GmbH\n");
}

void goaway(void)
{
	printf( "\nTOTAL TEST RESULT: %d errors\n", tot_errors );
	exit( tot_errors ? 1:0);	
}


void action_info( char *info )
{
	printf("ACTION: %s\n", info );
}

void fill_info( u_int32 bytes )
{
	if( fillInfo )
		printf( "FILLED: %6ldK\n", bytes/1024 );
}

void ok_after_retries( u_int32 address, int num_tries )
{
	printf("Addr %08lx ok after %d retries\n", address, num_tries );
}

void is_mirroring( u_int32 address, u_int32 size )
{
	printf("Addr %08lx mirroring after %08lx\n", address, size );
}

char *get_errtype_str( int err_type )
{
	switch(err_type){
	case ERR_READ: return "READ";
	case ERR_WRITE: return "WRIT";
	case ERR_BERR: return "BERR";
	case ERR_MIRR: return "MIRR";
	case ERR_RW:   return "RDWR";
	default:       return "????";
	}
}

void out_error( int err_type, u_int32 address, u_int32 is,
				   u_int32 shouldbe, int access_size )
{
	u_int32 mask = (1LL<<(access_size*8))-1;
	char fmt[200], *p=fmt;

	shouldbe &= mask;			/* mask unwanted bits */
	is &= mask;

	p += sprintf(fmt, "ERROR: %s", "%-4s %08x");

	if( err_type == ERR_READ || err_type == ERR_WRITE || err_type==ERR_RW){
		switch( access_size ){
		case 1:	sprintf( p, "%s", " is %02lx shouldbe %02lx"); 	break;
		case 2:	sprintf( p, "%s", " is %04lx shouldbe %04lx");		break;
		case 4:	sprintf( p, "%s", " is %08lx shouldbe %08lx");		break;
		}
	}


	printf( fmt, get_errtype_str(err_type), address, is,
			shouldbe );
	printf("\n");

	if( ++tot_errors >= max_errors ){
		goaway();
	}	
}

/********************************** mk_rand_pat *****************************
 *
 *  Description:  Creates a random pattern using a fast random algorithm.
 *				
 *
 *---------------------------------------------------------------------------
 *  Input......:  address - only used to dermine when to create a new random
 *							pattern. If address is long aligned, a new
 *							random pattern is created, otherwise the old
 *							random value is shifted according to size.
 *				  oldpat  - previous pattern returned by mk_rand_pat or
 *							starting value for pattern
 *				  size    - required size for random pattern
 *					        1=byte 2=word 4=long
 *
 *  Output.....:  return  - 8/16/32 bit random pattern
 *				
 *  Globals....:  ---
 ****************************************************************************/
u_int32 mk_rand_pat( u_int32 address, u_int32 oldpat, u_int8 size )
{
	u_int32 pattern;

	if( size!=4 ){
		pattern = oldpat << (size<<3);	/* emulates rol.l #y,xx */
		pattern |= (oldpat>>(32-(size<<3))) & ((1<<(size<<3))-1);
	} else {
		pattern = oldpat;
		if( swSwap )
		  pattern = SWAP32(pattern);
	}

	if((address & 0x3)==0){
		register u_int32 a = pattern;
			
		a <<= 11;
		a += pattern;
		a <<= 2;
		pattern += a;
		pattern += 13849;
	}

	if( swSwap && size==4)
	  pattern = SWAP32(pattern);


	return pattern;
}

/********************************** mk_lin_pat *****************************
 *
 *  Description:  Creates a linear testpattern which corresponds to the
 *				  given address
 *
 *---------------------------------------------------------------------------
 *  Input......:  address - used to produce the test pattern
 *
 *				  oldpat  - not used, just for compat. with mk_rand_pat
 *				  size    - required size for pattern
 *					        1=byte 2=word 4=long
 *
 *  Output.....:  return  - 8/16/32 bit linear pattern
 *				
 *  Globals....:  ---
 ****************************************************************************/
u_int32 mk_lin_pat( u_int32 address, u_int32 oldpat, u_int8 size )
{
	switch(size){
	case 4: return address;
	case 2: return (address&0x2) ? (address&~0x2) : address>>16;
	case 1:
		switch(address & 0x3 ){
		case 0:	return address>>24;
		case 1: return address>>16;
		case 2: return address>>8;
		case 3: return address&~0x3;
		}
	}
	CHK(0);
 ABORT:
	tot_errors++;
	goaway();
	return 0;
}
			

static int berr_check(volatile void *p)
{
	u_int32 vmeaddr = (u_int32)((char *)p-mapStart)+startadr;
	if( busErrCnt ){
		printf("*** VME bus error detected "
			   "(can be another program)\n");
		out_error( ERR_BERR, vmeaddr, 0,0,0 );
		busErrCnt = 0;
		return 1;
	}
	return 0;
}

/********************************** BWL_TEST ********************************
 *
 *  Description:  Macro for Byte/Word/Long tests
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  access - test access type (u_int8, u_int16, u_int32)
 *				  pat_f  - function name to build test pattern (mk_xxx_pat)
 *  Output.....:
 *  Variables..:  mapStart, mapEnd, pass, just_verify
 ****************************************************************************/
#define BWL_TEST(access,pat_f)\
{\
	register u_int32 randval=0xABCDEF02+pass;\
	register access rval;\
	access *p;\
	register u_int32 count=0, curlen;\
	u_int32 len = (mapEnd-mapStart)/sizeof(access);\
	\
    if( swSwap ) randval=SWAP32(randval);\
	p = (access*)mapStart;\
	\
	/*--------------+\
    |  Fill memory  |\
    +--------------*/\
	if( !just_verify){\
     action_info("Filling Memory");\
	 while( len > 0){\
		curlen = len;\
		if( curlen > INFOSTEP ) curlen = INFOSTEP;\
		count += curlen;\
		len -= curlen;\
		while(curlen--){\
			/* create a new test pattern */\
			randval = pat_f((unsigned long)p,randval,sizeof(access));	\
			*p++ = (access)randval;	/* store value */\
            errorcount += berr_check(p);\
		}\
		fill_info(count*sizeof(access));\
	 }\
    }\
	/*----------------+\
    |  Verify Memory  |\
    +----------------*/\
	if (opt_pause)\
        UOS_Delay(1000);\
	p = (access*)mapStart;\
	len = (mapEnd-mapStart)/sizeof(access);\
	randval=0xABCDEF02+pass;\
    if( swSwap ) randval=SWAP32(randval);\
	count=0;\
	\
    action_info("Verify Memory");\
	while( len > 0){\
		curlen = len;\
		if( curlen > INFOSTEP ) curlen = INFOSTEP;\
		count += curlen;\
		len -= curlen;\
		while(curlen--){\
			  /* create a new test pattern */\
 			  randval = pat_f((unsigned long)p,randval,sizeof(access));\
			  if( (rval = *p++) != (access)randval ){\
                uint32_t vmeaddr = (unsigned long)((char *)p-mapStart)+startadr;\
				int err_type=ERR_WRITE;\
				p--;\
				errorcount++;\
				/*------------------------------------------+\
                | Verify error detected..                   |\
                | 1) Set HW Trigger                         |\
                | 3) retry access 3 times                   |\
                | 2) Check for mirrored memory              |\
                | 4) Display error to user                  |\
                +------------------------------------------*/\
				{\
					/*--- retry access ---*/\
					int i=0;\
				    for(i=1; i<=3; i++)\
						if( *p == (access)randval){\
							ok_after_retries(vmeaddr,i);\
						    err_type=ERR_READ;\
							break;\
						}\
			    }\
				if( err_type == ERR_WRITE ){\
					access *t = p;\
					u_int32 step=1;\
					/*--- detect mirrored RAM ---*/\
					while( &t[step]<(access *)mapEnd ){\
						if( t[step]==(access)rval){\
							is_mirroring( vmeaddr, (&t[step])-p );\
							err_type = ERR_MIRR;\
							break;\
						}\
						step<<=1;\
					}\
				}\
				out_error( err_type, vmeaddr, \
						   rval, (access)randval,\
						   sizeof(access));\
				p++;\
			}\
            errorcount += berr_check(p);\
		}\
		fill_info(count*sizeof(access));\
	}\
}


/*---------------------------------------------+
| VMEbus block transfer using VME4L_Read/Write |
+----------------------------------------------*/
void vmeblt( void *src, void *dst, u_int32 len, int direction )
{
	if( direction ){
		CHK( VME4L_Write( spaceFd, (vmeaddr_t)(unsigned long)dst, accWidth, len, src, VME4L_RW_NOFLAGS ) >= 0 );
	}
	else {
		CHK( VME4L_Read( spaceFd, (vmeaddr_t)(unsigned long)src, accWidth, len, dst, VME4L_RW_NOFLAGS ) >= 0 );
	}
	return;
 ABORT:
	tot_errors++;
	goaway();
}


int blk_test(int rand_pattern, void (*readfunc)(), void (*writefunc)())
{
	u_int32 blk_size = INFOSTEP, cur_size;
	u_int32 pattern=0xabcdef02+pass;
	unsigned long *buf;
	unsigned long address, i, *p;
	int errcnt=0;
	buf = (unsigned long*)G_buf;
	memset(buf, 0x0, sizeof(G_buf));

	/*---------------+
    |  Write memory  |
	+---------------*/
	if( !just_verify){
		address = startadr;

		action_info("Filling Memory");
		while( address < endadr ){
			/*--------------------+
			  |  Fill write buffer  |
			  +--------------------*/
			if( rand_pattern )
				for(p=buf,i=0; i<blk_size/4; i++)
					*p++ = pattern = mk_rand_pat(0, pattern, 4 );
			else
				for(p=buf,i=0; i<blk_size/4; i++)
					*p++ = address+(i<<2);

			/*----------------------+
			  |  Write buffer to mem  |
			  +----------------------*/
			cur_size = blk_size;
			if( cur_size > endadr-address )
				cur_size = endadr-address;
			(*writefunc)( buf, address, cur_size, 1);

			address += cur_size;

			fill_info(address-startadr);
		}
	}

	address = startadr;
	pattern=0xabcdef02+pass;

	/* data written to VME receiver, wipe out buf again */
	memset(buf, 0x0, sizeof(G_buf));
	if (opt_pause)
		UOS_Delay(1000);

	/*---------------+
    |  Verify memory |
	+---------------*/
    action_info("Verify Memory");
	while( address < endadr ){
		/*------------------------+
        |  Read to buffer memory  |
        +------------------------*/
		cur_size = blk_size;
		if( cur_size > endadr-address )
			cur_size = endadr-address;

		memset( buf, 0x55, cur_size ); /* clear before reading */

		(*readfunc)( address, buf, cur_size, 0 );

		/*-----------------+
        |  Compare buffer  |
        +-----------------*/
		for(p=buf,i=0; i<cur_size/4; p++,i++){
			if( rand_pattern )
				pattern = mk_rand_pat(0, pattern, 4 );
			else
				pattern = address+(i<<2);
			
			if( *p != pattern ){
				errcnt++;
				out_error( ERR_RW, address+(i<<2), *p, pattern, 4 );
			}
		}
		address += cur_size;

		fill_info(address-startadr);
	}
	/* free(buf); */

	return errcnt;

}
				
int do_test(char test_id)
{
	u_int32 errorcount=0;
	int isMapped = 0;
	void* map2 = mapStart;

	if( strchr( "bwlBWL", test_id ) != NULL ){
		CHK( VME4L_Map( spaceFd, startadr, endadr-startadr,
						(void **)&map2 )==0);
		mapStart=map2;
		mapEnd = mapStart + (endadr-startadr);
		isMapped = 1;
		busErrCnt=0;
	}

	switch( test_id ){
	case 'b':
		BWL_TEST(volatile u_int8, mk_rand_pat); break;
	case 'w':
		BWL_TEST(volatile u_int16, mk_rand_pat); break;
	case 'l':
		BWL_TEST(volatile u_int32, mk_rand_pat); break;
	case 'B':
		BWL_TEST(volatile u_int8, mk_lin_pat); break;
	case 'W':
		BWL_TEST(volatile u_int16, mk_lin_pat); break;
	case 'L':
		BWL_TEST(volatile u_int32, mk_lin_pat); break;
	case 'v':
		errorcount = blk_test( 1, vmeblt, vmeblt); break;
	case 'V':
		errorcount = blk_test( 0, vmeblt, vmeblt); break;
	}
	if( isMapped ){
		CHK( VME4L_UnMap( spaceFd, mapStart, endadr-startadr ) == 0 );
	}
	return errorcount;
 ABORT:
	tot_errors++;
	goaway();
	return 0;
}

void SigBusHandler( int sigCode )
{
	busErrCnt++;
}

int main( int argc, char *argv[] )
{
	int i, total_pass;
	char *optp, *testlist;
	test_descr *test_p;
	char *test_id;
	int errors=0;
	int opt_mod=-1;
	int swapMode=0;

	if( UTL_TSTOPT("?")) usage(0);	

	/*-----------------------------+
    |  Parse command line options  |
    +-----------------------------*/
	startadr = endadr = 0xffffffff;

	for(i=1; i<argc; i++ )
		if( *argv[i]!='-'){
			if( startadr == 0xffffffff )
				sscanf( argv[i], "%lx", &startadr );
			else
				sscanf( argv[i], "%lx", &endadr );
		}

	if( startadr==0xffffffff || endadr==0xffffffff ){
		printf("missing start or end address\n");
		usage(1);
	}

	total_pass = 1;
	if( (optp=UTL_TSTOPT("n="))) total_pass=atoi(optp);

	just_verify = (long)UTL_TSTOPT("v");
	fillInfo = !(long)UTL_TSTOPT("f");
	testlist = UTL_TSTOPT("t=");
	if( !testlist ) usage(1);

	swSwap = (long)UTL_TSTOPT("x");
	if( (optp=UTL_TSTOPT("q="))) max_errors=atoi(optp);
	if( (optp=UTL_TSTOPT("a="))) sscanf(optp, "%d", &accWidth);
	if( (optp=UTL_TSTOPT("s="))) spaceNum=atoi(optp);
	if( (optp=UTL_TSTOPT("m="))) opt_mod=atoi(optp);
	if( (optp=UTL_TSTOPT("w="))) swapMode=atoi(optp);
	opt_pause = UTL_TSTOPT("p") ? 1 : 0;

	
	CHK( ((opt_mod < 4) && (opt_mod >=0)) || (opt_mod == -1));

	/* open space */
	printf("Open space %d: %s\n", spaceNum, VME4L_SpaceName(spaceNum));
	
	CHK( (spaceFd = VME4L_Open( spaceNum )) >= 0 );

	CHK( VME4L_SwapModeSet( spaceFd, swapMode ) == 0 );

	if( opt_mod >= 0) {
		printf("change AM to 0x%x\n", opt_mod & 0xff );
		CHK( VME4L_AddrModifierSet( spaceFd, (char)(opt_mod & 0xff))==0 );
	}

	
	/* install buserr signal */
	signal( SIGBUS, SigBusHandler );

	CHK( VME4L_SigInstall( spaceFd, VME4L_IRQVEC_BUSERR,
						   VME4L_IRQLEV_BUSERR, SIGBUS,
						   VME4L_IRQ_NOFLAGS) == 0 );

	/*----------------+
    |  Execute tests  |
    +----------------*/
	printf("Testing VME memory %08x .. %08x\n", startadr, endadr );


	for( pass=0; pass<total_pass || total_pass==0; pass++ ){
		printf("PASS %d...\n", pass );

		for( i=0,test_id=testlist; *test_id; test_id++,i++ ){
			for( test_p=test; test_p->test_id; test_p++ )
				if( test_p->test_id == *test_id )
					break;
			if( test_p->test_id ){
				printf( "%s\n", test_p->action_line );

				errors = do_test( *test_id );

				test_p->errcnt += errors;
				printf( "TEST result: %-32s %5d errors\n", test_p->action_line,
						 test_p->errcnt);
			}
		}
	}

 ABORT:

	goaway();
	return 0;
}


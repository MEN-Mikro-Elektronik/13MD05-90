/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: change.c
 *      Project: HwBug
 *
 *      $Author: GLeonhardt $
 *        $Date: 2008/09/15 13:15:15 $
 *    $Revision: 1.2 $
 *
 *  Description: change memory contents
 *                      
 *                      
 *     Required: - 
 *     Switches: - 
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  change_data( argc, argv )           
 *  void _change_data( adr, size, incr, wonly )
 *  fill_data( argc, argv )             
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
 
static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/change.c,v 1.2 2008/09/15 13:15:15 GLeonhardt Exp $";
#include "hwbug.h"

void _change_data();

int change_data( int argc, char **argv)
{
	int o_size = 1;		/* default=byte accesses */
	int o_wonly = 0;	/* default read/write */
	int o_incr = -1;	/* default increment = o_size */
	static u_int32 last_adr;
	u_int32 adr;
	char *cmdarg = argv[0];
    /* include ident string, suppress warning */
    char a = a = RCSid[0];
	
	/*--------------+
	| get options   |
	+--------------*/
	cmdarg++;				/* skip 'C' */
	while( !isspace(*cmdarg) && *cmdarg ){
		switch( *cmdarg ){
			case 'B':					/* byte accesses */
				o_size = 1;
				break;
			case 'W':					/* word accesses */
				o_size = 2;
				break;
			case 'L':					/* long accesses */
				o_size = 4;
				break;
			case 'N':
				o_wonly = 1;			/* write only */
				break;
			default:
				if( isdigit(*cmdarg))
					o_incr = *cmdarg - '0';
				break;
		}
		cmdarg++;
	}
	if( o_incr == -1 ) o_incr = o_size;

	if( argc >= 2 ){
		if( make_hex( argv[1], &adr ) == -1 )
			return 1;		/* say what? */
	}
	else
		adr = last_adr;				

	last_adr = adr;

	_change_data( adr, o_size, o_incr, o_wonly );

	return 0;
}	

void _change_data( adr, size, incr, wonly )
u_int32 adr;
int size, incr, wonly;
{
	extern char *get_line();
	u_int32 value;
	char value_fmt[10];
	char *line;
	int buserr;

	sprintf( value_fmt, "%%0%dlx ", size*2 );
	
	do {
		printf( "%08lx: ", adr );
		if( !wonly ){
			value = os_access_address( adr, size, 1, 0, &buserr );
			if( buserr ) return;				
			printf( value_fmt, value );
		}
		FLUSH;

		line = get_line("-> ");	/* enter new data */

		switch( *line ){
		case '.':
		case 'Q':	printf("\n");
					return;

		case '-':	adr -= incr;
					printf("\n");
					continue;
		}
		
		if( *line ){
			if( make_hex( line, &value ) == -1 ){
				printf(" *** bad hex value\n");
				continue;
			}
			/*--- write value ---*/
			os_access_address( adr, size, 0, value, &buserr );
			if( buserr ) return;
		}
		adr += incr;		
		printf("\n");
	} while(1);	
}


int fill_data( int argc, char **argv)
{
	u_int32 adr,fill=0,n,size,buserr;
	int o_size = 1;		/* default=byte accesses */
	int o_patt = 0;	
	int o_incr = -1;	/* default increment = o_size */
	char *cmdarg = argv[0];
	char value_fmt[10];

	/*--------------+
	| get options   |
	+--------------*/
	cmdarg++;				/* skip 'F' */
	while( !isspace(*cmdarg) && *cmdarg ){
		switch( *cmdarg ){
			case 'B':					/* byte accesses */
				o_size = 1;
				break;
			case 'W':					/* word accesses */
				o_size = 2;
				break;
			case 'L':					/* long accesses */
				o_size = 4;
				break;
			case 'P':
				o_patt = 1;				/* linear pattern */
				break;
			default:
				return 1;				/* say what? */
		}
		cmdarg++;
	}

	if( o_incr == -1 ) o_incr = o_size;

	if (argc >=3) {
		if (make_hex(argv[1], &adr) == -1	||		/* address */
			make_hex(argv[2], &size) == -1)			/* size */
			return 1;				/* say what? */
	}
	else
		return 1;				/* say what? */

	if (o_patt == 0) {							/* no pattern ? */
		if (argc < 4	||						/* too less args ? */
			make_hex(argv[3], &fill) == -1)		/* filler */
				return 1;		/* say what? */
	}

	/*--------------+
	| fill memory   |
	+--------------*/
	size = (size/o_size) * o_size;

	if (size==0) {
		printf(" *** <cnt> too small\n");
		return 0;
	}

	printf(" Filling %08lX..%08lX with ",adr,adr+size-1);

	if (o_patt == 0) {
		/*-------------------------+
        |  fill with fixed value   |
        +-------------------------*/
		sprintf( value_fmt, "%%0%dlX\n", o_size*2 );
		printf(value_fmt,fill);

		for (n=0; n<size; n+=o_incr) {
			os_access_address( adr+n, o_size, 0, fill, &buserr );

			if (buserr)
				return 0;				
		}
	}
	else {
		/*-------------------------+
        |  fill with lin. pattern  |
        +-------------------------*/
		printf("linear pattern (incr=%d)\n",o_incr);

		for (n=0; n<size; n+=o_incr, fill+=o_incr) {
			os_access_address( adr+n, o_size, 0, fill, &buserr );

			if (buserr)
				return 0;				
		}
	}

	return 0;
}	



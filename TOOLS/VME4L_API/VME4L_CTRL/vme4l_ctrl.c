/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_ctrl.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/12/15 15:02:39 $
 *    $Revision: 1.1 $
 *
 *  	 \brief  Example program for VME4L_ controller funcs
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_ctrl.c,v $
 * Revision 1.1  2003/12/15 15:02:39  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

static const char RCSid[]="$Id: vme4l_ctrl.c,v 1.1 2003/12/15 15:02:39 kp Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <MEN/men_typs.h>
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>
#include <MEN/pldz002-cham.h> /* for 16Z002 specific buserror info */


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

static void usage(void)
{
	printf("Usage: vme4l_ctrl <item> [<value>]\n"
		   "Where <item> is one of:\n"
		   " sys - system controller function\n"
		   " res - VMEbus system reset (write-only)\n"
		   " ato - arbitration timeout (read-only)\n"
		   " ber - last bus error info (read-only)\n"
		   " req - requester mode\n"
		   " lvl - requester level (0..3, default:3)\n"
	           " geo - read geographical address\n"
		   " pwr - posted write mode\n");
	exit(1);
}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd;
	int state=999, set=0;
	char *item;

	if( argc < 2 )
		usage();

	item = argv[1];

	if( argc > 2 ){
		set = 1;
		state = strtol( argv[2], NULL, 10 );
	}

	CHK( (fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 );

	if( !strcmp( item, "sys") ){
		if( set ){
			CHK( VME4L_SysCtrlFunctionSet( fd, state ) == 0 );
		}
		else {
			CHK( (state = VME4L_SysCtrlFunctionGet( fd )) >= 0 );
		}
	}
	else if( !strcmp( item, "res") ){
		CHK( VME4L_SysReset( fd ) == 0 );			
		goto DONE;
	}

	else if( !strcmp( item, "ato") ){
		if( !set ){
			CHK( (state = VME4L_ArbitrationTimeoutGet( fd, 1 )) >= 0 );
		}
		else goto INVAL;
	}

	else if( !strcmp( item, "ber") ){
		if( !set ){
			int attr=0;
			vmeaddr_t vmeaddr;
			
			CHK( (state = VME4L_BusErrorGet( fd, &attr, &vmeaddr, 1 )) >= 0 );
			if (state != 0) {
			printf("Bus error info is %svalid. Reason: %s VME addr 0x%lx AM 0x%02x %s state.\n",
					state > 0 ? "":"in",
					(attr & PLDZ002_BERR_RW_DIR) ? "read from" : "write to",
					vmeaddr,
					(attr & PLDZ002_BERR_ACC_AM_MASK),
					(attr & PLDZ002_BERR_IACK) ? "IACK" : "normal");
			} else {
				printf("No valid or new Bus error info available since last call to VME4L_BusErrorGet().\n");
			}
			goto DONE;
		}
		else goto INVAL;
	}

	else if( !strcmp( item, "req") ){
		if( set ){
			CHK( VME4L_RequesterModeSet( fd, state ) == 0 );
		}
		else {
			CHK( (state = VME4L_RequesterModeGet( fd )) >= 0 );
		}
	}

	else if( !strcmp( item, "geo") ){
	        CHK( (state = VME4L_GeoAddrGet( fd )) >= 0 );
	}

	else if( !strcmp( item, "lvl") ){
		if( set ){
			CHK( VME4L_RequesterLevelSet( fd, state ) == 0 );
		}
		else {
			CHK( (state = VME4L_RequesterLevelGet( fd )) >= 0 );
		}
	}

	else if( !strcmp( item, "pwr") ){
		if( set ){
			CHK( VME4L_PostedWriteModeSet( fd, state ) == 0 );
		}
		else {
			CHK( (state = VME4L_PostedWriteModeGet( fd )) >= 0 );
		}
	}
	else goto INVAL;


	if( !set )
		printf( "current state=%d\n", state );
 DONE:
	return 0;
 INVAL:
	printf("Operation not implemented\n");
 ABORT:
	return 1;
}

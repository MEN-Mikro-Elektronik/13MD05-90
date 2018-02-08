/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_crcsr.c
 *
 *      \author  adam.wujek@cern.ch
 *        $Date: 2018/02/08 19:19:53 $
 *    $Revision: 1.0 $
 *
 *  	 \brief  Example program for Set/Get address and AM in/from CR/CSR on
 *               which VME slave will respond
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2018 by CERN
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <errno.h>

#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>
#include <MEN/men_typs.h>
#include <MEN/usr_utl.h>

/*--------------------------------------+
  |   DEFINES                             |
  +--------------------------------------*/
#define CHK(expression)							\
	if( !(expression)) {						\
		printf("\n*** Error during: %s\nfile %s\nline %d\n",	\
			   #expression,__FILE__,__LINE__);		\
		printf("%s\n",strerror(errno));				\
		exit(1);						\
	}


#define FUN_ADER 0x7FF60

/*--------------------------------------+
  |   TYPDEFS                             |
  +--------------------------------------*/

/*--------------------------------------+
  |   GLOBALS                             |
  +--------------------------------------*/

/*--------------------------------------+
  |   PROTOTYPES                          |
  +--------------------------------------*/

static void usage(int excode)
{
	printf("Syntax:   vme4l_crcsr [<opts>]\n");
	printf("Function: Set/Get address and AM in/from CR/CSR on which VME slave will respond\n");
	printf("Options:\n\n");
	printf("-s=<spc>      Slot number\n");
	printf("-a=<width>    Address Modifier\n");
	printf("-v=<init.Val> VME address to which slave shall respond\n");
	printf("-r            Read func_ader\n");
	printf("-w            Write fun_ader\n");
	printf("-f=<fun_ader> Which func_ader to use\n");
	printf("\n");
	printf("git version: %s\n", GIT_VERSION);
	exit(excode);
}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd=-1, rv;
	void *map;
	vmeaddr_t vmeAddr;
	vmeaddr_t vmeAddr_page = 0;
	size_t size = 0xffffffff;
	char *optp=NULL;
	int opt_read=-1;
	uint32_t opt_slot = 0;
	uint32_t opt_dst_am = 0;
	uint32_t opt_dst_addr = 0xffffffff;
	uint32_t opt_fun_ader = 0;
	uint8_t ader_regs[16];
	int map_offset = 0;
	
	if( UTL_TSTOPT("?") || UTL_TSTOPT("h") || (argc == 1) )
		usage(0);

	/*-----------------------------+
	  |  Parse command line options  |
	  +-----------------------------*/

	/* Check / parse args */

	if (UTL_TSTOPT("r"))
		opt_read = 1;
	else if (UTL_TSTOPT("w"))
		opt_read = 0;

	if( (optp=UTL_TSTOPT("s="))) {
		opt_slot = strtoul (optp, NULL, 0);
		if (!opt_slot) {
			printf("Wrong slot number!\n");
			exit(1);
		}
	}

	if( (optp = UTL_TSTOPT("a="))) {
		opt_dst_am = strtoul (optp, NULL, 0);
	}

	if( (optp = UTL_TSTOPT("v="))) {
		opt_dst_addr = strtoul (optp, NULL, 0);
	}

	if( (optp = UTL_TSTOPT("f="))) {
		opt_fun_ader = strtoul (optp, NULL, 0);
	}

	if (opt_read == -1) {
		printf("*** specify either -r or -w!\n");
		usage(1);
	}

	if (opt_slot == 0) {
		printf("Missing slot!\n");
		exit(1);
	}

	if (!opt_read && opt_dst_addr == 0xffffffff) {
		printf("Missing vmeaddr!\n");
		exit(1);
	}

	if (!opt_read && opt_dst_am == 0) {
		printf("Missing address modifier!\n");
		exit(1);
	}

	size = 0x80000; /* map entire CR/CSR space of a board in a given slot*/
	vmeAddr = opt_slot * 0x80000;

	CHK( (fd = VME4L_Open( VME4L_SPC_CR_CSR )) >= 0 /*node /dev/vme4l_<spc> must exist*/ );

	vmeAddr_page = vmeAddr & ~(getpagesize() - 1);
	map_offset = vmeAddr - vmeAddr_page;
	CHK( (rv = VME4L_Map( fd, vmeAddr_page, size + map_offset, &map )) == 0 );

	if (opt_read) {
		memcpy(ader_regs,
		       map + FUN_ADER + opt_fun_ader * 0x10,
		       sizeof(ader_regs));
       		opt_dst_addr = (ader_regs[0+3] & 0xff) << 24;
		opt_dst_addr |= (ader_regs[4+3] & 0xff) << 16;
		opt_dst_addr |= (ader_regs[8+3] & 0xff) << 8;
		opt_dst_am = (ader_regs[12+3] & 0xff) >> 2;

		printf("Slot: %d addr: 0x%x am: 0x%x fun_ader_index %d\n",
		       opt_slot, opt_dst_addr, opt_dst_am, opt_fun_ader);
	} else {
		printf("Slot: %d addr: 0x%x am: 0x%x fun_ader_index %d\n",
		       opt_slot, opt_dst_addr, opt_dst_am, opt_fun_ader);
		ader_regs[0+3] = (opt_dst_addr >> 24) & 0xff;
		ader_regs[4+3] = (opt_dst_addr >> 16) & 0xff;
		ader_regs[8+3] = (opt_dst_addr >> 8) & 0xff;
		ader_regs[12+3] = ((opt_dst_addr >> 0) | (opt_dst_am << 2)) & 0xff;
		memcpy(map + FUN_ADER + opt_fun_ader * 0x10,
			ader_regs,
			sizeof(ader_regs));
	}

	CHK( (rv = VME4L_UnMap( fd, map, size + map_offset)) == 0 );
	VME4L_Close( fd );
	return 0;
}

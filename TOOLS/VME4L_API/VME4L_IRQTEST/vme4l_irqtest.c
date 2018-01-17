/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_irqtest.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/09/24 11:10:10 $
 *    $Revision: 1.4 $
 *
 *  	 \brief  Example program for VME4L_Read/Write
 *
 *	This program shows how to access VME space and VME interrupts. In order
 *  to run, it requires a A201 or A203N VME M-Module carrier board
 *  with an M99 M-module.
 *
 *  The A201/A203N + M99 is programmed to generate fixed number of interrupts
 *  per second. The M99Int() routine will measure the interrupt latency of
 *  the M99Int() handler, by reading the M99's timer.
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_irqtest.c,v $
 * Revision 1.4  2009/09/24 11:10:10  CRuff
 * R: compiler warnings
 * M: change pointer type of map in main to avoid compiler warning
 *
 * Revision 1.3  2009/06/03 19:15:45  rt
 * R: 1.) No support for VME addresses > 0xFFFFFFFF.
 * M: 1.) Use strtoull for VME address.
 *
 * Revision 1.2  2009/04/30 21:11:38  rt
 * R: 1.) Add support for A203N m-module carrier board.
 *    2.) Add support for TSI148 VME bridge.
 * M: 1.) Clear A203N interrupt.
 *    2.) Swap register accesses if needed.
 *
 * Revision 1.1  2003/12/15 15:02:36  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003-2009 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

static const char RCSid[]="$Id: vme4l_irqtest.c,v 1.4 2009/09/24 11:10:10 CRuff Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
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

#define PAGE_SIZE 		0x1000
#define M99_A201_REG_SIZE	0x200
#define M99_IRQ_SIG		(SIGRTMIN+1)

/* MC68230 registers */
#define PGC_REG    0x80    /* port general control */
#define PADD_REG   0x84    /* port A data dir */
#define PBDD_REG   0x86    /* port B data dir */
#define PAC_REG    0x8c    /* port A control */
#define PBC_REG    0x8e    /* port B control */
#define PAD_REG    0x90    /* port A data */
#define PBD_REG    0x92    /* port B data */
#define TC_REG     0xa0    /* timer control */
#define TIV_REG    0xa2    /* timer irq vector, not used */
#define CPH_REG    0xa6    /* counter preload high */
#define CPM_REG    0xa8    /* counter preload middle */
#define CPL_REG    0xaa    /* counter preload low */
#define CNTH_REG   0xae
#define CNTM_REG   0xb0
#define CNTL_REG   0xb2

#define TS_REG     0xb4    /* timer status */

#ifdef MEN_TSI148
	/* TSI148 VME bridge doesn't support hardware swapping, therefore user
	application has to care about swapping. */
	static inline uint16_t SwapD16( uint16_t val )
	{
	#ifdef __powerpc__
	return( val );
	#else
	return( ((val)<<8) | ((val)>>8) );
	#endif /*__powerpc__*/
	}
#else
	/* For other VME bridges we do not need to care about swapping */
	#define SwapD16(val) (val)
#endif /* MEN_TSI148 */

#define MWRITE_D16(ma,offs,val) \
   *(volatile uint16_t*)((uint8_t*)(ma)+(offs)) = SwapD16(val)
#define MREAD_D16(ma,offs) \
   (SwapD16(*(volatile uint16_t*)((uint8_t*)(ma)+(offs))))
/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
void *G_m99Base;
uint32_t G_timer;
uint32_t G_irqCount=0;
int G_maxLatency=0, G_minLatency=0x7FFFFFFF;
int G_level;
int G_fd;
int G_abort;
/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
int A201M99_Init( int vector, int level);
void M99Int();


static void usage(void)
{
	printf("Usage: vme4l_irqtest <level 1-7>\n");
	exit(1);
}

static void SigIntHandler( int sigNum )
{
	G_abort = 1;
}

static void M99SigHandler( int sigNum )
{
	VME4L_IrqEnable( G_fd, G_level ); /* reenable interrupt level */
}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int rv;
	vmeaddr_t vmeAddr;
	vmeaddr_t vmeMap;
	int vmeOff, vector, level;
	void *map;

	if( argc != 3 )
		usage();

	level   = strtoul( argv[1], NULL, 10 );
	G_level = level;
	vector=level;
	printf("vme4l_irqtest: waiting for Level %d\n", level );
	/* open VME space */
	CHK( (G_fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 );

	/* install signal for M99 interrupt (don't enable irq now) */
	signal( M99_IRQ_SIG, M99SigHandler ); /* catch sig 33 (typical) */
	signal( SIGINT, SigIntHandler );

	CHK( VME4L_SigInstall( G_fd, vector, level, SIGUSR1, VME4L_IRQ_NOFLAGS ) == 0 );

	/* enable VME interrupt level */
	CHK( VME4L_IrqEnable( G_fd, level ) == 0);

	while(!G_abort) {
		sleep(1);
		fprintf(".");
	}
	printf("\nreceived VME IRQ level %d, OK!\n", level);
	return 0;

 ABORT:
	return 1;
}


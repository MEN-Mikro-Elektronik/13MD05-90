/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_m99irq.c
 *
 *      \author  klaus.popp@men.de
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
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
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

#define PAGE_SIZE 			0x1000
#define M99_A201_REG_SIZE	0x200
#define M99_IRQ_SIG			(SIGRTMIN+1)

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
	printf("Usage: vme4l_m99irq <vmeaddr> <vector> <level> <timerval>\n");
	exit(1);
}

static void SigIntHandler( int sigNum )
{
	G_abort = 1;
}

static void M99SigHandler( int sigNum )
{
	M99Int();					/* handle M99 interrupt */
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

	if( argc != 5 )
		usage();

	vmeAddr = strtoull( argv[1], NULL, 16 );
	vector  = strtoul( argv[2], NULL, 10 );
	level   = strtoul( argv[3], NULL, 10 );
	G_timer = strtoul( argv[4], NULL, 10 );
	G_level = level;

	/* open VME space */
	CHK( (G_fd = VME4L_Open( VME4L_SPC_A24_D16 )) >= 0 );

	/*--- map VME space into our application ---*/
	/* align vmeMap to 4K */
	vmeMap = vmeAddr & ~(PAGE_SIZE-1);
	vmeOff = vmeAddr - vmeMap;

	CHK( (rv = VME4L_Map( G_fd, vmeMap, vmeOff + M99_A201_REG_SIZE,
						  (void**)&map )) == 0 );
	G_m99Base = (void *)(((char*)map) + vmeOff);
	printf("vmeMap=%llx m99Base=%p\n", vmeMap, G_m99Base );

	/* install signal for M99 interrupt (don't enable irq now) */
	signal( M99_IRQ_SIG, M99SigHandler ); /* catch sig 33 (typical) */
	signal( SIGINT, SigIntHandler );

	CHK( VME4L_SigInstall( G_fd, vector, level, M99_IRQ_SIG,
						   VME4L_IRQ_NOFLAGS ) == 0 );

	/*--- init A201/M99 ---*/
	A201M99_Init( vector, level );

	/* enable VME interrupt level */
	CHK( VME4L_IrqEnable( G_fd, level ) == 0);

	while(!G_abort){
		sleep(1);
		
		printf("signal latency: Min: %d, Max: %d us, ints: %d\n",
			   G_minLatency, G_maxLatency, G_irqCount );

	}
	printf("RESULT: Min: %d, Max: %d us, ints: %d\n",
		   G_minLatency, G_maxLatency, G_irqCount );
	MWRITE_D16(G_m99Base, 0x100, 0x00 ); /* disable A201 level */

	return 0;

 ABORT:
	return 1;
}

int A201M99_Init( int vector, int level)
{
	void *m99Base, *a201Base;
	uint32_t timerval = G_timer;

	m99Base = G_m99Base;
	a201Base = (void *)((char *)m99Base + 0x100);

	/* init M99 */
	MWRITE_D16( m99Base, 0x00, 0 );

    /* setup MC68230 */
    MWRITE_D16( m99Base, PGC_REG,  0x00 );
    MWRITE_D16( m99Base, PAD_REG,  0xaa );
    MWRITE_D16( m99Base, PADD_REG, 0xff );
    MWRITE_D16( m99Base, PBDD_REG, 0x00 );
    MWRITE_D16( m99Base, PAC_REG,  0x00 );
    MWRITE_D16( m99Base, PBC_REG,  0x00 );
    MWRITE_D16( m99Base, TIV_REG,  0x0f );

	/* setup timer */
    MWRITE_D16( m99Base, CPL_REG, timerval       & 0xff);
    MWRITE_D16( m99Base, CPM_REG, timerval >> 8  & 0xff);
    MWRITE_D16( m99Base, CPH_REG, timerval >> 16 & 0xff);
	
	MWRITE_D16( m99Base, TC_REG, 0x80); /* timer irq (disabled) */
	
	/* setup A201 */
	MWRITE_D16( a201Base, 0x02, vector );
	MWRITE_D16( a201Base, 0x00, 0x10 | level ); /* enable irq on board */

	/* enable timer interrupt */
	MWRITE_D16( m99Base, TC_REG, 0xa1 );

	return 0;
}

void M99Int()
{
	uint16_t tcReg;
	uint16_t irqCtrl;
	uint32_t timerval;
	static uint8_t pat=0;
	int latency;
	void *m99Base = G_m99Base;
	void *a201Base = (void *)((char *)m99Base + 0x100);
	
	if( !(MREAD_D16( m99Base, TS_REG) & 0x1) )
		return;					/* no interrupt pending */

	MWRITE_D16( m99Base, TS_REG, 0xff );		/* clear interrupt in M99 */
	irqCtrl = MREAD_D16( a201Base, 0x00 );
	MWRITE_D16( a201Base, 0x00, 0x80|irqCtrl ); /* clear interrupt in A203N */
	
	G_irqCount++;
	/* read timer to measure interrupt latency */
	tcReg = MREAD_D16( m99Base, TC_REG );      		/* get timer control */
	MWRITE_D16( m99Base, TC_REG, tcReg & 0xfe);		/* timer halt */

	timerval = (MREAD_D16( m99Base, CNTH_REG ) & 0xff) << 16;
	timerval |= (MREAD_D16( m99Base, CNTM_REG ) & 0xff) << 8;
	timerval |= (MREAD_D16( m99Base, CNTL_REG ) & 0xff);

	latency = (G_timer-timerval) * 4;	/* 4us/tick */

	if( G_irqCount > 1 ){
		if( latency > G_maxLatency ){
			G_maxLatency = latency;
		}
		if( latency < G_minLatency ){
			G_minLatency = latency;
		}
	}
	MWRITE_D16( m99Base, PAD_REG, ~(pat++) );
	MWRITE_D16( m99Base, TC_REG, tcReg );
}

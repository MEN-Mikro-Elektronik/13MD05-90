/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  chameleonload_a015.c
 *
 *      \author  ulf.franke@men.de
 *        $Date: 2006/05/30 08:30:58 $
 *    $Revision: 1.4 $
 *
 *	   \project  A015
 *  	 \brief  load routine for FPGA's on A15
 *               We have the VME-Bus FPGA and the M-Module FPGA configuration
 *               lines on the same register.
 *
 *    \switches   _LITTLE_ENDIAN_ / _BIG_ENDIAN_, MENMON
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: chameleonload_a015.c,v $
 * Revision 1.4  2006/05/30 08:30:58  ufranke
 * fixed
 *  - FPGA config line is undefined after reset, set it to a defined state first
 *
 * Revision 1.3  2006/01/20 10:23:31  ag
 * fixed: ChameleonLoad_A015 caused exception if Chameleon-Code was absent
 * removed unused variable: MAIN_MenmonGlobalsP
 *
 * Revision 1.2  2003/08/06 13:01:15  kp
 * changed file header for doxygen
 *
 * Revision 1.1  2003/06/10 14:07:11  UFranke
 * Initial Revision
 *
 * clone from chameleonloadem04.c Revision 1.2  2002/09/12 13:46:59
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#ifdef MENMON
# include <mmglob.h>			/* for strncmp */
#else
# include <string.h>
#endif

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/maccess.h>
#include <MEN/fpga_header.h>
#include <MEN/chameleon.h>


/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#ifdef MENMON
# include <mmglob.h>			/* for strncmp */
#else
# include <string.h>
#endif

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/maccess.h>
#include <MEN/fpga_header.h>
#include <MEN/chameleon.h>


/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
/*--- RCS2 latch defs ---*/
#define A15_LOAD_LATCH_ADRS 0x70000000 /* address of latch (I2C, FPGA load) */

#define A15_LOAD_LATCH_FPGA_CONF_STATUS_1	0x80 /* r */
#define A15_LOAD_LATCH_FPGA_INIT_DONE_1		0x40 /* r */
#define A15_LOAD_LATCH_FPGA_CONFIG_1		0x80 /* w */
#define A15_LOAD_LATCH_FPGA_CLK_1			0x40 /* w */
#define A15_LOAD_LATCH_FPGA_DATA_1			0x20 /* w */

#define A15_LOAD_LATCH_FPGA_CONF_STATUS_2	0x20 /* r */
#define A15_LOAD_LATCH_FPGA_INIT_DONE_2		0x10 /* r */
#define A15_LOAD_LATCH_FPGA_CONFIG_2		0x10 /* w */
#define A15_LOAD_LATCH_FPGA_CLK_2			0x08 /* w */
#define A15_LOAD_LATCH_FPGA_DATA_2			0x04 /* w */

/* 8245 bridge defs */
#define MPC106_PICR1		(0xa8 | OSS_PCI_ACCESS_32)
#define PICR1_FLASH_WR_EN	0x00001000	/* Flash writes enable */

#ifdef _LITTLE_ENDIAN_
# define SWAPWORD(w) (w)
# define SWAPLONG(l) (l)
#elif defined(_BIG_ENDIAN_)

# define SWAPWORD(w) ((((w)&0xff)<<8) + (((w)&0xff00)>>8))

# define SWAPLONG(l) ((((l)&0xff)<<24) + (((l)&0xff00)<<8) + \
					 (((l)&0xff0000)>>8) + (((l)&0xff000000)>>24))

#else
# error "Must define _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/**********************************************************************/
/** routine to load the FPGA code into the EM04's chameleon FPGA
 *
 *  This routine (re)loads the FPGA on EM04 with the code specified
 *  with \a fpgaCode.
 *
 *  The FPGA header magic and the FPGA code's XOR checksum is checked
 *
 *  This routine can be called without calling ChameleonInit() before
 *  loading.
 *
 *  Since this routine accesses the RCS2 latch, the calling routine must
 *  allow (setup MMU or BATs) access to the latch.
 *
 *  OSS_MikroDelayInit() must have been called before.
 * I
 *  \param osh			OSS handle
 *  \param icNumber		A015_PCI2VME_FPGA_IC or A015_PCI2MMOD_FPGA_IC
 *  \param fpgaCode		FPGA header followed by code (generated with
 *						fpga_addheader)
 *
 *  \return 0=ok, or error number CHAMELEON_xxx
 */
int32 ChameleonLoad_A015
(
	OSS_HANDLE *osh,
	int        icNumber,
	const FPGA_HEADER *fpgaCode
)
{
	register u_int8 ch;
	register u_int8 latchcfg;
	register u_int8 latchcfg_clk;
	register u_int32 n;
	u_int32 picr1=0xffffffff;
	u_int32 size,cnt=0;
	MACCESS ma_fpga = (MACCESS)A15_LOAD_LATCH_ADRS;
	MACCESS ma_data = (MACCESS)(fpgaCode + 1);
	int32 error=0;
	register u_int8 statusMsk;
	register u_int8 doneMsk;
	register u_int8 configMsk;
	register u_int8 clockMsk;
	register u_int8 dataMsk;
	register u_int8 otherConfigMsk;
	register u_int8 status;

	/* get size */
	size = fpgaCode->size;
	
	/*-----------------------+
	|  Allow writes to RCS2  |
	+-----------------------*/
	/* enable latch/flash writes */
	OSS_PciGetConfig(osh, 0, 0, 0, MPC106_PICR1, (int32 *)&picr1);
	OSS_PciSetConfig(osh, 0, 0, 0, MPC106_PICR1, picr1 | PICR1_FLASH_WR_EN);
	
	/*------------------------------+
	|  checking FPGA code in flash  |
	+-------------------------------*/
	if (fpgaCode->magic != FPGA_HEADER_MAGIC) /* checking magic word */
	{
		error = CHAMELEON_CODE_ABSENT;
		goto errexit;
	}
	else
	{
		/* checking FPGA type */
		if( (icNumber == A015_PCI2VME_FPGA_IC &&
		     !(strncmp(A015_PCI2VME_FPGA_TYPE,(char*)fpgaCode->fileName,9)))
		    ||
		    (icNumber == A015_PCI2MMOD_FPGA_IC &&
		     !(strncmp(A015_PCI2MMOD_FPGA_TYPE,(char*)fpgaCode->fileName,9)))
		  )
		{
			/* checking XOR checksum over FPGA data */
		    u_int32 i;
        	u_int32 *p = (u_int32 *)ma_data;
        	u_int32 xor;

	        for( xor=0,i=0; i<(size/4); i++ )
           		xor ^= *p++;

			if ( xor != fpgaCode->chksum)
			{
				error = CHAMELEON_CODE_CHKSUM_ERR;
				goto errexit;
			}
		}
		else
		{
			error = CHAMELEON_CODE_MISMATCH;
			goto errexit;
		}/*if*/
	}/*if*/

	/*-----------------------------+
	| config pins                  |
	+-----------------------------*/
	switch( icNumber )
	{
		case A015_PCI2VME_FPGA_IC:
			statusMsk	= A15_LOAD_LATCH_FPGA_CONF_STATUS_1;
			doneMsk		= A15_LOAD_LATCH_FPGA_INIT_DONE_1;
			configMsk	= A15_LOAD_LATCH_FPGA_CONFIG_1;
			clockMsk	= A15_LOAD_LATCH_FPGA_CLK_1;
			dataMsk		= A15_LOAD_LATCH_FPGA_DATA_1;
			otherConfigMsk = A15_LOAD_LATCH_FPGA_CONFIG_2;
			break;

		case A015_PCI2MMOD_FPGA_IC:
			statusMsk	= A15_LOAD_LATCH_FPGA_CONF_STATUS_2;
			doneMsk		= A15_LOAD_LATCH_FPGA_INIT_DONE_2;
			configMsk	= A15_LOAD_LATCH_FPGA_CONFIG_2;
			clockMsk	= A15_LOAD_LATCH_FPGA_CLK_2;
			dataMsk		= A15_LOAD_LATCH_FPGA_DATA_2;
			otherConfigMsk = A15_LOAD_LATCH_FPGA_CONFIG_1;
			break;

		default:
			error = CHAMELEON_CODE_MISMATCH;
			goto errexit;
	}/*switch*/
	
	/*--------------------------------+
	| Load FPGA code                  |
	+---------------------------------*/
	latchcfg = configMsk | otherConfigMsk;

	/* set CONFIG low - because state is undefined after reset */
	MWRITE_D8(ma_fpga,0,(latchcfg & ~configMsk));
	OSS_MikroDelay( osh, 500 );

	/* set config to HIGH */
	MWRITE_D8(ma_fpga,0,latchcfg);
	OSS_MikroDelay( osh, 500 );

	/* status must be HIGH */
	status = MREAD_D8(ma_fpga,0);
	if( !(status & statusMsk) )
	{
		error = CHAMELEON_CONF_STAT_ERR;
		goto errexit;
	}

	/* set CONFIG low */
	MWRITE_D8(ma_fpga,0,(latchcfg & ~configMsk));
	OSS_MikroDelay( osh, 20 );	/* Tcfg > 8 us */

	/* status must be LOW */
	status = MREAD_D8(ma_fpga,0);
	if ( (status & statusMsk) )
	{
		error = CHAMELEON_CONF_STAT_ERR;
		goto errexit;
	}

	/* set CONFIG high */
	MWRITE_D8(ma_fpga,0,latchcfg);
	OSS_MikroDelay( osh, 30 );	

	/* status must be HIGH */
	if ( !(MREAD_D8(ma_fpga,0) & statusMsk) )
	{
		error = CHAMELEON_CONF_STAT_ERR;
		goto errexit;
	}
	OSS_MikroDelay( osh, 20 );	
	
	/* transfer code */
	while (size--)
	{
		ch = MREAD_D8( ma_data, cnt++ );
		n = 8;
		
		while(n--) 							/* for all bits (0..7) */
		{
			if( ch & 1 )
				latchcfg |= dataMsk;
			else
				latchcfg &= ~dataMsk;
			
			/* build package CONFIG = HIGH, CLOCK = LOW, DATA = 0|1 */							
			MWRITE_D8( ma_fpga, 0, latchcfg );

			/* build package CONFIG = HIGH, CLOCK = HIGH, DATA = 0|1 */
			latchcfg_clk = latchcfg | clockMsk;
			MWRITE_D8( ma_fpga, 0, latchcfg_clk );

			/* next bit */
			ch >>= 1;
		}/*while*/

		/* check CONF_STATUS every 256 bytes */
		if( !(size & 0xff) )
		{
			if ( !(MREAD_D8(ma_fpga,0) & statusMsk) )
			{
				error = CHAMELEON_LOAD_ERR;
				goto errexit;
			}
		}
	}/* end while */

	if ( !(MREAD_D8(ma_fpga,0) & statusMsk) )
	{
		error = CHAMELEON_LOAD_ERR;
		goto errexit;
	}

	/* 10 additional clocks for ACEX 1K*/
	n = 10;
	latchcfg |= dataMsk;
	while(n--)
	{
		/* build package CONFIG = HIGH, CLOCK = LOW, DATA = 1 */							
		MWRITE_D8( ma_fpga, 0, latchcfg );

		/* build package CONFIG = HIGH, CLOCK = HIGH, DATA = 1 */
		latchcfg_clk = latchcfg | clockMsk;
		MWRITE_D8( ma_fpga, 0, latchcfg_clk );
	}/*while*/

	/* check if init done */
	status = MREAD_D8(ma_fpga,0);
	if( !(status & doneMsk) )
	{
		error = CHAMELEON_INIT_DONE_ERR;
		goto errexit;
	}

errexit:
		/* set config to high, clock to low always */
		MWRITE_D8( ma_fpga, 0,
					A15_LOAD_LATCH_FPGA_CONFIG_1 | A15_LOAD_LATCH_FPGA_CONFIG_2 );
	
		/* disable latch/flash writes */
		if( picr1 != 0xffffffff )
			OSS_PciSetConfig(osh, 0, 0, 0, MPC106_PICR1, picr1);
	return error;
}



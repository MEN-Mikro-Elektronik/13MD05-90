/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  chameleonloadem04.c
 *
 *      \author  klaus.popp@men.de/rene.lange@men.de
 *        $Date: 2005/06/23 16:12:07 $
 *    $Revision: 1.9 $
 *
 *	   \project  EM04
 *  	 \brief  load routine for FPGA on EM04
 *
 *    \switches   _LITTLE_ENDIAN_ / _BIG_ENDIAN_, MENMON
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: chameleonloadem04.c,v $
 * Revision 1.9  2005/06/23 16:12:07  kp
 * Copyright line changed (sbo)
 *
 * Revision 1.8  2005/05/04 08:59:44  kp
 * support long FPGA header format
 *
 * Revision 1.7  2004/12/20 08:46:28  kp
 * changed ChameleonFpgaCodeIdentEM04 (use fpgaheader.c functions instead)
 *
 * Revision 1.6  2004/04/30 16:21:15  kp
 * added ChameleonFpgaCodeIdentEm04 to identify flash code without loading it
 *
 * Revision 1.5  2003/08/06 13:01:13  kp
 * changed file header for doxygen
 *
 * Revision 1.4  2003/07/10 14:08:00  rla
 * config status waiting time adapted to CYCEP1C12 FPGA
 *
 * Revision 1.3  2003/05/06 11:34:53  kp
 * Bug fix: goto errexit when FPGA file does not begin with "EM04"
 *
 * Revision 1.2  2002/09/12 13:46:59  kp
 * 1) Pass OSS handle to all OSS functions
 * 2) Now automatically switching PICR1 to allow flash writes
 *
 * Revision 1.1  2002/09/11 10:13:31  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002-2003 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
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
|  DEFINES                                 |
+------------------------------------------*/
/*--- RCS2 latch defs ---*/
#define EM04LOAD_LATCH_ADRS 0x70000000 /* address of latch (I2C, FPGA load) */

#define EM04LOAD_LATCH_FPGA_CONF_STATUS	0x04 /* r */
#define EM04LOAD_LATCH_FPGA_INIT_DONE	0x02 /* r */

#define EM04LOAD_LATCH_COM1SW			0x10 /* r/w */
#define EM04LOAD_LATCH_FPGA_CONFIG		0x80 /* w */
#define EM04LOAD_LATCH_FPGA_CLK			0x40 /* w */
#define EM04LOAD_LATCH_FPGA_DATA		0x20 /* w */
#define EM04LOAD_LATCH_SDA_OUT			0x08 /* w */
#define EM04LOAD_LATCH_SCL				0x04 /* w */

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
/** identify FPGA code in memory, check everything but code XOR
 *
 *  \deprecated. Use ChameleonFpgaHeaderIdent instead
 *
 *  Also compute variant code and revision code from FPGA code name
 *
 *  \param fpgaCode		FPGA header followed by code (generated with
 *						fpga_addheader)
 *  \param variantP		if non-NULL, receives the variant from FPGA name (A-Z)
 *						-1 if invalid.
 *  \param revisionP	if non-NULL, receives the revision from FPGA
 *						name (0-9), -1 if invalid
 *
 *  \return 0=ok, or error number CHAMELEON_xxx
 */
int32 ChameleonFpgaCodeIdentEm04(
	const FPGA_HEADER *fpgaCode,
	int *variantP,
	int *revisionP)
{
	FPGA_HEADER_INFO info;
	int32 error;

	if( (error = ChameleonFpgaHeaderIdent( fpgaCode, 0x100000, &info )) == 0 ){
		if( variantP )
			*variantP = info.variant;
		if( revisionP )
			*revisionP = info.revision;

	}
	return error;
}	

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
 *
 *  \param osh			OSS handle
 *  \param fpgaCode		FPGA header followed by code (generated with
 *						fpga_addheader)
 *
 *  \return 0=ok, or error number CHAMELEON_xxx
 */
int32 ChameleonLoadEm04(OSS_HANDLE *osh, const FPGA_HEADER *fpgaCode)
{
	register u_int8 ch, latchcfg;
	register u_int32 n;
	u_int32 picr1=0xffffffff;
	u_int32 size,cnt=0;
	MACCESS ma_fpga = (MACCESS)EM04LOAD_LATCH_ADRS;
	MACCESS ma_data;
	int32 error=0;
	FPGA_HEADER_INFO info;

	/* check if header valid */
	if( (error = ChameleonFpgaHeaderIdent( fpgaCode, 0x100000, &info )) != 0 )
		return error;

	/* check code XOR */
	if( (error = ChameleonFpgaDataCheckXor( &info ) ) != 0 )
		return error;

	/* get size and start of netto data */
	size = info.size;
	ma_data = (MACCESS)info.data;

	/*-----------------------+
	|  Allow writes to RCS2  |
	+-----------------------*/
	OSS_PciGetConfig(osh, 0, 0, 0, MPC106_PICR1, (int32 *)&picr1);
	OSS_PciSetConfig(osh, 0, 0, 0, MPC106_PICR1, picr1 | PICR1_FLASH_WR_EN);

	/*--------------------------------+
	| Load FPGA code                  |
	+---------------------------------*/
	latchcfg = EM04LOAD_LATCH_FPGA_CONFIG |
		(MREAD_D8(ma_fpga,0) & EM04LOAD_LATCH_COM1SW) |
		EM04LOAD_LATCH_SDA_OUT |
		EM04LOAD_LATCH_SCL;

	/* set config to HIGH */
	MWRITE_D8(ma_fpga,0,latchcfg);

	/* pulse CONFIG low */
	MWRITE_D8(ma_fpga,0,(latchcfg & ~EM04LOAD_LATCH_FPGA_CONFIG));
	OSS_MikroDelay( osh, 50 );

	/* check if status is LOW */
	OSS_MikroDelay( osh, 10 );	/* delay a bit. This makes the timing */
	if ( (MREAD_D8(ma_fpga,0) & EM04LOAD_LATCH_FPGA_CONF_STATUS) )
	{
		error = CHAMELEON_CONF_STAT_ERR;
		goto errexit;
	}
	MWRITE_D8(ma_fpga,0,latchcfg);

	/* check if status is HIGH */
	OSS_MikroDelay( osh, 30 );	/* delay a bit. This makes the timing */
	if ( !(MREAD_D8(ma_fpga,0) & EM04LOAD_LATCH_FPGA_CONF_STATUS) )
	{
		error = CHAMELEON_CONF_STAT_ERR;
		goto errexit;
	}


	/* transfer code */
	while (size--)
	{
		ch = MREAD_D8(ma_data,cnt++);
		n = 8;

		while(n--) 							/* for all bits (0..7) */
		{
			/* build package CONFIG = HIGH, CLOCK = LOW, DATA = 0|1 */
			MWRITE_D8(ma_fpga,0,(latchcfg |
								 ((ch&0x01)*EM04LOAD_LATCH_FPGA_DATA)));
			/* build package CONFIG = HIGH, CLOCK = HIGH, DATA = 0|1 */
			MWRITE_D8(ma_fpga,0,(latchcfg | EM04LOAD_LATCH_FPGA_CLK |
				  	((ch&0x01)*EM04LOAD_LATCH_FPGA_DATA)));
			ch >>= 1;						/* next bit */
		}/* end while */

		/* check CONF_STATUS every 256 bytes */
		if (! (size & 0xff))
		{
			if ( !(MREAD_D8(ma_fpga,0) & EM04LOAD_LATCH_FPGA_CONF_STATUS) )
			{
				error = CHAMELEON_LOAD_ERR;
				goto errexit;
			}
		}
	}/* end while */

	/* check CONF_STATUS at the end */
	if (! (size & 0xff))
	{
		if ( !(MREAD_D8(ma_fpga,0) & EM04LOAD_LATCH_FPGA_CONF_STATUS) )
		{
			error = CHAMELEON_LOAD_ERR;
			goto errexit;
		}/* end if */
	}/* end if */

	/* wait for init done */
	OSS_MikroDelay( osh, 20 );	/* delay a bit. This makes the timing */

	/* check if init done */
	if ( !(MREAD_D8(ma_fpga,0) & EM04LOAD_LATCH_FPGA_INIT_DONE) )
	{
		error = CHAMELEON_INIT_DONE_ERR;
		goto errexit;
	}

errexit:
	/* disable flash writes */
	if( picr1 != 0xffffffff )
		OSS_PciSetConfig(osh, 0, 0, 0, MPC106_PICR1, picr1);
	return error;
}

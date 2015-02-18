/*********************  P r o g r a m  -  M o d u l e *************************/
/*!
 *         \file usmrw.c
 *      Project: common lib for m-modules usm
 *
 *       \author christian.kauntz@men.de
 *        $Date: 2008/09/18 17:28:59 $
 *    $Revision: 1.4 $
 *
 *        \brief Handling USModule-Identification (EEPROM)
 *               I2C Protocol
 *
 *     Required: none
 *     Switches: none
 */
 /*---------------------------[ Public Functions ]-------------------------------
 *
 * int usm_mread(addr,buff)            multiple read i=0..128
 * int usm_mwrite(addr,buff)           multiple write i=0..128
 * int usm_read(addr,index)            single read i
 * int usm_write(addr,index,data)      single write i
 *
 *
 *-------------------------------[ History ]------------------------------------
 *
 * $Log: usmrw.c,v $
 * Revision 1.4  2008/09/18 17:28:59  CKauntz
 * R1: No doxygen documentation
 * R2: No support for 64 bit OS
 * M1: Changed comments for doxygen documentation
 * M2: Changed base to U_INT32_OR_64 to support 64 bit OS
 *
 * Revision 1.3  2008/09/09 15:37:54  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 1.2  2007/08/09 10:26:20  CKauntz
 * cosmetics
 *
 * Revision 1.1  2007/08/09 09:16:00  CKauntz
 * Initial Revision
 *
 *
 *------------------------------------------------------------------------------
 * (c) Copyright 2007..2008 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/

static const char RCSid[]="$Id: usmrw.c,v 1.4 2008/09/18 17:28:59 CKauntz Exp $";

/*--------------------------------------+
|   INCLUDES                            |
+--------------------------------------*/
/* swapped access */
#ifdef ID_SW
#	define MAC_MEM_MAPPED
#	define MAC_BYTESWAP
#endif

#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/maccess.h>
#include <MEN/modcom.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#define DELAY   		60    	/* usm_clock's delay time */

/* id defines */
#define USM_ID_MAGIC	0x5553  /* USM id prom magic word */

/*--- instructions for serial EEPROM ---*/
#define _READ_USM		0xAF	/* Memory Area read */
#define _WRITE_USM		0xAE	/* Memory Area write */

/* bit definition USM*/
#define B_DAT			0x08	/* data in-;output		*/
#define B_CLK			0x10	/* clock				*/
#define B_SEL			0x20	/* chip-select			*/

/* A08 register address */
#define MODREG  		0xfe	/* ID-Register for M-Module and USM */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
int usm_mread( u_int8   *addr, u_int16  *buff );
int usm_mwrite( u_int8  *addr, u_int16 *buff );
int usm_write( u_int8 *addr, u_int8  index, u_int16 data );
int usm_read( U_INT32_OR_64 base, u_int8 index );
static void _opcode( U_INT32_OR_64 base, u_int8 code );
static void _start( U_INT32_OR_64 base );
static void _stop( U_INT32_OR_64 base );
static void _select( U_INT32_OR_64 base );
static void _deselect( U_INT32_OR_64 base );
static int  _clock( U_INT32_OR_64 base, u_int8 dbs , u_int8 lastdbs);
static void _delay( void );

/******************************* usm_mread ************************************/
/** Read all contents (words 0..128) from EEPROM at 'base'.
 *
 *------------------------------------------------------------------------------
 *  \param  addr   \IN  base address pointer
 *  \param  buff   \OUT  user buffer (128 words)
 *  \return 0=ok, 1=error
 *
 ******************************************************************************/
int usm_mread( u_int8   *addr, u_int16  *buff )
{

    register u_int8    index;

    for(index=0; index<128; index++)
        *buff++ = (u_int16)usm_read( (U_INT32_OR_64)addr, index);
    return 0;
}

/******************************* usm_mwrite ***********************************/
/** Write all contents (words 0..128) into EEPROM at 'base'.
 *
 *------------------------------------------------------------------------------
 *  \param addr   \IN base address pointer
 *  \param buff   \IN user buffer (128 words)
 *  \return 0=ok, 1=error
 *
 ******************************************************************************/
int usm_mwrite( u_int8  *addr, u_int16 *buff )
{
    register u_int8    index;

    for(index=0; index<128; index++)
        if( usm_write(addr,index,*buff++) )
            return 1;
    return 0;
}


/******************************* usm_write ************************************/
/** Write a specified word into EEPROM at 'base'.
 *
 *------------------------------------------------------------------------------
 *  \param addr   \IN base address pointer
 *  \param index  \IN index to write (0..128)
 *  \param data   \IN word to write
 *  \return   0=OK, 1..4=error
 *
 ******************************************************************************/
int usm_write( u_int8 *addr, u_int8  index, u_int16 data )
{
    register int        i;                  		/* counter      		*/
	register u_int8 	offset;						/* offset of the data 	*/

	offset = index *2;								/* word size			*/

  	_select((U_INT32_OR_64)addr);							/* select B_SEL line 	*/
    _start((U_INT32_OR_64)addr);							/* start condition 		*/
   	_opcode((U_INT32_OR_64) addr, (u_int8)(_WRITE_USM) );	/* opcode for write		*/
	if(	_clock((U_INT32_OR_64)addr, 0,0) != 0)			/* wait for acknowledge */
   		return 0x1;
	/* write address */
	for( i=7; i>=0; i-- )							/* send address 		*/

	_clock((U_INT32_OR_64)addr,(u_int8)((offset>>i)&0x01),(u_int8)((offset>>(i+1))&0x01));
 	if (_clock((U_INT32_OR_64)addr, 1 ,(u_int8)(offset&0x01))!= 0)	/* wait for acknowledge */
  		return 0x2;
	/* send first byte of the word */
    for( i=15; i>=8; i--)							/* send data at address */
  		_clock((U_INT32_OR_64)addr, (u_int8)((data>>i)&0x01),(u_int8)((data>>(i+1))&0x01));
	if(_clock((U_INT32_OR_64)addr,1,(u_int8)(data&0x01)) != 0)		/* wait for acknowledge */
		return 0x3;
	/* send second byte of the word */
	for( i=7; i>=0; i--)							/* send data at address */
 		_clock((U_INT32_OR_64)addr, (u_int8)((data>>i)&0x01),(u_int8)((data>>(i+1))&0x01));
	if(_clock((U_INT32_OR_64)addr,1,(u_int8)(data&0x01)) != 0)		/* wait for acknowledge */
		return 0x4;
	_stop((U_INT32_OR_64)addr);							/* stop condition 		*/
  	_deselect((U_INT32_OR_64)addr);						/* deselect B_SEL line 	*/

	return 0x0;
}

/******************************* usm_read *************************************/
/** Read a specified word from EEPROM at 'base'.
 *
 *------------------------------------------------------------------------------
 *  \param  base   \IN base address pointer
 *  \param index   \IN index to read (0..128)
 *  \return read word or error
 *
 ******************************************************************************/
int usm_read( U_INT32_OR_64 base, u_int8 index )
{
    register u_int16    wx;                 /* data word    				*/
    register int        i;                  /* counter      				*/
	register u_int8 	offset;				/* offet of the data 			*/

	offset = index *2;						/* word size					*/

   	_select(base);							/* select B_SEL line 			*/
    _start(base);							/* start condition 				*/
  	_opcode(base, (u_int8)(_WRITE_USM) );	/* opcode for write 			*/
	if (_clock(base, 1,0)!= 0)				/* wait for acknowledge 		*/
		return 0x1;

	/* write address */
	for( i=7; i>=0; i-- )					/* send address to be read from */
  		_clock(base,(u_int8)((offset>>i)&0x01),0);
	if( _clock(base,1,1)!= 0)				/* wait for acknowledge 		*/
		return 0x2;
 	_start(base);							/* start condition 				*/
  	_opcode(base, (u_int8)(_READ_USM) );	/* opcode for read 				*/
	if( _clock(base,1,1)!= 0)				/* wait for acknowledge 		*/
		return 0x3;

	/* read first byte of the word */
    for(wx=0, i=0; i<8; i++)				/* read EEPROM data 			*/
  		wx = (u_int16)((wx<<1)+_clock(base,1,1));
	_clock(base,0,0);						/* set acknowledge 				*/

	/* read second byte of the word */
    for(i=0; i<8; i++)						/* read EEPROM data 			*/
  		wx = (u_int16)((wx<<1)+_clock(base,1,1));
	_clock(base,1,1);						/* no acknowledge 				*/
   	_stop(base);							/* stop condition 				*/
 	_deselect(base);						/* deselect B_SEL line 			*/

 return(wx);
}

/******************************* _opcode **************************************/
/** Output opcode
 *
 *------------------------------------------------------------------------------
 *  \param  code    \IN opcode to write
 *
 ******************************************************************************/
static void _opcode( U_INT32_OR_64 base, u_int8 code )
{
    register int i;

    for(i=7; i>=0; i--)						/* output instruction code  	*/
        _clock(base,(u_int8)((code>>i)&0x01),(u_int8)((code>>(i+1))&0x01) );
}


/*----------------------------------------------------------------------
 * LOW-LEVEL ROUTINES FOR SERIAL EEPROM
 *--------------------------------------------------------------------*/

/******************************* _select **************************************/
/** Select EEPROM: output DI/CLK/CS low
 *                 delay
 *                 output CS high
 *                 delay
 *------------------------------------------------------------------------------
 *  \param base \IN base address pointer
 *
 ******************************************************************************/
static void _select( U_INT32_OR_64 base )
{
    MWRITE_D16( base, MODREG, 0 );					/* everything inactive 	*/
    _delay();
    MWRITE_D16( base, MODREG, (1<<3)|B_CLK|B_SEL );	/* select high 			*/
    										 		/* data bit high 		*/
    _delay();										/* delay 				*/
}

/******************************* _deselect ************************************/
/** Deselect EEPROM: output CS low
 *------------------------------------------------------------------------------
 *  \param base \IN base address pointer
 *
 ******************************************************************************/
static void _deselect( U_INT32_OR_64 base ) /* nodoc */
{
    MWRITE_D16( base, MODREG, 0 );					/* everything inactive 	*/
}

/******************************* _clock ***************************************/
/** Output data bit:
 *                 output clock low
 *                 output lastdata bit high/low
 *                 delay
 *				   output clock low
 *                 output data bit hith/low
 *                 delay
 *                 output clock high
 *                 output data bit high/low
 *                 delay
 *                 return state of data serial eeprom's SDA - line
 *                 (Note: keep CS asserted)
 *------------------------------------------------------------------------------
 *  \param base    \IN base address pointer
 *  \param dbs	   \IN data bit to send
 *  \param lastdbs \IN previus data bit to send
 *  \return current data bit
 *
 ******************************************************************************/
static int _clock( U_INT32_OR_64 base, u_int8 dbs, u_int8 lastdbs )
{
	MWRITE_D16( base, MODREG, (lastdbs<<3)|B_SEL ); /* output clock low 	*/
                                            		/* output data high/low */
    _delay();                          				/* delay    			*/
	MWRITE_D16( base, MODREG, (dbs<<3)|B_SEL ); 	/* output clock low 	*/
                                            		/* output data high/low */
    _delay();                              			/* delay    			*/
   MWRITE_D16( base, MODREG, (dbs<<3)|B_CLK|B_SEL );  /* output clock high */
    _delay();                               		/* delay    			*/

    return((MREAD_D16( base, MODREG) & B_DAT )>>3);	/* get data bit 		*/
}

/******************************* _start ***************************************/
/** Output data bit:
 *                 output clock low
 *                 output data bit high
 *                 delay
 *                 output clock high
 *                 output data bit high
 *                 delay
 *				   output clock high
 *                 output data bit low
 *                 delay
 *                 return state of data serial eeprom's SDA - line
 *                 (Note: keep CS asserted)
 *------------------------------------------------------------------------------
 *  \param base \IN base address pointer
 *
 ******************************************************************************/
static void _start( U_INT32_OR_64 base ) 				
{
	MWRITE_D16( base, MODREG, (1<<3)|B_SEL );  	/* output clock hihg */
                                            	/* output data low */
    _delay();                               	/* delay    */
    _delay();                               	/* delay    */
    MWRITE_D16( base, MODREG, (1<<3)|B_CLK|B_SEL );  	/* output clock hihg */
                                            	/* output data low */
    _delay();                               	/* delay    */
    _delay();                               	/* delay    */
    MWRITE_D16( base, MODREG, B_CLK|B_SEL );  	/* output clock hihg */
                                            	/* output data low */
    _delay();                               	/* delay    */
    _delay();                               	/* delay    */

}
/******************************* _stop ****************************************/
/** Output data bit:
 *                 output clock low
 *                 output data bit low
 *                 delay
 *                 output clock high
 *                 output data bit low
 *                 delay
 *                 output clock high
 *                 output data bit high
 *                 delay
 *                 output clock low
 *                 output data bit high
 *                 delay
 *                 (Note: keep CS asserted)
 *------------------------------------------------------------------------------
 *   \param base \IN base address pointer
 *
 ******************************************************************************/
static void _stop( U_INT32_OR_64 base ) 				
{
    MWRITE_D16( base, MODREG, B_SEL );  		/* output data/clock low */
    _delay();                               	/* delay    */
    MWRITE_D16( base, MODREG, B_CLK|B_SEL );  	/* output clock high */
                                            	/* output data low */
    _delay();                               	/* delay    */
    MWRITE_D16( base, MODREG, (1<<3)|B_CLK|B_SEL );  /* output data/clock high */
    _delay();                               	/* delay    */
    MWRITE_D16( base, MODREG, (1<<3)|B_SEL );  	/* output data high */
    _delay();                               	/* delay    */
}

/******************************* _delay ***************************************/
/** Delay (at least) one microsecond
 *------------------------------------------------------------------------------
 *
 ******************************************************************************/
static void _delay( void )
{
    register volatile int i,n;

  	for(i=DELAY; i>0; i--)
        n=10*10;
}


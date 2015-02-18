/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: flex10k.c
 *      Project: PLD loader library
 *
 *       Author: see
 *        $Date: 2008/09/05 15:37:08 $
 *    $Revision: 1.6 $
 *
 *  Description: Loader for ALTERA FLEX 8000/10k PLD's
 *               (PS Passive Serial Interface)
 *
 *               The PLD interface contains two loader functions:
 *
 *                   PLD_FLEX10K_LoadCallBk (callback i/o)
 *                       This function is for complicated PLD interfaces,
 *                       where PLD bits are located in different registers.
 *
 *                   PLD_FLEX10K_LoadDirect (direct access)
 *                       This function is for simple PLD interfaces,
 *                       where all PLD bits are located in one register.
 *
 *               PLD data can be loaded at once or blockwise depending on
 *               passed 'flags':
 *
 *                   PLD_FIRSTBLOCK  = perform PLD initialization before loading
 *                   PLD_LASTBLOCK   = perform PLD termination after loading
 *
 *               a) load data at once
 *               - load full block with flags=PLD_FIRSTBLOCK+PLD_LASTBLOCK
 *
 *               b) load data blockwise
 *               - load first block with flags=PLD_FIRSTBLOCK
 *               - load n-blocks    with flags=0
 *               - load last block  with flags=PLD_LASTBLOCK
 *
 *               The PLD data array passed to the loader function
 *               must contain the PLD data as raw byte stream.
 *
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 * PLD_FLEX10K_Identify   - Return loader identify string
 * PLD_FLEX10K_LoadCallBk - Loading FLEX 10K with binary data (callback i/o)
 * PLD_FLEX10K_LoadDirect - Loading FLEX 10K with binary data (direct access)
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: flex10k.c,v $
 * Revision 1.6  2008/09/05 15:37:08  ufranke
 * R: diab compiler warning
 * M: cosmetics
 *
 * Revision 1.5  1999/07/21 17:43:31  Franke
 * cosmetics
 *
 * Revision 1.4  1999/04/26 15:14:39  Schmidt
 * includes added to support different variants
 *
 * Revision 1.3  1998/10/26 14:17:05  see
 * loader functions: more header info added
 * file intro added
 * ACCPARAMS: bit masks must be u_int16
 * ACCPARAMS: ma stored as structure instead ptr
 *
 * Revision 1.2  1998/06/02 09:27:18  see
 * global IdentString removed, returned directly from PLD_FLEX10K_Identify
 *
 * Revision 1.1  1998/02/25 09:48:05  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="PLD - FLEX10K Loader: $Id: flex10k.c,v 1.6 2008/09/05 15:37:08 ufranke Exp $";

#include "pld_var.h"		/* defines variants */
#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/maccess.h>
#include <MEN/pld_load.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* bit state */
#define LOW		0
#define HIGH	1

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* parameters for the callback functions */
typedef struct {
	MACCESS ma;										/* access handle */
	void *arg;										/* callback fkt. arg */
	void (*msec_delay)(void *arg, u_int32 msec);	/* callback fkt. */
	u_int16 ctrl_bits;								/* shadow register */
	u_int16 data; 									/* bit mask */
	u_int16 dclk;  									/* bit mask */
	u_int16 config;  								/* bit mask */
	u_int16 status;  								/* bit mask */
	u_int16 cfgdone; 								/* bit mask */
} ACCPARAMS;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void local_msec_delay(void *arg, u_int32 msec);
static u_int8 local_get_status(void *arg);
static u_int8 local_get_cfgdone(void *arg);
static void local_set_data(void *arg, u_int8 state);
static void local_set_dclk(void *arg, u_int8 state);
static void local_set_config(void *arg, u_int8 state);

/***************************** PLD_FLEX10K_Identify **************************
 *
 *  Description: Return loader identify string
 *			
 *---------------------------------------------------------------------------
 *  Input......: -
 *  Output.....: return		identify string
 *  Globals....: -
 ****************************************************************************/
char *PLD_FLEX10K_Identify( void )
{
	return( (char*)RCSid );
}

/*************************** PLD_FLEX10K_LoadCallBk ***************************
 *
 *  Description:  Loading FLEX 10K with binary data (via callback i/o)
 *                PLD interface access is done via callback functions.
 *                A user-defineable pointer is passed to all callback functions.
 *
 *                The PLD data array passed to the loader function must
 *                contain the data from the .TTF file as raw byte stream.
 *                (Conversion can be done with the TTF2ARR tool)
 *
 *                NOTE: This function is for complicated PLD interfaces,
 *                      where PLD bits are located in different registers.
 *
 *                -----------------------------------------------------------
 *                Callback functions required:
 *                -----------------------------------------------------------
 *                void (*msec_delay)(void *arg, u_int32 msec)
 *
 *                Input:      arg       passed argument
 *                            msec		delay time in [msec] (0=none)
 *                Output:     -
 *
 *                msec_delay() waits for the specified time and may not be
 *                aborted by signals. The time may be rounded up to the
 *                system ticker resolution.
 *                -----------------------------------------------------------
 *                u_int8 (*get_status)(void *arg)
 *                u_int8 (*get_cfgdone)(void *arg)
 *
 *                Input:      arg       passed argument
 *                Output:     return    bit state (0=low, 1=high)
 *
 *                get_xxx() functions return the current state of a bit from
 *                the PLD interface.
 *                -----------------------------------------------------------
 *                void (*set_data)(void *arg, u_int8 state)
 *                void (*set_dclk)(void *arg, u_int8 state)
 *                void (*set_config)(void *arg, u_int8 state)
 *
 *                Input:      arg       passed argument
 *                            state     bit state (0=low, 1=high)
 *                Output:     -
 *
 *                set_xxx() functions set a bit of the PLD interface to
 *                the specified state.
 *                -----------------------------------------------------------
 *
 *---------------------------------------------------------------------------
 *  Input......:  buf         PLD data array
 *                size        PLD data size [byte]
 *                flags       loader flags
 *                            PLD_FIRSTBLOCK = load first data block
 *                            PLD_LASTBLOCK  = load last  data block
 *                arg         argument passed to all callback functions
 *                msec_delay  callback function for delay
 *                get_status  callback function for PLD bit reading
 *                get_cfgdone callback function for PLD bit reading
 *                set_data    callback function for PLD bit writing
 *                set_dclk    callback function for PLD bit writing
 *                set_config  callback function for PLD bit writing
 *  Output.....:  return      0					success
 *						      PLD_ERR_NOTFOUND	no response (CONF_DONE timeout)
 *						      PLD_ERR_INIT		init error (STATUS timeout)
 *						      PLD_ERR_LOAD		load error (STATUS error)
 *						      PLD_ERR_TERM		term error (CONF_DONE timeout)
 *  Globals....:  ---
 ****************************************************************************/

int32 PLD_FLEX10K_LoadCallBk(
	u_int8	*buf,
	u_int32 size,
	u_int32 flags,
	void *arg,
	void (*msec_delay)(void *arg, u_int32 msec),
	u_int8 (*get_status)(void *arg),
	u_int8 (*get_cfgdone)(void *arg),
	void (*set_data)(void *arg, u_int8 state),
	void (*set_dclk)(void *arg, u_int8 state),
	void (*set_config)(void *arg, u_int8 state))
{
	register u_int8 ch;
	register u_int32 cnt,n;

	/*-----------------------------+
	|  prepare for loading sequence|
	+-----------------------------*/
	if (flags & PLD_FIRSTBLOCK) {
		/*-----------------------------+
		|  reset FLEX				   |
		|  set config=1				   |
		+-----------------------------*/
		set_config(arg,LOW);
		msec_delay(arg,2);							/* wait 2ms (> 2us) */
		set_config(arg,HIGH);

		/*-----------------------------+
		|  wait for cfgdone=0 (<1us)   |
		+-----------------------------*/
		cnt = 0;

		while(get_cfgdone(arg) == HIGH)
			if (cnt++ == 100)
				return(PLD_ERR_NOTFOUND);

		/*-----------------------------+
		|  wait for status=1 (>2.5us)  |
		+-----------------------------*/
		cnt = 0;

		while(get_status(arg) == LOW)
			if (cnt++ == 500)
				return(PLD_ERR_INIT);
	}

	/*-----------------------------+
	|  load data				   |
	+-----------------------------*/
	if (size) {
		set_config(arg,HIGH);				/* be sure config=1	*/

		/*-----------------------------+
		|  load data				   |
		+-----------------------------*/
		while(size--) {						/* for all bytes */
			ch = *buf++;
			n = 8;

			while(n--) {							/* for all bits (0..7) */
				set_data(arg,(u_int8)(ch & 0x01));	/* write DATA bit */
				set_dclk(arg,HIGH);					/* set DCLK (strobe) */
				set_dclk(arg,LOW);					/* clear DCLK */
				ch >>= 1;							/* next bit */
			}

			if (get_status(arg) == LOW)		/* status error ? */
				return(PLD_ERR_LOAD);
		}
	}

	/*-----------------------------+
	|  terminate loading sequence  |
	+-----------------------------*/
	if (flags & PLD_LASTBLOCK) {
		set_config(arg,HIGH);				/* be sure config=1	*/

		/*-----------------------------+
		|  wait for cfgdone=1		   |
		+-----------------------------*/
		cnt = 0;

		do {
			set_dclk(arg,HIGH);				/* set DCLK (strobe) */
			set_dclk(arg,LOW);				/* clear DCLK */

			if (++cnt == 100)				/* timeout after 100 clocks */
				return(PLD_ERR_TERM);

		} while (get_cfgdone(arg) == LOW);	/* while not CONF_DONE */

		/*-----------------------------+
		|  18 additional clocks		   |
		|  (to set PLD in user state)  |
		+-----------------------------*/
		for (n=0; n<18; n++) {
			set_dclk(arg,HIGH);				/* set DCLK (strobe) */
			set_dclk(arg,LOW);				/* clear DCLK */
		}
	}
	
	return(0);					/* success */
}

/*************************** PLD_FLEX10K_LoadDirect **************************
 *
 *  Description:  Loading FLEX 10K with binary data (via direct access)
 *                PLD interface access is done via direct D16 register access.
 *                A user-defineable pointer is passed to the callback function.
 *
 *                A mask specifies the state written to the non-PLD bits
 *                during the loading process.
 *
 *                The PLD data array passed to the loader function must
 *                contain the data from the .TTF file as raw byte stream.
 *                (Conversion can be done with the TTF2ARR tool)
 *
 *                NOTE: This function is for simple PLD interfaces,
 *                      where all PLD bits are located in one register.
 *
 *                -----------------------------------------------------------
 *                Callback function required:
 *                -----------------------------------------------------------
 *                void (*msec_delay)(void *arg, u_int32 msec)
 *
 *                Input:      arg       passed argument
 *                            msec		delay time in [msec] (0=none)
 *                Output:     -
 *
 *                msec_delay() waits for the specified time and may not be
 *                aborted by signals. The time may be rounded up to the
 *                system ticker resolution.
 *                -----------------------------------------------------------
 *
 *---------------------------------------------------------------------------
 *  Input......:  ma          PLD interface access pointer (D16 access)
 *                buf         PLD data array
 *                size        PLD data size [byte]
 *                flags       loader flags
 *                            PLD_FIRSTBLOCK = load first data block
 *                            PLD_LASTBLOCK  = load last  data block
 *                arg         argument passed to callback function
 *                msec_delay  callback function for delay
 *                nonpld_bits non-PLD bits state
 *                data        DATA  	bit location (0..15: D0..D15)
 *                dclk        DCLK  	bit location (0..15: D0..D15)
 *                config      CONFIG	bit location (0..15: D0..D15)
 *                status      STATUS	bit location (0..15: D0..D15)
 *                cfgdone     CFGDONE	bit location (0..15: D0..D15)
 *  Output.....:  return      0				success
 *						      PLD_ERR_NOTFOUND	no response (CONF_DONE timeout)
 *						      PLD_ERR_INIT		init error (STATUS timeout)
 *						      PLD_ERR_LOAD		load error (STATUS error)
 *						      PLD_ERR_TERM		term error (CONF_DONE timeout)
 *  Globals....:  ---
 ****************************************************************************/

int32 PLD_FLEX10K_LoadDirect(
	MACCESS *ma,
	u_int8	*buf,
	u_int32 size,
	u_int32 flags,
	void *arg,
	void (*msec_delay)(void *arg, u_int32 msec),
	u_int16 nonpld_bits,
	u_int8 data,
	u_int8 dclk,
	u_int8 config,
	u_int8 status,
	u_int8 cfgdone)
{
	ACCPARAMS ac;

	/*--------------------------------------+
    |  create callback function parameters  |
    +--------------------------------------*/
	ac.ma			= *ma;
	ac.arg			= arg;
	ac.msec_delay	= msec_delay;

	/* create bitmasks */
	ac.data    = (u_int16)(1 << data);
	ac.dclk    = (u_int16)(1 << dclk);
	ac.config  = (u_int16)(1 << config);
	ac.status  = (u_int16)(1 << status);
	ac.cfgdone = (u_int16)(1 << cfgdone);

	/* initial control register state */
	ac.ctrl_bits = (u_int16)(nonpld_bits & ~(ac.config | ac.status | ac.cfgdone));		

	/*--------------------------------------+
    |  call loader						    |
    +--------------------------------------*/
	return( PLD_FLEX10K_LoadCallBk(buf, size, flags,
								   &ac,
								   local_msec_delay,
								   local_get_status,
								   local_get_cfgdone,
								   local_set_data,
								   local_set_dclk,
								   local_set_config) );
}

/*----------------------------------------------------------------------+
|  local callback functions                                             |
+----------------------------------------------------------------------*/
static void local_msec_delay(void *arg, u_int32 msec)
{
	ACCPARAMS *ac = (ACCPARAMS*)arg;

	ac->msec_delay(ac->arg,msec);
}

static u_int8 local_get_status(void *arg)
{
	ACCPARAMS *ac = (ACCPARAMS*)arg;

	if (MREAD_D16(ac->ma,0) & ac->status)
		return(HIGH);
	else
		return(LOW);
}

static u_int8 local_get_cfgdone(void *arg)
{
	ACCPARAMS *ac = (ACCPARAMS*)arg;

	if (MREAD_D16(ac->ma,0) & ac->cfgdone)
		return(HIGH);
	else
		return(LOW);
}

static void local_set_data(void *arg, u_int8 state)
{
	ACCPARAMS *ac = (ACCPARAMS*)arg;

	if (state)
		ac->ctrl_bits |= ac->data;
	else
		ac->ctrl_bits &= ~ac->data;

	MWRITE_D16(ac->ma,0,ac->ctrl_bits);
}

static void local_set_dclk(void *arg, u_int8 state)
{
	ACCPARAMS *ac = (ACCPARAMS*)arg;

	if (state)
		ac->ctrl_bits |= ac->dclk;
	else
		ac->ctrl_bits &= ~ac->dclk;

	MWRITE_D16(ac->ma,0,ac->ctrl_bits);
}

static void local_set_config(void *arg, u_int8 state)
{
	ACCPARAMS *ac = (ACCPARAMS*)arg;

	if (state)
		ac->ctrl_bits |= ac->config;
	else
		ac->ctrl_bits &= ~ac->config;

	MWRITE_D16(ac->ma,0,ac->ctrl_bits);
}




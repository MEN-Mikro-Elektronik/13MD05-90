/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: pld_load.h
 *
 *       Author: see
 *        $Date: 2004/05/07 17:07:23 $
 *    $Revision: 1.7 $
 *
 *  Description: PLD loader library defs
 *
 *     Switches: PLD_SW - swapped access
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: pld_load.h,v $
 * Revision 1.7  2004/05/07 17:07:23  cs
 * added PLD_ErrorMsg for swapped variants
 *
 * Revision 1.6  1999/11/02 10:30:47  Gromann
 * changed again prototype of identify to char *
 *
 * Revision 1.5  1999/09/24 09:27:41  kp
 * changed again prototype of identify to const char *
 *
 * Revision 1.4  1999/09/24 09:26:09  kp
 * added (void) in parameter list to identify
 *
 * Revision 1.3  1999/07/21 17:43:54  Franke
 * cosmetics
 *
 * Revision 1.2  1999/05/28 16:18:17  Schmidt
 * supports variant pld_sw
 *
 * Revision 1.1  1998/02/25 09:48:09  Schmidt
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _PLD_LOAD_H
#define _PLD_LOAD_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* swapped access */
#ifdef PLD_SW
#	define PLD_ErrorMsg		PLD_SW_ErrorMsg
#	define PLD_FLEX10K_Identify	PLD_SW_FLEX10K_Identify
#	define PLD_FLEX10K_LoadCallBk	PLD_SW_FLEX10K_LoadCallBk
#	define PLD_FLEX10K_LoadDirect	PLD_SW_FLEX10K_LoadDirect
#endif

/* PLD execution flags */
#define PLD_FIRSTBLOCK		0x1		/* init PLD before loading block */
#define PLD_LASTBLOCK		0x2		/* terminate PLD after loading block */

/* PLD error codes */
#define PLD_ERR_NOTFOUND	0x1		/* no response from PLD */
#define PLD_ERR_INIT		0x2		/* error when initializing PLD */
#define PLD_ERR_LOAD		0x3		/* error when loading PLD */
#define PLD_ERR_TERM		0x4		/* error when terminating PLD */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

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
/* Common */
extern char *PLD_ErrorMsg(int32 error);

/* FLEX10K */
extern char *PLD_FLEX10K_Identify(void);
extern int32 PLD_FLEX10K_LoadCallBk(u_int8	*buf,
							 u_int32 size,
							 u_int32 flags,
							 void *arg,
							 void (*msec_delay)(void *arg, u_int32 msec),
							 u_int8 (*get_status)(void *arg),
							 u_int8 (*get_cfgdone)(void *arg),
							 void (*set_data)(void *arg, u_int8 state),
							 void (*set_dclk)(void *arg, u_int8 state),
							 void (*set_config)(void *arg, u_int8 state));
extern int32 PLD_FLEX10K_LoadDirect(MACCESS *ma,
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
							 u_int8 cfgdone);


#ifdef __cplusplus
	}
#endif

#endif	/* _PLD_LOAD_H */



/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  mdelay.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2004/05/28 16:03:53 $
 *    $Revision: 1.4 $
 *
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  Mikrodelay routines
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mdelay.c,v $
 * Revision 1.4  2004/05/28 16:03:53  ww
 * use casts on different places
 *
 * Revision 1.3  2003/06/06 09:30:13  kp
 * Changed headers for doxygen
 *
 * Revision 1.2  2002/05/31 15:12:14  kp
 * Routines now set errno in addition to return code on failure
 *
 * Revision 1.1  2001/01/19 14:39:45  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
static const char RCSid[]="$Header: /dd2/CVSR/LINUX/LIBSRC/USR_OSS/mdelay.c,v 1.4 2004/05/28 16:03:53 ww Exp $";

#include "uos_int.h"
#include <time.h>

#define TRYS 3
#define LOOPS 4000
/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static u_int32 G_mDelayLoops;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void BusyLoop( u_int32 loopCount );

/**********************************************************************/
/** Calibrates the loop counter for UOS_MikroDelay
 *
 * \copydoc usr_oss_specification.c::UOS_MikroDelayInit()
 *
 * \linux Since we don't have a timer that we can use, we try to measure
 * a loop counter three times. The minimum time is used and then doubled
 * to get at least the required delay.
 *
 * \sa UOS_MikroDelay
 */
int32 UOS_MikroDelayInit()
{
	u_int32 loopCount, minLoops = 0xffffffff;
	int i;
	u_int32 msTick, msTick2;

	for(i=0; i<TRYS; i++){

		/*--- wait for tick to change ---*/
		msTick2 = UOS_MsecTimerGet();
		while( (msTick=UOS_MsecTimerGet()) == msTick2 )
			;

		loopCount = 0;

		/*--- perform busy loop for 200ms ---*/
		while( UOS_MsecTimerGet() < msTick+200 ){
			BusyLoop( LOOPS );
			loopCount++;
		}
		DBGCMD(printf("UOS_MikroDelayInit: try %d loops=%d\n", i,
					  (unsigned int)loopCount));
		if( loopCount < minLoops )
			minLoops = loopCount;
	}

	G_mDelayLoops = minLoops * 2;

	DBGCMD(printf("UOS_MikroDelayInit: G_mDelayLoops=%d\n", (unsigned int)G_mDelayLoops ));

	return 0;
}

/**********************************************************************/
/** Busy sleep for specified time
 *
 * \copydoc usr_oss_specification.c::UOS_MikroDelay()
 * \sa UOS_MikroDelayInit
 */
int32 UOS_MikroDelay(u_int32 usec)
{
	u_int32 i, loops;

	/*--- check if UOS_MikroDelayInit has been called before ---*/
	if( G_mDelayLoops == 0 ){
		errno = ERR_UOS_NO_MIKRODELAY;
		return ERR_UOS_NO_MIKRODELAY;
	}

	/*--- check for maximum time ---*/
	if( usec > UOS_MAX_USEC){
		errno = ERR_UOS_ILL_PARAM;
		return ERR_UOS_ILL_PARAM;
	}
	/*--- perform busy loops ---*/
	loops = usec * G_mDelayLoops / 200000;

	for(i=0; i<loops; i++ )
		BusyLoop( LOOPS );

	return 0;
}

static void BusyLoop( u_int32 loopCount )
{
	volatile int i;

	for(i=loopCount; i--; )
		;
}

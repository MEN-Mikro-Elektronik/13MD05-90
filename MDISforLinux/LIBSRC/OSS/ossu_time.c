/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_time.c
 *
 *      \author  christian.schuster@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Time related routines of the OSS module for user space
 *
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2005-2019, MEN Mikro Elektronik GmbH
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
#include "oss_intern.h"
#include <sys/time.h>

#define TRYS 3
#define LOOPS 10

 /*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static u_int32 G_mDelayLoops;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void BusyLoop( u_int32 loopCount );
static u_int32 OSS_MsecTimerGet(void);
static u_int32 OSS_MsecTimerResolution(void);

/**********************************************************************/
/** Let process sleep for specified time.
 *
 * \copydoc oss_specification.c::OSS_Delay()
 *
 * \linux The timer used has a resolution of aprox 10msec.
 *        This means that the delay will be at least between 10 and 20 msec
 *
 *
 * \sa OSS_MikroDelay
 */
int32 OSS_Delay( OSS_HANDLE *oss, int32 msec )
{
	struct timespec request, remain;
	u_int32 current, start, expires;
	int32 left;

	start = current = OSS_MsecTimerGet();
	expires = current + msec;

	DBGWRT_1((DBH,"OSS_Delay: msec=%d\n", msec ));

	while( (left = expires - current) > 0 ) {

		request.tv_sec = left / 1000;
		request.tv_nsec = (left % 1000) * 1000000;

		nanosleep( &request, &remain );
		
		current = OSS_MsecTimerGet();
	}

	return current-start;
}/*OSS_Delay*/

/**********************************************************************/
/** Initialisation routine for OSS_MikroDelay().
 *
 * \copydoc oss_specification.c::OSS_MikroDelayInit()
 *
 * \linux Since we don't have a timer that we can use, we try to measure
 * a loop counter three times. The minimum time is used and then doubled
 * to get at least the required delay.
 *
 * \sa OSS_MikroDelay
 */
int32 OSS_MikroDelayInit( OSS_HANDLE *oss )
{
	u_int32 loopCount, minLoops = 0xffffffff;
	int i;
	u_int32 msTick, msTick2;

	for(i=0; i<TRYS; i++){

		/*--- wait for tick to change ---*/
		msTick2 = OSS_MsecTimerGet();
		while( (msTick=OSS_MsecTimerGet()) == msTick2 )
			;

		loopCount = 0;

		/*--- perform busy loop for 200ms ---*/
		while( OSS_MsecTimerGet() < msTick+200 ){
			BusyLoop( LOOPS );
			loopCount++;
		}
		DBGWRT_1((DBH,"OSS_MikroDelayInit: try %d loops=%d\n",
					  i, (unsigned int)loopCount ));
		if( loopCount < minLoops )
			minLoops = loopCount;
	}

	G_mDelayLoops = minLoops * 2;

	DBGWRT_1((DBH,"OSS_MikroDelayInit: G_mDelayLoops=%d\n", (unsigned int)G_mDelayLoops ));

	return 0;
}/*OSS_MikroDelayInit*/

/**********************************************************************/
/** Busy sleep for specified time.
 *
 * \copydoc oss_specification.c::OSS_MikroDelay()
 *
 * \sa OSS_MikroDelayInit
 */
int32 OSS_MikroDelay( OSS_HANDLE *oss, u_int32 usec )
{
	u_int32 i, loops;

	/*--- check if OSS_MikroDelayInit has been called before ---*/
	if( G_mDelayLoops == 0 ){
		errno = ERR_OSS_NO_MIKRODELAY;
		return ERR_OSS_NO_MIKRODELAY;
	}

	/*--- check for maximum time ---*/
	if( usec > OSS_MAX_USEC){
		errno = ERR_OSS_ILL_PARAM;
		return ERR_OSS_ILL_PARAM;
	}
	/*--- perform busy loops ---*/
	loops = usec * G_mDelayLoops / 200000;
	if((G_mDelayLoops % 200000) > 100000)
		loops++;

	if(!loops)
		loops = 1;

	for(i=0; i<loops; i++ )
		BusyLoop( LOOPS );

	return 0;
}/*OSS_MikroDelay*/

/**********************************************************************/
/** Get the tick rate.
 *
 * \copydoc oss_specification.c::OSS_TickRateGet()
 *
 * \sa OSS_TickGet
 */
int32 OSS_TickRateGet( OSS_HANDLE *oss )
{
    return 1000/OSS_MsecTimerResolution();
}/*OSS_TickRateGet*/

/**********************************************************************/
/** Get the current system tick.
 *
 * \copydoc oss_specification.c::OSS_TickGet()
 *
 * \sa OSS_TickRateGet
 */
u_int32 OSS_TickGet(OSS_HANDLE *oss)
{
	return OSS_MsecTimerGet();
}/*OSS_TickGet*/

/**********************************************************************/
/** Helper funcion for MikroDelay(Init).
 *
 */
static void BusyLoop( u_int32 loopCount )
{
	volatile int i;

	for(i=loopCount; i--; )
		;
}

/**********************************************************************/
/** Read the current timer value in mseconds
 *
 * kp note: Wrap around safe. Verified with timer_test.c
 */
static u_int32 OSS_MsecTimerGet(void)
{
	struct timeval tv;

	if( gettimeofday( &tv, NULL ) < 0 )
		return 0;
	
	return (tv.tv_sec * 1000) + (tv.tv_usec/1000);
}

/**********************************************************************/
/** Get timer resolution of OSS_MsecTimerGet()
 *
 * \return timer resolution in milliseconds
 */
static u_int32 OSS_MsecTimerResolution(void)
{
	return(1);					/* resolution of ftime (I hope) */
}

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_alarm.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Alarm routines
 *
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
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

#define _OSS_ALARM_C
#include "oss_intern.h"


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void AlarmTimeout( unsigned long data );
#else
static void AlarmTimeout( struct timer_list *t );
#endif

/*! \page linossalarmusage

  \section linossalarmusagesect Linux notes to OSS alarms

  Under linux, the alarms are implemented using kernel timers.
  The granularity of alarm cycles is limited to the tick rate of Linux,
  typically 10ms.

  Alarm routines are called at interrupt level.

  See \ref ossalarmusagesect for more info.
*/

/**********************************************************************/
/** Create an alarm.
 *
 * \copydoc oss_specification.c::OSS_AlarmCreate()
 *
 * See \ref linossalarmusagesect for more info.
 *
 *
 * \sa OSS_AlarmRemove, OSS_AlarmSet, OSS_AlarmClear
 */
int32 OSS_AlarmCreate(
    OSS_HANDLE       *oss,
    void             (*funct)(void *arg),
	void             *arg,
    OSS_ALARM_HANDLE **alarmP)
{
    DBGWRT_1((DBH,"OSS - OSS_AlarmCreate func=0x%p arg=0x%lx\n", funct, arg));

	/*----------------------+
    |  alloc/init handle    |
    +----------------------*/
    if ((*alarmP = kmalloc( sizeof(OSS_ALARM_HANDLE), GFP_KERNEL )) == NULL )
       return(ERR_OSS_MEM_ALLOC);

	(*alarmP)->funct    = funct;
	(*alarmP)->arg		= arg;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	(*alarmP)->tmr.data = (unsigned long)*alarmP;
#endif
	(*alarmP)->active   = 0;
	(*alarmP)->cyclic   = 0;	/* set later */
	(*alarmP)->interval = 0;	/* set later */
	(*alarmP)->oss		= oss;

	return(ERR_SUCCESS);
}

/**********************************************************************/
/** Destroys alarm handle.
 *
 * \copydoc oss_specification.c::OSS_AlarmRemove()
 *
 * See \ref linossalarmusagesect for more info.
 *
 *
 * \sa OSS_AlarmCreate, OSS_AlarmSet, OSS_AlarmClear
 */
int32 OSS_AlarmRemove(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE **alarmP
)
{
	int32 error;

    DBGWRT_1((DBH,"OSS - OSS_AlarmRemove alm=%p\n", *alarmP));

	/* de-activate alarm if activated */
	if ((*alarmP)->active && (error = OSS_AlarmClear(oss, *alarmP)))
		return(error);

	kfree( *alarmP );
	*alarmP = NULL;
	return ERR_SUCCESS;
}

/**********************************************************************/
/** Activate an installed alarm routine
 *
 * \copydoc oss_specification.c::OSS_AlarmSet()
 *
 * See \ref linossalarmusagesect for more info.
 *
 * \sa OSS_AlarmCreate, OSS_AlarmRemove, OSS_AlarmClear
 */
int32 OSS_AlarmSet(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE *alarm,
    u_int32          msec,
    u_int32          cyclic,
    u_int32          *realMsecP
)
{
	OSS_ALARM_STATE flags;
	u_int32 ticks;

    DBGWRT_1((DBH,"OSS - OSS_AlarmSet: %s, msec=%d\n",
			  cyclic ? "cyclic":"single", msec));

	/* return error if already active */
	flags = OSS_AlarmMask( oss );

	if (alarm->active) {
		DBGWRT_ERR((DBH," *** OSS_AlarmSet: alarm already active\n"));
		OSS_AlarmRestore( oss, flags );
		return(ERR_OSS_ALARM_SET);
	}

	/*--- round time, and correct for timer inaccuracy ---*/
	ticks = (msec * HZ) / 1000;
	ticks++;

	if( (msec * HZ) % 1000 )
		ticks++;

	alarm->interval = ticks;

	/* calc rounded msec */
	*realMsecP = (ticks * 1000) / HZ;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer( &alarm->tmr );
	alarm->tmr.function = AlarmTimeout;
#else
	timer_setup(&alarm->tmr, (void *)(struct timer_list *)AlarmTimeout, 0);
#endif
	alarm->tmr.expires 	= jiffies + ticks;
	alarm->cyclic 		= cyclic;
	alarm->active 		= TRUE;

	/* activate timer */
	add_timer( &alarm->tmr );
	OSS_AlarmRestore( oss, flags );

	return(ERR_SUCCESS);
}

/**********************************************************************/
/** Deactivate an installed alarm routine
 *
 * \copydoc oss_specification.c::OSS_AlarmClear()
 *
 * See \ref linossalarmusagesect for more info.
 *
 * \sa OSS_AlarmCreate, OSS_AlarmRemove, OSS_AlarmSet
 */
int32 OSS_AlarmClear(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE *alarm
)
{
	OSS_ALARM_STATE flags;


    DBGWRT_1((DBH,"OSS - OSS_AlarmClear\n"));

	flags = OSS_AlarmMask( oss );

	if (!alarm->active) {
		DBGWRT_ERR((DBH," *** OSS_AlarmClear: alarm not active"));
		OSS_AlarmRestore( oss, flags );
		return(ERR_OSS_ALARM_CLR);
	}

	del_timer(&alarm->tmr);

	alarm->active = FALSE;

	OSS_AlarmRestore( oss, flags );


	return(ERR_SUCCESS);
}

/**********************************************************************/
/** Mask alarms
 *
 * \copydoc oss_specification.c::OSS_AlarmMask()
 *
 * \linux Masks \b all processor interrupts
 *
 * \sa OSS_AlarmRestore()
 */
OSS_ALARM_STATE OSS_AlarmMask( OSS_HANDLE *oss )
{
	DBGWRT_1((DBH,"OSS_AlarmMask (Lin)\n"));
	return OSS_IrqMaskR( oss, NULL );
}

/**********************************************************************/
/** Unmask alarms
 *
 * \copydoc oss_specification.c::OSS_AlarmRestore()
 *
 * \sa OSS_AlarmMask()
 */
void OSS_AlarmRestore( OSS_HANDLE *oss, OSS_ALARM_STATE oldState )
{
	DBGWRT_1((DBH,"OSS_AlarmRestore (Lin)\n"));
	OSS_IrqRestore( oss, NULL, oldState );
}


/******************************** AlarmTimeout *******************************
 *
 *  Description: Alarm handler routine
 *
 *---------------------------------------------------------------------------
 *  Input......: data	    alarm handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void AlarmTimeout( unsigned long data )
#else
static void AlarmTimeout( struct timer_list *t )
#endif
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	OSS_ALARM_HANDLE *alarm = (OSS_ALARM_HANDLE *)data;
#else
	OSS_ALARM_HANDLE *alarm = from_timer(alarm, t, tmr);
#endif
	OSS_ALARM_STATE flags;

	flags = OSS_AlarmMask( alarm->oss );
	/*DBGWRT_3((DBH,">>> alarm handler\n"));*/

	/* jump into installed function */
	alarm->funct(alarm->arg);		

	if (!alarm->cyclic)
		/* mark single-alarm as inactive */
		alarm->active = FALSE;
	else {
		/* reinstall alarm */
		alarm->tmr.expires = jiffies + alarm->interval;
		add_timer( &alarm->tmr );
	}
	OSS_AlarmRestore( alarm->oss, flags );
}






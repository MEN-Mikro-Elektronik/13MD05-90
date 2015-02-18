/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  timer.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2004/06/09 10:27:22 $
 *    $Revision: 1.3 $
 *
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  Timer routines
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: timer.c,v $
 * Revision 1.3  2004/06/09 10:27:22  kp
 * replaced call to obsolete ftime with gettimeofday
 *
 * Revision 1.2  2003/06/06 09:30:19  kp
 * Changed headers for doxygen
 *
 * Revision 1.1  2001/01/19 14:39:48  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
static const char RCSid[]="$Id: timer.c,v 1.3 2004/06/09 10:27:22 kp Exp $";

#include "uos_int.h"
#include <sys/time.h>

/**********************************************************************/
/** Read the current timer value in mseconds
 *
 * \copydoc usr_oss_specification.c::UOS_MsecTimerGet()
 * kp note: Wrap around safe. Verified with timer_test.c
 * \sa UOS_MsecTimerResolution
 */
u_int32 UOS_MsecTimerGet(void)
{
	struct timeval tv;

	if( gettimeofday( &tv, NULL ) < 0 )
		return 0;
	
	return (tv.tv_sec * 1000) + (tv.tv_usec/1000);
}

/**********************************************************************/
/** Get timer resolution of UOS_MsecTimerGet()
 *
 * \copydoc usr_oss_specification.c::UOS_MsecTimerResolution()
 * \sa UOS_MsecTimerGet
 */
u_int32 UOS_MsecTimerResolution(void)
{
	return(1);					/* resolution of ftime (I hope) */
}



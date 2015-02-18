/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  delay.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/09/25 19:19:56 $
 *    $Revision: 1.7 $
 *
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  Delay function
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: delay.c,v $
 * Revision 1.7  2009/09/25 19:19:56  CRuff
 * R: causes problems on slow cpus, e.g. A15
 * M: Restore version 1.3
 *
 * Revision 1.6  2009/09/08 18:16:04  CRuff
 * R: cosmetics
 * M: removed unused variable
 *
 * Revision 1.5  2009/08/04 17:21:40  CRuff
 * R: cosmetics
 * M: 1. change order of statements in UOS_Delay()
 *    2. comment out the debug printout in UOS_Delay()
 *
 * Revision 1.4  2009/07/28 12:38:26  CRuff
 * R: UOS_Delay causes too short delay if tickrollover occurs and delay is
 *    interrupted by signal
 * M: keep delay independent from tickrollover by not using system tick
 *    differences
 *
 * Revision 1.3  2004/06/09 10:27:20  kp
 * fixed problem with timer wrap
 *
 * Revision 1.2  2003/06/06 09:28:27  kp
 * Changed headers for doxygen
 * Temp. Workaround for UOS_Delay when signals received during wait
 *
 * Revision 1.1  2001/01/19 14:39:39  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: delay.c,v 1.7 2009/09/25 19:19:56 CRuff Exp $";

#include "uos_int.h"

/**********************************************************************/
/** Let a process sleep for a specified time
 *
 * \copydoc usr_oss_specification.c::UOS_Delay()
 *
 * \sa UOS_MikroDelay
 */
int32 UOS_Delay(u_int32 msec)
{
	struct timespec request, remain;
	u_int32 current, start, expires;
	int32 left;

	start = current = UOS_MsecTimerGet();
	expires = current + msec;

	while( (left = expires - current) > 0 ) {

		/*printf("UOS_Delay: msec=%d\n", msec );*/
		request.tv_sec = left / 1000;
		request.tv_nsec = (left % 1000) * 1000000;

		nanosleep( &request, &remain );
		
		current = UOS_MsecTimerGet();
	}

	return current-start;
}



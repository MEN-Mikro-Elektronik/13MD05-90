/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: usr_err.h
 *
 *       Author: see
 *        $Date: 1998/09/18 13:18:55 $
 *    $Revision: 2.1 $
 *
 *  Description: USR_OSS error code definitions
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: usr_err.h,v $
 * Revision 2.1  1998/09/18 13:18:55  see
 * Initial Revision
 *
 * moved from usr_oss.h
 * error codes changed
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _USR_ERR_H_
#define _USR_ERR_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include <MEN/usr_os.h>

/*--------------------------- UOS error codes ---------------------------------*/

/* no error */
#define ERR_UOS_SUCCESS			0x0000			/* success */

/* UOS errors */
#define ERR_UOS_BUSY            ERR_UOS+0x00    /* function is locked */
#define ERR_UOS_NOT_INIZED      ERR_UOS+0x01    /* function not initialized */
#define ERR_UOS_NOT_INSTALLED   ERR_UOS+0x02    /* function not installed */
#define ERR_UOS_ABORTED         ERR_UOS+0x03    /* function aborted */
#define ERR_UOS_TIMEOUT         ERR_UOS+0x04    /* function timeout */
#define ERR_UOS_OVERRUN         ERR_UOS+0x05    /* queue overrun */
#define ERR_UOS_ILL_SIG         ERR_UOS+0x06    /* illegal signal code */
#define ERR_UOS_NO_TIMER        ERR_UOS+0x07    /* no timer available */
#define ERR_UOS_NO_MIKRODELAY   ERR_UOS+0x08    /* no mikro delay available */
#define ERR_UOS_ILL_PARAM       ERR_UOS+0x09    /* illegal parameter */
#define ERR_UOS_MEM_ALLOC       ERR_UOS+0x0a    /* can't alloc memory */
#define ERR_UOS_GETSTAT         ERR_UOS+0x0b    /* driver getstat failed */
#define ERR_UOS_SETSTAT         ERR_UOS+0x0c    /* driver setstat failed */
#define ERR_UOS_NO_PERM       	ERR_UOS+0x0d    /* no permission accessing memory */

#ifdef __cplusplus
    }
#endif

#endif /*_USR_ERR_H_*/






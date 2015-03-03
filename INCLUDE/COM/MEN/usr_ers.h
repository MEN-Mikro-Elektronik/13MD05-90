/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: usr_ers.h
 *
 *       Author: ds
 *        $Date: 1998/09/18 13:18:59 $
 *    $Revision: 2.2 $
 *
 *  Description: UOS_OSS error code string table
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: usr_ers.h,v $
 * Revision 2.2  1998/09/18 13:18:59  see
 * wrong ifndef name changed
 * ERR_UOS_SUCCESS added
 * ERR_UOS_NO_PERM added
 *
 * Revision 2.1  1998/08/26 16:48:01  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _USR_ERS_H_
#  define _USR_ERS_H_

#  ifdef __cplusplus
      extern "C" {
#  endif

/*------------------------------+
|  table of error code strings  |
+------------------------------*/
struct _UOS_ERR_STR {
    int32  errCode;
    char*  errString;
} UOS_ErrStrTable[] =
{	/* no error */
	{ ERR_UOS_SUCCESS		  ,"UOS: (no error)" },
	/* UOS error codes */
	{ ERR_UOS_BUSY            ,"UOS: function is locked" },
	{ ERR_UOS_NOT_INIZED      ,"UOS: function not initialized" },
	{ ERR_UOS_NOT_INSTALLED   ,"UOS: function not installed" },
	{ ERR_UOS_ABORTED         ,"UOS: function aborted" },
	{ ERR_UOS_TIMEOUT         ,"UOS: timeout occurred" },
	{ ERR_UOS_OVERRUN         ,"UOS: queue overrun" },
	{ ERR_UOS_ILL_SIG         ,"UOS: illegal signal code" },
	{ ERR_UOS_NO_TIMER        ,"UOS: no timer available" },
	{ ERR_UOS_NO_MIKRODELAY   ,"UOS: no mikro delay available" },
	{ ERR_UOS_ILL_PARAM       ,"UOS: illegal parameter" },
	{ ERR_UOS_MEM_ALLOC       ,"UOS: can't alloc memory" },
	{ ERR_UOS_GETSTAT         ,"UOS: driver getstat failed" },
	{ ERR_UOS_SETSTAT         ,"UOS: driver setstat failed" },
	{ ERR_UOS_NO_PERM         ,"UOS: no permission accessing memory" },
};

#define UOS_NBR_OF_ERR sizeof(UOS_ErrStrTable)/sizeof(struct _UOS_ERR_STR)

#  ifdef __cplusplus
       }
#  endif

#endif/*_USR_ERS_H_*/





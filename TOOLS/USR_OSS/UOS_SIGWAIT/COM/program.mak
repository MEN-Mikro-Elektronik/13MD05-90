#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 1999/04/20 15:44:16 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for USR_OSS test program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  1999/04/20 15:44:16  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=uos_sigwait

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/usr_oss.h

MAK_INP1=uos_sigwait$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) 

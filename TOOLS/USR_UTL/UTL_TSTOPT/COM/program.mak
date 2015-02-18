#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 1999/04/20 15:46:08 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for USR_UTL test program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  1999/04/20 15:46:08  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=utl_tstopt

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/usr_utl.h

MAK_INP1=utl_tstopt$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) 

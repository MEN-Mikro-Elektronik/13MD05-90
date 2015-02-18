#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 1999/04/20 15:05:28 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for MDIS_API test program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  1999/04/20 15:05:28  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m_setstat

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h	  \
         $(MEN_INC_DIR)/usr_oss.h	  \
         $(MEN_INC_DIR)/usr_utl.h

MAK_INP1=m_setstat$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) 






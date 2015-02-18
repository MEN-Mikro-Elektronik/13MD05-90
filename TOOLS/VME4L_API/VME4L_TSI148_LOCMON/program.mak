#***************************  M a k e f i l e  *******************************
#
#         Author: rt
#          $Date: 2009/06/03 19:43:09 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for VME4L_TSI148_LOCMON
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  2009/06/03 19:43:09  rt
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=vme4l_tsi148_locmon

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/vme4l_api$(LIB_SUFFIX)


MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h	\
	     $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h	\
	     $(MEN_INC_DIR)/../../COM/MEN/tsi148.h

MAK_INP1=vme4l_tsi148_locmon$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

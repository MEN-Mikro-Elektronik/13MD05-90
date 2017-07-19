#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2003/12/15 15:02:22 $
#      $Revision: 1.1 $
#                      
#    Description: Makefile descriptor file for VME4L_API lib
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  2003/12/15 15:02:22  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=vme4l_api

MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_old.h\
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h


MAK_INP1 = vme4l_api$(INP_SUFFIX)

MAK_INP  = $(MAK_INP1)


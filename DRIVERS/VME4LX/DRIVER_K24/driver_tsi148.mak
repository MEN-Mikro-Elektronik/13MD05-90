#**************************  M a k e f i l e ********************************
#  
#         Author: sv
#          $Date: 2009/04/30 21:25:22 $
#      $Revision: 1.2 $
#  
#    Description: makefile descriptor for VME4L TSI148
#                      
#---------------------------------[ History ]---------------------------------
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=tsi148

MAK_LIBS=

MAK_SWITCH = -DTSI148_BUILD

MAK_INCL=$(MEN_MOD_DIR)/vme4l-core.h \
		 $(MEN_MOD_DIR)/vme4l-tsi148.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_old.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/men_vme_kernelif.h \
		 $(MEN_INC_DIR)/tsi148.h

MAK_INP1=vme4l-tsi148$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

WARN_LEVEL := -Wall

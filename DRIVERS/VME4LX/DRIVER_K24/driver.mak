#**************************  M a k e f i l e ********************************
#  
#         Author: kp
#          $Date: 2003/12/15 15:02:06 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for VME4L core
#                      
#---------------------------------[ History ]---------------------------------
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=vme4l-core

MAK_LIBS=

MAK_SWITCH = -DEXPORT_SYMTAB

MAK_INCL=$(MEN_MOD_DIR)/vme4l-core.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_old.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/men_vme_kernelif.h

MAK_INP1=vme4l-core$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

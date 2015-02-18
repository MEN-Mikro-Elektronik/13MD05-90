#**************************  M a k e f i l e ********************************
#  
#         Author: ub
#          $Date: 2004/11/23 09:33:54 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for chameleon Linux kernel module
#                      
#---------------------------------[ History ]---------------------------------
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2004 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=lx_chameleon

MAK_LIBS=

MAK_SWITCH =

MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/men_chameleon.h \
		 $(MEN_INC_DIR)/chameleon.h \

MAK_INP1=men_chameleon$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

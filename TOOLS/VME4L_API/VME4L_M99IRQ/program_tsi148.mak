#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2009/04/30 21:06:36 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for VME4L_M99IRQ_TSI148
#                 (TSI148 doesn't support hardware swapping, therefore user
#                  applications has to care for swapping)
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program_tsi148.mak,v $
#   Revision 1.1  2009/04/30 21:06:36  rt
#   Initial Revision
#
#   Revision 1.1  2003/12/15 15:02:35  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=vme4l_m99irq_tsi148

MAK_SWITCH = -DMEN_TSI148

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/vme4l_api$(LIB_SUFFIX)


MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h	\
	     $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h

MAK_INP1=vme4l_m99irq$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2001/01/19 14:37:57 $
#      $Revision: 1.1 $
#
#    Description: makefile for Linux BBIS kernel
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  2001/01/19 14:37:57  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=bbis_kernel

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/bk_intern.h  \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/bbis_bk.h

MAK_INP1=bk_module$(INP_SUFFIX)
MAK_INP2=
MAK_INP3=
MAK_INP4=
MAK_INP5=
MAK_INP6=
MAK_INP7=

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6) \
        $(MAK_INP7)

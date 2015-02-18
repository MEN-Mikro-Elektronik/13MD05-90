#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2003/02/21 13:34:35 $
#      $Revision: 1.2 $
#
#    Description: makefile for Linux MDIS kernel
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.2  2003/02/21 13:34:35  kp
#   added support for RTAI and non-mdis drivers
#
#   Revision 1.1  2001/01/19 14:58:32  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=mdis_kernel

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/mk_intern.h  \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/mdis_mk.h

MAK_INP1=mk_module$(INP_SUFFIX)
MAK_INP2=open$(INP_SUFFIX)
MAK_INP3=close$(INP_SUFFIX)
MAK_INP4=mk_calls$(INP_SUFFIX)
MAK_INP5=ident$(INP_SUFFIX)
MAK_INP6=mk_nonmdis$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6)

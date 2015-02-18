#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2011/04/07 16:41:16 $
#      $Revision: 1.7 $
#
#    Description: makefile for Linux OSS lib
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.7  2011/04/07 16:41:16  CRuff
#   R: new file oss_spinlock.c
#   M: added oss_spinlock.c as make input
#
#   Revision 1.6  2006/08/04 11:18:50  ts
#   removed source oss_smb.c
#
#   Revision 1.5  2006/06/21 17:52:49  ts
#   includes oss_smb.o build
#
#   Revision 1.4  2005/07/07 17:16:53  cs
#   Copyright line changed
#
#   Revision 1.3  2003/10/07 11:47:36  kp
#   added oss_ident
#
#   Revision 1.2  2003/02/21 11:25:01  kp
#   reworked for RTAI integration
#
#   Revision 1.1  2001/01/19 14:39:02  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
# (c) Copyright 2000-2015 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
#*****************************************************************************

MAK_NAME=oss

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/oss_intern.h  \
	 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h \

MAK_INP1=oss$(INP_SUFFIX)
MAK_INP2=oss_clib$(INP_SUFFIX)
MAK_INP3=oss_mem$(INP_SUFFIX)
MAK_INP4=oss_time$(INP_SUFFIX)
MAK_INP5=oss_resource$(INP_SUFFIX)
MAK_INP6=oss_map$(INP_SUFFIX)
MAK_INP7=oss_swap$(INP_SUFFIX)
MAK_INP8=oss_alarm$(INP_SUFFIX)
MAK_INP9=oss_dl_list$(INP_SUFFIX)
MAK_INP10=oss_bustoaddr$(INP_SUFFIX)
MAK_INP11=oss_task$(INP_SUFFIX)
MAK_INP12=oss_irq$(INP_SUFFIX)
MAK_INP13=oss_isa_pnp$(INP_SUFFIX)
MAK_INP14=oss_sem$(INP_SUFFIX)
MAK_INP15=oss_sig$(INP_SUFFIX)
MAK_INP16=oss_spinlock$(INP_SUFFIX)
MAK_INP17=oss_ident$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6) \
        $(MAK_INP7) \
        $(MAK_INP8) \
        $(MAK_INP9) \
        $(MAK_INP10) \
        $(MAK_INP11) \
        $(MAK_INP12) \
        $(MAK_INP13) \
        $(MAK_INP14) \
        $(MAK_INP15) \
        $(MAK_INP16) \
        $(MAK_INP17)


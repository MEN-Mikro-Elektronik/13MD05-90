#**************************  M a k e f i l e ********************************
#
#         Author: ds
#          $Date: 2007/08/09 09:15:58 $
#      $Revision: 1.3 $
#
#    Description: makefile descriptor for ID library
#
#                 variant id_sw for swapped access
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_sw.mak,v $
#   Revision 1.3  2007/08/09 09:15:58  CKauntz
#   added usmrw.c for USM EEPROM read/write
#
#   Revision 1.2  2000/03/16 16:10:06  kp
#   Added microwire_port
#
#   Revision 1.1  1999/04/26 14:40:23  Schmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************


MAK_NAME=id_sw

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)ID_SW

MAK_INCL=$(MEN_MOD_DIR)/id_var.h \
         $(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/modcom.h

MAK_INP1=c_drvadd$(INP_SUFFIX)
MAK_INP2=microwire_port$(INP_SUFFIX)
MAK_INP3=usmrw$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)\
		$(MAK_INP2)\
		$(MAK_INP3)



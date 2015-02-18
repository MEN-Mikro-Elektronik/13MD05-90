#**************************  M a k e f i l e ********************************
#
#         Author: uf
#          $Date: 2007/08/09 09:15:56 $
#      $Revision: 1.7 $
#
#    Description: makefile descriptor for ID library
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.7  2007/08/09 09:15:56  CKauntz
#   added usmrw.c for USM EEPROM read/write
#
#   Revision 1.6  1999/07/30 14:23:02  Franke
#   new microwire module added
#
#   Revision 1.5  1999/04/26 14:39:29  Schmidt
#   includes added, MAK_OPTIM removed
#
#   Revision 1.4  1999/04/14 14:43:41  kp
#   changed MAK_NAME to id
#   added MAK_SWITCH
#
#   Revision 1.3  1999/02/19 11:56:54  Franke
#   added maccess.h
#   removed modulid.h
#
#   Revision 1.2  1998/04/17 15:42:30  see
#   bug fixed: MEN_MOD_DIR changed to MEN_INC_DIR
#
#   Revision 1.1  1998/02/25 09:59:43  franke
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************


MAK_NAME=id

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_INCL=$(MEN_MOD_DIR)/id_var.h \
         $(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/modcom.h  \
         $(MEN_INC_DIR)/microwire.h

MAK_INP1=c_drvadd$(INP_SUFFIX)
MAK_INP2=microwire_port$(INP_SUFFIX)
MAK_INP3=usmrw$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)\
		$(MAK_INP2)\
		$(MAK_INP3)



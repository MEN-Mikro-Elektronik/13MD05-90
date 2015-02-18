#**************************  M a k e f i l e ********************************
#
#         Author: dieter.pfeuffer@men.de
#          $Date: 2007/07/13 17:35:30 $
#      $Revision: 1.1 $
#
#    Description: makefile descriptor for SMB2 library
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_usr_io.mak,v $
#   Revision 1.1  2007/07/13 17:35:30  cs
#   Initial Revision
#
#   Revision 1.1  2005/11/23 09:23:12  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************


MAK_NAME=smb2_all_usr_io

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_IO_MAPPED \
           $(SW_PREFIX)MAC_USERSPACE \


MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/smb2.h

MAK_INP1=smb2_com$(INP_SUFFIX)
MAK_INP2=smb2_ich$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)\
		$(MAK_INP2)

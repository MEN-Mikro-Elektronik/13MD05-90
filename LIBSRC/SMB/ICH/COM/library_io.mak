#**************************  M a k e f i l e ********************************
#  
#         Author: dieter.pfeuffer@men.de
#          $Date: 2005/10/12 09:33:50 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for SMB_ICH library - i/o access variant
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_io.mak,v $
#   Revision 1.1  2005/10/12 09:33:50  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=smb_ich_io

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_IO_MAPPED

MAK_INCL=$(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/maccess.h \
		 $(MEN_INC_DIR)/men_typs.h \
		 $(MEN_INC_DIR)/oss.h \
		 $(MEN_INC_DIR)/sysmanagbus.h
		 
MAK_INP1=sysmanagbus_ich$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

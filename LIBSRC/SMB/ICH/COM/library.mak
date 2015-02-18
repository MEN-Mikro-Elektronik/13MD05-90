#**************************  M a k e f i l e ********************************
#  
#         Author: dieter.pfeuffer@men.de
#          $Date: 2005/10/12 09:33:49 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for SMB_ICH library
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  2005/10/12 09:33:49  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=smb_ich

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_INCL=$(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/maccess.h \
		 $(MEN_INC_DIR)/men_typs.h \
		 $(MEN_INC_DIR)/oss.h \
		 $(MEN_INC_DIR)/sysmanagbus.h
		 
MAK_INP1=sysmanagbus_ich$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

#**************************  M a k e f i l e ********************************
#  
#         Author: ds
#          $Date: 2000/09/08 10:26:40 $
#      $Revision: 1.2 $
#  
#    Description: makefile descriptor for SMB_PORT library (all sources)
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.2  2000/09/08 10:26:40  Schmidt
#   !!! ATTENTION !!! : smb_port now with memory access instead of i/o access
#                     : use smb_port_io for i/o access
#
#   Revision 1.1  2000/06/09 09:49:04  Schmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=smb_port

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_INCL=$(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/maccess.h \
		 $(MEN_INC_DIR)/men_typs.h \
		 $(MEN_INC_DIR)/oss.h \
		 $(MEN_INC_DIR)/sysmanagbus.h
		 
MAK_INP1=sysmanagbus_port$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)


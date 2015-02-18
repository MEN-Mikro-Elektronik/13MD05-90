#***************************  M a k e f i l e  *******************************
#  
#         Author: ds
#          $Date: 2008/09/16 18:41:18 $
#      $Revision: 1.4 $
#  
#    Description: Makefile definitions for the D203 BBIS driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.4  2008/09/16 18:41:18  CKauntz
#   Changed copyright
#
#   Revision 1.3  2007/06/21 13:02:07  ufranke
#   fixed
#    - building variants for VxWorks
#
#   Revision 1.2  2006/08/29 13:25:41  DPfeuffer
#   D203 define added
#
#   Revision 1.1  2003/01/28 16:17:20  dschmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuremberg, Germany 
#*****************************************************************************

MAK_NAME=d203

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)D203=D203 \
		   $(SW_PREFIX)D203_VARIANT=D203
		    	 	 
MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
	 	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/bb_d203.h	\
		 $(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_MOD_DIR)/brd.h		\
		 $(MEN_MOD_DIR)/d203_int.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/maccess.h	\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/modcom.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_d203$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)


#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2001/04/11 11:05:49 $
#      $Revision: 1.1 $
#  
#    Description: Makefile definitions for the A21 BBIS driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.1  2001/04/11 11:05:49  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=a21

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
	 	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED	 	 

MAK_INCL=$(MEN_INC_DIR)/bb_a21.h	\
		 $(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_MOD_DIR)/a21_int.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_a21$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

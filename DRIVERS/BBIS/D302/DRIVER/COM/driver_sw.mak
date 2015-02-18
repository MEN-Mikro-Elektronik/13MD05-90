#***************************  M a k e f i l e  *******************************
#  
#         Author: ds
#          $Date: 2001/11/14 09:55:18 $
#      $Revision: 1.2 $
#  
#    Description: Makefile definitions for the D302_SW BBIS driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_sw.mak,v $
#   Revision 1.2  2001/11/14 09:55:18  Schmidt
#   MAK_INCL was wrong
#
#   Revision 1.1  2001/11/12 15:48:18  Schmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=d302_sw

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)MAC_BYTESWAP \
		   $(SW_PREFIX)D302_VARIANT=D302_SW 	 	 

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
	 	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX) 	 

MAK_INCL=$(MEN_INC_DIR)/bb_d302.h	\
		 $(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_MOD_DIR)/brd.h		\
		 $(MEN_MOD_DIR)/d302_int.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/maccess.h	\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_d302$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)


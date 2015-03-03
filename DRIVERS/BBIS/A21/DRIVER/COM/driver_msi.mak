#***************************  M a k e f i l e  *******************************
#  
#         Author: ts
#  
#    Description: Makefile definitions for the A21 BBIS driver, customer
#                 specific A21 Variant      
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuremberg, Germany 
#*****************************************************************************

MAK_NAME=a21_msi

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
	 	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
           $(SW_PREFIX)A21_USE_MSI

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

#***************************  M a k e f i l e  *******************************
#  
#         Author: ub/ts
#  
#    Description: Makefile definitions for the CHAMELEON BBIS driver
#                 Compile chameleon driver to read IRQ to use from
#                 PCI config space instead using the table inside FPGA. Also
#                 pass the MSI define for A21 MSI variant (customer specific)
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuremberg, Germany 
#*****************************************************************************

MAK_NAME=chameleon_pcitbl_msi

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_io$(LIB_SUFFIX) \
	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)CHAMELEON_USE_PCITABLE \
	   $(SW_PREFIX)CHAMELEON_USE_A21_MSI

MAK_INCL=$(MEN_INC_DIR)/bb_chameleon.h	\
	 $(MEN_INC_DIR)/bb_defs.h	\
	 $(MEN_INC_DIR)/bb_entry.h	\
	 $(MEN_INC_DIR)/dbg.h		\
	 $(MEN_INC_DIR)/desc.h		\
	 $(MEN_INC_DIR)/mdis_api.h	\
	 $(MEN_INC_DIR)/mdis_com.h	\
	 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_chameleon$(INP_SUFFIX)
MAK_INP2=io_access$(INP_SUFFIX)
MAK_INP=$(MAK_INP1) $(MAK_INP2)

#***************************  M a k e f i l e  *******************************
#  
#         Author: ds
#          $Date: 2004/09/06 13:02:54 $
#      $Revision: 1.2 $
#  
#    Description: Makefile definitions for the ISA BBIS driver - memory access
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.2  2004/09/06 13:02:54  dpfeuffer
#   unused ISA switch removed
#
#   Revision 1.1  2000/02/28 14:19:33  Schmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=isa

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
	 	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)
	 	 
MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED	 

MAK_INCL=$(MEN_INC_DIR)/bb_isa.h	\
		 $(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_isa$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

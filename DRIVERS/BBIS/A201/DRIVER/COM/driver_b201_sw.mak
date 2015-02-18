#***************************  M a k e f i l e  *******************************
#
#         Author: dp
#          $Date: 2004/07/30 11:35:54 $
#      $Revision: 1.2 $
#
#    Description: makefile descriptor for B201_SW BBIS driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_b201_sw.mak,v $
#   Revision 1.2  2004/07/30 11:35:54  ufranke
#   fixed
#    - function name GetEntry
#
#   Revision 1.1  2004/06/21 16:32:42  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=b201_sw

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id_sw$(LIB_SUFFIX)   \

MAK_INCL=$(MEN_INC_DIR)/bb_a201.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/maccess.h     \
         $(MEN_INC_DIR)/desc.h        \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_com.h    \
         $(MEN_INC_DIR)/bb_defs.h     \
         $(MEN_INC_DIR)/bb_entry.h    \
         $(MEN_INC_DIR)/dbg.h         \
         $(MEN_INC_DIR)/modcom.h      \

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED	\
		   $(SW_PREFIX)MAC_BYTESWAP     \
		   $(SW_PREFIX)B201=B201		\
		   $(SW_PREFIX)A201_VARIANT=B201_SW

MAK_OPTIM=$(OPT_1)

MAK_INP1=bb_a201$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)



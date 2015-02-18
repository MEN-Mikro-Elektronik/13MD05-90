#**************************  M a k e f i l e ********************************
#  
#         Author: kp
#          $Date: 2004/04/07 15:08:28 $
#      $Revision: 1.2 $
#  
#    Description: makefile descriptor for MDIS LL-Driver 
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_sw.mak,v $
#   Revision 1.2  2004/04/07 15:08:28  cs
#   cosmetics for MDIS4/2004 conformity
#   changed ID-library to ID_SW-library
#
#   Revision 1.1  2001/08/16 10:09:03  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=m43_sw

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id_sw$(LIB_SUFFIX)      \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/mdis_com.h   \
         $(MEN_INC_DIR)/men_typs.h   \
         $(MEN_INC_DIR)/oss.h        \
         $(MEN_INC_DIR)/mdis_err.h   \
         $(MEN_INC_DIR)/maccess.h    \
         $(MEN_INC_DIR)/desc.h       \
         $(MEN_INC_DIR)/ll_defs.h    \
         $(MEN_INC_DIR)/mdis_api.h   \
         $(MEN_INC_DIR)/ll_entry.h   \
         $(MEN_INC_DIR)/modcom.h     \
         $(MEN_INC_DIR)/m43_drv.h    \
         $(MEN_INC_DIR)/dbg.h


MAK_SWITCH=$(SW_PREFIX)MAC_BYTESWAP \
		   $(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)ID_SW \
		   $(SW_PREFIX)M43_SW

MAK_INP1=m43_drv$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

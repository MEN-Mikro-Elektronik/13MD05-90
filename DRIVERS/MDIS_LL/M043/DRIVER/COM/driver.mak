#**************************  M a k e f i l e ********************************
#  
#         Author: ds
#          $Date: 2004/04/07 15:08:21 $
#      $Revision: 1.3 $
#  
#    Description: makefile descriptor for MDIS LL-Driver 
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.3  2004/04/07 15:08:21  cs
#   cosmetics to reach MDIS4/2004 conformity
#     removed MAK_OPTIM=$(OPT_1)
#     added   MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED
#
#   Revision 1.2  1998/07/16 16:02:59  Schmidt
#   new order libs, cosmetics
#
#   Revision 1.1  1998/03/09 09:38:36  Schmidt
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=m43

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)      \
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

MAK_INP1=m43_drv$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

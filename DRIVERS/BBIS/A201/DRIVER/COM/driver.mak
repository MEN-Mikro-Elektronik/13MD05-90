#***************************  M a k e f i l e  *******************************
#
#         Author: uf
#          $Date: 2004/07/30 11:35:20 $
#      $Revision: 1.6 $
#
#    Description: makefile descriptor for A201 BBIS driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.6  2004/07/30 11:35:20  ufranke
#   fixed
#    - function name GetEntry
#
#   Revision 1.5  2004/06/21 16:32:36  dpfeuffer
#   id library added
#
#   Revision 1.4  1999/05/25 10:59:48  kp
#   added MAK_SWITCH
#
#   Revision 1.3  1999/03/19 13:33:28  Schmidt
#   MAK_NAME renamed to a201
#
#   Revision 1.2  1998/05/28 15:02:59  see
#   missing dbg.h added
#   missing DBG lib added
#
#   Revision 1.1  1998/02/19 13:28:39  franke
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=a201

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)   \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)     \

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

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED 		\
		   $(SW_PREFIX)A201=A201			\
		   $(SW_PREFIX)A201_VARIANT=A201

MAK_OPTIM=$(OPT_1)

MAK_INP1=bb_a201$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)




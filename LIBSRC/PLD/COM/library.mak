#**************************  M a k e f i l e ********************************
#  
#         Author: kp
#          $Date: 1999/04/26 15:14:46 $
#      $Revision: 1.2 $
#  
#    Description: makefile descriptor for PLD library (all sources)
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.2  1999/04/26 15:14:46  Schmidt
#   includes added, MAK_OPTIM removed
#
#   Revision 1.1  1999/04/14 15:33:15  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************


MAK_NAME=pld

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_INCL=$(MEN_MOD_DIR)/pld_var.h \
         $(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/pld_load.h

MAK_INP1=errmsg$(INP_SUFFIX)
MAK_INP2=flex10k$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) 


#**************************  M a k e f i l e ********************************
#  
#         Author: ds
#          $Date: 1999/04/26 15:14:51 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for PLD library (all sources)
#
#                 variation pld_sw for swapped access
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_sw.mak,v $
#   Revision 1.1  1999/04/26 15:14:51  Schmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************


MAK_NAME=pld_sw

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)PLD_SW

MAK_INCL=$(MEN_MOD_DIR)/pld_var.h \
         $(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/pld_load.h
                          
MAK_INP1=errmsg$(INP_SUFFIX)
MAK_INP2=flex10k$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) 

#**************************  M a k e f i l e ********************************
#  
#         Author: ts
#          $Date: 2007/11/16 15:55:43 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for Z077/87 Driver, unswapped
#                      
#---------------------------------[ History ]---------------------------------
#
# $Log: driver.mak,v $
# Revision 1.1  2007/11/16 15:55:43  ts
# Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=lx_z77

MAK_LIBS=

MAK_INCL= $(MEN_INC_DIR)/men_typs.h   \
          $(MEN_INC_DIR)/oss.h        \
          $(MEN_INC_DIR)/dbg.h        \
          $(MEN_INC_DIR)/mdis_err.h   \
          $(MEN_INC_DIR)/men_chameleon.h \
          $(MEN_INC_DIR)/sysparam2.h \

MAK_OPTIM=$(OPT_1)

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED 

MAK_INP1=men_16z077_eth$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

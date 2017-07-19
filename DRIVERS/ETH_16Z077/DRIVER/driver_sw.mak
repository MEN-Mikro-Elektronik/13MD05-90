#**************************  M a k e f i l e ********************************
#  
#         Author: ts
#          $Date: 2007/11/16 15:55:44 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for 16z077/87 Ethernet IP Core,
#                 swapped Variant
#                      
#---------------------------------[ History ]---------------------------------
#
# $Log: driver_sw.mak,v $
# Revision 1.1  2007/11/16 15:55:44  ts
# Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2007 by MEN mikro elektronik GmbH, Nuremberg, Germany 
#*****************************************************************************

MAK_NAME=lx_z77_sw

MAK_LIBS=

MAK_INCL= $(MEN_INC_DIR)/men_typs.h   \
          $(MEN_INC_DIR)/oss.h        \
          $(MEN_INC_DIR)/dbg.h        \
          $(MEN_INC_DIR)/mdis_err.h   \
          $(MEN_INC_DIR)/men_chameleon.h \
          $(MEN_INC_DIR)/sysparam2.h \

MAK_OPTIM=$(OPT_1)

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)MAC_BYTESWAP   \
		   $(SW_PREFIX)ID_SW

MAK_INP1=men_16z077_eth$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

#**************************  M a k e f i l e ********************************
#  
#         Author: ds
#          $Date: 2004/04/07 15:08:33 $
#      $Revision: 1.3 $
#  
#    Description: Makefile definitions for the m43_ex1 example program
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.3  2004/04/07 15:08:33  cs
#   cosmetics for MDIS4/2004 conformity
#     added $(MEN_INC_DIR)/mdis_err.h
#     removed MAK_OPTIM=$(OPT_1)
#
#   Revision 1.2  1998/07/16 16:03:16  Schmidt
#   cosmetics
#
#   Revision 1.1  1998/03/09 09:38:50  Schmidt
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=m43_ex1

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/mdis_api.h \
         $(MEN_INC_DIR)/mdis_err.h \
         $(MEN_INC_DIR)/usr_oss.h

MAK_INP1=m43_ex1$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2003/06/06 09:09:42 $
#      $Revision: 1.2 $
#                      
#    Description: Makefile descriptor file for MDIS_API lib
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.2  2003/06/06 09:09:42  kp
#   new name m_errstr
#
#   Revision 1.1  2001/01/19 14:58:45  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=mdis_api

MAK_SWITCH=


MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
		 $(MEN_INC_DIR)/mdis_err.h \
		 $(MEN_INC_DIR)/mdis_api.h \
		 $(MEN_INC_DIR)/mdis_ers.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/mdis_mk.h


MAK_INP01 = mdis_api$(INP_SUFFIX)
MAK_INP02 = m_errstr$(INP_SUFFIX)

MAK_INP = $(MAK_INP01) $(MAK_INP02)




#**************************  M a k e f i l e ********************************
#  
#        $Author: franke $
#          $Date: 1998/03/10 12:21:17 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for library 
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  1998/03/10 12:21:17  franke
#   Added by mcvs
#
#   Revision 1.1  1998/02/04 13:55:51  ds
#   initial revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************


MAK_NAME=mbuf

MAK_LIBS=

MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/mdis_err.h \
         $(MEN_INC_DIR)/mbuf.h \
         $(MEN_INC_DIR)/mdis_api.h

MAK_OPTIM=$(OPT_1)

MAK_INP1=mbuf$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

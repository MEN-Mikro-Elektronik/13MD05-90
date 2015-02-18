#***************************  M a k e f i l e  *******************************
#  
#        $Author: franke $
#          $Date: 1998/03/10 12:21:28 $
#      $Revision: 1.1 $
#        $Header: /dd2/CVSR/COM/LIBSRC/DESC/COM/library.mak,v 1.1 1998/03/10 12:21:28 franke Exp $
#                      
#    Description: makefile descriptor file for common
#                 modules MDIS 4.0   e.g. low level driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  1998/03/10 12:21:28  franke
#   Added by mcvs
#
#   Revision 1.1  1998/02/04 16:07:16  uf
#   initial
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=desc

MAK_LIBS=


MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/desctyps.h    \
         $(MEN_INC_DIR)/desc.h        \


MAK_OPTIM=$(OPT_1)

MAK_INP1=desc$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

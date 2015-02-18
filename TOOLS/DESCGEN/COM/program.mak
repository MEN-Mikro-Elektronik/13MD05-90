#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 1999/04/28 16:27:03 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for DESCGEN program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  1999/04/28 16:27:03  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=descgen

MAK_LIBS=

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/desctyps.h	  \
		 $(MEN_MOD_DIR)/descgen.h	  \
		 $(MEN_MOD_DIR)/os9k.h		

MAK_INP1=descgen$(INP_SUFFIX)
MAK_INP2=binary$(INP_SUFFIX)
MAK_INP3=csource$(INP_SUFFIX)
MAK_INP4=os9$(INP_SUFFIX)
MAK_INP5=os9k$(INP_SUFFIX)
MAK_INP6=parse$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) $(MAK_INP3) $(MAK_INP4) $(MAK_INP5) \
	    $(MAK_INP6) 






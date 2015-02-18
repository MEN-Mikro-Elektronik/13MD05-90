#***************************  M a k e f i l e  *******************************
#
#         Author: dieter.pfeuffer@men.de
#          $Date: 2008/03/31 11:31:42 $
#      $Revision: 1.2 $
#
#    Description: Makefile definitions for CHA_UT tool
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.2  2008/03/31 11:31:42  DPfeuffer
#   changed to run unit test
#
#   Revision 1.1  2005/04/29 14:45:26  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=cha_ut

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED $(SW_PREFIX)CHAM_UNITTEST

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/dbg.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/usr_oss.h	\
         $(MEN_INC_DIR)/chameleon.h

MAK_INP1=chameleonv2$(INP_SUFFIX)
MAK_INP2=chameleon$(INP_SUFFIX)
MAK_INP3=chameleon_strings$(INP_SUFFIX)
MAK_INP4=cha_emulate$(INP_SUFFIX)
MAK_INP5=cha_ut$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) $(MAK_INP3) $(MAK_INP4) $(MAK_INP5)


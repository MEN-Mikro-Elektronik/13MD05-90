#***************************  M a k e f i l e  *******************************
#
#         Author: ts
#          $Date: 2008/09/15 13:15:41 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for the FILETYPE mcvs tool
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  2008/09/15 13:15:41  GLeonhardt
#   Initial Revision
#
#   Revision 1.1  2008/09/01 16:04:54  ts
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Germany
#*****************************************************************************

MAK_NAME=hwbug

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/uti$(LIB_SUFFIX)\

MAK_INCL=$(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/usr_utl.h   \
         $(MEN_INC_DIR)/testutil.h  

MAK_INP1=hwbug$(INP_SUFFIX)
MAK_INP2=display$(INP_SUFFIX)
MAK_INP3=history$(INP_SUFFIX)
MAK_INP4=change$(INP_SUFFIX)
MAK_INP5=linux$(INP_SUFFIX)

MAK_SWITCH=-DLINUX

MAK_LD_SWITCH=-Wl,-elf2flt -Wl,-Map -Wl,hwbug.map

MAK_INP=$(MAK_INP1) $(MAK_INP2) $(MAK_INP3) $(MAK_INP4) $(MAK_INP5) 

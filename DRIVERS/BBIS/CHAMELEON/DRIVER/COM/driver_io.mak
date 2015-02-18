#***************************  M a k e f i l e  *******************************
#
#         Author: dieter.pfeuffer@men.de
#          $Date: 2011/01/19 11:02:50 $
#      $Revision: 1.3 $
#
#    Description: Makefile definitions for the CHAMELEON BBIS (io access)
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_io.mak,v $
#   Revision 1.3  2011/01/19 11:02:50  dpfeuffer
#   R: still support io variant build
#   M: libs, switches and sources modified
#
#   Revision 1.2  2009/10/14 09:39:24  CKauntz
#   R: Linker Error: cant find CHAM_InitMem
#   M: Added CHAMV2_VARIANT switch to use CHAM_InitIo
#
#   Revision 1.1  2005/07/27 12:54:39  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=chameleon_io

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)  \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)   \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_io$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)OLD_IO_VARIANT

MAK_INCL=$(MEN_INC_DIR)/bb_chameleon.h  \
         $(MEN_INC_DIR)/bb_defs.h   \
         $(MEN_INC_DIR)/bb_entry.h  \
         $(MEN_INC_DIR)/dbg.h       \
         $(MEN_INC_DIR)/desc.h      \
         $(MEN_INC_DIR)/mdis_api.h  \
         $(MEN_INC_DIR)/mdis_com.h  \
         $(MEN_INC_DIR)/mdis_err.h  \
         $(MEN_INC_DIR)/men_typs.h  \
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_chameleon$(INP_SUFFIX)
MAK_INP2=io_access$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2)

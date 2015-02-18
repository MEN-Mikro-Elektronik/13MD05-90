#***************************  M a k e f i l e  *******************************
#
#         Author: dieter.pfeuffer@men.de
#          $Date: 2011/01/19 11:02:55 $
#      $Revision: 1.3 $
#
#    Description: Makefile definitions for the CHAMELEON BBIS
#                 (IRQ from PCI-tbl, io access)
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_pcitbl_io.mak,v $
#   Revision 1.3  2011/01/19 11:02:55  dpfeuffer
#   R: still support io variant build
#   M: libs, switches and sources modified
#
#   Revision 1.2  2010/05/21 09:58:38  ts
#   R: IOmapped GPIO on F11S not accessible although IO .mak versions used
#   M: added CHAMV2_Variant define Io as in driver_io.mak
#
#   Revision 1.1  2005/08/03 11:22:43  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=chameleon_pcitbl_io

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)  \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)   \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_io$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)CHAMELEON_USE_PCITABLE \
           $(SW_PREFIX)OLD_IO_VARIANT

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

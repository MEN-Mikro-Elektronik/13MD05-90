#***************************  M a k e f i l e  *******************************
#
#         Author: Christian.Schuster@men.de
#          $Date: 2014/01/09 15:38:00 $
#      $Revision: 2.19 $
#
#    Description: linux makefile descriptor file for fpga_load
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 2.19  2014/01/09 15:38:00  awerner
#   R: Only an Library include example for ELINOS Project
#   M: Added example for MDIS Selfhosted project
#
#   Revision 2.18  2013/06/21 16:46:08  MRoth
#   R: changes for compiling as part of an MDIS project
#
#   Revision 2.16  2009/06/03 08:43:43  CRuff
#   R: libsmb2_all_usr only relevant for VxWorks
#   M: Removed libsmb2_all_usr from MAK_LIBS and added additional section
#      for VxWorks
#
#   Revision 2.15  2009/03/05 09:53:11  CKauntz
#   R: Commented line in middle of compiling order
#   M: Set commented line at end of block
#
#   Revision 2.14  2009/01/12 16:58:41  CKauntz
#   R: QNX section can not be used in other OS
#   M: Commented QNX section
#
#   Revision 2.13  2008/11/28 18:04:36  gvarlet
#   R : Do not call the right men-oss library to build statically
#   M : change the path and the library called from men_oss to ../men-oss
#
#   Revision 2.12  2008/09/01 14:27:40  gvarlet
#   R: USE_VME_SERVER not defined in every case
#   M: Check USE_VME_SERVER is set to yes (work in every cases)
#
#   Revision 2.11  2008/07/30 15:54:23  gvarlet
#   R: No support for QNX
#   M: Support for QNX added
#
#   Revision 2.10  2008/03/19 16:11:11  CKauntz
#   added: st_stubs and stm25p32.c for the SPI Flash M25P32
#
#   Revision 2.9  2007/10/17 17:21:55  CKauntz
#   Added Switch NO_IDPROM_CHECK for A404 without EEPROM assembled
#
#   Revision 2.8  2007/08/07 14:13:49  JWu
#   removed smb2_all_usr lib
#
#   Revision 2.7  2007/07/13 17:42:01  cs
#   added:
#     - SMB support by default
#     - smb2_all_usr lib (use SMB21 lib with MAC_USER_SPACE switch set)
#   removed DBG switch
#
#   Revision 2.6  2007/07/09 20:13:06  CKauntz
#   added: stubs for swapped and non-swapped versions
#          istratapc28fxxxp30 flash source files
#
#   Revision 2.5  2005/12/12 14:25:40  cschuster
#   Copyright line changed
#
#   Revision 2.4  2005/11/24 18:17:45  cschuster
#   Added support for A404/A500 (Flash access over SMBus and PLD)
#
#   Revision 2.3  2005/07/08 17:49:52  cs
#   use chameleon (+oss_usr, +dbg_usr) library
#   add VME bus support
#
#   Revision 2.2  2005/01/21 13:36:46  cs
#   switch Z100_IO_MAPPED_EN moved to library.mak
#
#   Revision 2.1  2004/12/23 15:10:48  cs
#   Initial Revision
#
#   Revision 1.2  2004/12/13 18:03:37  cs
#   added comment for MAC_IO_MAPPED_EN switch
#
#   Revision 1.1  2004/11/30 18:04:59  cs
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
# (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
#*****************************************************************************

MAK_NAME=fpga_load

MAK_SWITCH= \
            $(SW_PREFIX)Z100_CONFIG_VME \
            $(SW_PREFIX)MAC_USERSPACE \
            $(SW_PREFIX)Z100_IO_ACCESS_ENABLE \
#            $(SW_PREFIX)Z100_CONFIG_SMB \
#            $(SW_PREFIX)DBG \
#            $(SW_PREFIX)NO_IDPROM_CHECK

MAK_LIBS= 	$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)			\
			$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX)			\
        	$(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_usr$(LIB_SUFFIX)		\
        	$(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_usr_io$(LIB_SUFFIX)	\
        	$(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_usr_sw$(LIB_SUFFIX)	\
        	$(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_usr_io_sw$(LIB_SUFFIX)\
        	$(LIB_PREFIX)$(MEN_LIB_DIR)/oss_usr$(LIB_SUFFIX)			\
        	$(LIB_PREFIX)$(MEN_LIB_DIR)/dbg_usr$(LIB_SUFFIX)			\

# for VXWORKS, in case BSP does not include the SMB2 library:
#MAK_LIBS+=	$(LIB_PREFIX)$(MEN_LIB_DIR)/smb2_all_usr$(LIB_SUFFIX) \

# for LINUX (ELINOS)::
#MAK_LIBS+=	$(LIB_PREFIX)$(ELINOS_PROJECT)/src/pciutils/lib/libpci$(LIB_SUFFIX)\
#			$(LIB_PREFIX)$(MEN_LIB_DIR)/vme4l_api$(LIB_SUFFIX) \

# for LINIX (SELFHOSTED)_
#MAK_LIBS+=	$(LIB_PREFIX)$(MEN_LIB_DIR)/pciutils$(LIB_SUFFIX)\
#			$(LIB_PREFIX)$(MEN_LIB_DIR)/vme4l_api$(LIB_SUFFIX) \

# check for QNX:
#ifdef MEN_QNX_DIR
#	MAK_LIBS +=	$(LIB_PREFIX)$(MEN_LIB_DIR)/../men-oss$(DBGSUFFIX)$(LIB_SUFFIX)
#	ifeq ($(USE_VME_SERVER),yes)
#		MAK_LIBS +=	$(LIB_PREFIX)$(MEN_LIB_DIR)/../men-libc$(LIB_SUFFIX)
#	endif
#endif

MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
		 $(MEN_INC_DIR)/smb2.h \
		 $(MEN_INC_DIR)/usr_oss.h \
		 $(MEN_INC_DIR)/usr_err.h \
		 $(MEN_INC_DIR)/usr_utl.h \
		 $(MEN_INC_DIR)/fpga_header.h \
         $(MEN_MOD_DIR)/../COM/fpga_load.h \
         $(MEN_MOD_DIR)/../COM/hw_acc.c \
         $(MEN_MOD_DIR)/../COM/istratapc28fxxxp30.c \
		 $(MEN_MOD_DIR)/../COM/am29lvxxx.c \
		 $(MEN_MOD_DIR)/../COM/am29lvxxx_smb.c \
         $(MEN_MOD_DIR)/../COM/stm25p32.c \

MAK_INP1=_amd_stub$(INP_SUFFIX)
MAK_INP2=_amd_sw_stub$(INP_SUFFIX)
MAK_INP3=_amd_smb_stub$(INP_SUFFIX)
MAK_INP4=_amd_smb_sw_stub$(INP_SUFFIX)
MAK_INP5=_istrata_stub$(INP_SUFFIX)
MAK_INP6=_istrata_sw_stub$(INP_SUFFIX)
MAK_INP7=_istrata_mmod_stub$(INP_SUFFIX)
MAK_INP8=_istrata_mmod_sw_stub$(INP_SUFFIX)
MAK_INP9=fpga_load$(INP_SUFFIX)
MAK_INP10=flash_com$(INP_SUFFIX)
MAK_INP11=hw_acc_io$(INP_SUFFIX)
MAK_INP12=_st_stub$(INP_SUFFIX)
MAK_INP13=_st_sw_stub$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)\
		$(MAK_INP2)\
		$(MAK_INP3)\
		$(MAK_INP4)\
		$(MAK_INP5)\
		$(MAK_INP6)\
		$(MAK_INP7)\
		$(MAK_INP8)\
		$(MAK_INP9)\
		$(MAK_INP10)\
		$(MAK_INP11)\
        $(MAK_INP12)\
		$(MAK_INP13)\


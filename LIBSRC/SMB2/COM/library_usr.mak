#**************************  M a k e f i l e ********************************
#
#         Author: dieter.pfeuffer@men.de
#          $Date: 2007/07/13 17:35:26 $
#      $Revision: 1.1 $
#
#    Description: makefile descriptor for SMB2 library
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_usr.mak,v $
#   Revision 1.1  2007/07/13 17:35:26  cs
#   Initial Revision
#
#   Revision 1.4  2006/02/22 16:02:02  DPfeuffer
#   smb2_ich.c removed because device is always i/o mapped
#
#   Revision 1.3  2006/02/07 19:14:02  cschuster
#   added smb2_mgt5200.c
#
#   Revision 1.2  2005/12/12 10:51:54  cschuster
#   added smb2_menz001.c to list of MAK_INP
#
#   Revision 1.1  2005/11/23 09:23:09  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************


MAK_NAME=smb2_all_usr

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
           $(SW_PREFIX)MAC_USERSPACE \

MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/smb2.h

MAK_INP1=smb2_com$(INP_SUFFIX)
MAK_INP2=smb2_menz001$(INP_SUFFIX)
MAK_INP3=smb2_mgt5200$(INP_SUFFIX)
MAK_INP4=
MAK_INP5=
MAK_INP6=
MAK_INP7=
MAK_INP8=
MAK_INP9=

MAK_INP=$(MAK_INP1)\
		$(MAK_INP2)\
		$(MAK_INP3)\
		$(MAK_INP4)\
		$(MAK_INP5)\
		$(MAK_INP6)\
		$(MAK_INP7)\
		$(MAK_INP8)\
		$(MAK_INP9)


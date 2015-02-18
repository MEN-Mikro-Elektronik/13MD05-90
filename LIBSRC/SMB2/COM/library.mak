#**************************  M a k e f i l e ********************************
#
#         Author: dieter.pfeuffer@men.de
#          $Date: 2012/01/30 11:49:59 $
#      $Revision: 1.7 $
#
#    Description: makefile descriptor for SMB2 library
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.7  2012/01/30 11:49:59  dpfeuffer
#   R: support for FCH ASF SMB controller
#   M: smb2_ich.c added
#
#   Revision 1.6  2009/10/09 15:00:33  dpfeuffer
#   R: smb2_portcb.c not used for standard smb2 lib (not in smb2 fileset)
#   M: smb2_portcb.c removed (undo of last check-in)
#
#   Revision 1.5  2009/10/05 10:56:59  CKauntz
#   R: portcb file has not been compiled
#   M: Added smb2_portcb file
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


MAK_NAME=smb2

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/smb2.h

MAK_INP1=smb2_com$(INP_SUFFIX)
MAK_INP2=smb2_menz001$(INP_SUFFIX)
MAK_INP3=smb2_mgt5200$(INP_SUFFIX)
MAK_INP4=smb2_ich$(INP_SUFFIX)
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




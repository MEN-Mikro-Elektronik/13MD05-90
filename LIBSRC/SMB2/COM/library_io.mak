#**************************  M a k e f i l e ********************************
#  
#         Author: dieter.pfeuffer@men.de
#          $Date: 2010/12/20 13:14:54 $
#      $Revision: 1.3 $
#  
#    Description: makefile descriptor for SMB2 library
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_io.mak,v $
#   Revision 1.3  2010/12/20 13:14:54  dpfeuffer
#   R: IO mapped variants for 16Z001 SMB ctrl. required
#   M: smb2_menz001.c added
#
#   Revision 1.2  2008/10/20 10:42:56  MSoehnlein
#   added SCH support
#
#   Revision 1.1  2005/11/23 09:23:12  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************


MAK_NAME=smb2_io

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)MAC_IO_MAPPED

MAK_INCL=$(MEN_INC_DIR)/men_typs.h \
         $(MEN_INC_DIR)/oss.h \
         $(MEN_INC_DIR)/dbg.h \
         $(MEN_INC_DIR)/maccess.h \
         $(MEN_INC_DIR)/smb2.h

MAK_INP1=smb2_com$(INP_SUFFIX)
MAK_INP2=smb2_ich$(INP_SUFFIX)
MAK_INP3=smb2_sch$(INP_SUFFIX)
MAK_INP4=smb2_menz001$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)\
		$(MAK_INP2) \
		$(MAK_INP3) \
		$(MAK_INP4)


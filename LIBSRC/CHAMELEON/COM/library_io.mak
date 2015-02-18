#***************************  M a k e f i l e  *******************************
#  
#         Author: dieter.pfeuffer@men.de
#          $Date: 2011/09/10 11:42:31 $
#      $Revision: 2.3 $
#                      
#    Description: Makefile descriptor file for CHAMELEON lib (io access)
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_io.mak,v $
#   Revision 2.3  2011/09/10 11:42:31  ts
#   R: build of IO mapped chameleon on F19 failed in vxWorks 6.9
#   M: added missing Macro definition VX_SYS_IO..
#
#   Revision 2.2  2007/07/11 09:32:04  CKauntz
#   only Chameleon V2 API exported
#   added CHAMV2_VARIANT
#
#   Revision 2.1  2005/04/29 14:45:16  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=chameleon_io

MAK_INCL=$(MEN_MOD_DIR)/chameleon_int.h \
		 $(MEN_MOD_DIR)/chameleon.c     \
		 $(MEN_INC_DIR)/men_typs.h    	\
		 $(MEN_INC_DIR)/chameleon.h		\
		 $(MEN_INC_DIR)/maccess.h		\
		 $(MEN_INC_DIR)/oss.h

MAK_SWITCH=$(SW_PREFIX)MAC_IO_MAPPED      \
           $(SW_PREFIX)CHAM_VARIANT=IO    \
           $(SW_PREFIX)CHAMV2_VARIANT=Io  \
		   $(SW_PREFIX)VX_SYS_IO_FUNCT_ACCESS

MAK_INP1 = chameleonv2$(INP_SUFFIX)
MAK_INP2 = chameleon_strings$(INP_SUFFIX)

MAK_INP  = $(MAK_INP1) \
		   $(MAK_INP2)

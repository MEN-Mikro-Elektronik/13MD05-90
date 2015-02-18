#***************************  M a k e f i l e  *******************************
#  
#         Author: ub
#          $Date: 2012/02/08 09:21:58 $
#      $Revision: 1.8 $
#                      
#    Description: Makefile descriptor file for CHAMELEON lib
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.8  2012/02/08 09:21:58  dpfeuffer
#   R: compatibility problems under Windows and QNX
#   M: undo of last modification
#
#   Revision 1.7  2011/09/10 11:41:22  ts
#   R: unresolved externals CHAM_InitMem occured
#   M: added define for chameleon mem type
#
#   Revision 1.6  2007/07/11 09:31:57  CKauntz
#   only Chameleon V2 API exported
#   added CHAMV2_VARIANT
#
#   Revision 1.5  2005/07/27 11:39:01  dpfeuffer
#   CHAM_VARIANT=MEM removed due to compatibility problems
#
#   Revision 1.4  2005/04/29 14:45:14  dpfeuffer
#   updated for Chameleon-V2
#
#   Revision 1.3  2004/05/26 10:54:09  dpfeuffer
#   MAK_INCL fixed
#
#   Revision 1.2  2004/05/24 11:31:45  dpfeuffer
#   chameleon_strings.c added
#
#   Revision 1.1  2003/02/14 10:49:15  ub
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=chameleon

MAK_INCL=$(MEN_MOD_DIR)/chameleon_int.h \
		 $(MEN_MOD_DIR)/chameleon.c     \
		 $(MEN_INC_DIR)/men_typs.h    	\
		 $(MEN_INC_DIR)/chameleon.h		\
		 $(MEN_INC_DIR)/maccess.h		\
		 $(MEN_INC_DIR)/oss.h

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED    \
           $(SW_PREFIX)CHAMV2_VARIANT=Mem

MAK_INP1 = chameleonv2$(INP_SUFFIX)
MAK_INP2 = chameleon_strings$(INP_SUFFIX)

MAK_INP  = $(MAK_INP1) \
		   $(MAK_INP2)



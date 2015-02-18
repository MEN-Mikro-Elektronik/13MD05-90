#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2012/02/08 09:20:15 $
#      $Revision: 1.7 $
#  
#    Description: Makefile definitions for the CHAMELEON BBIS driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.7  2012/02/08 09:20:15  dpfeuffer
#   R: rollback of LIBSRC\CHAMELEON\COM\library.mak
#   M: undo of last modification
#
#   Revision 1.6  2012/02/06 11:38:34  dpfeuffer
#   R: LIBSRC\CHAMELEON\COM\library.mak: added CHAM_VARIANT=MEM changes
#      function names of CHAM_ModCodeToDevId and CHAM_DevIdToName
#   M: CHAM_VARIANT=MEM added here too to resolve external symbols
#
#   Revision 1.5  2011/01/19 11:02:48  dpfeuffer
#   R: chameleon BBIS shall support mem and io mapped chameleon units
#   M: libs, switches and sources modified
#
#   Revision 1.4  2007/06/12 10:22:56  aw
#   changed library sequence
#
#   Revision 1.3  2004/06/21 16:02:06  dpfeuffer
#   cosmetics
#
#   Revision 1.2  2004/05/24 10:25:26  dpfeuffer
#   chameleonsub.l now in driver_os9.mak
#
#   Revision 1.1  2003/02/03 10:44:37  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=chameleon

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_io$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=	 

MAK_INCL=$(MEN_INC_DIR)/bb_chameleon.h	\
		 $(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_chameleon$(INP_SUFFIX)
MAK_INP2=io_access$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2)






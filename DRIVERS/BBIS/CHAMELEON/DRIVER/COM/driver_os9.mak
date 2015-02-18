#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2011/01/19 11:02:57 $
#      $Revision: 1.2 $
#  
#    Description: Makefile definitions for the CHAMELEON BBIS driver
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_os9.mak,v $
#   Revision 1.2  2011/01/19 11:02:57  dpfeuffer
#   R: chameleon BBIS shall support mem and io mapped chameleon units
#   M: switches and sources modified, ToDo: MAK_LIBS must be extended
#
#   Revision 1.1  2004/05/24 10:25:28  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=chameleon

##!!! chameleonsub.l is OS-9 specific!
## ToDo: chameleonsub_io.l must be created and added! 
MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleonsub.l \
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

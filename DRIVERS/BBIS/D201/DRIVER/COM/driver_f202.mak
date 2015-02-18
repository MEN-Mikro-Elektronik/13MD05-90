#***************************  M a k e f i l e  *******************************
#  
#         Author: ds
#          $Date: 2004/03/10 17:31:37 $
#      $Revision: 1.4 $
#  
#    Description: makefile descriptor for F202 Base Board Handler
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_f202.mak,v $
#   Revision 1.4  2004/03/10 17:31:37  dpfeuffer
#   id lib/include added
#
#   Revision 1.3  2000/03/09 12:19:12  kp
#   changed F202 switch to _F202
#
#   Revision 1.2  1999/11/03 10:14:45  Gromann
#   bugfix dependencies
#
#   Revision 1.1  1999/09/08 09:30:26  Schmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=f202

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)_F202 \
		   $(SW_PREFIX)D201_VARIANT=F202

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/pld$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)

MAK_INCL=$(MEN_MOD_DIR)/brd.h		\
         $(MEN_MOD_DIR)/bb_d201e.h	\
         $(MEN_MOD_DIR)/d201.h		\
         $(MEN_INC_DIR)/bb_defs.h	\
         $(MEN_INC_DIR)/bb_entry.h	\
         $(MEN_INC_DIR)/bb_d201.h	\
         $(MEN_INC_DIR)/dbg.h		\
         $(MEN_INC_DIR)/desc.h		\
         $(MEN_INC_DIR)/maccess.h	\
         $(MEN_INC_DIR)/mdis_api.h	\
         $(MEN_INC_DIR)/mdis_com.h	\
         $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/modcom.h	\
         $(MEN_INC_DIR)/oss.h		\
         $(MEN_INC_DIR)/pci9050.h	\
         $(MEN_INC_DIR)/pld_load.h

MAK_INP1=bb_d201$(INP_SUFFIX)
MAK_INP2=bb_d201e$(INP_SUFFIX)
MAK_INP3=f201_pld$(INP_SUFFIX)
MAK_INP4=ident$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) $(MAK_INP3) $(MAK_INP4)



#***************************  M a k e f i l e  *******************************
#  
#         Author: ds
#          $Date: 2004/03/10 17:31:28 $
#      $Revision: 1.7 $
#  
#    Description: makefile descriptor for D201 Base Board Handler
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.7  2004/03/10 17:31:28  dpfeuffer
#   id lib/include added
#
#   Revision 1.6  2000/03/09 12:19:04  kp
#   changed D201 switch to _D201
#
#   Revision 1.5  1999/11/03 10:14:35  Gromann
#   bugfix dependencies
#
#   Revision 1.4  1999/09/08 09:30:18  Schmidt
#   updated
#
#   Revision 1.3  1998/09/21 11:32:33  Schmidt
#   ident.c added
#
#   Revision 1.2  1998/07/29 16:28:54  Schmidt
#   $Header removed, dbg.lib and dbg.h added
#
#   Revision 1.1  1998/02/23 17:38:47  Schmidt
#   Added by mcvs
#   
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=d201

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)_D201

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
MAK_INP3=d201_pld$(INP_SUFFIX)
MAK_INP4=ident$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) $(MAK_INP3) $(MAK_INP4)



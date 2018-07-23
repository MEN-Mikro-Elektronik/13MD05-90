#***************************  M a k e f i l e  *******************************
#  
#         Author: ts
#          $Date: 2006/11/14 18:38:41 $
#      $Revision: 1.3 $
#  
#    Description: Makefile definitions for the SMB2 pseudo BBIS driver
#                      
#---------------------------------[ History ]---------------------------------
#   $Log: driver.mak,v $
#   Revision 1.3  2006/11/14 18:38:41  ts
#   remove include of bb_smb2.h (outdated)
#
#   Revision 1.2  2006/06/20 18:26:10  ts
#   adapted for SMB2 pseudo BBIS driver
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


MAK_NAME=smb2

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
	 	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED	 	 

MAK_INCL=$(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_smb2$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

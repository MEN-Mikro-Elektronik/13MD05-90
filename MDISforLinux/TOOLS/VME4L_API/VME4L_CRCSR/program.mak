#***************************  M a k e f i l e  *******************************
#
#         Author: Adam Wujek
#          $Date: 2018/02/08 15:02:27 $
#      $Revision: 1.0 $
#
#    Description: Makefile definitions for VME4L_CRCSR
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.0  2018/02/08 15:02:27 Adam Wujek
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2018 by CERN
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

MAK_NAME=vme4l_crcsr

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/vme4l_api$(LIB_SUFFIX)\
		 $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)\
		 $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX) -lrt

MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h	\
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h \
		 $(MEN_INC_DIR)/usr_oss.h \
		 $(MEN_INC_DIR)/usr_utl.h \

MAK_INP1=vme4l_crcsr$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

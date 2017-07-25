#***************************  M a k e f i l e  *******************************
#
#         Author: rt
#          $Date: 2009/06/03 19:43:09 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for VME4L_TSI148_LOCMON
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  2009/06/03 19:43:09  rt
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

MAK_NAME=vme4l_tsi148_locmon

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/vme4l_api$(LIB_SUFFIX)


MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h	\
	     $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h	\
	     $(MEN_INC_DIR)/../../COM/MEN/tsi148.h

MAK_INP1=vme4l_tsi148_locmon$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

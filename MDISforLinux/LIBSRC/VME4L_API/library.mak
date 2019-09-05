#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#                      
#    Description: Makefile descriptor file for VME4L_API lib
#                      
#-----------------------------------------------------------------------------
#   Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
#*****************************************************************************
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General License
# along with this program. If not, see <http://www.gnu.org/licenses/>.


MAK_NAME=vme4l_api
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-191-g6b8334b-dirty_2019-06-10"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)
MAK_SWITCH=$(SW_PREFIX)$(DEF_REVISION)

MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_old.h\
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h


MAK_INP1 = vme4l_api$(INP_SUFFIX)

MAK_INP  = $(MAK_INP1)


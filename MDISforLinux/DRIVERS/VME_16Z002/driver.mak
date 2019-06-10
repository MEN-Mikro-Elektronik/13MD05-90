#**************************  M a k e f i l e ********************************
#  
#         Author: kp
#  
#    Description: makefile descriptor for VME4L core
#                      
#-----------------------------------------------------------------------------
#   Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
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


MAK_NAME=vme4l-core
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-190-g0fac6d8_2019-06-10"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)
MAK_SWITCH=$(SW_PREFIX)$(DEF_REVISION)

MAK_LIBS=

MAK_SWITCH = -DEXPORT_SYMTAB

MAK_INCL=$(MEN_MOD_DIR)/vme4l-core.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_old.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/men_vme_kernelif.h

MAK_INP1=vme4l-core$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

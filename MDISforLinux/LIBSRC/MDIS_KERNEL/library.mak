#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#
#    Description: makefile for Linux MDIS kernel
#
#-----------------------------------------------------------------------------
#   Copyright 2000-2019, MEN Mikro Elektronik GmbH
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


MAK_NAME=mdis_kernel
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-226-ge75b54b-dirty_2019-06-27"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
	$(SW_PREFIX)$(DEF_REVISION)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/mk_intern.h  \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/mdis_mk.h

MAK_INP1=mk_module$(INP_SUFFIX)
MAK_INP2=open$(INP_SUFFIX)
MAK_INP3=close$(INP_SUFFIX)
MAK_INP4=mk_calls$(INP_SUFFIX)
MAK_INP5=ident$(INP_SUFFIX)
MAK_INP6=mk_nonmdis$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6)

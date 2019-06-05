#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#
#    Description: makefile for Linux BBIS kernel
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
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


MAK_NAME=bbis_kernel

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/bk_intern.h  \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/bbis_bk.h

MAK_INP1=bk_module$(INP_SUFFIX)
MAK_INP2=
MAK_INP3=
MAK_INP4=
MAK_INP5=
MAK_INP6=
MAK_INP7=

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6) \
        $(MAK_INP7)

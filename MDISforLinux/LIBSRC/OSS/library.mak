#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#
#    Description: makefile for Linux OSS lib
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


MAK_NAME=oss
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-191-g6b8334b-dirty_2019-06-10"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)
MAK_SWITCH=$(SW_PREFIX)$(DEF_REVISION)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/oss_intern.h  \
	 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h

MAK_INP1=oss$(INP_SUFFIX)
MAK_INP2=oss_clib$(INP_SUFFIX)
MAK_INP3=oss_mem$(INP_SUFFIX)
MAK_INP4=oss_time$(INP_SUFFIX)
MAK_INP5=oss_resource$(INP_SUFFIX)
MAK_INP6=oss_map$(INP_SUFFIX)
MAK_INP7=oss_swap$(INP_SUFFIX)
MAK_INP8=oss_alarm$(INP_SUFFIX)
MAK_INP9=oss_dl_list$(INP_SUFFIX)
MAK_INP10=oss_bustoaddr$(INP_SUFFIX)
MAK_INP11=oss_task$(INP_SUFFIX)
MAK_INP12=oss_irq$(INP_SUFFIX)
MAK_INP13=oss_isa_pnp$(INP_SUFFIX)
MAK_INP14=oss_sem$(INP_SUFFIX)
MAK_INP15=oss_sig$(INP_SUFFIX)
MAK_INP16=oss_spinlock$(INP_SUFFIX)
MAK_INP17=oss_ident$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6) \
        $(MAK_INP7) \
        $(MAK_INP8) \
        $(MAK_INP9) \
        $(MAK_INP10) \
        $(MAK_INP11) \
        $(MAK_INP12) \
        $(MAK_INP13) \
        $(MAK_INP14) \
        $(MAK_INP15) \
        $(MAK_INP16) \
        $(MAK_INP17)



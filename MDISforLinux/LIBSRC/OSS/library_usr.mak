#***************************  M a k e f i l e  *******************************
#
#         Author: christian.schuster@men.de
#
#    Description: makefile for Linux OSS lib for user space
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2005-2019, MEN Mikro Elektronik GmbH
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

MAK_NAME=oss_usr
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-191-g6b8334b-dirty_2019-06-10"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)

MAK_SWITCH= $(SW_PREFIX)MAC_USERSPACE \
		$(SW_PREFIX)$(DEF_REVISION) \
			$(SW_PREFIX)OSS_CONFIG_PCI \
			$(SW_PREFIX)OSS_CONFIG_VME \
			$(SW_PREFIX)DBG \

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h       \
         $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h   \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/oss_intern.h  \
         $(MEN_MOD_DIR)/../pciutils/lib/pci.h  \
         $(MEN_MOD_DIR)/../pciutils/lib/config.h  \
         $(MEN_MOD_DIR)/../pciutils/lib/header.h  \
	 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h

MAK_INP1=ossu$(INP_SUFFIX)
MAK_INP2=oss_clib$(INP_SUFFIX)
MAK_INP3=ossu_mem$(INP_SUFFIX)
MAK_INP4=ossu_time$(INP_SUFFIX)
MAK_INP5=ossu_resource$(INP_SUFFIX)
MAK_INP6=ossu_map$(INP_SUFFIX)
MAK_INP7=oss_swap$(INP_SUFFIX)
MAK_INP8=oss_dl_list$(INP_SUFFIX)
MAK_INP9=ossu_bustoaddr$(INP_SUFFIX)
MAK_INP10=ossu_task$(INP_SUFFIX)
MAK_INP11=ossu_sem$(INP_SUFFIX)

MAK_INP12=ossu_ident$(INP_SUFFIX)

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

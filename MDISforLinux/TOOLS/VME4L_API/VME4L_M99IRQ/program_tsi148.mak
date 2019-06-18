#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#
#    Description: Makefile definitions for VME4L_M99IRQ_TSI148
#                 (TSI148 doesn't support hardware swapping, therefore user
#                  applications has to care for swapping)
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2009-2019, MEN Mikro Elektronik GmbH
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

MAK_NAME=vme4l_m99irq_tsi148
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-185-gbb44c53_2019-06-07"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/vme4l_api$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)$(DEF_REVISION) \
	-DMEN_TSI148

MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/vme4l_api.h	\
	     $(MEN_INC_DIR)/../../NATIVE/MEN/vme4l.h

MAK_INP1=vme4l_m99irq$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

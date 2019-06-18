#**************************  M a k e f i l e ********************************
#  
#         Author: ts
#  
#    Description: makefile descriptor for A21 MSI enabler helper
#                      
#
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


MAK_NAME=lx_a21_msi_enable
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-192-gccbbd88-dirty_2019-06-10"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)

MAK_LIBS=

MAK_SWITCH=$(SW_PREFIX)$(DEF_REVISION)

MAK_INCL=

MAK_INP1=a21_msi_enable$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

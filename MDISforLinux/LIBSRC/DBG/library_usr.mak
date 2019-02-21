#**************************  M a k e f i l e ********************************
#
#         Author: christian.schuster@men.de
#          $Date: 2005/07/08 11:40:36 $
#      $Revision: 1.1 $
#
#    Description: makefile descriptor for user space DBG library
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


MAK_NAME=dbg_usr

MAK_LIBS=

MAK_SWITCH= $(SW_PREFIX)MAC_MEM_MAPPED \
			$(SW_PREFIX)MAC_USERSPACE

MAK_INCL=

MAK_INP1=dbgu$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

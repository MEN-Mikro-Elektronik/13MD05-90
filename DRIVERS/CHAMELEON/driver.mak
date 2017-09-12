#**************************  M a k e f i l e ********************************
#  
#         Author: ub
#          $Date: 2004/11/23 09:33:54 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for chameleon Linux kernel module
#                      
#---------------------------------[ History ]---------------------------------
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2004 by MEN mikro elektronik GmbH, Nuernberg, Germany 
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


MAK_NAME=lx_chameleon

MAK_LIBS=

MAK_SWITCH =

MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/men_chameleon.h \
		 $(MEN_INC_DIR)/chameleon.h \

MAK_INP1=men_chameleon$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

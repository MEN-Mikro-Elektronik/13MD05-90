#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#      $Revision: 1.2 $
#
#    Description: makefile for Linux DBG module
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.2  2005/07/07 14:54:09  cs
#   Copyright line changed
#
#   Revision 1.1  2001/01/19 14:38:26  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
# (c) Copyright 2000 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
#*****************************************************************************
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


MAK_NAME=dbg

MAK_SWITCH=-DDBG_MODULE

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/dbg.h         \
         $(MEN_INC_DIR)/../../NATIVE/MEN/dbg_os.h

MAK_INP1=dbg$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)


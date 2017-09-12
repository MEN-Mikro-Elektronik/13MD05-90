#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 1999/04/28 16:27:03 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for DESCGEN program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  1999/04/28 16:27:03  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
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


MAK_NAME=descgen

MAK_LIBS=

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/desctyps.h	  \
		 $(MEN_MOD_DIR)/descgen.h	  \
		 $(MEN_MOD_DIR)/os9k.h		

MAK_INP1=descgen$(INP_SUFFIX)
MAK_INP2=binary$(INP_SUFFIX)
MAK_INP3=csource$(INP_SUFFIX)
MAK_INP4=os9$(INP_SUFFIX)
MAK_INP5=os9k$(INP_SUFFIX)
MAK_INP6=parse$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) $(MAK_INP3) $(MAK_INP4) $(MAK_INP5) \
	    $(MAK_INP6) 






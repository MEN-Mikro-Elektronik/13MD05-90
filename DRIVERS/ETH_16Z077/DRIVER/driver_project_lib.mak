#**************************  M a k e f i l e ********************************
#  
#         Author: aw
#          $Date: 2008/03/12 11:00:27 $
#      $Revision: 1.1 $
#  
#    Description: makefile descriptor for Z077/87 Driver, unswapped
#                      
#---------------------------------[ History ]---------------------------------
#
# $Log: driver_project_lib.mak,v $
# Revision 1.1  2008/03/12 11:00:27  aw
# Initial Revision
#
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany 
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


MAK_NAME=lx_z77

MAK_LIBS=$(PROJECT_LIBS)

MAK_INCL= $(MEN_INC_DIR)/men_typs.h   \
          $(MEN_INC_DIR)/oss.h        \
          $(MEN_INC_DIR)/dbg.h        \
          $(MEN_INC_DIR)/mdis_err.h   \
          $(MEN_INC_DIR)/men_chameleon.h \
          $(MEN_INC_DIR)/sysparam2.h \

MAK_OPTIM=$(OPT_1)

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_INP1=men_16z077_eth$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

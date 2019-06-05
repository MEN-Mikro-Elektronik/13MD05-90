#***************************  M a k e f i l e  *******************************
#  
#         Author: gl
#                      
#    Description: Makefile descriptor file for switch confgiration lib
#                  (Marvell 88E6095)
#                      
#-----------------------------------------------------------------------------
#   Copyright (c) 2008-2019, MEN Mikro Elektronik GmbH
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

MAK_NAME=uti
# the next line is updated during the MDIS installation
STAMPED_REVISION="13MD05-90_02_00-173-gaf125b9-dirty_2019-06-05"

DEF_REVISION=MAK_REVISION=$(STAMPED_REVISION)

MAK_SWITCH= \
		$(SW_PREFIX)$(DEF_REVISION)


MAK_INCL= 

MAK_INP01 = ask$(INP_SUFFIX)
MAK_INP02 = cntarg$(INP_SUFFIX)
MAK_INP03 = cntopt$(INP_SUFFIX)
MAK_INP04 = dirname$(INP_SUFFIX)
MAK_INP05 = dl_list$(INP_SUFFIX)
MAK_INP06 = dump$(INP_SUFFIX)
MAK_INP07 = fdump$(INP_SUFFIX)
MAK_INP08 = filename$(INP_SUFFIX)
MAK_INP09 = getint$(INP_SUFFIX)
MAK_INP10 = getshorterrmsg$(INP_SUFFIX)
MAK_INP11 = getstr$(INP_SUFFIX)
MAK_INP12 = line_args$(INP_SUFFIX)
MAK_INP13 = look_string$(INP_SUFFIX)
MAK_INP14 = macro_ex$(INP_SUFFIX)
MAK_INP15 = make_path$(INP_SUFFIX)
MAK_INP16 = panic$(INP_SUFFIX)
MAK_INP17 = printtime$(INP_SUFFIX)
MAK_INP18 = smatch$(INP_SUFFIX)
MAK_INP19 = str_to_upper$(INP_SUFFIX)
MAK_INP20 = str_to_lower$(INP_SUFFIX)
MAK_INP21 = strsave$(INP_SUFFIX)
MAK_INP22 = tstopt$(INP_SUFFIX)

MAK_INP = $(MAK_INP01) $(MAK_INP02) $(MAK_INP03) $(MAK_INP04) \
		  $(MAK_INP05) $(MAK_INP06) $(MAK_INP07) $(MAK_INP08) \
		  $(MAK_INP09) $(MAK_INP10) $(MAK_INP11) $(MAK_INP12) \
		  $(MAK_INP13) $(MAK_INP14) $(MAK_INP15) $(MAK_INP16) \
		  $(MAK_INP17) $(MAK_INP18) $(MAK_INP19) $(MAK_INP20) \
		  $(MAK_INP21) $(MAK_INP22)




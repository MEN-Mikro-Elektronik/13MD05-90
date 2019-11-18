#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#  
#    Description: Makefile to produce development tools
#
#	Make everything:
#		make all_dev_tools				Makes development tools
#		make installdevtools			installs the tools
#---------------------------------------------------------------------------
#   Copyright 2004-2019, MEN Mikro Elektronik GmbH
############################################################################
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


#-----------------------------------------------------------
# Tool chain definitions
export HOST_CC			:= gcc
export HOST_LD			:= ld

#-----------------------------------------------------------
# define the directory in which the executable binaries are to be installed
# Used for "make installbin"
# Usually this is the development platform's /usr/bin or 
# /usr/local/bin directory
#
BIN_INSTALL_DIR 	:= /usr/local/bin

#-----------------------------------------------------------
# Define the include root directory for the development platform's linux 
#
LIN_USR_INC_DIR		:= /usr/include

#-----------------------------------------------------------
# Define wether to build/use static or shared user state libraries
#
# static 	make static libs / link programs with static libs
# shared	make shared libs / link programs for use with shared libs
#
LIB_MODE			:= shared

#-----------------------------------------------------------
# Define common tools to build
# Specify the path to their common makefiles here 
# (path relative to TOOLS)
#
ALL_DEV_TOOLS	:= \
					DESCGEN/COM/program.mak \


.PHONY: makethedevtools

makethedevtools:
	$(MAKE) all_dev_tools

# include rules to build the components
include ../TPL/rules.mak


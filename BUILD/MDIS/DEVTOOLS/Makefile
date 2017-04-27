#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2004/06/09 12:06:26 $
#      $Revision: 1.4 $
#  
#    Description: Makefile to produce development tools
#
#	Make everything:
#		make all_dev_tools				Makes development tools
#		make installdevtools			installs the tools
#-----------------------------------------------------------------------------
#   (c) Copyright 2001 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

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
BIN_INSTALL_DIR 	?= /usr/local/bin

#-----------------------------------------------------------
# Define the include root directory for the development platform's linux 
#
LIN_USR_INC_DIR		?= /usr/include

#-----------------------------------------------------------
# Define wether to build/use static or shared user state libraries
#
# static 	make static libs / link programs with static libs
# shared	make shared libs / link programs for use with shared libs
#
LIB_MODE			?= shared

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


#***************************  M a k e f i l e  *******************************
#
#        \file  rules.mak
#
#        $Date: 2019/04/24 $
#    $Revision: 1.0 $
#
#        \brief This rules.mak file should be used only by fpga_load tool
#               File is based on MDIS rules.mak file
#
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2019, MEN Mikro Elektronik GmbH
#***************************************************************************/
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


# determine current directory and configuration name
cwd := $(shell pwd)

ifeq ($(V),1)
 Q =
else
 Q = @
endif
export Q

export THIS_DIR		:= $(cwd)

ifndef MEN_LIN_DIR
	MEN_LIN_DIR = /opt/menlinux
endif

export MEN_LIN_DIR
export LS_PATH		:= $(MEN_LIN_DIR)/LIBSRC
export TO_PATH		:= $(MEN_LIN_DIR)/TOOLS

export TPL_DIR	   	:= $(MEN_LIN_DIR)/BUILD/MDIS/TPL/

# older MDIS packages used LIN_INC_DIR. Newer ones use LIN_KERNEL_DIR
# make this compatible with both versions
#
# LIN_INC_DIR remains undefined when both LIN_INC_DIR and LIN_KERNEL_DIR
# wasn't defined
ifdef LIN_INC_DIR
# old Makefiles
	LIN_KERNEL_DIR := $(LIN_INC_DIR)/..
else
 ifdef LIN_KERNEL_DIR
	LIN_INC_DIR 	:= $(LIN_KERNEL_DIR)/include
 endif
endif

LIN_KERNEL_COMMON_DIR ?= $(LIN_KERNEL_DIR)


_ALL_USR_LIBS 	=  $(ALL_USR_LIBS) \
					MDIS_API/library.mak
#ifndef NO_STD_ALL_COM_TOOLS
_ALL_COM_TOOLS	=  $(ALL_COM_TOOLS) #\


# some tools
ifndef MAKE
 export MAKE			:= make
endif

ifndef ECHO
 export ECHO			:= echo
endif

ifndef DEBUG
 DEBUG := nodbg
endif

# set default MAK_SWITCH
MAK_SWITCH=-DMAC_MEM_MAPPED

# Get the linux version
ifdef LIN_INC_DIR

# ts@men: the gcc 5.2 in Ubuntu 15.10 showed -E dump different, breaking the previous awk statement.
#         So, made it robust by extracting the RELEASE element of the dump 

export LINUX_PLAIN_VERSION = $(shell \
	$(CC) -I$(LIN_INC_DIR) -E $(TPL_DIR)/getlinuxversion.c | grep "RELEASE" | awk '{print $$2}' | sed 's/"//g' )

# $(warning the LINUX_PLAIN_VERSION of kernel headers in $(LIN_INC_DIR) is $(LINUX_PLAIN_VERSION))

# this returns the version string, depending on selfhosted or ElinOS
ifeq ($(WIZ_CDK),Selfhosted)
export LINUX_VERSION = $(LINUX_PLAIN_VERSION)
else
export LINUX_VERSION = $(shell \
	$(CC) -I$(LIN_INC_DIR) -E $(TPL_DIR)/getlinuxversion.c | \
    awk -F\" '/REL/ {print $$2}' )
endif

else
# no LIN_INC_DIR defined
export LINUX_PLAIN_VERSION = "noversion"
export LINUX_VERSION = "noversion"
endif

export LIB_INSTALL_DIR
export STATIC_LIB_INSTALL_DIR
export LIN_KERNEL_DIR
export LIN_KERNEL_COMMON_DIR
export LIN_INC_DIR
export DEBUG

export CONFIG_NAME 	:= $(notdir $(THIS_DIR))
export USRLIB_MAK  	:= $(TPL_DIR)usrlib_$(LIB_MODE).mak
export USRPROG_MAK 	:= $(TPL_DIR)usrprog_$(LIB_MODE).mak

MOD_OUTPUT_DIR 	   := MODULES
OBJ_DIR			   := OBJ

MAKEIT 				= $(Q)$(MAKE) --no-print-directory

ifeq "$(origin DEBUG)" "command line"
	ALL_DBGS := $(DEBUG)
endif


#----------------------------------------
# Rules
#
.PHONY: all_usr_libs all_com_tools $(_ALL_USR_LIBS) $(_ALL_COM_TOOLS)

buildfpgaload: all_usr_libs all_com_tools

#kernelsettings

$(_ALL_USR_LIBS):
	$(MAKEIT) -f $(USRLIB_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		COMP_PREFIX=

#
$(_ALL_COM_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(TO_PATH)/$@ \
		COMP_PREFIX=

-include $(THIS_DIR)/.kernelsettings

all_usr_libs: kernelsettings $(_ALL_USR_LIBS)
all_com_tools:	kernelsettings $(_ALL_COM_TOOLS)

#
# Rule to get settings from Linux Kernel
# currently used only for USER PROGRAMS
.PHONY: kernelsettings endian

kernelsettings: $(THIS_DIR)/.kernelsettings


$(THIS_DIR)/.kernelsettings: $(LIN_KERNEL_COMMON_DIR)/Makefile
	@$(ECHO) "Getting Compiler/Linker settings from Linux Kernel Makefile"
	@$(Q)$(MAKE) -C $(LIN_KERNEL_COMMON_DIR) \
	--no-print-directory -f $(TPL_DIR)kernelsettings.mak \
	KERNEL_SETTINGS_FILE=$(THIS_DIR)/.kernelsettings > /dev/null 2>&1

$(THIS_DIR)/.endian: Makefile $(LIN_KERNEL_COMMON_DIR)/Makefile
	@$(ECHO) "Getting Compiler/Linker settings from Linux Kernel Makefile"


$(ALL_DBGS): 
	$(MAKEIT) RULE=$(RULE) DEBUG=$@ buildfordbg

clean: 
	@$(ECHO) "Removing all objects, modules, binaries, libraries, descriptors"
	rm -rf .kernelsettings .kernelsubdirs OBJ BIN 


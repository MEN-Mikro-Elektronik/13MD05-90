#***************************  M a k e f i l e  *******************************
#
#        \file  rules.mak
#
#      \author  kp/ts
#        $Date: 2012/11/02 14:41:39 $
#    $Revision: 2.27 $
#
#        \brief Defines top level rules for MDIS components
#                This file should be included at the bottom of the
#				 configurations makefile
#
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2006-2019, MEN Mikro Elektronik GmbH
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
export NAT_PATH		:= $(MEN_LIN_DIR)/DRIVERS
export LL_PATH		:= $(MEN_LIN_DIR)/DRIVERS/MDIS_LL
export BB_PATH		:= $(MEN_LIN_DIR)/DRIVERS/BBIS
export LS_PATH		:= $(MEN_LIN_DIR)/LIBSRC
export TO_PATH		:= $(MEN_LIN_DIR)/TOOLS

export PRG_OUTPUT_DIR := BIN
export LIB_OUTPUT_DIR := LIB

export TPL_DIR	   	:= $(MEN_LIN_DIR)/BUILD/MDIS/TPL/

export DEVNODE_NR	:=

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

# on Debian a workaround for kernel headers is needed
LIN_DISTRO_ID := $(findstring Debian,"$(shell uname --kernel-version)")
ifeq ($(LIN_DISTRO_ID),Debian)
	LIN_KERNEL_COMMON_DIR := $(LIN_KERNEL_DIR)/source
endif
LIN_KERNEL_COMMON_DIR ?= $(LIN_KERNEL_DIR)

#
# decide wether to force build of 32bit apps or leave it to build host
#
MDIS_FORCE_32BIT=$(FORCE_32BIT_BUILD)

GIT_VERSION=$(shell sed -e 's/ /_/g' -e 's/fatal:_//g' $(MEN_LIN_DIR)/HISTORY/13MD05-90_tag.txt)
# for CERN variant only!
# MDIS_EXTRA_DEFS += -DCONFIG_MEN_VME_KERNELIF -DA25_WRAPPER
DGIT_VERSION=\"$(GIT_VERSION)\"
MDIS_EXTRA_DEFS += -DGIT_VERSION=\"$(GIT_VERSION)\"

#-----------------------------------------------------------
# MDIS COMMON COMPONENTS
#
ALL_KERNEL		:= \
					MDIS_KERNEL/library.mak \
					BBIS_KERNEL/library.mak \

# the obligatory libraries
_ALL_CORE2 		:= \
					DESC/COM/library.mak \
					PLD/COM/library.mak \
					PLD/COM/library_sw.mak \
					ID/COM/library.mak \
					ID/COM/library_sw.mak \
					MBUF/COM/library.mak \
					SMB/PORT/COM/library.mak \
					SMB/PORT/COM/library_io.mak

SC24_I2C_DRIVER		:=		DRIVERS/I2C_BUSSES/i2cpiix4/driver.mak


# includable / excludable libraries 
# native: add to _ALL_CORE1 
# common: add to _ALL_CORE2			
ifndef ALL_CORE_LIBS
  _ALL_CORE1   += \
     OSS/library.mak \
     DBG/library.mak
  _ALL_CORE2	+= \
     CHAMELEON/COM/library.mak
else 
	ifneq (,$(findstring CHAMELEON,$(ALL_CORE_LIBS)))
		_ALL_CORE2		+= CHAMELEON/COM/library.mak \
						   CHAMELEON/COM/library_io.mak
	endif
	ifneq (,$(findstring OSS,$(ALL_CORE_LIBS)))
		_ALL_CORE1		+= OSS/library.mak
	endif
	ifneq (,$(findstring DBG,$(ALL_CORE_LIBS)))
		_ALL_CORE1		+= DBG/library.mak
	endif
endif

# common libs (only valid for x86 architectures)
_ALL_CORE_COM_X86	:= 

ALL_CORE		+= $(_ALL_CORE1)
ALL_CORE		+= $(_ALL_CORE2)

_ALL_USR_LIBS 	=  $(ALL_USR_LIBS) \
					MDIS_API/library.mak

ifndef NO_STD_ALL_COM_TOOLS
_ALL_COM_TOOLS	=  $(ALL_COM_TOOLS) \
					MDIS_API/MDIS_CREATEDEV/program.mak \

else
_ALL_COM_TOOLS  = $(ALL_COM_TOOLS)
endif

# some tools
ifndef MAKE
 export MAKE			:= make
endif

ifndef ECHO
 export ECHO			:= echo
endif

ifndef MKNOD
 export MKNOD			:= mknod
endif

ifndef _DESCGEN
 export _DESCGEN		:= $(MEN_LIN_DIR)/BUILD/MDIS/DEVTOOLS/BIN/descgen
endif

ifndef _DESCGEN_OBJ
 export _DESCGEN_OBJ	:= $(MEN_LIN_DIR)/BUILD/MDIS/DEVTOOLS/OBJ/descgen
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


# check if CONFIG_DEVFS_FS is defined
ifdef LIN_INC_DIR
NEW_INCLUDE_STRUCTURE := $(shell expr "$(LINUX_PLAIN_VERSION)" \>= "2.6.33" )
#$(warning NEW_INCLUDE_STRUCTURE = $(NEW_INCLUDE_STRUCTURE))
ifeq ($(NEW_INCLUDE_STRUCTURE),1)
	GEN_INCLUDE_DIR := "generated"
else
	GEN_INCLUDE_DIR := "linux"
endif

export HASDEVFS = $(shell grep 'define CONFIG_DEVFS_FS' $(LIN_INC_DIR)/$(GEN_INCLUDE_DIR)/autoconf.h | wc -l | sed 's/ //g')
endif


export MDIS_FORCE_32BIT
export MODS_INSTALL_DIR
export BIN_INSTALL_DIR
export LIB_INSTALL_DIR
export STATIC_LIB_INSTALL_DIR
export DESC_INSTALL_DIR
export LIN_KERNEL_DIR
export LIN_KERNEL_COMMON_DIR
export LIN_INC_DIR
export LIN_USR_INC_DIR
export DEBUG_FLAGS
export DEBUG
export KERNEL_CC
export KERNEL_LD
export KERNEL_CFLAGS
export KERNEL_LDFLAGS
export ALL_DESC
export MDIS_EXTRA_DEFS

export CONFIG_NAME 	:= $(notdir $(THIS_DIR))
export USRLIB_MAK  	:= $(TPL_DIR)usrlib_$(LIB_MODE).mak
export USRPROG_MAK 	:= $(TPL_DIR)usrprog_$(LIB_MODE).mak
export DESC_MAK    	:= $(TPL_DIR)desc.mak

MOD_OUTPUT_DIR 	   := MODULES
OBJ_DIR			   := OBJ

MAKEIT 				= $(Q)$(MAKE) --no-print-directory

ifeq "$(origin DEBUG)" "command line"
	ALL_DBGS := $(DEBUG)
endif


#----------------------------------------
# Rules
#
.PHONY: buildall buildusr builddesc builddbgs \
		all_usr_libs all_ll_tools all_com_tools all_nat_tools all_dev_tools all_desc \
		install installbin installdesc installdevnode \
		installlibs installstaticlibs \
		$(_DESCGEN) \
		$(_ALL_USR_LIBS) $(_ALL_COM_TOOLS) $(ALL_DEV_TOOLS) $(ALL_NATIVE_TOOLS)


# rule to make everything
buildall: buildmods buildusr builddesc

# rule to build all user state libs and programs
buildusr: all_usr_libs all_ll_tools all_com_tools all_nat_tools

# rule to build all descriptors
builddesc: all_desc

# rule to build descriptor generator
$(_DESCGEN):
	$(MAKEIT) -C $(MEN_LIN_DIR)/BUILD/MDIS/DEVTOOLS

# installation rules

install: installmods installlibs installstaticlibs installbin \
		 installdesc installdevnode

installlibs:
	$(MAKEIT) all_usr_libs RULE=installlibs
ifeq ($(WIZ_CDK),Selfhosted)
	@$(ECHO) "Updating library cache"
	/sbin/ldconfig
else
	@$(ECHO) "== You should now rebuild library cache on target using ldconfig"
endif

installstaticlibs:
	$(MAKEIT) all_usr_libs RULE=installstaticlibs

installbin:
	$(MAKEIT) all_ll_tools all_com_tools all_nat_tools RULE=installbin

installdesc:
	$(MAKEIT) -f $(DESC_MAK) installdesc

installdevnode:

ifeq ($(HASDEVFS),1)
else
 ifdef DEVNODE_INSTALL_DIR
	$(Q)install --directory $(DEVNODE_INSTALL_DIR)
	@$(ECHO) "============================================================================"
	@$(ECHO) "=> When rebuilding modified modules rmmod old modules before new modprobe <="
	@$(ECHO) "============================================================================"
 endif
endif

installdevtools:
	$(MAKEIT) all_dev_tools RULE=installbin


all_usr_libs: 	kernelsettings $(_ALL_USR_LIBS)
all_ll_tools:	kernelsettings $(ALL_LL_TOOLS)
all_com_tools:	kernelsettings $(_ALL_COM_TOOLS)
all_nat_tools:	kernelsettings $(ALL_NATIVE_TOOLS)
all_dev_tools:	kernelsettings $(ALL_DEV_TOOLS)
all_desc:		kernelsettings $(_DESCGEN) $(ALL_DESC)

$(_ALL_USR_LIBS):
	$(MAKEIT) -f $(USRLIB_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		COMP_PREFIX=

$(ALL_LL_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(LL_PATH)/$@ \
		COMP_PREFIX= \

$(_ALL_COM_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(TO_PATH)/$@ \
		COMP_PREFIX=

$(ALL_NATIVE_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(MEN_LIN_DIR)/$@ \
		COMP_PREFIX= \

$(ALL_DEV_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(TO_PATH)/$@ \
		COMP_PREFIX=

$(ALL_DESC):
	$(MAKEIT) -f $(DESC_MAK) $(RULE) DESC=$@

-include $(THIS_DIR)/.kernelsettings

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


printit:
	@echo LINUX_PLAIN_VERSION is !$(LINUX_PLAIN_VERSION)!
	@echo LINUX_VERSION is !$(LINUX_VERSION)!

#
# For gcov code coverage set LIB_MODE = static in MAKEFILE
# otherwise it will be ignored
#
#export CODE_COVERAGE := -fprofile-arcs -ftest-coverage
#export CODE_COVERAGE := -pg


export COMP_MAK    := $(TPL_DIR)component.mak

#----------------------------------------
# Rules
#
.PHONY: prepare_kernelsettings buildmods builddbgs \
		all_kernel all_ll all_bb all_core all_raw  \
		installmods 
		$(ALL_DBGS) $(ALL_LL_DRIVERS) \
		$(ALL_BB_DRIVERS) $(ALL_CORE) $(ALL_KERNEL)
		$(ALL_RAW) $(ALL_NATIVE_DRIVERS) 

# rule to build debug and/or non-debug version of all modules
prepare_kernelsettings:
	touch $(THIS_DIR)/.kernelsettings

sc24i2cmod: $(ALL_NATIVE_DRIVERS)

buildmods: $(ALL_DBGS)

$(ALL_DBGS): 
	$(MAKEIT) RULE=$(RULE) DEBUG=$@ buildfordbg

buildfordbg: kernelsettings kernelsubdirs all_kernel all_core  \
			 all_ll all_bb all_raw callkernelbuild

clean: 
	@$(ECHO) "Removing all objects, modules, binaries, libraries, descriptors"
	rm -rf .kernelsettings .kernelsubdirs OBJ BIN LIB DESC
	rm -f $(_DESCGEN_OBJ)/*

# install non-debug modules when ALL_DBGS is set to "nodbg" (or via command
# line paramter "DEBUG="
# install debug modules in any other case.
installmods: 
ifeq ($(ALL_DBGS),nodbg)
	$(MAKEIT) all_kernel all_core all_raw all_ll all_bb \
	 RULE=installmods DEBUG=nodbg
else
	$(MAKEIT) all_kernel all_core all_raw all_ll all_bb \
	RULE=installmods DEBUG=dbg
endif
ifeq ($(WIZ_CDK),Selfhosted)
	@$(ECHO) "Updating module dependencies"
	/sbin/depmod
else 
	@$(ECHO) "=== You should now rebuild module dependencies on target using depmod"
endif

all_ll:		$(ALL_LL_DRIVERS) 
all_bb:		$(ALL_BB_DRIVERS)
all_core: 	$(ALL_CORE)
all_kernel:	$(ALL_KERNEL)
i2c_sc24:	$(ALL_RAW) $(SC24_I2C_DRIVER)
all_raw: 	$(ALL_RAW) $(ALL_NATIVE_DRIVERS)
all_native: 	$(ALL_RAW) $(ALL_NATIVE_DRIVERS)

$(ALL_LL_DRIVERS):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(LL_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ll_ \
		LLDRV=-D_LL_DRV_ MODULE_COM=ll_module.o

$(ALL_BB_DRIVERS):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(BB_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_bb_ \
		LLDRV=-D_LL_DRV_ MODULE_COM=bb_module.o

$(_ALL_CORE1):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ MODULE_COM=module.o

$(_ALL_CORE2):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ MODULE_COM=nat_module.o

$(_ALL_CORE_COM_X86):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ MODULE_COM=nat_module.o

#$(SC24_I2C_DRIVER):
#	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
#		COMMAKE=$(LS_PATH)/$@ \
#		DEBUG=$(DEBUG) COMP_PREFIX=men_ MODULE_COM= SYMS=y

$(ALL_KERNEL):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ MODULE_COM= SYMS=y

$(ALL_RAW) $(ALL_NATIVE_DRIVERS):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(MEN_LIN_DIR)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ LLDRV= MODULE_COM=

#
# .kernelsubdirs contains a list of subdirs to be passed to linux
# kernel build 
# Each invokation of MDIS/component.mak adds one line to this file
#
.PHONY: kernelsubdirs $(THIS_DIR)/.kernelsubdirs

kernelsubdirs: $(THIS_DIR)/.kernelsubdirs

$(THIS_DIR)/.kernelsubdirs:
	@$(ECHO) "Cleaning .kernelsubdirs"
	@$(ECHO) -n >$@


# The actual call to the kernel build 
# This will work only with Linux >=2.6.6

.PHONY: callkernelbuild

callkernelbuild:
	@$(ECHO) "++++++++ Building kernel modules ++++++++++"
	@$(ECHO) -n "obj-m +=" >OBJ/Makefile
	cat $(THIS_DIR)/.kernelsubdirs >>OBJ/Makefile
	$(Q)$(MAKE) -C $(LIN_KERNEL_DIR) SUBDIRS=$(THIS_DIR)/OBJ

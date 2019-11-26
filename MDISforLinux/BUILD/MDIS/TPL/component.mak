#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#  
#    Description: Make a Linux MDIS component (Kernel 2.6)
#                      
#-----------------------------------------------------------------------------
#   Copyright 2004-2019, MEN Mikro Elektronik GmbH
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

#------------------------
# include common makefile

# the following three macros are don't care for Linux modules
LIB_PREFIX      := xxx
LIB_SUFFIX      := xxx
MEN_LIB_DIR     := xxx
SW_PREFIX       := -D
INC_DIR         := $(MEN_LIN_DIR)/INCLUDE/COM
MEN_INC_DIR     := $(INC_DIR)/MEN
INP_SUFFIX      := .c

# obsolete...
OPT_1           := opt1
OPT_2           := opt2
OPT_3           := opt3

MEN_MOD_DIR		:= $(dir $(COMMAKE))

include $(THIS_DIR)/.kernelsettings
CC				= $(KERNEL_CC)
include $(TPL_DIR)/endian.mak

ifdef COMMAKE
	include $(COMMAKE)
endif

#------------------------
# determine output paths
ifeq ($(DEBUG),dbg)
    TESTSUFFIX := dbg
	DBG		   := -DDBG
	DBGSTR	   := debug
endif
ifeq ($(DEBUG),nodbg)
    TESTSUFFIX := nodbg
	DBG		   :=
	DBGSTR	   := non-debug
endif 

ifndef TESTSUFFIX
	FAILURE! DEBUG must be dbg or nondbg!
endif

COMP_NAME := $(COMP_PREFIX)$(MAK_NAME)
COMP_OBJ  := $(COMP_NAME).o
COMP_KO	  := $(COMP_NAME).ko

TMP_OBJ_DIR	  	:= OBJ/$(TESTSUFFIX)/$(COMP_NAME)

# all sources specified in COM_MAKE
COMP_SRC	 	:= $(MAK_INP)

# sources with path
ABS_SRC			:= $(addprefix $(MEN_MOD_DIR),$(COMP_SRC))

# sources as .o files
TMP_COMP_OBJS 	:= $(MAK_INP:.c=.o) $(MODULE_COM)

ifeq ($(MODULE_COM),ll_module.o)
 ABS_MODULE_COM = $(LL_PATH)/NATIVE/ll_module.c
endif
ifeq ($(MODULE_COM),bb_module.o)
 ABS_MODULE_COM = $(BB_PATH)/NATIVE/bb_module.c
endif
ifeq ($(MODULE_COM),module.o)
 ABS_MODULE_COM = $(LS_PATH)/MDIS_COMPONENT_COMMON/module.c
endif
ifeq ($(MODULE_COM),nat_module.o)
 ABS_MODULE_COM = $(MEN_MOD_DIR)../NATIVE/nat_module.c
endif

#**************************************
#   include pathes
#
NAT_INC_DIR     :=  $(MEN_LIN_DIR)/INCLUDE/NATIVE

ifdef INITIAL_INC_DIR
INC_DIRS = -I$(INITIAL_INC_DIR)
else
INC_DIRS = 
endif

INC_DIRS+=-I$(INC_DIR)              \
    -I$(NAT_INC_DIR)                \
    -I$(MEN_MOD_DIR)

ifdef RTAI_INC_DIR
INC_DIRS += -I$(RTAI_INC_DIR)
endif


#**************************************
#   Compiler flags
#

DEF             :=-DLINUX -DMODULE -DCOMP_NAME=\\\"$(COMP_NAME)\\\" \
				  -D_ONE_NAMESPACE_PER_DRIVER_ $(MDIS_EXTRA_DEFS)


ifndef MAK_SWITCH
    MAK_SWITCH := -DMAC_MEM_MAPPED
endif

ifndef WARN_LEVEL
# enable all warnings, but disable warning about wrong type passing to kprintf
# and disable warning about breaking strict aliasing rules when optimization is
# above level 1 (compilers prior to gcc 7)
    WARN_LEVEL := -Wno-format -Wno-strict-aliasing
endif

# check if we have a 64bit system
LONG_WIDTH = $(shell $(CC) $(TPL_DIR)/checkTypeSize.c 2>&1 \
	          | awk '/\#error/ {print $$NF}')

ifeq ($(LONG_WIDTH),_64)	
  DEF += -D_LIN64
endif

CFLAGS          :=  $(INC_DIRS) $(DEF) $(DBG) $(MAK_SWITCH) \
					$(WARN_LEVEL) $(LLDRV) $(ENDIAN)


#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all installmods buildmsg build mklinks mkmakefile

all: buildmsg $(TMP_OBJ_DIR) mkmakefile mklinks build

installmods:
	@$(ECHO) installing $(COMP_NAME) $(DBGSTR) version in \
	 $(MODS_INSTALL_DIR)
	$(Q)install --directory --mode=775 $(MODS_INSTALL_DIR)
	$(Q)install --mode=644  \
	 $(TMP_OBJ_DIR)/$(COMP_KO) $(MODS_INSTALL_DIR)

buildmsg:
	@$(ECHO) 
	@$(ECHO) ++++++++ Preparing $(DBGSTR) version of module $(COMP_NAME) \
		+++++++++++


$(TMP_OBJ_DIR):
	- $(Q) mkdir -p $(TMP_OBJ_DIR)
	@$(ECHO) Directory $(TMP_OBJ_DIR) created

#
# Make symbolic links to sources
#
mklinks:
	$(Q)rm -f $(TMP_OBJ_DIR)/*.[ch]
	$(Q)ln -s $(ABS_SRC) $(TMP_OBJ_DIR)
ifdef ABS_MODULE_COM
	$(Q)ln -s $(ABS_MODULE_COM) $(TMP_OBJ_DIR)
endif

#
# Create the makefile that is included in the kernel build system
#
define gen_makefile
	$(Q)( set -e; \
	  echo "obj-m := $(COMP_OBJ)"; \
	  echo "EXTRA_CFLAGS := $(CFLAGS)"; \
	  echo "$(COMP_NAME)-objs := $(TMP_COMP_OBJS)"; \
	)
endef

mkmakefile:
	$(gen_makefile) >$(TMP_OBJ_DIR)/Makefile

#
#  
# 
build:
	@$(ECHO) -n "$(TESTSUFFIX)/$(COMP_NAME)/ " >>$(THIS_DIR)/.kernelsubdirs
#	@$(ECHO) -n "$(THIS_DIR)/$(TMP_OBJ_DIR) " >>$(THIS_DIR)/.kernelsubdirs

#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2010/02/25 16:10:46 $
#      $Revision: 1.10 $
#  
#    Description: Make a Linux MDIS user state program 
#                 Compile to use static libraries     
#       Switches: 
#				  COMMAKE		common makefile name (with path)
#				  CPU			cpu code to compile ???
#				  WARN_LEVEL 	(default -Wall)
#				  THIS_DIR		path of config directory
#				  MEN_LIN_DIR	absolute path to directory that contains the
#								MDIS4LINUX tree
#				  LIN_USR_INC_DIR	where to find linux includes (/usr/include)
#		Rules:	  all			main rule for libraries
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
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


# the following three macros are don't care for Linux
LIB_PREFIX      := 
LIB_SUFFIX      := .a
MEN_LIB_DIR     := $(THIS_DIR)/LIB
SW_PREFIX       := -D
INC_DIR         := $(MEN_LIN_DIR)/INCLUDE/COM
MEN_INC_DIR     := $(INC_DIR)/MEN
INP_SUFFIX      := .o

# obsolete...
OPT_1           := opt1
OPT_2           := opt1
OPT_3           := opt1

MEN_MOD_DIR		:= $(dir $(COMMAKE))


#------------------------
# include common makefile
#
ifdef COMMAKE
	include $(COMMAKE)
endif

# unfortunately the common makefiles append a / to LIB_PREFIX, remove it
MAK_LIBS		:= $(patsubst $(THIS_DIR)/LIB/%,$(THIS_DIR)/LIB/lib%,$(MAK_LIBS))

COMP_NAME := $(MAK_NAME)
COMP_OBJ  := ../../$(PRG_OUTPUT_DIR)/$(COMP_NAME)

TMP_OBJ_DIR	  := OBJ/$(COMP_NAME)

TMP_COMP_OBJS := $(MAK_INP)

vpath %.c $(MEN_MOD_DIR)

#**************************************
#   include pathes
#
ifdef INITIAL_INC_DIR
INC_DIRS := -I$(INITIAL_INC_DIR)
else
INC_DIRS := 
endif

NAT_INC_DIR     :=  $(MEN_LIN_DIR)/INCLUDE/NATIVE

ifdef LIN_USR_INC_DIR
	INC_DIRS += -I$(LIN_USR_INC_DIR)
endif


INC_DIRS+=\
    -I$(INC_DIR)                    \
    -I$(NAT_INC_DIR)                \
    -I$(MEN_MOD_DIR)

#**************************************
#   Compiler flags
#
# include the kernel settings (c-compiler etc...)

include $(THIS_DIR)/.kernelsettings
CC				:= $(KERNEL_CC)
include $(TPL_DIR)/endian.mak

ifndef CODE_COVERAGE
    CODE_COVERAGE := 
endif

ifeq ($(CODE_COVERAGE),  )
    FLAGS         :=-O2 
else
    FLAGS         :=-O0
endif

DEF             :=-DLINUX $(MDIS_EXTRA_DEFS)

ifndef MAK_SWITCH
    MAK_SWITCH := 
endif

# check if we have a 64bit system
LONG_WIDTH = $(shell $(CC) $(TPL_DIR)/checkTypeSize.c 2>&1 \
	          | awk '/\#error/ {print $$NF}')

ifeq ($(LONG_WIDTH),_64)	
  DEF += -D_LIN64
endif

ifndef WARN_LEVEL
# enable all warnings, but disable warning about wrong type passing to printf 
    WARN_LEVEL := -Wall -Wno-format
endif


CFLAGS          :=  $(FLAGS) $(DEF) $(MAK_SWITCH) $(INC_DIRS) \
					$(WARN_LEVEL) $(ENDIAN) $(CODE_COVERAGE)

LDFLAGS			:= -g -lm -static $(CODE_COVERAGE)

#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all installbin buildmsg buildinobjdir build 

all: buildmsg $(TMP_OBJ_DIR) $(PRG_OUTPUT_DIR) buildinobjdir

buildmsg:
	@$(ECHO)
	@$(ECHO) ++++++++ Building $(COMP_NAME) +++++++++++

$(TMP_OBJ_DIR):
	- @ mkdir -p $(TMP_OBJ_DIR)
	@$(ECHO) Directory $(TMP_OBJ_DIR) created

$(PRG_OUTPUT_DIR):
	- @ mkdir -p $(PRG_OUTPUT_DIR)
	@$(ECHO) Directory $(PRG_OUTPUT_DIR) created


buildinobjdir:
	@$(MAKE) -C $(TMP_OBJ_DIR) --no-print-directory\
	 -f $(USRPROG_MAK) build

installbin:
ifdef BIN_INSTALL_DIR
	@$(ECHO) installing $(COMP_NAME) in $(BIN_INSTALL_DIR)
	@install --mode=755 --directory $(BIN_INSTALL_DIR)
	@install --mode=755 \
	 $(PRG_OUTPUT_DIR)/$(COMP_NAME) $(BIN_INSTALL_DIR)
endif

#----------------------------------------------------------------------
# Rules for level #1 of make
#

build: $(COMP_OBJ)

# override default buildin rule for .c -> .o files (just to get less messages)
%.o : %.c
	@$(ECHO) Compiling $*.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(COMP_OBJ): $(TMP_COMP_OBJS) $(MAK_LIBS)
	@$(ECHO) Linking $@
	$(Q)$(CC) $(LDFLAGS) $(TMP_COMP_OBJS) $(MAK_LIBS) -o $@

# Dependencies
$(TMP_COMP_OBJS): 	$(MAK_INCL) \
			 		$(COMMAKE) 



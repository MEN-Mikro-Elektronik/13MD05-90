#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2009/09/23 11:29:55 $
#      $Revision: 1.8 $
#  
#    Description: Make a Linux MDIS user state library (shared library)
#                      
#       Switches: 
#				  COMMAKE		common makefile name (with path)
#				  WARN_LEVEL 	(default -Wall)
#				  THIS_DIR		directory where this file is located
#				  MEN_LIN_DIR	absolute path to directory that contains the
#								MDIS4LINUX tree
#				  LIN_USR_INC_DIR	where to find linux includes (/usr/include)
#		Rules:	  all			main rule for libraries
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

# the following three macros are don't care for Linux
LIB_PREFIX      := xxx
LIB_SUFFIX      := xxx
MEN_LIB_DIR     := xxx
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

COMP_NAME := $(MAK_NAME)
LIB_NAME  := lib$(COMP_NAME).so
COMP_OBJ  := ../../$(LIB_OUTPUT_DIR)/$(LIB_NAME)


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

FLAGS           :=-O2 
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
					$(WARN_LEVEL) $(ENDIAN)

#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all buildmsg buildinobjdir build

all: buildmsg $(TMP_OBJ_DIR) $(LIB_OUTPUT_DIR) buildinobjdir 

buildmsg:
	@$(ECHO) 
	@$(ECHO) ++++++++ Building shared user library $(LIB_NAME) +++++++++++

$(TMP_OBJ_DIR):
	- @ mkdir -p $(TMP_OBJ_DIR)
	@$(ECHO) Directory $(TMP_OBJ_DIR) created

$(LIB_OUTPUT_DIR):
	- @ mkdir -p $(LIB_OUTPUT_DIR)
	@$(ECHO) Directory $(LIB_OUTPUT_DIR) created

installlibs:
ifdef LIB_INSTALL_DIR
	@$(ECHO) installing $(LIB_NAME) in $(LIB_INSTALL_DIR)
	$(Q)install --directory --mode=755 $(LIB_INSTALL_DIR)
	$(Q)install --mode=644  \
	 $(LIB_OUTPUT_DIR)/$(LIB_NAME) $(LIB_INSTALL_DIR)
endif

installstaticlibs:

buildinobjdir:
	@$(MAKE) -C $(TMP_OBJ_DIR) --no-print-directory\
	 -f $(USRLIB_MAK) build


#----------------------------------------------------------------------
# Rules for level #1 of make
#

build: $(COMP_OBJ)

# override default buildin rule for .c -> .o files (just to get less messages)
# -fPIC tells the compiler to produce position independent code
%.o : %.c
	@$(ECHO) Compiling $*.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -c $< -o $@

# create shared library
$(COMP_OBJ): $(TMP_COMP_OBJS)
	$(Q)$(CC) -shared -Wl,-soname,$(LIB_NAME) -o $(COMP_OBJ) $(TMP_COMP_OBJS) -lc

# Dependencies
$(TMP_COMP_OBJS): 	$(MAK_INCL) \
			 		$(COMMAKE) 



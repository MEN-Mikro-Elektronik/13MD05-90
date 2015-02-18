#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2004/06/09 11:12:57 $
#      $Revision: 1.5 $
#  
#    Description: Make LINUX MDIS descriptors
#                      
#       Switches: DESC			filename of descriptor (without extension)
#				  THIS_DIR		directory where this file is located
#				  MEN_LIN_DIR	absolute path to directory that contains the
#								MDIS4LINUX tree
#
#		Rules:	  all			make descriptor
#			     
#				  		  
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

OBJOUTPUT_DIR := DESC

vpath %.dsc $(THIS_DIR)

include $(THIS_DIR)/.kernelsettings
CC = $(KERNEL_CC)
include $(TPL_DIR)/endian.mak

#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all clean installdesc buildmsg

all: buildmsg $(OBJOUTPUT_DIR)/$(DESC).bin

buildmsg:
	@$(ECHO) +
	@$(ECHO) ++++++++ Building descriptor $(DESC) +++++++++++

$(OBJOUTPUT_DIR)/$(DESC).bin: $(THIS_DIR)/$(DESC).dsc
	- @mkdir -p $(OBJOUTPUT_DIR)
	$(Q)$(_DESCGEN) $(DESCGEN_BYTEORDER) -r $< -o=$(OBJOUTPUT_DIR)


ALL_DESC_BINS := $(shell echo $(OBJOUTPUT_DIR)/*.bin)

installdesc:
ifdef ALL_DESC
	@$(ECHO) installing $(ALL_DESC_BINS) in $(DESC_INSTALL_DIR)
	@install --directory $(DESC_INSTALL_DIR)
	@install --mode=644  \
	 $(ALL_DESC_BINS) $(DESC_INSTALL_DIR)
endif


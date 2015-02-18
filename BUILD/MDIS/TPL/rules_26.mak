#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2011/02/11 16:15:04 $
#      $Revision: 2.7 $
#  
#    Description: Defines top level rules for MDIS components (Kernel 2.6)
#     
#       Switches: 
#		Rules:	  
#
#-------------------------------[ History ]---------------------------------
#
#  $Log: rules_26.mak,v $
#  Revision 2.7  2011/02/11 16:15:04  CRuff
#  R: 1. files .kernelsettings and .kernelsubdirs were left over after clean and
#        caused problems with mixed permissions
#     2. mdis device node is created in udev device directory with make install
#  M: 1. also remove .kernelsubdirs and .kernelsettings with make clean
#     2. remove mdis device node from udev dir during cleanup
#
#  Revision 2.6  2010/10/19 13:46:32  CRuff
#  R: 1. chameleon library has to be build as native module
#     2. cosmetics
#  M: 1. change module type for _ALL_CORE_X86 from module to nat_module
#     2. rename symbol _ALL_CORE_X86 to _ALL_CORE_COM_X86
#
#  Revision 2.5  2010/08/23 17:13:30  CRuff
#  R: 1. chameleon lib is now built as core library (ALL_CORE_LIBS)
#     2. cosmetics
#  M: 1. changed module type of chameleon lib from nat_module to module
#     2. added revision history string to file header
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000-2004 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************
export COMP_MAK    := $(TPL_DIR)component_26.mak

#----------------------------------------
# Rules
#
.PHONY: buildmods builddbgs \
		all_kernel all_ll all_bb all_core all_raw  \
		installmods 
		$(ALL_DBGS) $(ALL_LL_DRIVERS) \
		$(ALL_BB_DRIVERS) $(ALL_CORE) $(ALL_KERNEL)
		$(ALL_RAW) $(ALL_NATIVE_DRIVERS) 

# rule to build debug and/or non-debug version of all modules
buildmods: $(ALL_DBGS)

$(ALL_DBGS): 
	$(MAKEIT) RULE=$(RULE) DEBUG=$@ buildfordbg

buildfordbg: kernelsettings kernelsubdirs all_kernel all_core  \
			 all_ll all_bb all_raw callkernelbuild


clean: 
	@$(ECHO) "Removing all objects, modules, binaries, libraries, descriptors"
	rm -rf .kernelsettings .kernelsubdirs OBJ BIN LIB DESC
	rm -f $(_DESCGEN_OBJ)/*
	rm -f /lib/udev/devices/mdis

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


all_ll:			$(ALL_LL_DRIVERS) 
all_bb:			$(ALL_BB_DRIVERS)
all_core: 		$(ALL_CORE)
all_kernel:		$(ALL_KERNEL)
all_raw: 		$(ALL_RAW) $(ALL_NATIVE_DRIVERS)


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

#
# The actual call to the kernel build 
# This will work only with Linux >=2.6.6

.PHONY: callkernelbuild

callkernelbuild:
	@$(ECHO) "++++++++ Building kernel modules ++++++++++"
	@$(ECHO) -n "obj-m +=" >OBJ/Makefile
	cat $(THIS_DIR)/.kernelsubdirs >>OBJ/Makefile
	$(Q)$(MAKE) -C $(LIN_KERNEL_DIR) SUBDIRS=$(THIS_DIR)/OBJ


#	$(Q)$(MAKE) -C $(LIN_KERNEL_DIR) \
#	 "SUBDIRS=$(shell cat $(THIS_DIR)/.kernelsubdirs)" modules

#----------------------------------------
# Get byteorder of target machine 
#
# Find out byteordering for target
# run the target's preprocessor on a dummy source file that includes
# endian.h
#
#
# $Log: endian.mak,v $
# Revision 2.4  2011/02/11 16:18:07  CRuff
# R: typo (additional space) caused build errors
# M: removed space character
#
# Revision 2.3  2007/12/10 12:49:59  ts
# enhanced endianess detection for descgen byteorder to generate proper
# byte endings on cygwin (Windows) environments
#
# 2.2  01.06.2006 11:38:55 by ts
# use $$NF instead $$3 to ensure 'big' or 'little' is catched
#
# 2.1  09.06.2004 11:13:00 by kp
# Initial Revision
#
# $(warning - HOST_OS = $(HOST_OS) ----)

ifdef KERNEL_CC
 ifndef DESCGEN_BYTEORDER
 export HOST_OS = $(shell uname -s | awk '{print toupper(substr($$NF,1,5))}')

 export TARGET_BYTEORDER = $(shell $(CC) $(TPL_DIR)/gettargetbyteorder.c 2>&1 \
	| awk '/\#error/ {print $$NF}')
#$(warning - HOST_OS = $(HOST_OS) ----)
  ifeq ($(HOST_OS),LINUX)
   ifeq ($(TARGET_BYTEORDER),little)
	 export DESCGEN_BYTEORDER = -bin_lit
	 export ENDIAN=-D_LITTLE_ENDIAN_
   endif
   ifeq ($(TARGET_BYTEORDER),big)
	 export DESCGEN_BYTEORDER = -bin_big
	 export ENDIAN=-D_BIG_ENDIAN_ 
   endif
  endif 
  ifeq ($(HOST_OS),CYGWI)
   ifeq ($(TARGET_BYTEORDER),little)
	 export DESCGEN_BYTEORDER = -bin_big
	 export ENDIAN=-D_LITTLE_ENDIAN_
   endif
   ifeq ($(TARGET_BYTEORDER),big)
	 export DESCGEN_BYTEORDER = -bin_lit
	 export ENDIAN=-D_BIG_ENDIAN_ 
   endif
  endif

 else # already defined external
	export DESCGEN_BYTEORDER
 endif # ifndef DESCGEN_BYTEORDER

 # sanity checks
 ifndef DESCGEN_BYTEORDER
$(warning *** DESCGEN_BYTEORDER not set or Host OS not detected  )
 ERROR!
 endif

endif #KERNEL_CC

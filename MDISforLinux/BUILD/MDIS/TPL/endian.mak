#----------------------------------------
# Get byteorder of target machine 
#
# Find out byteordering for target
# run the target's preprocessor on a dummy source file that includes
# endian.h
#
#-----------------------------------------------------------------------------
#   Copyright 2004-2019, MEN Mikro Elektronik GmbH
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

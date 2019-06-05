#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#
#    Description: Determine compiler/linker and flags used for kernel
#                 Invoke this within the kernel's root dir
#       Switches:
#          Rules:
#
#-----------------------------------------------------------------------------
#   Copyright (c) 2002-2019, MEN Mikro Elektronik GmbH
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
#
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.

include Makefile

KERNEL_SETTINGS_FILE ?= /dev/null

.DEFAULT_GOAL := getsettings_for_mdis

getsettings_for_mdis:
	@echo "KERNEL_CC := $(CC)" > $(KERNEL_SETTINGS_FILE)
	@echo "KERNEL_LD := $(LD)" >> $(KERNEL_SETTINGS_FILE)
	@echo "KERNEL_CFLAGS := $(CFLAGS)" >> $(KERNEL_SETTINGS_FILE)
	@echo "KERNEL_LDFLAGS := $(LDFLAGS)" >> $(KERNEL_SETTINGS_FILE)
	@echo "KERNEL_ARCH := $(ARCH)" >> $(KERNEL_SETTINGS_FILE)

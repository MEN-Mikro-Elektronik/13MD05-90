#
# Makefile for VME4L_API examples in case you don't want
# to use the MDIS build system
#
#/*
# * This program is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 2 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# */

# your native or cross compiler
CC = gcc


API = ../../LIBSRC/VME4L_API/libvme4l_api.a
CFLAGS = -I ../../INCLUDE/NATIVE 

all: vme4l_ctrl vme4l_irqgen vme4l_m99irq vme4l_mmap vme4l_rwex \
	 vme4l_rwspeed vme4l_spcycle vme4l_crcsr

vme4l_ctrl: VME4L_CTRL/vme4l_ctrl.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

vme4l_irqgen: VME4L_IRQGEN/vme4l_irqgen.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

vme4l_m99irq: VME4L_M99IRQ/vme4l_m99irq.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

vme4l_mmap: VME4L_MMAP/vme4l_mmap.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

#vme4l_mtest: VME4L_MTEST/vme4l_mtest.c
#	$(CC) $(CFLAGS) $< -o $@ $(API)

vme4l_rwex: VME4L_RWEX/vme4l_rwex.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

vme4l_rwspeed: VME4L_RWSPEED/vme4l_rwspeed.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

vme4l_spcycle: VME4L_SPCYCLE/vme4l_spcycle.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

vme4l_crcsr: VME4L_CRCSR/vme4l_crcsr.c
	$(CC) $(CFLAGS) $< -o $@ $(API)

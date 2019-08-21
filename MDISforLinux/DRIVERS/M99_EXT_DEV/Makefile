#***************************  M a k e f i l e  *******************************
#   Copyright 2019, MEN Mikro Elektronik GmbH
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

LINUX_DIR = /usr/src/linux

MEN_COM	  = /opt/menlinux/INCLUDE/COM
MEN_NAT	  = /opt/menlinux/INCLUDE/NATIVE

CC			= gcc
LD			= ld

CFLAGS		= -Wall -O2 -I$(LINUX_DIR)/include \
			  -I$(MEN_COM) -I$(MEN_NAT) \
			  -D__KERNEL__ -DMODULE


m99.o: m99.c
	$(CC) $(CFLAGS) -c $<





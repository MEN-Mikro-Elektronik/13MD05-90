#
# Makefile for static VME4L_API library in case you don't want
# to use the MDIS build system
#
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

# your native or cross compiler
CC = gcc

LIB = libvme4l_api.a

CFLAGS = -I ../../INCLUDE/NATIVE

vme4l_api.o: vme4l_api.c
	$(CC) $(CFLAGS) -c $< -o $@
	ar rv $(LIB) $@ >/dev/null


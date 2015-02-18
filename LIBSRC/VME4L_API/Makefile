#
# Makefile for static VME4L_API library in case you don't want
# to use the MDIS build system
#

# your native or cross compiler
CC = gcc

LIB = libvme4l_api.a

CFLAGS = -I ../../INCLUDE/NATIVE

vme4l_api.o: vme4l_api.c
	$(CC) $(CFLAGS) -c $< -o $@
	ar rv $(LIB) $@ >/dev/null


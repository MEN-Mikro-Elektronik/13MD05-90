#
#	Makefile 
#

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





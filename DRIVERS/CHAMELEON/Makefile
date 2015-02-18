############################  M a k e f i l e ###############################/
#
#      \author  thomas.schnuerer@men.de
#        $Date: 2009/02/11 12:01:52 $
#    $Revision: 1.3 $
# 
#	   \project  ElinOS/Linux BSP for MEN PPC5200 CPUs
#      
#      \brief Sub-Makefile for men chameleon FPGA support, native driver
#
#-------------------------------[ History ]---------------------------------
#
# $Log: Makefile,v $
# Revision 1.3  2009/02/11 12:01:52  ts
# R: different location for native men_chameleon driver needed
#
#
#---------------------------------------------------------------------------
# (c) Copyright 2000-2009 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
#############################################################################
#
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now inherited from the
# parent makes..

EXTRA_CFLAGS += $(call addinclude,include/men/com) \
	            $(call addinclude,include/men/native) \
                                -DLINUX \
                                -D_BIG_ENDIAN_ \
                                -DMAC_MEM_MAPPED \
                                -DCHAMV2_VARIANT=Mem

obj-$(CONFIG_MEN_CHAMELEON) += men_chameleon.o

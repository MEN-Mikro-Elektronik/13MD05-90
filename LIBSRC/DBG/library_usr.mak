#**************************  M a k e f i l e ********************************
#
#         Author: christian.schuster@men.de
#          $Date: 2005/07/08 11:40:36 $
#      $Revision: 1.1 $
#
#    Description: makefile descriptor for user space DBG library
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_usr.mak,v $
#   Revision 1.1  2005/07/08 11:40:36  cs
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
# (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
#*****************************************************************************

MAK_NAME=dbg_usr

MAK_LIBS=

MAK_SWITCH= $(SW_PREFIX)MAC_MEM_MAPPED \
			$(SW_PREFIX)MAC_USERSPACE

MAK_INCL=

MAK_INP1=dbgu$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

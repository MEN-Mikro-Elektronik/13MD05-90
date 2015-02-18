#***************************  M a k e f i l e  *******************************
#
#         Author: see
#          $Date: 2012/12/05 10:57:13 $
#      $Revision: 1.3 $
#
#    Description: Makefile descriptor file for USR_UTL lib
#
#---------------------------------[ History ]---------------------------------
#
#
#   ts@men.de: permit.c, protect.c and log.c removed.. 
#              was an OS9 special cucumber..
#
#   $Log: library.mak,v $
#   Revision 1.3  2012/12/05 10:57:13  ww
#   added permit.c, protect.c and log.c.
#
#   Revision 1.2  1998/09/09 10:01:07  Schmidt
#   sourcefile ident.c added
#
#   Revision 1.1  1998/07/02 15:29:09  see
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=usr_utl

MAK_LIBS=


MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/usr_utl.h

MAK_OPTIM=$(OPT_1)

MAK_INP1=atox$(INP_SUFFIX)
MAK_INP2=bindump$(INP_SUFFIX)
MAK_INP3=memdump$(INP_SUFFIX)
MAK_INP4=tstopt$(INP_SUFFIX)
MAK_INP5=ident$(INP_SUFFIX)

MAK_INP=\
        $(MAK_INP1)\
        $(MAK_INP2)\
        $(MAK_INP3)\
        $(MAK_INP4)\
        $(MAK_INP5)


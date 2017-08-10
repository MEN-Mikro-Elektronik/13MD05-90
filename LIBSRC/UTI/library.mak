#***************************  M a k e f i l e  *******************************
#  
#         Author: gl
#          $Date: 2008/09/15 12:57:33 $
#      $Revision: 1.1 $
#                      
#    Description: Makefile descriptor file for switch confgiration lib
#                  (Marvell 88E6095)
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  2008/09/15 12:57:33  GLeonhardt
#   Initial Revision
#
#   Revision 1.2  2008/05/19 13:13:53  GLeonhardt
#   R:1. Virtual LANs added
#   M:1. Added dependency of mrvl88E6095.h
#
#   Revision 1.1  2008/03/03 14:09:18  GLeonhardt
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=uti

MAK_SWITCH=


MAK_INCL= 

MAK_INP01 = ask$(INP_SUFFIX)
MAK_INP02 = cntarg$(INP_SUFFIX)
MAK_INP03 = cntopt$(INP_SUFFIX)
MAK_INP04 = dirname$(INP_SUFFIX)
MAK_INP05 = dl_list$(INP_SUFFIX)
MAK_INP06 = dump$(INP_SUFFIX)
MAK_INP07 = fdump$(INP_SUFFIX)
MAK_INP08 = filename$(INP_SUFFIX)
MAK_INP09 = getint$(INP_SUFFIX)
MAK_INP10 = getshorterrmsg$(INP_SUFFIX)
MAK_INP11 = getstr$(INP_SUFFIX)
MAK_INP12 = line_args$(INP_SUFFIX)
MAK_INP13 = look_string$(INP_SUFFIX)
MAK_INP14 = macro_ex$(INP_SUFFIX)
MAK_INP15 = make_path$(INP_SUFFIX)
MAK_INP16 = panic$(INP_SUFFIX)
MAK_INP17 = printtime$(INP_SUFFIX)
MAK_INP18 = smatch$(INP_SUFFIX)
MAK_INP19 = str_to_upper$(INP_SUFFIX)
MAK_INP20 = str_to_lower$(INP_SUFFIX)
MAK_INP21 = strsave$(INP_SUFFIX)
MAK_INP22 = tstopt$(INP_SUFFIX)

MAK_INP = $(MAK_INP01) $(MAK_INP02) $(MAK_INP03) $(MAK_INP04) \
		  $(MAK_INP05) $(MAK_INP06) $(MAK_INP07) $(MAK_INP08) \
		  $(MAK_INP09) $(MAK_INP10) $(MAK_INP11) $(MAK_INP12) \
		  $(MAK_INP13) $(MAK_INP14) $(MAK_INP15) $(MAK_INP16) \
		  $(MAK_INP17) $(MAK_INP18) $(MAK_INP19) $(MAK_INP20) \
		  $(MAK_INP21) $(MAK_INP22)




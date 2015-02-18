#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffermen.de
#         $Date: 2005/08/03 11:22:44 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for chameleon BBIS for F8
#
#****************************************************************************

CHAMF8 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   CHAMELEON_PCITBL   # driver

	#------------------------------------------------------------------------
	#	PCI specific parameters   
	#------------------------------------------------------------------------
    PCI_BUS_NUMBER 	 	= U_INT32  0x00
    PCI_DEVICE_NUMBER	= U_INT32  0x0d

    DEVICE_ID_0 		= U_INT32  0x0800 	# 16Z029_CAN instance 0
    DEVICE_ID_1 		= U_INT32  0x0801 	# 16Z029_CAN instance 1
    DEVICE_ID_2 		= U_INT32  0x1900 	# 16Z034_GPIO instance 0
}

#************************** BBIS3 board descriptor **************************
#
#        Author: kp
#         $Date: 2004/06/08 16:25:29 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for chameleon BBIS
#
#****************************************************************************

CHAMEM05EC01 {
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

    DEVICE_ID_0 		= U_INT32  0x0800 	# CAN boromir instance 0
    DEVICE_ID_1 		= U_INT32  0x0801 	# CAN boromir instance 1
}

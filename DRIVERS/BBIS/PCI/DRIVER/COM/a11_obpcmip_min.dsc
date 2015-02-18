#************************** BBIS3 board descriptor **************************
#
#        Author: kp
#         $Date: 1999/08/12 09:12:07 $
#     $Revision: 1.3 $
#
#   Description: Metadescriptor for A11 onboard PC-MIP slots
#
#****************************************************************************

A11_OBPCMIP {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   PCI         # hardware name of device

	#------------------------------------------------------------------------
	#	PCI specific parameters (don't modify it !)
	#------------------------------------------------------------------------
	PCI_BUS_PATH		= BINARY   0x10		   # device number of PCI-PCI
											   # bridge on bus 0 to PC-MIPs
	DEVICE_SLOT_0		= U_INT32  0x00
	DEVICE_SLOT_1		= U_INT32  0x01

}

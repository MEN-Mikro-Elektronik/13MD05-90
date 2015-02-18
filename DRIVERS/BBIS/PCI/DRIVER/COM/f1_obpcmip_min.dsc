#************************** BBIS3 board descriptor **************************
#
#        Author: kp
#         $Date: 2001/08/16 10:06:55 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for F1 onboard PC-MIP slots
#
#****************************************************************************

F1_OBPCMIP {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   PCI         # hardware name of device

	#------------------------------------------------------------------------
	#	PCI specific parameters (don't modify it !)
	#------------------------------------------------------------------------
	PCI_BUS_PATH		= BINARY   			   # leave blank!

	DEVICE_SLOT_0		= U_INT32  0x1a
	DEVICE_SLOT_1		= U_INT32  0x1d
}

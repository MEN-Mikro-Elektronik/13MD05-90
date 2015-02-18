#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/03/16 16:17:29 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for C203
#
#****************************************************************************

C203_SW_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   C203_SW	# hardware name of device

	#------------------------------------------------------------------------
	#	PCI configuration
	#------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0		# always 0 in standard PCs
    PCI_DEVICE_ID       = U_INT32 10		# pci device ID (example)
    PCI_CHECK_LOCATION  = U_INT32  0		# don't check geographic location
}

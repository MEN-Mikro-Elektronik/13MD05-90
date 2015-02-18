#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2001/11/12 15:51:09 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for D302_SW
#
#****************************************************************************

D302_SW_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   D302_SW	# hardware name of device

	#------------------------------------------------------------------------
	#	PCI configuration
	#------------------------------------------------------------------------
	# define device IDs of bridges to CompactPCI backplane
	# see D201/DOC/pcibuspath.txt for list

	PCI_BUS_PATH		= BINARY   0x08		# device IDs of bridges to 
											# compact PCI bus
    PCI_BUS_SLOT        = U_INT32  3		# Compact PCI bus slot 1=CPU
    PCI_CHECK_LOCATION  = U_INT32  0		# don't check geographic location
    #PCI_BUS_NUMBER     = U_INT32  1		# optional overwrites PCI_BUS_PATH
    #PCI_DEVICE_ID      = U_INT32 10		# optional overwrites PCI_BUS_SLOT

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
    ID_CHECK            = U_INT32  1		# check board ID prom
}

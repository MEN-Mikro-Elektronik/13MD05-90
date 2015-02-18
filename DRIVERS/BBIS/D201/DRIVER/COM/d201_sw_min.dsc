#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/03/16 16:17:25 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for D201
#
#****************************************************************************

D201_SW_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   D201_SW	# hardware name of device

	#------------------------------------------------------------------------
	#	PCI configuration
	#------------------------------------------------------------------------

	# define device IDs of bridges to CompactPCI backplane
	# see D201/DOC/pcibuspath.txt for list

	PCI_BUS_PATH		= BINARY   0x08		# device IDs of bridges to 
											# compact PCI bus
    PCI_BUS_SLOT        = U_INT32  3		# Compact PCI bus slot 1=CPU
    #PCI_BUS_NUMBER     = U_INT32  1		# optional overwrites PCI_BUS_PATH
    #PCI_DEVICE_ID      = U_INT32 10		# optional overwrites PCI_BUS_SLOT
}

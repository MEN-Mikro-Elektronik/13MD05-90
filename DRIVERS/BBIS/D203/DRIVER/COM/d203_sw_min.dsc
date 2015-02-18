#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2003/01/28 16:24:15 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for D203_SW
#
#****************************************************************************

D203_SW_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   D203_SW	# hardware name of device

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

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
    ID_CHECK            = U_INT32  1		# check board ID prom
}

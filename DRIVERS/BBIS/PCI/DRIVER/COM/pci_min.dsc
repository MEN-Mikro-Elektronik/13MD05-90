#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/08/30 16:39:34 $
#     $Revision: 1.2 $
#
#   Description: Template Metadescriptor for generic PCI base boards
#
#****************************************************************************

PCI_xxx {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   PCI         # hardware name of device

	#------------------------------------------------------------------------
	#	PCI specific parameters:
	#   
	#   PCI_BUS_PATH specifies a list of PCI device numbers of all PCI-PCI
	#     bridges which connect PCI bus 0 with the PCI bus of the slots
	#     which the PCI BBIS should handle.
	# 
	#   Example: '0x0a,0x11,0x0c' bus of bridge  |  pci device number
	#                             -----------------------------------
	#                                         0  |  0x0a
	#                                         1  |  0x11
	#                                         2  |  0x0c
	#                                      
	#   PCI_BUS_NUMBER can be used to specify a fix PCI bus number for the
	#     slots that the PCI BBIS should handle. If this key is set, the
	#     PCI_BUS_PATH will not be used.
	#     Note: The bus number may change if additional devices are installed.
	#
	#   DEVICE_SLOT_n associates for each existing slot the corresponding
	#     PCI device number.
	#
	#------------------------------------------------------------------------
	PCI_BUS_PATH		= BINARY   0x0a,0x11,0x0c
	
	#PCI_BUS_NUMBER		= U_INT32  2      # optional: overwrites PCI_BUS_PATH
	
	DEVICE_SLOT_0		= U_INT32  0x0C
	DEVICE_SLOT_1		= U_INT32  0x0B
	DEVICE_SLOT_2		= U_INT32  0x0D
	DEVICE_SLOT_3		= U_INT32  0x0E
}

#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/03/16 16:17:31 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for C204
#
#****************************************************************************

C204_SW_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   C204_SW	# hardware name of device

	#------------------------------------------------------------------------
	#	PCI configuration
	#------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0		# always 0 in standard PCs
    PCI_DEVICE_ID       = U_INT32 10		# pci device ID (example)
    PCI_CHECK_LOCATION  = U_INT32  0		# don't check geographic location

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#   this keys have only effect on debug drivers
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32  0xc0008000	# BBIS driver
    DEBUG_LEVEL_BK      = U_INT32  0xc0008000	# BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32  0xc0008000	# OSS calls
    DEBUG_LEVEL_DESC    = U_INT32  0xc0008000	# DESC calls

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
    ID_CHECK            = U_INT32  1	# check board ID prom
	PLD_LOAD			= U_INT32  1	# load PLD
	NONE_A24			= U_INT32  0	# request no A24 addr space (requires
										#  board with special eeprom data) 
}

#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/08/30 16:39:37 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for D202 PC*MIP carrier board
#                in a MEN standard D1 system with five CPCI slots.
#
#****************************************************************************

D202_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   PCI         # hardware name of device

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#   this keys have only effect on debug drivers
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32  0xc0008000  # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32  0xc0008000  # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32  0xc0008000  # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32  0xc0008000  # DESC calls

	#------------------------------------------------------------------------
	#	PCI specific parameters   
	#------------------------------------------------------------------------
	#
	# Select the right PCI_BUS_PATH according to the geographical
	# backplane slot where the D202 is plugged in.
	#
	# Note: 0x14 is the PCI device number of the PCI-to-PCI bridge which
	#       connects the backplane (bus 1) of the D1 system with PCI bus 0. 
	 
	#PCI_BUS_PATH		= BINARY   0x14,0x0c	# for slot 5
	#PCI_BUS_PATH		= BINARY   0x14,0x0d	# for slot 4
	PCI_BUS_PATH		= BINARY   0x14,0x0e	# for slot 3
	#PCI_BUS_PATH		= BINARY   0x14,0x0f	# for slot 2

	# Don't modify the DEVICE_SLOT_x parameters!
	
	DEVICE_SLOT_0		= U_INT32  0x0C		# PC*MIP slot A
	DEVICE_SLOT_1		= U_INT32  0x0D		# PC*MIP slot B
	DEVICE_SLOT_2		= U_INT32  0x0E		# PC*MIP slot C
	DEVICE_SLOT_3		= U_INT32  0x0F		# PC*MIP slot D
	DEVICE_SLOT_4		= U_INT32  0x0A		# PC*MIP slot E
	DEVICE_SLOT_5		= U_INT32  0x0B		# PC*MIP slot F
}

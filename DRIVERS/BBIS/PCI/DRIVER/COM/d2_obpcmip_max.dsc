#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/08/30 16:39:45 $
#     $Revision: 1.3 $
#
#   Description: Metadescriptor for D2 onboard PC*MIP slots
#
#****************************************************************************

D2_OBPCMIP {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   PCI		# hardware name of device

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#   this keys have only effect on debug drivers
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32  0xc0008000  # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32  0xc0008000  # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32  0xc0008000  # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32  0xc0008000  # DESC calls

	#------------------------------------------------------------------------
	#	PCI specific parameters (don't modify)
	#------------------------------------------------------------------------
	PCI_BUS_PATH		= BINARY   
	DEVICE_SLOT_0		= U_INT32  0x0C   	# PC*MIP slot A    
	DEVICE_SLOT_1		= U_INT32  0x0B		# PC*MIP slot B
	DEVICE_SLOT_2		= U_INT32  0x0D		# PC*MIP slot E
}


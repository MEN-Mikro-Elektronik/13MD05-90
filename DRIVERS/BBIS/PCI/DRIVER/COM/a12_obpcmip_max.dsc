#************************** BBIS3 board descriptor **************************
#
#        Author: kp
#         $Date: 2001/08/16 10:06:51 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for A12 onboard PC-MIP/PMC slots
#
#****************************************************************************

A12_OBPCMIP {
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
	#	PCI specific parameters (don't modify it !)
	#------------------------------------------------------------------------
	PCI_BUS_PATH		= BINARY   0x1d		   # device number of PCI-PCI
											   # bridge on bus 0 to PC-MIPs

	DEVICE_SLOT_0		= U_INT32  0x00		   # PC-MIP 0		
	DEVICE_SLOT_1		= U_INT32  0x01		   # PC-MIP 1	
	DEVICE_SLOT_2		= U_INT32  0x02		   # PC-MIP 2	
	DEVICE_SLOT_3		= U_INT32  0x03		   # PMC 0
	DEVICE_SLOT_4		= U_INT32  0x04		   # PMC 1

}

#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2004/09/06 13:02:58 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for generic ISA BBIS - memory access
#
#****************************************************************************

ISA_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   ISA         # hardware name of device

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#   this keys have only effect on debug drivers
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32  0xc0008000  # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32  0xc0008000  # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32  0xc0008000  # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32  0xc0008000  # DESC calls

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
    DEVICE_ADDR 		= U_INT32 0x00f00000  # device base address (mem or i/o)
	DEVICE_ADDRSIZE 	= U_INT32 0x00000100  # device address space size
	IRQ_NUMBER 			= U_INT32 0x09		  # irq number (or 0 if no interrupt)
}

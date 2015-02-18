#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2006/08/29 13:30:01 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for D203
#
#****************************************************************************

D203_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   D203		# hardware name of device

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
    ID_CHECK            = U_INT32  1		# check board ID prom
    
    #--- Global Trigger Settings (see HW manual)
    PXI_TRIG_SRC        = U_INT32  0x00		# PXI Source
    PXI_TRIG_DST        = U_INT32  0x00		# PXI Destination
    
    #--- M-Module specific Trigger Settings (see HW manual)
    MSLOT_0 {                               # M-Module #0
        TRIGA_SRC       = U_INT32  0x00		# M-Module Source TRIGA
        TRIGA_DST       = U_INT32  0x00		# M-Module Destination TRIGA
        TRIGB_SRC       = U_INT32  0x00		# M-Module Source TRIGB
        TRIGB_DST       = U_INT32  0x00		# M-Module Destination TRIGB
    }
    MSLOT_1 {                               # M-Module #1
        TRIGA_SRC       = U_INT32  0x00		# M-Module Source TRIGA
        TRIGA_DST       = U_INT32  0x00		# M-Module Destination TRIGA
        TRIGB_SRC       = U_INT32  0x00		# M-Module Source TRIGB
        TRIGB_DST       = U_INT32  0x00		# M-Module Destination TRIGB
    }
    MSLOT_2 {                               # M-Module #2
        TRIGA_SRC       = U_INT32  0x00		# M-Module Source TRIGA
        TRIGA_DST       = U_INT32  0x00		# M-Module Destination TRIGA
        TRIGB_SRC       = U_INT32  0x00		# M-Module Source TRIGB
        TRIGB_DST       = U_INT32  0x00		# M-Module Destination TRIGB
    }
    MSLOT_3 {                               # M-Module #3
        TRIGA_SRC       = U_INT32  0x00		# M-Module Source TRIGA
        TRIGA_DST       = U_INT32  0x00		# M-Module Destination TRIGA
        TRIGB_SRC       = U_INT32  0x00		# M-Module Source TRIGB
        TRIGB_DST       = U_INT32  0x00		# M-Module Destination TRIGB
    }
}

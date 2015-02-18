#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffer@men.de
#         $Date: 2005/08/03 12:03:56 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for A203N
#
#****************************************************************************

A203N_1 {
    #------------------------------------------------------------------------
    #   general parameters (don't modify)
    #------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       2             # descriptor type (2=board)
    HW_TYPE         = STRING        A203N         # hardware name of board

    #------------------------------------------------------------------------
    #   debug levels (optional)
    #------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32   0xc0008000    # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32   0xc0008000    # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32   0xc0008000    # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32   0xc0008000    # DESC calls

    #------------------------------------------------------------------------
    #   base address
    #------------------------------------------------------------------------
    # A16/A24 base addr on VMEbus for A08 M-Mod space (required)
	# Note: VME16_ADDR_MMOD08 or VME24_ADDR_MMOD08 must be specified but not
	#       both together.
	#
    #VME16_ADDR_MMOD08  = U_INT32  0x00e00000     # A16 VME base address
    VME24_ADDR_MMOD08   = U_INT32  0x00e00000     # A24 VME base address

    # A32 base addr on VMEbus for A24 M-Mod space (optional)
	# 
    VME32_ADDR_MMOD24   = U_INT32  0x20000000     # A32 VME base address

    #------------------------------------------------------------------------
    #   slot interrupts
    #------------------------------------------------------------------------
    IRQ_VECTOR      = BINARY   0x81, 0x82, 0x83, 0x84  # irq vectors  (slot 0..3) 
    IRQ_LEVEL       = BINARY   0x03, 0x03, 0x03, 0x03  # irq level    (slot 0..3)
    IRQ_PRIORITY    = BINARY   0x00, 0x00, 0x00, 0x00  # irq priority (slot 0..3)
}

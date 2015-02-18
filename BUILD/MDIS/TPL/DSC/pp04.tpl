pp04_SCAN_MDIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING PP04
    _WIZ_MODEL = STRING PP04

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING f207_SCAN_BBIS_INSTANCE
    DEVICE_SLOT = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # mapping between low-level driver address space and PCI base address registers
    PCI_BASEREG_ASSIGN_0 = U_INT32 1
    PCI_VENDOR_ID = U_INT32 0x1172
    PCI_DEVICE_ID = U_INT32 0xc

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}

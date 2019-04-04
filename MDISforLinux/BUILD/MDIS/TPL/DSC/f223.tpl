f223_SCAN_BBIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING PCI
    _WIZ_MODEL = STRING F223
    _WIZ_BUSIF = STRING cpu,1

    # ------------------------------------------------------------------------
    #        PCI configuration
    # ------------------------------------------------------------------------
    PCI_BUS_NUMBER = U_INT32 SCAN_PCI_BUS_NR
    PCI_DEVICE_NUMBER = U_INT32 SCAN_PCI_DEV_NR
    DEVICE_SLOT_0 = U_INT32 0x0
    SKIP_DEV_BRIDGE_CHECK = U_INT32 0x1

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
pi7c9_gpio_SCAN_MDIS_INSTANCE {
    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING PI7C9_GPIO
    _WIZ_MODEL = STRING F223_GPIO

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING f223_SCAN_BBIS_INSTANCE
    DEVICE_SLOT = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------
    PCI_VENDOR_ID = U_INT32 0x12d8
    PCI_DEVICE_ID = U_INT32 0x400a

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008007
    DEBUG_LEVEL_MK = U_INT32 0xc0008007
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008001
}

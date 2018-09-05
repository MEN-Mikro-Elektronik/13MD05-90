d203_a24_SCAN_BBIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING D203_A24
    _WIZ_MODEL = STRING G204_A24
    _WIZ_BUSIF = STRING cpu,SCAN_SMBUSIF

    # ------------------------------------------------------------------------
    #        PCI configuration
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------
    # --- C-PCI parameters
    PCI_BUS_PATH = BINARY SCAN_PCIPATH_PRIM
    PCI_DEVICE_ID = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}

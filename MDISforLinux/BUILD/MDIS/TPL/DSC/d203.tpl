d203_SCAN_BBIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING D203
    _WIZ_MODEL = STRING SCAN_BBIS_NAME
    _WIZ_BUSIF = STRING cpu,SCAN_SMBUSIF

    # ------------------------------------------------------------------------
    #        PCI configuration
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------
    # --- C-PCI parameters
    PCI_BUS_NUMBER = U_INT32 SCAN_PCI_BUS_NR
    PCI_DEVICE_NUMBER = U_INT32 SCAN_PCI_DEV_NR
    PCI_DEVICE_ID = U_INT32 SCAN_PCI_DEV_NR #required by mdiswiz

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}

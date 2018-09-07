
mezz_cham_SCAN_BBIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING CHAMELEON_PCITBL
    _WIZ_MODEL = STRING SCAN_WIZ_MODEL
    _WIZ_BUSIF = STRING SCAN_WIZ_BUSIF

    # ------------------------------------------------------------------------
    #  		PCI configuration
    # ------------------------------------------------------------------------
    PCI_BUS_NUMBER = U_INT32 SCAN_PCI_BUS_NR
    PCI_DEVICE_NUMBER = U_INT32 SCAN_PCI_DEV_NR
    PCI_DEVICE_ID = U_INT32 SCAN_PCI_DEV_NR

    # ------------------------------------------------------------------------
    #        Chameleon BBIS Device: 
    #            DEVICE_IDV2_X is:   ((Cham devId) << 8 | instance)
    #        inside groups:  
    #            DEVICE_IDV2_X is:   ((Cham devId) << 8 | index inside group )
    # ------------------------------------------------------------------------

#SCAN_NEXT_DEVID

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}

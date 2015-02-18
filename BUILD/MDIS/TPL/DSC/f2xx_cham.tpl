SCAN_BBIS_NAME_SCAN_BBIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING CHAMELEON_PCITBL
    _WIZ_MODEL = STRING SCAN_WIZ_MODEL
    _WIZ_BUSIF = STRING cpu,0

    # ------------------------------------------------------------------------
    #  		PCI configuration
    # ------------------------------------------------------------------------
    PCI_BUS_PATH = BINARY SCAN_PCIPATH_PRIM
    PCI_BUS_SLOT = U_INT32 SCAN_PCI_BUS_SLOT

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

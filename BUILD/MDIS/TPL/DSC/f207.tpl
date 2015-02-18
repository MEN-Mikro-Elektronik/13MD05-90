f207_SCAN_BBIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING PCI
    _WIZ_MODEL = STRING F207
    _WIZ_BUSIF = STRING cpu,0

    # ------------------------------------------------------------------------
    #  		PCI configuration
    # ------------------------------------------------------------------------
    PCI_BUS_PATH = BINARY SCAN_PCIPATH_PRIM,SCAN_PCIPATH_SEC
    DEVICE_SLOT_0 = U_INT32 0x4
    DEVICE_SLOT_1 = U_INT32 0x5
    DEVICE_SLOT_2 = U_INT32 0x6
    DEVICE_SLOT_3 = U_INT32 0x7

    # ------------------------------------------------------------------------
    #        Chameleon BBIS Device: 
    #            DEVICE_IDV2_X is:   ((Cham devId) << 8 | instance)
    #        inside groups:  
    #            DEVICE_IDV2_X is:   ((Cham devId) << 8 | index inside group )
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}

SCAN_MMODULE_NAMELCASE_SCAN_MMODULE_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING SCAN_MMODULE_NAME
    _WIZ_MODEL = STRING SCAN_WIZMODEL_NAME

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING SCAN_BBIS_NAMEUSCORESCAN_BBIS_INSTANCE
    DEVICE_SLOT = U_INT32 SCAN_DEV_SLOT

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}

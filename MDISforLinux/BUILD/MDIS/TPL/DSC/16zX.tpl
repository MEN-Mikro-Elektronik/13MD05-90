SCAN_IPCORE_TYPE_SCAN_MDIS_INSTANCE {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING SCAN_IPCORE_NAME
    _WIZ_MODEL = STRING SCAN_WIZMODEL_NAME

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING SCAN_BBIS_NAMEUSCORESCAN_BBIS_INSTANCE
    DEVICE_SLOT = U_INT32 SCAN_DEV_SLOT

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------
}

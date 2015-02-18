qspim_SCAN_MDIS_INSTANCE  {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING Z76_QSPIM
    _WIZ_MODEL = STRING 16Z076_QSPI

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING SCAN_BBIS_NAMEUSCORESCAN_BBIS_INSTANCE
    DEVICE_SLOT = U_INT32 SCAN_DEV_SLOT

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # enable interrupts
    # 0 := disabled
    # 1 := enabled
    IRQ_ENABLE = U_INT32 1

    # load PLD -- only on D201 --
    # 0 := don't load PLD
    # 1 := load PLD
    PLD_LOAD = U_INT32 0

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}

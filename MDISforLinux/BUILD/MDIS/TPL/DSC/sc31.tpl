cpu {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x0
    HW_TYPE = STRING SC31
    _WIZ_MODEL = STRING Bx51x
}
sc31_fpga {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING CHAMELEON_ISA
    _WIZ_MODEL = STRING CHAMELEON_ISA
    _WIZ_BUSIF = STRING cpu,0

    # ------------------------------------------------------------------------
    #        Chameleon BBIS Device: 
    #            DEVICE_IDV2_X is:   ((Cham devId) << 8 | instance)
    #        inside groups:  
    #            DEVICE_IDV2_X is:   ((Cham devId) << 8 | index inside group )
    # ------------------------------------------------------------------------
    DEVICE_IDV2_0 = U_INT32 0x2200
    DEVICE_IDV2_1 = U_INT32 0x2202
    DEVICE_IDV2_2 = U_INT32 0x2500
    DEVICE_IDV2_3 = U_INT32 0x1d00
    DEVICE_IDV2_4 = U_INT32 0x5200
    DEVICE_ADDR = U_INT32 0xf000e000
    DEVICE_ADDR_IO = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
gpio_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING Z17
    _WIZ_MODEL = STRING 16Z034_GPIO

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING sc31_fpga
    DEVICE_SLOT = U_INT32 0x0

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
gpio_2 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING Z17
    _WIZ_MODEL = STRING 16Z034_GPIO

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING sc31_fpga
    DEVICE_SLOT = U_INT32 0x1

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
gpio_3 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING Z17
    _WIZ_MODEL = STRING 16Z037_GPIO

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING sc31_fpga
    DEVICE_SLOT = U_INT32 0x2

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
can_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING Z15
    _WIZ_MODEL = STRING 16Z029_CAN

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING sc31_fpga
    DEVICE_SLOT = U_INT32 0x3

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # CAN controller input clock in Hz
    CANCLOCK = U_INT32 32000000

    # Minimum baud rate prescaler value. 2 for most cores, 1 in rare cases.
    MIN_BRP = U_INT32 2

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
impulse_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING Z82
    _WIZ_MODEL = STRING 16Z082_IMPULSE

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING sc31_fpga
    DEVICE_SLOT = U_INT32 0x4

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
smb2_bus0 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING SMB2
    _WIZ_MODEL = STRING SMBPCI_FCH
    _WIZ_BUSIF = STRING cpu,1

    # ------------------------------------------------------------------------
    #  		SMBus parameters
    # ------------------------------------------------------------------------
	SMB_BUSNBR = U_INT32 0x9
    SMBDEV_0 {
        NAME = STRING xm01bc_1
    }
    SMBDEV_1 {
        NAME = STRING smb2_2
    }

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
xm01bc_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING XM01BC
    _WIZ_MODEL = STRING XM01BC

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING smb2_bus0
    DEVICE_SLOT = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    #  		SMBus parameters
    # ------------------------------------------------------------------------

    # SMBus bus number
	SMB_BUSNBR = U_INT32 9	

    # SMBus address of XM1BC
    SMB_DEVADDR = U_INT32 0x9a

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
smb2_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING SMB2
    _WIZ_MODEL = STRING SMB2

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING smb2_bus0
    DEVICE_SLOT = U_INT32 0x1

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    #  		SMBus parameters
    # ------------------------------------------------------------------------

    # SMBus bus number
	SMB_BUSNBR = U_INT32 9	

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
# EOF


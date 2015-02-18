#************************** BBIS3 board descriptor **************************
#
#        Author: kp
#         $Date: 2006/08/18 12:01:47 $
#     $Revision: 1.4 $
#
#   Description: Metadescriptor for chameleon BBIS
#                Works for AD66 and EC4 carrier.
#
#****************************************************************************

CHAMEM04AD66 {
    #------------------------------------------------------------------------
    #   general parameters (don't modify)
    #------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   CHAMELEON   # driver

    #------------------------------------------------------------------------
    #   PCI specific parameters
    #------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0x00
    PCI_DEVICE_NUMBER   = U_INT32  0x1d

    DEVICE_ID_0         = U_INT32  0x0800   # 16Z029_CAN instance 0
    DEVICE_ID_1         = U_INT32  0x0900   # 16Z022_GPIO
    DEVICE_ID_2         = U_INT32  0x0801   # 16Z029_CAN instance 1
    DEVICE_ID_3         = U_INT32  0x0802   # 16Z029_CAN instance 2
    DEVICE_ID_4         = U_INT32  0x1900   # 16Z034_GPIO
}

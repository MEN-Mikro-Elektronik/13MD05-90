#************************** BBIS3 board descriptor **************************
#
#        Author: kp
#         $Date: 2007/06/18 15:23:37 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for chameleon BBIS
#                Works for EM09(CPU board) with EC06(carrier board).
#
#****************************************************************************

CHAMEM09EC06 {
    #------------------------------------------------------------------------
    #   general parameters (don't modify)
    #------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2           # descriptor type (2=board)
    HW_TYPE             = STRING   CHAMELEON   # driver

    #------------------------------------------------------------------------
    #   PCI specific parameters
    #------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0x00
    PCI_DEVICE_NUMBER   = U_INT32  0x13

    DEVICE_IDV2_0         = U_INT32  0x1d00   # 16Z029_CAN instance 0
    DEVICE_IDV2_1         = U_INT32  0x2200   # 16Z034_GPIO instance 0
    DEVICE_IDV2_2         = U_INT32  0x1d01   # 16Z029_CAN instance 1
    DEVICE_IDV2_3         = U_INT32  0x1d02   # 16Z029_CAN instance 2
    DEVICE_IDV2_4         = U_INT32  0x2201   # 16Z034_GPIO instance 1
    DEVICE_IDV2_5         = U_INT32  0x2202   # 16Z034_GPIO instance 2
    DEVICE_IDV2_6         = U_INT32  0x1d03   # 16Z029_CAN instance 3
    
   DEBUG_LEVEL_OSS     = U_INT32  0xC0008000   
   #DEBUG_LEVEL_DESC    = U_INT32  0xC0008000
   
}

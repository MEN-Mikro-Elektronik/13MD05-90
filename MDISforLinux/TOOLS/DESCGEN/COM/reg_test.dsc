#************************** MDIS4 test descriptor ***************************
#
#        Author: ds
#         $Date: 2000/07/04 09:21:27 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for test of descgen option -winnt to
#                generate a WinNT .reg-file
#
#****************************************************************************

# TEST: d1_min.dsc
D1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2         # descriptor type (2=board)
    HW_TYPE             = STRING   D1        # hardware name of device
}

# TEST: MY_DEVICE -> my_device, MY_DRIVER -> men_my_driver, MY_BOARD -> my_board
MY_DEVICE {
    DESC_TYPE       = U_INT32       1 
    HW_TYPE         = STRING        MY_DRIVER
    BOARD_NAME      = STRING        MY_BOARD
    DEVICE_SLOT     = U_INT32       0x3579abcd
}

# TEST: M -> men_m
M_1  {
    DESC_TYPE       = U_INT32       1
    HW_TYPE         = STRING        M		# driver  M
    BOARD_NAME      = STRING        D201_1
    DEVICE_SLOT     = U_INT32       0x0
}

# TEST: M000 ->men_ m0
M0_1  {
    DESC_TYPE       = U_INT32       1
    HW_TYPE         = STRING        M000	# driver M0
    BOARD_NAME      = STRING        D201_1
    DEVICE_SLOT     = U_INT32       0x0
}

# TEST: M001 -> men_m1
M1_1  {
    DESC_TYPE       = U_INT32       1
    HW_TYPE         = STRING        M001	# driver M1
    BOARD_NAME      = STRING        D201_1
    DEVICE_SLOT     = U_INT32       0x0
}

# TEST: M012 -> men_m12
M12_1  {
    DESC_TYPE       = U_INT32       1
    HW_TYPE         = STRING        M012	# driver M12
    BOARD_NAME      = STRING        D201_1
    DEVICE_SLOT     = U_INT32       0x0
}

# TEST: M123 -> men_m123
M123_1  {
    DESC_TYPE       = U_INT32       1
    HW_TYPE         = STRING        M123	# driver M123
    BOARD_NAME      = STRING        D201_1
    DEVICE_SLOT     = U_INT32       0x0
}

# TEST: value types and sub-pathes
MAIN_DIR {
    DESC_TYPE       = U_INT32       1
    HW_TYPE         = STRING        DRV
    U_INT32_VAL     = U_INT32       305441741     # -> 1234abcd
    STRING_VAL      = STRING        StRiNgTeSt    # -> StRiNgTeSt
    BINARY_VAL      = BINARY        0x01,0x02,0x30,0x40,0x55,0x66,0xaa,0xBB,0xcc

    SUB1_A {
             U_INT32_VAL     = U_INT32       305441741     # -> 1234abcd                 
             STRING_VAL      = STRING        StRiNgTeSt    # -> StRiNgTeSt               
             BINARY_VAL      = BINARY        0x01,0x02,0x30,0x40,0x55,0x66,0xaa,0xBB,0xcc
    }
    
    SUB1_B {
             U_INT32_VAL     = U_INT32       305441741     # -> 1234abcd                 
             STRING_VAL      = STRING        StRiNgTeSt    # -> StRiNgTeSt               
             BINARY_VAL      = BINARY        0x01,0x02,0x30,0x40,0x55,0x66,0xaa,0xBB,0xcc
	    
	    SUB2_B {
    	        U_INT32_VAL     = U_INT32       305441741     # -> 1234abcd                 
        	    STRING_VAL      = STRING        StRiNgTeSt    # -> StRiNgTeSt               
    	        BINARY_VAL      = BINARY        0x01,0x02,0x30,0x40,0x55,0x66,0xaa,0xBB,0xcc
        }
    }
    
    SUB1_C {                                                                                
             U_INT32_VAL     = U_INT32       305441741     # -> 1234abcd                    
             STRING_VAL      = STRING        StRiNgTeSt    # -> StRiNgTeSt                  
             BINARY_VAL      = BINARY        0x01,0x02,0x30,0x40,0x55,0x66,0xaa,0xBB,0xcc   
	                                                                                        
	    SUB2_C {                                                                            
    	        U_INT32_VAL     = U_INT32       305441741     # -> 1234abcd                 
        	    STRING_VAL      = STRING        StRiNgTeSt    # -> StRiNgTeSt               
    	        BINARY_VAL      = BINARY        0x01,0x02,0x30,0x40,0x55,0x66,0xaa,0xBB,0xcc
       
            SUB3_C {
                    U_INT32_VAL     = U_INT32       305441741     # -> 1234abcd                 
                    STRING_VAL      = STRING        StRiNgTeSt    # -> StRiNgTeSt               
                    BINARY_VAL      = BINARY        0x01,0x02,0x30,0x40,0x55,0x66,0xaa,0xBB,0xcc                                                                            
            }
        }    
    }
}

# TEST: binary alignment
BIN {
    DESC_TYPE       = U_INT32       1
    HW_TYPE         = STRING        DRV
	
	a		= BINARY   0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a

	a___5	= BINARY   0x11,0x0c,0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x0a,\
					   0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
					   0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
					   0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11,\
					   0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
					   0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11,\
					   0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11
	
	a___5___10	= BINARY   0x11,0x0c,0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x0a,\
						   0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
						   0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
						   0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11,\
						   0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
						   0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11,\
						   0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11
								   
	a___5___10___15___20	= BINARY   	0x11,0x0c,0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x0a,\
						   				0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
						   				0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
						   				0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11,\
						   				0x11,0x0c,0x08,0x0a,0x11,0x0c,0x0a,0x11,0x0c,\
						   				0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11,\
						   				0x0a,0x11,0x0c,0x0a,0x11,0x0c,0x08,0x0a,0x11
}

# TEST: m27_max.dsc
M27_1  {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       1         # descriptor type (1=device)
    HW_TYPE         = STRING        M027      # hardware name of device

	#------------------------------------------------------------------------
	#	base board configuration
	#------------------------------------------------------------------------
    BOARD_NAME      = STRING        D201_1    # device name of baseboard
    DEVICE_SLOT     = U_INT32       0         # used slot on baseboard (0..n)

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#   this keys have only effect on debug drivers
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32   0xc0008007    # LL driver
    DEBUG_LEVEL_MK      = U_INT32   0xc0008007    # MDIS kernel
    DEBUG_LEVEL_OSS     = U_INT32   0xc0008002    # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32   0xc0008002    # DESC calls

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
    ID_CHECK            = U_INT32   1             # check module ID prom
}

# TEST: m27_min.dsc
M27_2  {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       1         # descriptor type (1=device)
    HW_TYPE         = STRING        M027      # hardware name of device

	#------------------------------------------------------------------------
	#	base board configuration
	#------------------------------------------------------------------------
    BOARD_NAME      = STRING        D201_1    # device name of baseboard
    DEVICE_SLOT     = U_INT32       1         # used slot on baseboard (0..n)
}

# TEST: d201_max.dsc
D201_1 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   D201		# hardware name of device

	#------------------------------------------------------------------------
	#	PCI configuration
	#------------------------------------------------------------------------

	# define device IDs of bridges to CompactPCI backplane
	# see D201/DOC/pcibuspath.txt for list

	PCI_BUS_PATH		= BINARY   0x08		# device IDs of bridges to 
											# compact PCI bus
    PCI_BUS_SLOT        = U_INT32  3		# Compact PCI bus slot 1=CPU
    PCI_CHECK_LOCATION  = U_INT32  0		# don't check geographic location
    #PCI_BUS_NUMBER     = U_INT32  1		# optional overwrites PCI_BUS_PATH
    #PCI_DEVICE_ID      = U_INT32 10		# optional overwrites PCI_BUS_SLOT

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#   this keys have only effect on debug drivers
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32  0xc0008000	# BBIS driver
    DEBUG_LEVEL_BK      = U_INT32  0xc0008000	# BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32  0xc0008000	# OSS calls
    DEBUG_LEVEL_DESC    = U_INT32  0xc0008000	# DESC calls

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
    ID_CHECK            = U_INT32  1	# check board ID prom
	PLD_LOAD			= U_INT32  1	# load PLD
	NONE_A24			= U_INT32  0	# request no A24 addr space (requires
										#  board with special eeprom data) 
}

# TEST: d201_min.dsc
D201_2 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2		# descriptor type (2=board)
    HW_TYPE             = STRING   D201		# hardware name of device

	#------------------------------------------------------------------------
	#	PCI configuration
	#------------------------------------------------------------------------
    PCI_BUS_NUMBER     = U_INT32  1		# optional overwrites PCI_BUS_PATH
    PCI_DEVICE_ID      = U_INT32 10		# optional overwrites PCI_BUS_SLOT
}
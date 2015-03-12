#!/bin/bash
############################################################################
#
# scan_system.sh   script to generate an automatic MDIS configuration
#                  when doing a selfhosted project
#
#                  The script checks which CPU we are on. The SMB2 drivers
#                  are always added to the CPU to provide BMC and watchdog
#                  drivers.
#
#                  Some simplifications were made to make generating the
#                  system.dsc easier:
#                  - BBIS instances are numbered in straight ascending order.
#                    E.g. if 2xF208, 2xF210 are present they are not labeled
#                    like by MDIS wizard: f208_1, f208_2, f210_1, f210_2
#                    but instead: f208_1,f208_2,f210_3,f210_4.
#                  - the same is done with MDIS devices.
#
# parameters:
# $1    MEN_LIN_DIR   path to MDIS installation, passed by mdiswizard
#                     (if script is run manually it must be passed)
# $2    verbosity     if "1" then additional debug info is dumped
# $3    PCI drytest   if passed as alternative PCI devices temp file the
#                     default file /tmp/men_pci_devs is not written. Used to
#                     test system.dsc generation with predefined test data.
############################################################################
# copyright (c) 2113 MEN Mikro Elektronik GmbH Nuremberg
############################################################################

##########
# DEFINES

# proc entry to scan pci devs
PROC_PCI_DEV=/proc/bus/pci/devices
# where the linux kernel sources/headers are expected
LIN_SRC_DIR=/usr/src/linux
# where are the SMBus controller drivers located ?
MOD_DIR=/lib/modules/`uname -r`

# currently detected CPU boards. ADD NEW BOARDS HERE!
# also take care for special (native) driver adding etc.
CPU_KNOWN_BOARDS="SC24 F011 F11S F14- F014 F15- F015 F17- F017 F075 F19P F19C F019 F21P F21C F021 XM01 MM01 G20- G22-"

# which SMB adresses to scan for CPU ID eeproms
ID_EEPROM_ADRESSES="0x57 0x55"
# the generated example system dsc
DSC_FILE=system.dsc  #.example
# the generated example Makefile
MAKE_FILE=Makefile   #.example
# the generated example Makefile
TMP_MAKE_FILE=/tmp/Makefile   #.example
# temporary BBIS section file to add BBIS data first, then DEVICE_IDV2_x data
TMP_BBIS_DSC=/tmp/bbis.dsc.tmp
# temporary PCI device file to iterate through
TMP_PCIDEVS=/tmp/men_pci_devs.tmp
# temporary chameleon table file to scan IPcore IDs
TMP_CHAM_TBL=/tmp/men_cham_tbl.tmp
# fpga_load to use (32/64 bit)
# (Mind: without ia32 lib 32bit programs cant run under 64bit)
FPGA_LOAD=fpga_load_x86-32
# debugging/verbosity: 0=none, 1=verbose 2=verbose+function arguments
VERBOSE_PRINT=0
# run drytest with test PCI device list ?
PCI_DRYTEST=""

# really writing pcitree/temp. cham table or use simulation data ?
# 0 = normal operation, scan and write PCI devices
# 1 = use predefined /tmp/men_pci_devs.tmp file to run tests
SCAN_SIM=0

# for simulation add here which SMBus and CPU shall be assumed
SCAN_SIM_SMBUS=0
SCAN_SIM_CPU="MM01"

#########
# GLOBALS

# MDIS instance label
G_mdisInstanceCount=1

# CPU on which we run, detected in ID EEprom on SMbus <G_SmBusNumber>
G_cpu=""
G_SmBusNumber=0
# PCI_BUS_xxx entries for all MDIS devs within one chameleon BBIS device.
G_bus_slot_count=0
G_primPciPath=0

# lists for driver/library/program mak files
G_makefileLlDriver=""
G_makefileLlTool=""
G_makefileBbisDriver=""
G_makefileNatDriver=""
G_makefileUsrLibs=""
# DEVICE_IDV2_xx list
G_deviceIdV2=""

############################################################################
# verbose debug outputs if VERBOSE_PRINT is 1 or 2
#
# parameters: $1  text to print
#
function debug_print {
    if [ "$VERBOSE_PRINT" == "1" ]; then
        echo $1
    elif [ "$VERBOSE_PRINT" == "2" ]; then
        echo $1
    fi
}

function debug_args {
    if [ "$VERBOSE_PRINT" == "2" ]; then
        echo $1
    fi
}

############################################################################
# usage text
#
function usage {
    echo " scan_system <MEN_LIN_DIR> [verbosity] [PCI simulation file]"
    echo
    echo " used together with MDIS wizard or to be called standalone."
}

############################################################################
# determine if an ID EEprom content is really a MEN CPU. A check if the
# characters are ASCII isnt enough because from DDC ports or other bitbang
# interfaces any char salad might be returned. Save approach: check
# explicitely for our CPU names
#
# parameters: $1     found string
#
function check_if_men_cpu {
	detectedCpu=$1
	for i in $CPU_KNOWN_BOARDS;
	do
		debug_print "\$i = $i  detectedCpu = $detectedCpu"
		if [ "$i" == "$detectedCpu" ] ; then
			G_cpu=$i;
			echo "found matching CPU: $i"
			break
		else
			G_cpu="";
		fi
	done
}


############################################################################
# scan for production data EEPROM to find out who we are
#
# parameters: N/A
#
function detect_board_id {
    echo "Scanning for MEN ID EEProm."
    # these can be safely assumed to exist on any recent distribution.
    modprobe i2c-dev   # for i2cdetect
    modprobe i2c-i801  # F1x SMB controllers

    # these might not exist on all Distros
    if [ -e "$MOD_DIR/kernel/drivers/i2c/busses/i2c-isch.ko" ]; then
        modprobe i2c-isch
    fi
    if [ -e "$MOD_DIR/kernel/drivers/i2c/busses/i2c-piix4.ko" ]; then	
        modprobe i2c-piix4 # SC24 SMB controller. Use 13SC24-90 modified driver!
    fi

    G_cpu=""
    for adrs in $ID_EEPROM_ADRESSES; do
	for dev in /dev/i2c-*; do
	    smbus=`echo $dev | awk '{print substr($1,10,2)}'`
            # scan for ID EEprom 0x55 at every present bus
	    echo "scan SMBus: bus $smbus SMB addr $adrs"
	    RES=`i2cdump -r 9-12 -y $smbus $adrs b | tail -n 1 | awk '{print $6}'`

	    check_if_men_cpu $RES
	    
	    if [[ $SCAN_SIM == 1 && $smbus == $SCAN_SIM_SMBUS ]]; then
		G_cpu=$SCAN_SIM_CPU
		echo "- simulating CPU $G_cpu on bus $smbus"
		break
	    fi
	    # board found ? break inner loop
	    if [ "$G_cpu" != "" ]; then
		break
	    fi
	done;
	
	# board found ? break outer loop
	if [ "$G_cpu" != "" ]; then
	    break
	fi
    done

    if [ "$G_cpu" == "" ]; then
        echo "*** could not find CPU type or no ID EEprom present!"
	exit 1
    fi
}


############################################################################
# Map SC24 FPGA (in case old BIOS is present)
# the IRQ settings are switched on loading BBIS driver
# parameters: N/A
function map_sc24_fpga {
    echo "SC24 detected, mapping FPGA."
    # PCI location is fixed (inside AMD chipset)
    setpci -s 00:14.3 0x48.B=0x27
    setpci -s 00:14.3 0x60.B=0x00
    setpci -s 00:14.3 0x61.B=0xe0
    setpci -s 00:14.3 0x62.B=0x01
    setpci -s 00:14.3 0x63.B=0xe0
}


############################################################################
# write the smb section for the cpu in the system.dsc
#
# parameters:
# $1    DSC template directory
# $2    SM Bus number
# $3    SMB driver, e.g. SMBPCI_ICH
# $4    SMB bus IF nr "cpu,0' or "cpu,1" etc
#
function create_entry_dsc_smb_type {
    echo "Writing CPU SMB BBIS section."
    #echo " _WIZ_MODEL = $3, SM Bus nr. = $2  SM Bus IF nr. = $4 "
    cat $1/smb.tpl | sed "s/SCAN_SMBDRV/$3/g; s/SCAN_SMBNR/$2/g; s/SCAN_SMBUSIF/$4/g" >> $DSC_FILE
}

############################################################################
# write the smb section for the cpu in the system.dsc
#
# parameters:
# $1    DSC template directory
# $2    SCAN_MDIS_INSTANCE nr of found PP04
# $3    SCAN_BBIS_INSTANCE carrier boards instance
#
function create_entry_dsc_pp04 {
    echo "Writing PP04 MDIS driver section."
    #echo " _WIZ_MODEL = $3, SM Bus nr. = $2  SM Bus IF nr. = $4 "
    cat $1/pp04.tpl | sed "s/SCAN_MDIS_INSTANCE/$2/g;s/SCAN_BBIS_INSTANCE/$3/g;" >> $DSC_FILE
}


############################################################################
# write a SMB driver section
#
# parameters:
# $1    DSC template directory
# $2    SM Bus number
# $3    MDIS device name, e.g. xm01bc_1 or smb2_2
# $4    HW_TYPE, e.g. XM01BC (not always the capitalized MDIS dev. type
# $5    WIZ_MODEL
function create_entry_dsc_smb_drv {
    echo "creating CPU SMB driver section: _WIZ_MODEL = $3, SM Bus nr. = $2"

    cat $1/smb_drv.tpl | \
        sed "s/SCAN_DEVNAME/$3/g;s/SCAN_HWTYPE/$4/g;s/SCAN_WIZMODEL/$5/g;s/SCAN_SMBNR/$2/g" >> $DSC_FILE
}


############################################################################
# write the cpu section of the system.dsc file
#
# parameters:
# $1    DSC template directory
# $2    _WIZ_MODEL
#
function create_entry_dsc_cpu_type {
    echo "Writing system.dsc cpu section: _WIZ_MODEL = $2"
    cat $1/cpu.tpl | sed "s/SCAN_CPU/$2/g" >> $DSC_FILE
}


############################################################################
# write a chameleon BBIS section
#
# parameters:
# $1 DSC template directory
# $2 BBIS instance number (subst. SCAN_BBIS_INSTANCE )
# $3 _WIZ_MODEL, e.g. F210 (subst. SCAN_WIZ_MODEL)
# $4 PCI_BUS primary path (subst. SCAN_PCIPATH_PRIM )
# $5 PCI devnr. (subst. SCAN_PCI_BUS_SLOT)
# $6 DEVICE_IDV2_x array of found IP cores inside this BBIS device
#
# convert PCI dev.nr. to pci slot on standard backplanes:
# slot |  dev.nr
#   2  |  0x0f
#   3  |  0x0e
#   4  |  0x0d
#   5  |  0x0c
#   6  |  0x0b
#   ... -> slot = 17-devnr [slot=2-8]
function create_entry_dsc_bbis_cham {
    echo "create chameleon BBIS device..."
    debug_args " \$1 = $1  \$2 = $2  \$3 = $3  \$4 = $4  \$5 = $5  \$6 = $6"
        # BBIS driver name = WIZ_MODEL in lower letters
    pci_devnr=$5
    device_id=$6

    bbis_name=`echo $3 | awk '{print tolower($1)}'`
        # calculate bus_slot from given PCI devnr. on standard backplanes
    pci_bus_slot=`expr 17 - $pci_devnr`

        # cat template to temp file that gets DEVICE_IDV2_xx added after IPcore scan

	# unfortunately some FPGA BBIS models dont match the IC file name. E.g. on MM1
    # the IC filename starts with "MM01-IC..." but modelname is just 'fpga'
	if [ "$bbis_name" == "mm01" ]; then
		tplname=mm1_cham.tpl
	else
		tplname=f2xx_cham.tpl
	fi

	# TODO generate the long filter commands dynamically..
    cat $1/$tplname | sed "s/SCAN_BBIS_NAME/$bbis_name/g;s/SCAN_BBIS_INSTANCE/$2/g;s/SCAN_WIZ_MODEL/$3/g;s/SCAN_PCIPATH_PRIM/$4/g;s/SCAN_PCI_BUS_SLOT/$pci_bus_slot/g" > $TMP_BBIS_DSC

}

############################################################################
# write all MDIS device sections for the detected BBIS entry (cham device)
#
# The scan of IPcore entries per chameleon device is done 2 times, first to
# just collect DEVICE_IDV2 data to write the BBIS section in system dsc,
# then also to generate every MDIS device section. The BBIS section
#
# parameters:
# $1  DSC template directory
# $2  FPGA file name, e.g. 'F210'
# $3  BBIS board instance number ( x as in f210_x)
# $4  0: just get DEVICE_IDV2 data (to complete BBIS dsc entry)
#     1: also write MDIS dsc data  (to add device sections below BBIS entry)
function scan_cham_table {
    debug_args "\$1 = $1 \$2 = $2 \$3 = $3 \$4 = $4 "
    fpga_file=$2
    bbis_instance=$3
    do_parse=0
    write_mdis_dsc=$4

    while read devline; do
		if [ "$do_parse" == "1" ]; then
			ipcore=`echo $devline | awk '{print $3}' | awk '{print substr($1,1,6)}'`
			devid=`echo $devline | awk '{print $2}' | awk '{print substr($1,5,2)}'`
			inst_raw=`echo $devline | awk '{print $5}'`
			instance=`printf "%02x" $inst_raw`
			is_cham_dev=1
			isNativeDriver=0
			case $ipcore in
				16Z025) # native drivers: add also men_lx_chameleon driver!
					G_makefileNatDriver+=" DRIVERS/FRODO/driver.mak DRIVERS/CHAMELEON/driver.mak"
					isNativeDriver=1
					;;
				16Z057)
					G_makefileNatDriver+=" DRIVERS/FRODO/driver.mak DRIVERS/CHAMELEON/driver.mak"
					isNativeDriver=1
					;;
				16Z125)
					G_makefileNatDriver+=" DRIVERS/FRODO/driver.mak DRIVERS/CHAMELEON/driver.mak"
					isNativeDriver=1
					;;
				16Z001)
					G_makefileNatDriver+=" DRIVERS/Z001_SMB/driver.mak DRIVERS/CHAMELEON/driver.mak"
					isNativeDriver=1
					;;
				16Z087)
					G_makefileNatDriver+=" DRIVERS/ETH_16Z077/DRIVER/driver.mak DRIVERS/CHAMELEON/driver.mak"
					isNativeDriver=1
					;;
				16Z077)
					G_makefileNatDriver+=" DRIVERS/ETH_16Z077/DRIVER/driver.mak DRIVERS/CHAMELEON/driver.mak"
					isNativeDriver=1
					;;
				16Z034) # GPIO
					;;
				16Z037) # GPIO serial
					;;
				16Z029) # CAN
					;;
				16Z076) # QSPIM
					;;
				*)  # skip other IP cores not handled above
					debug_print "omitting IP core $ipcore"
					is_cham_dev=0
					;;
			esac

			if [ "$is_cham_dev" == "1" ] ; then
				G_deviceIdV2+=" 0x$devid$instance"
				if [ $write_mdis_dsc == "1" ]; then
					bbis_name=`echo $fpga_file | awk '{print tolower($1)}'`
	            # special case MM1: MM1 FPGA BBIS model is not 'mm01_x', just 'fpga'. So
				# naming the BBIS section 'mm01_x' would cause mdiswiz to refuse loading devices.

				# TODO generate the long sed filter commands dynamically..
				if [ "$bbis_name" == "mm01" ]; then
					cat $1/$ipcore.tpl | sed "s/SCAN_MDIS_INSTANCE/$G_mdisInstanceCount/g;s/SCAN_BBIS_NAME/fpga/g;s/USCORESCAN_BBIS_INSTANCE//g;s/SCAN_DEV_SLOT/$G_bus_slot_count/g;" >> $DSC_FILE
				else
				      cat $1/$ipcore.tpl | sed "s/SCAN_MDIS_INSTANCE/$G_mdisInstanceCount/g;s/SCAN_BBIS_NAME/$bbis_name/g;s/USCORESCAN_BBIS_INSTANCE/_$bbis_instance/g;s/SCAN_DEV_SLOT/$G_bus_slot_count/g;" >> $DSC_FILE
				fi

			       G_bus_slot_count=`expr $G_bus_slot_count + 1`
				G_mdisInstanceCount=`expr $G_mdisInstanceCount + 1`

                    # collect non native drivers .mak macro definitions from xml files
					if [ "$isNativeDriver" == "0"  ]; then
						xmlfile=`fgrep $ipcore $MEN_LIN_DIR/PACKAGE_DESC/* | head -n 1 | awk '{print $1}' | sed "s/://"`
                        # the name is not always "driver.mak" e.g. CAN: driver_boromir.mak.

						lldrv=`fgrep ".mak" $xmlfile | grep DRIVER | sed "s/<makefilepath>//;s/<\/makefilepath>//"`
						G_makefileLlDriver+=" "$lldrv

						lltool=`fgrep "program.mak" $xmlfile | sed "s/<makefilepath>//;s/<\/makefilepath>//"`
						G_makefileLlTool+=" "$lltool
						usrlib=`fgrep "library.mak" $xmlfile | sed "s/<makefilepath>//;s/<\/makefilepath>//"`
						G_makefileUsrLibs+=" "$usrlib
					fi
				fi
			fi
		fi # do_parse

        # skip to begin of IP cores table
		delimiter=`echo $devline | awk '{print $1}'`
		if [ $delimiter == "---" ]; then
            do_parse=1
		fi

    done < $TMP_CHAM_TBL
}


############################################################################
# check if a PCI device is a MEN chameleon device
#
# if a chameleon device is found ( valid chameleon table could be dumped)
# the table is parsed, BBIS name and wizard model extracted.
# After scanning the IP cores the DEVICE_IDV2_x = U_INT32 0x.... entry is
# added.
#
# example:
#  8 0x0022 16Z034_GPIO      0    0   0   9  63   0 0x00000a00 0x91100a00
#  9 0x0022 16Z034_GPIO      0    1   0   9  63   0 0x00000b00 0x91100b00
# 10 0x0022 16Z034_GPIO      0    2   0   9  63   0 0x00000c00 0x91100c00
#  is converted into:
#  DEVICE_IDV2_0 = U_INT32 0x2200
#  DEVICE_IDV2_1 = U_INT32 0x2201
#  DEVICE_IDV2_2 = U_INT32 0x2202
#
# parameters:
# $1  MEN linux dir (/opt/menlinux)
# $2  PCI vendor
# $3  PCI device id
# $4  PCI device nr.
# $5  PCI subvendor
# $6  instance count
# $7  pci primary path
function check_for_cham_devs {
    debug_args "PCIvd.$2 PCIdev. $3 PCIdevnr $4  PCIsubv $5 instance $6  PCIprimPath $7"
    cham_file=""
    device_id_count=0

    # check if we find a chameleon table
    $1/BIN/$FPGA_LOAD $2 $3 $5 0 -t > /dev/null
    if [[ $? == "0" || "$SCAN_SIM" == "1" ]]; then
        # store temp. cham table ( '>' to start new file!)
		if [ "$SCAN_SIM" == "0" ]; then
            $1/BIN/$FPGA_LOAD $2 $3 $5 0 -t > $TMP_CHAM_TBL
		fi

        # extract board name from 'file='
        cham_file=`fgrep "file=" $TMP_CHAM_TBL | awk '{print substr($1,6,4)}'`
        debug_print "cham_file = $cham_file (= _WIZ_MODEL)"
        inst_count=$6

        G_bus_slot_count=0 # in next BBIS carrier PCI_BUS_SLOT starts at 0 again
        G_deviceIdV2=""    # clear list of IDs for next BBIS

	# gather DEVICE_IDV2 data for this BBIS first
		scan_cham_table $DSC_TPL_DIR $cham_file $inst_count 0

        # Now add the found device IDs to temporary BBIS des file
        create_entry_dsc_bbis_cham $DSC_TPL_DIR $6 $cham_file $7 $4
		for id in $G_deviceIdV2; do
        # format data into a DEVICE_IDV2 entry and add same scan tag in next line
            idv2line="    DEVICE_IDV2_$device_id_count = U_INT32 $id\n#SCAN_NEXT_DEVID"
            sed -i.bak "s/#SCAN_NEXT_DEVID/$idv2line/g" $TMP_BBIS_DSC
            device_id_count=`expr $device_id_count + 1`
		done
        # BBIS dsc section now complete. append it to $DSC_FILE
		cat $TMP_BBIS_DSC >> $DSC_FILE

        # all F2xx BBIS are supported by chameleon PCI table driver..
        G_makefileBbisDriver+=" CHAMELEON/DRIVER/COM/driver_pcitbl.mak"

        # create MDIS dev entries under this BBIS device
		G_bus_slot_count=0
        G_deviceIdV2=""    # clear list of IDs for next BBIS
		scan_cham_table $DSC_TPL_DIR $cham_file $inst_count 1
    fi

}


############################################################################
# create a f223_x BBIS section plus 1 gpio driver on it.
# The assigned gpio driver instance number is equal to
# the BBIS instance (instance of found F223) so on f223_1
# theres pi7c9_gpio_1, on f223_2 theres pi7c9_gpio_2 etc.
#
# parameters:
# $1  DSC template directory
# $2  instance number (subst. SCAN_BBIS_INSTANCE tag)
# $3  PCI_BUS primary path (subst. SCAN_PCIPATH_PRIM tag)
# $4  PCI_BUS sec. path (subst. SCAN_PCIPATH_SEC tag)
function create_entry_dsc_f223 {
    echo "Writing f223_$2 section to system.dsc "
    debug_args " \$1 = $1   \$2 = $2    \$3 = $3    \$4 = $4  "
    cat $1/f223.tpl  | sed "s/SCAN_BBIS_INSTANCE/$2/g;"\
"s/SCAN_MDIS_INSTANCE/$2/g;s/SCAN_PCIPATH_PRIM/$3/g;"\
"s/SCAN_PCIPATH_SEC/$4/g" >> $DSC_FILE

}

############################################################################
# create a f207_x BBIS section.
# F207 section is added when a PP04 was found.
#
# parameters:
# $1  DSC template directory
# $2  instance number (subst. SCAN_BBIS_INSTANCE tag)
# $3  PCI_BUS primary path (subst. SCAN_PCIPATH_PRIM tag)
# $4  PCI_BUS sec. path (subst. SCAN_PCIPATH_SEC tag)
function create_entry_dsc_f207 {
    echo "Writing f207_$2 section to system.dsc "
    debug_args " \$1 = $1 \$2 = $2  \$3 = $3  \$4 = $4 "
    cat $1/f207.tpl  | sed "s/SCAN_BBIS_INSTANCE/$2/g;s/SCAN_PCIPATH_PRIM/$3/g;s/SCAN_PCIPATH_SEC/$4/g" >> $DSC_FILE
}


############################################################################
# add the xm01bc LL driver, tool to LL driver and LL tool list. Some
# F-Cards have a BMC, some dont (F21P). bundle necessary mak file adding here
#
function add_xm01bc_support {
	G_makefileLlTool+=" XM01BC/TOOLS/XM01BC_CTRL/COM/program.mak"
	G_makefileLlDriver+=" XM01BC/DRIVER/COM/driver.mak"
}

############################################################################
# add the xm01bc LL driver, tool to LL driver and LL tool list. Some
# F-Cards have a BMC, some dont (F21P). bundle necessary mak file adding here
#
function add_f14bc_support {
	G_makefileLlTool+=" F14BC/TOOLS/F14BC_CTRL/COM/program.mak"
	G_makefileLlDriver+=" F14BC/DRIVER/COM/driver.mak"
}

############################################################################
# add the xm01bc LL driver, tool to LL driver and LL tool list. Some
# F-Cards have a BMC, some dont (F21P). bundle necessary mak file adding here
#
function add_z001_io_support {
	G_makefileNatDriver+=" DRIVERS/Z001_SMB/driver_g2x.mak DRIVERS/CHAMELEON/driver.mak"
}


############################################################################
# scan the generated PCI list tmp file for known devices. read the PCI list
# linewise to find multiple cards etc. Parse the generated temporary file for
# F2xx cards.
# For F223 the search is performed in a state machine like manner. A F223 is
# identified by a Pericom PCI bridge with 4 USB bridges directly after it.
# PP04 cards are identified by a TI cPCI bride + PP04 FPGA directly after it.
#
# parameters:
# $1    WIZ_BUS_PATH_PRIM primary PCI bus path of CPU
#
function scan_for_pci_devs {

    state_check_f223=0
    state_check_pp04=0
    count_usb_devs=0
	count_pp04_devs=0
    count_instance_f223=0 # handled separately
    count_instance_f207=0
    count_instance_f2xx=0 # any other F2xx cham. carrier
    bus_path_prim=$1
    bus_path_sec=0
    bus_path_sec_f223=0

    while read line; do
    # Nr.|bus|dev|fun| Ven ID | Dev ID | SubVen ID |
    #  25   5  15   0  0x12d8   0xe110    0x0000
        listnr=`echo $line | awk '{print $1}'`
        pcibus=`echo $line | awk '{print $2}'`
        pcidevnr=`echo $line | awk '{print $3}'`
        pcivend=`echo $line | awk '{print $5}'`
        pcidevid=`echo $line | awk '{print $6}'`
        pcisubvend=`echo $line | awk '{print $7}'`
        debug_print "Vendor: $pcivend Device: $pcidevid PCI bus: $pcibus PCI devnr. $pcidevnr "

        ###################
        # state actions
        if [ "$state_check_f223" == "1" ]; then
            # previous line was a Pericom bridge. Are now 4 subsequent USB bridges following?
            if [ "$pcivend" == "0x12d8" ] && [ "$pcidevid" == "0x400a" ]; then
                count_usb_devs=`expr $count_usb_devs + 1`
            else # other device showed up in between -> its no F223.
                state_check_f223=0
                count_usb_devs=0
            fi

            if [ "$count_usb_devs" == "4" ] && [ "$pcidevid" == "0x400a" ]; then
                count_instance_f223=`expr $count_instance_f223 + 1`
                echo " -> F223 nr. $count_instance_f223 found, adding to system descriptor"
                count_usb_devs=0
                state_check_f223=0
                create_entry_dsc_f223 $DSC_TPL_DIR $count_instance_f223 \
                    $bus_path_prim $bus_path_sec_f223
            fi
        fi

		# does a PP04 show up ?
        if [ "$state_check_pp04" == "1" ]; then
            # previous line was a F207. Do we find a PP04 now? PP04 has V0 cham table.
            if [ "$pcivend"=="0x1172" ] && [ "$pcidevid"=="0x000c" ] && [ "$pcisubvend"=="0x4d45" ]; then
				echo "Found PP04 MVB card. adding dsc entries for F207 and PP04."
				count_pp04_devs=`expr $count_pp04_devs + 1`
                create_entry_dsc_f207 $DSC_TPL_DIR $count_instance_f207 $bus_path_prim $bus_path_sec
                create_entry_dsc_pp04 $DSC_TPL_DIR $count_pp04_devs $count_instance_f207
	        else # other device showed up in between -> its no F223.
                state_check_pp04=0
            fi
        fi


        ############################
        # state events

		# check if a F223 starts here
        if [ "$pcivend" == "0x12d8" ] && [ "$pcidevid" == "0xe110" ]; then
            echo "Found Pericom PCI bridge. Keep looking if F223 appears"
                        # store PCI devnr. in case its a F223
            bus_path_sec_f223=$pcidevnr
            state_check_f223=1
        fi

		# check if TI Bridge (=F207), if so check if a PP04 follows next
        if [ "$pcivend" == "0x104c" ] && [ "$pcidevid" == "0xac28" ]; then
            echo "Found TI2050 PCI bridge, possibly F207. Keep looking if PP04 appears."
                       # store PCI devnr. in case its a F223
			bus_path_sec=$pcidevnr
			count_instance_f207=`expr $count_instance_f207 + 1`
            state_check_pp04=1
        fi

        # any other F2xx carrier ?
        if [ "$pcivend" == "0x1172" ] && [ "$pcidevid" == "0x4d45" ] || [ "$pcivend" == "0x1a88" ]; then
            count_instance_f2xx=`expr $count_instance_f2xx + 1`
            echo "Found possible MEN chameleon device(s), checking."
            check_for_cham_devs $MEN_LIN_DIR \
                $pcivend $pcidevid $pcidevnr $pcisubvend \
                $count_instance_f2xx $bus_path_prim
        fi

    done <  $TMP_PCIDEVS
}


############################################################################
# create MDIS Makefile from collected driver data
#
#   Makefile.tpl is completed by replacing following tags
#   with real driver/library/program.mak references:
#
#   #SCAN_LIN_KERNEL_DIR   -> LIN_KERNEL_DIR
#   #SCAN_NEXT_LL_DRIVER   -> ALL_LL_DRIVERS
#   #SCAN_NEXT_BB_DRIVER   -> ALL_BB_DRIVERS
#   #SCAN_NEXT_USR_LIB     -> ALL_USR_LIBS
#   #SCAN_NEXT_LL_TOOL     -> ALL_LL_TOOLS
#   #SCAN_NEXT_NAT_DRIVER  -> ALL_NATIVE_DRIVERS
#
#   Also check .mak files to really exist in the MEN_LIN_DIR folder.
#   The xml files contain also MEN internal test tools
#
function create_makefile {
    echo "creating Makefile..."

    # start with the template
    cat $DSC_TPL_DIR/Makefile.tpl > $TMP_MAKE_FILE

    # write linux kernel directory
    kern_dir=`echo "$LIN_SRC_DIR" | sed "s/\//@/g"`
    sed -i.bak "s/SCAN_LIN_KERNEL_DIR/$kern_dir/g" $TMP_MAKE_FILE

	# insert all collected BBIS drivers into Makefile
    for i in $G_makefileBbisDriver; do
	debug_print "checking bbis driver: $i"
        if [ -f $MEN_LIN_DIR/DRIVERS/BBIS/$i ]; then
            debug_print "bbis driver: $i"
            subs=`echo "     $i" | sed "s/\//@/g"`
            sed -i.bak "s/#SCAN_NEXT_BB_DRIVER/$subs\n#SCAN_NEXT_BB_DRIVER/g" $TMP_MAKE_FILE
        else
			debug_print "BB driver '$i' not found in MDIS tree, skipping."
        fi
    done

	# insert all collected LL drivers into Makefile
    for i in $G_makefileLlDriver; do
	debug_print "checking LL driver: $i"
        if [ -f $MEN_LIN_DIR/DRIVERS/MDIS_LL/$i ]; then
            debug_print "ll driver: $i "
            subs=`echo "     $i" | sed "s/\//@/g"`
            sed -i.bak "s/#SCAN_NEXT_LL_DRIVER/$subs\n#SCAN_NEXT_LL_DRIVER/g" $TMP_MAKE_FILE
        else
            debug_print "skipping LL driver '$i'"
        fi
    done

	# insert all collected LL Tools into Makefile
    for i in $G_makefileLlTool; do
        if [ -f $MEN_LIN_DIR/DRIVERS/MDIS_LL/$i ]; then
            debug_print "ll tool: $i"
            subs=`echo "     $i" | sed "s/\//@/g"`
            sed -i.bak "s/#SCAN_NEXT_LL_TOOL/$subs\n#SCAN_NEXT_LL_TOOL/g" $TMP_MAKE_FILE
        else
            debug_print "skipping LL tool '$i'"
        fi
    done

	# insert all collected native drivers into Makefile
    for i in $G_makefileNatDriver; do
        if [ -f $MEN_LIN_DIR/$i ]; then
            debug_print "native driver: $i"
            subs=`echo "     $i" | sed "s/\//@/g"`
            sed -i.bak "s/#SCAN_NEXT_NAT_DRIVER/$subs\n#SCAN_NEXT_NAT_DRIVER/g" $TMP_MAKE_FILE
        else
            echo "native driver '$i' not found in MDIS tree, skipping."
        fi

    done

    for i in $G_makefileUsrLibs; do
        if [ -f $MEN_LIN_DIR/LIBSRC/$i ]; then
            debug_print "usr lib: $i"
            subs=`echo "     $i" | sed "s/\//@/g"`
            sed -i.bak "s/#SCAN_NEXT_USR_LIB/$subs\n#SCAN_NEXT_USR_LIB/g" $TMP_MAKE_FILE
        else
            echo "user lib '$usrlib' not found in MDIS tree, skipping."
        fi
    done


    # add '\' behind every .mak and replace @ with '/'
    sed -i.bak "s/@/\//g;s/\.mak/\.mak\\\/g" $TMP_MAKE_FILE

    # remove all doublette mak files added above
    awk '!seen[$0]++' $TMP_MAKE_FILE > $MAKE_FILE

    # finally replace ##REPLNEWLINExxx tags with LF (after
    # removing all doublette line no linefeeds would be left
    # making the Makefile looking ugly...)

    sed -i.bak "s/##REPLNEWLINE.../\n/g" $MAKE_FILE

}

############################################################################
############################################################################
## main
##

#set -x
echo "============================================================"
echo "MDIS System Scan - generate initial system.dsc / Makefile"
echo "============================================================"
echo

if [ $# -lt 1 ]; then
	usage
	exit 1
fi

VERBOSE_PRINT=$2
if [ "$3" != "" ]; then
    PCI_DRYTEST=$3
else
    PCI_DRYTEST=""
fi

MEN_LIN_DIR=$1
DSC_TPL_DIR=$MEN_LIN_DIR/BUILD/MDIS/TPL/DSC
debug_print "MEN_LIN_DIR = $1"

##
# prerequisites
#
if [[ -e "$DSC_FILE" && $SCAN_SIM == 0 ]]; then
	echo "backing up system.dsc..."
    mv $DSC_FILE $DSC_FILE.bak
fi
if [[ -e "$MAKE_FILE" && $SCAN_SIM == 0 ]]; then
	echo "backing up Makefile..."
    mv $MAKE_FILE $MAKE_FILE.bak
fi


if [ "$PCI_DRYTEST" == "" ]; then
    # write PCI devices into tmp file for further analysis
    if [[ -e $TMP_PCIDEVS && $SCAN_SIM == 0 ]]; then
        rm $TMP_PCIDEVS
    fi
    if [ $SCAN_SIM == 0 ]; then
		$MEN_LIN_DIR/BIN/$FPGA_LOAD -s > $TMP_PCIDEVS
    fi

else
    echo "PCI_DRYTEST set, using file $PCI_DRYTEST."
    TMP_PCIDEVS=$PCI_DRYTEST
fi

echo "checking if gksu/gksudo exists..."
have_gksu=`which gksu`
if [ "$have_gksu" == "" ]; then
    echo "*** error: please install gksu. Examples: Ubuntu: apt-get install gksu, Fedora: yum install gksu"
    exit 1
else
    echo "OK."
fi


echo "checking if I2C tools exists..."
have_i2ctools=`which i2cdump`
if [ "$have_i2ctools" == "" ]; then
    echo "*** error: please install i2c-tools. Examples: Ubuntu: apt-get install i2c-tools, Fedora: yum install i2c-tools"
    exit 1
else
    echo "OK."
fi

echo "checking if PCI utils exists..."
have_pciutils=`which setpci`
if [ "$have_pciutils" == "" ]; then
    echo "*** error: please install pciutils. Ubuntu: apt-get install pciutils, Fedora: yum install pciutils"
    exit 1
else
    echo "OK."
fi

# check if /usr/src/linux/ exists and if its a valid kernel src/header folder
echo "checking if /usr/src/linux/ points to valid kernel headers..."
find $LIN_SRC_DIR/include/ -name "autoconf.h" > /dev/null 2>&1
if [ "$?" == "0" ]; then
    echo "OK"
else
    echo "*** error: please set a symlink /usr/src/linux to the headers of your current running kernel:"
    echo "           e.g.   ln -s /usr/src/linux-headers.x.y.z /usr/src/linux"
    exit 1
fi

# all ok, let the games begin...
# check on which CPU we are running

detect_board_id

G_SmBusNumber=$smbus
main_cpu=`echo $G_cpu | awk '{print substr($1,1,4)}'`
wiz_model_cpu=""
echo "Found CPU: $main_cpu. Using SMB address $G_SmBusNumber for SMB2 based drivers"

#default for most F1x cards
wiz_model_busif=1
bCreateXm01bcDrv=0
bCreateF14bcDrv=0

#unfortunately some F-cards seem to be have IDs with and without '0' (marketing name)
case $main_cpu in
    SC24)
		wiz_model_cpu=SC24_BC2_BCxx
		wiz_model_smb=SMBPCI_FCH
		;;
    F011)
		wiz_model_cpu=F11S
		wiz_model_smb=SMBPCI_SCH
		G_primPciPath=0x3c
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F11S)
		wiz_model_cpu=F11S
		wiz_model_smb=SMBPCI_SCH
		G_primPciPath=0x3c
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F14)
		wiz_model_cpu=F14
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		add_xm01bc_support
		bCreateXm01bcDrv=1
		;;
    F014)
		wiz_model_cpu=F14
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F15)
		wiz_model_cpu=F15
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F015)
		wiz_model_cpu=F15
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F17)
		wiz_model_cpu=F17
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateF14bcDrv=1
		add_f14bc_support
		;;
    F017)
		wiz_model_cpu=F17
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateF14bcDrv=1
		add_f14bc_support
		;;
    F19P)
		wiz_model_cpu=F19P_F19C
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F19C)
		wiz_model_cpu=F19P_F19C
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F019)
		wiz_model_cpu=F19P_F19C
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    F21P)
		wiz_model_cpu=F21P_F21C
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		wiz_model_busif=0
		;;
    F21C)
		wiz_model_cpu=F21P_F21C
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		wiz_model_busif=0
		;;
    F021)
		wiz_model_cpu=F21P_F21C
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1e
		wiz_model_busif=0
		;;
    F075)
		wiz_model_cpu=F75P
		wiz_model_smb=SMBPCI_SCH
		G_primPciPath=0x18
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    XM01)
		wiz_model_cpu=XM1
		wiz_model_smb=SMBPCI_SCH
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    MM01)
		wiz_model_cpu=MM1
		wiz_model_smb=SMBPCI_SCH
		G_primPciPath=0x1c
		bCreateXm01bcDrv=1
		add_xm01bc_support
		;;
    G20-)
		wiz_model_cpu=G20
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1c
		wiz_model_busif=7
		add_z001_io_support
		;;
    G22-)
		wiz_model_cpu=G20
		wiz_model_smb=SMBPCI_ICH
		G_primPciPath=0x1c
		wiz_model_busif=7
		add_z001_io_support
		;;
    *)
		echo "No F1x CPU type found!"
		;;
esac

debug_print "Using _WIZ_MODEL = $wiz_model_cpu"

if [ "$main_cpu" == "SC24" ]; then
	map_sc24_fpga
	cat $DSC_TPL_DIR/sc24.tpl >> $DSC_FILE
	cat $DSC_TPL_DIR/Makefile.sc24.tpl >> $MAKE_FILE
else
    #all other CPUs: detect PCI boards, start with CPU/SMB drivers
	create_entry_dsc_cpu_type $DSC_TPL_DIR $wiz_model_cpu
	create_entry_dsc_smb_type $DSC_TPL_DIR $G_SmBusNumber $wiz_model_smb $wiz_model_busif
	if [ $bCreateXm01bcDrv == 1 ]; then
		create_entry_dsc_smb_drv  $DSC_TPL_DIR $G_SmBusNumber xm01bc_1 XM01BC XM01BC
	fi
	if [ $bCreateF14bcDrv == 1 ]; then
		create_entry_dsc_smb_drv  $DSC_TPL_DIR $G_SmBusNumber f14bc_1 F14BC F14BC
	fi
    # add the SMB2 userland API too
	G_makefileUsrLibs+=" SMB2_API/COM/library.mak"
	echo " Scanning for MEN PCI devices: "
	scan_for_pci_devs $G_primPciPath

    # dsc section build done, now create the Makefile
	create_makefile
fi
echo "finished."
sudo chmod 777 *

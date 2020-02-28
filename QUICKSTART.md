# MDIS for Linux - Quick Start Guide

This document is intended for developers who have already read the MDIS User Manual
for Linux and know the details of MDIS. You can use it as a cheat sheet to set up
MDIS for Linux, but it doesn't provide any background information on MDIS.
For comprehensive information, please refer to the MDIS User Manual for Linux:
<https://www.men.de/software/13md05-90/#doc>

## Conventions

- **Host system**: Development workstation, where you build the MDIS software modules.
- **Target system**: MEN computer system where the MDIS software modules run.
- ``$ <command>``: Indicates to run command as normal user
- ``# <command>``: Indicates to run command as root

## MDIS Software and Documentation

The MDIS5 Software and Documentation for Linux is available from the MEN website
<https://www.men.de/software/13md05-90/>:

- MDIS5 System Package (*13md05-90.zip*)
- MDIS5 under Linux User Manual (*21md05-90.pdf*)
- MDIS5 Developer Guide (*21md05-06.pdf*)

## Host Prerequisites

To compile kernel modules on the host system, you require:

- **Build Toolchain**
  - gcc, make, ...

- **Kernel Headers (or Sources)**
  - The kernel headers must match the exact version of the running kernel on the
    target system. Determine the running kernel version:

    ```
    $ uname -r
    4.18.0-80.11.2.el8_0.x86_64
    ```

  - You must prepare the kernel headers. Otherwise, you may get error messages
    saying that a version file like *utsrelease.h*/*version.h* can't be found or
    "No rule to make target ../.kernelsettings" during compilation of kernel modules.

- **Symlink to Kernel Headers**
  - /usr/src/linux --> kernel-header-location

### Ubuntu / Debian

- Install the build toolchain:

  ``# apt-get install build-essential``
  
- Install the kernel headers
  - for specified kernel version:

    ``# apt-get install kernel-headers-<kernel-version>``

  - for running kernel version:

    ``# apt-get install kernel-headers-$(uname -r)``
  
- Create the Symlink to the kernel headers
  - for specified kernel version:

    ``# ln -s /usr/src/linux-headers-<kernel-version> /usr/src/linux``

  - for running kernel version:

    ``# ln -s /usr/src/linux-headers-$(uname -r) /usr/src/linux``
  
- Prepare the kernel headers:

  ```
  # cd /usr/src/linux
  # make prepare
  ```

### CentOs / RedHat

Note: RedHat Linux is not officially supported!

- Install the build toolchain:

  ```
  # yum groupinstall "Development Tools"
  # yum install elfutils-libelf-devel
  ```

- Install the kernel headers
  - for specified kernel version:

    ``# yum install kernel-devel-<kernel-version>``

  - for running kernel version:

    ``# yum install kernel-devel``

- Create the Symlink to the kernel headers
  - for specified kernel version:

    ``# ln -s /usr/src/kernels/<kernel-version> /usr/src/linux``

  - for running kernel version:

    ``# ln -s /usr/src/kernels/$(uname -r) /usr/src/linux``

- Prepare the kernel headers:

  ```
  # cd /usr/src/linux
  # make oldconfig
  # make prepare
  ```

### Manjaro / Arch Linux

Note: Manjaro/Arch Linux is not officially supported!

Example for self-hosted development:

- Install the build toolchain:

  ``# pacman -Sy base-devel``

- Determine the matching kernel header package:

  ```
  # uname -r
  5.3.11-1-MANJARO
  # pacman -Ss linux-headers | grep 5.3.11-1
  core/linux53-headers 5.3.11-1
  ```

- Install the kernel headers:

  ``# pacman -S linux53-headers``

- Create the Symlink to the kernel headers:

  ``# ln -s /usr/lib/modules/$(uname -r)/build /usr/src/linux``

### Suse Linux

Note: Suse Linux is not officially supported!

Example for self-hosted development:

- Install the build toolchain and the kernel sources:

  ``# zypper in -t pattern kernel-devel``

- Create the Symlink to the kernel sources:

  ``# ln -s /usr/src/linux-$(uname -r) /usr/src/linux``

- Prepare the kernel sources:

  - Configure kernel sources:

    ```
    # cd /usr/src/linux
    # make cloneconfig
    # make prepare
    # make scripts
    ```

  - Copy Module.symvers:

    ``# cp /usr/src/linux-obj/x86_64/default/Module.symvers /usr/src/linux``

## Target Prerequisites

For a system scan with *scan_system.sh*, the following packages must be installed
on the target system:

- *i2c-utils*
- *pci-utils*

Note: *scan_system.sh* checks the existence of these packages.

## Installing MDIS on the Host

- Download the *13md05-90.zip* file and extract it on the host:

  ``$ unzip 13md05-90.zip``

  Note: *13md05-90.zip* contains a compressed tarball.

- Extract the compressed tarball, e.g.:

  ``$ tar -xvzf 13MD05-90_02_00.tar.gz``

- Call install script as root and follow the installation instructions:

  ```
  # cd 13MD05-90
  # ./INSTALL
  ```

- If you are using the target system as host system (self-hosted), you can optionally:

  - Perform a system scan to detect MEN hardware and to automatically create a
    base MDIS configuration for your target system.

  - Build the MDIS modules with the configuration from the system scan.

  - Install the built MDIS modules at your system.

## MDIS Project Configuration

An MDIS project configuration is stored in two files:

- *Makefile*: stores the build configuration

- *system.dsc*: stores device configuration for the drivers

### Creating the Configuration by Scanning the Target

Note: This description is applicable for self-hosted environments!

The *scan_system.sh* script performs a scan. You can execute it in different ways:

- During the MDIS Host Installation with the INSTALL script (see above).

- Manually from the root console:

  ```
  # mkdir myproj
  # cd myproj
  # /opt/menlinux/scan_system.sh /opt/menlinux
  ```

- From the MDIS Wizard:
  
  - Start the MDIS Wizard

    ```
    # mkdir myproj
    # cd myproj
    # /opt/menlinux/BIN/mdiswiz
    ```

  - Click *Cancel*

  - Click *Build* > *Scan*

The created *Makefile*/*system.dsc* will be stored in the current working directory
(in the examples above in the *myproj* directory).

### Creating the Configuration Manually on the Host

- Start the MDIS Wizard (from an arbitrary working directory):

  ``# /opt/menlinux/BIN/mdiswiz``

- Select *Create new project* (default), then press *OK* and follow the MDIS Wizard
  instructions

### Changing an MDIS Configuration

- Change the working directory to an existing MDIS project
  (where *Makefile*/*system.dsc* resides):

  ``# cd ~/myproj``

- Start the MDIS Wizard:

  ``# /opt/menlinux/BIN/mdiswiz``

## Building MDIS Modules

The Linux make utility builds MDIS modules on the **host**.
You can execute the utility manually or from the MDIS Wizard (*Build* > *Build*).

### Building MDIS Modules Using the Command Line

- Change the working directory to an existing MDIS project
  (where *Makefile*/*system.dsc* resides):

  ``# cd ~/myproj``

- Build all MDIS Modules:

  ``# make``
  
- Perform the special build task:

  ``# make <build-target>``

  You can add the following build targets as optional parameters to perform special
  build tasks:

  - *clean*         : Remove all binaries

  - *buildmods*     : Build all kernel modules,
                     this includes the following sub targets:
    - *all_ll*        : low-level drivers
    - *all_bb*        : BBIS drivers
    - *all_kernel*    : MDIS/BBIS kernel
    - *all_core*      : core libs

  - *buildusr*      : Build all user mode libs and programs,
                     this includes the following sub targets:
    - *all_usr_libs*  : user mode libs
    - *all_ll_tools*  : low-lever driver tools
    - *all_com_tools* : common tools
    - *all_nat_tools* : native tools

  - *all_desc*      : Build all descriptors

## Installing MDIS Binaries

The Linux make utility installs MDIS binaries on the **target**.
You can execute the utility manually or from the MDIS Wizard (*Build* > *Install*).

### Installing MDIS Binaries Using the Command Line

- Change the working directory to an existing MDIS project
  (where *Makefile*/*system.dsc* resides):

  ``# cd ~/myproj``

- Build all MDIS Modules:

  ``# make``

### Binary Locations

The binaries will be installed in the following directories on the target:

- Kernel modules

  ``/lib/modules/<kernel-version>/misc``

- Shared/Static user mode libraries

  ``/usr/local/lib`` or ``/usr/lib``

- Executables

  ``/usr/local/bin``

- Descriptor files

  ``/etc/mdis``

## Loading MDIS Drivers

It is sufficient to load just the *men_mdis_kernel* kernel module with *modprobe*:

``# modprobe men_mdis_kernel``

- *modprobe* loads the specified and all dependent kernel modules
  (*men_oss*, *men_dbg*, ..)

- When a device handle to an MDIS device instance is opened by calling *M_open()*,
  all necessary MDIS kernel modules are loaded automatically.

### Automatic driver load during Linux boot

You can configure your Linux to automatically load *men_mdis_kernel* at boot time.
Under most Linux distributions, you can achieve this by adding a new config file
in the */etc/modules-load.d* directory:

``# echo "men_mdis_kernel" > /etc/modules-load.d/mdis.conf``

### Blacklist mcb and mcb_pci

When using MDIS for MEN Chameleon FPGAs, it may be necessary to blacklist the
*mcb* and *mcb_pci* kernel modules if these modules are available as loadable
kernel modules in the Linux distribution. Otherwise, you may have problems using
the MDIS *men_lx_chameleon* driver.

Note: *mcb* stands for MEN Chameleon Bus. These are Linux native drivers from MEN
and mainline since Linux kernel 3.15.

- Check if *mcb*/*mcb_pci* is available:

  ```
  # modinfo mcb mcb_pci
  modinfo: ERROR: Module mcb not found.
  modinfo: ERROR: Module mcb_pci not found.
  ```

- Blacklist *mcb*/*mcb_pci*:

  ```
  # echo mcb >> /etc/modprobe.d/blacklist.conf
  # echo mcb_pci >> /etc/modprobe.d/blacklist.conf
  ```

## Unloading MDIS Drivers

To unload all MDIS drivers, manually unload all MDIS kernel modules
(e.g. *men_bb_*, *men_ll_*) that are not used by other kernel modules with
*modprobe -r*:

- *modprobe -r* unloads the specified and all dependent kernel modules
  (*men_oss*, *men_dbg*, ..)

- Only kernel modules which are not in use by other kernel modules can be unloaded.

**Example:**

If these MDIS drivers are currently loaded:

```
# lsmod | grep men
men_bb_chameleon_pcitbl    30100  0
men_chameleon_io           30428  1 men_bb_chameleon_pcitbl
men_chameleon              30406  1 men_bb_chameleon_pcitbl
men_ll_z17                 17215  0
men_mdis_kernel            44953  1 men_ll_z17
men_bbis_kernel            18021  2 men_mdis_kernel,men_bb_chameleon_pcitbl
men_desc                   14052  4 men_ll_z17,men_mdis_kernel, ..
men_oss                    37746  7 men_chameleon,men_ll_z17,men_mdis_kernel,..
men_dbg                    13038  8 men_chameleon,men_ll_z17,men_mdis_kernel,men_oss, ..
```

You have to unload this kernel modules:

```
# modprobe -r men_ll_z17
# modprobe -r men_bb_chameleon_pcitbl
```

## Testing the Drivers

In general, MDIS tools require a device name as first parameter. All MDIS device
names are defined in the MDIS project configuration. You can change the names
individually within the MDIS Wizard.

### Using the m_open Tool

You can test if the driver is ready to use with the *m_open* utility, that calls
*M_open()* and *M_close()*:

```
# m_open gpio_1
open gpio_1
path=3 opened
close path
```

### Using Driver Specific Tools

Each MDIS driver comes with one or more example programs and tools. Usually the
binary names of the tools use the driver name as prefix.

#### Example for the Z17 GPIO MDIS driver

List available tools:

```
# z17_<tab><tab>
z17_io    z17_simp
```

Show usage info by calling the tool without parameter:

```
# z17_io
Usage:    z17_io <device> <opts> [<opts>]
Function: Access the 16Z034 GPIOs (8-bit)
Options:
    device        device name (e.g. z17_1)
    -p=<port>     i/o port(s) 0..7.....................[all]
    -g            get state of input port(s)
    -s=0/1        set output port(s) low/high
    -t            toggle all output ports in turn
    -G            get state of input port(s) in a loop
    -T [<opts>]   toggle output port(s) endlessly
        -o=<time>     toggle high (in ms)..............[1000]
        -l=<time>     toggle low (in ms)...............[1000]
    -h            hold path open until keypress

Copyright 2006-2019, MEN Mikro Elektronik GmbH
13Z017-06_01_30-16-g4cecd4d_2019-11-27
```

Toggle all output ports until keypress:

```
# z17_io gpio_1 -T -h
toggle output state ports 0..7
holding path open until keypress
^C
```

## Getting MDIS Related Information

### View SW Module Documentation

Most MDIS modules (drivers, APIs) come with an HTML documentation generated from
source code:

- The documentation is usually located in a *DOC* subfolder of the module sources.

- The documentation file with the main page is always named *index.html*.

- You can view the HTML documentation with any web browser. Example:

  ``# firefox /opt/menlinux/LIBSRC/MDIS_API/DOC/html/index.html``

### Check for Available MDIS Modules

- List MDIS kernel modules:

  ```
  [root@mdis4lin mproj_npi]# ls /lib/modules/$(uname -r)/misc
  men_bb_chameleon_pcitbl.ko  men_dbg.ko        men_ll_z15.ko        men_oss.ko
  men_bb_d203.ko              men_desc.ko       men_ll_z17.ko        men_pld.ko
  men_bbis_kernel.ko          men_id.ko         men_lx_chameleon.ko  men_pld_sw.ko
  men_bb_smb2.ko              men_id_sw.ko      men_lx_z25.ko        men_smb_port_io.ko
  men_chameleon_io.ko         men_ll_smb2.ko    men_mbuf.ko          men_smb_port.ko
  men_chameleon.ko            men_ll_xm01bc.ko  men_mdis_kernel.ko
  ```

- List shared/static MDIS user mode libraries:

  ```
  # ls /usr/lib/lib<tab><tab>
  libmdis_api.so   libsmb2_api.so   libusr_oss.so
  libmscan_api.so  libsmb2_shc.so   libusr_utl.so
  ```

- List MDIS executable:

  ```
  # ls /usr/local/bin
  mdis_createdev  m_rev_id       smb2_bmc         smb2_simp         z127_io
  m_errstr        mscan_alyzer   smb2_boardident  smb2_stm32_flash  z127_out
  m_getblock      mscan_loopb    smb2_ctrl        smb2_test         z17_io
  m_getstat       mscan_menu     smb2_eeprod2     smb2_touch        z17_simp
  m_getstat_blk   m_setblock     smb2_eetemp      wdog_simp
  m_mod_id        m_setstat      smb2_f601        wdog_test
  m_open          m_setstat_blk  smb2_poe         xm01bc_ctrl
  m_read          m_write        smb2_shc_ctrl    z127_in
  ```

- List MDIS descriptor files:

  ```
  # ls /etc/mdis
  can_15.bin  cpu.bin          gpio_g215_1.bin  smb2_1.bin     uart_2.bin
  can_16.bin  d203_1.bin       gpio_g215_2.bin  smb2_bus0.bin  uart_3.bin
  can_7.bin   gpio_f215_1.bin  mezz_cham_1.bin  uart_10.bin    xm01bc_1.bin
  can_8.bin   gpio_f215_2.bin  mezz_cham_2.bin  uart_11.bin
  ```

  Note: Useful to show the MDIS device names. The descriptor file are named
  according the device names (with the extension *.bin*).

### Check for Loaded MEN Drivers

```
# lsmod | grep men
men_mdis_kernel        44953  0
men_bbis_kernel        18021  1 men_mdis_kernel
men_desc               14052  2 men_mdis_kernel,men_bbis_kernel
men_oss                37746  3 men_mdis_kernel,men_bbis_kernel,men_desc
men_dbg                13038  4 men_mdis_kernel,men_oss,men_bbis_kernel,men_desc
```

### Get MDIS Module Versions

- Show kernel module info:

  ```
  # modinfo men_ll_z17
  filename:       /lib/modules/3.10.0-862.el7.x86_64/misc/men_ll_z17.ko
  version:        13Z017-06_01_30-16-g4cecd4d_2019-11-27
  license:        GPL
  author:         Klaus Popp <klaus.popp@men.de>
  description:    men_ll_z17 MDIS low level driver
  retpoline:      Y
  rhelversion:    7.5
  srcversion:     ED61FCDA601747B842B7A4F
  depends:        men_oss,men_desc,men_dbg,men_mdis_kernel
  vermagic:       3.10.0-862.el7.x86_64 SMP mod_unload modversions
  ```

- Show MDIS revision strings:

  ```
  # m_rev_id gpio_1
  -----[ MDIS Kernel ]-----
  13MD05-90_02_00_01-387-g1a9f671-dirty_2019-11-27
  -----[ Low Level Driver ]-----
  13Z017-06_01_30-16-g4cecd4d_2019-11-27 Z34/Z37 model
  mdis_libsrc_desc_com_01_45-4-g92e6d2a_2019-11-26
  5a538dd_2019-11-27
  -----[ BBIS Kernel ]-----
  13MD05-90_02_00_01-387-g1a9f671-dirty_2019-11-27
  -----[ BBIS Driver ]-----
  mdis_drivers_bbis_chameleon_com_01_74-6-ga51d41c_2019-11-26
  mdis_libsrc_desc_com_01_45-4-g92e6d2a_2019-11-26
  5a538dd_2019-11-27
  ```

### Show Debug Output

Only MDIS binaries with debug information prints debug messages. By default, the
binaries are built without debug information. You can specify to build debug
binaries within MDIS Wizard or directly in the makefile.

```
# dmesg
...
[21185.592922] HEnumTbl#0 : -------------- leaving with ret=0x0 ---------------
[21185.592926]  Unit                devId   Grp Rev  Inst  IRQ   BAR  Offset      Addr
[21185.592959]  00 16Z024_SRAM      0x0018  0   12   0x00  0x3f   0   0x00000000  0x00000000b0500000
[21185.592973]  01 16Z125_UART      0x007d  0   12   0x00  0x02   0   0x00000100  0x00000000b0500100
[21185.592985]  02 16Z125_UART      0x007d  0   12   0x01  0x03   0   0x00000110  0x00000000b0500110
[21185.592999]  03 16Z069_RST       0x0045  0   10   0x00  0x3f   0   0x00000140  0x00000000b0500140
[21185.593011]  04 16Z052_GIRQ      0x0034  0    6   0x00  0x3f   0   0x00000160  0x00000000b0500160
[21185.593028]  05 16Z034_GPIO      0x0022  0   10   0x00  0x3f   0   0x00000180  0x00000000b0500180
[21185.593040]  06 16Z037_GPIO      0x0025  0    1   0x00  0x04   0   0x000001a0  0x00000000b05001a0
[21185.593052]  07 16Z126_SERFLASH  0x007e  0    3   0x00  0x3f   0   0x000001c0  0x00000000b05001c0
[21185.593065]  08 16Z029_CAN       0x001d  0   16   0x00  0x00   0   0x00000200  0x00000000b0500200
[21185.593077]  09 16Z029_CAN       0x001d  0   16   0x01  0x01   0   0x00000300  0x00000000b0500300
[21185.593081] CHAM - TableIdent: idx=0
...
```

### Show PCI Configuration

- List MEN PCI Devices (*lspci*):

  ```
  # lspci -d 1a88:
  04:0e.0 Bridge: MEN Mikro Elektronik Multifunction IP core (rev 01)
  05:00.0 Communication controller: MEN Mikro Elektronik Multifunction IP core (rev 01)
  ```

  Note: Check for either MEN PCI vendor ID 0x1a88 or Altera's PCI vendor ID 0x1172,
  which is used on older MEN Chameleon FPGA designs.

- List MEN PCI devices with PCI vendor, device and subvendor IDs (*lspci*):

  ```
  # lspci -d 1a88: -vmn
  Device: 04:0e.0
  Class:  0680
  Vendor: 1a88
  Device: 4d45
  SVendor:        006a
  SDevice:        5a14
  Rev:    01
  
  Device: 05:00.0
  Class:  0780
  Vendor: 1a88
  Device: 4d45
  SVendor:        00a2
  SDevice:        5a91
  Rev:    01
  ```

- List all PCI devices with PCI vendor, device and subvendor IDs (*fpga_load*):

  ```
  # /opt/menlinux/BIN/fpga_load -s
  
  Nr.| dom|bus|dev|fun| Ven ID | Dev ID | SubVen ID |
    0|  0   0   0   0   0x8086   0x0c04    0x8086
    1|  0   0   1   0   0x8086   0x0c01    0x0000
    2|  0   0   2   0   0x8086   0x0406    0x8086
    3|  0   0   3   0   0x8086   0x0c0c    0x8086
    4|  0   0  20   0   0x8086   0x8c31    0x8086
    5|  0   0  22   0   0x8086   0x8c3a    0x8086
    6|  0   0  25   0   0x8086   0x15a0    0x8086
    7|  0   0  26   0   0x8086   0x8c2d    0x8086
    8|  0   0  27   0   0x8086   0x8c20    0x8086
    9|  0   0  28   0   0x8086   0x8c12    0x0000
   10|  0   0  29   0   0x8086   0x8c26    0x8086
   11|  0   0  31   0   0x8086   0x8c4f    0x8086
   12|  0   1   0   0   0x8086   0x1539    0x8086
   13|  0   2   0   0   0x8086   0x1539    0x8086
   14|  0   3   0   0   0x12d8   0xe110    0x0000
   15|  0   4  14   0   0x1a88   0x4d45    0x006a
   16|  0   5   0   0   0x1a88   0x4d45    0x00a2
  ```

- Show Chameleon table of MEN FPGAs:

  ```
  # /opt/menlinux/BIN/fpga_load 0x1a88 0x4d45 0x006a 0 -t
  
  BARs of FPGA PCI device 04:14.0:
    BAR0: 0xb0500000; size: 0x00000000, mapType: MEM;
    BAR1: 0x00004001; size: 0x00000000, mapType: IO;
    BAR2: 0x00000000; size: 0x00000000, mapType: unused;
    BAR3: 0x00000000; size: 0x00000000, mapType: unused;
    BAR4: 0x00000000; size: 0x00000000, mapType: unused;
    BAR5: 0x00000000; size: 0x00000000, mapType: unused;
  
  Information about the Chameleon FPGA:
  FPGA File='215-00IC001A' table model=0x41('A') Revision 1.0 Magic 0xABCE
  List of the Chameleon units:
  Idx DevId  Module                   Grp Inst Var Rev IRQ BAR Offset     Address
  --- ------ ------------------------ --- ---- --- --- --- --- ---------- ----------
    0 0x0018 16Z024_SRAM                0    0   1  12  63   0 0x00000000 0xb0500000
    1 0x007d 16Z125_UART                0    0   0  12   2   0 0x00000100 0xb0500100
    2 0x007d 16Z125_UART                0    1   0  12   3   0 0x00000110 0xb0500110
    3 0x0045 16Z069_RST                 0    0   0  10  63   0 0x00000140 0xb0500140
    4 0x0034 16Z052_GIRQ                0    0   0   6  63   0 0x00000160 0xb0500160
    5 0x0022 16Z034_GPIO                0    0   0  10  63   0 0x00000180 0xb0500180
    6 0x0025 16Z037_GPIO                0    0   1   1   4   0 0x000001a0 0xb05001a0
    7 0x007e 16Z126_SERFLASH            0    0   0   3  63   0 0x000001c0 0xb05001c0
    8 0x001d 16Z029_CAN                 0    0   1  16   0   0 0x00000200 0xb0500200
    9 0x001d 16Z029_CAN                 0    1   1  16   1   0 0x00000300 0xb0500300
  
   Current FPGA file/usage status: fallback image active, no configuration error occurred.
  ```

### Show I2C/SMB Configuration

- List all available I2C/SMB busses:

  ```
  # i2cdetect -l
  i2c-0   i2c             i915 gmbus ssc                          I2C adapter
  i2c-1   i2c             i915 gmbus vga                          I2C adapter
  i2c-2   i2c             i915 gmbus panel                        I2C adapter
  i2c-3   i2c             i915 gmbus dpc                          I2C adapter
  i2c-4   i2c             i915 gmbus dpb                          I2C adapter
  i2c-5   i2c             i915 gmbus dpd                          I2C adapter
  i2c-6   smbus           SMBus I801 adapter at 7040              SMBus adapter
  ```

- List all I2C/SMB devices on a specified bus:

  ```
  # i2cdetect -y 6
       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
  00:          -- -- -- -- -- 08 -- -- -- -- -- -- --
  10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 1f
  20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  30: -- -- 32 -- -- -- -- 37 -- -- -- -- -- -- -- --
  40: -- -- -- -- 44 -- -- -- -- -- -- -- -- 4d -- --
  50: -- -- -- -- -- -- -- 57 -- -- -- -- -- -- -- --
  60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
  70: -- -- -- -- -- -- -- --
  ```

- Dump the registers of a specified I2C/SMB device:

  ```
  # i2cdump -y 6 0x57
  No size specified (using byte-data access)
       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f    0123456789abcdef
  00: e7 00 03 00 00 00 00 3a 02 46 32 33 50 00 00 ff    ?.?....:?F23P...
  10: 32 52 ff ff ff ff ff ff ff ff ff ff ff ff ff ff    2R..............
  20: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
  30: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
  ...
  ```

  Note: The content of the Board Information EEPROM with SMB address 0x57
  (7-bit notation) was dumped. The address in 8-bit notation is 0xAE (=0x57<<1).

For further information about the usage of Linux I2C/SMB controller drivers and
i2c tools on MEN CPUs, see application note
*Using the Standard I2C Tools on MEN CPUs under Linux*:
<https://www.men.de/downloads/search/dl/sk/Application%20Note%20I2C%20Tools%20Linux/>

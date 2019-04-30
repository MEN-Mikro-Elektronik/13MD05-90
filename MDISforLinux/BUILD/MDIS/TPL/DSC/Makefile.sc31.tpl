#CREATION_NOTE
ifndef MEN_LIN_DIR
MEN_LIN_DIR = /opt/menlinux
endif

# You need to select the development environment so that MDIS
# modules are compiled with the correct tool chain

WIZ_CDK = Selfhosted

# All binaries (modules, programs and libraries) will be
# installed under this directory.

# TARGET_TREE

# The directory of the kernel tree used for your target's
# kernel. If you're doing selfhosted development, it's
# typically /usr/src/linux. This directory is used when
# building the kernel modules.

LIN_KERNEL_DIR = /usr/src/linux

# Defines wether to build MDIS to support RTAI. If enabled,
# MDIS modules support RTAI in addition to the standard Linux
# mode. Set it to "yes" if you want to access MDIS devices from
# RTAI applications

MDIS_SUPPORT_RTAI = no

# The directory where you have installed the RTAI distribution
# via "make install"

# RTAI_DIR

# The include directory used when building user mode libraries
# and applications. If you're doing selfhosted development,
# it's typically /usr/include. If you're doing cross
# development, select the include directory of your cross
# compiler. Leave it blank if your compiler doesn't need this
# setting.

# LIN_USR_INC_DIR

# Define wether to build/use static or shared user state
# libraries. In "static" mode, libraries are statically linked
# to programs. In "shared" mode, programs dynamically link to
# the libraries. "shared" mode makes programs smaller but
# requires installation of shared libraries on the target

LIB_MODE = shared

# Defines wether to build and install the release (nodbg) or
# debug (dbg) versions of the kernel modules. The debug version
# of the modules issue many debug messages using printk's for
# trouble shooting

ALL_DBGS = nodbg

# The directory in which the kernel modules are to be
# installed. Usually this is the target's
# /lib/modules/$(LINUX_VERSION)/misc directory.

MODS_INSTALL_DIR = /lib/modules/$(LINUX_VERSION)/misc

# The directory in which the user state programs are to be
# installed. Often something like /usr/local/bin. (relative to
# the target's root tree)

BIN_INSTALL_DIR = /usr/local/bin

# The directory in which the shared (.so) user mode libraries
# are to be installed. Often something like /usr/local/lib.
# (relative to the target's root tree)

LIB_INSTALL_DIR = /usr/local/lib

# The directory in which the static user mode libraries are to
# be installed. Often something like /usr/local/lib on
# development host. For cross compilation select a path
# relative to your cross compilers lib directory.

STATIC_LIB_INSTALL_DIR = /usr/local/lib

# The directory in which the MDIS descriptors are to be
# installed. Often something like /etc/mdis. (Relative to the
# targets root tree)

DESC_INSTALL_DIR = /etc/mdis

# The directory in which the MDIS device nodes are to be
# installed. Often something like /dev. (Relative to the
# targets root tree)

DEVNODE_INSTALL_DIR = /dev

ALL_LL_DRIVERS = \
	Z017/DRIVER/COM/driver.mak \
	XM01BC/DRIVER/COM/driver.mak \
	SMB2/DRIVER/COM/driver.mak \
	MSCAN/DRIVER/COM/driver_z15.mak \
	Z082/DRIVER/COM/driver.mak

ALL_BB_DRIVERS = CHAMELEON/DRIVER/COM/driver_isa.mak \
		 SMB2BB/DRIVER/NATIVE/driver.mak

ALL_USR_LIBS = \
	USR_OSS/library.mak \
	USR_UTL/COM/library.mak \
	MSCAN_API/COM/library.mak

ALL_CORE_LIBS = \
	DBG/library.mak \
	OSS/library.mak \
	CHAMELEON/COM/library.mak

ALL_LL_TOOLS = \
	Z017/EXAMPLE/Z17_SIMP/COM/program.mak \
	Z017/TOOLS/Z17_IO/COM/program.mak \
	Z017/TOOLS/Z127_IO/COM/program.mak \
	MSCAN/TOOLS/MSCAN_ALYZER/COM/program.mak \
	MSCAN/TOOLS/MSCAN_LOOPB/COM/program.mak \
	MSCAN/TOOLS/MSCAN_MENU/COM/program.mak \
	Z082/EXAMPLE/Z82_SIMP/COM/program.mak \
 	XM01BC/TOOLS/XM01BC_CTRL/COM/program.mak


ALL_COM_TOOLS = \
	MDIS_API/M_ERRSTR/COM/program.mak \
	MDIS_API/M_GETBLOCK/COM/program.mak \
	MDIS_API/M_GETSTAT/COM/program.mak \
	MDIS_API/M_GETSTAT_BLK/COM/program.mak \
	MDIS_API/M_MOD_ID/COM/program.mak \
	MDIS_API/M_OPEN/COM/program.mak \
	MDIS_API/M_READ/COM/program.mak \
	MDIS_API/M_REV_ID/COM/program.mak \
	MDIS_API/M_SETBLOCK/COM/program.mak \
	MDIS_API/M_SETSTAT/COM/program.mak \
	MDIS_API/M_SETSTAT_BLK/COM/program.mak \
	MDIS_API/M_WRITE/COM/program.mak

ALL_NATIVE_DRIVERS =

ALL_NATIVE_LIBS = 

ALL_NATIVE_TOOLS = 

ALL_DESC = system

include $(MEN_LIN_DIR)/BUILD/MDIS/TPL/rules.mak

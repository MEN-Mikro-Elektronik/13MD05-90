# Changelog for 13MD05-90 "MEN MDIS for Linux System Package plus lowlevel drivers"

## [13MD05-90_02_04] - 2021-03-01

### Added
- 13Z025 driver parameter for different baud base for each of the UART instances
- Linux kernel lockdown detection in fpga_load, mm_ident and hwbug

### Changed
- Updated documentation

### Fixed
- PLDZ002 VME driver interrupt handling
- Adding SMB2 tools to SC31
- Compilation with Linux kernel 5.10


## [13MD05-90_02_03] - 2020-10-02

### Changed
- MDIS Wizard binary update

### Fixed
- System scanner script
- vme4l-core compilation on CentOS 8
- Real-time signal definitions
- MDIS locking mechanism
- Compatibility with kernel 5.6.0
- Compatibility with CentOS 7 with kernel 3.10


## [13MD05-90_02_02] - 2020-05-15

### Added
- fpga_load_ppc binary for PowerPC
- 13Z055-90 submodule
- m47_test tool for 13M047-06
- PCI memory regions enabling for fpga_load

### Changed
- Improved installation script
- Improved system scanner script
- MDIS Wizard binary update
- fpga_load binary update
- hwbug_ppc and hwbug_cmd_ppc binaries update
- Module name from smb_z001 to men_lx_z001 for 13Z001-90
- Logo files for 13Z044-90

### Fixed
- Installation script
- System scanner script
- Compiler warnigns
- icanl2_veri tool for 13M065-07
- Compilation on Linux kernel 4.19 for 13Z055-90
- MAC address issue for 13Z077-90
- IRQ issue for mdis_drivers_bbis_a21_com
- Chameleon table display when accessed by LPC/ISA bus for fpga_load


## [13MD05-90_02_01] - 2019-12-13

### Added
- Automatic device node creating/destroying on VME4L module initialization/clean up
- Automated M-Module detection in scanner script
- hwbug as submodule
- Version info to kernel submodules
- Generating commit history for submodules
- 13Z050-06 submodule
- 13Z073-06 submodule
- 13Z075-06 submodule
- Automated IP core detection in scanner script
- 13M065-07 submodule
- VME4L_API as submodule
- UTI as submodule
- OSS as submodule
- DBG as submodule
- 13Z072-06 submodule
- End-of-line normalization in text files
- Automatic enabling of memory regions in PCI devices in scanner script
- MDIS Quickstart Guide

### Changed
- MDIS Wizard binary update
- Renamed installation script  INSTALL to INSTALL.sh
- Set default build mode for kernel modules to 'nodbg'
- mm_ident and fpga_load tools compiled from sources when needed
- Improved scanner script
- Updated documentation

### Removed
- 13M065-06 submodue
- fpga_load 64-bit binary
- mdis_libsrc_smb2_com submodule
- mdis_drivers_bbis_smbpci_com submodule

### Fixed
- Creation of sysfs directories
- File permissions
- Installation script
- Scanner script
- Warnings during compilation
- IO mapping support in hwbug
- Documentation
- Copyrights
- Licenses
- Signal handling in OSS library


## [13MD05-90_02_00] - 2019-03-04

### Added
- 13z12590 package description
- Readme files with brief description
- GNU License for MEN source files
- SMBus generic support in system scanner for F11S, F19, F21, F022/F22P, F075/F75P, XM01, MM01, G22-G022, G23-G023
- M-Module support in system scanner for M36N, M72, M77N, M82
- Support for BL51E in system scanner
- Changelog

### Changed
- GNU License for MEN source files
- MEN Copyright for MEN source files
- Readme files update
- MDIS Wizard binary update
- System scanner only runs as root
- descgen not cross-compiled on cross compilation
- Installer does not copy .git directories and files being copied are automatically overwritten
- Default library installation path on CentOS

### Removed
- In-file change history
- History files
- Junk files

### Fixed
- 13z04490 package name
- Compiler warnings and errors
	- 13Z077-90 on kernels 4.11-4.14
	- men_lx_z77 driver on CentOS 7.5
	- vme4l related drivers
	- and others
- System scanner portability
- Board support in system scanner for D203, F205, F204, SC31, F223
- List of ignored files and directories in .gitignore

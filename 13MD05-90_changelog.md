# Changelog for 13MD05-90 "MEN MDIS for Linux System Package plus lowlevel drivers"

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
13M065-06 submodue
fpga_load 64-bit binary
mdis_libsrc_smb2_com submodule
mdis_drivers_bbis_smbpci_com


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

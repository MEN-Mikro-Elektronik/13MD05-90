# Changelog for 13MD05-90 "MEN MDIS for Linux System Package plus lowlevel drivers"

## [13MD05-90_02_00] - 2019-03-04

### Added
- 13z12590 package description
- Readme file with brief description where missing
- License where missing
- SMBus generic support in system scanner for F11S, F19, F21, F022/F22P, F075/F75P, XM01, MM01, G22-G022, G23-G023
- M-Module support in system scanner for M36N, M72, M77N, M82
- Support for BL51E in system scanner
- Changelog

### Changed
- License
- Copyright
- Readme file
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
- Compilation of 13Z077-90 on kernels 4.11-4.14
- Compilation of men_lx_z77 driver on CentOS 7.5
- Compilation of vme4l related drivers
- Compiler warnings
- System scanner interoperability
- Board support in system scanner for D203, F205, F204, SC31, F223
- Ignored files and directories in .gitignore

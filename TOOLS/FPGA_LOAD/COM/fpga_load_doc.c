/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  fpga_load_doc.c
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2008/05/09 19:26:48 $
 *    $Revision: 1.8 $
 *
 *      \brief   User documentation for Z100 FPGA_LOAD tool
 *
 *     Required: -
 *
 *     \switches -
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: fpga_load_doc.c,v $
 * Revision 1.8  2008/05/09 19:26:48  CKauntz
 * Added Description of the parameter -z
 *
 * Revision 1.7  2007/07/13 18:26:47  CKauntz
 * removed swapped variants
 * added new option for direct addressing
 *
 * Revision 1.6  2007/07/09 20:10:47  CKauntz
 *   changes: no (_sw) only one version
 *            example for direct addressing
 *
 * Revision 1.5  2006/02/08 12:18:31  cschuster
 * added docu for setting A500 bus switches
 *
 * Revision 1.4  2005/12/12 14:25:26  cschuster
 * added page Tool Usage
 *
 * Revision 1.3  2005/07/08 17:49:44  cs
 * use MACCESS macros and chameleon (+oss_usr, +dbg_usr) library
 * add VME bus support
 * cosmetics
 *
 * Revision 1.2  2004/12/23 15:10:40  cs
 * added documentation for the build process
 *
 * Revision 1.1  2004/11/30 18:04:55  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*! \mainpage
    This is the documentation for the tool to load FPGA configurations
    into Flash memory over PCI and VME bus.

    <b>!!! Attention !!!</b>\n
    If you use direct hardware access and the FPGA is updated by any other
    method than 16Z045_FLASH flash interace (e.g. over SMBus/PLD) you
    have to make sure that you have exclusive access to the coresponding
    hardware, e.g. the SMBus. If other SW is concurrently accessing e.g. the
    SMBus host controller, the result is unpredictable.

    \n
    \section Variants Variants
    The FPGA_LOAD tool adapts at runtime to 8bit or 16bit FPGA-Flash interface
    and swapped or non-swapped versions.


    \n \section FuncDesc Functional Description

    \n \subsection General General
	The tool supports the writing/reading of data/fpga configuration files
	to flash memory.\n
	For easy update the tool provides an option to read all	parameters,
	necessary for updating an FPGA configuration, from the Chameleon table
	and the FPGA header of the fallback FPGA configuration.
	\remark For this purpose, the fallback FPGA configuration in the flash
	must have a long FPGA configuration file header, filled with the correct
	offsets for all configurations to be updated.

	\n \subsection Usage Usage
	see page \ref pageUsage

    \n \subsection Build Build
	The tool is made to be built with the MEN MDIS build environment.\n

	The tool needs the following libraries to be able to access
	the hardware: Chameleon library, OSS library (for user space)

	Please specify the switch "Z100_CONFIG_VME" in the program.mak file
	if you intend to access hardware over the VME bus.

	\linux For Linux the VME4L_API library, the VME4LX driver and
	the pciutils library are needed by the OSS_USR library.

	In order to use the tool without installing the MDIS environment
	on your target, you can built the tool using static linking of the user
	mode libraries (MDIS Makefile: LIB_MODE = static).

*/

/*! \page pageUsage Tool Usage

	By default the tool will access the flash using the MEN 16Z045_FLASH FPGA
	module which directly accesses the flash device.

	On some boards (e.g. A404, A500) the flash is accessed via another flash
	interface in a PLD. This PLD is usually addressed using an SMBus interface.

	\n \section sectusage Access modes and examples
	Here are some explanations and examples for the different access modes. For
	exact syntax and definition of the single parameters please see the help
	function of the software:
	\verbatim
	fpga_load -h
	\endverbatim

	\subsection pciZ045 Access over PCI bus and 16Z045_FLASH or 16Z126_SPI_FLASH
	  here you can normally use the Chameleon Table in the FPGA to find the
	  correct address of the FLASH interface:
	  - identify the desired PCI device:
	    \verbatim
	    > fpga_load -s
	    > fpga_load <ven ID> <dev ID> <subVen ID> -s
	    \endverbatim
	  - check that you access the correct device, read the header from FLASH
	    \verbatim
	    > fpga_load <ven ID> <dev ID> <subVen ID> <idx> -r <offs in flash> <len>
	    \endverbatim
	  - show the Chameleon Table within the FPGA
	    \verbatim
	    > fpga_load <ven ID> <dev ID> <subVen ID> <idx> -t
	    \endverbatim
	  - use the 16Z126_SPI_FLASH interface, read the header from FLASH
	    \verbatim
	    > fpga_load <ven ID> <dev ID> <subVen ID> <idx> -z -r <offs in flash> <len>
	    \endverbatim
	  - load/update the FPGA
	    \verbatim
	    > fpga_load <ven ID> <dev ID> <subVen ID> <idx> -u <FPGA update file> <offset in flash>
	    \endverbatim
	  e.g.
	  \verbatim
	  > fpga_load -s
	  > fpga_load 0x1172 0x4d45 0x7 0 -r 0x0 0x100
	  > fpga_load 0x1172 0x4d45 0x7 0 -t
	  > fpga_load 0x1172 0x4d45 0x7 0 -z -r 0x0 0x100
	  > fpga_load 0x1172 0x4d45 0x7 0 -u P018-00IC001D4.rbf 0x0
	  \endverbatim


	\subsection vmeZ045 Access over VME bus and 16Z045_FLASH or 16Z126_SPI_FLASH
	  there is no support for Chameleon tables in the FPGA on VME bus.
	  the correct address of the FLASH interface has to be identified manually.
	  The A24/D16 address window will be used. Please only specify the offset
	  in the VME access window.
	  - check that you access the correct address, read the header from FLASH
	    \verbatim
	    > fpga_load -a <reg offs in VME window> -r <offs in flash> <len>
	    \endverbatim
	  - check that you access the correct address with the SPI Flash, read the header from FLASH
	    \verbatim
	    > fpga_load -a <reg offs in VME window> -z -r <offs in flash> <len>
	    \endverbatim
	  - load/update the FPGA (verify header)
	    \verbatim
	    > fpga_load -a <reg offs in VME window> -u <FPGA update file> <offset in flash>
	    \endverbatim
	  - load/update the FPGA (ignore header)
	    \verbatim
	    > fpga_load -a <reg offs in VME window> -f -w <FPGA update file> <offset in flash>
	    \endverbatim
	  e.g.
	  \verbatim
	  > fpga_load -a 0xe003a0 -r 0x0 0x100
	  > fpga_load -a 0xe003a0 -z -r 0x0 0x100
	  > fpga_load -a 0xe003a0 -u A203N00IC041A1.rbf 0x0
	  > fpga_load -a 0xe003a0 -f -w A203N00IC041A1.rbf 0x0
	  \endverbatim


	\subsection vmeSmbPld Access over VME bus and SMB/PLD, using direct HW access
	  there is no support for Chameleon tables in the FPGA on VME bus.
	  the corresponding SMBus controller and the offset of the controlers
	  registers have to be identified manually.
	  The A24/D16 address window will be used. Please only specify the offset
	  in the VME access window.
	  - check that you access the correct address, read the header from FLASH
	    \verbatim
	    > fpga_load -a <reg offs in VME window> -p <smb controller> -r <offs in flash> <len>
	    \endverbatim
	  - load/update the FPGA (verify header)
	    \verbatim
	    > fpga_load -a <reg offs in VME window> -p <smb controller> -u <FPGA update file> <offset in flash>
	    \endverbatim
	  - load/update the FPGA (ignore header)
	    \verbatim
	    > fpga_load -a <reg offs in VME window> -p <smb controller> -f -w <FPGA update file> <offset in flash>
	    \endverbatim
	  - chek that you access the correct address for direct addressing for e.g. M-Modules
	  	\verbatim
	    > fpga_load -d <reg offs> -r <offs in flash> <len> [<file>]
	  	\endverbatim
	  e.g.
	  \verbatim
	  > fpga_load -a 0xfd8000 -p "menz001" -r 0x0 0x100
	  > fpga_load -a 0xfd8000 -p "menz001" -u A404-00IC090A1.rbf 0x0
	  > fpga_load -a 0xfd8000 -p "menz001" -f -w A404-00IC090A1.rbf 0x0
	  > fpga_load -d 0x88e000f0 -r 0x0 0x800000 file.bin
	  \endverbatim

	\subsection SmbNumPld Access over SMB/PLD, using installed SMBus controllers
	  the corresponding SMBus controller has to be identified manually.
	  - check that you access the correct SMB device, read the header from FLASH
	    \verbatim
	    > fpga_load -p <smb controller number> -r <offs in flash> <len>
	    \endverbatim
	  - load/update the FPGA (verify header)
	    \verbatim
	    > fpga_load -p <smb controller number> -u <FPGA update file> <offset in flash>
	    \endverbatim
	  - load/update the FPGA (ignore header)
	    \verbatim
	    > fpga_load -p <smb controller number> -f -w <FPGA update file> <offset in flash>
	    \endverbatim
	  e.g.
	  \verbatim
	  > fpga_load -p 0 -r 0x0 0x100
	  > fpga_load -p 0 -u A404-00IC090A1.rbf 0x0
	  > fpga_load -p 0 -f -w A404-00IC090A1.rbf 0x0
	  \endverbatim

 	\subsection SmbA500BusSw Set bus switches on A500 ESM carrier board
	  the corresponding SMBus controller has to be identified manually. Since the
	  ESM onboard SMB controller is used, the access is normally performed using
	  a preinstalled SMBus controller.
	  - check that you access the correct SMBus/device, read the header/start of
	    FPGA file from FLASH. The FPGA data in A500-R02 boards Flash memory starts
	    at 0x100 (sometimes 0x200?).
	    Here the FPGA data will start with a pattern similiar to the following:
	    \verbatim
	    hexdump:
	    FF FF 62 77 3F 00 FF FF  FF FF FF FF FF FF FF FF
	    \endverbatim
	    This can also be used to check the correct version of the tool (swap/nonswap)!
	    \verbatim
	    > fpga_load -p <smb controller number> -r 0x00 0x300
	    \endverbatim
	  - set bus switches
	    \verbatim
	    > fpga_load -p <smb controller number> -x <1/2/3>
	    \endverbatim
	  e.g.
	  \verbatim
	  > fpga_load -p 1 -x 1
	  \endverbatim
*/

/*! \page dummy
  \menimages
 */

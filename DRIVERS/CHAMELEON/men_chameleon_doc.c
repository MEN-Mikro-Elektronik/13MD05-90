/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  men_chameleon_doc.c
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2007/03/28 15:53:58 $
 *    $Revision: 1.5 $
 *
 *      \brief   User documentation for MEN Chameleon FPGA driver/device
 *               registration
 *
 *     Required: -
 *
 *     \switches -
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: men_chameleon_doc.c,v $
 * Revision 1.5  2007/03/28 15:53:58  ts
 * Cosmetics, added Description for kernel build
 *
 * Revision 1.4  2006/02/01 10:03:26  cs
 * enhanced docu for building driver as pure native
 *
 * Revision 1.3  2005/02/11 11:56:10  ub
 * documentation improved
 *
 * Revision 1.2  2005/02/04 14:02:41  ub
 * added descripion about driver parameter
 *
 * Revision 1.1  2004/12/06 11:18:43  cs
 * Initial Revision
 *
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \mainpage Linux native driver for MEN Chameleon FPGA driver/device registration.

    The men_lx_chameleon Linux kernel module serves as an
    universal interface for any functional unit inside the FPGA. It is needed
    by all Linux native drivers dealing with these FPGA units e.g. the
    13z025-90 UART driver. \n

	\n \section installation Driver Build and Installation

	There are several ways to build the men_lx_chameleon driver.

	\subsection installation_native Build pure native

	As a pure native driver, the driver can be built by itself. Therefore edit
	the \$(MEN_LIN_DIR)/DRIVERS/CHAMELEON/Makefile to adapt the defines
	MEN_LIN_DIR, KERNELDIR, DRV_SRC to match your environment and needs.
    Since the driver is intenden to be run together with the other MDIS for
    Linux Modules, there are several other files necessary to be included if
    the driver is intended to be built in the kernel.

	- Copy the directory of the driver
          \$(MEN_LIN_DIR)/LINUX/DRIVERS/CHAMELEON.  \n
      to the linux kernel directory
          \$(LINUX_DIR)/linux/drivers/men_chameleon/ \n

    - To make the kernel build process aware of the new driver directory,
      add the following line to \$(LINUX_DIR)/linux/drivers/Makefile: \n
      Linux Kernel 2.4.x: \n
\verbatim
          subdir-m += men_chameleon
\endverbatim
      Linux Kernel 2.6.x: \n
\verbatim
          obj-m += men_chameleon\
\endverbatim

	- build your system, the driver is built as men_lx_chameleon module

	\subsection installation_mdis Build using MEN MDIS framework

	When using the ElinOS and MEN MDIS environments, the driver can be built
	and	installed just by adding the MDIS makefile \n

	\$(MEN_LIN_DIR)/DRIVERS/CHAMELEON/driver.mak \n

    to the
	\$(ELINOS_PROJECT)/src/mdis/Makefile under the ALL_NATIVE_DRIVERS entry.
	The drivers object file is installed properly into the ElinOS environment
	(path: \$(ELINOS_PROJECT)/kernel.rootfs/lib/modules/\$(LINUX_VERSION)/misc).    \n

    If you are using the MEN MDIS Configuration Wizard (MDISWIZ) the driver is
    automatically built if a device is configured which needs it.

	\n \section parameter Parameter

    The driver's single parameter usePciIrq tells the it to use the
    interrupt number from the PCI config table instead of taking it from the
    FPGA's built-in table. It is set in most cases. \n

    Example: modprobe men_lx_chameleon usePciIrq=1
*/


/** \page dummy
  \menimages
*/



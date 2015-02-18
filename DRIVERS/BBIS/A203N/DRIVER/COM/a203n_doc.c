/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  a203n_doc.c
 *
 *      \author  dieter.pfeuffer@men.de
 *        $Date: 2005/08/03 12:03:51 $
 *    $Revision: 1.1 $
 *
 *      \brief   User documentation for A203N board driver
 *
 *     Required: -
 *
 *     \switches -
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: a203n_doc.c,v $
 * Revision 1.1  2005/08/03 12:03:51  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*! \mainpage
    This is the documentation of the BBIS driver for the A203N board.


    The A203N is an M-Module carrier board for universal I/O on the VMEbus.
    The A203N is a VME64 master and slave card and supports four D16/D32 M-Modules.
    Additional, the A203N provides onboard trigger capabilities and an onboard
	temperature sensor.

    Please refer to the A203N hardware documentation for further details about
	the A203N hardware.

    <b>Slot assignment:</b>

    Slot assignment:

    \code
     slot   |  device
    --------+--------------------------------
          0 |  M-Module slot 0
          1 |  M-Module slot 1
          2 |  M-Module slot 2
          3 |  M-Module slot 3
     0x1000 |  Trigger device for TRIGA/TRIGB lines
     0x1001 |  Temperature sensor (LM75 on I2C interface)\endcode


    <b>Notes:</b>
    - Note: The DMA features of the A203N are not supported!

    \n
    \section Variants Variants
    In order to support different CPU boards and to achieve maximum performance,
	the A203N driver can be built in some variants at compilation time:

    \code
    Driver              Variant Description
    --------            --------------------------------
    Standard            Non-swapped register access,
    _sw                 Swapped register access
    \endcode

    Here are some combinations of MEN CPU and carrier boards together with the
    required variants:

    \code
    CPU                     Driver Variant
    ----------------        --------------
    MEN D2 (Pentium CPCI)   Standard
    MEN F1 (PPC CPCI)       _sw
    \endcode

    \n \section interrupts Interrupts
    The driver supports INTA interrupts from the M-Module. The M-Module’s interrupt
    can be enabled/disabled through the M_MK_IRQ_ENABLE SetStat code.

    \n \section descriptor_entries A203N Descriptor Entries

    The bbis initialization routine A203N_Init() decodes the following entries
    ("keys") in addition to the general descriptor keys:

    <table border="0">
    <tr><td><b>Descriptor entry</b></td>
        <td><b>Description</b></td>
        <td><b>Values</b></td>
    </tr>
    <tr><td>VME16_ADDR_MMOD08</td>
        <td>A16 base addr on VMEbus for A08 M-Mod space</td>
        <td>0x0XXX..0xfXXX</td>
    </tr>
    <tr><td>VME24_ADDR_MMOD08</td>
        <td>A24 base addr on VMEbus for A08 M-Mod space</td>
        <td>0x000XXX..0xfffXXX</td>
    </tr>
    <tr><td>VME32_ADDR_MMOD24</td>
        <td>A32 base addr on VMEbus for A24 M-Mod space</td>
        <td>0x0XXXXXXX..0xfXXXXXXX</td>
    </tr>
    <tr><td>IRQ_VECTOR</td>
        <td>Irq vectors for module 0..3</td>
        <td>\<vec0\>,\<vec1\>,\<vec2\>,\<vec3\>\n
			where \<vecX\>=0x00..0xff</td>
    </tr>
    <tr><td>IRQ_LEVEL</td>
        <td>Irq levels for module 0..3</td>
        <td>\<lev0\>,\<lev1\>,\<lev2\>,\<lev3\>\n
			where \<levX\>=1..7</td>
    </tr>
    <tr><td>IRQ_PRIORITY</td>
        <td>Irq priorities for module 0..3</td>
        <td>\<pri0\>,\<pri1\>,\<pri2\>,\<pri3\>\n
			where \<priX\>=0x00..0xff\n
			Default: 0,0,0,0</td>
    </tr>
    <tr><td>PNP_FORCE_FOUND</td>
        <td>"Force found" for module slot 0..3:\n
		    0: only a recognized M-Module will be reported\n
            1: an unknown M-Module will be reported regardless if there is one plugged</td>
        <td>\<fofo0\>,\<fofo1\>,\<fofo2\>,\<fofo3\>\n
			where \<fofoX\>=0 or 1\n
			Default: 0,0,0,0</td>
    </tr>
    </table>

    <b>A08 M-Module support (always)</b>\n
    VME16_ADDR_MMOD08 or VME24_ADDR_MMOD08 must be specified to get access to
	the board's control register and to support the A08 M-Module address space.\n
    Notes:\n
    - VME16_ADDR_MMOD08 and VME24_ADDR_MMOD08 cannot be specified together.
    - If VME16_ADDR_MMOD08 is specified, the "short non-privileged access mode
	  (0x29)" will be used on the VMEbus.
    - If VME24_ADDR_MMOD08 is specified, the "Standard non-privileged data
	  access mode (0x39)" will be used on the VMEbus.

    <b>A24 M-Module support (optional)</b>\n
    The bbis supports access to the A24 M-Module address space if VME32_ADDR_MMOD24
    is specified.\n
    Notes:\n
	- The "Extended non-privileged data access mode (0x09)" will be used on the VMEbus.

    \n \subsection a203n_min   Minimum descriptor
    a203n_min.dsc, a203n_sw_min.dsc (see Examples section)\n
    Demonstrates the minimum set of options necessary for using the driver.

    \n \subsection a203n_max   Maximum descriptor
    a203n_max.dsc, a203n_sw_max.dsc (see Examples section)\n
    Shows all possible configuration options for this driver.

    \n<b>Getstat/Setstat codes</b>\n
	Refer to the A203N_GetStat() and  A203N_SetStat() routine for the supported
	Getstat/Setstat codes.

    \n<b>Windows specifics</b>\n
	<a href="../../../A201/DOC/readme_win.txt">Windows specifics for VMEbus carrier boards</a>
*/

/** \example a203n_min.dsc */
/** \example a203n_max.dsc */
/** \example a203n_sw_min.dsc */
/** \example a203n_sw_max.dsc */

/*! \page dummy
  \menimages
*/

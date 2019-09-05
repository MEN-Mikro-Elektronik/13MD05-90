/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_api.c
 *
 *      \author  klaus.popp@men.de
 *
 *  	 \brief  API functions and documentation to access VME for Linux driver
 *
 *     Switches: -
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/***************************************************************************/
/*! \mainpage

  \anchor VME4L_API

  This document describes the features of MEN's VME for Linux (VME4L)
  API as well as most of the features of the underlying device
  driver. It operates on top of the \em vme4l-core device driver.

  Currently supported bridges are
  - PLDZ002 (MEN A13/A15)
  - PLDA12 (MEN A12/B11)
  - TSI148 (MEN A17/A19/A20)

  This interface is designed to support other bridges (such as Tundra Universe
  II) as well, but no bridge driver exists yet.

  The VME4L_API is to be used in Linux userland only. Separate APIs exist
  for Linux kernel mode drivers or RTAI applications (see header file
  \em men_vme_kernelif.h and the functions within \em vme4l-core.c)

  VME4L has the following features:
  - \ref vme4laccvme
  - \ref vme4lirq
  - \ref vme4lsys
  - \ref vme4lipc

  \section vme4lgeneral General Usage

  Before VME4L functions can be used, a path has to be opened via
  VME4L_Open() to the VME space you wish to access. For some functions
  (such as the functions for interrupt handling), it is not important
  which space you have opened.


  \code
  int spaceFd;

  spaceFd = VME4L_Open( VME4L_SPC_A24_D16 );

  if( spaceFd < 0 )
      exit(1);

  // Access VME functions here

  VME4L_Close( spaceFd ); \endcode

  The value returned by VME4L_Open() has to be passed to all further
  VME4L functions.

  When you have finished accessing the VMEbus, you should close the path
  by calling VME4L_Close(). If your program exits without closing the path(s)
  before, the Linux OS will automatically close the paths for you.

  \subsection vme4lret VME4L Return values

  All VME4L functions return a value >= 0 on success or -1 on failure.
  On failure, the global variable \em errno is set to a Linux standard
  error code. The VME4L_API does not use special error codes.

  \subsection vme4lhdr VME4L header files and libraries

  Any user program that uses VME4L_API must include at least those
  header files:

  \code
  #include <stdint.h>
  #include <errno.h>
  #include <unistd.h>
  #include <MEN/vme4l.h>
  #include <MEN/vme4l_api.h> \endcode

  The application must be linked against the library \c vme4l_api.so or
  \c vme4l_api.a

*/

/***************************************************************************/
/*! \page vme4laccvme Accessing VME spaces

  To access other boards on the VMEbus, you can either
  - \ref vme4lmap
  - \ref vme4lapiacc


  \section vme4lmap Map parts of VME space into user space

  You can map parts of the VME address space into your applications space.
  This mapped address can then be accessed by normal pointer accesses. The
  following maps VME standard space addresses 0x10000..0x107FF into the
  application space and writes 0xAAAA to VME address 0x10004:

  \code
  int spaceFd;
  u16_t *vmeP;

  spaceFd = VME4L_Open( VME4L_SPC_A24_D16 );
  if( spaceFd < 0 ) exit(1);

  if( VME4L_Map( spaceFd, 0x10000, 0x800, &vmeP) < 0 ) exit(1);

  vmeP[2] = 0xaaaa; \endcode

  This method is the fastest way to access VME space, since it does not
  introduce any overhead to switch context to kernel mode (as it would
  when using the VME4L_Read() and VME4L_Write() functions).

  You should do VME4L_UnMap() when you no longer need to access this region.
  This is automatically done when you perform VME4L_Close() or when the
  program exits.

  Note that with VME4L_Map() you cannot access the block transfer spaces,
  you must use the VME4L_Read() or VME4L_Write() API functions.

  Another restriction of this mode is that you will not immediately
  know whether your VME access was OK or if a VME bus error has
  occurred. On reads from VME, the data received by the program is
  just garbage. However, you can install a signal for bus errors, see
  \ref vme4lberr.

  \subsection vme4lmapswp Swapping on little-endian systems for mapped regions

  On X86 systems, a problem can arise due to the different endian mode
  of the X86 CPU (little-endian) and the VMEbus. For example if you
  want to access a byte on VME address 0x10000 in a 16-bit VME space,
  your program must invert the low-order address bit to the virtual
  address:

  \code
  int spaceFd;
  u8_t *vmeP;

  // Error checking omitted in this example
  spaceFd = VME4L_Open( VME4L_SPC_A24_D16 );

  VME4L_Map( spaceFd, 0x10000, 0x800, &vmeP);

  vmeP[1] = 0xaa; // Would access VME address 0x10000
  vmeP[0] = 0x55; // Would access VME address 0x10001 \endcode

  You can however, use the hardware byte-swapping circuit of the VME bridge
  in this case. The hardware byte-swapping circuit would correct the address
  swapping in the above case:

  \code
  int spaceFd;
  u8_t *vmeP;

  // Error checking omitted in this example
  spaceFd = VME4L_Open( VME4L_SPC_A24_D16 );

  VME4L_SwapModeSet( spaceFd, VME4L_HW_SWAP1 );
  VME4L_Map( spaceFd, 0x10000, 0x800, &vmeP);

  vmeP[0] = 0xaa; // Would access VME address 0x10000
  vmeP[1] = 0x55; // Would access VME address 0x10001 \endcode

  N.B.: The byte-swapping circuit will also swap byte lanes of
  all 16- and 32-bit accesses to the VMEbus. See \ref vme4lapiswp .

  Note that swapping modes cannot be set (and are not required) on
  PowerPC.

  \tsi148 TSI148 VME bridge does not support hardware swapping! User
  application has to care for swapping by its own!


  \section vme4lapiacc Use API functions to exchange data with VME

  Instead of VME4L_Map(), you can also use VME4L_Read() or VME4L_Write()
  to transfer data between the VME space and user space.

  Using the API functions is necessary whenever you need to access
  VME block transfer spaces (spaces ending with \c _BLT).

  In addition, these calls report whether a VMEbus error has
  occurred or not.

  But for non-BLT spaces these functions are relatively slow compared
  to accesses done from regions mapped by VME4L_Map() due to
  additional bus error checking after each access.

  The following example reads a data block from VME standard space,
  address 0x20000 to the user buffer using programmed I/O (i.e. non-DMA):

  \code
  uint8_t buf[0x800];
  int spaceFd;

  // Error checking omitted in this example
  spaceFd = VME4L_Open( VME4L_SPC_A24_D16 );

  // Transfer width=2 bytes
  VME4L_Read( spaceFd, 0x20000, 2, 0x800, buf, VME4L_RW_NOFLAGS); \endcode


  \subsection vme4lapiswp Swapping on little-endian systems for API functions

  On X86 systems, the same endian problems arise as described above
  for regions mapped via VME4L_Map().

  However, you can use either
  - the hardware byte-swapping circuit of the VME bridge or (#VME4L_HW_SWAP1)
  - software address swapping (#VME4L_SW_ADR_SWAP).

  Pass these flags to VME4L_SwapModeSet().


  \c VME4L_SW_ADR_SWAP: Corrects the address offsets for byte accesses.

  \c VME4L_HW_SWAP1: Same effect as \c VME4L_SW_ADR_SWAP but also swaps
  byte-lanes of all 16- and 32-bit accesses.

  The \c VME4L_HW_SWAP1 flag is also used for DMA transfers; the
  \c VME4L_SW_ADR_SWAP however has no effect on DMA, since DMA always
  accesses the VME space with its maximum possible witdth.



  \section vme4la32 Restrictions in VME extended (A32) space

  Since the VME A32 space occupies 4GB, only a part of this space
  is accessible by the CPU at any given time.

  Functions VME4L_Read()/VME4L_Write() and VME4L_Map() program a
  hardware address window according to the specified parameters and
  then lock this hardware address window. Locking a hardware address
  window does not mean that it is reserved and therefore cannot be
  used by other processes. It simply means the hardware address window
  cannot be altered by other processes. Therefore, multiple processes
  can map different address ranges in the same hardware address window
  as long as the current settings of that hardware address window are
  acceptable to all the processes using it.

  If none of the already locked hardware address windows is usable for the
  specified parameters and if no more unlocked hardware address windows are
  available, the functions return with \em errno set to \c EBUSY.

  \pldz002 The A32 windows map 512MB of VME space into the PCI space,
  therefore you can address different VME devices in A32 space as long
  as the upper three bits of each VME address are identical.


  \section vme4lberr Bus error handling during master accesses

  When an application calls VME4L_Read() or VME4L_Write(), bus
  errors are reported by return value and \em errno is set to \c EIO.

  \tsi148 Since posted writes cannot be disabled, it's not guaranted to detect
  bus errors at VME4L_Write().

  However, whenever direct accesses through mapped VME space (either in
  userland or kernel) are performed, the calling application will not
  know that a bus error has ocurred.

  The userland program, however can check if a bus error has occurred
  using VME4L_BusErrorGet() or it can install a signal (using
  VME4L_SigInstall()) that is sent on VME bus errors.

  <b>But N.B.</b>: The bus error information is global for the
  entire VME bridge, you will not know which process or which kernel
  module has produced the bus error!

  More than one process can install a signal for bus errors. If any
  bus error occurs, all registered processes will be informed. The bus error
  signal will be suppressed when the bus error was caused by a VME4L_Read()
  or VME4L_Write() call (in PIO or DMA mode).


  \section vme4lpostwr Posted Write Mode

  Writes to VMEbus can be performed in two modes:
  - Delayed write mode (default): PCI access will wait for VME transfer to
    finish.
  - Posted write mode: PCI access will finish immediately after passing data
    to the VME bridge.

  This mode influences only access from memory-mapped regions (either
  in userland or kernel). VME4_Write() always disables posted writes
  temporarily.

  Posted write mode can be enabled or disabled by calling
  VME4L_PostedWriteModeSet().

  The posted write mode will speed up writes to VMEbus, but this can
  be dangerous, since you don't know exactly when the actual VME
  transfer is performed.

  The write mode is a global setting for all address spaces of the VME
  bridge, so enabling posted write mode can also influence the
  behavior of kernel drivers that access the VMEbus.

  <b>Do not enable posted write mode unless you are absolutely sure
  what you are doing!</b>

  \tsi148 Posted writes cannot be disabled at TSI148 (see chapter "PCI
  Transactions" in "Tsi148 PCI/X-to-VME Bus Bridge User Manual").


  \section vme4lreq Controlling VME requester

  VMEbus request mode can be controlled by VME4L_RequesterModeSet().
  The requester can be set to Release-when-done or Release-on-Request.

*/


/***************************************************************************/
/*! \page vme4lirq VME interrupt handling in user space

  With VME4L, it is possible to handle interrupts from VMEbus in Linux
  userland. VMEbus interrupts are forwarded as Linux signals from the
  VME4L driver to the userland process.

  The handling depends on the interrupt release mode used for that
  interrupt:
  - <b>Release on register access (RORA)</b>\n
    In this mode, which is the default, the VME4L driver masks the
    corresponding interrupt level and sends the signal to the userland process.
    The userland process' signal handler must clear the interrupt by writing
    to a device-specific register and must then re-enable the interrupt level.
  - <b>Release on acknowledge (ROAK)</b>\n
    In this mode, the interrupt is implicitly cleared by the VMEbus IACK cycle.
    Therefore the VME4L driver will not mask this level before it sends the
    signal to the process.

  To install a signal for a VME interrupt, you call VME4L_SigInstall().

  You have to pass information on the interrupt release mode to the VME4L
  driver by setting or clearing the #VME4L_IRQ_ROAK flag in the \em flags
  parameter of VME4L_SigInstall().

  The VMEbus interrupt level is automatically enabled by
  VME4L_SigInstall(), when #VME4L_IRQ_ENBL is passed to the \em flags
  parameter, otherwise the level must be explicitly enabled by calling
  VME4L_IrqEnable().

  Example to install and handle VME interrupts:
  \code
  // Error checking omitted in this example
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdint.h>
  #include <signal.h>
  #include <errno.h>
  #include <unistd.h>
  #include <MEN/vme4l.h>
  #include <MEN/vme4l_api.h>

  int G_fd;
  volatile uint8_t *G_map;

  static void IrqSigHandler( int sigNum ) {
      G_map[0x80] = 0x20; // Service interrupt (device specific)

      VME4L_IrqEnable( G_fd, 3 ); // Re-enable interrupt level
  }

  int main( int argc, char *argv[] )
  {
      G_fd = VME4L_Open( VME4L_SPC_A24_D16 );


      VME4L_Map( G_fd, 0x10000, vmeOff + 0x800, (void **)&G_map );

	  // Instruct VME4L to send signal 33 on interrupt
	  signal( 33, IrqSigHandler );

	  // Install vector 121, level 3
	  VME4L_SigInstall( G_fd, 121, 3, 33, VME4L_IRQ_NOFLAGS );

	  // Prepare your device for interrupts...
	  ...

	  VME4L_IrqEnable( G_fd, 3 );       // enable interrupt level

	  // Let signals come in
	  while(1)
	      sleep(10);

  } \endcode

  Notes and Restrictions:
  - The same VME vector shall not be shared between userland process and
    kernel modules.
  - The same VME level shall not be shared between userland process and
    kernel modules (as it may result in long delays for the kernel
	modules' interrupt handler).
  - The same signal can be installed for the same process for
    different vectors.

*/

/***************************************************************************/
/*! \page vme4lsys Functions to control system controller functionality


  VME4L provides the following features for a system controller:

  - Control system controller functionality: VME4L_SysCtrlFunctionGet(),
    VME4L_SysCtrlFunctionSet()
  - Perform VME reset: VME4L_SysReset()
  - Arbiter functions: VME4L_ArbitrationTimeoutGet()

*/

/***************************************************************************/
/*! \page vme4lipc Interprocessor communication features


  VME4L provides the following features for interprocessor communication:

  - Interrupt generation on VMEbus: VME4L_IrqGenerate()
  - Special VME cycles: VME4L_RmwCycle(), VME4L_AOnlyCycle()
  - \ref vme4lshram
  - \ref vme4lmbox
  - \ref vme4llocmon
*/

/***************************************************************************/
/*! \page vme4lshram Shared RAM support

  VME4L supports shared RAM of the VME bridge. Shared RAM is accessible
  both from VME and from the local user space application.

  It depends on the underlying bridge wether the shared RAM is a
  dedicated memory or a part of the system RAM. Each slave window
  corresponds to an VME4L space (see list below).

  By default, none of these windows are enabled. User has to call
  VME4L_SlaveWindowCtrl() to set the VMEbus address and occupied size
  of the window.

  After the window has been enabled, user can map the shared RAM into
  its application space:

  \code
  // Error checking omitted in this example
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdint.h>
  #include <signal.h>
  #include <errno.h>
  #include <unistd.h>
  #include <MEN/vme4l.h>
  #include <MEN/vme4l_api.h>

  int main( int argc, char *argv[] )
  {
      uint8_t *map;
      int fd = VME4L_Open( VME4L_SPC_SLV3 );

      // enable slave window, set vmeAddress to 0x400000 and size to 1MB
      VME4L_SlaveWindowCtrl( fd, 0x400000, 0x100000 )

      // map slave window into application space
      // note: the vmeAddr argument of VME4L_Map() is the offset within
      // the shared RAM here!
      VME4L_Map( fd,
                  0,            // offset
                  0x100000,     // size
                  &map)
      map[0] = 0x55;            // write to first shared RAM cell

  }
  \endcode

  A slave window can be disabled by calling VME4L_SlaveWindowCtrl()
  with a size of zero.

  Further notes to slave windows:
  - More than one process can map the same window at the same time
  - The shared RAM area is marked non cacheable, so there is no extra
    flushing required by application

  \pldz002 The following assignment is used (only valid for PLD rev. >=10)

  \verbatim
  Space code       VME space  Local resource              Size
  --------------   ---------  --------------              -----------
  VME4L_SPC_SLV0   A16        Bridge control regs         4K
  VME4L_SPC_SLV1   A24        dedicated SRAM              64K..1M
  VME4L_SPC_SLV2   A32        dedicated SRAM              1M..256M
  VME4L_SPC_SLV3   A24        kernel memory               64K..1M
  VME4L_SPC_SLV4   A32        kernel memory               1M..256M
  \endverbatim

  - The VME address of each window must be aligned to its size (e.g.
    when size is 1MB, the VME address must be 1MB aligned)
  - VME4L_SPC_SLV1 and VME4L_SPC_SLV2 hit the same dedicated SRAM when
    accessed from VME.
  - VME4L_SPC_SLV3 and VME4L_SPC_SLV4 hit the same physical RAM when
    accessed from VME. This RAM is alloced from kernel memory and remains
    active until both windows are closed. The VME address or size of the
	window cannot be changed as long at is open. Please close it first.
  - Since the RAM for VME4L_SPC_SLV3 and VME4L_SPC_SLV4 will be allocated
    from kernel memory, Linux will limit the maximum size to 2MB typically.
	If this is not enough, you can only change this by recompiling
	the kernel with a different MAX_ORDER constant, see linux/mmzone.h:
	CONFIG_FORCE_MAX_ORDER for more info.
  - No HW swapping is provided by VME bridge
  - If your PLD revision is < 10=, VME4L_SPC_SLV3 and VME4L_SPC_SLV4 are
    not available, and the size of VME4L_SPC_SLV1/VME4L_SPC_SLV2 is fixed
    to 1MB/256MB respectively.


  \plda12 The following assignment is used:

  \verbatim
  Space code       VME space  Local resource              Size
  --------------   ---------  --------------              -----------
  VME4L_SPC_SLV1   A24        dedicated SRAM              1M
  \endverbatim

  - The VME address of the window must be aligned to 1MB
  - No HW swapping is provided by VME bridge

  \tsi148 The following assignment is used per default (settings can be
  adjusted in \ref vme4l-tsi148.h)

  \verbatim
  Space code       VME space  Local resource              Size
  --------------   ---------  --------------              -----------
  VME4L_SPC_SLV0   A16        TSI148 regs (GCSR)          4K
  VME4L_SPC_SLV3   A24        kernel memory               4K..16M
  VME4L_SPC_SLV4   A32        kernel memory               64K..4G
  VME4L_SPC_SLV5   A64        kernel memory               64K..16E
  \endverbatim

  - The VME address of each window must be aligned to its size (e.g.
    when size is 64KB, the VME address must be 64KB aligned).
  - Since the RAM will be allocated from kernel memory, Linux will limit the
    maximum size.
  - No HW swapping is provided by VME bridge.
*/

/***************************************************************************/
/*! \page vme4lmbox Mailbox support

  VME4L supports up to 8 mailboxes. A mailbox is a register that can
  be read or written both from VME and from the local CPU.

  Upon read/write accesses from VME, an interrupt can be generated and
  VME4L can send a signal to the application process. The value
  of the mailbox can then be read or written by application.

  \code
  // Error checking omitted in this example
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdint.h>
  #include <signal.h>
  #include <errno.h>
  #include <unistd.h>
  #include <MEN/vme4l.h>
  #include <MEN/vme4l_api.h>

  int G_fd;
  volatile uint8_t *G_map;
  uint32_t G_mboxValue;

  static void IrqSigHandler( int sigNum )
  {
       if( sigNum == 33 )
           VME4L_MboxRead( G_fd, 2, &G_mboxValue );
  }

  int main( int argc, char *argv[] )
  {
      G_fd = VME4L_Open( VME4L_SPC_A24_D16 ); // space number don't care

      signal( 33, IrqSigHandler );

      // Instruct VME4L to send signal 33
      // when 3rd mailbox is written from VME
      VME4L_SigInstall( G_fd, VME4L_IRQVEC_MBOXWR(2),
                              VME4L_IRQLEV_MBOXWR(2), 33, VME4L_IRQ_ENBL );

      // Let signals come in
      while(1)
          sleep(10);

  } \endcode

  \warning Any handshaking between the sending CPU and the receiving CPU must
  be done at application level. The mailbox routines may loose data
  when the sending CPU writes faster to the mailbox than the receiving
  CPU is handling the interrupt and the corresponding signal.


  \pldz002 The VME bridge has 4 mailboxes, each 32 bit wide.

  \plda12 Mailboxes are not supported

  \tsi148 The VME bridge has 4 mailboxes. Interrupt is only generated if least
  significant byte of a mailbox register is written.

*/

/***************************************************************************/
/*! \page vme4llocmon Location monitor support

  VME4L supports VME location monitor(s).

  It provides direct access to the location monitor registers using
  VME4L_LocMonRegRead() and VME4L_LocMonRegWrite().

  This allows maximum flexibility, although you must know the register
  layout of the location monitor.

  To get informed about a location monitor hit, install a signal similar
  to mailbox signals:

  \code
      // install signal for second location monitor
      VME4L_SigInstall( G_fd, VME4L_IRQVEC_LOCMON(1),
                              VME4L_IRQLEV_LOCMON(1), 38, VME4L_IRQ_ENBL );

  \endcode

  \pldz002 Supports two location monitors.

  The register numbers to pass to VME4L_LocMonRegRead() and
  VME4L_LocMonRegWrite() are:

  \verbatim
  reg    PLD register
  ----   -------------
    0	 Location monitor 0 control
    1	 Location monitor 1 control
    2	 Location monitor 0 address
    3	 Location monitor 1 address
  \endverbatim
  When reg 0 or 1 is written, the interrupt enable bit supplied
  by user is ignored.

  See HW manual for more details

  \plda12 Locations monitors are not supported

  \tsi148 Supports four locations (each location is 8 bytes). Address bits 3
  and 4 are used to define the specific location.

  The register numbers to pass to VME4L_LocMonRegRead() and
  VME4L_LocMonRegWrite() are:

  \verbatim
  reg    TSI148 register
  ----   -------------
    0	 LMBAU (Location Monitor Base Address Upper)
    1	 LMBAL (Location Monitor Base Address Lower)
    2	 LMAT (Location Monitor Attribute Register)
  \endverbatim

  See chapter 2.3.6.1 Location Monitor in TSI148 User Manual for more details

*/

/*!
 * \page vme4ldummy MEN logo
 * \menimages
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <MEN/vme4l.h>


/*--------------------------------+
|  DEFINES                        |
+--------------------------------*/

/*--------------------------------+
|  PROTOTYPES                     |
+--------------------------------*/

static const char *G_devNameTbl[] = {
	/* numerical Value for test tool: 	  */
 	 "a16d16",		/*  0 */
	 "a24d64_blt",	/*  1 */
	 "a16d32",		/* 	2 */
	 "a16d32_blt",	/* 	3 */
	 "a24d16",		/* 	4 */
	 "a24d16_blt",	/* 	5 */
	 "a24d32",		/* 	6 */
	 "a24d32_blt",	/* 	7 */
	 "a32d32",		/* 	8 */
	 "a32d32_blt",	/* 	9 */
	 "a32d64_blt",	/* 10 */
	 "slave0", 		/* 11 VME4L_SPC_SLV0  A16  Bridge ctrl regs       4K  	*/
	 "slave1", 		/* 12 VME4L_SPC_SLV1  A24  dedicated SRAM    64K..1M  	*/
	 "slave2", 		/* 13 VME4L_SPC_SLV2  A32  dedicated SRAM   1M..256M  	*/
	 "slave3", 		/* 14 VME4L_SPC_SLV3  A24  kernel memory     64K..1M 	*/
	 "slave4", 		/* 15 VME4L_SPC_SLV4  A32  kernel memory     1M..256M	*/
	 "slave5", 		/* 16 */
	 "slave6", 		/* 17 */
	 "slave7", 		/* 18 */
	 "master0",		/* 19 */
	 "master1",		/* 20 */
	 "master2",		/* 21 */
	 "master3",		/* 22 */
	 "master4",		/* 23 */
	 "master5",		/* 24 */
	 "master6",		/* 25 */
	 "master7",		/* 26 */
	 "a64d32",		/* 27 */
	 "a64_2evme",	/* 28 */
	 "a64_2esst",	/* 29 */
	 "cr_csr"		/* 30 */
};
#define DEVNAME_TBL_SIZE (sizeof(G_devNameTbl)/sizeof(char *))

/**********************************************************************/
/** Open path to specified VME space
 *
 * A single process can have multiple VME spaces open at the same time.
 *
 * \plda12 Only the VME spaces #VME4L_SPC_A16_D16, #VME4L_SPC_A24_D16 and
 *         #VME4L_SPC_SLV1 are available
 *
 * \pldz002 Supports #VME4L_SPC_A16_D16, #VME4L_SPC_A16_D32,
 *			#VME4L_SPC_A24_D16, #VME4L_SPC_A24_D16_BLT, #VME4L_SPC_A24_D32,
 *			#VME4L_SPC_A24_D32_BLT, #VME4L_SPC_A32_D32,
 *			#VME4L_SPC_A32_D32_BLT, #VME4L_SPC_A32_D64_BLT and
 *			#VME4L_SPC_SLV0 to #VME4L_SPC_SLV4.
 *
 * \tsi148 Supports #VME4L_SPC_A16_D16, #VME4L_SPC_A16_D32,
 *			#VME4L_SPC_A24_D16, #VME4L_SPC_A24_D16_BLT, #VME4L_SPC_A24_D32,
 *			#VME4L_SPC_A24_D32_BLT, #VME4L_SPC_A32_D32,
 *			#VME4L_SPC_A32_D32_BLT, #VME4L_SPC_A32_D64_BLT, #VME4L_SPC_A64_D32,
 *			#VME4L_SPC_A64_2EVME, #VME4L_SPC_A64_2ESST,
 *			#VME4L_SPC_SLV0 to #VME4L_SPC_SLV7 and
 *			#VME4L_SPC_MST0 to #VME4L_SPC_MST7. VME space settings can be
 *			adjusted in \ref vme4l-tsi148.h.
 *
 * \param space		\IN Requested VME space, one of #VME4L_SPACE
 *
 * \return File descriptor on success, or -1 on error and \em errno is set to\n
 * - \c ENODEV: Space not supported
 *
 */
int VME4L_Open( VME4L_SPACE space )
{
	int fd;
	char buf[128];

	if( space >= DEVNAME_TBL_SIZE ){
		errno = EINVAL;
		return -1;
	}

	sprintf(buf, "/dev/vme4l_%s", G_devNameTbl[(int)space] );
	fd = open( buf, O_RDWR );

	return fd;
}

/**********************************************************************/
/** Close path to VME space
 *
 * \param spaceFd	\IN File descriptor for VME space,
 *						returned by VME4L_Open()
 *
 * \return 0 on success, or -1 on error and \em errno is set
 *
 * \sa VME4L_Open
 */
int VME4L_Close(int spaceFd)
{
	return close( spaceFd );
}


/**********************************************************************/
/** Returns a symbolic name for numeric space
 *
 * e.g. returns \c a16d16 when \a space is \c VME4L_SPC_A16_D16
 *
 * \param space		\IN Requested VME space, one of #VME4L_SPACE
 * \return String or NULL if space invalid
 *
 * \sa VME4L_Open
 */
const char *VME4L_SpaceName( VME4L_SPACE space )
{
	if( space >= DEVNAME_TBL_SIZE )
		return NULL;
	return G_devNameTbl[(int)space];
}


/**********************************************************************/
/** Set swapping mode for this file descriptor (space)
 *
 * Set the swapping mode used to access the VME space opened by
 * \a spaceFd. This function changes only the swapping mode of the
 * passed file descriptor and has no effect on the swapping mode of other
 * processes.
 *
 * This function should be used on little-endian systems (X86) only.
 * On PowerPC, any \a swapMode other than 0 results in an error.
 *
 * \tsi148 VME bridge does not support hardware swapping! User
 * application has to care for swapping by its own!
 *
 * \param spaceFd 	\IN File descriptor for VME space,
 *						returned by VME4L_Open()
 * \param swapMode	\IN Swapping mode to use, see \ref VME4L_SWAPMODE
 *
 * \return 	0 on success, or -1 on error\n
 *			- \c EINVAL: Bad parameter
 *
 * \sa VME4L_Open, VME4L_Read, VME4L_Map, \ref vme4lapiacc
 *
 */
int VME4L_SwapModeSet( int spaceFd, int swapMode)
{
	return ioctl( spaceFd, VME4L_IO_SWAP_MODE_SET, swapMode );
}

/**********************************************************************/
/** Set VMEbus address modifiers for this file descriptor (space)
 *
 * Allows to alter the predefined master VME address modifiers of a
 * \a spaceFd. This function changes only the AMs of the
 * passed file descriptor and has no effect on the AMs of other
 * processes.
 *
\verbatim
    0 (00b) = non-privileged data access (default)
    1 (01b) = non-privileged program access
    2 (10b) = supervisory data access
    3 (11b) = supervisory program access
\endverbatim
 *
 * On A16 Space only modes 0 and >0 are used.
 * On BLT spaces only the supervisor bit is considered.
 *
 * \param spaceFd 		\IN File descriptor for VME space,
 *							returned by VME4L_Open()
 * \param addrModifier	\IN new address modifier for this space to set, 0-3
 *
 * \return 	0 on success, or -1 on error\n
 *			- \c EINVAL: Bad parameter
 *
 * \sa VME4L_Open, VME4L_Read, VME4L_Map, \ref vme4lapiacc
 *
 */
int VME4L_AddrModifierSet( int spaceFd, char addrModifier)
{
	int retval = ioctl( spaceFd, VME4L_IO_ADDR_MOD_SET, addrModifier );
	return retval;
}


/**********************************************************************/
/** Get VMEbus address modifiers for this file descriptor (space)
 *
 * Counterpart to VME4L_AddrModifierSet
 *
 * \param spaceFd 		\IN File descriptor for VME space,
 *							returned by VME4L_Open()
 *
 * \return 	address modifier on success, or -1 on error\n
 *			- \c EINVAL: Bad parameter
 *
 * \sa VME4L_AddrModifierSet, \ref vme4lapiacc
 *
 */
int VME4L_AddrModifierGet( int spaceFd )
{
	return ioctl( spaceFd, VME4L_IO_ADDR_MOD_GET, 0 );
}

/**********************************************************************/
/** Get geographical address (slot nr.) of this board
 *
 * Counterpart to VME4L_AddrModifierSet
 *
 * \param spaceFd 		\IN File descriptor for VME space,
 *					returned by VME4L_Open()
 *
 * \return 	slot number on success, or -1 on error\n
 *		- \c EINVAL: Bad parameter
 *
 */
int VME4L_GeoAddrGet( int spaceFd )
{
	return ioctl( spaceFd, VME4L_IO_GEO_ADDR_GET, 0 );
}

/**********************************************************************/
/** Set VME bus requester level l address (slot nr.) of this board
 *
 * Counterpart to VME4L_AddrModifierSet
 *
 * \param spaceFd 		\IN File descriptor for VME space,
 *					returned by VME4L_Open()
 *
 * \param level  		\IN requester level: 0,1,2,3 (default)
 *
 * \return 	0 on success, or -1 on error\n
 *		- \c EINVAL: Bad parameter
 *
 */
int VME4L_RequesterLevelSet( int spaceFd, char level )
{
	return ioctl( spaceFd, VME4L_IO_REQUESTER_LVL_SET, level );
}

/**********************************************************************/
/** Get VME bus requester level l address (slot nr.) of this board
 *
 * Counterpart to VME4L_AddrModifierSet
 *
 * \param spaceFd 		\IN File descriptor for VME space,
 *					returned by VME4L_Open()
 *
 * \return 	requester level on success, or -1 on error\n
 *		- \c EINVAL: Bad parameter
 *
 */
int VME4L_RequesterLevelGet( int spaceFd )
{
	return ioctl( spaceFd, VME4L_IO_REQUESTER_LVL_GET, 0 );
}

/**********************************************************************/
/** Read data block from VME
 *
 * This transfers a data block from VME into the user's buffer.
 *
 * Data is automatically transferred by DMA engine if a BLT (incl. 2EVME/2ESST)
 * space has been selected or if the #VME4L_RW_USE_SGL_DMA flag was specified.
 *
 * \plda12 DMA/BLT mode is not available.
 *
 * \pldz002 For BLT spaces, \a dataP must be aligned to 4 bytes and
 * 			\a vmeAddr must be aligned to 4 bytes for 16-/32-bit wide space
 *			or 8-byte aligned for 64-bit spaces.\n
 *          For A32 spaces, the 3 MSBs of the VME address must be identical
 *          to all other A32 spaces currently in use.
 *
 * \tsi148 	The #VME4L_RW_USE_SGL_DMA flag is not supported. Non-BLT spaces
 *          must be transferred in PIO mode.\n
 *
 * \param spaceFd 	\IN  File descriptor for VME space,
 *						 returned by VME4L_Open()
 * \param vmeAddr	\IN  Start address within VME space. Must be aligned to
 *						 \a accWidth.
 * \param accWidth 	\IN  1 = Use 8-bit access, 2 = Use 16-bit access,
 *						 4 = use 32-bit access.\n
 *						 Parameter is ignored when DMA is used.
 * \param size		\IN  Number of bytes to transfer. Must be a multiple
 *						 of \a accWidth
 * \param dataP		\IN  Pointer to buffer to receive data from VMEbus
 * \param *dataP	\OUT Received data from VMEbus
 * \param flags		\IN  See \ref VME4L_RWFLAGS.
 *
 * \return 	Number of bytes transferred or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EIO: 	 A VME bus error occurred
 *			- \c EBUSY:  The requested address range is currently not available
 *			- \c EINVAL: Bad parameter
 *			- \c ETIME:  DMA controller timeout (hardware problem)
 *
 * \sa VME4L_Open, VME4L_Write, VME4L_Map, VME4L_SwapModeSet, \ref vme4lapiacc
 *
 */
int VME4L_Read(
	int spaceFd,
	vmeaddr_t vmeAddr,
	int accWidth,
	size_t size,
	void *dataP,
	int flags )
{
	VME4L_RW_BLOCK blk;

	blk.vmeAddr 		= vmeAddr;
	blk.accWidth		= accWidth;
	blk.direction		= 0;	/* read */
	blk.size			= size;
	blk.dataP			= dataP;
	blk.flags			= flags;

	return ioctl( spaceFd, VME4L_IO_RW_BLOCK, &blk );
}

/**********************************************************************/
/** Write data block to VME
 *
 * This transfers a data block from the user's buffer to VME.
 *
 * Data is automatically transferred by DMA engine if a BLT (incl. 2EVME/2ESST)
 * space has been selected or if the #VME4L_RW_USE_SGL_DMA flag was specified.
 *
 * \plda12 DMA/BLT mode is not available.
 *
 * \pldz002 For BLT spaces, \a dataP must be aligned to 4 bytes and
 * 			\a vmeAddr must be aligned to 4 bytes for 16-/32-bit wide space
 *			or 8-byte aligned for 64-bit spaces.\n
 *          For A32 spaces, the 3 MSBs of the VME address must be identical
 *          to all other A32 spaces currently in use.\n
 *			Always uses delayed write accesses, even when posted write
 *			accesses have been enabled by VME4L_PostedWriteModeSet().
 *
 * \tsi148 	The #VME4L_RW_USE_SGL_DMA flag is not supported. Non-BLT spaces
 *          must be transferred in PIO mode.\n
 *
 * \param spaceFd 	\IN  File descriptor for VME space,
 *						 returned by VME4L_Open()
 * \param vmeAddr	\IN  Start address within VME space. Must be aligned to
 *						 \a accWidth.
 * \param accWidth 	\IN  1 = Use 8-bit access, 2 = Use 16-bit access,
 *						 4 = Use 32-bit access. \n
 *						 Parameter is ignored when DMA is used.
 * \param size		\IN  Number of bytes to transfer. Must be a multiple
 *						 of \a accWidth.
 * \param dataP		\IN  Pointer to buffer with data to write to VMEbus
 * \param flags		\IN  See \ref VME4L_RWFLAGS
 *
 * \return 	Number of bytes transferred or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EIO: 	 A VME bus error occurred
 *			- \c EBUSY:  The requested address range is currently not available
 *			- \c EINVAL: Bad parameter
 *			- \c ETIME:  DMA controller timeout (HW problem)
 *
 * \sa VME4L_Open, VME4L_Read, VME4L_Map, VME4L_SwapModeSet, \ref vme4lapiacc
 *
 */
int VME4L_Write(
	int spaceFd,
	vmeaddr_t vmeAddr,
	int accWidth,
	size_t size,
	void *dataP,
	int flags )
{
	VME4L_RW_BLOCK blk;

	blk.vmeAddr 		= vmeAddr;
	blk.accWidth		= accWidth;
	blk.direction		= 1;	/* write */
	blk.size			= size;
	blk.dataP			= dataP;
	blk.flags			= flags;

	return ioctl( spaceFd, VME4L_IO_RW_BLOCK, &blk );
}

/**********************************************************************/
/** Map VMEbus address space
 *
 * The function maps the VMEbus address space starting at address \a vmeAddr
 * with the size of \a size bytes into user address space and stores the
 * corresponding user address into \a mappedAddrP.
 *
 * Note that \a vmeAddr <b>must be aligned to an MMU page</b> (typically 4K)
 * and addresses > 0xFFFFFFFF are not supported, yet.
 *
 * The VMEbus address remains mapped until VME4L_UnMap() is called or
 * \a spaceFd is closed.
 *
 * The mapped region uses posted write accesses if this has been
 * enabled by VME4L_PostedWriteModeSet().
 *
 * \param spaceFd 	\IN  File descriptor for VME space,
 *						 returned by VME4L_Open()
 * \param vmeAddr	\IN  Start address within VME space (aligned to MMU page).
 *						 For VME slave windows, the offset within the
 *						 shared RAM.
 * \param size		\IN  Number of bytes to map
 * \param mappedAddrP  \IN Pointer to variable that receives the user address
 * \param *mappedAddrP \OUT User address to access the mapped VMEbus address
 *
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EBUSY: The requested address range is currently not available
 *			- \c EINVAL: Bad parameter
 *
 * \sa VME4L_Write, VME4L_Read, VME4L_UnMap, VME4L_SwapModeSet, \ref vme4lmap
 */
int VME4L_Map(
	int spaceFd,
	vmeaddr_t vmeAddr,
	size_t size,
	void **mappedAddrP)
{
	void *vaddr;

	vaddr = mmap( NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, spaceFd,
				  vmeAddr );
	if( vaddr == MAP_FAILED )
		return -1;
	*mappedAddrP = vaddr;

	return 0;
}

/**********************************************************************/
/** Unmap VMEbus address space
 *
 * The function unmaps a mapped VMEbus address space previously mapped
 * by VME4L_Map().
 *
 * \param spaceFd 	\IN  File descriptor for VME space,
 *						 returned by VME4L_Open()
 * \param mappedAddr \IN Address returned by VME4L_Map()
 * \param size		\IN  Number of bytes to unmap. Shall be the same size
 *						 as passed to VME4L_Map()
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set.
 *
 * \sa VME4L_Map, \ref vme4lmap
 *
 */
int VME4L_UnMap(
	int spaceFd,
	void *mappedAddr,
	size_t size)
{
	return munmap( mappedAddr, size );
}




/*---- Interrupts ----*/

/**********************************************************************/
/** Install signal for VME interrupt or special interrupts
 *
 * The function installs a Linux signal for the specified VMEbus
 * interrupt vector \a vector at the specified \a level.  If the
 * specified interrupt occurs, the installed \a signal will be sent to
 * the corresponding process.
 *
 * It is advisable to use signal numbers above \c SIGRTMIN, since these
 * signals are queued.
 *
 * The function's behavior can be altered through the \a flags
 * parameter.
 *
 * <b>Special Interrupts</b>\n
 * Apart from a "normal" VMEbus interrupt, a signal can also be installed for
 * the following special interrupts:
 *  - VME4L_IRQVEC_BUSERR/VME4L_IRQLEV_BUSERR: Bus-Error Interrupt: \n
 *	  Occurs if the VMEbus line BUSERR is asserted. (see \ref vme4lberr)
 *  - VME4L_IRQLEV_ACFAIL/VME4L_IRQVEC_ACFAIL: VMEbus ACFAIL Interrupt: \n
 *	  Occurs if the VMEbus line ACFAIL is asserted.
 *  - VME4L_IRQLEV_SYSFAIL/VME4L_IRQVEC_SYSFAIL: VMEbus SYSFAIL Interrupt: \n
 *	  Occurs if the VMEbus line SYSFAIL is asserted.
 *  - Location Monitor (not yet supported)
 *  - Mailbox (not yet supported)
 *
 * \pldz002 SYSFAIL is not supported
 * \plda12 	SYSFAIL is not supported
 * \tsi148 	VME4L_IRQVEC_BUSERR/VME4L_IRQLEV_BUSERR: Occurs as well if PCI
 *			parity error is detected.
 *
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param vector	\IN  VMEbus interrupt vector (0..255) or special interrupt
 *                       vector (see \ref VME4L_IRQVEC)
 * \param level 	\IN  VMEbus interrupt level or special interrupt level
 *						 (see \ref VME4L_IRQLEV)
 * \param signal    \IN  Linux signal number
 * \param flags		\IN  Bitwise OR of the \ref VME4L_IRQFLAGS flags:\n
 *						 - #VME4L_IRQ_ROAK: Interrupt is released on IACK
 *						 - #VME4L_IRQ_ENBL: Enable VME IRQ level
 *
 * \return 	0=success <0=error\n
 *			In case of error, \em errno is set.
 *
 * \sa VME4L_Open, VME4L_SigUnInstall, \ref vme4lirq
 */
int VME4L_SigInstall( int fd, int vector, int level, int signal, int flags )
{
	VME4L_SIG_INSTALL2 blk;

	blk.vector			= vector;
	blk.level			= level;
	blk.signal			= signal;
	blk.flags			= flags;

	return ioctl( fd, VME4L_IO_SIG_INSTALL2, &blk );
}

/**********************************************************************/
/** Uninstall signal for VME interrupt or special interrupts
 *
 * Removes all signals installed by the calling process for that vector.
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param vector	\IN  VMEbus interrupt vector (0..255) or special interrupt
 *                       vector (see \ref VME4L_IRQVEC)
 *
 * \return 	0=success <0=error\n
 *			In case of error, \em errno is set.
 *
 * \sa VME4L_Open, VME4L_SigInstall, \ref vme4lirq
 */
int VME4L_SigUnInstall( int fd, int vector )
{
	return ioctl( fd, VME4L_IO_SIG_UNINSTALL, vector );
}

/***********************************************************************/
/** Enables the specified VME interrupt level.
 *
 *  Unmask the specified VME interrupt \a level. Note that the VME4L
 *  driver keeps track of the number of mask/unmask calls. The
 *  interrupt is unmasked only if the number of unmask calls is equal
 *  to the number of previous mask calls.
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param level		\IN  VME interupt level (see \ref VME4L_IRQLEV)
 *
 *
 * \return 	0=success <0=error\n
 *			In case of error, \em errno is set.
 *
 * \sa VME4L_IrqDisable, VME4L_SigInstall
 */
int VME4L_IrqEnable( int fd, int level )
{
	return ioctl( fd, VME4L_IO_IRQ_ENABLE2, level & 0x7f );
}

/***********************************************************************/
/** Disables the specified VME interrupt level.
 *
 *  The level is always masked immediately.
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param level		\IN  VME interupt level (see \ref VME4L_IRQLEV)
 *
 *
 * \return 	0=success <0=error\n
 *			In case of error, \em errno is set.
 *
 * \sa VME4L_IrqEnable, VME4L_SigInstall
 */
int VME4L_IrqDisable( int fd, int level )
{
	return ioctl( fd, VME4L_IO_IRQ_ENABLE2,
				  level | VME4L_IO_IRQ_ENABLE_DISABLE_MASK);
}



/*---- System controller ----*/

/***********************************************************************/
/** Check if system controller function has been enabled
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 *
 * \return 	0 if VME bridge is not system controller\n
 *			1 if VME bridge is system controller\n
 *			-1 on error and \em errno set\n
 *
 * \sa VME4L_SysCtrlFunctionSet
 */
int VME4L_SysCtrlFunctionGet( int fd )
{
	return ioctl( fd, VME4L_IO_SYS_CTRL_FUNCTION_GET, 0);
}

/***********************************************************************/
/** Enable/Disable system controller function
 *
 *  Normally, the VME bridge detects automatically whether it is the
 *  system controller. This function can be used to overwrite this
 *  setting.
 *
 * \tsi148 	Not supported by TSI148 VME bridge.
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param state		\IN  Controls system controller function
 *						 (0 = disable,1 = enable)
 *
 * \return 	0 on success or -1 on error and \em errno set\n
 *
 * \sa VME4L_SysCtrlFunctionGet
 */
int VME4L_SysCtrlFunctionSet( int fd, int state )
{
	return ioctl( fd, VME4L_IO_SYS_CTRL_FUNCTION_SET, state);
}

/***********************************************************************/
/** Generate VME system reset
 *
 * This function can be used to perform a VMEbus reset pulse
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 *
 * \return 	0 on success or -1 on error and \em errno set\n
 *
 */
int VME4L_SysReset( int fd )
{
	return ioctl( fd, VME4L_IO_SYS_RESET, 0);
}

/***********************************************************************/
/** Check if arbiter has detected a timeout
 *
 * \tsi148 	Not supported by TSI148 VME bridge.
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param clear		\IN  1 = If arbitration timeout pending, clear it\n
 *						 0 = Leave arbitration timeout indication untouched
 *
 * \return 	0 if no arbitration timeout pending\n
 *			1 if arbitration timeout was pending\n
 *			-1 on error and \em errno set\n
 *
 */
int VME4L_ArbitrationTimeoutGet( int fd, int clear )
{
	return ioctl( fd, VME4L_IO_ARBITRATION_TIMEOUT_GET, clear);
}

/* master funcs */


/**********************************************************************/
/** Get information about last VME bus error
 *
 * The driver stores every indication of bus errors. This function
 * allows you to read this information from the driver.
 *
 * If the VME bridge supports it, you will also get the information
 * about the faulted address, otherwise *spaceP will receive
 * #VME4L_SPC_INVALID.
 *
 * \plda12  Address/attribute information is not provided.
 * \pldz002 Address/attribute information is provided.
 * \tsi148  attribute information is not provided.
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param attrP		\IN  Pointer to variable that receives attribute
 * 						 info like AM, direction and IACK or normal state
 * 						 of the last bus error.
 * \param addrP		\IN  Pointer to variable that receives the VME address
 *						 info of the last bus error. If passed as NULL,
 *						 no address info is returned.
 * \param clear		\IN  1 = If bus error info pending, clear it\n
 *						 0 = Leave bus error info untouched.
 *
 * \return 	0 if no valid bus error info was pending\n
 *			1 if valid bus error info was pending\n
 *			-1 on error and \em errno set\n
 *
 * \sa \ref vme4lberr
 */
int VME4L_BusErrorGet(
	int fd,
	int *attrP,
	vmeaddr_t *addrP,
	int clear )
{
	VME4L_BUS_ERROR_INFO blk;
	int rv;

	blk.clear = clear;

	rv = ioctl( fd, VME4L_IO_BUS_ERROR_GET, &blk );

	*attrP = blk.attr;
	*addrP  = blk.addr;

	return rv;
}

/***********************************************************************/
/** Get VMEbus requester mode
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 *
 * \return 	0 if requester mode is "release on request"\n
 *			1 if requester mode is "release when done"\n
 *			256 if requester mode is "user defined"\n
 *			-1 on error and \em errno set\n
 *
 * \sa VME4L_RequesterModeSet
 */
int VME4L_RequesterModeGet( int fd )
{
	return ioctl( fd, VME4L_IO_REQUESTER_MODE_GET, 0);
}

/***********************************************************************/
/** Control VMEbus requester mode
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param state		\IN  0 = Set requester mode to "release on request"\n
 *						 1 = Set requester mode to "release when done"
 *
 * \return 	0 on success or -1 on error and \em errno set
 *
 * \sa VME4L_RequesterModeGet
 */
int VME4L_RequesterModeSet( int fd, int state )
{
	return ioctl( fd, VME4L_IO_REQUESTER_MODE_SET, state);
}

/***********************************************************************/
/** Get VMEbus master write mode
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 *
 * \return 	0 if write mode is "delayed"\n
 *			1 if write mode is "posted write"\n
 *			-1 on error and \em errno set\n
 *
 * \sa VME4L_VME4L_PostedWriteModeSet, \ref vme4lpostwr
 */
int VME4L_PostedWriteModeGet( int fd )
{
	return ioctl( fd, VME4L_IO_POSTED_WRITE_MODE_GET, 0);
}

/***********************************************************************/
/** Control VMEbus master write mode
 *
 * \tsi148 All writes are posted.
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param state		\IN  0 = Set write mode to "delayed"\n
 *						 1 = Set write mode to "posted write"
 *
 * \return 	0 on success or -1 on error and \em errno set
 *
 * \sa VME4L_RequesterModeGet, \ref vme4lpostwr
 */
int VME4L_PostedWriteModeSet( int fd, int state )
{
	return ioctl( fd, VME4L_IO_POSTED_WRITE_MODE_SET, state);
}


/* IPC funcs */

/**********************************************************************/
/** Generate a VMEbus interrupt
 *
 * The function triggers a VMEbus interrupt with the specified level
 * \a level and the specified vector \a vector.
 *
 * Usually, the VMEbus interface hardware includes only one interrupter.
 * Each interrupter can generate only one interrupt level at a time.
 * If the caller tries to generate an interrupt and there is no interrupter
 * available (e.g. the only interrupter is still generating an interrupt)
 * the function returns with -1 and sets \em errno to \c EBUSY.
 *
 * \pldz002 Only one interrupter is available.
 * \plda12  Only one interrupter is available.
 * \tsi148  Only one interrupter is available.
 *
 * The generated interrupt is auto-cleared after an IACK cycle
 * (ROAK). However, a pending interrupt level can be manually removed by
 * VME4L_IrqGenClear().
 *
 * You can check if the interrupt was acknowledged by calling
 * VME4L_IrqGenAcked().
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param level		\IN  VME interupt level (1..7)
 * \param vector	\IN  VME vector (0..255)
 *
 * \return >= 0 interrupter ID that is issuing this interrupt or -1
 *				on error and \em errno is set to:\n
 *			- \c EBUSY:  No interrupter available
 *			- \c EINVAL: Bad parameter (vector/level invalid)
 *
 * \sa VME4L_IrqGenClear, VME4L_IrqGenAcked
 */
int VME4L_IrqGenerate( int fd, int level, int vector)
{
	return ioctl( fd, VME4L_IO_IRQ_GENERATE2,
				  VME4L_LEVEL_VECTOR_PUT( level, vector ) );
}

/**********************************************************************/
/** Check if generated interrupt has been acknowledged
 *
 * The function checks if the interrupt triggered by VME4L_IrqGenerate()
 * has been acknowledged at the VMEbus.
 *
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param id		\IN  Interrupter ID that has been returned by
 * 						 VME4L_IrqGenerate
 *
 * \return 0 if interrupt not yet acknowledged\n
 *		   1 if interrupt has been acknowledged\n
 *		   -1 on error, and \em errno is set to:\n
 *			- \c EINVAL: ID invalid
 *
 * \sa VME4L_IrqGenerate, VME4L_IrqGenClear
 */
int VME4L_IrqGenAcked( int fd, int id )
{
	return ioctl( fd, VME4L_IO_IRQ_GEN_ACKED, id );
}

/**********************************************************************/
/** Clear a pending interrupt
 *
 * The function clears (aborts) a pending VME interrupt initiated by
 * VME4L_IrqGenerate().
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param id		\IN  Interrupter ID that has been returned by
 * 						 VME4L_IrqGenerate
 *
 * \return 0 on success, or -1 on error, and \em errno is set to:\n
 *			- \c EINVAL: ID invalid
 *
 * \sa VME4L_IrqGenerate, VME4L_IrqGenAcked
 */
int VME4L_IrqGenClear( int fd, int id )
{
	return ioctl( fd, VME4L_IO_IRQ_GEN_CLEAR, id );
}


/**********************************************************************/
/** Perform VME Read-Modify-Write cycle
 *
 * This function issues a VME RMW cycle as follows:
 * - Reads data from \a vmeAddr (this value will be returned in \a *rvP).
 * - Performs a binary OR of the value read using \a mask.
 * - Writes the new value to \a vmeAddr.
 *
 * The RMW cycle is performed atomically on the VMEbus.
 *
 * \tsi148 [VME4] VMEbus Write During Wrap-around RMW work-around is not
 *  implemented per default due to the lower performance. If you really need
 *  to use wrap-around RMW cycles you have to set VMEFL[ACKD] to slow (0) in
 *  \ref vme4l-tsi148.h.
 *
 * \param spaceFd 	\IN  File descriptor for VME space, returned by
 *						 VME4L_Open()
 * \param vmeAddr	\IN  Start address within VME space.
 * \param accWidth 	\IN  1 = Use 8-bit access, 2 = Use 16-bit access,
 *						 4 = Use 32-bit access.
 * \param mask		\IN  Value to OR into original value read
 * \param rvP		\IN  Pointer to variable that receives read value
 * \param *rvP		\OUT Contains original value read
 *
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EIO: 	 A VME bus error occurred
 *			- \c EBUSY:  The requested address range is currently not available
 *			- \c EINVAL: Bad parameter
 *
 * \sa VME4L_SwapModeSet
 */
int VME4L_RmwCycle(
	int spaceFd,
	vmeaddr_t vmeAddr,
	int accWidth,
	uint32_t mask,
	uint32_t *rvP)
{
	VME4L_RMW_CYCLE blk;
	int rv;

	blk.vmeAddr 	= vmeAddr;
	blk.accWidth	= accWidth;
	blk.mask		= mask;
	blk.rv			= 0;

	rv = ioctl( spaceFd, VME4L_IO_RMW_CYCLE, &blk );

	*rvP = blk.rv;

	return rv;
}

/**********************************************************************/
/** Perform VME Address-Only cycle
 *
 * This function issues a VME address-only cycle at the specified
 * \a vmeAddr
 *
 * \tsi148 Address-only cycles are not supported.
 *
 * \param spaceFd 	\IN  File descriptor for VME space, returned by
 *						 VME4L_Open()
 * \param vmeAddr	\IN  Address within VME space
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EBUSY: The requested address range is currently not available
 *			- \c EINVAL: Bad parameter
 *
 * \sa VME4L_SwapModeSet
 */
int VME4L_AOnlyCycle(
	int spaceFd,
	vmeaddr_t vmeAddr)
{
	VME4L_AONLY_CYCLE blk;

	blk.vmeAddr 	= vmeAddr;

	return ioctl( spaceFd, VME4L_IO_AONLY_CYCLE, &blk );
}

/**********************************************************************/
/** Control VME slave window
 *
 * Enables a VME slave window. If \a size is 0, the window is disabled.
 *
 * \a vmeAddr and \a size must match the capabilities of the underlying
 * VME bridge.
 *
 * A change of parameters or disabling of the window is only allowed
 * if no process has the window mapped by VME4L_Map().
 *
 * \param spaceFd 	\IN  File descriptor for a slave VME space, returned by
 *						 VME4L_Open()
 * \param vmeAddr	\IN  slave window start address on VME
 * \param size		\IN  size in bytes of window. If 0, disable window
 *
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c ENOTTY: the window does not exist in your HW or it is
 *						 not a slave window
 *			- \c EBUSY:  Some process has the window mapped and you attempted
 *						 to change the size.
 *			- \c EINVAL: Bad parameter. \a vmeAddr and/or \a size are
 *						 not supported by HW
 *
 *
 * \sa \ref vme4lshram
 */
int VME4L_SlaveWindowCtrl(
	int spaceFd,
	vmeaddr_t vmeAddr,
	size_t size)
{
	VME4L_SLAVE_WINDOW_CTRL blk;

	blk.vmeAddr 	= vmeAddr;
	blk.size		= size;

	return ioctl( spaceFd, VME4L_IO_SLAVE_WINDOW_CTRL, &blk );
}

/**********************************************************************/
/** Read value from mailbox register
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param mbox		\IN  mailbox number (0..n), VME bridge dependent
 * \param *rvP		\OUT Contains value read
 *
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EINVAL: the specified mbox number does not exist
 *
 * \sa \ref vme4lmbox
 */
int VME4L_MboxRead( int fd,	int mbox, uint32_t *rvP)
{
	VME4L_MBOX_RW blk;
	int rv;

	blk.mbox		= mbox;
	blk.direction	= 0;	/* read */
	blk.val			= 0;

	rv = ioctl( fd, VME4L_IO_MBOX_RW, &blk );

	*rvP = blk.val;

	return rv;
}

/**********************************************************************/
/** Write value to mailbox register
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param mbox		\IN  mailbox number (0..n), VME bridge dependent
 * \param val		\IN  Value to write into mailbox
 *
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EINVAL: the specified mbox number does not exist
 *
 * \sa \ref vme4lmbox
 */
int VME4L_MboxWrite( int fd, int mbox, uint32_t val)
{
	VME4L_MBOX_RW blk;

	blk.mbox		= mbox;
	blk.direction	= 1;	/* write */
	blk.val			= val;

	return ioctl( fd, VME4L_IO_MBOX_RW, &blk );
}

/**********************************************************************/
/** Read value from location monitor register
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param reg		\IN  register index (0..n), VME bridge dependent
 * \param *rvP		\OUT Contains value read
 *
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EINVAL: the specified register number does not exist
 *
 * \sa \ref vme4llocmon
 */
int VME4L_LocMonRegRead( int fd, int reg, uint32_t *rvP)
{
	VME4L_MBOX_RW blk;
	int rv;

	blk.mbox		= reg;
	blk.direction	= 0;	/* read */
	blk.val			= 0;

	rv = ioctl( fd, VME4L_IO_LOCMON_REG_RW, &blk );

	*rvP = blk.val;

	return rv;
}

/**********************************************************************/
/** Write value to location monitor register
 *
 * \param fd	 	\IN  File descriptor for any VME space,
 *						 returned by VME4L_Open()
 * \param reg		\IN  register index (0..n), VME bridge dependent
 * \param val		\IN  Value to write into register
 *
 * \return 	0 on success or -1 on error\n
 *			In case of error, \em errno is set to\n
 *			- \c EINVAL: the specified register number does not exist
 *
 * \sa \ref vme4llocmon
 */
int VME4L_LocMonRegWrite( int fd, int reg, uint32_t val)
{
	VME4L_MBOX_RW blk;

	blk.mbox		= reg;
	blk.direction	= 1;	/* write */
	blk.val			= val;

	return ioctl( fd, VME4L_IO_LOCMON_REG_RW, &blk );
}

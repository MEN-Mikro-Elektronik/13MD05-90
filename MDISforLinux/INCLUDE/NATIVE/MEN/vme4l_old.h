/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  vme4l_old.h
 *
 *      \author  uf
 *        $Date: 2003/12/15 15:02:18 $
 *    $Revision: 1.3 $
 *
 *  	 \brief  Header file for users of original VME4L drivers.
 *
 *               !!! This file is provided for backward compatibiliy
 *				 !!! only.
 *
 *               Provides VME4Linux device names, minor device numbers,
 *               I/O controls codes and arguments
 *				 and VME4L user mode library interface.
 *
 *     Switches: -
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2002-2019, MEN Mikro Elektronik GmbH
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
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/*! \mainpage

  This documentation describes the general issues about the VME4L interface
  and the VME4L drivers.

  \section XXX_SEC1 Introduction

    A <b> user mode interface </b> will be provided by supporting
    following file operations:

        - open
		- close
		- ioctl
       		- installing user mode interrupt handling
         	  ( The user mode signal handler may be swapped to disk! )
       		- installing user mode BUS ERROR handling
       		- access probing on VMEbus
       		- VMEbus interrupt generating
		- mmap
         	- mapping VME memory into user space

    For Linux <b> kernel mode drivers </b> i.e. MEN's MDIS/BBIS
    following functions will be exported:

		- vme_request_irq
		- vme_free_irq
		- vme_bus_to_phys
		- vme_ilevel_control

    This functions support the VMEbus interrupt controller
    interrupt handling and VMEbus to CPU address conversion.

  \section XXX_SEC2 License

  	This program is free software; you can redistribute it and/or
  	modify it under the terms of the GNU General Public License
  	as published by the Free Software Foundation; version 2 of the License.
  	
  	Neither Klaus Popp, Ulf Franke nor MEN admit liability nor provide
  	warranty for any of this software. This material is provided "AS-IS".	
  	
  	(c) Copyright 2002 Klaus Popp, Ulf Franke, All Rights Reserved.
  	
  				http://www.men.de , http://www.men-micro.com

  \section Generation Driver Generation

  \subsection ELINOS ELINOS
  Copy vme4l.h into ../linux/include and overwrite vme-menpci2vme.c
  in drivers/char directory. Enable the driver under elk -> Kernel
  Configuration -> Character Devices as kernel driver or as module
  and rebuild it.

  \subsection THIRD_PATRY Third Party Linux Distribution

  In the \c VME4L/Makefile you must adapt the following variables:

  \c KERNELDIR must point to the include directory within your Linux
  kernel tree (not the \c /usr/include directory !). It is important
  to specify the correct directory, since the driver takes some
  configuration settings from there (e.g. \c CONFIG_DEVFS_FS ).

  \c CROSS_COMPILE and/or \c CC/LD must be set to match your tool
  chain of your cross compiler.

  After you have adapted the \c Makefile, you should be able to
  compile the driver:

  \code
  $ make
  \endcode

  This generates \c vme-menpci2vme.o that is the loadable module. On your
  target, you should be able to load this module.

  \section Install Installation

  Before loading it on the target, you should check if you are using
  the "device filesystem" support. Check \c linux/autoconf.h file for
  the \c CONFIG_DEVFS_FS switch.

  If \c CONFIG_DEVFS_FS is set, the driver creates \c /dev/vme4l_a16d16
  and \c /dev/vme4l_a24d16 automatically at load time, you can just do

  \code insmod vme-menpci2vme.o\endcode on the target.

  If \c CONFIG_DEVFS_FS is not set, you must find a free major number
  for the driver (look at \c /proc/devices). Then create a device node:

  \code
  # mknod /dev/vme4l_a16d16 c 230 0
  # mknod /dev/vme4l_a24d16 c 230 1
  # mknod /dev/vme4l_a32d32 c 230 4
  \endcode

  The major number must also be passed to the module at load time:
  \code
  # insmod vme-menpci2vme.o major=230

	menpci2vme $Id: vme4l_old.h,v 1.3 2003/12/15 15:02:18 kp Exp $ Created Jul 14 2002 15:31:57
	    found bridge (Device number 0x19)
 		physBase=0xbc000000
	menpci2vme: using major 230

  \endcode

*/



#ifndef _VME4L_OLD_H
#define _VME4L_OLD_H

#ifdef __cplusplus
   extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/** obsolete IO control parameter structure for VME4L_IO_BUS_PROBE  */
typedef struct
{
	char 	*vmeAddr;	/**< VME address must be size aligned */
	int		direction;	/**< 0 for read, 1 for write */
	int		size;		/**< 1,2 or 4 for access size */
	void	*buf;		/**< pointer to variable where read value will be stored */
}VME4L_IO_BUS_PROBE_STRUCT;

/** obsolete IO control parameter structure for VME4L_IO_IRQ_SIG_INSTALL */
typedef struct
{
	int signal; 		/**< SIGBUS for BUS ERROR always, other for user mode handlers */
	int vector;			/**< 0..255 - must match the vector, delivered by the VME device */
	int irqReleaseMode; /**< default VME4L_IO_IRQ_RELEASE_MODE_RORA */
	int install;		/**< 1 for install, 0 for remove */
}VME4L_SIG_DATA;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/* ========================================================================================*/
//! device name for short user - address modifier 0x29
/*!
    example for device names and minor numbers

    crw-rw-rw-    1 root     root     254,   0 Jun  4  2002 /dev/vme4l_a16d16

    crw-rw-rw-    1 root     root     254,   1 Jun  4  2002 /dev/vme4l_a24d16
	\sa VME4L_DEV_MINOR_A16_D16
*/
															/* address modifiers */
#define VME4L_DEV_NAME_A16_D16		"/dev/vme4l_a16d16"		//!< 0x29 - short user
#define VME4L_DEV_NAME_A24_D16		"/dev/vme4l_a24d16" 	//!< 0x39 - standard user data
#define VME4L_DEV_NAME_A24_D32		"/dev/vme4l_a24d32"		//!< 0x39 - standard user data
#define VME4L_DEV_NAME_A32_D16		"/dev/vme4l_a32d16"		//!< 0x09 - extended user data
#define VME4L_DEV_NAME_A32_D32		"/dev/vme4l_a32d32"		//!< 0x09 - extended user data
#define VME4L_DEV_NAME_A16_D16_SUP	"/dev/vme4l_a16d16_sup"	//!< 0x2D - short supervisor
#define VME4L_DEV_NAME_A24_D16_SUP	"/dev/vme4l_a24d16_sup" //!< 0x3D - standard sup  data
#define VME4L_DEV_NAME_A24_D32_SUP	"/dev/vme4l_a24d32_sup" //!< 0x3D - standard sup  data
#define VME4L_DEV_NAME_A32_D16_SUP	"/dev/vme4l_a32d16_sup" //!< 0x0D - extended sup  data
#define VME4L_DEV_NAME_A32_D32_SUP	"/dev/vme4l_a32d32_sup" //!< 0x0D - extended sup  data

#define VME4L_DEV_MINOR_A16_D16			0 //!< minor device number
#define VME4L_DEV_MINOR_A24_D16			1 //!< minor device number
#define VME4L_DEV_MINOR_A24_D32			2 //!< minor device number
#define VME4L_DEV_MINOR_A32_D16			3 //!< minor device number
#define VME4L_DEV_MINOR_A32_D32			4 //!< minor device number
#define VME4L_DEV_MINOR_A16_D16_SUP		5 //!< minor device number
#define VME4L_DEV_MINOR_A24_D16_SUP		6 //!< minor device number
#define VME4L_DEV_MINOR_A24_D32_SUP		7 //!< minor device number
#define VME4L_DEV_MINOR_A32_D16_SUP		8 //!< minor device number
#define VME4L_DEV_MINOR_A32_D32_SUP		9 //!< minor device number

/* ========================================================================================*/
/*! \defgroup ioctl_cmd IO-Control Codes */
//@{
#define VME4L_IOC_MAGIC		'v'		/* possible conflict - see ioctl-number.txt */

//! enable/disable posted writes
/*!

	\code ioctl( fd, VME4L_IO_SET_POSTED_WRITE, VME4L_IO_SET_POSTED_WRITE_ENABLE ) \endcode

	\sa VME4L_IO_SET_POSTED_WRITE_DISABLE VME4L_IO_SET_POSTED_WRITE_ENABLE
*/
#define VME4L_IO_SET_POSTED_WRITE	_IO(   VME4L_IOC_MAGIC, 1 )

//! probe a vme access read, write 1,2 | 4 byte
/*!

	\code
	VME4L_IO_BUS_PROBE_STRUCT  busProbeData;

	memProbeData.vmeAddr	= (char*)A201_BASE;
	memProbeData.size		= 2;
	memProbeData.buf		= &word;
	memProbeData.direction	= 0;
	if( ioctl( fd, VME4L_IO_BUS_PROBE, &memProbeData ) == EACCES )
	{
		printf("*** BUS ERROR at VMEbus %08x\n", A201_BASE );
	}
	\endcode

	\sa VME4L_IO_BUS_PROBE_STRUCT
*/
#define VME4L_IO_BUS_PROBE			_IOWR( VME4L_IOC_MAGIC, 2, VME4L_IO_BUS_PROBE_STRUCT )

//! installs/deinstalls a signal for VMEbus IRQs or BUS ERROR
/*!

	\code
	VME4L_SIG_DATA  sigData;

	signal( SIGBUS, berrSignalHandler );
	
	sigData.signal  = SIGBUS;
	sigData.install = 1;
	ioctl( fd, VME4L_IO_IRQ_SIG_INSTALL, &sigData );
	ioctl( fd, VME4L_IO_IRQ_ENABLE, VME4L_IO_IRQ_ENABLE_BUS_ERROR );


	signal( SIGUSR1, m22SignalHandler );
	sigData.signal			= SIGUSR1;
	sigData.vector	 		= A201_VECTOR;
	sigData.irqReleaseMode	= VME4L_IO_IRQ_RELEASE_MODE_RORA;
	sigData.install 		= 1;
	ioctl( fd, VME4L_IO_IRQ_SIG_INSTALL, &sigData );
	ioctl( fd, VME4L_IO_IRQ_ENABLE, VME4L_IO_IRQ_ENABLE_LEV_5 );
	\endcode

	\note VMEbus IRQ levels and the BUS ERROR IRQ will be not enabled
	automatically.

    It is allowed to install two different signals from the
    same process for the same vector.
      - i.e. handling of 2 CAN channels on M-Module M51
        - SIGUSR1 fire user IRQ handler for channel0
        - SIGUSR2 fire user IRQ handler for channel1

	\sa VME4L_SIG_DATA VME4L_IO_IRQ_ENABLE VME4L_IO_IRQ_RELEASE_MODE_RORA VME4L_IO_IRQ_RELEASE_MODE_ROAK
*/
#define VME4L_IO_IRQ_SIG_INSTALL	_IOW(  VME4L_IOC_MAGIC, 3, VME4L_SIG_DATA )

//! enables/disables a IRQ specified by level | BUS ERROR
/*!

	\code
	ioctl( fd, VME4L_IO_IRQ_ENABLE, VME4L_IO_IRQ_ENABLE_BUS_ERROR );
	ioctl( fd, VME4L_IO_IRQ_ENABLE, VME4L_IO_IRQ_ENABLE_DISABLE_MASK | VME4L_IO_IRQ_ENABLE_BUS_ERROR );
	if( ioctl( fd, VME4L_IO_IRQ_ENABLE, VME4L_IO_IRQ_ENABLE_DISABLE_MASK | VME4L_IO_IRQ_ENABLE_LEV_5 ) )
	
	\endcode

	\sa VME4L_IO_IRQ_SIG_INSTALL
*/
#define VME4L_IO_IRQ_ENABLE			_IO(   VME4L_IOC_MAGIC, 4 )


//! generate VMEbus IRQ a specified by level/vector
/*!

	\code
	ioctl( fd, VME4L_IO_IRQ_GENERATE, VME4L_LEVEL_VECTOR_PUT( 2, 123 ) );
	\endcode

*/
#define VME4L_IO_IRQ_GENERATE		_IO(   VME4L_IOC_MAGIC, 5 )

//! get pending VMEbus IRQs
/*!

	\code
	ioctl( fd, VME4L_IO_IRQ_GET_PENDING, &pending );
	if( pending )
		printf("VME IRQ pending\n");
	\endcode

*/
#define VME4L_IO_GET_IRQ_PENDING		_IO(   VME4L_IOC_MAGIC, 6 )

//@}

/* ========================================================================================*/
/*! \defgroup ioctl_args IO-Control Arguments */
//@{
//! ioctl argument to disable posted writes - default
#define VME4L_IO_SET_POSTED_WRITE_DISABLE		0

//! ioctl argument to enable posted writes
/*!
   You can decrease the CPU load by enabling posted writes
   on the VMEbus.

   Normaly the CPU stays until the write access is completed.
   Enabled posted writes don't wait until the VME access is
   finished.

    - the program sequence
     	-# write to VMEbus
     	-# delay
     	-# read from VMEbus

	- can be changed by hardware with enabled posted writes
     	-# delay
     	-# write to VMEbus
     	-# read from VMEbus

   \warning This may unsave! Do not enable posted writes if you are
   not sure!


	\sa VME4L_IO_SET_POSTED_WRITE VME4L_IO_SET_POSTED_WRITE_DISABLE
*/
#define VME4L_IO_SET_POSTED_WRITE_ENABLE		1



//! ioctl argument - user must reenable the level after IRQ - default
/*!
	IRQ release modes - needed by signal fired user interrupt routines
	\sa VME4L_IO_IRQ_SIG_INSTALL
*/
#define VME4L_IO_IRQ_RELEASE_MODE_RORA	0x1234
//! ioctl argument  - the level will not disabled in the kernel mode IRQ - be careful
/*!
	IRQ release modes - needed by signal fired user interrupt routines
	
	This mode can be used only if all devices are in ROAK mode on this level.
	If not - use mode VME4L_IO_IRQ_RELEASE_MODE_RORA.
	\sa VME4L_IO_IRQ_SIG_INSTALL VME4L_IO_IRQ_RELEASE_MODE_RORA
*/
#define VME4L_IO_IRQ_RELEASE_MODE_ROAK	0x8765

/* IRQ enable/disable */
#define VME4L_IO_IRQ_ENABLE_LEV_1			0x01	//!< level 1
#define VME4L_IO_IRQ_ENABLE_LEV_2			0x02	//!< level 2	
#define VME4L_IO_IRQ_ENABLE_LEV_3			0x03	//!< level 3
#define VME4L_IO_IRQ_ENABLE_LEV_4			0x04	//!< level 4
#define VME4L_IO_IRQ_ENABLE_LEV_5			0x05	//!< level 5
#define VME4L_IO_IRQ_ENABLE_LEV_6			0x06	//!< level 6
#define VME4L_IO_IRQ_ENABLE_LEV_7			0x07	//!< level 7
#define VME4L_IO_IRQ_ENABLE_BUS_ERROR		0x08	//!< BUS ERROR
#define VME4L_IO_IRQ_ENABLE_DISABLE_MASK	0x80    //!< disable mask

/* IRQ level/vector */
#define VME4L_LEVEL_VECTOR_PUT( level, vector )		( (level<<8) | vector )
#define VME4L_LEVEL_GET( level_vector )		        (level_vector>>8)
#define VME4L_VECTOR_GET( level_vector )		    (level_vector & 0xff)

//@}

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

#ifdef __cplusplus
   }
#endif
#endif /*_VME4L_OLD_H*/

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  men_16z077_doc.c
 *
 *      \author  thomas.schnuerer@men.de
 *        $Date: 2012/11/06 20:27:22 $
 *    $Revision: 1.6 $
 *
 *      \brief   User documentation for native Linux driver of
 *				 16Z077_ETH / 16Z087_ETH IP cores
 *
 *     Required: -
 *
 *     \switches -
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: men_16z077_doc.c,v $
 * Revision 1.6  2012/11/06 20:27:22  ts
 * R: added description of features, updated usable kernels
 *
 * Revision 1.5  2012/09/07 18:04:59  ts
 * R: support for IEEE 802.1Q VLAN tagging requested by customer
 * M: updated driver documentation and added dbglvl description
 *
 * Revision 1.4  2009/02/10 19:18:36  ts
 * R: driver supports also 15P511 PMC
 * M: added explanations about non DMA modes
 *
 * Revision 1.3  2007/11/28 15:37:15  ts
 * spelling / typo corrections by PU
 *
 * Revision 1.2  2007/11/19 13:46:47  ts
 * Documentation finished
 *
 * Revision 1.1  2007/10/23 18:00:39  ts
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2007 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
/*! \mainpage Linux native driver for Ethernet IP cores

    The men_lx_z77(_sw).ko module serves as a Linux driver for the MEN FPGA 
	IP cores 16Z077_ETH (10-Mbit HD only, obsolete) and 16Z087_ETH (10/100Mbit).
	The IP core 16Z087 is available e.g in the PPC based EM1 and the P511 PMC 
	card.
	\n
	The driver integrates seamlessly as a native network driver into the Linux 
	kernels TCP/IP stack and can be accessed as a regular network device, 
	e.g. eth1 (the assigned interface number may vary depending on already 
	installed network cards).
	\n \n
	\section install Driver build and Installation

	The driver is primarily designed to be used in conjunction with the
	MDIS system package for Linux. It is possible with some modifications
	to build the driver into the kernel but its primary intended use is to 
	load it as a module.

	\subsection kernver Supported kernel versions and features

	The driver was build tested with kernel versions 2.6.20 up to 3.6. Tests
    were done with kernels 2.6.22 (PPC), 2.6.32 (x86, Ubuntu 10.04) and 3.2.0 
    (x86, Ubuntu 12.04). The most important features are:
    - Support for IEEE 802.1q VLAN tagging (kernel must be enabled for it)
    - Support for Multicast MAC addresses (tool ipmaddr can be used to apply addresses)
    - verbose reporting when message level set with ethtool

	\subsection builddrv Building the driver
	
	The driver can be built either as part of an ELinOS project 
	(for MEN PowerPC CPUs) or to run as a self-hosted project (on x86 desktop
	or cPCI PCs). The driver depends on the Chameleon FPGA base driver, which is
	named \p men_lx_chameleon.ko (The ethernet driver registers itself using
	the base driver, so the module \p men_lx_chameleon.ko has to be loaded too
	or must be built statically into the kernel). \n
	the MDIS system package for Linux is needed to build and use the 
	driver. The drivers package 13z07790.zip can be added to the MDIS Wizard 
	like any other driver package. It can be applied to any carrier or ESM 
	that is equipped with a Chameleon FPGA, e.g. ESMs EM7 or EM1.\n

	\warning The label ETH_1 assigned in the MDIS Wizard is just for maintenance
	purposes in the generated \c system.dsc file. The device cannot be used 
	with MDIS utilities! trying something like \c m_open \c eth_1 will cause 
	an error.\n \n

	\subsection loaddrv Loading the driver

	In case of the PMC module P511 the driver can operate either on a PowerPC 
	based CPU like the MEN D3C or with a x86 based board like the F14 
	(together with a PMC carrier). The difference here is that the kernel
    module will be named according to the endianness of the used CPU. 
	The driver will be named\n
 - \p men_lx_z77_sw.ko        on big endian architectures (PowerPC, e.g. EM1)\n
 - \p men_lx_z77.ko           on little endian architectures (x86, e.g. F14)\n	
 \n
	
	When all MDIS modules have been built successfully, they are installed into
	the default path for kernel modules, typically in \n
	/lib/modules/`uname -r`/misc/. The command 'uname -r' returns the currently	
	used kernel version, see section Troubleshooting. When installation is done
	using the MDIS Wizard, the modules are all in place and module dependencies
	have been updated.\n
	For the P511 the procedure is similar, the driver is either built as an
	ElinOS MDIS Project or a self-hosted MDIS project. Typically a self-hosted 
	MDIS project is used when the P511 is operating on the same x86 CPU.\n
	Now the driver can be loaded via the modprobe command, here shown with all
	parameters for a P511 together with F14 CPU:\n
\verbatim
 #> modprobe men_lx_z77 phyadr=0,1 nodma=0 mode=AUTO
MEN ETH driver built Feb  5 2009 17:07:20
passed PHY mode: 'AUTO'
found 16Z087 (phys 0xfbffe300 irq 0x3 base addr 0xe003e300 )
eth2: 16Z087 found at 0xe003e300
passed PHY mode: 'AUTO'
found 16Z087 (phys 0xfbffe400 irq 0x3 base addr 0xe0050400 )
eth3: 16Z087 found at 0xe0050400
\endverbatim
\n
When used on the EM1, the \p modprobe command looks like 
\verbatim
 #> modprobe men_lx_z77_sw phyadr=1 mode=AUTO
MEN ETH driver built Feb  5 2009 17:07:20
passed PHY mode: 'AUTO'
found 16Z087 (phys 0xfbffe300 irq 0x3 base addr 0xff039000 )
eth1: 16Z087 found at 0xff039000
\endverbatim

On the EM1 or other PowerPC CPU the loaded modules list should now look like
this:
\verbatim
 #> lsmod
 Module                  Size  Used by    Not tainted
 men_lx_z77_sw          23956  0
 men_lx_chameleon        5016  1 men_lx_z77_sw
 men_chameleon          10640  1 men_lx_chameleon
 men_oss                16112  2 men_lx_chameleon,men_chameleon
\endverbatim

\subsection phyint PHY interface settings
    The default mode for the PHY interface is AUTO, which means the 
	communication settings are handled with the partner interface by
	autonegotiation. See the kernel parameters below to see how other
	fixed values can be passed to the driver.

\subsection usage Driver usage and interaction with standard tools

	The driver supports all standard tools like ifconfig and ethtool. 
	This way all information about the link status and number of received and
	transmitted packets can be retrieved. \n \n 

\section modparm Available module parameters
	The driver supports the following parameters, which can be retrieved with the \p modinfo command:
\verbatim
/# modinfo men_lx_z77
filename:       /lib/modules/2.6.22.18-ELinOS-486/misc/men_lx_z77.ko
license:        GPL
description:    MEN Ethernet IP core unit driver
author:         thomas.schnuerer@men.de
vermagic:       2.6.22.18-ELinOS-486 mod_unload 486 
depends:        men_lx_chameleon
parm:           nodma:0: standard (DMA) function 1: P511 (no DMA) mode (int)
parm:           phyadr:Address of PHY connected to each Z87 unit (array of int)
parm:           mode: AUTO,10HD,10FD,100HD,100FD. default: AUTO (charp)
parm:           dbglvl: debug level 0..3 (none..very verbose) (int)
\endverbatim

- \p mode \n This is the initial PHY interface mode that shall be used upon starting
    the network interface. Valid values are
\verbatim
	mode=10HD	Force PHY Mode to 10 Megabit half duplex
	mode=10FD	Force PHY Mode to 10 Megabit full duplex
	mode=100HD	Force PHY Mode to 100 Megabit half duplex
	mode=100FD	Force PHY Mode to 100 Megabit full duplex
	mode=AUTO	Let Phy autonegotiate its settings   (default setting) 

	usage for multiple instances, when multiple Z87 cores are found:

	mode=100FD,AUTO		force 1st Phy to 100 Mbit full duplex, 2nd Phy to Autoneg.

\endverbatim
	e.g.  \p modprobe \p men_lx_z77_sw \p mode=100FD \n
\b Attention: for the P511 the passed \p mode parameter is assigned to both PHYs.     

- \p nodma (P511 only!) \n when passed as 1, the P511 does \b not use PCI DMA data transfer but stores the Ethernet frames locally in the P511 SDRAM. Then the driver has to transfer the frame data in programmed mode which significantly decreases the transfer rate from about 10 MB/s at 100Mbit mode to around 1,8 MB/s. \n
\verbatim
	nodma=0		standard PCI DMA transfer of Ethernet data    (default setting)
	nodma=1		no PCI DMA, frames stored locally on P511
\endverbatim
	e.g.  \p modprobe \p men_lx_z77_sw \p nodma=1\n
\warning this mode is used only in special applications where it is desired to have full control over system activity. For all other purposes the parameter can be omitted. \n

- \p dbglvl \n The debug level is identical to the msglvl parameter passed in the ethtool command. It allows to enable verbose debugging at driver load time.\n

\verbatim
	dbglvl=0	no debug outputs, driver runs with maximum performance. (default setting)
	dbglvl=1	basic debugging, function entry and leave debugs are shown.
	dbglvl=2	verbose debugging, like level 1 plus frame send/receive messages.
	dbglvl=3	very verbose debugging, like level 2 plus IRQ and sent/received frame debugs.
\endverbatim

\warning debug levels 2 and 3 decrease driver performance noticable. They should be used only for hardware debugging

  \n

- \p phyadr \n Address of PHYs connected to each 16Z087- unit. This parameter tells the driver the PHY address (a value between 0 and 31) of each found instance of the IP core. This parameter is determined by the hardware and therefore fixed. For the P511 this parameter \b must be passed as \p phyadr=0,1. For the EM1 the parameter \b must be passed as 1.
\verbatim
	phyadr=0,1	PHY addresses of 1st and 2nd P511 Ethernet IP core instance
	phyadr=1	PHY address of the Micrel KS8721 PHY on the EM1
\endverbatim

    Keep in mind that the connection is not set up on driver load time
    but when the interface is brought up using \c ifconfig !

    \subsection etht_link Retrieving link status information

	Since the necessary support functions for ethtool are used in the
	driver, the link status can be retrieved as shown in this example. 
	The interface name used here (eth0) may vary on your system.

\verbatim
/# ethtool eth0
Settings for eth0:
        Supported ports: [ TP MII ]
        Supported link modes:   10baseT/Half 10baseT/Full
                                100baseT/Half 100baseT/Full
        Supports auto-negotiation: Yes
        Advertised link modes:  10baseT/Half 10baseT/Full
                                100baseT/Half 100baseT/Full
        Advertised auto-negotiation: Yes
        Speed: 100Mb/s
        Duplex: Full
        Port: MII
        PHYAD: 1
        Transceiver: internal
        Auto-negotiation: on
        Current message level: 0x00000000 (0)
        Link detected: yes
\endverbatim

	\n 

	\section vlan VLAN support
	\n
	
	The driver supports IEEE 802.1Q VLAN (virtual LAN) tagging to setup several virtual LANs in one physical connection.
	For this the used kernel must be enabled for VLAN support. Either \p CONFIG_VLAN_8021Q must be set in the kernel configuration
	or the kernel module 8021q.ko must be loaded prior to using VLAN.\n

	\subsection vconfig using vconfig to instantiate a VLAN

	To instantiate one or more VLAN connections, the linux utility \p vconfig must be present on the target. For detailled usage
	check the manpage of \p vconfig. Here a short example is shown how to instantiate a VLAN with ID 810. If the physical Z87 core is
	named eth0 on the target, the VLAN can be generated with
\verbatim root@men-desktop:/home/men/f11mdis# vconfig add eth0 810
Added VLAN with VID == 810 to IF -:eth0:- \endverbatim
    and an IP address can be assigned with the \p ifconfig tool in the regular way: \n
	\n
    \p ifconfig \p eth0.810 \p 10.1.1.88
	\n

	\subsection vlanprio Setting VLAN priority values

	The VLAN contains a 3-bit field named PCP in which a priority for the data can be specified. This value can
	be set also with the vconfig tool. To assign a value of 4 to frames with socket buffer priority 0, the egress and
	ingress parameter of vconfig is used: \n
	\p vconfig \p set_egress_map \p eth0.810 \p 0 \p 4  \n \n

	When the VLAN ID and priority value is set like above, the generated VLAN frame can be shown like in the following example.\n
	Mind that there the physical interface eth0 is used, not the VLAN interface eth0.810. This is because the VLAN instance of the
    interface does not support the ethtool calls to set the message levels. In the frame dump the resulting VLAN tag 
	\p 81 \p 00 \p 83 \p 2a can be seen.
	(0x8100 = VLAN header, 0x832a = PCP value 4, VLAN ID 0x32a (810).
\verbatim 
root@men-desktop:/home/men/f11mdis# ethtool -s eth0 msglvl 3; ping 10.1.1.20 -c 3; ethtool -s eth0 msglvl 0
root@men-desktop:/home/men/f11mdis# dmesg
...
[ 5016.897073] z77_send_packet[15] len 0x002a DMAadr 36b9f000
[ 5016.897097] VLAN frame: np->vlgrp = ef503580  vlan_id = 0x832a  vlan_tag = 0x2a830081
[ 5016.897201] 0x000: ff ff ff ff ff ff 00 c0 3a ff 00 01 81 00 83 2a 
[ 5016.897261] 0x010: 08 06 00 01 08 00 06 04 00 01 00 c0 3a ff 00 01 
[ 5016.897320] 0x020: 0a 01 01 58 00 00 00 00 00 00 0a 01 01 14 
[ 5016.897401] Z77 INTSRC: 01 [TXB]
[ 5016.897517] Z77 INTSRC: 04 [RXF]
[ 5016.897538] --> z77_poll:
[ 5016.897558] z77_pass_packet[34]: pktlen=0040
[ 5016.897568] Frame:
[ 5016.897578] 0x000: 00 c0 3a ff 00 01 00 30 05 ad 55 e1 81 00 03 2a 
[ 5016.897635] 0x010: 08 06 00 01 08 00 06 04 00 02 00 30 05 ad 55 e1 
[ 5016.897693] 0x020: 0a 01 01 14 00 c0 3a ff 00 01 0a 01 01 58 00 00 
[ 5016.897753] 0x030: 00 00 00 00 00 00 00 00 00 00 00 00 5a e6 f6 c0 
[ 5016.897823] <-- z77_poll:
...
\endverbatim


	\section trouble Troubleshooting

	If you have problems using the driver check the following points before contacting MEN support:

	- Problems regarding module load
	      - Does the version of the built modules match the running kernel? This can happen on self-hosted build environments with several kernel sources installed in /usr/src/. Execute e.g. the commands \p modinfo \p men_lx_z77 and \p uname \p -a to see if the versions match.
		  - Have the module dependencies been updated? 
		    Run the \p depmod command to rebuild the module dependencies file if an 'unresolved symbols' error occurs.

	- Problems with Ethernet communication
	      - If no communication seems to occur check if the link status changes when plugging the network cable in and out.
		  - Try to use the ping command to reach the external communication partner.
		  - Check if the interrupt count increases as ping commands are sent. The proc file /proc/interrupts can be used to do this:
\verbatim
/#cat /proc/interrupts
           CPU0
  5:        539   MPC52xx   Level     16Z087
 39:      20425   MPC52xx   Level     mpc52xx_psc_uart
 53:      10121   MPC52xx   Level     i2c-mpc
 54:        140   MPC52xx   Level     i2c-mpc
BAD:          0
\endverbatim
          - If the status LEDs work properly but the device eth(x) is inaccessible: check if the IP cores inside the P511 FPGA were found. Run the \p dmesg command (or do \p cat \p /var/log/messages) to see if the \p men_lx_chameleon driver detected the IP cores. As an example the output of the \p dmesg command on the used test system with a P511 looks like this:
\verbatim
 Init MEN Chameleon PNP subsystem. Build: Feb  5 2009 / 17:07:19

 Found MEN Chameleon FPGA at bus 4 dev 0d
 FPGA File='' table model=0x41('A') Revision 0 Magic 0xABCE
  Unit                devId   Grp Rev    Inst	IRQ	BAR Offset   Addr
 -------------------------------------------------------------------------------
  00 16Z024_SRAM      0x0018  0   17   0x00	0x3f	0   0x0000   0xfbffe000
  01 16Z045_FLASH     0x002d  0    4   0x00	0x3f	0   0x0100   0xfbffe100
  02 16Z001_SMB       0x0001  0    8   0x00	0x00	0   0x0200   0xfbffe200
  03 16Z087_ETH       0x0057  0    6   0x00	0x01	0   0x0300   0xfbffe300
  04 16Z087_ETH       0x0057  0    6   0x01	0x02	0   0x0400   0xfbffe400
  05 16Z034_GPIO      0x0022  0   16   0x00	0x03	0   0x0500   0xfbffe500
  06 16Z084_IDEEPROM  0x0054  0    2   0x00	0x3f	0   0x0600   0xfbffe600
  07 16Z052_GIRQ      0x0034  0    6   0x00	0x3f	0   0x0700   0xfbffe700
  08 16Z043_SDRAM     0x002b  0    9   0x00	0x3f	1   0x0000   0xf8000000
\endverbatim
Here the Units number 3 and 4 represent the 2 Ethernet cores. The Unit 8 is the SDRAM controller which stores Ethernet frames locally if parameter \p nodma is set. \n
	
	\subsection ethtool Switching on message level 
	To get detailed information about called procedures in the driver, the 
	debugging message level can be increased to a value up to 3 using the 
	\c msglvl command of ethtool. The values have the following meaning:
	- msglvl 0: all messages disabled, normal function
	- msglvl 1: only some function entries are shown
	- msglvl 2: more detailed messages
	- msglvl 3: maximum detailed messages including the driver's interrupt service routine
	            and sent / received ethernet frames

	The following example shows how to enable the debug messages:

\verbatim
/#ping 10.1.1.10
PING 10.1.1.10 (10.1.1.10): 56 data bytes
84 bytes from 10.1.1.10: icmp_seq=0 ttl=64 time=87.1 ms
84 bytes from 10.1.1.10: icmp_seq=1 ttl=64 time=0.2 ms
84 bytes from 10.1.1.10: icmp_seq=2 ttl=64 time=0.2 ms
84 bytes from 10.1.1.10: icmp_seq=3 ttl=64 time=0.2 ms
84 bytes from 10.1.1.10: icmp_seq=4 ttl=64 time=0.2 ms

--- 10.1.1.10 ping statistics ---
5 packets transmitted, 5 packets received, 0% packet loss
round-trip min/avg/max = 0.2/17.5/87.1 ms

/#ethtool -s eth0 msglvl 3
/#
/#ping 10.1.1.10
PING 10.1.1.10 (10.1.1.10): 56 data bytes
[19773.315172] z77_send_packet[30] len 0x0076 DMAadr 06d10000
[19773.324083] Z77 INTSRC: 01 [TXB]
[19773.327288] Z77 INTSRC: 04 [RXF]
[19773.330493] --> z77_poll:
[19773.333086] z77_pass_packet[1]: pktlen=007a
[19773.337258] <-- z77_poll:
84 bytes from 10.1.1.10: icmp_seq=0 ttl=64 time=24.8 ms
[19774.342014] z77_send_packet[31] len 0x0076 DMAadr 06d16000
[19774.347481] Z77 INTSRC: 01 [TXB]
[19774.350688] Z77 INTSRC: 04 [RXF]
[19774.353886] --> z77_poll:
[19774.356477] z77_pass_packet[2]: pktlen=007a
[19774.360649] <-- z77_poll:
84 bytes from 10.1.1.10: icmp_seq=1 ttl=64 time=21.3 ms

--- 10.1.1.10 ping statistics ---
2 packets transmitted, 2 packets received, 0% packet loss
round-trip min/avg/max = 21.3/23.0/24.8 ms
\endverbatim
\n \n \n
\section addinfo Additional information sources about network utilities

For more information about the network configuration commands like \p ethtool,
\p ifconfig and \p ping see the manual pages that can be displayed with the 
\p man command. It shows all available options and usage info about a command.
\verbatim
tschnuer@tslinux2:~/work> man ethtool

ETHTOOL(8)

NAME
       ethtool - Display or change ethernet card settings
[...]
\endverbatim
 
*/


/** \page dummy
  \menimages
*/

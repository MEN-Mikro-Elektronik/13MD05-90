VME4L README File
-----------------

VME4L is a kernel mode driver that allows userland applications and kernel
mode drivers to access the VMEbus, handle interrupts and make use of
other VME bridge functions.


See the following files for documentation about VME4L:

- INSTALL_VME4L.txt                     - Installation instructions
- LIBSRC/VME4L_API/DOC/html/index.html  - VME4L userland API


Supported Boards and VME Bridges
--------------------------------

The current release supports following boards:

  Board       Architecture     VME bridge     Bridge driver Required PLD 
  ----------  ---------------  -------------- ------------- ------------
  MEN A20     Intel Core Duo   Tundra TSI148  vme4l-tsi148       -
  MEN A19     Intel Core Duo   Tundra TSI148  vme4l-tsi148       -
  MEN A17     PowerPC 854x     Tundra TSI148  vme4l-tsi148       -
  MEN A15     PowerPC 8245     MEN PLDZ002    vme4l-pldz002 PLD >= Rev. 7
												recommended:PLD >= Rev. 18 	
  MEN A12     PowerPC 8245     MEN A12	      vme4l-pldz002     any
  MEN B11     PowerPC 8245     MEN A12	      vme4l-pldz002     any
  MEN A13     PentiumIII       MEN PLDZ002    vme4l-pldz002 PLD >= Rev. 7
												recommended:PLD >= Rev. 18 	
  

Supported Kernel Releases
-------------------------

VME4L currently has been tested under Linux 2.4.18 to 2.6.24 under the
following distributions:

SLIND Clydesdale/PPC (2.6.24)
Ubuntu 8.04/i686 (2.6.24), including SMP kernel
ELinOS 2.1/PPC (2.4.18)
ELinOS 2.2/X86 (2.4.18), including preemptible kernel
ELinOS 3.0/PPC (2.4.25), including preemptible kernel
ELinOS 4.2/PPC (2.4.31)
SuSE 8.2/i386  (2.4.20)
SuSE 9.0/i386  (2.4.21)
SuSE 9.1/i386  (2.6.4)


Compatibility to Previous VME4L Version
---------------------------------------

On PowerPC, this release of VME4L is source and binary compatible to the
previous vme-menpci2vme driver, although the old userland interface is
no longer documented and no longer recommended.

The current release has been rewritten from scratch and has a new userland
API, using the libvme4l_api.so (.a) library.


License
-------
VME4L is Copyright 2003-2009 MEN Mikro Elektronik GmbH, and is free software;
you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation,
Inc., 675 Mass Ave, Cambridge MA 02139, USA; either version 2 of the
License, or (at your option) any later version.


Known Problems
--------------

- Rev <=7 of PLDZ002 does not support DMA into main memory.  Therefore
  an intermediate buffer (top 256K of the PLD's shared RAM) is
  used. This slows down DMA transfers to approx.  5 MB/s.  

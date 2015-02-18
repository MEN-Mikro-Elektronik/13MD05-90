Windows specifics for VMEbus carrier boards                        2/22/2007 dp
===========================================


Requirements
------------
A special VME4WIN driver from MEN must be installed for the used VMEbus system
prior to the installation of a driver for a VMEbus carrier board. Please
contact MEN (www.men.de) to obtain the suitable VME4WIN software package for
your MEN VMEbus system. 


Swapped Drivers
---------------
Swapped MDIS driver variants are required for the MEN VMEbus systems at the MEN 
A19 and A20.
But now swapped MDIS driver variants are required for the MEN A13 VMEbus systems.


Installing the Board Driver
---------------------------
* You have to install the board driver (e.g. A201, A203N) manually, because the
  VMEbus does not support PnP.

* Use the Add Hardware Wizard accessible via the Control Panel or hdwwiz 
  from command line at Windows 7 to install the board driver.

* Choose the proper wizard options to install a new device manually and specify
  the location of the inf-file belonging to the VMEbus carrier board (e.g.
  a201.inf, a203n.inf).
  Then select the correct driver (e.g. A201/B201/B202) for the carrier board and
  continue the installation with the wizard.

* After the wizard installation was finished, Windows may prompt you to restart
  the computer. At this point you have to decline the restart and to configure
  the installed BBIS board device: 
	
	- Open the Device Manager and double-click the new installed device listed
	  under BBIS boards in the device tree to view the property sheet.
	
	- Switch to the Device Settings tab and press the Configure button.
	
	- Now, you have to set the VME address of your carrier board according to
	  the hardware configuration. You also have to specify the IRQ vector and
	  level you want to use for each slot.
	  The 'Force Found' option forces for a slot that an unknown M-Module will
	  be reported regardless if there is one plugged. This option should be
	  enabled if a plugged M-Module on the carrier board may not be found (e.g.
	  an M-Module without an ID-PROM).
    
    - Click the OK button, then close the property sheet.
    
    - Windows may prompt you again to restart the computer. This is necessary if
      drivers for the plugged M-Modules are already installed.

* If you have successfully installed the board driver, the driver should
  recognize the plugged M-Modules automatically and the Found New Hardware
  Wizard should appear for each found M-Module to guide you through the
  installation of the matching device drivers.
  
* If no M-Module is found, you have to restart the computer. If still no
  M-Module is found after the restart, then open the Device Settings tab of the
  VMEbus carrier board and enable the 'Force Found' option for the slots where
  M-Modules are plugged.

Please refer to the MDIS4 under Windows user manual for further information
about the installation of MEN's Windows MDIS drivers.

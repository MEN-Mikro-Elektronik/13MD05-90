/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: nat_module.c
 *      Project: MDIS4LINUX
 *
 *       Author: ts
 *
 *     Description: Module stuff for CHAMELEON MDIS module
 *
 *
 *---------------------------------------------------------------------------
 * Copyright 2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/msi.h>
#include <linux/version.h>

#include <MEN/men_typs.h>

#define A21_MSI_BLOCK_SIZE    32 

/* not present in pci_ids.h of kernel 3.4.18 yet... */
#define PCI_VENDOR_ID_MEN_MIKRO         0x1a88
#define PCI_DEVICE_ID_MEN_MIKRO_CHAM    0x4d45

/*****************************  A21_MSI_enable ******************************
 *
 *  Description:  dummy helper to create an artificial dependency so this
 *                is loaded before A21 BBIS is opened
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  returns: 0 on success
 *  Globals....:  -
 ****************************************************************************/
int A21_MSI_enable(void)
{
  return 0;
}
EXPORT_SYMBOL(A21_MSI_enable);

/*****************************  init_module  *********************************
 *
 *  Description:  Called when module is loaded by insmod
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  returns: 0 on success
 *  Globals....:  -
 ****************************************************************************/
int mod_init(void)
{
	struct pci_dev *pdev = NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
	struct msix_entry *entries = NULL;
#endif
	int error;
	int nvec;
	printk( KERN_INFO "MEN " COMP_NAME " init_module.\n");

	pdev = pci_get_device(PCI_VENDOR_ID_MEN_MIKRO, PCI_DEVICE_ID_MEN_MIKRO_CHAM, pdev);
	if (!pdev) {
		printk(KERN_ERR "PCI device not found\n");
		return -ENODEV;
	}

	if (pdev->msi_enabled) {
	  printk( KERN_INFO "MSIs enabled already, leaving.\n");
	  goto out;
	}

	error = pci_enable_device(pdev);
	if (error) {
		printk(KERN_ERR "pci_enable_device failed\n");
		return -ENODEV;
	}

	pci_set_master(pdev);

	if ( pci_msi_enabled()) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0)
	  if((nvec = pci_enable_msi_block(pdev, A21_MSI_BLOCK_SIZE ))) {
	    if (pci_enable_msi_block(pdev, nvec) < 0) {
	      printk(KERN_ERR " *** failed to request MSI, falling back to legacy IRQs\n");
	    }
	  }
#else
	  if((nvec = pci_enable_msix_exact(pdev, entries, A21_MSI_BLOCK_SIZE ))) {
	    if (pci_enable_msix_exact(pdev, entries, nvec) < 0) {
	      printk(KERN_ERR " *** failed to request MSI, falling back to legacy IRQs\n");
	    }
	  }
#endif
	}
	printk( KERN_INFO "MSIs enabled successfully.\n");
out:
	return 0;
}

/*****************************  cleanup_module  ******************************
 *
 *  Description:  Called before module is unloaded by rmmod
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
void mod_exit(void)
{
	printk( KERN_INFO "MEN " COMP_NAME " cleanup_module\n");
}

module_init( mod_init );
module_exit( mod_exit );
MODULE_DESCRIPTION( COMP_NAME " MDIS module");
MODULE_AUTHOR("Thomas Schnuerer <thomas.schnuerer@men.de>");
MODULE_LICENSE("GPL");
#ifdef MAK_REVISION
MODULE_VERSION(MENT_XSTR(MAK_REVISION));
#endif

/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: nat_module.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2007/12/10 12:56:29 $
 *    $Revision: 1.5 $
 *
 *  Description: Module stuff for CHAMELEON MDIS module
 *
 *     Required: -
 *     Switches: COMP_NAME		name of component as a string
 *				 DBG_MODULE		set when compiling DBG module
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: nat_module.c,v $
 * Revision 1.5  2007/12/10 12:56:29  ts
 * CHAMELEONV2_EXPORT_CHAMELEONV0 not longer defined here
 * Exporting CHAM_Init, CHAM_DevIdToModCode
 *
 * Revision 1.4  2007/08/09 19:21:23  ts
 * Now explicitely define CHAMELEONV2_EXPORT_CHAMELEONV0
 *
 * Revision 1.3  2006/08/04 10:55:13  ts
 * + EXPORT_SYMBOL CHAM_ModCodeToDevId/CHAM_InitMem/CHAM_DevIdToName
 *
 * Revision 1.2  2005/07/08 14:41:38  ub
 * ChameleonHwName() removed
 *
 * Revision 1.1  2004/06/09 11:12:20  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
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
#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#ifndef MAC_MEM_MAPPED
 #define MAC_MEM_MAPPED
#endif
#include <MEN/chameleon.h>

/* all functions must be explicitely exported in Linux 2.6...*/
EXPORT_SYMBOL(CHAM_Init);
EXPORT_SYMBOL(CHAM_ModCodeToDevId);
EXPORT_SYMBOL(CHAM_DevIdToModCode);
EXPORT_SYMBOL(CHAM_DevIdToName);
#ifdef CHAMELEONV2_EXPORT_CHAMELEONV0
 EXPORT_SYMBOL(ChameleonInit);
 EXPORT_SYMBOL(ChameleonGlobalIdent);
 EXPORT_SYMBOL(ChameleonUnitIdent);
 EXPORT_SYMBOL(ChameleonUnitFind);
 EXPORT_SYMBOL(ChameleonModName);
 EXPORT_SYMBOL(ChameleonTerm);
#endif /* CHAMELEONV2_EXPORT_CHAMELEONV0  */

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
    printk( KERN_INFO "MEN " COMP_NAME " init_module\n");
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
MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

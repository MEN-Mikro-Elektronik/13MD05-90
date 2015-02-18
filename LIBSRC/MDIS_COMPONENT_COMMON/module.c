/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: module.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2004/06/09 11:12:43 $
 *    $Revision: 1.3 $
 *
 *  Description: Common functions for all MDIS module components
 *
 *     Required: -
 *     Switches: COMP_NAME		name of component as a string
 *				 DBG_MODULE		set when compiling DBG module
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: module.c,v $
 * Revision 1.3  2004/06/09 11:12:43  kp
 * Linux 2.6 support
 *
 * Revision 1.2  2003/02/21 11:39:02  kp
 * added module description, author and license
 *
 * Revision 1.1  2001/01/19 14:58:41  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
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
MODULE_DESCRIPTION( COMP_NAME "MDIS module");
MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_module.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2004/06/09 11:12:47 $
 *    $Revision: 1.3 $
 *
 *  Description: Glue code for all Linux MDIS board drivers
 *
 *     Required: -
 *     Switches: COMP_NAME		name of component as a string
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_module.c,v $
 * Revision 1.3  2004/06/09 11:12:47  kp
 * pass THIS_MODULE to bbis_register_bb_driver
 *
 * Revision 1.2  2003/02/21 12:22:14  kp
 * added module description, license, author
 *
 * Revision 1.1  2001/01/19 14:58:43  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#define EXPORT_SYMTAB

#if !defined(MAC_MEM_MAPPED) && !defined(MAC_IO_MAPPED)
# define MAC_MEM_MAPPED
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/desc.h>
#include <MEN/bb_defs.h>
#include <MEN/bb_entry.h>
#include <MEN/bbis_bk.h>

extern void BBIS_GetEntry( BBIS_ENTRY *bbisP );

static BBIS_ENTRY G_orgEnt, G_ent;

/*
 * The following three routines serve as wrappers around the BBIS driver's
 * GetEntry, Init and Exit function. The purpose is just to increment
 * and derement the module's usage counter so that nobody can remove a module
 * that is in use.
 */
static void GetEntryWrapper( BBIS_ENTRY *ent )
{
	*ent = G_ent;
}

static int32 InitWrapper(
	OSS_HANDLE *osh,
	DESC_SPEC *desc,
	BBIS_HANDLE **bbP)
{
	int32 error;

	error = G_orgEnt.init( osh, desc, bbP );
	return error;
}

static int32 ExitWrapper( BBIS_HANDLE **bbP )
{
	return G_orgEnt.exit( bbP );
}

/*****************************  init_module  *********************************
 *
 *  Description:  Called when module is loaded by insmod
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  returns: 0 on success
 *  Globals....:  -
 ****************************************************************************/
int init_module(void)
{
	printk( KERN_INFO "MEN " COMP_NAME " init_module\n");

	BBIS_GetEntry( &G_orgEnt );		/* get original function pointers */
	G_ent = G_orgEnt;				/* copy all pointers */
	G_ent.init = InitWrapper;		/* replace pointers for wrapper functions*/
	G_ent.exit = ExitWrapper;
	return bbis_register_bb_driver( COMP_NAME, GetEntryWrapper, THIS_MODULE );
}

/*****************************  cleanup_module  ******************************
 *
 *  Description:  Called before module is unloaded by rmmod
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
void cleanup_module(void)
{
	printk( KERN_INFO "MEN " COMP_NAME " cleanup_module\n");
    (void)bbis_unregister_bb_driver( COMP_NAME );
}

MODULE_DESCRIPTION( COMP_NAME " BBIS driver");
MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

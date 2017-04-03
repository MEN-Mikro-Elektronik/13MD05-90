/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: ll_module.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2004/06/09 11:12:45 $
 *    $Revision: 1.3 $
 *
 *  Description: Glue code for all Linux MDIS board drivers
 *
 *     Required: -
 *     Switches: COMP_NAME		name of component as a string
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: ll_module.c,v $
 * Revision 1.3  2004/06/09 11:12:45  kp
 * pass THIS_MODULE to mdis_register_ll_driver
 *
 * Revision 1.2  2003/02/21 12:22:35  kp
 * added module description, license, author
 *
 * Revision 1.1  2001/01/19 14:58:42  kp
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
#include <MEN/maccess.h>
#include <MEN/ll_defs.h>
#include <MEN/ll_entry.h>
#include <MEN/mdis_mk.h>

extern void LL_GetEntry( LL_ENTRY * );

static LL_ENTRY G_orgEnt, G_ent;

/*
 * The following three routines serve as wrappers around the LL driver's
 * GetEntry, Init and Exit function. The purpose is just to increment
 * and derement the module's usage counter so that nobody can remove a module
 * that is in use.
 */
static void GetEntryWrapper( LL_ENTRY *ent )
{
	*ent = G_ent;
}

static int32 InitWrapper(
	DESC_SPEC *desc,
	OSS_HANDLE *osh,
	MACCESS *ma,
	OSS_SEM_HANDLE *devSem,
	OSS_IRQ_HANDLE *irqH,
	LL_HANDLE **llP)
{
	int32 error;

	error = G_orgEnt.init( desc, osh, ma, devSem, irqH, llP );
	if( error == 0 ){
		MOD_INC_USE_COUNT;
	}
	return error;
}

static int32 ExitWrapper( LL_HANDLE **llP )
{
	MOD_DEC_USE_COUNT;
	return G_orgEnt.exit( llP );
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

	LL_GetEntry( &G_orgEnt );		/* get original function pointers */
	G_ent = G_orgEnt;				/* copy all pointers */
	G_ent.init = InitWrapper;		/* replace pointers for wrapper functions*/
	G_ent.exit = ExitWrapper;

	return mdis_register_ll_driver( COMP_NAME, GetEntryWrapper, THIS_MODULE );
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
    (void)mdis_unregister_ll_driver( COMP_NAME );
}

MODULE_DESCRIPTION( COMP_NAME " MDIS low level driver");
MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bk_module.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *
 *  Description: Main file for BBIS kernel module
 *				 Contains the Linux filesystem's entry points
 *     Required: -
 *     Switches: DBG
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
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

#define EXPORT_SYMTAB
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#include "bk_intern.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define BK_DRV_PREFIX "men_bb_"
#define PROC_BUF_LEN   2048  /* local buffer for new proc read fops read function */

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
u_int32 bk_dbglevel		= OSS_DBG_DEFAULT;	/* debug level */
u_int32 G_initialOpen 	= 1;				/* SC24: initial FPGA remap */

		
OSS_HANDLE		*G_osh;							/* OSS handle */
DBG_HANDLE 		*G_dbh;							/* debug handle */
OSS_SEM_HANDLE  *G_bkLockSem; 					/* global BK sempahore */
OSS_DL_LIST		G_drvList;						/* list of reg. LL drivers */
OSS_DL_LIST		G_devList;						/* list of devices */
OSS_DL_LIST		G_freeUsrBufList;	/* list of free user buffers */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int32 OpenDevice( DESC_SPEC *bbDesc, BK_DEV *node );
static void strtolower( char *s );
static BK_DRV *FindDrvByName( char *name );

/*--- symbols exported by BBIS kernel module ---*/
EXPORT_SYMBOL(bbis_register_bb_driver);
EXPORT_SYMBOL(bbis_unregister_bb_driver);
EXPORT_SYMBOL(bbis_open);
EXPORT_SYMBOL(bbis_close);
EXPORT_SYMBOL(bbis_ident);

/*********************************** bbis_ident *******************************
 *
 *  Description:  Return ident string
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
char* bbis_ident( void )
{
	return("BK - BDIS Kernel: $Id: bk_module.c,v 1.9 2014/07/29 14:35:51 ts Exp $");
}

/****************************** bbis_open ***********************************
 *
 *  Description:  Open a BBIS device
 *
 *	Checks if device is already initialized. If not, creates an instance
 *  of the BB driver and initializes the board
 *---------------------------------------------------------------------------
 *  Input......:  devName		device name of board (e.g. D201_1)
 *				  bbDesc		board descriptor data
 *  Output.....:  returns		0=ok, or negative error number
 *				  *bbHdlP		base board handle
 *				  bbEntries		filled with base board entries
 *  Globals....:  -
 ****************************************************************************/
int bbis_open(
	char *devName,
	DESC_SPEC *bbDesc,
	BBIS_HANDLE **bbHdlP,
	BBIS_ENTRY *bbEntries)
{
	int32 error;
	BK_DEV *node=NULL;
	int ret=0;

	DBGWRT_1((DBH,"bbis_open: %s\n", devName ));

	*bbHdlP = NULL;

	BK_LOCK(error);
	if( error ) return -error;

	if( strlen(devName) > BK_MAX_DEVNAME ){
		ret = -E2BIG;			/* name too long */
		goto errexit;
	}

	for( node=(BK_DEV *)G_devList.head;
		 node->node.next;
		 node = (BK_DEV *)node->node.next ){

		if( strcmp(node->devName, devName ) == 0 ){
			/* found node */
			DBGWRT_2((DBH," found\n"));
			break;
		}
	}
	if( node->node.next == NULL ){
		/*
		 * New device
		 */

		/* alloc memory for node */
		node = kmalloc( sizeof(*node), GFP_KERNEL );
		if( node == NULL ){
			ret = -ENOMEM;
			goto errexit;
		}

		memset( node, 0, sizeof(*node));
		strcpy( node->devName, devName );
		node->useCount = 0;

		/*--- Try to initialize the device ---*/
		error = OpenDevice( bbDesc, node );
		if( error ){
			ret = -error;
			goto errexit;
		}

		OSS_DL_AddTail( &G_devList, &node->node );
	}

	node->useCount++;
	*bbHdlP = node->bb;					/* return board handle */
	node->drv->getEntry( bbEntries ); 	/* get board function entry points */
	goto xit;

 errexit:
	if( node != NULL ) kfree(node);
 xit:
	DBGWRT_1((DBH,"bbis_open ex: %s ret=%d -0x%x\n", devName, ret, -ret ));
	BK_UNLOCK;
	return ret;
}

/****************************** OpenDevice ***********************************
 *
 *  Description:  Do the first open on the BBIS device
 *
 *---------------------------------------------------------------------------
 *  Input......:  bbDesc		board descriptor data
 *				  node			device node
 *  Output.....:  returns		0=ok, or (pos.) error number
 *				  node->bb		device handle
 *				  node->drv		ptr to driver structure
 *  Globals....:  -
 ****************************************************************************/
static int32 OpenDevice( DESC_SPEC *bbDesc, BK_DEV *node )
{
	int32 error, value;
	u_int32 strLen, isaAddr=0;
	char drvName[BK_MAX_DRVNAME+1];
	DESC_HANDLE *descHdl=NULL;
	BK_DRV *drvNode;
	BBIS_ENTRY ent;
	BBIS_HANDLE *bb = NULL;

	if( (error = DESC_Init( bbDesc, OSH, &descHdl ))){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't init dev desc "
					"err=0x%lx\n", error ));
		goto errexit;
	}

	if( (error = OSS_Init( node->devName, &node->osh )))
		goto errexit;

	/*--- get debug levels ---*/
	if ((error = DESC_GetUInt32(descHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_OSS")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto errexit;

	OSS_DbgLevelSet(node->osh, value);

	if ((error = DESC_GetUInt32(descHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_DESC")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto errexit;

	DESC_DbgLevelSet(descHdl, value);

	if ((error = DESC_GetUInt32(descHdl, OSS_DBG_DEFAULT, &DBG_MYLEVEL,
								"DEBUG_LEVEL_BK")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto errexit;

	error = DESC_GetUInt32(descHdl, 0, &isaAddr, "DEVICE_ADDR");

	/*--- get name of BBIS driver ---*/
	strcpy( drvName, BK_DRV_PREFIX );  /* preset module name prefix */
	strLen = BK_MAX_DRVNAME - strlen(drvName);

	if( (error = DESC_GetString( descHdl, "", &drvName[strlen(drvName)],
								 &strLen, "HW_TYPE"))){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't find HW_TYPE "
					"err=0x%lx\n", error ));
		goto errexit;
	}
	strtolower( drvName );
	DBGWRT_2((DBH," HW_TYPE=%s\n", drvName ));

	/*--- get debug level ---*/
	if( (error = DESC_GetUInt32( descHdl, OSS_DBG_DEFAULT,
								 &DBG_MYLEVEL,"DEBUG_LEVEL_BK" )) &&
		error != ERR_DESC_KEY_NOTFOUND ){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't find DEBUG_LEVEL_BK "
					"err=0x%lx\n", error ));
		goto errexit;
	}

	/* ... here jt USE_MSI descriptor read... */
	/* if (use_msi) */
	/*   bk_enable_msi(struct pci_dev); */

	DESC_Exit( &descHdl );		/* descriptor handle no longer required */


	/*--- search for driver ---*/

	/*
	 * first, let's check if the BBIS driver is already loaded.
	 * If not, try to load it automatically through kerneld
	 */
	if( (drvNode = FindDrvByName( drvName )) == NULL ){
		DBGWRT_2((DBH," try to load %s trough kerneld\n", drvName ));

		BK_UNLOCK;

		request_module( drvName );

		BK_LOCK(error);
		if( error ) {
			error = EINTR;
			goto errexit;
		}
	}

	if( (drvNode = FindDrvByName( drvName )) == NULL ){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't find driver %s\n",
				   drvName ));
		error = ERR_BK_NO_LLDRV;
		goto errexit;
	}

	/* increment module's use count */
	if( !try_module_get( drvNode->module ) ){
		DBGWRT_ERR((DBH, "*** BBIS:OpenDevice: try_module failed\n"));
		error = ERR_BK_NO_LLDRV;
		goto errexit;
	}
	node->drv = drvNode;

	/*--- create baseboard handle ---*/

	node->drv->getEntry( &ent ); 	/* get board function entry points */
	if( (error = ent.init( node->osh, bbDesc, &bb ))){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't create board "
					"handle on %s\n", node->devName ));
		bb = NULL;
		goto errexit;
	}

	node->bb = bb;

	/*--- initialize the board ---*/
	if( (error = ent.brdInit( bb ))){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't init board "
					"on %s\n", node->devName ));
		goto errexit;
	}

	goto xit;

 errexit:
	if( node->drv )
		module_put( node->drv->module );
	if( node->osh )
		OSS_Exit( &node->osh );
	if( descHdl )
		DESC_Exit( &descHdl );
	if( bb )
		ent.exit( &bb );
 xit:
	return error;
}


/****************************** bbis_close ***********************************
 *
 *  Description:  Close a BBIS device
 *
 *	The last close deinitializes the base board
 *---------------------------------------------------------------------------
 *  Input......:  devName		device name of board (e.g. D201_1)
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int bbis_close(char *devName)
{
	int32 error;
	int ret=0;
	BK_DEV *node=NULL;

	DBGWRT_1((DBH,"bbis_close: %s\n", devName ));

	BK_LOCK(error);
	if( error ) return -error;

	for( node=(BK_DEV *)G_devList.head;
		 node->node.next;
		 node = (BK_DEV *)node->node.next ){

		if( strcmp(node->devName, devName ) == 0 ){
			/* found node */
			DBGWRT_2((DBH," found\n"));
			break;
		}
	}
	if( node->node.next == NULL ){
		ret = -ENOENT;			/* device not open */
		goto errexit;
	}

	if( --node->useCount == 0 ){
		/*
		 * Final termination call. Deinit device
		 */
		BBIS_ENTRY ent;

		node->drv->getEntry( &ent ); 	/* get board function entry points */

		error = ent.brdExit( node->bb ); /* deinit board */
		if( error == 0 ){
			error = ent.exit( &node->bb ); /* destroy handle */
		}
		if( error )
			ret = -error;				/* save error, but exit normally */

		OSS_DL_Remove( &node->node ); 	/* detach device from list */
		OSS_Exit( &node->osh );			/* exit OSS */
		module_put( node->drv->module ); /* decrease module's use count */
		kfree(node);
	}

 errexit:
	DBGWRT_1((DBH,"bbis_close ex: %s ret=%d -0x%x\n", devName, ret, -ret ));
	BK_UNLOCK;
	return ret;
}

/****************************** FindDrvByName *********************************
 *
 *  Description:  Search for a driver in BBIS driver list
 *---------------------------------------------------------------------------
 *  Input......:  name			name to look for
 *  Output.....:  returns		ptr to driver struct or NULL if not found
 *  Globals....:  -
 ****************************************************************************/
static BK_DRV *FindDrvByName( char *name )
{
	BK_DRV *node;

	for( node=(BK_DRV *)G_drvList.head;
		 node->node.next;
		 node = (BK_DRV *)node->node.next ){

		if( strcmp(node->drvName, name ) == 0 )
			break;
	}

	if( node->node.next == NULL )
		node=NULL;

	return node;
}

/****************************** bbis_register_bb_driver **********************
 *
 *  Description:  Register an BBIS Driver in the BBIS kernel
 *---------------------------------------------------------------------------
 *  Input......:  bbName		name of low level driver e.g. "men_bb_d201"
 *				  getEntry		ptr to GetEntry function of BB driver
 *				  module		calling module structure
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int bbis_register_bb_driver(
	char *bbName,
	void (*getEntry)(BBIS_ENTRY *),
	struct module *module)
{
	BK_DRV *node;
	int32 error;

	DBGWRT_1((DBH,"bbis_register_bb_driver: name=%s getentry=%p\n",
			  bbName, getEntry));

	if( strlen(bbName) > BK_MAX_DRVNAME )
		return -E2BIG;			/* name too long */

	if( strncmp( BK_DRV_PREFIX, bbName, strlen(BK_DRV_PREFIX )) != 0){
		DBGWRT_ERR((DBH,"*** bbis_register_bb_driver: %s doesn't start with "
					"%s\n", bbName, BK_DRV_PREFIX ));
		return -EINVAL;
	}

	BK_LOCK(error);
	if( error )
		return -EINTR;

	/* check if already installed */
	if( (node = FindDrvByName( bbName ))){
		DBGWRT_ERR((DBH,"*** bbis_register_bb_driver: %s already registered\n",
					bbName ));
		BK_UNLOCK;
		return -EBUSY;
	}

	/* alloc memory for node */
	node = kmalloc( sizeof(*node), GFP_KERNEL );
	if( node == NULL ){
		BK_UNLOCK;
		return -ENOMEM;
	}

	strcpy( node->drvName, bbName );
	node->getEntry = getEntry;
	node->module   = module;

	OSS_DL_AddTail( &G_drvList, &node->node );

	BK_UNLOCK;
	return 0;
}

/****************************** bbis_unregister_bb_driver ********************
 *
 *  Description:  Un-registers an BBIS Low Level Driver in the BBIS kernel
 *---------------------------------------------------------------------------
 *  Input......:  bbName		name of low level driver e.g. "men_bb_d201"
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int bbis_unregister_bb_driver( char *bbName )
{
	BK_DRV *node;
	int32 error;

	DBGWRT_1((DBH,"bbis_unregister_bb_driver: name=%s\n", bbName));

	BK_LOCK(error);
	if( error )
		return -EINTR;

	node = FindDrvByName( bbName );
	if( node != NULL ){
		OSS_DL_Remove( &node->node );
		kfree(node);
		BK_UNLOCK;
		return 0;
	}
	BK_UNLOCK;
	return -ENOENT;				/* drv not registered */
}

#define INC_LEN if (len+begin > off+count) goto done;\
                if (len+begin < off) {\
				   begin += len;\
				   len = 0;\
				}

/****************************** bk_read_procmem ****************************
 *
 *  Description:  Function to fill in data when /proc/bbis file is read
 *
 *---------------------------------------------------------------------------
 *  Input......:  proc			page start
 *				  offset		offset within file
 *				  count			max bytes to read
 *				  data			?
 *  Output.....:  returns		0=ok, or negative error number
 *				  *start		ptr to first valid char in page
 *				  *eof			true if all characters output
 *  Globals....:  -
 ****************************************************************************/
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
static ssize_t bk_read_procmem( char *page, 
				char **start, 
				off_t off,
				int count, 
				int *eof, 
				void *data)
{
    int i, len = 0, rv;
	off_t begin = 0;

	DBGWRT_3((DBH,"bk_read_procmem: count %d page=%p\n", count, page));
	BK_LOCK(i);
	if( i ) return (-ERESTARTSYS);

	/* Drivers */
	len += sprintf( page+len, "\nDrivers\n" );
	{
		BK_DRV *node;
		for( node=(BK_DRV *)G_drvList.head;
			 node->node.next;
			 node = (BK_DRV *)node->node.next ){

			len += sprintf( page+len, "  %s\n", node->drvName );
			INC_LEN;
		}
	}

	/* Devices */
	len += sprintf( page+len, "Devices\n" );
	INC_LEN;

	{
		BK_DEV *node;
		for( node=(BK_DEV *)G_devList.head;
			 node->node.next;
			 node = (BK_DEV *)node->node.next ){
			len += sprintf( page+len, "  %s drv=%s "
							"usecnt=%d\n",
							node->devName,
							node->drv->drvName,
							node->useCount);
			INC_LEN;
		}
	}

 done:
	if (off >= len+begin){
		rv = 0;
		goto end;
	}
	*start = page + (off-begin);
	rv = ((count < begin+len-off) ? count : begin+len-off);

 end:
	DBGWRT_3((DBH,"bk_read_procmem: ex eof=%d rv=%d\n", *eof, rv));
	BK_UNLOCK;

	return rv;
}

#else /* newer kernel >= 3.10 */
static ssize_t bk_read_procmem( struct file *filp, char *buf, size_t count, loff_t *pos)
{

  int i, rv=0;
  char *locbuf;
  char *tmp;
  static int len=0;

  /* ts: page wise readers like cat read until no chars are returned,
     so keep len in a persistent static value and avoid reentering twice.*/
  if( len ) {
    len = 0;
    return 0;
  }

	locbuf = vmalloc(PROC_BUF_LEN);
	if (!locbuf)
		return -ENOMEM;
	tmp = locbuf;

  DBGWRT_3((DBH,"mk_read_procmem: count %d\n", count));
  BK_LOCK(i);

	memset(locbuf, 0x00, PROC_BUF_LEN);


  /* Drivers */
  len += sprintf( tmp+len, "\nDrivers:\n" );

  {
    BK_DRV *node;
    for( node=(BK_DRV *)G_drvList.head;
	 node->node.next;
	 node = (BK_DRV *)node->node.next )
      {
	len += sprintf( tmp+len, "  %s\n", node->drvName );
      }
  }
	
  /* Devices */
  len += sprintf( tmp+len, "Devices:\n" );
  {
    BK_DEV *node;
    for( node=(BK_DEV *)G_devList.head;
	 node->node.next;
	 node = (BK_DEV *)node->node.next ){

      len += sprintf( tmp+len, "  %s drv=%s usecnt=%d\n",
		      node->devName,
		      node->drv ? node->drv->drvName : "?",
		      node->useCount);
    }
  }

  len+=copy_to_user(buf, locbuf, len );

	vfree(locbuf);

  (void)rv;
  DBGWRT_3((DBH,"bk_read_procmem: ex rv=%d\n", rv));
  BK_UNLOCK;
  return len;
}
#endif


/* since kernel 3.10 new proc entry fops are needed */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
static struct file_operations bk_proc_fops = {
     .read=      	bk_read_procmem,
};
#endif


/***************************
 * Finally, the module stuff
 */
int init_module(void)
{
	int ret=0;

	DBGINIT((NULL,&DBH));

	printk( KERN_INFO "MEN BBIS Kernel init_module\n");

	/* init OSS */
	if( OSS_Init( "BBIS_KERNEL", &OSH )){
		ret = -ENOMEM;
		goto clean0;
	}

	/* create global BBIS lock sem */
	if( OSS_SemCreate(OSH, OSS_SEM_BIN, 1, &G_bkLockSem )){
		ret = -ENOMEM;
		goto clean1;
	}

	/* init lists */
	OSS_DL_NewList( &G_drvList );
	OSS_DL_NewList( &G_devList );

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
	create_proc_read_entry ("bbis", 0, NULL, (read_proc_t *)bk_read_procmem, NULL);
#else
	proc_create (           "bbis", 0, NULL, &bk_proc_fops);
#endif

	goto clean0;

 clean1:
	OSS_Exit( &OSH );
 clean0:

	return 0;
}


void cleanup_module(void)
{
	OSS_SemRemove( OSH, &G_bkLockSem );
	OSS_Exit( &OSH );

    remove_proc_entry ("bbis", 0);

	DBGEXIT((&DBH));
	printk( KERN_INFO "MEN BBIS Kernel cleanup_module\n");
}

static void strtolower( char *s )
{
	while( *s ){
		if( *s >= 'A' && *s <= 'Z' )
			*s += 'a' - 'A';
		s++;
	}
}

MODULE_DESCRIPTION("MEN BBIS kernel");
MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

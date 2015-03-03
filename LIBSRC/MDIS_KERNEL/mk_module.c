/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: mk_module.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2014/07/21 17:38:25 $
 *    $Revision: 1.26 $
 *
 *  Description: Main file for MDIS kernel module
 *				 Contains the Linux filesystem's entry points
 *     Required: -
 *     Switches: DBG
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mk_module.c,v $
 * Revision 1.26  2014/07/21 17:38:25  ts
 * R: 1. compiler warning about return type of proc_memread
 *    2. compiler warning about unused return value of copy_to_user
 * M: 1. correct return value type from int to ssize_t
 *    2. perform dummy add of copy_to_user return value
 *
 * Revision 1.25  2014/07/14 17:04:16  ts
 * R: 1. proc interface changed from 3.10 on
 *    2. RTAI no longer supported
 * M: 1. use new API with proc fops depending on kernel version
 *    2. removed RTAI enabling code
 *
 * Revision 1.24  2013/10/24 09:58:55  ts
 * R: gcc 4.6 under Ubuntu 12.10 warned about type mismatch of mk_dbglevel
 * M: changed int32 to int, needed for module parameter declaration to kernel
 *
 * Revision 1.23  2013/08/29 16:12:58  ts
 * R: (D.Robinson) Locking occured in multithreaded application
 * M: Remove semaphore protection in mk_unl_ioctl
 *
 * Revision 1.23  2013/08/09 10:24:30 dr
 * R: Locking occured in multithreaded application
 * M: Remove semaphore protection in mk_unl_ioctl
 *
 * Revision 1.22  2012/05/14 20:23:50  ts
 * R: Compiler warned about data type declaration of parameter mk_dbglevel
 * M: Changed data type to ulong instead u_int
 *
 * Revision 1.21  2012/04/25 18:03:10  ts
 * R: build failed on Ubuntu kernel 2.6.32
 * M: removed ifdef statement in mk_ioctl for kernels < 2.6.36
 *
 * Revision 1.20  2012/04/23 12:35:56  ts
 * R: 1. previous MDIS version used BKL, is now depecated and removed
 *    2. #ifdef to decide whether unlocked ioctl method is to be used was wrong
 * M: 1. added semaphore to protect ioctls if HAVE_UNLOCKED_IOCTL defined
 *    2. replaced with check for HAVE_KERNEL_UNLOCKED
 *
 * Revision 1.19  2011/05/23 11:34:04  CRuff
 * R: 1. support for kernel 2.6.37
 * M: 1a) for kernels > 2.6.36, replace entry point mk_ioctl by mk_unl_ioctl
 *    1b) explicitly call lock_kernel() before calling mk_ioctl method
 *
 * Revision 1.18  2009/08/10 12:18:33  CRuff
 * R: cleanup
 * M: remove debug prints and unnecessary statements
 *
 * Revision 1.17  2009/08/04 16:57:03  CRuff
 * R: module parameter mk_dbglevel causes type incompatibilities
 *    on 64 bit systems
 * M: change type of mk_dbglevel to uint (32bit on ILP32 and LP64 data models)
 *
 * Revision 1.16  2007/12/10 13:01:06  ts
 * rename MDIS_IrqHandler to macro MDIS_IRQHANDLER defined by kernel version
 * let compilation break with #error when kernel 2.6 and CONFIG_DEVFS_FS selected
 * (possible in ElinOS elk tool)
 *
 * Revision 1.15  2007/12/03 18:09:41  ts
 * change rights in module_param_array into 0664, 0666 causes error at 2.6.19
 * and up
 *
 * Revision 1.14  2007/03/28 17:09:33  ts
 * Bugfix: Device wasnt registered when devfs not used
 *
 * Revision 1.13  2007/02/23 19:27:30  ts
 * Bugfix: register_chrdev not called for 2.4 kernels
 *
 * Revision 1.12  2007/01/08 12:24:15  ts
 * make devfs usage depending on CONFIG_DEVFS_FS (removed in 2.6.17)
 *
 * Revision 1.11  2006/08/04 11:00:59  ts
 * removed warning
 *
 * Revision 1.10  2006/06/01 12:17:00  ts
 * correct last module_param Parameter:is permission,not init Value
 *
 * Revision 1.9  2006/06/01 11:56:01  ts
 * change MODULE_PARM to module_param for kernel >2.6.13
 *
 * Revision 1.8  2005/01/19 15:46:24  ts
 * corrected bug with devfs registration of mdis device under 2.6
 *
 * Revision 1.7  2004/06/09 11:12:32  kp
 * Linux 2.6 support
 *
 * Revision 1.6  2003/04/11 16:26:22  kp
 * finalized RTAI port
 *
 * Revision 1.5  2003/02/21 13:34:41  kp
 * added support for RTAI and non-mdis drivers
 * added module description, author, license
 *
 * Revision 1.4  2002/05/06 11:52:51  kp
 * support device filesystem (CONFIG_DEVFS_FS)
 *
 * Revision 1.3  2001/09/18 15:18:45  kp
 * added mdis_find_ll_handle to access LL driver from another LL driver
 *
 * Revision 1.2  2001/08/16 10:15:53  kp
 * added error number compression for PPC
 *
 * Revision 1.1  2001/01/19 14:58:35  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2012 by MEN mikro elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#include "mk_intern.h"
#include <linux/module.h>
#include <linux/moduleparam.h>

#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#define PROC_BUF_LEN   2028  /* local buffer for new proc read fops read function */

/*
 * the maximum size of a M_getblock/M_setblock or block Get/Setstat call
 * that will fit into the statically allocated users buffers. Larger requests
 * will be kmalloc'ed or vmalloc'ed
 */
#define MK_USRBUF_SIZE (PAGE_SIZE-sizeof(OSS_DL_NODE))

/* Sanity check: ElinOS allows devfs still
 * to be selected in the elk even for kernels 2.6.x where its gone
 */
#if defined(CONFIG_DEVFS_FS) && (LINUX_VERSION_CODE > VERSION_CODE(2,6,9))
# error "!! kernel > 2.6.9 with device file support selected! DEVFS is"
# error "!! removed in kernels 2.6.9 and up. If you"
# error "!! build an MDIS Project for ElinOS, deselect devfs in 'Filesystems'"
# error "!! Settings and select UDEV instead. "
#endif

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--- Module parameters ---*/
static int mk_major 	= 	MK_MAJOR; 	/* major device number for /dev/mdis */
static int mk_nbufs 	= 	16;			/* number of static users buffers to allocate*/
int mk_dbglevel 	=	OSS_DBG_DEFAULT;/* debug level */

#if LINUX_VERSION_CODE < VERSION_CODE(2,6,14)
MODULE_PARM( mk_major, "i" );
MODULE_PARM_DESC(mk_major, "MDIS major number");
MODULE_PARM( mk_nbufs, "i" );
MODULE_PARM_DESC(mk_nbufs, "number of static users buffers to allocate");
MODULE_PARM( mk_dbglevel, "i" );
MODULE_PARM_DESC(mk_dbglevel, "MDIS kernel debug level");
#else
module_param( mk_major, int, 0664 );
MODULE_PARM_DESC(mk_major, "MDIS major number");
module_param( mk_nbufs, int, 0664 );
MODULE_PARM_DESC(mk_nbufs, "number of static users buffers to allocate");
module_param( mk_dbglevel, int, 0664 );
MODULE_PARM_DESC(mk_dbglevel, "MDIS kernel debug level");
#endif

OSS_HANDLE		*G_osh;							/* MK's OSS handle */
DBG_HANDLE 		*G_dbh;							/* debug handle */
OSS_SEM_HANDLE  *G_mkLockSem; 					/* global MK sempahore */
OSS_SEM_HANDLE  *G_mkIoctlSem; 					/* MK ioctl sempahore */
OSS_DL_LIST		G_drvList;						/* list of reg. LL drivers */
OSS_DL_LIST		G_devList;						/* list of devices */

OSS_DL_LIST		G_freeUsrBufList;				/* list of free user buffers */

#ifdef CONFIG_DEVFS_FS
# if LINUX_VERSION_CODE < VERSION_CODE(2,6,0)
static devfs_handle_t devfs_handle = NULL;
# endif
#endif

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

static int MDIS_OpenDevice( int ioctlCode,
							unsigned long usrMop, MK_PATH **mkPathP );
static int MDIS_GetStat( MK_PATH *mkPath, unsigned long arg );
static int MDIS_SetStat( MK_PATH *mkPath, unsigned long arg );

static int MDIS_Read( MK_PATH *mkPath, unsigned long arg );
static int MDIS_Write( MK_PATH *mkPath, unsigned long arg );
static void *MDIS_GetUsrBuf( u_int32 size, void **bufIdP );
static void MDIS_RelUsrBuf( void *data, void *bufId );

#if defined(PPC)
static int CompressErrno(int mdisErr);
#endif

/*--- symbols exported by MDIS kernel module ---*/
EXPORT_SYMBOL(mdis_register_ll_driver);
EXPORT_SYMBOL(mdis_unregister_ll_driver);
EXPORT_SYMBOL(mdis_find_ll_handle);

/*--- Non-MDIS driver interface ---*/
EXPORT_SYMBOL(mdis_open_external_dev);
EXPORT_SYMBOL(mdis_close_external_dev);
EXPORT_SYMBOL(mdis_install_external_irq);
EXPORT_SYMBOL(mdis_remove_external_irq);

/****************************** mk_open ***************************************
 *
 *  Description:  Open entry point of MDIS kernel
 *				  Note: the real "open" work is done by ioctl
 *---------------------------------------------------------------------------
 *  Input......:  inode			inode structure
 *				  filp			file structure
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int mk_open (struct inode *inode, struct file *filp)
{
	DBGWRT_1((DBH,"mk_open file=%p\n", filp));

	filp->private_data = NULL;

    MOD_INC_USE_COUNT;
    return 0;          /* success */
}


/****************************** mk_release ************************************
 *
 *  Description:  Release entry point of MDIS kernel (close)
 *---------------------------------------------------------------------------
 *  Input......:  inode			inode structure
 *				  filp			file structure
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int mk_release (struct inode *inode, struct file *filp)
{
	MK_PATH *mkPath = filp->private_data;
	MK_DEV *dev;
	int ret = 0;
	int32 err;

	DBGWRT_1((DBH,"mk_release file=%p\n", filp));

	if( mkPath != NULL ){
		dev = mkPath->dev;		/* point to MDIS device */

		MK_LOCK( err );

		DBGWRT_2((DBH," %s useCount was %d\n", dev->devName, dev->useCount ));
		if( (--dev->useCount == 0) && (dev->persist == FALSE) ){
			/* do the final close */
			if( (err = MDIS_FinalClose(dev)))
				ret = -err;
		}
		MK_UNLOCK;

		kfree( mkPath );		/* free path structure */
	}
    MOD_DEC_USE_COUNT;
    return ret;
}

/****************************** mk_ioctl ************************************
 *
 *  Description:  Ioctl entry point of MDIS kernel
 *---------------------------------------------------------------------------
 *  Input......:  inode			inode structure
 *				  filp			file structure
 *				  cmd			ioctl number
 *				  arg			argument to ioctl
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int mk_ioctl (
	struct file *filp,
	unsigned int cmd,
	unsigned long arg)
{
    int err = 0, size = _IOC_SIZE(cmd); /* the size bitfield in cmd */
    int ret = 0;
	MK_PATH *mkPath = (MK_PATH *)filp->private_data;

    DBGWRT_1((DBH,"mk_ioctl: cmd=0x%x arg=0x%lx\n", cmd, arg ));

    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (_IOC_TYPE(cmd) != MDIS_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > MDIS_IOC_MAXNR) return -ENOTTY;

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, size);
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, (void *)arg, size);
	
	if (err)	
		return -EFAULT;	

    switch(cmd) {

	case MDIS_GETSTAT:
		ret = MDIS_GetStat( mkPath, arg );
		break;

	case MDIS_SETSTAT:
		ret = MDIS_SetStat( mkPath, arg );
		break;

	case MDIS_READ:
		ret = MDIS_Read( mkPath, arg );
		break;

	case MDIS_WRITE:
		ret = MDIS_Write( mkPath, arg );
		break;

	case MDIS_OPEN_DEVICE:
	case MDIS_CREATE_DEVICE:
		/*
		 * MDIS_OPEN_DEVICE is called twice in each M_open:
		 * The first time it is called, it just parses the device descriptor
		 * for the board device name, the second time the real work is done.
		 */
		ret = MDIS_OpenDevice( cmd, arg, &mkPath );
		if( ret == 0 ){
			/* save the MDIS path structure in the file structure */
			filp->private_data = (void *)mkPath;
		}
		break;

	case MDIS_OPEN_BOARD:
	    {
			MDIS_OPEN_DEVICE_DATA mop;
			BBIS_ENTRY bbEnt;	/* thrown away */
			BBIS_HANDLE *bh; 	/* thrown away */

			if( copy_from_user ((void *)&mop, (void *)arg, sizeof(mop)) ){
				ret = -EFAULT;
				break;
			}
			ret = bbis_open( mop.brdName, (DESC_SPEC)mop.brdData, 	
							 &bh, &bbEnt );
		}
		break;

	case MDIS_REMOVE_BOARD:
	    {
			MDIS_OPEN_DEVICE_DATA mop;

			if( copy_from_user ((void *)&mop, (void *)arg, sizeof(mop)) ){
				ret = -EFAULT;
				break;
			}
			ret = bbis_close( mop.brdName );
		}
		break;

	case MDIS_REMOVE_DEVICE:
	    {
			MK_DEV *dev;
			MDIS_OPEN_DEVICE_DATA mop;

			if( copy_from_user ((void *)&mop, (void *)arg, sizeof(mop)) ){
				ret = -EFAULT;
				break;
			}
			/* find device by its name and remove it */
			if( (dev = MDIS_FindDevByName( mop.devName )) != NULL ){
				if( dev->useCount == 0 ){
					if( (ret = MDIS_FinalClose(dev)))
						ret = -ret;
				}
				else {
					ret = -EBUSY;
				}
			}
			else
				ret = -ENOENT;
		}
		break;

	default:  /* redundant, as cmd was checked against MAXNR */
        ret = -ENOTTY;
		break;
    }
	ret = -COMPRESS_ERRNO(-ret);		/* make errno for PPC < 515  */
	DBGWRT_1((DBH,"mk_ioctl exit: cmd=0x%x ret=%d (-0x%x)\n", cmd, ret, -ret));
    return ret;

}

#if defined (HAVE_UNLOCKED_IOCTL)
long mk_unl_ioctl(
	struct file *filp,
	unsigned int cmd,
	unsigned long arg)
{
	int ret =0;
	ret = mk_ioctl( filp, cmd, arg );
	return ret;
}
#endif

/****************************** mk_read **************************************
 *
 *  Description: fs layer's read entry point. Used for MDIS M_getblock
 *---------------------------------------------------------------------------
 *  Input......: filp			file structure
 *				 buf			buffer where to place read data
 *				 count			max number of bytes to read
 *				 *pos			current position in file (not used)
 *  Output.....: returns:		>=0 number of bytes read
 *								<0 negative error number
 *  Globals....:  -
 ****************************************************************************/
static ssize_t mk_read (
	struct file *filp,
	char *buf,
	size_t count,
	loff_t *pos)
{
	void *data = NULL;
	void *bufId = NULL;
	int32 error;
	MK_DEV *dev;
	MK_PATH *mkPath;
	int32 readCount;

	/* be sure that device has been opened by M_open */
	if( (mkPath = (MK_PATH *)filp->private_data) == NULL )
		return -ENODEV;			

	dev = mkPath->dev;

	DBGWRT_1((DBH,"MDIS_ReadBlock %s max %d\n", dev->devName, count ));

	/*--- allocate buffer for LL driver ---*/
	if( (data = MDIS_GetUsrBuf( count, &bufId )) == NULL )
		return -ENOMEM;

	if( (error = MDIS_DevLock( mkPath, dev->semBlkRead )) == 0 ) {

		/*--- call LL driver's blockRead ---*/
		error = dev->llJumpTbl.blockRead( dev->ll, mkPath->chan, data,
						  count, &readCount);

		MDIS_DevUnLock( mkPath, dev->semBlkRead );
	}

	if( error == 0 ){
		if( __copy_to_user( buf, data, readCount ))
			error = EFAULT;
	}
	MDIS_RelUsrBuf( data, bufId );

	error = COMPRESS_ERRNO(error);		/* make errno for PPC < 515  */
	DBGWRT_1((DBH,"MDIS_ReadBlock ex %s error 0x%x %d bytes read\n",
			  dev->devName, error, readCount ));

	return error ? -error : readCount;
}

/****************************** mk_write *************************************
 *
 *  Description: fs layer's write entry point. Used for MDIS M_setblock
 *---------------------------------------------------------------------------
 *  Input......: filp			file structure
 *				 buf			buffer that contains data to write
 *				 count			max number of bytes to write
 *				 *pos			current position in file (not used)
 *  Output.....: returns:		>=0 number of bytes written
 *								<0 negative error number
 *  Globals....:  -
 ****************************************************************************/
static ssize_t mk_write (
	struct file *filp,
	const char *buf,
	size_t count,
	loff_t *pos)
{
	void *data = NULL;
	void *bufId = NULL;
	int32 error;
	MK_DEV *dev;
	MK_PATH *mkPath;
	int32 writeCount;

	/* be sure that device has been opened by M_open */
	if( (mkPath = (MK_PATH *)filp->private_data) == NULL )
		return -ENODEV;			

	dev = mkPath->dev;

	DBGWRT_1((DBH,"MDIS_WriteBlock %s max %d\n", dev->devName, count ));

	/*--- allocate buffer for LL driver ---*/
	if( (data = MDIS_GetUsrBuf( count, &bufId )) == NULL )
		return -ENOMEM;

	if( copy_from_user( data, buf, count ) == 0 ){

		if( (error = MDIS_DevLock( mkPath, dev->semBlkWrite )) == 0 ) {

			/*--- call LL driver's blockWrite ---*/
			error = dev->llJumpTbl.blockWrite( dev->ll, mkPath->chan, data,
											   count, &writeCount);

			MDIS_DevUnLock( mkPath, dev->semBlkWrite );
		}
	}
	else
		error = EFAULT;

	MDIS_RelUsrBuf( data, bufId );

	error = COMPRESS_ERRNO(error);		/* make errno for PPC < 515  */
	DBGWRT_1((DBH,"MDIS_WriteBlock ex %s error 0x%x %d bytes written\n",
			  dev->devName, error, writeCount ));

	return error ? -error : writeCount;
}


/****************************** MDIS_GetUsrBuf *******************************
 *
 *  Description:  Allocate a buffer for copying user space to kernel space
 *
 *  If the requested size is <= (PAGE_SIZE-4) a statically allocated buffer
 *  from the MK buffer pool will be used.
 *  Otherwise is the buffer is smaller 130000 bytes, the buffer is kmalloc'ed.
 *  Bigger buffers are vmalloc'ed (slower, but unlimited in size)
 *---------------------------------------------------------------------------
 *  Input......:  size			size of requested buffer in bytes
 *  Output.....:  returns		ptr to buffer or NULL if no memory
 *				  *bufIdP		a buffer ID that has to be passed to
 *								MDIS_RelUsrBuf
 *  Globals....:  -
 ****************************************************************************/
static void *MDIS_GetUsrBuf( u_int32 size, void **bufIdP )
{
	void *buf = NULL;
	OSS_DL_NODE *node;

	*bufIdP = NULL;

	if( size <= MK_USRBUF_SIZE ){
		if( (node = OSS_DL_RemHead( &G_freeUsrBufList )) != NULL ){
			buf = (void *)(node+1);
			*bufIdP = (void *)node;
		}
		if( buf == NULL )
			buf = kmalloc( size, GFP_KERNEL );
	}
	else if( size <= 130000 ){
		buf = kmalloc( size, GFP_KERNEL );
	}
	else {
		buf = vmalloc( size );
		*bufIdP = (void *)-1;
	}
	return buf;
}

/****************************** MDIS_RelUsrBuf *******************************
 *
 *  Description:  Return buffer allocated by MDIS_GetUsrBuf
 *---------------------------------------------------------------------------
 *  Input......:  data			ptr to buffer to be returned
 *				  bufId			Id that was returned by MDIS_GetUsrBuf
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
static void MDIS_RelUsrBuf( void *data, void *bufId )
{
	if( bufId == (void *)-1)
		vfree( data );
	else if (bufId)		
		OSS_DL_AddTail( &G_freeUsrBufList, (OSS_DL_NODE *)bufId );
	else
		kfree( data );
}


/****************************** MDIS_GetStat *********************************
 *
 *  Description:  Perform MDIS getstat call (API call M_getstat)
 *---------------------------------------------------------------------------
 *  Input......:  mkPath		MDIS kernel path structure
 *				  arg			user space ptr to MDIS_LINUX_SGSTAT struct
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
static int MDIS_GetStat( MK_PATH *mkPath, unsigned long arg )
{
#ifdef DBG
	MK_DEV *dev = mkPath->dev;
#endif
	MDIS_LINUX_SGSTAT stat;
	int32 error;

	/* copy struct to kernel space */
	if( copy_from_user ((void *)&stat, (void *)arg, sizeof(stat)) )
		return -EFAULT;

	DBGWRT_1((DBH,"MDIS_GetStat %s chan %d code=0x%lx\n",
			  dev->devName, mkPath->chan, stat.code ));

	/*
	 * Now we have to handle BLK and STD codes differently.
	 * For block codes, stat.p.data points to a structure of type
	 * M_SG_BLOCK; for standard codes, stat.p.value points to a 32 bit value
	 * in user space
	 */
	if( stat.code & M_OFFS_BLK ){
		void *data = NULL;
		void *bufId = NULL;
		M_SG_BLOCK sgblk, llSgBlk;

		/* get the M_SG_BLOCK structure */
		if( copy_from_user ((void *)&sgblk, stat.p.data, sizeof(sgblk)) )
			return -EFAULT;

		/*
		 * Block getstats allow to exchange data to and from the kernel,
		 * so we have to copy from user space first
		 */
		if( (data = MDIS_GetUsrBuf( sgblk.size, &bufId )) == NULL )
			return -ENOMEM;

		if( copy_from_user (data, sgblk.data, sgblk.size) ){
			error = EFAULT;
			goto errexit;
		}

		llSgBlk.size = sgblk.size;
		llSgBlk.data = data;

		switch( stat.code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = MDIS_LlGetStat( mkPath, stat.code, (INT32_OR_64 *)&llSgBlk );
			break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = MDIS_BbGetStat( mkPath, stat.code, (INT32_OR_64 *)&llSgBlk );
			break;
		case M_OFFS_MK:
			error = MDIS_MkGetStat( mkPath, stat.code, (INT32_OR_64 *)&llSgBlk );
			break;
		default:
			error = ERR_MK_UNK_CODE;
		}
		
		/*
		 * copy back data to user
		 */
		if( copy_to_user( sgblk.data, data, llSgBlk.size )){
			error = EFAULT;
			goto errexit;
		}

		/* maybe M_SG_BLOCK.size was updated */
		if( sgblk.size != llSgBlk.size ){
			sgblk.size = llSgBlk.size;
			if( copy_to_user ( stat.p.data, (void *)&sgblk, sizeof(sgblk)) ){
				error = EFAULT;
				goto errexit;
			}
		}
	  errexit:
		if( data ) MDIS_RelUsrBuf( data, bufId );
		
	}
	else if( stat.code & M_OFFS_STD ){
		INT32_OR_64 value;

		if( get_user( value, (int32 *)stat.p.data )){
			error = EFAULT;
			goto end;
		}
		/*
		 * Standard getstat
		 */
		switch( stat.code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = MDIS_LlGetStat( mkPath, stat.code, &value ); break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = MDIS_BbGetStat( mkPath, stat.code, &value ); break;
		case M_OFFS_MK:
			error = MDIS_MkGetStat( mkPath, stat.code, &value ); break;
		default:
			error = ERR_MK_UNK_CODE;
		}
		if( put_user( value, (int32 *)stat.p.data ))
			error = EFAULT;
	} else
		error = ERR_MK_UNK_CODE;
end:
	DBGWRT_1((DBH, "MDIS_GetStat: %s exit error=0x%lx\n", dev->devName,
			  error ));
	return -error;
}

/****************************** MDIS_SetStat *********************************
 *
 *  Description:  Perform MDIS setstat call (API call M_setstat)
 *---------------------------------------------------------------------------
 *  Input......:  mkPath		MDIS kernel path structure
 *				  arg			user space ptr to MDIS_LINUX_SGSTAT struct
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
static int MDIS_SetStat( MK_PATH *mkPath, unsigned long arg )
{
#ifdef DBG
	MK_DEV *dev = mkPath->dev;
#endif
	MDIS_LINUX_SGSTAT stat;
	int32 error = 0;
	void *data  = NULL;
	void *bufId = NULL;
	M_SG_BLOCK sgblk;
	M_SG_BLOCK llSgBlk;

	/* copy struct to kernel space */
	if( copy_from_user ((void *)&stat, (void *)arg, sizeof(stat)) ) {
		return -EFAULT;
	}

	DBGWRT_1((DBH,"MDIS_SetStat %s chan %d code=0x%lx\n",
			  dev->devName, mkPath->chan, stat.code ));

	/*
	 * Now we have to handle BLK and STD codes differently.
	 * For block codes, stat.p.data points to a structure of type
	 * M_SG_BLOCK; for standard codes, stat.p.value is the 32 bit value
	 */

	if( stat.code & M_OFFS_BLK ) {
		/* get the M_SG_BLOCK structure */
		if( copy_from_user ((void *)&sgblk, stat.p.data, sizeof(sgblk)))
			return -EFAULT;
		

		/*
		 * Copy the data from user space
		 */
		if( (data = MDIS_GetUsrBuf( sgblk.size, &bufId )) == NULL )
			return -ENOMEM;

		if( copy_from_user (data, sgblk.data, sgblk.size) ){
			error = EFAULT;
			goto errexit;
		}

		llSgBlk.size = sgblk.size;
		llSgBlk.data = data;

		switch( stat.code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = MDIS_LlSetStat( mkPath, stat.code, (void *)&llSgBlk );
			break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = MDIS_BbSetStat( mkPath, stat.code, (void *)&llSgBlk );
			break;
		case M_OFFS_MK:
			error = MDIS_MkSetStat( mkPath, stat.code, (void *)&llSgBlk );
			break;
		default:
			error = ERR_MK_UNK_CODE;
		}

		/* maybe M_SG_BLOCK.size was updated */
		if( sgblk.size != llSgBlk.size ){
			sgblk.size = llSgBlk.size;
			if( copy_to_user ( stat.p.data, (void *)&sgblk, sizeof(sgblk)) ){
				error = EFAULT;
				goto errexit;
			}
		}
	  errexit:
		if( data ) MDIS_RelUsrBuf( data, bufId );
		
	}
	else if( stat.code & M_OFFS_STD ){
		/*
		 * Standard setstat
		 */
		switch( stat.code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = MDIS_LlSetStat( mkPath,
									stat.code,
									(void*)(U_INT32_OR_64)stat.p.value );
			break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = MDIS_BbSetStat( mkPath,
									stat.code,
									(void *)(U_INT32_OR_64)stat.p.value );
			break;
		case M_OFFS_MK:
			error = MDIS_MkSetStat( mkPath,
									stat.code,
									(void *)(U_INT32_OR_64)stat.p.value );
			break;
		default:
			error = ERR_MK_UNK_CODE;
		}
	} else
		error = ERR_MK_UNK_CODE;

	DBGWRT_1((DBH, "MDIS_SetStat: %s exit error=0x%lx\n", dev->devName,
			  error ));
	return -error;
}

/******************************** MDIS_DevLock ********************************
 *
 *  Description: Lock the device
 *
 *
 *	If Lockmode is CHAN, locks the channel's sempahore
 *	If Lockmode is CALL, locks the semaphore pointed to by callSem
 *	Finally, locks the device semaphore			
 *---------------------------------------------------------------------------
 *  Input......: mkPath	  MDIS kernel path structure
 *				 callSem  semaphore to lock when lock mode is CALL
 *  Output.....: return   success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_DevLock( MK_PATH *mkPath, OSS_SEM_HANDLE *callSem )
{
	MK_DEV *dev = mkPath->dev;
	int32 error;

	switch( dev->lockMode ){
	case LL_LOCK_CHAN:
		if( (error = OSS_SemWait( dev->osh, dev->semChanP[mkPath->chan],
								  OSS_SEM_WAITFOREVER ))){
			DBGWRT_ERR((DBH,"*** MK:DevLock Error 0x%04x locking chansem\n",
						error));
			return error;
		}
		break;
	case LL_LOCK_CALL:
		if( (error = OSS_SemWait( dev->osh, callSem, OSS_SEM_WAITFOREVER ))){
			DBGWRT_ERR((DBH,"*** MK:DevLock Error 0x%04x locking callsem\n",
						error));
			return error;
		}
		break;
	}
	if( (error = OSS_SemWait( dev->osh, dev->semDev, OSS_SEM_WAITFOREVER ))){
		DBGWRT_ERR((DBH,"*** MK:DevLock Error 0x%04x locking devsem\n",
					error));

		switch( dev->lockMode ){
		case LL_LOCK_CHAN:
			OSS_SemSignal( dev->osh, dev->semChanP[mkPath->chan] );
			break;
		case LL_LOCK_CALL:
			OSS_SemSignal( dev->osh, callSem );
			break;
		}
	}
	return error;
}

/******************************** MDIS_DevUnLock ****************************
 *
 *  Description: UnLock the device
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: mkPath	  MDIS kernel path structure
 *				 callSem  semaphore to unlock when lock mode is CALL
 *  Output.....: -
 *  Globals....:
 ****************************************************************************/
void MDIS_DevUnLock( MK_PATH *mkPath, OSS_SEM_HANDLE *callSem )
{
	MK_DEV *dev = mkPath->dev;

	OSS_SemSignal( dev->osh, dev->semDev );

	switch( dev->lockMode ){
	case LL_LOCK_CHAN:
		OSS_SemSignal( dev->osh, dev->semChanP[mkPath->chan] );
		break;
	case LL_LOCK_CALL:
		OSS_SemSignal( dev->osh, callSem );
		break;
	}
}

/******************************** MDIS_LlGetStat ****************************
 *
 *  Description: Call the device drivers GetStat() routine, lock processes
 *
 *
 *			
 *---------------------------------------------------------------------------
 *  Input......: mkPath	  MDIS kernel path structure
 *				 code     status code
 *               value	  either ptr to value or M_SG_BLOCK struct
 *  Output.....: return   success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_LlGetStat(MK_PATH *mkPath, int32 code, INT32_OR_64 *valueP)
{
	MK_DEV *dev = mkPath->dev;
	int32 error;

	if( (error = MDIS_DevLock( mkPath, dev->semGetStat ))) return error;

	error = dev->llJumpTbl.getStat(dev->ll, code, mkPath->chan, valueP);

	MDIS_DevUnLock( mkPath, dev->semGetStat );
	return error;
}

/******************************** MDIS_BbGetStat ****************************
 *
 *  Description: Call the board drivers GetStat() routine
 *			
 *
 *			
 *---------------------------------------------------------------------------
 *  Input......: mkPath	  MDIS kernel path structure
 *				 code     status code
 *               value	  either ptr to value or M_SG_BLOCK struct
 *  Output.....: return   success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_BbGetStat(MK_PATH *mkPath, int32 code, INT32_OR_64 *valueP)
{
	MK_DEV *dev = mkPath->dev;
	return dev->brdJumpTbl.getStat(dev->brd, dev->devSlot, code, valueP);
}

/******************************** MDIS_LlSetStat ****************************
 *
 *  Description: Call the device drivers SetStat() routine, lock processes
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: mkPath	  MDIS kernel path structure
 *				 code     status code
 *               value	  either the value or M_SG_BLOCK struct
 *  Output.....: return   success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_LlSetStat(MK_PATH *mkPath, int32 code, void *value)
{
	MK_DEV *dev = mkPath->dev;
	int32 error;

	if( (error = MDIS_DevLock( mkPath, dev->semSetStat ))) return error;

	error = dev->llJumpTbl.setStat(dev->ll, code, mkPath->chan, (U_INT32_OR_64)value);

	MDIS_DevUnLock( mkPath, dev->semSetStat );
	return error;
}

/******************************** MDIS_BbSetStat ****************************
 *
 *  Description: Call the board drivers SetStat() routine
 *			
 *
 *			
 *---------------------------------------------------------------------------
 *  Input......: mkPath	  MDIS kernel path structure
 *				 code     status code
 *               value	  either the value or M_SG_BLOCK struct
 *  Output.....: return   success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_BbSetStat(MK_PATH *mkPath, int32 code, void *value)
{
	MK_DEV *dev = mkPath->dev;
	return dev->brdJumpTbl.setStat(dev->brd, dev->devSlot, code, (INT32_OR_64)value);
}

/****************************** MDIS_Read ***********************************
 *
 *  Description:  Perform Low Level driver read (API call M_read)
 *
 *  current channel number is incremented according to ioMode
 *---------------------------------------------------------------------------
 *  Input......:  mkPath		MDIS kernel path structure
 *				  arg		    ptr into user space int32 variable
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
static int MDIS_Read( MK_PATH *mkPath, unsigned long arg )
{
	int32 error, value;
	MK_DEV *dev = mkPath->dev;

	if( (error = MDIS_DevLock( mkPath, dev->semRead ))) return -error;

	error = dev->llJumpTbl.read(dev->ll, mkPath->chan, &value);

	MDIS_DevUnLock( mkPath, dev->semRead );

	/* increment channel */
	if (mkPath->ioMode==M_IO_EXEC_INC && error==0)
		if (++(mkPath->chan) == dev->devNrChan)
			mkPath->chan = 0;
	
	if( put_user( value, (int32 *)arg ))
		error = EFAULT;
	return -error;
}

/****************************** MDIS_Write ***********************************
 *
 *  Description:  Perform Low Level driver write (API call M_write)
 *
 *  current channel number is incremented according to ioMode
 *---------------------------------------------------------------------------
 *  Input......:  mkPath		MDIS kernel path structure
 *				  arg			value to write
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
static int MDIS_Write( MK_PATH *mkPath, unsigned long arg )
{
	int32 error;
	MK_DEV *dev = mkPath->dev;

	if( (error = MDIS_DevLock( mkPath, dev->semWrite ))) return -error;

	error = dev->llJumpTbl.write(dev->ll, mkPath->chan, arg);

	MDIS_DevUnLock( mkPath, dev->semWrite );

	/* increment channel */
	if (mkPath->ioMode==M_IO_EXEC_INC && error==0)
		if (++(mkPath->chan) == dev->devNrChan)
			mkPath->chan = 0;
	
	return -error;
}

/****************************** MDIS_IrqHandler ******************************
 *
 *  Description:  Global MDIS Interrupt handler
 *
 *               - calls board handler service init (irqSrvcInit)
 *               - dispatch to low-level-handler      (MXX_Irq)
 *               - calls board handler service exit (irqSrvcExit)
 *
 *---------------------------------------------------------------------------
 *  Input......:  irq			interrupt number
 *				  dev_id		for MDIS, ptr to MK_DEV struct
 *				  regs			register snapshot (not used)
 *  Output.....:  Linux_2.6 only: IRQ_HANDLED / IRQ_NONE
 *  Globals....:  -
 ****************************************************************************/
FUNCTYPE MDIS_IRQHANDLER(int irq, void *dev_id, struct pt_regs *regs)
{
	MK_DEV *dev = (MK_DEV *)dev_id;
    int32            	irqFromBB;
	int handled = 0;


    IDBGWRT_1((DBH,">>> MDIS_IrqHandler %s vector %d \n", dev->devName, irq ));

    /*-----------------------------+
    |  board handler service init  |
    +-----------------------------*/
	if( dev->irqSrvInitFunc )
		irqFromBB = dev->brdJumpTbl.irqSrvInit( dev->brd, dev->devSlot );
	else
		irqFromBB = BBIS_IRQ_UNK;

	if( irqFromBB & BBIS_IRQ_EXP ){
		if( dev->initialized )
			printk( KERN_WARNING "*** MDIS: BBIS exception on %s / %s\n",
					dev->brdName, dev->devName );
		dev->exceptionOccurred++;
		handled++;
	}

    /*-----------------------------+
    |  ll irq call                 |
    +-----------------------------*/
    if( irqFromBB & (BBIS_IRQ_UNK | BBIS_IRQ_YES)){

        IDBGWRT_2((DBH," call LL driver irq\n" ));

        /* board detected irq on device (or board doesn't know it) */
		if(dev->initialized )		/* device initialisation finished? */
			/* call low level driver interrupt handler */
			if( dev->llJumpTbl.irq( dev->ll ) != LL_IRQ_DEV_NOT ){
				handled++;
				dev->irqCnt++;
			}
    }

    /*-----------------------------+
    |  board handler service exit  |
    +-----------------------------*/
	if( dev->irqSrvExitFunc )
		dev->brdJumpTbl.irqSrvExit( dev->brd, dev->devSlot );	


#ifdef LINUX_26
	return handled ? IRQ_HANDLED : IRQ_NONE;
#endif
}

/****************************** MDIS_FindDevByName ***************************
 *
 *  Description:  Search for a device in MDIS device list
 *---------------------------------------------------------------------------
 *  Input......:  name			name to look for
 *  Output.....:  returns		ptr to device struct or NULL if not found
 *  Globals....:  -
 ****************************************************************************/
MK_DEV *MDIS_FindDevByName( const char *name )
{
	MK_DEV *node;

	for( node=(MK_DEV *)G_devList.head;
		 node->node.next;
		 node = (MK_DEV *)node->node.next ){
		
		if( strcmp(node->devName, name ) == 0 )
			break;
	}

	if( node->node.next == NULL )
		node=NULL;

	return node;
}

/****************************** MDIS_FindDrvByName ***************************
 *
 *  Description:  Search for a driver in MDIS driver list
 *---------------------------------------------------------------------------
 *  Input......:  name			name to look for
 *  Output.....:  returns		ptr to driver struct or NULL if not found
 *  Globals....:  -
 ****************************************************************************/
MK_DRV *MDIS_FindDrvByName( const char *name )
{
	MK_DRV *node;

	for( node=(MK_DRV *)G_drvList.head;
		 node->node.next;
		 node = (MK_DRV *)node->node.next ){
		
		if( strcmp(node->drvName, name ) == 0 )
			break;
	}

	if( node->node.next == NULL )
		node=NULL;

	return node;
}

/****************************** OpenDevice **********************************
 *
 *  Description:  Ioctl handler for MDIS_OPEN_DEVICE/MDIS_CREATE_DEVICE
 *---------------------------------------------------------------------------
 *  Input......:  ioctlCode	MDIS_OPEN_DEVICE or MDIS_CREATE_DEVICE
 *		  usrMop	user space address of MDIS_OPEN_DEVICE_DATA
 *				or MDIS_CREATE_DEVICE_DATA struct
 *  Output.....:  returns	0=ok, or negative error number
 *				  *mkPathP		if successfull, contains the MK path struct
 *  Globals....:  -
 ****************************************************************************/
static int MDIS_OpenDevice( int ioctlCode,
							unsigned long usrMop, MK_PATH **mkPathP )
{
	MDIS_CREATE_DEVICE_DATA mcdd;
	char *devDesc = NULL;
	char *brdDesc = NULL;
	DESC_HANDLE *devDescHdl = NULL;
	u_int32 value;
	int err, ret=0;
	MK_DEV *dev;
	MK_PATH *mkPath = NULL;

	*mkPathP = NULL;

	/* copy struct to kernel space */
	if( ioctlCode == MDIS_OPEN_DEVICE ){
		if( copy_from_user ((void *)&mcdd.d, (void *)usrMop, sizeof(mcdd.d)) )
			return -EFAULT;
	}
	else {
		if( copy_from_user ((void *)&mcdd, (void *)usrMop, sizeof(mcdd)) )
			return -EFAULT;
	}		

	DBGWRT_2((DBH, " MDIS_OPEN_DEVICE dev=%s"
			  " devdata=%p devlen=%ld"
			  " brddata=%p brdlen=%ld\n", mcdd.d.devName,
			  mcdd.d.devData, mcdd.d.devDescLen, mcdd.d.brdData,
			  mcdd.d.brdDescLen  ));
	/*
	 * MDIS_OPEN_DEVICE is called twice in each M_open:
	 * The first time it is called, it just parses the device descriptor
	 * for the board device name, the second time the real work is done.
	 */


	/* copy device descriptor into kernel space */
	devDesc = kmalloc( mcdd.d.devDescLen, GFP_KERNEL );
	if( devDesc == NULL ) return -ENOMEM;

	if( copy_from_user ((void *)devDesc, mcdd.d.devData, mcdd.d.devDescLen)){
		DBGWRT_ERR((DBH,"*** copy_from_user(1)\n"));
		ret = -EFAULT;
		goto errexit;
	}

	if( (err = DESC_Init( (DESC_SPEC)devDesc, OSH, &devDescHdl ))){
		DBGWRT_ERR((DBH,"*** MDIS_OPEN_DEVICE: %s can't init dev desc "
					"err=0x%x\n", mcdd.d.devName, err ));
		ret = -err;
		goto errexit;
	}
	
	/* check if descriptor type is of type "device" */
	if( (err = DESC_GetUInt32( devDescHdl, 0, &value, "DESC_TYPE" )) ||
		(value != DESC_TYPE_DEVICE)){
		DBGWRT_ERR((DBH,"*** MDIS_OPEN_DEVICE: %s: DESC_TYPE in dev desc "
					"not found or bad\n", mcdd.d.devName));		
		ret = -err;
		goto errexit;
	}

	if( mcdd.d.brdData == NULL ){

		/*--- first call to MDIS_OPEN_DEVICE ---*/
		
		/* get board name */
		value = sizeof(mcdd.d.brdName);
		if( (err = DESC_GetString( devDescHdl, "", mcdd.d.brdName, &value,
								   "BOARD_NAME" ))){
			DBGWRT_ERR((DBH,"*** MDIS_OPEN_DEVICE: BOARD_NAME "
						"not found\n"));
			ret = -err;
			goto errexit;
		}

		DBGWRT_2((DBH," board name = %s\n", mcdd.d.brdName ));
		/* copy back the board name to user space */
		if(copy_to_user( ((MDIS_OPEN_DEVICE_DATA *)usrMop)->brdName,
						 mcdd.d.brdName,
						 sizeof( mcdd.d.brdName )))
			DBGWRT_ERR((DBH,"*** copy_to_user BOARD_NAME failed!\n"));

	}
	else {

		/*--- second call to MDIS_OPEN_DEVICE ---*/

		/* copy board descriptor into kernel space */
		brdDesc = kmalloc( mcdd.d.brdDescLen, GFP_KERNEL );
		if( brdDesc == NULL ) return -ENOMEM;

		if( copy_from_user ((void *)brdDesc, mcdd.d.brdData,
							mcdd.d.brdDescLen)){
			DBGWRT_ERR((DBH,"*** copy_from_user(2)\n"));
			ret = -EFAULT;
			goto errexit;
		}

		MK_LOCK(err);
		if( err ){
			ret = -EINTR;
			goto errexit;
		}

		/*--- check if device already known ---*/
		if( (dev = MDIS_FindDevByName( mcdd.d.devName )) == NULL ){

			/*--- not known, do initial open ---*/
			if( (err = MDIS_InitialOpen( mcdd.d.devName,
						     devDescHdl,
						     (DESC_SPEC)devDesc,	
						     mcdd.d.brdName,
						     (DESC_SPEC)brdDesc,
						     0,
						     &dev))){
				DBGWRT_ERR((DBH,"*** MDIS_OpenDevice: %s: initial open failed"
							"err=0x%x\n", mcdd.d.devName, err ));
				ret = -err;
				MK_UNLOCK;
				goto errexit;
			}
		}

		dev->useCount++;

		MK_UNLOCK;

		/*--- create path structure ---*/
		if( (mkPath = kmalloc( sizeof(*mkPath), GFP_KERNEL )) == NULL ){
			ret = -ENOMEM;
			goto errexit;
		}
		mkPath->chan 	= 0;
		mkPath->ioMode 	= M_IO_EXEC;
		mkPath->dev		= dev;

		*mkPathP = mkPath;
	}

 errexit:
	if( devDescHdl ) DESC_Exit( &devDescHdl );
	if( devDesc ) kfree(devDesc);
	if( brdDesc ) kfree(brdDesc);

	return ret;
}

#if defined(PPC)

/***************************** CompressErrno **********************************
 *
 *  Description:  PowerPC special routine to compress MDIS error numbers
 *
 * On this machine, error numbers returned by system calls
 * cannot be higher than 515. So try to compress MDIS error numbers so that
 * they fit into this range.
 * On the user level, this value is expanded to its original value in mdis_api
 *
 * The following table shows the mapping:
 *
 * MDIS class   org. range		compressed error code
 * ------------------------------------------------
 * ERR_MK    	(0x400..0x4ff)	0xa0..0xbf
 * ERR_MBUF  	(0x600..0x6ff) 	0xc0..0xcf
 * ERR_BBIS  	(0x700..0x7ff) 	0xd0..0xdf
 * ERR_OSS	 	(0x800..0x87f) 	0xe0..0xff
 * ERR_OSS_PCI	(0x880..0x89f)	0x100..0x10f
 * ERR_OSS_VME	(0x8a0..0x8bf)	0x110..0x117
 * ERR_OSS_ISA	(0x8c0..0x8ff)	0x118..0x11f
 * ERR_DESC		(0x900..0x97f)	0x120..0x12f
 * ERR_ID		(0x980..0x9ff)  0x130..0x137
 * ERR_PLD		(0xa00..0xa7f)	0x138..0x13f
 * ERR_BK		(0xb00..0xbff)	0x140..0x14f
 * ERR_LL		(0x500..0x5ff)	0x150..0x15f
 * ERR_DEV		(0xc00..0xcff)	0x160..0x1ff
 *---------------------------------------------------------------------------
 *  Input......:  mdisErr		original MDIS error code (positive)
 *  Output.....:  returns		compressed error code
 *  Globals....:  -
 ****************************************************************************/
int CompressErrno( int mdisErr )
{	
	MK_ERRNO_COMPRESSION_TABLE;
	const MDIS_ERRNO_COMPRESSION_TABLE *p = mdisErrnoCompressionTable;
	int rv;

	if( mdisErr < ERR_OS )
		return mdisErr;			/* non MDIS error or no error */

	for( ; p->orgStart>0; p++ ){
		if( mdisErr >= p->orgStart && mdisErr <= p->orgEnd ){
			rv = p->compStart + mdisErr - p->orgStart;

			if( rv > p->compEnd ){
				DBGWRT_ERR((DBH,"*** MK:CompressErrno: can't map 0x%x (1)\n",
							mdisErr ));
				rv = EINVAL;	/* return generic error */
			}
			DBGWRT_3((DBH,"CompressErrno: org=0x%x comp=0x%x\n", mdisErr, rv));
			return rv;
		}
	}
	DBGWRT_ERR((DBH,"*** MK:CompressErrno: can't map 0x%x (2)\n",
				mdisErr ));
	return EINVAL;	/* return generic error */
}
#endif /* PPC */

#define INC_LEN if (len+begin > off+count) goto done;\
                if (len+begin < off) {\
				   begin += len;\
				   len = 0;\
				}

/****************************** mk_read_procmem ****************************
 *
 *  Description:  Function to fill in data when /proc/mdis file is read
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
#if LINUX_VERSION_CODE < VERSION_CODE(3,10,0)
static int mk_read_procmem( char *page, char **start, off_t off, int count, int *eof, void *data)
{
  int i, error, len = 0, rv;
  off_t begin = 0;

  DBGWRT_3((DBH,"mk_read_procmem: count %d page=%p\n", count, page));
  MK_LOCK(error);

  if( i )
    return (-ERESTARTSYS);

  /* user buffers */
  {
    OSS_DL_NODE *node;

    for( i=0, node=G_freeUsrBufList.head; node->next;
	 node=node->next, i++ ){
    }
  }
  len += sprintf( page+len, "User API Buffers: total %d"
		  ", free %d, each %d bytes\n",
		  mk_nbufs, i, MK_USRBUF_SIZE);

  /* Drivers */
  len += sprintf( page+len, "\nDrivers:\n" );
  INC_LEN;

  {
    MK_DRV *node;
    for( node=(MK_DRV *)G_drvList.head;
	 node->node.next;
	 node = (MK_DRV *)node->node.next ){

      len += sprintf( page+len, "  %s\n", node->drvName );
      INC_LEN;
    }
  }
	
  /* Devices */
  len += sprintf( page+len, "Devices:\n" );
  INC_LEN;
  {
    MK_DEV *node;
    for( node=(MK_DEV *)G_devList.head;
	 node->node.next;
	 node = (MK_DEV *)node->node.next ){

      len += sprintf( page+len, "  %s brd=%s slot=%d drv=%s usecnt=%d \n",
		      node->devName, node->brdName, node->devSlot, node->drv ? node->drv->drvName : "?",
		      node->useCount);
      INC_LEN;
    }
  }

  *eof = 1;
 done:
  if (off >= len+begin){
    rv = 0;
    goto end;
  }
  *start = page + (off-begin);
  rv = ((count < begin+len-off) ? count : begin+len-off);

 end:
  DBGWRT_3((DBH,"mk_read_procmem: ex eof=%d rv=%d\n", *eof, rv));
  MK_UNLOCK;

  return rv;
}

#else /* newer kernel >= 3.10 */
static ssize_t mk_read_procmem( struct file *filp, char *buf, size_t count, loff_t *pos)
{

  int i, error, rv=0;
  char locbuf[PROC_BUF_LEN];
  char *tmp = locbuf;
  static int len=0;

  /* ts: page wise readers like cat read until no chars are returned, so keep len in a persistent static value
     and avoid reentering twice */
  if( len ) {
    len = 0;
    return 0;
  }

  DBGWRT_3((DBH,"mk_read_procmem: count %d\n", count));
  MK_LOCK(error);

  memset(locbuf, 0x00, sizeof(locbuf));

  /* user buffers */
  {
    OSS_DL_NODE *node;

    for( i=0, node=G_freeUsrBufList.head; node->next;
	 node=node->next, i++ ){
    }
  }
  len += sprintf( tmp+len, "User API Buffers: total %d, free %d, each %d bytes\n",
		  mk_nbufs, i, MK_USRBUF_SIZE);

  /* Drivers */
  len += sprintf( tmp+len, "\nDrivers:\n" );

  {
    MK_DRV *node;
    for( node=(MK_DRV *)G_drvList.head;
	 node->node.next;
	 node = (MK_DRV *)node->node.next ){

      len += sprintf( tmp+len, "  %s\n", node->drvName );
    }
  }
	
  /* Devices */
  len += sprintf( tmp+len, "Devices:\n" );
  {
    MK_DEV *node;
    for( node=(MK_DEV *)G_devList.head;
	 node->node.next;
	 node = (MK_DEV *)node->node.next ){

      len += sprintf( tmp+len, "  %s brd=%s slot=%d drv=%s usecnt=%d\n",
		      node->devName, node->brdName, node->devSlot, node->drv ? node->drv->drvName : "?",
		      node->useCount);
    }
  }

  len+=copy_to_user(buf, locbuf, len );

  DBGWRT_3((DBH,"mk_read_procmem: ex rv=%d\n", rv));
  MK_UNLOCK;
  return len;
}

#endif



/*
 * The different file operations
 */

static struct file_operations mk_fops = {
    read:       	mk_read,
    write:		mk_write,
	/* Do not lock when entering MDIS kernel
	   Locking is the responsability of driver developper */
#if defined(HAVE_UNLOCKED_IOCTL)
    unlocked_ioctl:	mk_unl_ioctl,
#else
    ioctl:      	mk_ioctl,
#endif
    open:       	mk_open,
    release:    	mk_release,
};


#if LINUX_VERSION_CODE >= VERSION_CODE(3,10,0)
static struct file_operations mk_proc_fops = {
     read:       	mk_read_procmem,
};
#endif


/*
 * Finally, the module stuff
 */
int init_module(void)
{
	int result, ret=0, n;
	
	DBGINIT((NULL,&DBH));

	printk( KERN_INFO "MEN MDIS Kernel init_module\n");

	if ( (result = register_chrdev (mk_major, "mdis_kernel", &mk_fops)) < 0) {
          printk (KERN_ERR "mk: unable to get major %d\n", mk_major);
          return -EIO;
     }

	if( mk_major == 0 )
		mk_major = result;
	
	DBGWRT_1((DBH,"mk: using major %d\n", mk_major));

#ifdef CONFIG_DEVFS_FS
# if LINUX_VERSION_CODE >= VERSION_CODE(2,6,0)
	 if (devfs_mk_cdev( MKDEV( mk_major, 0), S_IFCHR | S_IRUSR | S_IWUSR,
						"/mdis", 0))
		 printk	(KERN_ERR "devfs_mk_cdev: cant register 'mdis' major %d\n",
				 mk_major);
#  else
	if( (result = devfs_register_chrdev(mk_major, "mdisKernel",
										&mk_fops)) < 0 	) {
		printk("mk: unable to get major %d\n", mk_major);
		return -EIO;
	}
# endif
	if( mk_major == 0 )
		mk_major = result;

	DBGWRT_1((DBH,"mk: using major %d\n", mk_major));
	/*
	 * Create /dev/mdis
	 */

	devfs_handle = devfs_register(NULL, "mdis", DEVFS_FL_DEFAULT,
								  mk_major, 0,
								  S_IFCHR | S_IRUSR | S_IWUSR,
								  &mk_fops, NULL);
#endif /* version 2.6 */

	/* init OSS */
	if( OSS_Init( "MDIS_KERNEL", &OSH )){
		ret = -ENOMEM;
		goto clean1;
	}

	/* create global MDIS lock sem */
	if( OSS_SemCreate(OSH, OSS_SEM_BIN, 1, &G_mkLockSem )){
		ret = -ENOMEM;
		goto clean2;
	}

	/* init lists */
	OSS_DL_NewList( &G_drvList );
	OSS_DL_NewList( &G_devList );

	/* init user buffer pool */
	OSS_DL_NewList( &G_freeUsrBufList );

	/* allocate one page for each user buffer, put the node struct in front */
	for( n=0; n<mk_nbufs; n++ ){
		unsigned long pg;

		if( (pg = __get_free_page( GFP_KERNEL )) == 0 ){
			printk( KERN_INFO "Could not allocate user buffer %d\n", n );
			goto clean3;
		}

		OSS_DL_AddTail( &G_freeUsrBufList, (OSS_DL_NODE *)pg );
	}

#if LINUX_VERSION_CODE < VERSION_CODE(3,10,0)
	create_proc_read_entry ("mdis", 0, NULL, mk_read_procmem, NULL);
#else
	proc_create (           "mdis", 0, NULL, &mk_proc_fops);
#endif

	goto clean0;

 clean3:
	{
		OSS_DL_NODE *node, *nnode;
		
		for( node=G_freeUsrBufList.head; node->next; node = nnode ){
			nnode = node->next;
		    free_page( (unsigned long)node );
		}
	}
	OSS_SemRemove( OSH, &G_mkLockSem );

 clean2:
	OSS_Exit( &OSH );
 clean1:


	unregister_chrdev ( mk_major, "mdisKernel");

#if LINUX_VERSION_CODE >= VERSION_CODE(2,6,0)
# ifdef CONFIG_DEVFS_FS
	devfs_remove ("mdis", 0);
# endif
#else /* kernel 2.4.x */
# ifdef CONFIG_DEVFS_FS
	devfs_unregister(devfs_handle);
	devfs_unregister_chrdev(mk_major, "mdisKernel");
# endif
#endif /* version 2.6 */

 clean0:

	return ret;
}

	
void cleanup_module(void)
{

	OSS_SemRemove( OSH, &G_mkLockSem );


	/* free the user buffers */
	{
		OSS_DL_NODE *node, *nnode;
		int cnt=0;

		for( node=G_freeUsrBufList.head; node->next; node = nnode ){
			nnode = node->next;
			free_page( (unsigned long)node );
			cnt++;
		}
		if( cnt != mk_nbufs )
			printk( KERN_ERR "*** MEN MDIS Kernel cleanup_module: Only %d"
					" of %d user bufs free'd\n", cnt, mk_nbufs);
	}
	OSS_Exit(&OSH);


#if LINUX_VERSION_CODE >= VERSION_CODE(2,6,0)
	unregister_chrdev ( mk_major, "mdisKernel");
# ifdef CONFIG_DEVFS_FS
	devfs_remove ("mdis", 0);
# endif
#else
# ifdef CONFIG_DEVFS_FS
	devfs_unregister(devfs_handle);
	devfs_unregister_chrdev(mk_major, "mdisKernel");
# endif
#endif /* version 2.6 */

    remove_proc_entry ("mdis", 0);

	DBGEXIT((&DBH));
	printk( KERN_INFO "MEN MDIS Kernel cleanup_module\n");
}

/****************************** mdis_register_ll_driver **********************
 *
 *  Description:  Register an MDIS Low Level Driver in the MDIS kernel
 *---------------------------------------------------------------------------
 *  Input......:  llName		name of low level driver e.g. "men_ll_m55"
 *				  getEntry		ptr to GetEntry function of LL driver
 *				  module		calling module structure
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int mdis_register_ll_driver(
	char *llName,
	void (*getEntry)(LL_ENTRY *),
	struct module *module)
{
	MK_DRV *node;
	int32 error;

	DBGWRT_1((DBH,"mdis_register_ll_driver: name=%s getentry=%p\n",
			  llName, getEntry));

	if( strlen(llName) > MK_MAX_DRVNAME )
		return -E2BIG;			/* name too long */

	if( strncmp( MK_DRV_PREFIX, llName, strlen(MK_DRV_PREFIX )) != 0){
		DBGWRT_ERR((DBH,"*** mdis_register_ll_driver: %s doesn't start with "
					"%s\n", llName, MK_DRV_PREFIX ));
		return -EINVAL;
	}		

	MK_LOCK(error);
	if( error )
		return -EINTR;

	/* check if already installed */
	if( (node = MDIS_FindDrvByName( llName ))){
		DBGWRT_ERR((DBH,"*** mdis_register_ll_driver: %s already registered\n",
					llName ));
		MK_UNLOCK;
		return -EBUSY;
	}

	/* alloc memory for node */
	node = kmalloc( sizeof(*node), GFP_KERNEL );
	if( node == NULL ){
		MK_UNLOCK;
		return -ENOMEM;
	}

	strcpy( node->drvName, llName );
	node->getEntry = getEntry;
	node->module   = module;

	OSS_DL_AddTail( &G_drvList, &node->node );

	MOD_INC_USE_COUNT;
	MK_UNLOCK;
	return 0;
}

/****************************** mdis_unregister_ll_driver ********************
 *
 *  Description:  Un-registers an MDIS Low Level Driver in the MDIS kernel
 *---------------------------------------------------------------------------
 *  Input......:  llName		name of low level driver e.g. "men_ll_m55"
 *  Output.....:  returns		0=ok, or negative error number
 *  Globals....:  -
 ****************************************************************************/
int mdis_unregister_ll_driver( char *llName )
{
	MK_DRV *node;
	int32 error;

	DBGWRT_1((DBH,"mdis_unregister_ll_driver: name=%s\n", llName));

	MK_LOCK(error);
	if( error )
		return -EINTR;

	node = MDIS_FindDrvByName( llName );
	if( node != NULL ){
		OSS_DL_Remove( &node->node );
		kfree(node);
		MOD_DEC_USE_COUNT;
		MK_UNLOCK;
		return 0;
	}
	MK_UNLOCK;
	return -ENOENT;				/* drv not registered */
}

/********************************* mdis_find_ll_handle ************************
 *
 *  Description: Find low level handle by given device name
 *			
 *	This routine can be used to access MDIS drivers from other MDIS drivers
 *  (such as LM78/Z8536 on F2).
 *
 *  The device to be queried must be already opened before calling this
 *  routine.
 *
 *  It returns the LL handle and the LL entry points. Note that the MDIS
 *  locking mechanism will be bypassed in this case.	 	
 *---------------------------------------------------------------------------
 *  Input......: devName		device name to look for
 *  Output.....: returns:		0=ok or negative error number
 *				 *hP			low level handle for that device
 *				 *entry		low level driver jump table filled with entries
 *  Globals....: -
 ****************************************************************************/
int mdis_find_ll_handle( char *devName, LL_HANDLE **hP, LL_ENTRY *entry )
{
	MK_DEV *node;

	if( (node = MDIS_FindDevByName( devName )) == NULL )
		return -ENOENT;

	*hP = node->ll;
	*entry = node->llJumpTbl;
	return 0;
}

MODULE_DESCRIPTION("MEN MDIS kernel");
MODULE_AUTHOR("Klaus Popp <klaus.popp@men.de>");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

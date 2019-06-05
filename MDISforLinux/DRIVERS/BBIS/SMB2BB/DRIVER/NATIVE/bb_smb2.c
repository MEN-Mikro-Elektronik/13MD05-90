/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_smb2.c
 *      Project: Pseudo BBIS for SMB2
 *
 *       Author: ts
 *
 *  Description: Pseudo BBIS handler _for Linux_ to use SMB2 type LL Drivers
 *
 *
 *     Required: SMB_BUSNBR
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_, LINUX
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2019, MEN Mikro Elektronik GmbH
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

#define _NO_BBIS_HANDLE		/* bb_defs.h: don't define BBIS_HANDLE struct */

/* linux specifics */
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/version.h>	/* LINUX_VERSION_CODE & friends */
#include <linux/list.h>		/* linked list functions	*/
#include <linux/i2c.h>

#include <linux/interrupt.h>
#include <linux/workqueue.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
	#include <linux/slab.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
	#include <linux/semaphore.h>
#endif

/* same values in <linux/i2c.h> and <MEN/smb2.h>! */
#undef I2C_M_TEN
#undef I2C_M_RD

#include <MEN/mdis_com.h>
#include <MEN/men_typs.h>   /* system dependend definitions   */
#include <MEN/dbg.h>        /* debug functions                */
#include <MEN/oss.h>        /* oss functions                  */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/bb_defs.h>    /* bbis definitions				  */
#include <MEN/mdis_err.h>   /* MDIS error codes               */
#include <MEN/mdis_api.h>   /* MDIS global defs               */
#include <MEN/smb2.h>   	/* SMB_HANDLE				      */

/* Sanity checks: this BBIS driver is linux specific and kernel 2.6 only  */
#ifndef LINUX
# error *** ERROR: SMB2 pseudo BBIS driver is intended for use under Linux only
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
# error *** ERROR: SMB2 pseudo BBIS driver supports kernel 2.6 only!
#endif


/*-----------------------------------------+
  |  DEFINES                                 |
  +-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL				h->debugLevel
#define DBH             		h->debugHdl
#define BBNAME					"SMB2BB"
#define SMB2BB_BBIS_MAX_DEVS	4	/* max number of devices supported */
#define DEVNAME_SIZE			30
#define SMBACCESS_TIMEOUT		10000

#ifdef DBG
#define DBGBB(x...)				printk(x)
#else
#define DBGBB(x...)
#endif

/* Somewhere after kernel 2.6.13 I2C API was changed, workqueue API in 2.6.20 */
#define LX_VERSION_I2C_DIFFERS      13
#define WORKQUEUE_API_CHANGE		20

#define OSS_SMB_DRV_NAME		    "OSS_SMB2-I2C"
#define OSS_SMB_NAM_SIZE			13

#define I2C_DRIVERID_OSS_SMB2    	0xf0	/* was once "EXP0" test ID 	*/
#define ADAP_NAME_LEN				255		/* I2C Adapter name length 	*/
#define SMB2_I2C_END_ADDR     		0x77	/* LSB aligned! 			*/
#define SMB2_I2C_START_ADDR   		0x08

#define SMB2_I2C_ADDR_COUNT   		SMB2_I2C_END_ADDR-SMB2_I2C_START_ADDR


/*-----------------------------------------+
  |  TYPEDEFS                                |
  +-----------------------------------------*/
typedef struct {
    MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table			*/
    u_int32     ownMemSize;			/* own memory size				*/
    OSS_HANDLE* osHdl;				/* os specific handle			*/
    DESC_HANDLE *descHdl;			/* descriptor handle pointer	*/
    u_int32     debugLevel;			/* debug level for BBIS     	*/
    DBG_HANDLE  *debugHdl;			/* debug handle					*/

    /* name of devices */
    u_int32		smBusNbr;			/* SMBus number(=i2c Adapter #)	*/
    char		devName[DEVNAME_SIZE][SMB2BB_BBIS_MAX_DEVS];
    int32		devCount;			/* nr. of devs specified in desc */

} BBIS_HANDLE;

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>			/* bbis jumptable				*/

/* dynamic list of found clients per Adapter */
typedef struct i2ctest_data {
    struct list_head 	node;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
    struct i2c_client 	client;
#else
	struct i2c_client*  client;
#endif
	unsigned int		adapNr;		/**< Adapter# we are on (0,1,2...)	*/
    unsigned int		smbusNr;	/**< SMB_BUSNBR passed in dsc file	*/
    OSS_HANDLE			*osHdl;		/**< OSS handle   					*/
} SMB2_I2C_DATA;

/* helper struct to pass data for I2C access to workqueue */
typedef struct smb_access_struct {
	void 	*smbH;
	u_int32  flags;
	u_int32  ready;
	u_int16  addr;
	u_int8 	cmdAddr;
	u_int8	dat;
} SMB_ACC;


/*-----------------------------------------+
  |  PROTOTYPES                             |
  +-----------------------------------------*/

/*
 *  SMBus registering/access functions differ a bit throughout 2.4 to 2.6.x.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
 #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
static int oss_smb2_probe(struct i2c_adapter *adapter, int address, int kind);
 #else
static int oss_smb2_probe(struct i2c_client* client, const struct i2c_device_id *id);
 #endif
#else
static int oss_smb2_probe(struct i2c_adapter *adapter, int address,
						  unsigned short flags, int kind);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
static int oss_smb2_attach(struct i2c_adapter *adap);
static int oss_smb2_detach(struct i2c_client *client);
#else

static int oss_smb2_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int oss_smb2_remove(struct i2c_client *client);

 #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
static int oss_smb2_detect(struct i2c_client *new_client, struct i2c_board_info *info);
 #else
static int oss_smb2_detect(struct i2c_client *new_client, int kind, struct i2c_board_info *info);
 #endif

#endif /* #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31) */

/* init/exit */
static int32 SMB2BB_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 SMB2BB_BrdInit(BBIS_HANDLE*);
static int32 SMB2BB_BrdExit(BBIS_HANDLE*);
static int32 SMB2BB_Exit(BBIS_HANDLE**);
static int32 OSS_SmbExit( void **smbHdlP );
static char* SMB2BB_Ident(void);
/* info */
static int32 SMB2BB_BrdInfo(u_int32, ...);
static int32 SMB2BB_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */

/* get module address */
static int32 SMB2BB_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 SMB2BB_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 SMB2BB_GetMAddr(BBIS_HANDLE*, u_int32, u_int32,
							 u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 SMB2BB_SetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64);
static int32 SMB2BB_GetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64*);
/* unused */
static int32 SMB2BB_Unused(void);
static int32 SMB2BB_UnusedIrqHandle(BBIS_HANDLE * bbhP,OSS_IRQ_HANDLE * ossH);

/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *h, int32 retCode);

/* unused Funcs, just return ERR_BBIS_ILL_FUNC */
static int32 SMB2BB_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 SMB2BB_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  SMB2BB_IrqSrvExit(BBIS_HANDLE*, u_int32);
static int32 SMB2BB_ExpEnable( BBIS_HANDLE *h, u_int32 mSlot, u_int32 enable);
static int32 SMB2BB_ExpSrv( BBIS_HANDLE *h, u_int32 mSlot );


/*-------------------------------+
 |  GLOBALS                      |
 +-------------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
static unsigned short ignore[] = { I2C_CLIENT_END };
#endif

/* Linux specific extensions */
static struct list_head		G_smb2ListHead;
static spinlock_t 			G_smb2Lock;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
static unsigned int			G_globalAdapterNr;
#endif
static SMB_ACC 				G_smbAccess;

static struct workqueue_struct *smb2_wq;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, WORKQUEUE_API_CHANGE)
static void smb2_wqfunc(void* data);
DECLARE_WORK( work_obj, smb2_wqfunc, NULL );
# else
static void smb2_wqfunc(struct work_struct *workstruct);
DECLARE_WORK( work_obj, smb2_wqfunc );
#endif

/*
 * Major change in struct i2c_client_address_data: all xxx_range
 * members are now gone.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,LX_VERSION_I2C_DIFFERS)
static unsigned short normal_i2c_range[] = { SMB2_I2C_START_ADDR,
											 SMB2_I2C_END_ADDR,
											 I2C_CLIENT_END };
static struct i2c_client_address_data addr_data = {
    .normal_i2c			= ignore,
    .normal_i2c_range	= normal_i2c_range,
    .probe				= ignore,
    .probe_range		= ignore,
    .ignore				= ignore,
    .ignore_range		= ignore,
    .force				= ignore,

};
#else
static unsigned short normal_addr[SMB2_I2C_ADDR_COUNT];

/* this struct is only necessary up to kernel 2.6.32;
   in newer kernel versions, addresses are handed over directly*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
static struct i2c_client_address_data addr_data = {
    .normal_i2c 	= normal_addr,
    .probe 			= ignore,
    .ignore 		= ignore,
 #if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,14)
    .forces			= NULL,
 #else
    .force			= ignore,
 #endif
};
#endif /*#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)*/

#endif /*#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,LX_VERSION_I2C_DIFFERS)*/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)
struct i2c_device_id oss_smb2_idtable[] = {
	{ "smb2", 0 },
	{}
};

MODULE_DEVICE_TABLE(i2c, oss_smb2_idtable);
#endif

static struct i2c_driver smb2_driver = {
#ifdef I2C_DF_NOTIFY
	.flags      	= I2C_DF_NOTIFY,
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
	.name   		= OSS_SMB_DRV_NAME,
	.owner      	= THIS_MODULE,
#endif
	.driver = {
		.name	= OSS_SMB_DRV_NAME,
	},
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
	.id    	 		= I2C_DRIVERID_OSS_SMB2,
	.attach_adapter = oss_smb2_attach,
	.detach_client  = oss_smb2_detach,
#else
	.id_table		= oss_smb2_idtable,
	.probe 			= oss_smb2_probe,
    .remove 		= oss_smb2_remove,
    .detect			= oss_smb2_detect,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
    .address_data	= &addr_data,
#else
	.address_list = normal_addr,
#endif
	.class			= I2C_CLASS_HWMON,
#endif
};


/*-----------------------------------------+
 |  STATICS                                |
 +-----------------------------------------*/

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static DEFINE_SEMAPHORE(G_smb2Mutex);
#else
static DECLARE_MUTEX(G_smb2Mutex);
#endif

static struct i2c_client *getClientFromAddrAndBusnr(int addr, int busnr);
static int32 SMB2BB_QuickComm( void *smbHdl, u_int32 flags, u_int16 addr,
							   u_int8 read_write );
static int32 SMB2BB_WriteByte( void *smbHdl, u_int32 flags, u_int16 addr,
							   u_int8 data );
static int32 SMB2BB_ReadByte( void *smbHdl, u_int32 flags, u_int16 addr,
							  u_int8 *data );
static int32 SMB2BB_WriteByteData( void *smbHdl, u_int32 flags, u_int16 addr,
								   u_int8 cmdAddr, u_int8 data );
static int32 SMB2BB_ReadByteData( void *smbHdl, u_int32 flags, u_int16 addr,
								  u_int8 cmdAddr, u_int8 *data );
static int32 SMB2BB_WriteWordData( void *smbHdl, u_int32 flags, u_int16 addr,
								   u_int8 cmdAddr, u_int16 data );
static int32 SMB2BB_ReadWordData( void *smbHdl, u_int32 flags, u_int16 addr,
								  u_int8 cmdAddr, u_int16 *data );
static int32 SMB2BB_WriteBlockData ( void *smbHdl, u_int32 flags, u_int16 addr,
									 u_int8 cmdAddr,u_int8 length,
									 u_int8 *data );
static int32 SMB2BB_ReadBlockData( void *smbHdl, u_int32 flags, u_int16 addr,
								   u_int8 cmdAddr, u_int8 *length,
								   u_int8 *data);
static int32 SMB2BB_ProcessCall( void *smbHdl, u_int32 flags, u_int16 addr,
								 u_int8 cmdAddr, u_int16 *data );
static int32 SMB2BB_BlockProcessCall(void *smbHdl, u_int32 flags, u_int16 addr,
									 u_int8 cmdAddr, u_int8 writeDataLen,
									 u_int8 *writeData, u_int8 *readDataLen,
									 u_int8 *readData );
/* the only exported one */
int32 OSS_GetSmbHdl( OSS_HANDLE *oss, u_int32 busNbr, void **smbHdlP);

/*  We _have_ to put this EXPORT into this men_bb_smb2 module, because
 *  otherwise men_oss and men_bb_smb2 modules have circular dependencies
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
EXPORT_SYMBOL(OSS_GetSmbHdl);
#endif

/***************************** SMB2BB_GetEntry *******************************/
/** Initialize drivers jump table.
 *
 *  \param bbisP      \IN  pointer to the not initialized structure
 *  \param *bbisP     \OUT initialized structure
 *
 *  \return           \c -
 */
#ifdef _ONE_NAMESPACE_PER_DRIVER_
extern void BBIS_GetEntry( BBIS_ENTRY *bbisP )
#else
	extern void SMB2BB_GetEntry( BBIS_ENTRY *bbisP )
#endif
{
    /* init/exit */
    bbisP->init         =   SMB2BB_Init;
    bbisP->brdInit      =   SMB2BB_BrdInit;
    bbisP->brdExit      =   SMB2BB_BrdExit;
    bbisP->exit         =   SMB2BB_Exit;
    bbisP->fkt04        =   SMB2BB_Unused;
    /* info */
    bbisP->brdInfo      =   SMB2BB_BrdInfo;
    bbisP->cfgInfo      =   SMB2BB_CfgInfo;
    bbisP->fkt07        =   SMB2BB_Unused;
    bbisP->fkt08        =   SMB2BB_Unused;
    bbisP->fkt09        =   SMB2BB_Unused;

    /* interrupt handling, these return just ILL_FUNC  */
    bbisP->irqEnable    = 	SMB2BB_IrqEnable;
    bbisP->irqSrvInit   = 	SMB2BB_IrqSrvInit;
    bbisP->irqSrvExit   = 	SMB2BB_IrqSrvExit;
    bbisP->setIrqHandle =   SMB2BB_UnusedIrqHandle;
    bbisP->fkt14        =   SMB2BB_Unused;

    /* exception handling */
    bbisP->expEnable    =	SMB2BB_ExpEnable;
    bbisP->expSrv       =	SMB2BB_ExpSrv;
    bbisP->fkt17        =   SMB2BB_Unused;
    bbisP->fkt18        =   SMB2BB_Unused;
    bbisP->fkt19        =   SMB2BB_Unused;
    /* */
    bbisP->fkt20        =   SMB2BB_Unused;
    bbisP->fkt21        =   SMB2BB_Unused;
    bbisP->fkt22        =   SMB2BB_Unused;
    bbisP->fkt23        =   SMB2BB_Unused;
    bbisP->fkt24        =   SMB2BB_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   SMB2BB_SetStat;
    bbisP->getStat      =   SMB2BB_GetStat;
    bbisP->setMIface    =   SMB2BB_SetMIface;
    bbisP->clrMIface    =   SMB2BB_ClrMIface;
    bbisP->getMAddr     =   SMB2BB_GetMAddr;
    bbisP->fkt30        =   SMB2BB_Unused;
    bbisP->fkt31        =   SMB2BB_Unused;
}


/***************************** SMB2BB_Init ***********************************/
/** Allocate and return board handle.
 *
 *  \param osHdl      \IN  pointer to os specific structure
 *  \param descSpec   \IN  pointer to os specific descriptor specifier
 *  \param hP	      \OUT initialized BBIS Handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_Init(OSS_HANDLE *osHdl, DESC_SPEC *descSpec,
						 BBIS_HANDLE **hP )
{

    BBIS_HANDLE	*h = NULL;
    u_int32     gotsize;
    int32       status;
    u_int32		value;

    /*-------------------------------+
      | initialize the board structure |
      +-------------------------------*/
    /* get memory for the board structure */
    *hP = h = (BBIS_HANDLE*) (OSS_MemGet(osHdl,sizeof(BBIS_HANDLE), &gotsize));

    if ( h == NULL )
        return ERR_OSS_MEM_ALLOC;

    /* cleanup */
	OSS_MemFill( osHdl, sizeof(BBIS_HANDLE), (char*)h, 0x00 );

    /* store data into the board structure */
    h->ownMemSize = gotsize;
    h->osHdl = osHdl;

     /*------------------------------+
	 |  prepare debugging           |
	 +------------------------------*/
    DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
    DBGINIT((NULL,&DBH));

    /*------------------------------+
      |  init id function table       |
      +------------------------------*/
    /* drivers ident function */
    h->idFuncTbl.idCall[0].identCall = Ident;

    /* libraries ident functions */
    h->idFuncTbl.idCall[1].identCall = DESC_Ident;
    h->idFuncTbl.idCall[2].identCall = OSS_Ident;
    /* terminator */
    h->idFuncTbl.idCall[3].identCall = NULL;

    /*------------------------------+
      |  scan descriptor              |
      +------------------------------*/
    /* init descHdl */
    status = DESC_Init( descSpec, osHdl, &h->descHdl );
    if (status)
		return( Cleanup( h,status) );

	/* set debug level for DESC module */
	DESC_DbgLevelSet(h->descHdl, value);

	/* get DEBUG_LEVEL */
    status = DESC_GetUInt32( h->descHdl,
    						 OSS_DBG_DEFAULT,
							 &(h->debugLevel),
							 "DEBUG_LEVEL");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(h,status) );


    /* get DEBUG_LEVEL_DESC - optional*/
    status = DESC_GetUInt32(h->descHdl,
							OSS_DBG_DEFAULT,
							&value,
							"DEBUG_LEVEL_DESC");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(h, status) );


    /* SMB_BUSNBR (required) */
    if((status = DESC_GetUInt32(h->descHdl,
								0,
								&h->smBusNbr,
								"SMB_BUSNBR" ))) {
		DBGWRT_ERR((DBH,"*** %s_Init: Descriptor SMB_BUSNBR missing!\n",
					BBNAME));

		return( Cleanup(h, status) );
    }

    /* set debug level for DESC module */
    DESC_DbgLevelSet(h->descHdl, value);

    DBGWRT_1((DBH,"BB - %s_Init\n",BBNAME));

    /* init linux lists and locking */
    spin_lock_init(&G_smb2Lock);
    INIT_LIST_HEAD( &G_smb2ListHead );

    smb2_wq = create_workqueue(BBNAME);

    /*------------------------------+
     |  prepare debugging           |
     +------------------------------*/
    DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
    DBGINIT((NULL,&DBH));

    return 0;
}


/***************************** SMB2BB_BrdInit ********************************/
/** Pseudo Board initialization
 *
 *  \param h	      \IN  pointer to board handle structure
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_BrdInit( BBIS_HANDLE *h )
{

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,LX_VERSION_I2C_DIFFERS)
    unsigned int i	=	0;
#endif

    OSS_HANDLE *oss 	= 	h->osHdl;
    SMB_HANDLE 	*newHdl = 	NULL;
    u_int32	gotsize 	= 	0;
    DBGWRT_1((DBH, "BB - %s_BrdInit\n",BBNAME));

    /*-----------------------------+
      |  prepare the handle         |
      +-----------------------------*/
    newHdl=(SMB_HANDLE*)OSS_MemGet(oss, sizeof(SMB_HANDLE),&gotsize);
    if ( !newHdl ) {
		DBGWRT_ERR(( DBH, "*** OSS_GetSmbHdl: error allocating Memory\n"));
		return(ERR_OSS_ILL_HANDLE);
    }
    OSS_MemFill(oss, gotsize, (u_int8*)newHdl, 0);


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,LX_VERSION_I2C_DIFFERS)
    /* init addr_data, we have to probe the whole Bus on a adapter */
    for ( i = 0; i<SMB2_I2C_ADDR_COUNT-1; i++)
		normal_addr[i] = (SMB2_I2C_START_ADDR+i);
    normal_addr[SMB2_I2C_ADDR_COUNT-1] = I2C_CLIENT_END;
#endif

    /*-----------------------------+
      |  Register client driver	   |
      +-----------------------------*/
    if (i2c_add_driver(&smb2_driver)) {
		printk("*** OSS_GetSmbHdl: failed to add i2c driver!\n");
		OSS_MemFree( oss, (void*)newHdl, gotsize);
		up(&G_smb2Mutex);
		return(ERR_LL_DEV_NOTRDY);
    }

    return 0;
}





/***************************** SMB2BB_BrdExit ********************************/
/** Pseudo Board deinitialization
 *
 *  \param h	      \IN  pointer to board handle structure
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_BrdExit( BBIS_HANDLE *h )
{
    DBGWRT_1((DBH, "BB - %s_BrdExit\n",BBNAME));

    /*-----------------------------+
      | detach SMBus clients 	   |
      +-----------------------------*/

    i2c_del_driver(&smb2_driver);
    flush_workqueue(smb2_wq);
    destroy_workqueue(smb2_wq);
    return 0;
}


/****************************** SMB2BB_Exit **********************************/
/** deinitializes the bbis handle
 *
 *  \param hP   \IN  hP  pointer to board handle structure
 *
 *  \return        \c 0 On success or error code
 */
static int32 SMB2BB_Exit( BBIS_HANDLE **hP )
{
    BBIS_HANDLE	*h = *hP;
    int32		error = 0;

    DBGWRT_1((DBH, "BB - %s_Exit\n",BBNAME));

    /*------------------------------+
      |  de-init hardware             |
      +------------------------------*/
    /* nothing to do */


    /*------------------------------+
      |  cleanup memory               |
      +------------------------------*/
    error = Cleanup(h, error);
    *hP = NULL;

    return error;
}

/****************************** SMB2BB_BrdInfo *******************************/
/** used to return driver Information like Bustype
 *
 *  \param code    \IN reference to the information we need
 *  \param ...     \IN variable arguments
 *  \return        \c 0 On success or error code
 */
static int32 SMB2BB_BrdInfo( u_int32 code, ... )
{
    int32		status = ERR_SUCCESS;
    va_list     argptr;
    u_int32		dummy;

    va_start(argptr,code);

    switch ( code ) {

        /* supported functions */
    case BBIS_BRDINFO_FUNCTION:
	{
		u_int32 funcCode = va_arg( argptr, u_int32 );
		u_int32 *status = va_arg( argptr, u_int32* );

	    dummy = funcCode;
	    /* no optional BBIS function do anything */
		*status = FALSE;
		break;
	}

	/* number of devices */
    case BBIS_BRDINFO_NUM_SLOTS:
	{
		u_int32 *numSlot = va_arg( argptr, u_int32* );
	    /*
	     * No board handle here, return maximum
	     */
		*numSlot = SMB2BB_BBIS_MAX_DEVS;
		break;
	}

	/* bus type */
    case BBIS_BRDINFO_BUSTYPE:
	{
		u_int32 *busType = va_arg( argptr, u_int32* );

	    *busType = OSS_BUSTYPE_SMB;
		break;
	}

	/* device bus type */
    case BBIS_BRDINFO_DEVBUSTYPE:
	{
		u_int32 mSlot       = va_arg( argptr, u_int32 );
		u_int32 *devBusType = va_arg( argptr, u_int32* );

	    dummy = mSlot;
	    *devBusType = OSS_BUSTYPE_SMB;
		break;
	}

	/* interrupt capability */
    case BBIS_BRDINFO_INTERRUPTS:
	{
		u_int32 mSlot = va_arg( argptr, u_int32 );
		u_int32 *irqP = va_arg( argptr, u_int32* );

	    dummy = mSlot;
		*irqP = 0 /* should be ok since clients need no IRQ */;
		break;
	}

	/* address space type */
    case BBIS_BRDINFO_ADDRSPACE:
	{
		u_int32 mSlot      = va_arg( argptr, u_int32 );
		u_int32 *addrSpace = va_arg( argptr, u_int32* );

	    dummy = mSlot;
	    /*
	     * Indeed we can return anything here. MK will
	     * determine the addresspace itself
	     */
	    *addrSpace = OSS_ADDRSPACE_MEM;
		break;
	}

	/* board name */
    case BBIS_BRDINFO_BRDNAME:
	{
	    char	*brdName = va_arg( argptr, char* );
	    char	*from;

	    /*
	     * build hw name
	     */
	    from = "SMB device";
	    while( (*brdName++ = *from++) );	/* copy string */

	    break;
	}

	/* error */
    default:
		status = ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}


/****************************** SMB2BB_CfgInfo *******************************/
/** Get information about board configuration.
 *
 *  \param h	   \IN  pointer to board handle structure
 *  \param code    \IN reference to the information we need
 *  \param ...     \IN variable arguments
 *
 *  \return        \c 0 On success or error code
 */
static int32 SMB2BB_CfgInfo( BBIS_HANDLE *h,u_int32 code, ... )
{
    va_list		argptr;
    int32       status=0;

    DBGWRT_1((DBH, "BB - %s_CfgInfo\n",BBNAME));

    va_start(argptr,code);

    switch ( code ) {
        /* bus number */
    case BBIS_CFGINFO_BUSNBR:
	{
		u_int32 *busNbr = va_arg( argptr, u_int32* );
	    *busNbr 		= h->smBusNbr;
		break;
	}

	/* error */
    default:
		DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",
					BBNAME,code));
		va_end( argptr );
		return ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}


/************************** SMB2BB_SetMIfacet ********************************/
/** unused here
 *
 *  \return           \c 0 always
 */
static int32 SMB2BB_SetMIface( BBIS_HANDLE *h, u_int32 mSlot,
							   u_int32 addrMode, u_int32 dataMode)
{
    DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=%d\n",BBNAME,mSlot));
    return 0;
}

/************************** SMB2BB_IrqEnable *********************************/
/** unused here
 *
 *  \return           \c ERR_BBIS_ILL_FUNC always
 */
static int32 SMB2BB_IrqEnable(BBIS_HANDLE* h, u_int32 x, u_int32 y)
{
    return ERR_BBIS_ILL_FUNC;
}

/************************** SMB2BB_IrqSrvInit ********************************/
/** unused here
 *
 *  \return           \c ERR_BBIS_ILL_FUNC always
 */
static int32 SMB2BB_IrqSrvInit(BBIS_HANDLE* h, u_int32 x)
{
    return ERR_BBIS_ILL_FUNC;
}

/************************** SMB2BB_IrqSrvExit ********************************/
/** unused here
 *
 *  \return           \c ERR_BBIS_ILL_FUNC always
 */
static void  SMB2BB_IrqSrvExit(BBIS_HANDLE* h, u_int32 x)
{
}


/************************** SMB2BB_ExpEnable *********************************/
/** unused here
 *
 *  \return           \c ERR_BBIS_ILL_FUNC always
 */
static int32 SMB2BB_ExpEnable(BBIS_HANDLE *h, u_int32 mSlot, u_int32 enable)
{
    return ERR_BBIS_ILL_FUNC;
}


/***************************** SMB2BB_ExpSrv *********************************/
/** unused here
 *
 *  \return           \c ERR_BBIS_ILL_FUNC always
 */
static int32 SMB2BB_ExpSrv(BBIS_HANDLE *h, u_int32 mSlot )
{
    return ERR_BBIS_ILL_FUNC;
}


/************************** SMB2BB_clrMIface ********************************/
/** unused here
 *
 *  \return           \c 0 always
 */
static int32 SMB2BB_ClrMIface(BBIS_HANDLE *h,u_int32 mSlot)
{
    DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=%d\n",BBNAME,mSlot));

    return 0;
}


/*************************** SMB2BB_GetMaddr ********************************/
/** unused here
 *
 *  \return           \c 0 always
 */
static int32 SMB2BB_GetMAddr(BBIS_HANDLE *h, u_int32 mSlot, u_int32 addrMode,
							 u_int32 dataMode, void **mAddr, u_int32 *mSize )
{
    DBGWRT_ERR((DBH, "*** BB - %s_GetMAddr: should not be called by MK\n",
				BBNAME,mSlot));
    return 0;
}


/****************************** SMB2BB_SetStat *******************************/
/** Set a specific board parameter
 *
 *  \param h	   \IN  pointer to board handle structure
 *  \param mSlot   \IN  module slot number
 *  \param code    \IN  setstat code
 *  \param value   \IN  setstat value or ptr to blocksetstat data
 *
 *  \return        \c 0 On success or error code
 */
static int32 SMB2BB_SetStat( BBIS_HANDLE *h, u_int32 mSlot, int32 code,
							 INT32_OR_64 value32_or_64 )
{
	int32 value = (int32)value32_or_64; /*32bit value */

    DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=%d code=0x%04x value=0x%x\n",
			  BBNAME, mSlot, code, value));

    switch (code) {
        /* set debug level */
    case M_BB_DEBUG_LEVEL:
		h->debugLevel = value;
		break;

        /* unknown */
    default:
		return ERR_BBIS_UNK_CODE;
    }
    return 0;
}


/****************************** SMB2BB_GetStat *******************************/
/** Get a specific board parameter
 *
 *  \param h	   \IN  pointer to board handle structure
 *  \param mSlot   \IN  module slot number
 *  \param code    \IN  setstat code
 *  \param valueP  \OUT getstat value or ptr to blocksetstat data
 *
 *  \return        \c 0 On success or error code
 */
static int32 SMB2BB_GetStat(BBIS_HANDLE *h, u_int32 mSlot, int32 code,
							INT32_OR_64 *value32_or_64P )
{
	int32* valueP = (int32*)value32_or_64P; /* pointer to 32bit value */

    DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=%d code=0x%04x\n",
			  BBNAME,mSlot,code));

    switch (code) {
        /* get debug level */
    case M_BB_DEBUG_LEVEL:
		*valueP = h->debugLevel;
		break;

        /* ident table */
    case M_MK_BLK_REV_ID:
		*value32_or_64P = (INT32_OR_64)&h->idFuncTbl;
		break;

        /* unknown */
    default:
		return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/******************************* SMB2BB_Unused *******************************/
/** Dummy function
 *
 *  \return        \c 0 ERR_BBIS_ILL_FUNC always
 */
static int32 SMB2BB_Unused( void )		/* nodoc */
{
    return ERR_BBIS_ILL_FUNC;
}

/******************************* SMB2BB_Unused *******************************/
/** Dummy function
 *
 *  \return        \c 0 ERR_BBIS_ILL_FUNC always
 */
static int32 SMB2BB_UnusedIrqHandle(BBIS_HANDLE * bbhP, OSS_IRQ_HANDLE * ossH)
{
    /* return ERR_BBIS_ILL_FUNC */;
	return 0;
	/* 	or open.c complains, this is a "Linux native" BBIS driver anyway */
}




/********************************* Ident used *******************************/
/**  Return ident string
 *
 *  \return        \c pointer to ident string
 */
static char* Ident( void )		/* nodoc */
{
    return ( "SMB2- Pseudo BB Handler: $Id: bb_smb2.c,v 1.21 2014/07/22 13:15:39 ts Exp $" );
}


/********************************* Cleanup  **********************************/
/** Close all handles, free memory and return error code
 *
 *  \param h	   \IN  pointer to board handle structure
 *  \param retCode \IN  return value
 *
 *  \return        \c retCode
 */
static int32 Cleanup(BBIS_HANDLE  *h,int32 retCode /* nodoc */)
{
    DBGWRT_1((DBH, "BB - Cleanup \n"));

    /*------------------------------+
      |  close handles                |
      +------------------------------*/
    /* clean up desc */
    if (h->descHdl){
		DESC_Exit(&h->descHdl);
    }

    /* cleanup debug */
    DBGEXIT((&DBH));

    /*------------------------------+
      |  free memory                |
      +------------------------------*/
    /* release memory for the board handle */
    OSS_MemFree( h->osHdl, (int8*)h, h->ownMemSize);

    /*------------------------------+
	  |  return error code           |
	  +------------------------------*/
    return(retCode);
}


/*****************************************************************************/
/********************** end of Standard BBIS API *****************************/
/*****************************************************************************/

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,WORKQUEUE_API_CHANGE)
static void smb2_wqfunc(void* notneeded)
# else
static void smb2_wqfunc(struct work_struct *notneeded)
#endif
{

	SMB_ACC *smbP = (SMB_ACC *)&G_smbAccess;

	SMB2BB_ReadByteData( smbP->smbH, smbP->flags, smbP->addr, smbP->cmdAddr,
						 &smbP->dat);
	/* signal that we are ready */
	smbP->ready = 1;
	return;
}



#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)

/*************************** oss_smb2_probe **********************************/
/** create a entry in client list for each probed(found) SMBus device
 *
 *	\brief the given BUSNBR in the system.dsc corresponds linear with the found
 *		   Adapters. For example, the first Adapter may be reported as
 *         Adapter 'SMBus I801 adapter at 3400' by i2c-core. Then this Adapter
 *		   is associated with BUSNBR = 0.
 *
 *  \param adapter   \IN  pointer to I2C Adapter struct
 *  \param address   \IN  The LSB aligned SMBus address
 *  \param flags     \IN  SMBus access flags (only in 2.6.x)
 *  \param kind      \IN  blindly probe device
 *  \return           \c 0 On success or error code
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
static int oss_smb2_probe(struct i2c_adapter *adapter, int address, int kind)
#else
static int oss_smb2_probe(struct i2c_adapter *adapter, int address,
						  unsigned short flags, int kind )
#endif
{

    struct i2c_client *client;

	/*
	 * take given SMB_BUSNBR from system.dsc file into account,
	 * since I2C devices with same Address can sit on 2 different adapters.
	 * Member 'unsigned int nr' in struct i2c_adapter (which would give us
	 * this number directly) is available only in 2.6.x kernels.
	 *
	 * So we use member 'name' from the struct i2c_adapter.
	 */
	static char nameold[ADAP_NAME_LEN];
	static char namenew[ADAP_NAME_LEN];

    SMB2_I2C_DATA *data = NULL;
    int err = 0;

    spin_lock(&G_smb2Lock);

	/*
	 * if Adapter name changed since last call to oss_smb2_probe then
	 * we must be on the next Adapter -> Increment global Adapter Nr.
	 */
    strncpy( namenew, adapter->name, ADAP_NAME_LEN);
	if (strncmp( namenew, nameold, ADAP_NAME_LEN ))
		G_globalAdapterNr ++;

    printk( KERN_INFO "Adapter Nr. %d '%s': probe SMBus client 0x%02x\n",
			G_globalAdapterNr - 1, namenew, address);

    if (!(data = kmalloc(sizeof(SMB2_I2C_DATA), GFP_KERNEL))) {
		err = -ENOMEM;
		goto exit;
    }

    /* Initialize/clean our structures */
    memset(data, 0x0, sizeof(SMB2_I2C_DATA));
    client 				= &data->client;
    client->addr 		= address;
    client->driver 		= &smb2_driver;
    client->adapter		= adapter;
    strncpy(client->name, OSS_SMB_DRV_NAME, OSS_SMB_NAM_SIZE);

	data->adapNr		= G_globalAdapterNr - 1; /* SMB_BUSNBR starts at 0 */

	/* new name becomes old name... */
    strncpy( nameold, namenew, ADAP_NAME_LEN);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    i2c_set_clientdata(client, data);
#else
    client->data = data;
#endif

    /* Inform the i2c layer a new client has arrived */
    if ((err = i2c_attach_client(client)))
		goto exit_kfree;

    list_add(&data->node, &G_smb2ListHead);

    spin_unlock(&G_smb2Lock);
    return 0;

exit_kfree:
    kfree(data);

exit:
    return err;
    /* take from i2c101 example */

}

/******************************** oss_smb2_attach ****************************/
/** attach a single client to the given SMBus Adapter
 *
 *  \param adap     	\IN  The SMBus Adapter, passed by Linux i2c-core
 *
 *  \return             \c 0 or error code
 */

static int oss_smb2_attach(struct i2c_adapter *adap)
{
    return i2c_probe(adap, &addr_data, oss_smb2_probe);
}

/******************************** oss_smb2_detach ****************************/
/** detach a single client from our used SMBus Adapter
 *
 *  \param client     	\IN  The i2c_client struct for a particular device
 *
 *  \return             \c 0 or error code
 */
static int oss_smb2_detach(struct i2c_client *client)
{

    int err;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    SMB2_I2C_DATA *data = i2c_get_clientdata(client);
#else
    SMB2_I2C_DATA *data = client->data;
#endif

    printk( KERN_INFO "detaching SMBus client: 0x%02x\n", client->addr );

    if ((err = i2c_detach_client(client)))
		return err;

    list_del(&data->node);
    kfree(data);

    /* -- data invalid now -- */

    return err;
}

#else

/*************************** oss_smb2_probe **********************************/
/** set the client data for this driver (set adapter nr)
 *  and add client to internal list for later use
 *
 *  \param client   \IN  client device that is probed
 *  \param id   	\IN  used device id from id table; not used here (only 1 id)
 *  \return         \c 0 On success or error code
 */
static int oss_smb2_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    SMB2_I2C_DATA *data = NULL;
    int err = 0;

    spin_lock(&G_smb2Lock);

    printk( KERN_INFO "Adapter Nr. %d '%s': probe SMBus client 0x%02x\n",
			client->adapter->nr, client->adapter->name, client->addr);

    if (!(data = kmalloc(sizeof(SMB2_I2C_DATA), GFP_KERNEL))) {
		err = -ENOMEM;
		return err;
    }

    /* Initialize/clean our structures */
    memset(data, 0x0, sizeof(SMB2_I2C_DATA));
	data->adapNr = client->adapter->nr;
	data->client = client;
    i2c_set_clientdata(client, data);

    list_add(&data->node, &G_smb2ListHead);

    spin_unlock(&G_smb2Lock);

    return 0;
}

/******************************** oss_smb2_remove ****************************/
/** remove a single client from our list of used devices
 *
 *  \param client     	\IN  The i2c_client struct for a particular device
 *
 *  \return             \c 0 or error code
 */
static int oss_smb2_remove(struct i2c_client *client)
{

    int err =0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    SMB2_I2C_DATA *data = i2c_get_clientdata(client);
#else
    SMB2_I2C_DATA *data = client->data;
#endif

    printk( KERN_INFO "remove SMB client 0x%02x\n", client->addr );

    list_del(&data->node);
    kfree(data);

    /* -- data invalid now -- */

    return err;
}

/******************************** oss_smb2_detect ****************************/
/** report to OS if the given smb device matches this driver;
 *  if device matches, the name of this driver is written to the info structure
 *
 *  \param new_client     	\IN  The i2c_client struct for a particular device
 *  \param kind				\IN  not used
 *  \param info				\INOUT board info
 *
 *  \return             \c 0 if match; -ENODEV if no match
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
static int oss_smb2_detect(struct i2c_client *new_client, struct i2c_board_info *info)
#else
static int oss_smb2_detect(struct i2c_client *new_client, int kind, struct i2c_board_info *info)
#endif
{
	struct i2c_adapter* adapter = new_client->adapter;
	int address = new_client->addr;
	const char* client_name;

	printk( KERN_INFO "oss_smb2_detect: addr 0x%02x adapter 0x%02x\n", address, adapter->nr);

	if (strlen(info->type) == 0)
	{
		client_name = "smb2";
		strlcpy(info->type, client_name, I2C_NAME_SIZE);
		printk( KERN_INFO " registered driver 'smb2' for device. \n", address, adapter->nr);
		return 0;
	}
	else
		return -ENODEV;
}

#endif /*if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)*/

/******************************* OSS_GetSmbHdl *******************************/
/** Create a fully populated and SMBus access enabled SMB Handle
 *
 *  \brief The function Creates and returns the OSS SMB Handle for use in
 *         SMB2 LL Driver. The busNr correlates to /dev/i2c-<busNbr> in Linux.
 *
 *  \param oss      \IN  The OSS Handle
 *  \param busNbr   \IN  Nr. of SMBus Adapter used
 *  \param smbHdlP  \OUT The Full smb Handle populated with functions
 *
 *  \return         \c 0 On success or error code
 */
int32 OSS_GetSmbHdl( OSS_HANDLE *oss, u_int32 busNr, void **smbHdlP)
{
    u_int32	gotsize 	= 0;
    SMB_HANDLE 	*newHdl = NULL;

    /* protect us from concurrent calls */
    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    /*-----------------------------+
	 |  prepare the handle         |
	 +-----------------------------*/
    newHdl=(SMB_HANDLE*)OSS_MemGet(oss, sizeof(SMB_HANDLE), &gotsize);
    if (newHdl==NULL){
		printk( KERN_ERR "*** OSS_GetSmbHdl: error allocating Memory\n");
		return(ERR_OSS_ILL_HANDLE);
    }
    OSS_MemFill(oss, gotsize, (u_int8*)newHdl, 0);

    /*-----------------------------+
	 |  populate the SMB functions |
	 +-----------------------------*/
    newHdl->Ident  		= 	SMB2BB_Ident;
    newHdl->Exit 		= 	OSS_SmbExit;
    newHdl->QuickComm 		= 	SMB2BB_QuickComm;
    newHdl->WriteByte 		= 	SMB2BB_WriteByte;
    newHdl->ReadByte 		= 	SMB2BB_ReadByte;
    newHdl->WriteByteData	=	SMB2BB_WriteByteData;
    newHdl->ReadByteData 	=	SMB2BB_ReadByteData;
    newHdl->WriteWordData	= 	SMB2BB_WriteWordData;
    newHdl->ReadWordData 	= 	SMB2BB_ReadWordData;
    newHdl->WriteBlockData	=	SMB2BB_WriteBlockData;
    newHdl->ReadBlockData	= 	SMB2BB_ReadBlockData;
    newHdl->ProcessCall		= 	SMB2BB_ProcessCall;
    newHdl->BlockProcessCall	= 	SMB2BB_BlockProcessCall;
    newHdl->AlertResponse	= 	NULL;
    newHdl->AlertCbInstall 	= 	NULL;
    newHdl->AlertCbRemove	= 	NULL;
    newHdl->ReservedFctP1	= 	NULL;
    newHdl->ReservedFctP2	= 	NULL;
    newHdl->ReservedFctP3	= 	NULL;
	newHdl->UseOssDelay		= 	NULL;
    newHdl->SmbXfer			= 	NULL;
    newHdl->I2CXfer			= 	NULL;
    newHdl->Reserved1[0]	= 	busNr;	/* store SMB_BUSNBR of caller */
    newHdl->Reserved1[1]	= 	0;
    newHdl->Capability		= 	0;

    /* finally pass back the populated ready Handle */
    *smbHdlP = newHdl;

    up(&G_smb2Mutex);

    return(ERR_SUCCESS);

}


/************************* getClientFromAddrAndBusnr *************************/
/** Retrieve the i2c_client from list with client->addr == <addr>
 *
 *  \param address   \IN  The LSB aligned SMBus address (including LSBit)
 *  \param busnr     \IN  The Adapter number, must match SMB_BUSNBR
 *                        Descriptor of caller
 *
 *  \return          \c 0 On success or error code
 */
static struct i2c_client *getClientFromAddrAndBusnr(int addr, int busnr )
{

    struct list_head  *pos 		= NULL;
    struct i2c_client *cl		= NULL;
    SMB2_I2C_DATA 	  *ent 		= NULL;
    int shifted_addr			= 0;

    // shift the smb addr (probed clients are stored in Linux style without LSBit)
    shifted_addr = addr >> 1;

    DBGBB( "%s : native (shifted) addr=0x%08x\n",  __FUNCTION__, shifted_addr );

    list_for_each( pos, &G_smb2ListHead ) {

		ent = list_entry( pos, SMB2_I2C_DATA, node );
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
		cl	= &(ent->client);
#else
        cl	= ent->client;
#endif

		/* Got a matching 7bit client address <addr> on Adapter <busnr> ? */
		if ((cl->addr == shifted_addr) && (ent->adapNr == busnr))
			return cl;
    }
    printk( KERN_ERR "*** Client 0x%02x on Adapter 0x%02x not found!\n",
			shifted_addr, busnr );
    return NULL;
}


/****************************** SMB2BB_Ident *********************************/
/** return identification String
 *
 *  \return           \c Ident String
 */
static char* SMB2BB_Ident(void)
{
    return("MDIS OSS_SMB Driver");
}



/****************************** SMB2BB_QuickComm *****************************/
/** do a quick write to the SMBus with just the r/w bit
 *
 *  \brief The function deinitializes the SMB2 device
 *
 *  \param smbHdl     \IN  Pointer to SMBus handle
 *  \param flags      \IN  flags usable for SMBus access
 *  \param addr       \IN  the 7/10 bit SMBus address
 *  \param read_write \IN  the r/w (here data) bit, usually LSB in SMBAddr byte
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_QuickComm( void *smbHdl,
							   u_int32 flags,
							   u_int16 addr,
							   u_int8 read_write )
{

    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    int32 retval = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( "%s : addr=0x%08x\n", __FUNCTION__, addr );

    if ((cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0]))){
		if ( false) //i2c_smbus_write_quick(cl, read_write) < 0)
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		else
			retval = ERR_SUCCESS;

		up(&G_smb2Mutex);
		return(retval);

    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		return(ERR_OSS_UNK_RESOURCE);
    }

}


/****************************** SMB2BB_WriteByte *****************************/
/** Write a byte without addressing to a device
 *
 *  \brief This is the reverse of Read Byte: it sends a single byte
 *         to a device. See Read Byte for more information. Data flow:
 * 		   S Addr Wr [A] Data [A] P
 *
 *  \param smbHdl     \IN  Pointer to SMBus handle
 *  \param flags      \IN  flags usable for SMBus access
 *  \param addr       \IN  the 7/10 bit SMBus address
 *  \param data 	  \IN  data byte to write
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_WriteByte( void *smbHdl,
							   u_int32 flags,
							   u_int16 addr,
							   u_int8 data )
{

    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    int32 retval = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( KERN_INFO "%s : addr=0x%04x data=0x%08x\n",
		   __FUNCTION__, addr, data );

    if ((cl = getClientFromAddrAndBusnr( addr, h->Reserved1[0] ))) {
		if (i2c_smbus_write_byte(cl, data) < 0)
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		else
			retval = ERR_SUCCESS;

		up(&G_smb2Mutex);
		return(retval);

    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		return(ERR_OSS_UNK_RESOURCE);
    }
}



/******************************* SMB2BB_ReadByte *****************************/
/** Read a Byte of data from a device
 *
 *  \brief This reads a single byte from a device, without specifying a device
 *		   register. Some devices are so simple that this interface is enough;
 *		   for others, it is a shorthand if you want to read the same register
 *  	   as in the previous SMBus command. Signal Flow:
 *		 	S Addr Rd [A] [Data] NA P
 *
 *  \param smbHdl     \IN  Pointer to SMBus handle
 *  \param flags      \IN  flags usable for SMBus access
 *  \param addr       \IN  the 7/10 bit SMBus address
 *  \param *data 	  \OUT Pointer to store the read byte
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_ReadByte( void *smbHdl,
							  u_int32 flags,
							  u_int16 addr,
							  u_int8 *data )
{

    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    int32 retval = 0;
    s32 dat = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( KERN_INFO "%s : addr 0x%08x :", __FUNCTION__, addr );

    if ( (cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0])) ){
		/* the read byte is [0x0..0xff] -> fits safely in s32 */
		if ( (dat = i2c_smbus_read_byte(cl)) < 0 )
		{
			DBGBB(KERN_INFO "** error!\n");
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		} else {
			*data = (u_int8)dat;
			DBGBB(KERN_INFO "read: data = %02x\n", *data);
			retval = ERR_SUCCESS;
		}

		up(&G_smb2Mutex);
		return(retval);

    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		return(ERR_OSS_UNK_RESOURCE);
    }

}


/**************************** SMB2BB_WriteByteData ***************************/
/** Write a Byte of data to a device at a given Address
 *
 *  \brief This writes a single byte to a device, to a designated register.
 *		   The register is specified through the Comm byte. This is the
 *		   opposite of the Read Byte Data command. Basic Flow:
 *		   S Addr Wr [A] cmdAddr [A] data [A] P
 *
 *  \param smbHdl     \IN  The SMBus Handle
 *  \param flags      \IN  Flags usable for SMBus access
 *  \param addr       \IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    \IN  The first byte sent after the address
 *  \param data       \IN  the byte which shall be written
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_WriteByteData( void *smbHdl,
								   u_int32 flags,
								   u_int16 addr,
								   u_int8  cmdAddr,
								   u_int8  data )
{

    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    int32 retval = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( KERN_INFO"%s : addr=0x%08x data=0x%08x\n",
		   __FUNCTION__, addr, data );

    if ((cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0] ))){
		if (i2c_smbus_write_byte_data(cl, cmdAddr, data) < 0)
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		else
			retval = ERR_SUCCESS;

		up(&G_smb2Mutex);
		return(retval);

    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		return(ERR_OSS_UNK_RESOURCE);
    }
}


/**************************** SMB2BB_ReadByteData **************************/
/** Read a Byte of data from a device at a given Address
 *
 *  \brief This reads a single byte from a device, from a designated register.
 *         The register is specified through the cmdAddr byte. Basic Flow:
 *         Addr Wr [A] Comm [A] S Addr Rd [A] [Data] NA P
 *
 *  \param smbHdl     \IN  The SMBus Handle
 *  \param flags      \IN  Flags usable for SMBus access
 *  \param addr       \IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    \IN  The first byte sent after the address
 *  \param *data      \OUT Pointer to store the byte which was read
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_ReadByteData( void *smbHdl,
								  u_int32 flags,
								  u_int16 addr,
								  u_int8  cmdAddr,
								  u_int8  *data )
{

    struct i2c_client *cl = NULL;
    SMB_HANDLE* h 	= (SMB_HANDLE*)smbHdl;
    s32 dat 		= 0;
    int32 retval 	= 0;
    int32 tout 		= 0;

/*
 *	OSS Timers are soft IRQs under linux, so care not to call commands that
 *	cat put the process to sleeping state. Not allowed in IRQ context
 */
	if (!in_interrupt()) {
		if (down_interruptible(&G_smb2Mutex)){
			return -EAGAIN;
		}

		DBGBB( KERN_INFO "%s : addr 0x%02x cmdAddr 0x%02x: \n",
			   __FUNCTION__, addr, cmdAddr );
		if ((cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0] ))){
			if ((dat = i2c_smbus_read_byte_data(cl, cmdAddr)) < 0 ) {
				DBGBB( KERN_INFO "error!\n");
				retval = ERR_BUSERR; /* error occured on SMBus */
			} else {
				*data  = (u_int8)dat;
				DBGBB( KERN_INFO "read data = %02x\n", *data);
				retval = ERR_SUCCESS;
			}
			up(&G_smb2Mutex);
			return(retval);

		} else { /* unknown SMBus address */
			up(&G_smb2Mutex);
			return(ERR_OSS_UNK_RESOURCE);
		}
	} else {

		/* call from within IRQ context (OSS alarm timer). Use workqueue */
		G_smbAccess.smbH 	= smbHdl;
		G_smbAccess.flags 	= flags;
		G_smbAccess.addr 	= addr;
		G_smbAccess.cmdAddr = cmdAddr;
		G_smbAccess.ready	= 0;

		/* schedule the ReadByteData call in the queue within process context*/
		queue_work( smb2_wq, &work_obj);

		/*	poll until access is finished - since SMBus access functions
			are blocking anyway its no great time loss */
		while(!G_smbAccess.ready && tout < SMBACCESS_TIMEOUT )
			tout ++;

		if (tout == SMBACCESS_TIMEOUT)
			printk("*** %s: workqueue call timeout!\n", __FUNCTION__ );
		*data = G_smbAccess.dat;

		return 0;
	}
}


/**************************** SMB2BB_WriteWordData **************************/
/** Write a Word(16 bit) of data to a device
 *
 *  \brief This is the opposite operation of the Read Word Data command.
 *		   16 bits of data is read from a device, from a designated register
 *         that is specified through the Comm byte. Signalling:
 *  		S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P
 *
 *  \param smbHdl     \IN  The SMBus Handle
 *  \param flags      \IN  Flags usable for SMBus access
 *  \param addr       \IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    \IN  The first byte sent after the address
 *  \param data       \IN  Pointer to store the word which was read
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_WriteWordData( void *smbHdl,
								   u_int32 flags,
								   u_int16 addr,
								   u_int8  cmdAddr,
								   u_int16 data )
{

    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    int32 retval = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( KERN_INFO "%s : addr=0x%08x data=0x%08x\n",
		   __FUNCTION__, addr, data );

    if (( cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0])) ){
		if (i2c_smbus_write_word_data(cl, cmdAddr, data) < 0)
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		else
			retval = ERR_SUCCESS;

		up(&G_smb2Mutex);
		return(retval);

    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		return(ERR_OSS_UNK_RESOURCE);
    }
}



/**************************** SMB2BB_ReadWordData **************************/
/** Read a Word(16 bit) of data from a device
 *
 *  \brief This is the opposite operation of the Read Word Data command.
 *           16 bits of data is read from a device, from a designated register
 *		   that is specified through the Comm byte. Signalling:
 *		   S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P
 *
 *  \param smbHdl     \IN  The SMBus Handle
 *  \param flags      \IN  Flags usable for SMBus access
 *  \param addr       \IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    \IN  The first byte sent after the address
 *  \param data       \IN  Pointer to store the word which was read
 *
 *  \return           \c 0 On success or error code
 */
static int32 SMB2BB_ReadWordData( void *smbHdl,
								  u_int32 flags,
								  u_int16 addr,
								  u_int8 cmdAddr,
								  u_int16 *data )
{

    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    s32 dat = 0;
    int32 retval = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( KERN_INFO "%s : addr 0x%02x cmdAddr 0x%02x ",
		   __FUNCTION__, addr, cmdAddr );

    if ((cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0]))) {
		/* the read Word is [0x0..0xffff] -> fits safely in s32 */
		if ((dat = i2c_smbus_read_word_data(cl, cmdAddr)) < 0 ) {
			DBGBB( KERN_INFO " ** error\n");
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		} else {
			*data = (u_int16)dat;
			DBGBB( KERN_INFO "read: data = %04x\n", *data);
			retval = ERR_SUCCESS;
		}
		up(&G_smb2Mutex);
		return(retval);
    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		return(ERR_OSS_UNK_RESOURCE);
    }
}


/**************************** SMB2BB_WriteBlockData **************************/
/** Write a block of data from an device
 *
 *  \brief The write Blockdata Function is currently unused and returns
 *         an error.
 *
 *  \param smbHdl     	\IN  The SMBus Handle
 *  \param flags      	\IN  Flags usable for SMBus access
 *  \param addr       	\IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    	\IN  The first byte sent after the address
 *  \param length     	\OUT Pointer to store the byte which was read
 *  \param data     	\IN  Pointer to store the byte which was read
 *
 *  \return             \c ERR_OSS_ILL_PARAM always
 */
static int32 SMB2BB_WriteBlockData ( void *smbHdl,
									 u_int32 flags,
									 u_int16 addr,
									 u_int8 cmdAddr,
									 u_int8 length,
									 u_int8 *data )
{
    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    int32 retval = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( KERN_INFO "%s : addr=0x%08x data=0x%08x\n",
		   __FUNCTION__, addr, data );

    if (( cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0])) ){
		if (i2c_smbus_write_block_data(cl, cmdAddr, length, data) < 0)
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		else
			retval = ERR_SUCCESS;

		up(&G_smb2Mutex);
		return(retval);

    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		return(ERR_OSS_UNK_RESOURCE);
    }

//    return(ERR_OSS_ILL_PARAM);
}


/***************************** SMB2BB_ReadBlockData **************************/
/** Read a block of data from an device
 *
 *  \brief The Read Blockdata Function is currently unused and returns
 *         an error.
 *
 *  \param smbHdl     	\IN  The SMBus Handle
 *  \param flags      	\IN  Flags usable for SMBus access
 *  \param addr       	\IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    	\IN  The first byte sent after the address
 *  \param length     	\OUT Pointer to store the byte which was read
 *  \param data     	\IN  Pointer to store the byte which was read
 *u8
 *  \return             \c ERR_OSS_ILL_PARAM always
 */
static int32 SMB2BB_ReadBlockData( void *smbHdl,
								   u_int32 flags,
								   u_int16 addr,
								   u_int8 cmdAddr,
								   u_int8 *length,
								   u_int8 *data)
{
    struct i2c_client *cl = NULL;
    SMB_HANDLE* h = (SMB_HANDLE*)smbHdl;
    s32 dat = 0;
    int32 retval = 0;

    if (down_interruptible(&G_smb2Mutex))
		return -EAGAIN;

    DBGBB( KERN_INFO "%s : addr 0x%02x cmdAddr 0x%02x ",
		   __FUNCTION__, addr, cmdAddr );
	
	if ((cl = getClientFromAddrAndBusnr(addr, h->Reserved1[0]))) {
		/* the read Word is [0x0..0xffff] -> fits safely in s32 */
		if ((dat = i2c_smbus_read_block_data(cl, cmdAddr, data)) < 0 ) {
			DBGBB( KERN_INFO " ** error\n");
			retval = ERR_BUSERR; /* some error occured on SMBus transaction */
		} else {
			*length = (u_int8)dat;
			DBGBB( KERN_INFO "read: data[0] = %02x\n", data[0]);
			retval = ERR_SUCCESS;
		}
		up(&G_smb2Mutex);
		return(retval);
    } else { /* unknown SMBus address */
		up(&G_smb2Mutex);
		DBGBB( KERN_INFO " ** unknown SMBus address\n");
		return(ERR_OSS_UNK_RESOURCE);
    }

//    return(ERR_OSS_ILL_PARAM);
}


/****************************** SMB2BB_ProcessCall ***************************/
/** do a SMBus Process Call
 *
 *  \brief The Process Call Function is currently unused and returns
 *         an error. It refers mostly to the linux smbus_xfer function
 *  	   which shall not be called directly.
 *
 *  \param smbHdl     	\IN  The SMBus Handle
 *  \param flags      	\IN  Flags usable for SMBus access
 *  \param addr       	\IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    	\IN  The first byte sent after the address
 *  \param data     	\IN  Pointer to store the byte which was read
 *
 *  \return             \c   ERR_OSS_ILL_PARAM always
 */
static int32 SMB2BB_ProcessCall( void *smbHdl,
								 u_int32 flags,
								 u_int16 addr,
								 u_int8 cmdAddr,
								 u_int16 *data )
{
    return(ERR_OSS_ILL_PARAM);
}


/*************************** SMB2BB_BlockProcessCall *************************/
/** do a SMBus Block Process Call
 *
 *  \brief The Block Process Call Function is currently unused and returns
 *         an error.
 *
 *  \param smbHdl     	\IN  The SMBus Handle
 *  \param flags      	\IN  Flags usable for SMBus access
 *  \param addr       	\IN  The LSB aligned SMBus address of the device
 *  \param cmdAddr    	\IN  The first byte sent after the address
 *  \param writeDataLen \IN  Amount of Data bytes to write
 *  \param writeData    \IN  Pointer to Data which are to be written
 *  \param readDataLen  \OUT Amount of Data bytes that were read.
 *  \param readData     \IN  Pointer to store the bytes to which were read
 *
 *  \return             \c ERR_OSS_ILL_PARAM always
 */
static int32 SMB2BB_BlockProcessCall( void *smbHdl,
									  u_int32 flags,
									  u_int16 addr,
									  u_int8 cmdAddr,
									  u_int8 writeDataLen,
									  u_int8 *writeData,
									  u_int8 *readDataLen,
									  u_int8 *readData )
{
    return( ERR_OSS_ILL_PARAM );
}


/******************************** OSS_SmbExit ********************************/
/** Detach clients and driver, free memory
 *
 * \brief The function deinitializes the SMB2 device, detached SMBus clients
 *        and frees allocated Memory and work_queue
 *
 *  \param smbHdlP     \IN Pointer to SMB2 Handle to deallocate
 *
 *  \return            \c currently always success
 */
static int32 OSS_SmbExit( void **smbHdlP )
{

    SMB_HANDLE *smbH = (SMB_HANDLE*)*smbHdlP;
    printk("-> OSS_SmbExit called\n");
    /*-----------------------------+
     | detach clients and driver   |
     +-----------------------------*/
    i2c_del_driver(&smb2_driver);

    flush_workqueue(smb2_wq);

    destroy_workqueue(smb2_wq);

    /* free my allocated Memory */
    if (smbH)
	kfree((void*)smbH);

    smbH = NULL;
    return(ERR_SUCCESS);
}


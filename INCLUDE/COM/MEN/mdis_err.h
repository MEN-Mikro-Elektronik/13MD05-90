/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mdis_err.h
 *
 *       Author: uf
 *        $Date: 2010/10/14 10:07:55 $
 *    $Revision: 1.35 $
 *
 *  Description: MDIS error code definitions
 *
 *     Switches: HPRT LYNX OS9 MSDOS WINNT VXWORKS MENMON LINUX
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mdis_err.h,v $
 * Revision 1.35  2010/10/14 10:07:55  CKauntz
 * R:1. MK returns ERR_MK_SLEEP_ILLHDL when low-level driver hdl has been
 *      closed by changing power state to sleep state
 *   2. MK returns ERR_MK_SLEEP_LOCKED while power state change to sleep
 * M:1. Changed name
 *   2. Added
 *
 * Revision 1.34  2010/09/24 13:12:25  CKauntz
 * R: Illegal handle in MK when ll handle invalid
 * M: Added error code ERR_MK_ILL_HANDLE
 *
 * Revision 1.33  2010/04/14 10:13:38  CRuff
 * R: new oss pci error code needed (bus nr is not found on given pci domain)
 * M: added new error code ERR_OSS_PCI_BUS_NOTFOUND
 *
 * Revision 1.32  2008/11/17 14:05:50  ufranke
 * R: MIPIOS
 * M: added MIPIOS error codes
 *
 * Revision 1.31  2008/07/14 14:51:04  ww
 * added ERR_LL_ILL_MAGIC define.
 *
 * Revision 1.30  2007/02/07 15:59:49  aw
 * added NIOS_II defines
 *
 * Revision 1.29  2005/07/14 10:40:10  cs
 * added ERR_OSSU* defines
 *
 * Revision 1.28  2005/06/23 16:03:19  kp
 * Copyright line changed
 *
 * Revision 1.27  2005/05/10 10:21:33  ts
 * Cosmetics - line width corrected to 80 lines max
 *
 * Revision 1.26  2004/05/26 11:50:33  dpfeuffer
 * ERR_OSS_VME_NO_ADDRWIN added
 *
 * Revision 1.25  2003/08/04 11:56:34  kp
 * added QNX support
 *
 * Revision 1.24  2003/01/28 16:17:26  dschmidt
 * ERR_OSS_BUF_TOOSMALL added
 *
 * Revision 1.23  2002/09/30 11:42:49  RLange
 * cosmetics
 *
 * Revision 1.22  2001/01/19 14:47:08  kp
 * LINUX error codes added
 *
 * Revision 1.21  2000/03/09 16:26:39  Schmidt
 * added VCIRTX error codes
 *
 * Revision 1.20  1999/06/21 12:27:06  Franke
 * readded ERR_MK_NO_BBISDESC
 *
 * Revision 1.19  1999/05/04 17:13:11  Schmidt
 * ERR_LL_ILL_CHAN error added
 *
 * Revision 1.18  1999/05/03 14:16:14  Schoberl
 * ERR_LL_DEV_NOTRDY error added
 *
 * Revision 1.17  1999/04/23 13:30:40  kp
 * deleted ERR_OSS_CALLBACK_CREATE/REMOVE
 *
 * Revision 1.16  1999/03/24 16:51:25  Schmidt
 * ERR_OSS_UNK_ADDRSPACE added
 *
 * Revision 1.15  1999/03/03 17:07:12  kp
 * added OS-9000 error codes
 *
 * Revision 1.14  1999/01/29 14:20:12  Schmidt
 * ERR_OSS_VME_xxx offsets corrected
 * ERR_OSS_ISA_xxx errors added
 *
 * Revision 1.13  1998/09/30 17:21:49  Schmidt
 * ERR_DEV for device specific error codes added
 *
 * Revision 1.12  1998/09/18 12:09:43  see
 * ERR_OSS_SHMEM_xxx errors added
 * ERR_BK_xxx errors added
 * ERR_MK_NO_BBISxxx errors removed
 * ERR_MK_NO_REVID removed
 *
 * Revision 1.11  1998/09/07 11:22:49  Schmidt
 * ERR_MK_NO_REVID added
 *
 * Revision 1.10  1998/09/02 12:08:03  Schmidt
 * ERR_OSS_ILL_HANDLE : illegal handle (any handle not only the OSS handle)
 *
 * Revision 1.9  1998/08/20 13:45:26  see
 * ERR_OSS_CALLBACK_xxx errors added
 *
 * Revision 1.8  1998/07/15 13:48:04  Schmidt
 * ERR_LL_ILL_FUNC error added
 *
 * Revision 1.7  1998/06/17 14:30:43  kp
 * added MENMON switch
 *
 * Revision 1.6  1998/06/12 16:42:17  Franke
 * added ERR_BK
 *
 * Revision 1.5  1998/05/29 16:34:02  see
 * ERR_MK_ILL_DESC error added
 * ERR_OSS_VME_xxx errors added
 * ERR_CBIS_xxx errors added
 * ERR_MK_ILL_MSIZE added
 * ERR_MK_NO_IRQ added
 * ERR_OSS_NO_SYSCLOCK error added
 * ERR_OSS_ALARM_xxx errors added
 *
 * Revision 1.4  1998/04/17 13:34:54  see
 * ERR_MID_xxx renamed to ERR_ID_xxx
 * ERR_SUCCESS added
 *
 * Revision 1.3  1998/03/13 09:47:57  Schmidt
 * add ERR_BBIS_NO_CHECKLOC
 *
 * Revision 1.2  1998/03/12 14:25:54  see
 * ERR_OSS_NO_BUSTOPHYS error added
 *
 * Revision 1.1  1998/02/23 11:17:43  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#ifndef _MDIS_ERR_H_
#define _MDIS_ERR_H_

#ifdef __cplusplus
    extern "C" {
#endif

/* no error */
#define ERR_SUCCESS				0x0000			/* success */

/*--------------------------- OS specific error codes -----------------------*/

/* LynxOS errors */
#if defined(LYNX) || defined (HPRT) || defined(__QNXNTO__)
#define ERR_OS                  0x1000          /* OS specific offset   */
#define ERR_BAD_PATH            EBADF           /* bad path number      */
#define ERR_PATH_FULL           ENFILE          /* path table full      */
#define ERR_BUSERR              EIO             /* bus error occured    */
#endif

/* Linux errors */
#if defined(LINUX)
#define ERR_OS                  0x300           /* OS specific offset   */
#define ERR_BAD_PATH            EBADF           /* bad path number      */
#define ERR_PATH_FULL           ENFILE          /* path table full      */
#define ERR_BUSERR              EIO             /* bus error occured    */

/*
 * must remap these errors under LINUX, since error codes can't be bigger
 * than 0xfff
 */
#define ERR_ID					ERR_OS+0x680
#define ERR_PLD					ERR_OS+0x700
#define ERR_CBIS				ERR_OS+0x780
#define ERR_BK					ERR_OS+0x800
#define ERR_DEV					ERR_OS+0x900
#define ERR_END					ERR_OS+0x9ff
#endif

/* NIOS_II errors */
#if defined(NIOS_II)
#define ERR_OS                  0x300           /* OS specific offset   */
#define ERR_BAD_PATH            EBADF           /* bad path number      */
#define ERR_PATH_FULL           ENFILE          /* path table full      */
#define ERR_BUSERR              EIO             /* bus error occured    */

/*
 * must remap these errors under LINUX, since error codes can't be bigger
 * than 0xfff
 */
#define ERR_ID          ERR_OS+0x680
#define ERR_PLD         ERR_OS+0x700
#define ERR_CBIS        ERR_OS+0x780
#define ERR_BK          ERR_OS+0x800
#define ERR_DEV         ERR_OS+0x900
#define ERR_END         ERR_OS+0x9ff
#endif

/* OS9 errors */
#ifdef OS9
#define ERR_OS                  0x1000          /* OS specific offset   */
#define ERR_BAD_PATH            E_BPNUM         /* bad path number      */
#define ERR_PATH_FULL           E_PTHFUL        /* path table full      */
#define ERR_BUSERR              E_BUSERR        /* bus error occured    */
#endif

/* OS9000 errors */
#ifdef OS9000
#define ERR_OS                  0x1000          /* OS specific offset   */
#define ERR_BAD_PATH            E_BPNUM         /* bad path number      */
#define ERR_PATH_FULL           E_PTHFUL        /* path table full      */
#define ERR_BUSERR              ERR_OS+0x1      /* bus error occured    */
#endif

/* VXWORKS errors */
#ifdef VXWORKS
#define ERR_OS                  0x1000          /* OS specific offset   */
#define ERR_BAD_PATH            EBADF           /* bad path number      */
#define ERR_PATH_FULL           ENFILE          /* path table full      */
#define ERR_BUSERR              ERR_OS+0x01     /* bus error occured    */
#endif

/* MSDOS errors */
#ifdef MSDOS
#define ERR_OS                  0x1000          /* OS specific offset   */
#define ERR_BAD_PATH            ERR_OS+0x01     /* bad path number      */
#define ERR_PATH_FULL           ERR_OS+0x02     /* path table full      */
#define ERR_BUSERR              ERR_OS+0x03     /* bus error occured    */
#endif

/* WINNT errors */
#ifdef WINNT
#define ERR_OS                  0xE00E0000L     /* OS specific offset   */
#define ERR_BAD_PATH            ERR_OS+0x01     /* bad path number      */
#define ERR_PATH_FULL           ERR_OS+0x02     /* path table full      */
#define ERR_BUSERR              ERR_OS+0x03     /* bus error occured    */
#endif

/* VCIRTX errors */
#ifdef VCIRTX
#define ERR_OS                  0xE00E0000L		/* OS specific offset   */
#define ERR_BAD_PATH            ERR_OS+0x01     /* bad path number      */
#define ERR_PATH_FULL           ERR_OS+0x02     /* path table full      */
#define ERR_BUSERR              ERR_OS+0x03     /* bus error occured    */
#endif

/* MENMON errors */
#ifdef MENMON
#define ERR_OS                  0x1000		    /* OS specific offset   */
#define ERR_BAD_PATH            ERR_OS+0x01     /* bad path number      */
#define ERR_PATH_FULL           ERR_OS+0x02     /* path table full      */
#define ERR_BUSERR              ERR_OS+0x03     /* bus error occured    */
#endif

/*----------------------- MK error codes -----------------------------------*/

#define ERR_MK               ERR_OS+0x100  /* offset to MK error codes     */
#define ERR_MK_USERBUF       ERR_MK+0x01   /* user buffer too small (MK)   */
#define ERR_MK_UNK_CODE      ERR_MK+0x02   /* unknown status code (MK)     */
#define ERR_MK_ILL_PARAM     ERR_MK+0x03   /* illegal parameter (MK)       */
#define ERR_MK_DESC_PARAM    ERR_MK+0x04   /* descriptor values out of range */
#define ERR_MK_NO_LLDESC     ERR_MK+0x05   /* device descriptor not found  */
#define ERR_MK_NO_BBISDESC   ERR_MK+0x06   /* board descriptor not found   */
#define ERR_MK_NO_LLDRV      ERR_MK+0x07   /* device driver not found      */
#define ERR_MK_IRQ_INSTALL   ERR_MK+0x09   /* can't install interrupt      */
#define ERR_MK_IRQ_REMOVE    ERR_MK+0x0a   /* can't remove interrupt       */
#define ERR_MK_IRQ_ENABLE    ERR_MK+0x0b   /* can't enable/disable int.    */
#define ERR_MK_ILL_DESC      ERR_MK+0x0c   /* illegal descriptor type      */
#define ERR_MK_ILL_MSIZE	 ERR_MK+0x0d /* address space size conflict    */
#define ERR_MK_NO_IRQ        ERR_MK+0x0e /* board doesn't support interrupts */
#define ERR_MK_SLEEP_ILLHDL  ERR_MK+0x0f   /* sleep closed low-level drv-hdl */
#define ERR_MK_SLEEP_LOCKED  ERR_MK+0x10   /* sleep locked driver usage */

/*----------------------- MIPIOS error codes ------------------------------*/

#define ERR_MIPIOS            		ERR_OS+0x180 /* offset MIPIOS error codes */
#define ERR_MIPIOS_ILL_PARAM  		ERR_MIPIOS+0x01  /* illegal parameter     */
#define ERR_MIPIOS_NOT_SUPPORTED 	ERR_MIPIOS+0x02  /* MIPIOS not supported  */
#define ERR_MIPIOS_NO_DEV     		ERR_MIPIOS+0x03  /* no device             */
#define ERR_MIPIOS_NOT_INITIALIZED	ERR_MIPIOS+0x04  /* not initialized       */
#define ERR_MIPIOS_ALREADY_INIT		ERR_MIPIOS+0x05  /* already initialized   */
#define ERR_MIPIOS_NO_SPACE			ERR_MIPIOS+0x06  /* no space              */
#define ERR_MIPIOS_TGT_CALL    		ERR_MIPIOS+0x07  /* target call           */
#define ERR_MIPIOS_TIMEOUT			ERR_MIPIOS+0x08  /* timeout		          */
#define ERR_MIPIOS_CRC				ERR_MIPIOS+0x09  /* crc			          */
#define ERR_MIPIOS_NO_BUF			ERR_MIPIOS+0x0a  /* no buffer             */
#define ERR_MIPIOS_SOCKET			ERR_MIPIOS+0x0b  /* socket                */

/*------------------------ LL error codes -----------------------------------*/

#define ERR_LL             ERR_OS+0x200    /* offset to LL error codes     	*/
#define ERR_LL_USERBUF     ERR_LL+0x01     /* user buffer too small (LL)   	*/
#define ERR_LL_UNK_CODE    ERR_LL+0x02     /* unknown status code (LL)		*/
#define ERR_LL_ILL_PARAM   ERR_LL+0x03     /* illegal parameter (LL)       	*/
#define ERR_LL_DESC_PARAM  ERR_LL+0x04     /* descriptor values out of range*/
#define ERR_LL_ILL_ID      ERR_LL+0x05     /* wrong device id detected		*/
#define ERR_LL_ILL_DIR     ERR_LL+0x06     /* illegal i/o direction        	*/
#define ERR_LL_DEV_BUSY    ERR_LL+0x07     /* device is busy               	*/
#define ERR_LL_READ        ERR_LL+0x08     /* device read error            	*/
#define ERR_LL_WRITE       ERR_LL+0x09     /* device write error           	*/
#define ERR_LL_ILL_FUNC    ERR_LL+0x0a     /* ll-driver fct. not supported 	*/
#define ERR_LL_DEV_NOTRDY  ERR_LL+0x0b	   /* device is not ready			*/
#define ERR_LL_ILL_CHAN	   ERR_LL+0x0c	   /* illegal channel	 			*/
#define ERR_LL_ILL_MAGIC   ERR_LL+0x0d     /* wrong magic code detected		*/

/*--------------------------- MBUF error codes -----------------------------*/

#define ERR_MBUF           ERR_OS+0x300    /* offset to MBUF error codes    */
#define ERR_MBUF_USERBUF   ERR_MBUF+0x01   /* user buffer too small (MBUF)  */
#define ERR_MBUF_UNK_CODE  ERR_MBUF+0x02   /* unknown status code (MBUF)    */
#define ERR_MBUF_ILL_PARAM ERR_MBUF+0x03   /* illegal parameter (MBUF)      */
#define ERR_MBUF_OVERFLOW  ERR_MBUF+0x04   /* buffer overflow occured       */
#define ERR_MBUF_UNDERRUN  ERR_MBUF+0x05   /* buffer underrun occured       */
#define ERR_MBUF_NO_BUF    ERR_MBUF+0x06   /* no buffer installed           */
#define ERR_MBUF_ILL_SIZE  ERR_MBUF+0x07   /* illegal buffer size           */
#define ERR_MBUF_ILL_DIR   ERR_MBUF+0x08   /* illegal buffer direction      */

/*--------------------------- BBIS error codes -----------------------------*/

#define ERR_BBIS              ERR_OS+0x400  /* offset to BBIS error codes */
#define ERR_BBIS_USERBUF      ERR_BBIS+0x01 /* user buffer too small (BBIS)*/
#define ERR_BBIS_UNK_CODE     ERR_BBIS+0x02 /* unknown status code (BBIS) */
#define ERR_BBIS_ILL_PARAM    ERR_BBIS+0x03 /* illegal parameter (BBIS)   */
#define ERR_BBIS_DESC_PARAM   ERR_BBIS+0x04 /* descriptor values out of range*/
#define ERR_BBIS_ILL_ID       ERR_BBIS+0x05 /* wrong board id detected  */
#define ERR_BBIS_NO_IRQ       ERR_BBIS+0x06 /* cant determine IRQ parameters */
#define ERR_BBIS_ILL_IRQPARAM ERR_BBIS+0x07 /* illegal interrupt parameter */
#define ERR_BBIS_EEPROM       ERR_BBIS+0x08 /* eeprom access error  */
#define ERR_BBIS_ILL_SLOT     ERR_BBIS+0x09 /* illegal board slot   */
#define ERR_BBIS_ILL_DATAMODE ERR_BBIS+0x0a /* illegal addr space (data mode)*/
#define ERR_BBIS_ILL_ADDRMODE ERR_BBIS+0x0b /* illegal addr space (addr mode)*/
#define ERR_BBIS_ILL_FUNC     ERR_BBIS+0x0c /* board handler fct. not supported */
#define ERR_BBIS_NO_CHECKLOC  ERR_BBIS+0x0d /* can't check board location */

/*--------------------------- OSS error codes ------------------------------*/

/* general */
#define ERR_OSS                ERR_OS+0x500 /* offset to OSS error codes */
#define ERR_OSS_UNK_BUSTYPE    ERR_OSS+0x01 /* unknown bus type          */
#define ERR_OSS_TIMEOUT        ERR_OSS+0x02 /* timeout occured           */
#define ERR_OSS_NO_PERM        ERR_OSS+0x03 /* no permission accessing memory*/
#define ERR_OSS_ILL_PARAM      ERR_OSS+0x04 /* illegal parameter (OSS)   	*/
#define ERR_OSS_ILL_HANDLE      ERR_OSS+0x05 /* illegal handle           	*/
#define ERR_OSS_SIG_OCCURED     ERR_OSS+0x06 /* signal occured           	*/
#define ERR_OSS_SIG_SEND        ERR_OSS+0x07 /* cant send signal       	 	*/
#define ERR_OSS_SIG_SET         ERR_OSS+0x08 /* cant install signal     	*/
#define ERR_OSS_SIG_CLR         ERR_OSS+0x09 /* cant remove signal      	*/
#define ERR_OSS_MEM_ALLOC       ERR_OSS+0x0a /* cant allocate memory    	*/
#define ERR_OSS_MEM_FREE        ERR_OSS+0x0b /* cant free memory        	*/
#define ERR_OSS_SEM_CREATE      ERR_OSS+0x0c /* cant create semaphore   	*/
#define ERR_OSS_SEM_REMOVE      ERR_OSS+0x0d /* cant remove semaphore   	*/
#define ERR_OSS_UNK_RESOURCE    ERR_OSS+0x0e /* unknown ressource       	*/
#define ERR_OSS_BUSY_RESOURCE   ERR_OSS+0x0f /* busy ressource          	*/
#define ERR_OSS_MAP_FAILED      ERR_OSS+0x10 /* cant map address space  	*/
#define ERR_OSS_NO_MIKRODELAY   ERR_OSS+0x11 /* mikrodelay not available	*/
#define ERR_OSS_NO_BUSTOPHYS    ERR_OSS+0x12 /* cant map bus address    	*/
#define ERR_OSS_NO_SYSCLOCK     ERR_OSS+0x13 /* no system ticker available	*/
#define ERR_OSS_ALARM_CREATE    ERR_OSS+0x14 /* cant create alarm       	*/
#define ERR_OSS_ALARM_REMOVE    ERR_OSS+0x15 /* cant remove alarm       	*/
#define ERR_OSS_ALARM_SET       ERR_OSS+0x16 /* cant install alarm routine   */
#define ERR_OSS_ALARM_CLR       ERR_OSS+0x17 /* cant remove alarm routine    */
#define ERR_OSS_CALLBACK_SET    ERR_OSS+0x1a /* cant install callback routine*/
#define ERR_OSS_CALLBACK_CLR    ERR_OSS+0x1b /* cant remove callback routine */
#define ERR_OSS_CALLBACK_EMPTY  ERR_OSS+0x1c /* callback queue empty         */
#define ERR_OSS_CALLBACK_OVER   ERR_OSS+0x1d /* callback queue overflow      */
#define ERR_OSS_SHMEM_CREATE    ERR_OSS+0x1e /* cant init shared memory      */
#define ERR_OSS_SHMEM_REMOVE    ERR_OSS+0x1f /* cant exit shared memory      */
#define ERR_OSS_SHMEM_SET       ERR_OSS+0x20 /* cant create shared mem area  */
#define ERR_OSS_SHMEM_LINK      ERR_OSS+0x22 /* cant link to shared mem area */
#define ERR_OSS_SHMEM_CLR       ERR_OSS+0x21 /* cant remove shared mem  area */
#define ERR_OSS_UNK_ADDRSPACE   ERR_OSS+0x22 /* unknown address space type   */
#define ERR_OSS_BUF_TOOSMALL    ERR_OSS+0x23 /* buffer too small             */

/* PCI specific */
#define ERR_OSS_PCI              ERR_OSS+0x80   /* offs. to OSS_PCI err codes*/
#define ERR_OSS_PCI_ILL_DEV		 ERR_OSS_PCI+0x01 /*illegal PCI device       */
#define ERR_OSS_PCI_ILL_DEVNBR   ERR_OSS_PCI+0x02 /*illegal PCI device number*/
#define ERR_OSS_PCI_ILL_ADDRNBR  ERR_OSS_PCI+0x03 /*illegal PCI adress number*/
#define ERR_OSS_PCI_NO_DEVINSLOT ERR_OSS_PCI+0x04 /* no PCI dev found in slot*/
#define ERR_OSS_PCI_UNK_REG      ERR_OSS_PCI+0x05 /* unknown PCI register    */
#define ERR_OSS_PCI_SLOT_TO_DEV  ERR_OSS_PCI+0x06 /* cant map PCI slot to dev*/
#define ERR_OSS_PCI_NO_DEV_FOUND ERR_OSS_PCI+0x07 /* no PCI dev found        */
#define ERR_OSS_PCI_SET_CONFIG   ERR_OSS_PCI+0x08 /* PCI set config failed   */
#define ERR_OSS_PCI_BUS_NOTFOUND ERR_OSS_PCI+0x09 /* PCI bus not found		 */

/* VME specific */
#define ERR_OSS_VME				ERR_OSS+0xa0	  /* offs to OSS_VME errcodes*/
#define ERR_OSS_VME_ILL_SPACE   ERR_OSS_VME+0x01  /* illegal addr. space   	*/
#define ERR_OSS_VME_ILL_SIZE	ERR_OSS_VME+0x02  /* illegal addr space size */
#define ERR_OSS_VME_NO_ADDRWIN	ERR_OSS_VME+0x03  /* cant map VME addr window*/

/* ISA specific */
#define ERR_OSS_ISA				ERR_OSS+0xc0	  /* offs to OSS_ISA errcodes*/
#define ERR_OSS_ISA_NO_PNPDATA	ERR_OSS_ISA+0x01  /* no ISAPNP cfg-data avail*/
#define ERR_OSS_ISA_NO_SUCH_DEV	ERR_OSS_ISA+0x02  /* no such ISA device	*/
#define ERR_OSS_ISA_UNK_RES		ERR_OSS_ISA+0x03  /* unknown ISA resource*/
#define ERR_OSS_ISA_UNAVAIL_RES ERR_OSS_ISA+0x04  /* unavailable ISA resource*/

/* USER MODE */
#define ERR_OSSU				ERR_OSS+0xd0  /* offset for OSSU error codes */
#define ERR_OSSU_PCI_ACC_INIT	ERR_OSSU+0x01 /* initialize PCI access failed*/

/*--------------------------- DESC error codes ------------------------------*/

#define ERR_DESC                ERR_OS+0x600    /* offs to DESC error codes  */
#define ERR_DESC_CORRUPTED      ERR_DESC+0x01   /* descriptor data corrupted */
#define ERR_DESC_KEY_NOTFOUND   ERR_DESC+0x02   /* descriptor key not found  */
#define ERR_DESC_BUF_TOOSMALL   ERR_DESC+0x03   /* descr. buffer too small   */

/*--------------------------- ID error codes --------------------------------*/

#ifndef ERR_ID
# define ERR_ID                 ERR_OS+0x700   	/* offset to ID error codes  */
#endif
#define ERR_ID_NOTFOUND        ERR_ID+0x01    	/* eeprom not found          */
#define ERR_ID_CORRUPTED       ERR_ID+0x02    	/* eeprom data corrupted     */
#define ERR_ID_ILL_PARAM       ERR_ID+0x03    	/* illegal parameter         */

/*--------------------------- PLD error codes -------------------------------*/
#ifndef ERR_PLD
# define ERR_PLD                 ERR_OS+0x800   /* offset to PLD error codes */
#endif
#define ERR_PLD_NOTFOUND        ERR_PLD+0x01    /* no response from PLD      */
#define ERR_PLD_INIT            ERR_PLD+0x02    /* error initializing PLD    */
#define ERR_PLD_LOAD            ERR_PLD+0x03    /* error loading PLD         */
#define ERR_PLD_TERM            ERR_PLD+0x04    /* errro terminating PLD     */

/*--------------------------- CBIS error codes ------------------------------*/
#ifndef ERR_CBIS
# define ERR_CBIS              	ERR_OS+0x900    /* offs to CBIS error codes */
#endif
#define ERR_CBIS_UNK_CODE       ERR_CBIS+0x01   /* unknown status code      */
#define ERR_CBIS_ILL_PARAM      ERR_CBIS+0x02   /* illegal parameter        */
#define ERR_CBIS_ILL_FUNC       ERR_CBIS+0x03   /* function not supported   */

/*---------------------------  BK  error codes -----------------------------*/
#ifndef ERR_BK
# define ERR_BK                  ERR_OS+0xa00 /* offset to BK error codes*/
#endif
#define ERR_BK_USERBUF          ERR_BK+0x01   /* user buffer too small    */
#define ERR_BK_UNK_CODE         ERR_BK+0x02   /* unknown status code      */
#define ERR_BK_ILL_PARAM        ERR_BK+0x03   /* illegal parameter        */
#define ERR_BK_DESC_PARAM       ERR_BK+0x04   /* descr. values out of range */
#define ERR_BK_NO_LLDESC        ERR_BK+0x05   /* board descriptor not found */
#define ERR_BK_NO_LLDRV         ERR_BK+0x06   /* board handler not found   */
#define ERR_BK_ILL_DESC         ERR_BK+0x07   /* illegal descriptor type   */

/*----------------------  DEVICE SPECIFIC error codes ----------------------*/
#ifndef ERR_DEV
# define ERR_DEV	ERR_OS+0xf00    /* offset to DEVICE SPECIFIC error codes */
#endif
/* Remark: Device specific error codes are defined */
/*         in the device specific include files.   */

/*----------------------- end of error codes ------------------------------*/

#ifndef ERR_END
# define ERR_END                 ERR_OS+0xfff    /* last possible error code */
#endif

#ifdef __cplusplus
    }
#endif

#endif/*_MDIS_ERR_H_*/




















/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bb_defs.h
 *
 *       Author: ds
 *        $Date: 2011/05/18 16:50:25 $
 *    $Revision: 1.16 $
 *
 *  Description: BBIS board handler common definitions
 *
 *     Switches: _NO_BBIS_HANDLE	exclude BBIS_HANDLE declaration
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_defs.h,v $
 * Revision 1.16  2011/05/18 16:50:25  CRuff
 * R: support of pci domains
 * M: added new CFGINFO identifier: BBIS_CFGINFO_PCI_DOMAIN
 *
 * Revision 1.15  2011/01/19 09:36:26  dpfeuffer
 * R: board handle dependent address space type required (e.g. for chameleon BBIS)
 * M: BBIS_CFGINFO_ADDRSPACE define added
 *
 * Revision 1.14  2010/02/24 14:23:44  dpfeuffer
 * R: BBIS_BRDINFO_BRDNAME_MAXSIZE too small for smbpci_16z001 bbis
 *    (causes BAD_POOL_HEADER BSOD under Windows)
 * M: BBIS_BRDINFO_BRDNAME_MAXSIZE changed from 20 to 80 chars
 *
 * Revision 1.13  2005/06/10 15:18:55  ts
 * corrected wrong empty string BBIS_SLOT_STR_UNK to "" ,not '\0'.
 * caused kernel oops.
 *
 * Revision 1.12  2004/05/24 10:27:46  dpfeuffer
 * BBIS_BRDINFO_BRDNAME_MAXSIZE changed
 *
 * Revision 1.11  2003/04/30 17:19:59  dschmidt
 * BBIS_CFGINFO_PCI_FUNCNBR added
 *
 * Revision 1.10  2003/01/28 16:17:23  dschmidt
 * defines for slot information added
 *
 * Revision 1.9  1999/08/05 16:11:05  kp
 * changed BBIS_BRDINFO_PCI_DEVNBR to BBIS_CFGINFO_PCI_DEVNBR
 *
 * Revision 1.8  1999/07/26 15:57:45  Schmidt
 * BBIS_BRDINFO_PCI_DEVNBR added
 * BBIS_BRDINFO_DEVBUSTYPE added
 *
 * Revision 1.7  1999/03/24 16:52:28  Schmidt
 * BBIS_BRDINFO_ADDRSPACE added
 *
 * Revision 1.6  1998/06/18 14:19:27  Franke
 * added  BBIS_SLOTS_ONBOARDDEVICE_START
 *
 * Revision 1.5  1998/06/02 11:00:08  see
 * ifdef _NO_BBIS_HANDLE added to exclude BBIS_HANDLE declaration
 * BBIS_IRQ_NONE added
 *
 * Revision 1.4  1998/04/14 16:10:17  Schmidt
 * BBIS_IRQ_EXCLUSIVE (0x0004) added, BBIS_IRQ_SHARED set to 0x0008
 *
 * Revision 1.3  1998/04/14 15:03:48  Schmidt
 * BBIS_IRQ_EXCLUSIVE removed
 *
 * Revision 1.2  1998/04/14 14:53:22  Schmidt
 * BBIS_BRDINFO_INTERRUPTS, BBIS_IRQ_DEVIRQ, BBIS_IRQ_EXPIRQ added
 *
 * Revision 1.1  1998/02/19 09:14:27  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BB_DEFS_H_
#define _BB_DEFS_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*---------------------------------------------------------------------------+
|    DEFINITIONS FOR BrdInfo()                                               |
+---------------------------------------------------------------------------*/

/*------------- bus type ---------------*/
#define BBIS_BRDINFO_BUSTYPE        0x0100

/*----------- function info ------------*/
#define BBIS_BRDINFO_FUNCTION       0x0200
#define BBIS_FUNC_IRQENABLE         0x0201
#define BBIS_FUNC_IRQSRVINIT        0x0202
#define BBIS_FUNC_IRQSRVEXIT        0x0203
#define BBIS_FUNC_EXPENABLE         0x0204
#define BBIS_FUNC_EXPSRV            0x0205

/*------------ slot number -------------*/
#define BBIS_BRDINFO_NUM_SLOTS      0x0300

/*-------- interrupt capability --------*/
#define BBIS_BRDINFO_INTERRUPTS		0x0400
#define BBIS_IRQ_DEVIRQ				0x0001	/* device IRQ supported */
#define BBIS_IRQ_EXPIRQ				0x0002	/* exception IRQ supp.  */

/*---- type of device address space ----*/
#define BBIS_BRDINFO_ADDRSPACE      0x0500

/*-------- type of device bus ----------*/
#define BBIS_BRDINFO_DEVBUSTYPE     0x0600

/*------------ board name --------------*/
#define BBIS_BRDINFO_BRDNAME		 0x0700
#define BBIS_BRDINFO_BRDNAME_MAXSIZE 80

/*---------------------------------------------------------------------------+
|    DEFINITIONS FOR CfgInfo()                                               |
+---------------------------------------------------------------------------*/

/*------------- bus number -------------*/
#define BBIS_CFGINFO_BUSNBR         0x1100

/*----------- interrupt info -----------*/
#define BBIS_CFGINFO_IRQ            0x1200
#define BBIS_CFGINFO_EXP            0x1300

/*------ flags for interrupt info ------*/
#define BBIS_IRQ_NONE               0x0000
#define BBIS_IRQ_EXCEPTION          0x0002
#define BBIS_IRQ_EXCLUSIVE          0x0004
#define BBIS_IRQ_SHARED             0x0008

/*-------- PCI dev/func number ---------*/
#define BBIS_CFGINFO_PCI_DEVNBR     0x1400
#define BBIS_CFGINFO_PCI_FUNCNBR    0x1410
#define BBIS_CFGINFO_PCI_DOMAIN	    0x1420

/*-------- slot information ------------*/
#define BBIS_CFGINFO_SLOT			0x1500

/*---- flags for slot information ------*/
/* for BK/MK usage */
#define BBIS_SLOT_PLGAB_DEV		0x1000		/* pluggable device */
#define BBIS_SLOT_ONBRD_DEV		0x2000		/* onboard device */
#define BBIS_SLOT_EXIST_YES		0x0001		/* device exist */
#define BBIS_SLOT_EXIST_NO		0x0002		/* device doesn't exist */

/* BBIS usage for pluggable devices */
#define BBIS_SLOT_OCCUP_YES		(BBIS_SLOT_PLGAB_DEV | BBIS_SLOT_EXIST_YES)
#define BBIS_SLOT_OCCUP_NO		(BBIS_SLOT_PLGAB_DEV | BBIS_SLOT_EXIST_NO)

/* BBIS usage for onboard devices */
#define BBIS_SLOT_OCCUP_ALW		(BBIS_SLOT_ONBRD_DEV | BBIS_SLOT_EXIST_YES)
#define BBIS_SLOT_OCCUP_DIS		(BBIS_SLOT_ONBRD_DEV | BBIS_SLOT_EXIST_NO)

#define BBIS_SLOT_NBR_UNK		0xffffffff	/* indicates an unknown number */
#define BBIS_SLOT_STR_UNK		"" 			/* indicates an unknown string 	*/
#define BBIS_SLOT_STR_MAXSIZE	80			/* max size for strings 		*/

/*---- type of mSlot address space -----*/
#define BBIS_CFGINFO_ADDRSPACE      0x1600

/*---------------------------------------------------------------------------+
|    DEFINITIONS FOR IRQ-FKT.                                                |
+---------------------------------------------------------------------------*/
#define BBIS_IRQ_NO     0x0001      /* no module interrupt */
#define BBIS_IRQ_UNK    0x0002      /* perhaps module interrupt */
#define BBIS_IRQ_YES    0x0004      /* module interrupt */
#define BBIS_IRQ_EXP    0x0008      /* exception interrupt */


/*---------------------------------------------------------------------------+
|    DEFINITIONS FOR GetMAddr()  / SetMIface()                               |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
|    DEFINITIONS FOR BUS TO LOCAL CONVERSION                                 |
+---------------------------------------------------------------------------*/
#define BBIS_BUS_TO_LOCAL_NO    0       /* no bus to local conversion */
#define BBIS_BUS_TO_LOCAL       1       /* bus to local conversion */

/*---------------------------------------------------------------------------+
|    DEFINITIONS FOR MISCELLANOUS                                            |
+---------------------------------------------------------------------------*/
#define BBIS_SLOTS_ONBOARDDEVICE_START      0x1000

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
#ifndef _NO_BBIS_HANDLE
   typedef void *BBIS_HANDLE;
#endif


#ifdef __cplusplus
    }
#endif

#endif /* _BB_DEFS_H_ */












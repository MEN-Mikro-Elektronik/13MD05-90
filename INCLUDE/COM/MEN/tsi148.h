/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *		  \file	 tsi148.h
 *
 *		\author	 rt
 *		  $Date: 2010/06/25 08:52:01 $
 *	  $Revision: 3.10 $
 *
 *		 \brief	 Tundra	TSI148 PCI2VME bridge register defines
 *
 *	   Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: tsi148.h,v $
 * Revision 3.10  2010/06/25 08:52:01  UFranke
 * R: gctrl, ga, revid wrong addresses
 * M: _TSI148_GCSR structure fixed
 *
 * Revision 3.9  2010/04/23 16:53:57  CKauntz
 * R: Mailbox bit offset in intex register not defined
 * M: Added define TSI148_INTEX_MB0_BIT  for Mailbox bit offset
 *
 * Revision 3.8  2009/06/03 19:35:29  rt
 * R: 1.) Missing defines.
 * M: 1.) TSI148_VMCTRL..., VCTRL_GTO..., TSI148_LM..., TSI148_INTEX..., TSI148_DCTL
 *        defines added
 *
 * Revision 3.7  2009/05/07 09:52:04  rt
 * R: 1) Not compilable with some flags.
 *     2) Code clean-up.
 * M: 1) Flags TSI148_DXAT_TYP_MASK and TSI148_DXAT_TM_MASK redefined.
 *     2) Comments changed.
 *
 * Revision 3.6  2009/04/30 22:10:11  rt
 * R: 1) Wrong register bit masks.
 *    2) Missing defines.
 * M: 1) TSI148_ITXAX_MASK_Axx fixed.
 *    2) TSI148_EDPAT_EDPCL and TSI148_OTOF_OFFL_MASK  added.
 *
 * Revision 3.5  2008/08/05 14:54:03  rt
 * R:1. Some of the defined values are wrong and may cause compiler errors
 * M:1. Removed leading '0' from integer values since C compiler will interpret
 *      them as octal values
 *
 * Revision 3.4  2008/08/01 12:54:39  cs
 * M: added some defines for location monitor and GCSR inbound mapping
 *
 * Revision 3.3  2008/07/29 22:23:07  cs
 * R: most defines of TSI148 chip registers where missing
 * M: added all necessary defines for chip registers
 *
 * Revision 3.2  2008/03/25 13:30:38  rt
 * GPL
 *
 * Revision 3.1  2008/01/09 13:54:17  rt
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2007-2008 by MEN Mikro	Elektronik GmbH, Nuremberg,	Germany
 ****************************************************************************/


#ifndef	_TSI148_H
#define	_TSI148_H

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * PCFS	Register Group
 */
#define	TSI148_VEN_ID		0x10E3
#define	TSI148_DEV_ID		0x0148

/*
 * LCSR	Register Group
 */

/* Outbound	Window */

#define TSI148_OUTBOUND_NO	8

typedef	struct _TSI148_OUTBOUND
{
	u_int32		otsau;		/* starting	address	upper */
	u_int32		otsal;		/* starting	address	lower */
	u_int32		oteau;		/* ending address upper	*/
	u_int32		oteal;		/* ending address lower	*/
	u_int32		otofu;		/* translation offset upper	*/
	u_int32		otofl;		/* translation offset lower	*/
	u_int32		otbs;		/* 2eSST broadcast select */
	u_int32		otat;		/* attribute */
} TSI148_OUTBOUND;

/* Inbound Window */

#define TSI148_INBOUND_NO	8

typedef	struct _TSI148_INBOUND
{
	u_int32		itsau;		/* starting	address	upper */
	u_int32		itsal;		/* starting	address	lower */
	u_int32		iteau;		/* ending address upper	*/
	u_int32		iteal;		/* ending address lower	*/
	u_int32		itofu;		/* translation offset upper	*/
	u_int32		itofl;		/* translation offset lower	*/
	u_int32		itat;		/* attribute */
	u_int8		reserved[4];
} TSI148_INBOUND;

#define TSI148_DMACTL_NO	2

typedef	struct _TSI148_DMACTL
{
	u_int32		dctl;		/* DMA Control  */
	u_int32		dsta;		/* DMA Status */
	u_int32		dcsau;		/* DMA Current Source Address Upper */
	u_int32		dcsal;		/* DMA Current Source Address Lower */
	u_int32		dcdau;		/* DMA Current Destination Address Upper */
	u_int32		dcdal;		/* DMA Current Destination Address Lower */
	u_int32		dclau;		/* DMA Current Link Address Upper */
	u_int32		dclal;		/* DMA Current Link Address Lower */
	u_int32		dsau;		/* DMA Source Address Upper */
	u_int32		dsal;		/* DMA Source Address Lower */
	u_int32		ddau;		/* DMA Destination Address Upper */
	u_int32		ddal;		/* DMA Destination Address Lower */
	u_int32		dsat;		/* DMA Source Attribute */
	u_int32		ddat;		/* DMA Destination Attribute */
	u_int32		dnlau;		/* DMA Next Link Address Upper */
	u_int32		dnlal;		/* DMA Next Link Address Lower */
	u_int32		dcnt;		/* DMA Count */
	u_int32		ddbs;		/* DMA Destination Broadcast Select */
	u_int8		reserved[0x038]; /* !!not actually part of the DMA controllers!! */
} TSI148_DMACTL;

/* whole PCFS (0x100 Bytes, standard PCI/X header )*/
typedef struct _TSI148_PCFS
{
	u_int16		veni;
	u_int16		devi;
	u_int16		cmmd;
	u_int16		stat;
	u_int8		revi;
	u_int8		clas[3];
	u_int8		clsz;
	u_int8		mlat;
	u_int8		head;
	u_int8		reserved0;
	u_int32		mbarl;
	u_int32		mbaru;
	u_int8		reserved2[0x024];
	/* PCI/X Interrupt Line/Interrupt Pin/Minimum Grant/Maximum Latency		0x03c */
	u_int8		intl;
	u_int8		intp;
	u_int8		mngn;
	u_int8		mxla;
	/* PCI-X Capabilities/Status 		0x040 */
	u_int32		pcixcap;
	u_int32		pcixstat;
	u_int8		reserved4[0x0b8];
} TSI148_PCFS;

/* whole LCSR */
typedef	struct _TSI148_LCSR
{
	/* Outbound Translation Registers					0x100 */
	TSI148_OUTBOUND	outbound[TSI148_OUTBOUND_NO];
	u_int8			reserved0[0x004];
	/* VMEbus interrupt acknowledge						0x204 */
	u_int32			viack[7];
	/* RMW 												0x220 */
	u_int32			rmwau;
	u_int32			rmwal;
	u_int32			rmwen;
	u_int32			rmwc;
	u_int32			rmws;
	/* VMEbus control									0x234 */
	u_int32			vmctrl;
	u_int32			vctrl;
	u_int32			vstat;
	/* PCI/X status										0x240 */
	u_int32			pcsr;
	u_int8          reserved2[0x00c];
	/* VME filter										0x250 */
	u_int32			vmefl;
	u_int8			reserved4[0x00c];
	/* VME exception status								0x260 */
	u_int32			veau;
	u_int32			veal;
	u_int32			veat;
	u_int8			reserved6[0x004];
	/* PCI/X error status								0x270 */
	u_int32			edpau;
	u_int32			edpal;
	u_int32			edpxa;
	u_int32			edpxs;
	u_int32			edpat;
	u_int8			reserved8[0x07c];
	/* Inbound Translation Registers					0x300 */
	TSI148_INBOUND	inbound[TSI148_INBOUND_NO];
	/* Inbound Translation GCSR							0x400 */
	u_int32			gbau;
	u_int32			gbal;
	u_int32			gcsrat;
	/* Inbound Translation CRG							0x40C */
	u_int32			cbau;
	u_int32			cbal;
	u_int32			crgat;
	/* Inbound Translation CR/CSR						0x418 */
	u_int32			crou;
	u_int32			crol;
	u_int32			crat;
	/* Inbound Translation Location Monitor				0x424 */
	u_int32			lmbau;
	u_int32			lmbal;
	u_int32			lmat;
	/* VMEbus Interrupt control							0x430 */
	u_int32			bcu64;
	u_int32			bcl64;
	u_int32			bpgtr;
	u_int32			bpctr;
	u_int32			vicr;
	u_int8			reserved12[0x004];
	/* Local bus Interrupt control						0x448 */
	u_int32			inten;
	u_int32			inteo;
	u_int32			ints;
	u_int32			intc;
	u_int32			intm1;
	u_int32			intm2;
	u_int8			reserved14[0x0a0];
	/* DMA controllers 									0x500/0x580 */
	TSI148_DMACTL	dmactl[TSI148_DMACTL_NO];
} TSI148_LCSR;


/* whole GCSR */
typedef	struct _TSI148_GCSR
{
	/* Header 											0x600 */
	u_int16			veni;
	u_int16			devi;
	/* Control 											0x604 */
	u_int16			gctrl;
	/* Geographic Address 								0x606 */
	u_int8			ga;
	/* Geographic Address 								0x607 */
	u_int8			revid;
	/* Semaphores										0x608 */
	u_int8			sema[8];
	/* Mail Boxes 										0x610 */
	u_int32			mbox[4];
} TSI148_GCSR;

/* whole CSR */
typedef	struct _TSI148_CSR
{
	u_int8			reserved[1012];
	u_int32			csrbcr;							/*  0xff4 */
	u_int32			csrbsr;							/*  0xff8 */
	u_int32			cbar;							/*  0xffc */
} TSI148_CSR;


/*
 * TSI148 Combined Register Group (whole registers)
 */
typedef	struct _TSI148_CRG
{
	TSI148_PCFS	pcfs;			/* PCFS Register Group 		0x000 */
	TSI148_LCSR	lcsr;			/* LCSR Register Group		0x100 */
	TSI148_GCSR gcsr;			/* GCSR Register Group 		0x600 */
	u_int8		reserved[1504];	/*							0x620 */
	TSI148_CSR	csr;			/* CSR Register Group 		0xC00 */
} TSI148_CRG;

/* LCSR Register	Bit	Definitions	*/

/* Outbound Translation Offset (OTOF) */
#define TSI148_OTOF_OFFL_MASK		(0xffffffffffff0000ULL)	/* Offset Mask */

/* Outbound	Translation Attribute (OTAT) */
#define	TSI148_OTAT_EN				(0x1<<31)	/* Enable */
#define	TSI148_OTAT_MRPFD			(0x1<<18)	/* Memory Read Pref. Disable */

#define	TSI148_OTAT_PFS_MASK		(0x3<<16)	/* Prefetch	Size Mask */
#define	TSI148_OTAT_PFS_2			(0x0<<16)	/* 2 Cache Lines */
#define	TSI148_OTAT_PFS_4			(0x1<<16)	/* 4 Cache Lines */
#define	TSI148_OTAT_PFS_8			(0x2<<16)	/* 8 Cache Lines */
#define	TSI148_OTAT_PFS_16			(0x3<<16)	/* 16 Cache	Lines */

#define	TSI148_OTAT_2ESSTM_MASK		(0x3<<11)	/* 2eSST Mode Mask */
#define	TSI148_OTAT_2ESSTM_160		(0x0<<11)	/* SST160 */
#define	TSI148_OTAT_2ESSTM_267		(0x1<<11)	/* SST267 */
#define	TSI148_OTAT_2ESSTM_320		(0x2<<11)	/* SST320 */

#define	TSI148_OTAT_TM_MASK			(0x7<<8)	/* Transfer Mode Mask */
#define	TSI148_OTAT_TM_SCT			(0x0<<8)	/* SCT */
#define	TSI148_OTAT_TM_BLT			(0x1<<8)	/* BLT */
#define	TSI148_OTAT_TM_MBLT			(0x2<<8)	/* MBLT	*/
#define	TSI148_OTAT_TM_2EVME		(0x3<<8)	/* 2eVME */
#define	TSI148_OTAT_TM_2ESST		(0x4<<8)	/* 2eSST */
#define	TSI148_OTAT_TM_2ESSTB		(0x5<<8)	/* 2eSST Broadcast */

#define	TSI148_OTAT_DBW_MASK		(0x3<<6)	/* VMEbus Data Bus Width */
#define	TSI148_OTAT_DBW_16			(0x0<<6)	/* 16 bit */
#define	TSI148_OTAT_DBW_32			(0x1<<6)	/* 32 bit */

#define	TSI148_OTAT_SUP				(0x1<<5)	/* VMEbus Supervisory Mode */
#define	TSI148_OTAT_PGM				(0x1<<4)	/* VMEbus Program Mode */

#define	TSI148_OTAT_AMODE_MASK		(0xf<<0)	/* Address Mode	Mask */
#define	TSI148_OTAT_AMODE_A16		(0x0<<0)	/* A16 */
#define	TSI148_OTAT_AMODE_A24		(0x1<<0)	/* A24 */
#define	TSI148_OTAT_AMODE_A32		(0x2<<0)	/* A32 */
#define	TSI148_OTAT_AMODE_A64		(0x4<<0)	/* A64 */
#define	TSI148_OTAT_AMODE_CSR		(0x5<<0)	/* CSR */
#define	TSI148_OTAT_AMODE_UM1		(0x8<<0)	/* User1 */
#define	TSI148_OTAT_AMODE_UM2		(0x9<<0)	/* User2 */
#define	TSI148_OTAT_AMODE_UM3		(0xa<<0)	/* User3 */
#define	TSI148_OTAT_AMODE_UM4		(0xb<<0)	/* User4 */

/* VME Master Control Register (VMCTRL) */
#define TSI148_VMCTRL_RMWEN  		(0x1<<20) 	/* RMW Enable */
#define TSI148_VMCTRL_VTON_MASK		(0x7<<8)	/* VME Master Time On */
#define TSI148_VMCTRL_VTON_512US	(0x7<<8)	/* 512us VME Master Time On */
#define TSI148_VMCTRL_VREL_MASK		(0x3<<3) 	/* Release Mode Mask */
#define TSI148_VMCTRL_VREL_RWD		(0x0<<3) 	/* Release When Done */
#define TSI148_VMCTRL_VREL_ROR		(0x1<<3) 	/* Release on Request */
#define TSI148_VMCTRL_FAIR			(0x1<<2) 	/* Fair Mode */
#define TSI148_VMCTRL_VREQL_MASK	(0x3<<0) 	/* Request Level Mask */
#define TSI148_VMCTRL_VREQL_3		(0x3<<0) 	/* Request Level 3 */

/* VMEbus Control Register (VCTRL) */
#define TSI148_VCTRL_DLT_MASK  		(0xf<<24) 	/* Deadlock Timer Mask */
#define TSI148_VCTRL_NELBB	     	(0x1<<20) 	/* No Early Release of Bus Busy */
#define TSI148_VCTRL_SRESET    		(0x1<<17) 	/* System Reset */
#define TSI148_VCTRL_LRESET		   	(0x1<<16) 	/* Local Reset */
#define TSI148_VCTRL_SFAILAI		(0x1<<15) 	/* System Fail Auto Slot ID): */
#define TSI148_VCTRL_BID_MASK  		(0x1f<<8)  	/* Broadcast ID */
#define TSI148_VCTRL_ATOEN     		(0x1<<7)  	/* Arbiter Time-out Enable */
#define TSI148_VCTRL_GTO_MASK 		(0xf<<0)  	/* Global Time-Out Mask */
#define TSI148_VCTRL_GTO_2048US		(0x8<<0)  	/* Global Time-Out 2048 us */

/* VMEbus Status Register (VSTAT) */
#define TSI148_VSTAT_CPURST    		(0x1<<15) 	/* Clear Power Up Reset */
#define TSI148_VSTAT_BDFAIL     	(0x1<<14) 	/* Board Fail */
#define TSI148_VSTAT_PURSTS    		(0x1<<12) 	/* Power Up Reset Status */
#define TSI148_VSTAT_BDFAILS	   	(0x1<<11) 	/* Board Fail Status */
#define TSI148_VSTAT_SYSFLS			(0x1<<10) 	/* System Fail Status): */
#define TSI148_VSTAT_ACFAILS  		(0x1<<9)  	/* AC Fail Status */
#define TSI148_VSTAT_SCONS     		(0x1<<8)  	/* System Controller Status */
#define TSI148_VSTAT_GAP       		(0x1<<5)  	/* Geographic Address Parity */
#define TSI148_VSTAT_GA_MASK		(0x1f<<0)	/* Geographic Address Mask */

/* VME Exception Status (VEAT) */
#define TSI148_VEAT_VES				(0x1<<31)	/* VMEbus Exception Status */
#define TSI148_VEAT_VEOF			(0x1<<30)	/* VMEbus Exception Overflow */
#define TSI148_VEAT_VESCL			(0x1<<29)	/* VMEbus Exception Status Clear */
#define TSI148_VEAT_2EOT			(0x1<<21)	/* 2e Odd Termination */
#define TSI148_VEAT_2EST			(0x1<<20)	/* 2e Slave Terminated */
#define TSI148_VEAT_BERR			(0x1<<19)	/* VMEbus Error */
#define TSI148_VEAT_LWORD			(0x1<<18)	/* LWORD */
#define TSI148_VEAT_WRITE			(0x1<<17)	/* WRITE */
#define TSI148_VEAT_IACK			(0x1<<16)	/* IACK */
#define TSI148_VEAT_DS1				(0x1<<15)	/* DS1 */
#define TSI148_VEAT_DS0				(0x1<<14)	/* DS0 */
#define TSI148_VEAT_AM_MASK			(0x3F<<8)	/* AM */
#define TSI148_VEAT_XAM_MASK		(0xFF<<0)	/* XAM */

/* Inbound Translation Start and End Address Lower (ITSAU+ITAL/ITEAU+ITEAL) */
#define	TSI148_ITXAX_MASK			((u_int64)0xfffffffffffffff0LL)	/* bits 0 - 3 never compared */
#define	TSI148_ITXAX_MASK_A16		((u_int64)0x000000000000fff0LL)	/* bits 15 - 4 compared for A16 */
#define	TSI148_ITXAX_MASK_A24		((u_int64)0x0000000000fff000LL)	/* bits 23 - 12 compared for A24 */
#define	TSI148_ITXAX_MASK_A32		((u_int64)0x00000000ffff0000LL)	/* bits 31 - 16 compared for A32 */
#define	TSI148_ITXAX_MASK_A64		((u_int64)0xffffffffffff0000LL)	/* bits 63 - 16 compared for A64 */

/* Inbound Translation Offset (ITOFU+ITOFL) */
#define	TSI148_ITOFX_MASK			((u_int64)0xfffffffffffffff0LL)	/* bits 0 - 3 always 0 */

/* Inbound Translation Attribute (ITAT) */
#define	TSI148_ITAT_EN				(0x1<<31)	/* Enable */
#define	TSI148_ITAT_TH				(0x1<<18)	/* Threshold */

#define	TSI148_ITAT_VFS_MASK		(0x3<<16)	/* Virtual FIFO	Size Mask */
#define	TSI148_ITAT_VFS_64			(0x0<<16)	/* 64 Bytes	*/
#define	TSI148_ITAT_VFS_128			(0x1<<16)	/* 128 Bytes */
#define	TSI148_ITAT_VFS_256			(0x2<<16)	/* 256 Bytes */
#define	TSI148_ITAT_VFS_512			(0x3<<16)	/* 512 Bytes */

#define	TSI148_ITAT_2ESSTM_MASK		(0x7<<12)	/* 2eSST Mode */
#define	TSI148_ITAT_2ESSTM_160		(0x0<<12)	/* SST160 */
#define	TSI148_ITAT_2ESSTM_267		(0x1<<12)	/* SST267 */
#define	TSI148_ITAT_2ESSTM_320		(0x2<<12)	/* SST320 */

#define	TSI148_ITAT_2ESSTB			(0x1<<11)	/* 2eSST Broadcast */
#define	TSI148_ITAT_2ESST			(0x1<<10)	/* 2eSST */
#define	TSI148_ITAT_2EVME			(0x1<<9)	/* 2eVME */
#define	TSI148_ITAT_MBLT			(0x1<<8)	/* MBLT	*/
#define	TSI148_ITAT_BLT				(0x1<<7)	/* BLT */

#define	TSI148_ITAT_AS_MASK			(0x7<<4)	/* Address Space Mask */
#define	TSI148_ITAT_AS_A16			(0x0<<4)	/* A16 */
#define	TSI148_ITAT_AS_A24			(0x1<<4)	/* A24 */
#define	TSI148_ITAT_AS_A32			(0x2<<4)	/* A32 */
#define	TSI148_ITAT_AS_A64			(0x4<<4)	/* A64 */

#define	TSI148_ITAT_AC_MASK			(0xf<<0)	/* Access Cycle Type Mask */
#define	TSI148_ITAT_SUPR			(0x1<<3)	/* Supervisor */
#define	TSI148_ITAT_NPRIV			(0x1<<2)	/* Non-privileged */
#define	TSI148_ITAT_PGM				(0x1<<1)	/* Program */
#define	TSI148_ITAT_DATA			(0x1<<0)	/* Data */

/* GCSR Base Address Lower Register (GBAL) */
#define TSI148_GBAL_MASK			(0xffffffe0)	/* Address Mask */

/* GCSR Attribute Register (GCSRAT) */
#define TSI148_GCSRAT_EN			(0x1<<7)	/* Enable */

/* Location Monitor Base Addr Register (LMBA) */
#define TSI148_LMBA_MASK			((u_int64)0xffffffffffffffe0LL)	/* bits 0 - 4 never compared */
#define TSI148_LMX_MASK				(0x3<<3)
#define TSI148_LM0					(0x0<<3)	/* offset 0-7 is LM0 */
#define TSI148_LM1					(0x1<<3)	/* offset 8-f is LM1 */
#define TSI148_LM2					(0x2<<3)	/* offset 10-17 is LM2 */
#define TSI148_LM3					(0x3<<3)	/* offset 18-1f is LM3 */

/* Location Monitor Attribute Register (LMAT) */
#define TSI148_LMAT_EN				(0x1<<7)	/* Enable */
#define TSI148_LMAT_AS_MASK			(0x7<<4)	/* Address Space Mask */
#define TSI148_LMAT_AS_A16			(0x0<<4)	/* Address Space A16 */
#define TSI148_LMAT_AS_A24			(0x1<<4)	/* Address Space A24 */
#define TSI148_LMAT_AS_A32			(0x2<<4)	/* Address Space A32 */
#define TSI148_LMAT_AS_A64			(0x4<<4)	/* Address Space A64 */
#define TSI148_LMAT_SUPR			(0x1<<3)	/* Supervisor */
#define TSI148_LMAT_NPRIV			(0x1<<2)	/* Non-priviledged */
#define TSI148_LMAT_PGM				(0x1<<1)	/* Program */
#define TSI148_LMAT_DATA			(0x1<<0)	/* Data */

/* Interrupt Control (VICR) */
#define TSI148_VICR_CNTS_MASK		(0x3<<30)	/* Counter Source Mask */
#define TSI148_VICR_CNTS_BIT			 (30)	/* Counter Source first Bit number */
#define TSI148_VICR_EDGIS_MASK		(0x3<<28)	/* Edge Interrupt Source */
#define TSI148_VICR_EDGIS_BIT			 (28)	/* Edge Interrupt Source first Bit number */
#define TSI148_VICR_IRQ1F_MASK		(0x3<<26)	/* IRQ1 Function */
#define TSI148_VICR_IRQ1F_BIT			 (26)	/* IRQ1 Function first Bit number */
#define TSI148_VICR_IRQ2F_MASK		(0x3<<24)	/* IRQ2 Function */
#define TSI148_VICR_IRQ2F_BIT			 (24)	/* IRQ2 Function first Bit number */
#define TSI148_VICR_IRQC			(0x1<<15)	/* VMEbus IRQ Clear */
#define TSI148_VICR_IRQLS_MASK		(0x7<<12)	/* VMEbus IRQ Level Status */
#define TSI148_VICR_IRQLS_BIT			 (12)	/* VMEbus IRQ Level Status first Bit number */
#define TSI148_VICR_IRQS			(0x1<<11)	/* VMEbus IRQ Status */
#define TSI148_VICR_IRQL_MASK		(0x7<< 8)	/* VMEbus IRQ Level */
#define TSI148_VICR_IRQL_BIT			 ( 8)	/* VMEbus IRQ Level first Bit number */
#define TSI148_VICR_STID_MASK		(0xff<<0)	/* STATUS/ID */

/* Interrupt Enable (Output) (INTEN/INTEO) */
#define TSI148_INTEX_DMAX_MASK		(0x03<<24)	/* DMA Interrupts Mask */
#define TSI148_INTEX_DMA1			(0x01<<25)	/* DMA1 Interrupt */
#define TSI148_INTEX_DMA0			(0x01<<24)	/* DMA0 Interrupt */
#define TSI148_INTEX_LMX_MASK		(0x0f<<20)	/* Location Monitor Interrupts Mask */
#define TSI148_INTEX_LM3			(0x01<<23)	/* Location Monitor 3 Interrupt */
#define TSI148_INTEX_LM2			(0x01<<22)	/* Location Monitor 2 Interrupt */
#define TSI148_INTEX_LM1			(0x01<<21)	/* Location Monitor 1 Interrupt */
#define TSI148_INTEX_LM0			(0x01<<20)	/* Location Monitor 0 Interrupt */
#define TSI148_INTEX_LM0_BIT			  (20)	/* Location Monitor 0 Interrupt Bit  */
#define TSI148_INTEX_MBX_MASK		(0x0f<<16)	/* Mail Box Interrupts Mask */
#define TSI148_INTEX_MB3			(0x01<<19)	/* Mail Box 3 Interrupt */
#define TSI148_INTEX_MB2			(0x01<<18)	/* Mail Box 2 Interrupt */
#define TSI148_INTEX_MB1			(0x01<<17)	/* Mail Box 1 Interrupt */
#define TSI148_INTEX_MB0			(0x01<<16)	/* Mail Box 0 Interrupt */
#define TSI148_INTEX_MB0_BIT              (16)  /* Mail Box 0 Interrupt Bit */
#define TSI148_INTEX_PERR			(0x01<<13)	/* PCI/X Bus Error Interrupt */
#define TSI148_INTEX_VERR			(0x01<<12)	/* VMEbus Error Interrupt */
#define TSI148_INTEX_VIE			(0x01<<11)	/* VMEbus IRQ Edge Interrupt */
#define TSI148_INTEX_IACK			(0x01<<10)	/* Interrupt Acknowledge Interrupt */
#define TSI148_INTEX_SYSFL			(0x01<<9)	/* System Fail Interrupt */
#define TSI148_INTEX_ACFL			(0x01<<8)	/* AC Fail Interrupt */
#define TSI148_INTEX_IRQX_MASK		(0x7f<<1)	/* IRQx Interrupt Mask */
#define TSI148_INTEX_IRQ7			(0x01<<7)	/* IRQ7 Interrupt */
#define TSI148_INTEX_IRQ6			(0x01<<6)	/* IRQ6 Interrupt */
#define TSI148_INTEX_IRQ5			(0x01<<5)	/* IRQ5 Interrupt */
#define TSI148_INTEX_IRQ4			(0x01<<4)	/* IRQ4 Interrupt */
#define TSI148_INTEX_IRQ3			(0x01<<3)	/* IRQ3 Interrupt */
#define TSI148_INTEX_IRQ2			(0x01<<2)	/* IRQ2 Interrupt */
#define TSI148_INTEX_IRQ1			(0x01<<1)	/* IRQ1 Interrupt */

/* Error Diagnostic PCI/X Attributes (EDPAT) */
#define TSI148_EDPAT_EDPCL			(0x1<<29)	/* Error Diagnostic PCI/X Clear */

/* GCSR Register bit definitions */

/* Control and Status GCTRL */
#define TSI148_GCTRL_LRST			(0x1<<15)	/* Local Reset */
#define TSI148_GCTRL_SFAILEN		(0x1<<14)	/* System Fail Enable */
#define TSI148_GCTRL_BDFAILS		(0x1<<13)	/* Board Fail Status */
#define TSI148_GCTRL_SCONS			(0x1<<12)	/* System Controller Status */
#define TSI148_GCTRL_MEN			(0x1<<11)	/* Module Enable */
#define TSI148_GCTRL_LMI3S			(0x1<<7)	/* Location Monitor 3 Interrupt Status */
#define TSI148_GCTRL_LMI2S			(0x1<<6)	/* Location Monitor 2 Interrupt Status */
#define TSI148_GCTRL_LMI1S			(0x1<<5)	/* Location Monitor 1 Interrupt Status */
#define TSI148_GCTRL_LMI0S			(0x1<<4)	/* Location Monitor 0 Interrupt Status */
#define TSI148_GCTRL_MB3EN			(0x1<<3)	/* Mail Box 3 Interrupt Status */
#define TSI148_GCTRL_MB2EN			(0x1<<2)	/* Mail Box 2 Interrupt Status */
#define TSI148_GCTRL_MB1EN			(0x1<<1)	/* Mail Box 1 Interrupt Status */
#define TSI148_GCTRL_MB0EN			(0x1<<0)	/* Mail Box 0 Interrupt Status */


/* DMA Register bit definitions */

/* DMA Control  DCTL */
#define TSI148_DCTL_ABT 			(0x1<<27)	/* Abort */
#define TSI148_DCTL_PAU 			(0x1<<26)	/* Pause */
#define TSI148_DCTL_DGO 			(0x1<<25)	/* DMA Go */
#define TSI148_DCTL_MOD 			(0x1<<23)	/* Mode (1=direct, 0=linked */
#define TSI148_DCTL_VFAR			(0x1<<17)	/* VMEbus Flush on Aborted Read */
#define TSI148_DCTL_PFAR			(0x1<<16)	/* PCI/X  Flush on Aborted Read */
#define TSI148_DCTL_VBKS_BIT		(12)		/* VMEbus Block Size Bit begin */
#define TSI148_DCTL_VBKS_MASK		(0x7<<TSI148_DCTL_VBKS_BIT)	/* VMEbus Block Size */
#define TSI148_DCTL_VBKS_4096		(0x7<<TSI148_DCTL_VBKS_BIT)	/* VMEbus Block Size 4096 bytes */
#define TSI148_DCTL_VBOT_BIT		(8)			/* VMEbus Back-off Timer Bit begin */
#define TSI148_DCTL_VBOT_MASK		(0x7<<TSI148_DCTL_VBOT_BIT)	/* VMEbus Back-off Timer */
#define TSI148_DCTL_PBKS_BIT		(4)			/* PCI/X Block Size Bit begin */
#define TSI148_DCTL_PBKS_MASK		(0x7<<TSI148_DCTL_PBKS_BIT)	/* PCI/X Block Size */
#define TSI148_DCTL_PBKS_4096		(0x7<<TSI148_DCTL_PBKS_BIT)	/* PCI/X Block Size 4096 bytes */
#define TSI148_DCTL_PBOT_BIT		(0)			/* PCI/X Back-off Timer Bit begin */
#define TSI148_DCTL_PBOT_MASK		(0x7<<TSI148_DCTL_PBKS_BIT)	/* PCI/X Back-off Timer */

/* DMA Status  DSTA */
#define TSI148_DSTA_ERR				(0x1<<28)	/* Error */
#define TSI148_DSTA_ABT 			(0x1<<27)	/* Abort */
#define TSI148_DSTA_PAU 			(0x1<<26)	/* Pause */
#define TSI148_DSTA_DON				(0x1<<25)	/* Done */
#define TSI148_DSTA_BSY				(0x1<<24)	/* Busy */
#define TSI148_DSTA_ERRS			(0x1<<20)	/* Error Source */
#define TSI148_DSTA_ERT_MASK		(0x3<<16)	/* Error Type */

/* DMA Source and Destination Attribute Registers */
	/* DMA type,  *_PATTERN_* only applies to src descriptors */
#define TSI148_DXAT_TYP_BIT0		(24)		/* Type start bit */
#define TSI148_DXAT_TYP_MASK			(0x33<<TSI148_DXAT_TYP_BIT0)	/* Type mask */
#define TSI148_DXAT_TYP_PCIX			(0x00<<TSI148_DXAT_TYP_BIT0)	/* PCI/X bus */
#define TSI148_DXAT_TYP_VME 			(0x10<<TSI148_DXAT_TYP_BIT0)	/* VME bus */
#define TSI148_DXAT_TYP_PATTERN_SZ32_I	(0x20<<TSI148_DXAT_TYP_BIT0)	/* PATTERN, pattern size = 32bit, incremented */
#define TSI148_DXAT_TYP_PATTERN_SZ32_NI	(0x21<<TSI148_DXAT_TYP_BIT0)	/* PATTERN, pattern size = 32bit, not incremented */
#define TSI148_DXAT_TYP_PATTERN_SZ8_I	(0x22<<TSI148_DXAT_TYP_BIT0)	/* PATTERN, pattern size = 8bit, incremented */
#define TSI148_DXAT_TYP_PATTERN_SZ8_NI	(0x23<<TSI148_DXAT_TYP_BIT0)	/* PATTERN, pattern size = 8bit, not incremented */
	/* VMEbus 2ESST Transfer Rate */
#define TSI148_DXAT_SSTM_BIT0		(11)		/* 2ESST mode start bit */
#define TSI148_DXAT_SSTM_MASK		(0x3<<TSI148_DXAT_SSTM_BIT0)	/* 2ESST mode mask */
#define TSI148_DXAT_SSTM_2ESST160	(0x0<<TSI148_DXAT_SSTM_BIT0)	/* 160 MB/s */
#define TSI148_DXAT_SSTM_2ESST267	(0x1<<TSI148_DXAT_SSTM_BIT0)	/* 267 MB/s */
#define TSI148_DXAT_SSTM_2ESST320	(0x2<<TSI148_DXAT_SSTM_BIT0)	/* 320 MB/s */
								  /* 0x3 - 0x7  reserved */
	/* VMEbus Transfer Mode */
#define TSI148_DXAT_TM_BIT0			(8)			/* Transfer mode start bit */
#define TSI148_DXAT_TM_MASK			(0x7<<TSI148_DXAT_TM_BIT0)	/* Transfer mode mask */
#define TSI148_DXAT_TM_SCT			(0x0<<TSI148_DXAT_TM_BIT0)	/* SCT */
#define TSI148_DXAT_TM_BLT			(0x1<<TSI148_DXAT_TM_BIT0)	/* BLT */
#define TSI148_DXAT_TM_MBLT			(0x2<<TSI148_DXAT_TM_BIT0)	/* MBLT */
#define TSI148_DXAT_TM_2EVME		(0x3<<TSI148_DXAT_TM_BIT0)	/* 2EVME */
#define TSI148_DXAT_TM_2ESST		(0x4<<TSI148_DXAT_TM_BIT0)	/* 2ESST */
#define TSI148_DXAT_TM_2ESSTB		(0x5<<TSI148_DXAT_TM_BIT0)	/* 2ESST Broadcast */
						          /* 0x6, 0x7   reserved */
	/* VMEbus max Data Width
	 * applies to SCT and BLT transfers only, MBLT, 2eVME and 2eSST transfers are always 64-bit */
#define TSI148_DXAT_DBW_BIT0		(6)			/* VMEbus Data Bus Width bit */
#define TSI148_DXAT_DBW_MASK		(0x3<<TSI148_DXAT_DBW_BIT0)	/* VMEbus Data Bus Width mask */
#define TSI148_DXAT_DBW_16			(0x0<<TSI148_DXAT_DBW_BIT0)
#define TSI148_DXAT_DBW_32			(0x1<<TSI148_DXAT_DBW_BIT0)
								  /* 0x2 - 0x3  reserved */
#define TSI148_DXAT_SUP				(0x1<<5)			/* VMEbus Supervisory Mode */
#define TSI148_DXAT_PGM				(0x1<<4)			/* VMEbus Program Mode */
/* VMEbus Address Mode */
#define TSI148_DXAT_AM_BIT0			(0)			/* VMEbus Address Mode */
#define TSI148_DXAT_AM_MASK			(0xf<<TSI148_DXAT_AM_BIT0)	/* VMEbus Address Mode */
#define TSI148_DXAT_AM_16			(0x0<<TSI148_DXAT_AM_BIT0)
#define TSI148_DXAT_AM_24			(0x1<<TSI148_DXAT_AM_BIT0)
#define TSI148_DXAT_AM_32			(0x2<<TSI148_DXAT_AM_BIT0)
 								  /* 0x3 reserved */
#define TSI148_DXAT_AM_64			(0x4<<TSI148_DXAT_AM_BIT0)
#define TSI148_DXAT_AM_CRCSR		(0x5<<TSI148_DXAT_AM_BIT0)
 								  /* 0x6, 0x7 reserved */
#define TSI148_DXAT_AM_UM1			(0x8<<TSI148_DXAT_AM_BIT0)
#define TSI148_DXAT_AM_UM2			(0x9<<TSI148_DXAT_AM_BIT0)
#define TSI148_DXAT_AM_UM3			(0xa<<TSI148_DXAT_AM_BIT0)
#define TSI148_DXAT_AM_UM4			(0xb<<TSI148_DXAT_AM_BIT0)
								  /* 0xc - 0xf reserved */

/* DMA Next Link Address Lower */
#define TSI148_DNLAL_LLA			(0x01)	/* Last Linked Address */

/* DMA linked list descriptor */
/* the format of this descriptor has to be kept stable,
 * it is set through the TSI148 hardware needs			*/
typedef struct _TSI148_DMA_LL_DESC
{
	u_int32		dsau;		/* DMA Source Address Upper */
	u_int32		dsal;		/* DMA Source Address Lower */
	u_int32		ddau;		/* DMA Destination Address Upper */
	u_int32		ddal;		/* DMA Destination Address Lower */
	u_int32		dsat;		/* DMA Source Attribute */
	u_int32		ddat;		/* DMA Destination Attribute */
	u_int32		dnlau;		/* DMA Next Link Address Upper */
	u_int32		dnlal;		/* DMA Next Link Address Lower */
	u_int32		dcnt;		/* DMA Count */
	u_int32		ddbs;		/* DMA Destination Broadcast Select */
} TSI148_DMA_LL_DESC;

#ifdef __cplusplus
	}
#endif

#endif /* _TSI148_H	*/



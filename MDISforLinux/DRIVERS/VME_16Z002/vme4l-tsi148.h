/*********************	P r	o g	r a	m  -  M	o d	u l	e ***********************/
/*!
 *		  \file	 vme4l-tsi148.h
 *
 *		\author	 Ralf Trübenbach
 *
 *		  \brief Header File for Tundra TSI148 VME bridge driver (A17/A19/
 *               A20...). Can be used to adjust timing and in-/outbound
 *               windows.
 *
 *	   Switches: -
 *	   Requires: - tsi148.h
 *---------------------------------------------------------------------------
 * Copyright 2008-2019, MEN Mikro Elektronik GmbH
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
#ifndef	_VME4L_TSI148_H
#define	_VME4L_TSI148_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*------------------------+
 | TIMING/ARBITRATION     |
 +------------------------*/

/** Init value for VME Master Control Register */
#define TSI148_VMCTRL	( TSI148_VMCTRL_VTON_512US | TSI148_VMCTRL_VREL_RWD \
						| TSI148_VMCTRL_FAIR | TSI148_VMCTRL_VREQL_3 )
						
/** Init value for VMEbus Control Register */
#define TSI148_VCTRL	( TSI148_VCTRL_GTO_2048US )

/** Init value for DMA Control Register (incl. block size & back-off timer) */
#define TSI148_BLT_DCTL	( TSI148_DCTL_VBKS_4096 | TSI148_DCTL_PBKS_4096 )	

/** Init value for VMEbus Filter Register
    NOTE: [VME4] VMEbus Write During Wrap-around RMW work-around is not
    implemented due to the lower performance. If you really need to use
    wrap-around RMW cycles you have to set VMEFL[ACKD] to slow (0) */
#define TSI148_VMEFL	0x02000f00

/** Init value for PCI-X Capabilities Register */
#define TSI148_PCIXCAP	0x007c0007


/*------------------------+
 | OUTBOUND WINDOWS       |
 +------------------------*/

/**********************************************************************/
/** \defgroup TSI148_MA16D16 Settings for VME4L_SPC_A16_D16 space
 * Used to set-up outbound (master) window for #VME4L_SPC_A16_D16
 * (/dev/vme4l_a16d16) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA16D16_END		(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MA16D16_MINSIZE	(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MA16D16_OTAT		( TSI148_OTAT_EN | TSI148_OTAT_MRPFD \
								| TSI148_OTAT_PFS_8 | TSI148_OTAT_TM_SCT \
								| TSI148_OTAT_DBW_16 | TSI148_OTAT_AMODE_A16 )
/** Translation 2eSST Broadcast Select */
#define TSI148_MA16D16_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA16D32 Settings for VME4L_SPC_A16_D32 space
 * Used to set-up outbound (master) window for #VME4L_SPC_A16_D32
 * (/dev/vme4l_a16d32) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA16D32_END		(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MA16D32_MINSIZE	(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MA16D32_OTAT		( TSI148_OTAT_EN \
								| TSI148_OTAT_MRPFD | TSI148_OTAT_PFS_8 \
								| TSI148_OTAT_TM_SCT \
								| TSI148_OTAT_DBW_32 | TSI148_OTAT_AMODE_A16 )
/** Translation 2eSST Broadcast Select */
#define TSI148_MA16D32_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA24D16 Settings for VME4L_SPC_A24_D16 space
 * Used to set-up outbound (master) window for #VME4L_SPC_A24_D16
 * (/dev/vme4l_a24d16) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA24D16_END		(0x00FFFFFF) /* 16 MB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MA24D16_MINSIZE	(0x01000000) /* 16 MB */
/** Translation Attribute */
#define TSI148_MA24D16_OTAT		( TSI148_OTAT_EN \
								| TSI148_OTAT_MRPFD | TSI148_OTAT_PFS_8 \
								| TSI148_OTAT_TM_SCT \
								| TSI148_OTAT_DBW_16 | TSI148_OTAT_AMODE_A24 )
/** Translation 2eSST Broadcast Select */
#define TSI148_MA24D16_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA24D32 Settings for VME4L_SPC_A24_D32 space
 * Used to set-up outbound (master) window for #VME4L_SPC_A24_D32
 * (/dev/vme4l_a24d32) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA24D32_END		(0x00FFFFFF) /* 16 MB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MA24D32_MINSIZE	(0x01000000) /* 16 MB */
/** Translation Attribute */
#define TSI148_MA24D32_OTAT		( TSI148_OTAT_EN \
								| TSI148_OTAT_MRPFD | TSI148_OTAT_PFS_8 \
								| TSI148_OTAT_TM_SCT \
								| TSI148_OTAT_DBW_32 | TSI148_OTAT_AMODE_A24 )
/** Translation 2eSST Broadcast Select */
#define TSI148_MA24D32_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA32D32 Settings for VME4L_SPC_A32_D32 space
 * Used to set-up outbound (master) window for #VME4L_SPC_A32_D32
 * (/dev/vme4l_a32d32) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA32D32_END		(0xFFFFFFFF) /* 4 GB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MA32D32_MINSIZE	(0x08000000) /* 128 MB */
/** Translation Attribute */
#define TSI148_MA32D32_OTAT		( TSI148_OTAT_EN \
								| TSI148_OTAT_MRPFD | TSI148_OTAT_PFS_8 \
								| TSI148_OTAT_TM_SCT \
								| TSI148_OTAT_DBW_32 | TSI148_OTAT_AMODE_A32 )
/** Translation 2eSST Broadcast Select */
#define TSI148_MA32D32_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA64D32 Settings for VME4L_SPC_A64_D32 space
 * Used to set-up outbound (master) window for #VME4L_SPC_A64_D32
 * (/dev/vme4l_a64d32) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA64D32_END		(0xFFFFFFFFFFFFFFFFULL) /* 16 EB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MA64D32_MINSIZE	(0x08000000) /* 128 MB */
/** Translation Attribute */
#define TSI148_MA64D32_OTAT		( TSI148_OTAT_EN \
								| TSI148_OTAT_MRPFD | TSI148_OTAT_PFS_8 \
								| TSI148_OTAT_TM_SCT \
								| TSI148_OTAT_DBW_32 | TSI148_OTAT_AMODE_A64 )
/** Translation 2eSST Broadcast Select */
#define TSI148_MA64D32_OTBS		(0)

/*! @} */


/*---------------------------------+
 | OUTBOUND WINDOWS (user defined) |
 +---------------------------------*/

/**********************************************************************/
/** \defgroup TSI148_MST0 Settings for user defined VME4L_SPC_MST0 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST0
 * (/dev/vme4l_master0) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST0_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST0_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST0_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST0_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MST1 Settings for user defined VME4L_SPC_MST1 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST1
 * (/dev/vme4l_master1) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST1_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST1_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST1_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST1_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MST2 Settings for user defined VME4L_SPC_MST2 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST2
 * (/dev/vme4l_master2) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST2_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST2_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST2_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST2_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MST3 Settings for user defined VME4L_SPC_MST3 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST3
 * (/dev/vme4l_master3) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST3_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST3_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST3_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST3_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MST4 Settings for user defined VME4L_SPC_MST4 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST4
 * (/dev/vme4l_master4) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST4_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST4_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST4_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST4_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MST5 Settings for user defined VME4L_SPC_MST5 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST5
 * (/dev/vme4l_master5) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST5_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST5_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST5_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST5_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MST6 Settings for user defined VME4L_SPC_MST6 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST6
 * (/dev/vme4l_master6) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST6_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST6_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST6_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST6_OTBS		(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MST7 Settings for user defined VME4L_SPC_MST7 space
 * Used to set-up outbound (master) window for #VME4L_SPC_MST7
 * (/dev/vme4l_master7) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MST7_END			(0x0000FFFF) /* 64 kB */
/** Minimal Window Size (>= TSI148 minimal window size!) */
#define	TSI148_MST7_MINSIZE		(0x00010000) /* 64 kB */
/** Translation Attribute */
#define TSI148_MST7_OTAT		(0)	/* not used */
/** Translation 2eSST Broadcast Select */
#define TSI148_MST7_OTBS		(0)

/*! @} */


/*------------------------+
 |  INBOUND WINDOWS       |
 +------------------------*/

/**********************************************************************/
/** \defgroup TSI148_SLV0 Settings for VME4L_SPC_SLV0 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV0
 * (/dev/vme4l_slave0) VME address space. Default set-up is
 * A16 TSI148 register access (CRG) for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV0_END			(0x0000FFFF) /* 64 kB */
/** Translation Attribute */
#define TSI148_SLV0_ITAT		( TSI148_ITAT_EN \
								| TSI148_ITAT_TH | TSI148_ITAT_VFS_512 \
								| TSI148_ITAT_2ESSTM_320 | TSI148_ITAT_2ESST \
								| TSI148_ITAT_2EVME | TSI148_ITAT_MBLT \
								| TSI148_ITAT_BLT | TSI148_ITAT_AS_A16 \
								| TSI148_ITAT_SUPR | TSI148_ITAT_NPRIV \
								| TSI148_ITAT_PGM | TSI148_ITAT_DATA )
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV0_TARGET  	TSI148_SLVX_REGS

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_SLV1 Settings for VME4L_SPC_SLV1 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV1
 * (/dev/vme4l_slave1) VME address space. No default set-up is
 * done for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV1_END			(0x0000FFFF) /* 64 kB */
/** Translation Attribute */
#define TSI148_SLV1_ITAT		(0) /* not used */
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV1_TARGET  	TSI148_SLVX_UNSHAREDKERNMEM

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_SLV2 Settings for VME4L_SPC_SLV2 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV2
 * (/dev/vme4l_slave2) VME address space. No default set-up is
 * done for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV2_END			(0x0000FFFF) /* 64 kB */
/** Translation Attribute */
#define TSI148_SLV2_ITAT		(0) /* not used */
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV2_TARGET  	TSI148_SLVX_UNSHAREDKERNMEM

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_SLV3 Settings for VME4L_SPC_SLV3 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV3
 * (/dev/vme4l_slave3) VME address space. Default set-up is
 * A24 shared kernel memory for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV3_END			(0x00FFFFFF) /* 16 MB */
/** Translation Attribute */
#define TSI148_SLV3_ITAT		( TSI148_ITAT_EN \
								| TSI148_ITAT_TH | TSI148_ITAT_VFS_512 \
								| TSI148_ITAT_2ESSTM_320 | TSI148_ITAT_2ESST \
								| TSI148_ITAT_2EVME | TSI148_ITAT_MBLT \
								| TSI148_ITAT_BLT | TSI148_ITAT_AS_A24 \
								| TSI148_ITAT_SUPR | TSI148_ITAT_NPRIV \
								| TSI148_ITAT_PGM | TSI148_ITAT_DATA )
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV3_TARGET  	TSI148_SLVX_UNSHAREDKERNMEM

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_SLV4 Settings for VME4L_SPC_SLV4 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV4
 * (/dev/vme4l_slave4) VME address space. Default set-up is
 * A32 shared kernel memory for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV4_END			(0xFFFFFFFF) /* 4 GB */
/** Translation Attribute */
#define TSI148_SLV4_ITAT		( TSI148_ITAT_EN \
								| TSI148_ITAT_TH | TSI148_ITAT_VFS_512 \
								| TSI148_ITAT_2ESSTM_320 | TSI148_ITAT_2ESST \
								| TSI148_ITAT_2EVME | TSI148_ITAT_MBLT \
								| TSI148_ITAT_BLT | TSI148_ITAT_AS_A32 \
								| TSI148_ITAT_SUPR | TSI148_ITAT_NPRIV \
								| TSI148_ITAT_PGM | TSI148_ITAT_DATA )
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV4_TARGET  	TSI148_SLVX_UNSHAREDKERNMEM

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_SLV5 Settings for VME4L_SPC_SLV5 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV5
 * (/dev/vme4l_slave5) VME address space. Default set-up is
 * A64 shared kernel memory for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV5_END			(0xFFFFFFFFFFFFFFFFULL) /* 16 EB */
/** Translation Attribute */
#define TSI148_SLV5_ITAT		( TSI148_ITAT_EN \
								| TSI148_ITAT_TH | TSI148_ITAT_VFS_512 \
								| TSI148_ITAT_2ESSTM_320 | TSI148_ITAT_2ESST \
								| TSI148_ITAT_2EVME | TSI148_ITAT_MBLT \
								| TSI148_ITAT_BLT | TSI148_ITAT_AS_A64 \
								| TSI148_ITAT_SUPR | TSI148_ITAT_NPRIV \
								| TSI148_ITAT_PGM | TSI148_ITAT_DATA )
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV5_TARGET  	TSI148_SLVX_UNSHAREDKERNMEM

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_SLV6 Settings for VME4L_SPC_SLV6 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV6
 * (/dev/vme4l_slave6) VME address space. No default set-up is
 * done for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV6_END			(0x0000FFFF) /* 64 kB */
/** Translation Attribute */
#define TSI148_SLV6_ITAT		(0) /* not used */
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV6_TARGET  	TSI148_SLVX_UNSHAREDKERNMEM

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_SLV7 Settings for VME4L_SPC_SLV7 space
 * Used to set-up inbound (slave) window for #VME4L_SPC_SLV7
 * (/dev/vme4l_slave7) VME address space. No default set-up is
 * done for this window.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_SLV7_END			(0x0000FFFF) /* 64 kB */
/** Translation Attribute */
#define TSI148_SLV7_ITAT		(0) /* not used */
/** Target Memory (kernel memory, TSI148 regs or another PCI device) */
#define TSI148_SLV7_TARGET  	TSI148_SLVX_UNSHAREDKERNMEM

/*! @} */


/*------------------------+
 |   BLT (DMA)            |
 +------------------------*/

/**********************************************************************/
/** \defgroup TSI148_MA24D16BLT Settings for VME4L_SPC_A24_D16_BLT space
 * Used to set-up DMA transfer for #VME4L_SPC_A24_D16_BLT
 * (/dev/vme4l_a24d16_blt) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA24D16BLT_END	(0x00FFFFFF) /* 16 MB */
/** Translation Attribute */
#define TSI148_MA24D16BLT_DXAT	( TSI148_DXAT_TYP_VME \
								| TSI148_DXAT_TM_BLT \
								| TSI148_DXAT_DBW_16 | TSI148_DXAT_AM_24 )
/** 2eSST Broadcast Select */
#define TSI148_MA24D16BLT_DXBS	(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA24D32BLT Settings for VME4L_SPC_A24_D32_BLT space
 * Used to set-up DMA transfer for #VME4L_SPC_A24_D32_BLT
 * (/dev/vme4l_a24d32_blt) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA24D32BLT_END	(0x00FFFFFF) /* 16 MB */
/** Translation Attribute */
#define TSI148_MA24D32BLT_DXAT	( TSI148_DXAT_TYP_VME \
								| TSI148_DXAT_TM_BLT \
								| TSI148_DXAT_DBW_32 | TSI148_DXAT_AM_24 )
/** 2eSST Broadcast Select */
#define TSI148_MA24D32BLT_DXBS	(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA32D32BLT Settings for VME4L_SPC_A32_D32_BLT space
 * Used to set-up DMA transfer for #VME4L_SPC_A32_D32_BLT
 * (/dev/vme4l_a32d32_blt) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA32D32BLT_END	(0xFFFFFFFF) /* 4 GB */
/** Translation Attribute */
#define TSI148_MA32D32BLT_DXAT	( TSI148_DXAT_TYP_VME \
								| TSI148_DXAT_TM_BLT \
								| TSI148_DXAT_DBW_32 | TSI148_DXAT_AM_32 )
/** 2eSST Broadcast Select */
#define TSI148_MA32D32BLT_DXBS	(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA32D64BLT Settings for VME4L_SPC_A32_D64_BLT (MBLT) space
 * Used to set-up DMA transfer for #VME4L_SPC_A32_D64_BLT
 * (/dev/vme4l_a32d64_blt) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA32D64BLT_END	(0xFFFFFFFF) /* 4 GB */
/** Translation Attribute */
#define TSI148_MA32D64BLT_DXAT	( TSI148_DXAT_TYP_VME \
								| TSI148_DXAT_TM_MBLT \
								| TSI148_DXAT_DBW_32 | TSI148_DXAT_AM_32 )
/** 2eSST Broadcast Select */
#define TSI148_MA32D64BLT_DXBS	(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA642EVME Settings for VME4L_SPC_A64_2EVME space
 * Used to set-up DMA transfer for #VME4L_SPC_A64_2EVME
 * (/dev/vme4l_a64_2evme) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA642EVME_END	(0xFFFFFFFFFFFFFFFFULL) /* 16 EB */
/** Translation Attribute */
#define TSI148_MA642EVME_DXAT	( TSI148_DXAT_TYP_VME \
								| TSI148_DXAT_TM_2EVME \
								| TSI148_DXAT_DBW_32 | TSI148_DXAT_AM_64 )
/** 2eSST Broadcast Select */
#define TSI148_MA642EVME_DXBS	(0)

/*! @} */

/**********************************************************************/
/** \defgroup TSI148_MA642ESST Settings for VME4L_SPC_A64_2ESST space
 * Used to set-up DMA transfer for #VME4L_SPC_A64_2ESST
 * (/dev/vme4l_a64_2esst) VME address space.
 *  @{
 */

/** End of Addr Space */
#define	TSI148_MA642ESST_END	(0xFFFFFFFFFFFFFFFFULL) /* 16 EB */
/** Translation Attribute */
#define TSI148_MA642ESST_DXAT	( TSI148_DXAT_TYP_VME \
								| TSI148_DXAT_TM_2ESST \
								| TSI148_DXAT_SSTM_2ESST320 \
								| TSI148_DXAT_DBW_32 | TSI148_DXAT_AM_64 )
/** Translation 2eSST Broadcast Select */
#define TSI148_MA642ESST_DXBS	(0)

/*! @} */


/*------------------------+
 |   INTERRUPT PRIORITY   |
 +------------------------*/
/** Table that maintains priority of VME interrupts
 *  (starting with highest priority)
 */
static const uint32_t G_IrqPriority[] = {
	TSI148_INTEX_ACFL,
	TSI148_INTEX_SYSFL,
	TSI148_INTEX_VERR,
	TSI148_INTEX_PERR,
	TSI148_INTEX_DMA1,
	TSI148_INTEX_DMA0,
	TSI148_INTEX_LM3,
	TSI148_INTEX_LM2,
	TSI148_INTEX_LM1,
	TSI148_INTEX_LM0,
	TSI148_INTEX_MB3,
	TSI148_INTEX_MB2,
	TSI148_INTEX_MB1,
	TSI148_INTEX_MB0,
	TSI148_INTEX_IRQ7,
	TSI148_INTEX_IRQ6,
	TSI148_INTEX_IRQ5,
	TSI148_INTEX_IRQ4,
	TSI148_INTEX_IRQ3,
	TSI148_INTEX_IRQ2,
	TSI148_INTEX_IRQ1,
	TSI148_INTEX_VIE,
	TSI148_INTEX_IACK
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VME4L_TSI148_H */

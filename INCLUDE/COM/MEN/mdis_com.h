/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mdis_com.h
 *
 *       Author: ds
 *        $Date: 2013/11/28 17:01:31 $
 *    $Revision: 1.10 $
 *
 *  Description: MDIS common driver definitions
 *
 *     Switches:
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mdis_com.h,v $
 * Revision 1.10  2013/11/28 17:01:31  ts
 * R: MDIS projects with F50P didnt work on VxWorks versions without multiple PCI domains
 * M: added define to test for VxWorks with PCI domain support
 *
 * Revision 1.9  2009/11/12 16:52:15  CRuff
 * R: avoid that change of MK is needed for every new address mode
 * M: defined flag MDIS_MA_FLAG_NOMAP for address modes that do not need
 *    address mapping
 *
 * Revision 1.8  2009/11/12 09:39:03  CRuff
 * R: address mode MDIS_MA_BB_INFO_PTR is not part of chameleon address modes
 * M: change grouping and value of MDIS_MA_BB_INFO_PTR
 *
 * Revision 1.7  2009/11/12 09:15:44  CRuff
 * R: new address mode MDIS_MA_CHAMELEON_U can be used for passing information
 *    from the BBIS to the LL Driver in general
 * M: rename the address mode MDIS_MA_CHAMELEON_U to MDIS_MA_BB_INFO_PTR
 *
 * Revision 1.6  2009/10/14 17:15:35  CRuff
 * R: 1. new address mode needed for accessing the chameleonV2 unit
 *    2. cosmetics
 * M: 1. added adress mode define MDIS_MA_CHAMELEON_U
 *    2. added new data mode define MDIS_MD_CHAM_MAX which contains the maximum
 *       data mode value
 *
 * Revision 1.5  2006/03/01 12:12:11  cs
 * added
 *     + defines for CHAMELEON address and data modes
 *
 * Revision 1.4  1999/07/23 09:50:47  Schmidt
 * MDIS_MA_NONE, MDIS_MA_PCICFG, MDIS_MD_NONE added
 *
 * Revision 1.3  1998/06/02 11:00:04  see
 * MDIS_MAX_MSPACE added
 *
 * Revision 1.2  1998/03/12 14:23:48  see
 * missing extern C definition added
 * missing comments added
 *
 * missing extern C definition added
 * missing comments added
 *
 * Revision 1.1  1998/02/25 17:50:53  Schmidt
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _MDIS_COM_H
#define _MDIS_COM_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
/* m-module address modes */
#define MDIS_MA_NONE        0x0000
#define MDIS_MA08           0x0001
#define MDIS_MA24           0x0002
#define MDIS_MA_PCICFG      0x0100

/* chameleon address mode */
#define MDIS_MA_CHAMELEON   0x0200

/* special bbis info address mode */
#define MDIS_MA_BB_INFO_PTR 0x0400

/* flag to distinguish address modes which need
   physical to virtual address mapping and which do not */
#define MDIS_MA_FLAG_NOMAP	0x0400

/* m-module data access modes */
#define MDIS_MD_NONE        0x0000
#define MDIS_MD08           0x0004
#define MDIS_MD16           0x0008
#define MDIS_MD32           0x0010

/* chameleon address space index */
#define MDIS_MD_CHAM_0      0x0000
#define MDIS_MD_CHAM_1      0x0001
#define MDIS_MD_CHAM_2      0x0002
#define MDIS_MD_CHAM_3      0x0003
#define MDIS_MD_CHAM_4      0x0004
#define MDIS_MD_CHAM_5      0x0005
#define MDIS_MD_CHAM_6      0x0006
#define MDIS_MD_CHAM_7      0x0007
#define MDIS_MD_CHAM_MAX    MDIS_MD_CHAM_7

/* max address spaces */
#define MDIS_MAX_MSPACE     8

/* ident call table size */
#define MDIS_MAX_IDCALLS 	16

#if defined(VXWORKS) &&  (_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR >=6)
# define VXW_PCI_DOMAIN_SUPPORT
#endif


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/* ident call data struct */
typedef struct {
    char* (*identCall) (void);
} ID_CALL;

/* ident call table struct */
typedef struct {
    ID_CALL idCall[MDIS_MAX_IDCALLS];
} MDIS_IDENT_FUNCT_TBL;

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
/* none */

#ifdef __cplusplus
   }
#endif

#endif  /* _MDIS_COM_H  */



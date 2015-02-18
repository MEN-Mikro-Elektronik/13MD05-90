/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: modcom.c
 *      Project: common lib for m-modules
 *
 *       Author: kp
 *        $Date: 2008/09/16 18:41:21 $
 *    $Revision: 1.8 $
 *
 *  Description: Handling Module-Identification (EEPROM)
 *
 *     Required: -
 *     Switches: ID_SW - swapped access
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 * int m_mread(addr,buff)            multiple read i=0..15
 * int m_mwrite(addr,buff)           multiple write i=0..15
 * int m_read(addr,index)            single read i
 * int m_write(addr,index,data)      single write i
 * int usm_mread(addr,buff)          multiple read i=0..128
 * int usm_mwrite(addr,buff)         multiple write i=0..128
 * int usm_read(addr,index)          single read i
 * int usm_write(addr,index,data)    single write i
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: modcom.h,v $
 * Revision 1.8  2008/09/16 18:41:21  CKauntz
 * R: base address is only 32 bit long and not compatible to 64 bit
 * M: Changed base address to U_INT32_OR_64
 *
 * Revision 1.7  2007/08/09 09:16:02  CKauntz
 * added usm functions for USM EEPROM read/write
 *
 * Revision 1.6  2003/04/03 14:42:35  Rlange
 * fixed
 *   - missing ID_SW_m_getmodinfo
 *
 * Revision 1.5  2003/01/13 11:16:43  dschmidt
 * added: define MODCOM_MOD_XXX, prototype m_getmodinfo()
 *
 * Revision 1.4  2001/01/16 11:07:40  Schmidt
 * Bug fix - m_read declaration restored
 *
 * Revision 1.3  2000/12/13 12:40:58  ww
 * cosmetics
 *
 * Revision 1.2  1999/05/28 15:01:37  Schmidt
 * supports variant id_sw
 *
 * Revision 1.1  1998/02/23 11:29:57  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1993 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#if !defined(_MODCOM_H)
#  define _MODCOM_H

#ifdef __cplusplus
   extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
/* swapped access */
#ifdef ID_SW
#	define m_mread	ID_SW_m_mread
#	define m_mwrite	ID_SW_m_mwrite
#	define m_write	ID_SW_m_write
#	define m_read	ID_SW_m_read
#	define m_getmodinfo ID_SW_usm_getmodinfo
#	define usm_mread	ID_SW_usm_mread
#	define usm_mwrite	ID_SW_usm_mwrite
#	define usm_write	ID_SW_usm_write
#	define usm_read		ID_SW_usm_read
#endif

/* for m_getmodinfo */
#define MODCOM_MOD_MEN		1
#define MODCOM_MOD_THIRD	2

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
int m_mread( u_int8 *addr, u_int16 *buff );
int m_mwrite( u_int8 *addr, u_int16 *buff );
int m_write( u_int8 *addr, u_int8 index, u_int16 data );
int m_read( U_INT32_OR_64 base, u_int8 index );
int m_getmodinfo(
	U_INT32_OR_64 base,
	u_int32 *modtype,
	u_int32 *devid,
	u_int32 *devrev,
	char    *devname );
int usm_mread( u_int8 *addr, u_int16 *buff );
int usm_mwrite( u_int8 *addr, u_int16 *buff );
int usm_write( u_int8 *addr, u_int8 index, u_int16 data );
int usm_read( U_INT32_OR_64 base, u_int8 index );

#ifdef __cplusplus
   }
#endif

#endif  /*_MODCOM_H*/




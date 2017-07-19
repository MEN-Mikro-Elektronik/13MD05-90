/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mac_linux_ppc.h
 *
 *       Author: kp
 *        $Date: 2009/09/08 18:43:02 $
 *    $Revision: 1.2 $
 *
 *  Description: access macros for linux for PowerPC platforms
 *				 Since asm-ppc/io.h (kernel 2.4.0) offers both swapped and
 *				 non-swapped access macros we use them directly depending
 *				 on the MAC_BYTESWAP switch. This way the user can control
 *				 wether to swap or not hw accesses.
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mac_linux_ppc.h,v $
 * Revision 1.2  2009/09/08 18:43:02  CRuff
 * R: make 64bit compatible
 * M: add typecast to U_INT32_OR_64 in MBLOCK_WRITE_D16 macro
 *
 * Revision 1.1  2001/01/19 14:58:24  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef _MAC_LINUX_PPC_H
#define _MAC_LINUX_PPC_H


typedef unsigned long MACCESS;         /* access pointer */
typedef volatile unsigned char  __vu8;
typedef volatile unsigned short __vu16;
typedef volatile unsigned  __vu32;

#define MACCESS_CLONE(ma_src,ma_dst,offs)	ma_dst=(ma_src)+((offs))

#define _MAC_INB_	in_8
#define _MAC_OUTB_	out_8

#ifndef MAC_BYTESWAP
# define _MAC_INW_	in_be16
# define _MAC_INL_	in_be32
# define _MAC_OUTW_	out_be16
# define _MAC_OUTL_ out_be32
#else
# define _MAC_INW_	in_le16
# define _MAC_INL_	in_le32
# define _MAC_OUTW_	out_le16
# define _MAC_OUTL_ out_le32
#endif

#ifdef MAC_IO_MAPPED
# define _MAC_OFF_	(_IO_BASE)
#else
# define _MAC_OFF_	0			
#endif


#define MREAD_D8(ma,offs)		\
            _MAC_INB_((__vu8*)((MACCESS)(ma)+(offs)+_MAC_OFF_))
#define MREAD_D16(ma,offs)		\
            _MAC_INW_((__vu16*)((MACCESS)(ma)+(offs)+_MAC_OFF_))
#define MREAD_D32(ma,offs)		\
             _MAC_INL_((__vu32*)((MACCESS)(ma)+(offs)+_MAC_OFF_))

#define MWRITE_D8(ma,offs,val)	\
            _MAC_OUTB_((__vu8*)((MACCESS)(ma)+(offs)+_MAC_OFF_),val)
#define MWRITE_D16(ma,offs,val) \
            _MAC_OUTW_((__vu16*)((MACCESS)(ma)+(offs)+_MAC_OFF_),val)
#define MWRITE_D32(ma,offs,val) \
            _MAC_OUTL_((__vu32*)((MACCESS)(ma)+(offs)+_MAC_OFF_),val)

#define MSETMASK_D8(ma,offs,mask) \
            MWRITE_D8(ma,offs,MREAD_D8(ma,offs)|(mask))
#define MSETMASK_D16(ma,offs,mask) \
            MWRITE_D16(ma,offs,MREAD_D16(ma,offs)|(mask))
#define MSETMASK_D32(ma,offs,mask) \
            MWRITE_D32(ma,offs,MREAD_D32(ma,offs)|(mask))

#define MCLRMASK_D8(ma,offs,mask) \
            MWRITE_D8(ma,offs,MREAD_D8(ma,offs)&~(mask))
#define MCLRMASK_D16(ma,offs,mask) \
            MWRITE_D16(ma,offs,MREAD_D16(ma,offs)&~(mask))
#define MCLRMASK_D32(ma,offs,mask) \
            MWRITE_D32(ma,offs,MREAD_D32(ma,offs)&~(mask))


#define MBLOCK_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INB_( (__vu8*)hw );\
              hw++;	\
          }             \
        }

#define MBLOCK_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw =(MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INW_( (__vu16*)hw );\
              hw += 2;	\
          }             \
        }

#define MBLOCK_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INL_( (__vu32*)hw );\
              hw += 4;	\
          }             \
        }

#define MBLOCK_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTB_((__vu8*)hw,*mem);\
              mem++;\
              hw++;	\
          }             \
        }

#define MBLOCK_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (U_INT32_OR_64)((MACCESS)(ma)+(offs)+_MAC_OFF_); \
          while(sz--){ \
              _MAC_OUTW_((__vu16*)hw,*mem);\
              mem++; \
              hw += 2;	\
          }             \
        }

#define MBLOCK_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTL_((__vu32*)hw,*mem);\
              mem++; \
              hw += 4;	\
          }             \
        }

#define MFIFO_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INB_((__vu8*)hw);\
          }             \
        }

#define MFIFO_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INW_((__vu16*)hw);\
          }             \
        }

#define MFIFO_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
			  *mem++ = _MAC_INL_((__vu32*)hw);\
          }             \
        }

#define MFIFO_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTB_((__vu8*)hw,*mem);\
              mem++;\
          }             \
        }

#define MFIFO_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTW_((__vu16*)hw,*mem);\
              mem++; \
          }             \
        }

#define MFIFO_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs)+_MAC_OFF_; \
          while(sz--){ \
              _MAC_OUTL_((__vu32*)hw,*mem);\
              mem++; \
          }             \
        }


#define	MBLOCK_SET_D8(ma,offs,size,val)		\
		{ 	u_int32 i;					\
		for( i=0; i<(size); i++ )		\
			MWRITE_D8(ma,(offs)+i,val);		\
		}

#define MBLOCK_SET_D16(ma,offs,size,val)	\
		{ 	u_int32 i;	\
		for( i=0; i<(size); i+=2 )	\
			MWRITE_D16(ma,(offs)+i,val);			\
		}

#define MBLOCK_SET_D32(ma,offs,size,val)	\
		{ 	u_int32 i;		\
		for( i=0; i<(size); i+=4 )			\
			MWRITE_D32(ma,(offs)+i,val); 		\
		}


#endif /* _MAC_LINUX_PPC_H */


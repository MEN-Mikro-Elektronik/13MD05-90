/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mac_linux_usr.h
 *
 *       Author: christian.schuster@men.de
 *        $Date: 2007/07/13 17:56:29 $
 *    $Revision: 1.1 $
 *
 *  Description: access macros for linux for user space
 *
 *     Switches: OSS_USR_IO_MAPPED_ACC_EN
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2005-2019, MEN Mikro Elektronik GmbH
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

#ifndef _MAC_LINUX_USERSPACE_H
#define _MAC_LINUX_USERSPACE_H

/*-------------------------------+
|  read/write byte/word/dword    |
+-------------------------------*/
#if defined(MAC_MEM_MAPPED) || !defined(OSS_USR_IO_MAPPED_ACC_EN)
/*---- MEMORY MAPPED I/O ---*/
typedef unsigned long MACCESS;         /* access pointer */

#define MACCESS_CLONE(ma_src,ma_dst,offs)	ma_dst=(ma_src)+((offs))

#define MREAD_D8(ma,offs)			RSWAP8(*(volatile u_int8*)((MACCESS)(ma)+(offs)))
#define MREAD_D16(ma,offs)			RSWAP16(*(volatile u_int16*)((MACCESS)(ma)+(offs)))
#define MREAD_D32(ma,offs)			RSWAP32(*(volatile u_int32*)((MACCESS)(ma)+(offs)))

#define MWRITE_D8(ma,offs,val)		*(volatile u_int8*)((MACCESS)(ma)+(offs))  = WSWAP8(val)
#define MWRITE_D16(ma,offs,val)		*(volatile u_int16*)((MACCESS)(ma)+(offs)) = WSWAP16(val)
#define MWRITE_D32(ma,offs,val)		*(volatile u_int32*)((MACCESS)(ma)+(offs)) = WSWAP32(val)

#else
/*---- I/O mapped hardware (only CPUs supporting real I/O access (x86))----*/
typedef unsigned MACCESS;         /* access pointer */

#define MACCESS_CLONE(ma_src,ma_dst,offs)	ma_dst=(ma_src)+((offs))

#define MREAD_D8(ma,offs)			RSWAP8(inb((MACCESS)(ma)+(offs)))
#define MREAD_D16(ma,offs)			RSWAP16(inw((MACCESS)(ma)+(offs)))
#define MREAD_D32(ma,offs)			RSWAP32(inl((MACCESS)(ma)+(offs)))

#define MWRITE_D8(ma,offs,val)		outb(WSWAP8(val),(MACCESS)(ma)+(offs))
#define MWRITE_D16(ma,offs,val)		outw(WSWAP16(val),(MACCESS)(ma)+(offs))
#define MWRITE_D32(ma,offs,val)		outl(WSWAP32(val),(MACCESS)(ma)+(offs))

#endif

/*---------------------------------------------------------------------------+
|  Macros that use only MWRITE_.. / MREAD_..                                |
+---------------------------------------------------------------------------*/
/*-------------------------------+
|  read block                    |
+-------------------------------*/
//// SWAP (mem / io) ////
#define MBLOCK_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = offs; \
          while(sz--){ \
			  *mem++ = MREAD_D8(ma,hw);\
              hw++;	\
          }             \
        }

#define MBLOCK_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw = offs; \
          while(sz--){ \
			  *mem++ = MREAD_D16(ma,hw);\
              hw += 2;	\
          }             \
        }

#define MBLOCK_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = offs; \
          while(sz--){ \
			  *mem++ = MREAD_D32(ma,hw);\
              hw += 4;	\
          }             \
        }

/*-------------------------------+
|  write block                   |
+-------------------------------*/
//// SWAP (mem / io) ////
#define MBLOCK_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = offs; \
          while(sz--){ \
              MWRITE_D8( ma, hw, *mem );\
              mem++;\
              hw++;	\
          }             \
        }

#define MBLOCK_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = offs; \
          while(sz--){ \
              MWRITE_D8( ma, hw, *mem );\
              mem++; \
              hw += 2;	\
          }             \
        }

#define MBLOCK_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = offs; \
          while(sz--){ \
              MWRITE_D8( ma, hw, *mem );\
              mem++; \
              hw += 4;	\
          }             \
        }
/*-------------------------------+
|  set block (uses MWRITE_..)    |
+-------------------------------*/
#define	MBLOCK_SET_D8(ma,offs,size,val)		\
		{ 	u_int32 i;					\
		for( i=0; i<(size); i++ )		\
			MWRITE_D8(ma,(offs)+i,val);		\
		}

#define MBLOCK_SET_D16(ma,offs,size,val)	\
		{ 	u_int32 i; 	\
		for( i=0; i<(size); i+=2 )	\
			MWRITE_D16(ma,(offs)+i,val);			\
		}

#define MBLOCK_SET_D32(ma,offs,size,val)	\
		{ 	u_int32 i;	\
		for( i=0; i<(size); i+=4 )			\
			MWRITE_D32(ma,(offs)+i,val); 		\
		}

/*-------------------------------+
|  set mask (uses MWRITE_..)     |
+-------------------------------*/
#define MSETMASK_D8(ma,offs,mask)	\
			MWRITE_D8(  ma,offs,(MREAD_D8( ma,offs)) | (mask) )

#define MSETMASK_D16(ma,offs,mask)	\
			MWRITE_D16( ma,offs,(MREAD_D16(ma,offs)) | (mask) )

#define MSETMASK_D32(ma,offs,mask)	\
			MWRITE_D32( ma,offs,(MREAD_D32(ma,offs)) | (mask) )

/*-------------------------------+
|  clear mask (uses MWRITE_..)   |
+-------------------------------*/
#define MCLRMASK_D8(ma,offs,mask)	\
			MWRITE_D8(  ma,offs,(MREAD_D8( ma,offs)) &~ (mask) )

#define MCLRMASK_D16(ma,offs,mask)	\
			MWRITE_D16( ma,offs,(MREAD_D16(ma,offs)) &~ (mask) )

#define MCLRMASK_D32(ma,offs,mask)	\
			MWRITE_D32( ma,offs,(MREAD_D32(ma,offs)) &~ (mask) )

/*-------------------------------+
|  FIFO read (uses MREAD_..)     |
+-------------------------------*/
#define MFIFO_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          while(sz--){ \
			  *mem++ = MREAD_D8(ma, offs);\
          }             \
        }

#define MFIFO_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          while(sz--){ \
			  *mem++ = MREAD_D16(ma, offs);\
          }             \
        }

#define MFIFO_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          while(sz--){ \
			  *mem++ = MREAD_D32(ma, offs);\
          }             \
        }


/*-------------------------------+
|  FIFO write (uses MWRITE_..)   |
+-------------------------------*/
#define MFIFO_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          while(sz--){ \
              MWRITE_D8(ma, offs, *mem);\
              mem++;\
          }             \
        }

#define MFIFO_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          while(sz--){ \
              MWRITE_D16(ma, offs, *mem);\
              mem++; \
          }             \
        }

#define MFIFO_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (MACCESS)(ma)+(offs); \
          while(sz--){ \
              MWRITE_D32(ma, offs, *mem);\
              mem++; \
          }             \
        }

#endif /* _MAC_LINUX_USERSPACE_H */


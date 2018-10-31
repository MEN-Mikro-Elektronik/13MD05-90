/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: mac_linux_generic.h
 *
 *       Author: kp
 *        $Date: 2009/09/08 18:43:00 $
 *    $Revision: 1.4 $
 *
 *  Description: access macros for linux for non PowerPC platforms
 *
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#ifndef _MAC_LINUX_GENERIC_H
#define _MAC_LINUX_GENERIC_H

/* define here too, not just in mac_linux_ppc.h or MDIS driver builds under x86 fail */
#ifdef MAC_IO_MAPPED
# define _MAC_OFF_	(_IO_BASE)
#else
# define _MAC_OFF_	0			
#endif

#ifdef __KERNEL__
# include <asm/io.h>
#endif

/*---- MEMORY MAPPED I/O ---*/
#ifdef MAC_MEM_MAPPED
typedef void* MACCESS;         	/* access pointer */

#define MACCESS_CLONE(ma_src,ma_dst,offs)	ma_dst=(ma_src)+((offs))

#define MREAD_D8(ma,offs)		readb((MACCESS)((ma)+(offs)))
#define MREAD_D16(ma,offs)		RSWAP16(readw((MACCESS)((int*)((ma)+(offs)))))
#define MREAD_D32(ma,offs)		RSWAP32(readl((MACCESS)((int*)((ma)+(offs)))))

#define MWRITE_D8(ma,offs,val)		writeb(val,(MACCESS)(int*)((ma)+(offs)))
#define MWRITE_D16(ma,offs,val)		writew(WSWAP16(val),(MACCESS)(ma)+(offs))
#define MWRITE_D32(ma,offs,val)		writel(WSWAP32(val),(MACCESS)(ma)+(offs))

#define MSETMASK_D8(ma,offs,mask)	\
            writeb( readb( (MACCESS)(ma)+(offs))|(mask),(MACCESS)(ma)+(offs))

#define MSETMASK_D16(ma,offs,mask)	\
            writew( readw((MACCESS)(ma)+(offs)) | (u_int16)WSWAP16(mask) , \
            (MACCESS)(ma)+(offs))

#define MSETMASK_D32(ma,offs,mask)	\
            writel(readl((MACCESS)(ma)+(offs))|(u_int32)WSWAP32(mask),\
            (MACCESS)(ma)+(offs))

#define MCLRMASK_D8(ma,offs,mask)	\
            writeb( readb((MACCESS)(ma)+(offs)) & ~(mask),(MACCESS)(ma)+(offs))

#define MCLRMASK_D16(ma,offs,mask)	\
            writew( readw((MACCESS)(ma)+(offs)) & ~(u_int16)WSWAP16(mask) , \
            (MACCESS)(ma)+(offs))

#define MCLRMASK_D32(ma,offs,mask)	\
            writel( readl((MACCESS)(ma)+(offs)) & ~(u_int32)WSWAP32(mask) , \
            (MACCESS)(ma)+(offs))


#define MBLOCK_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = readb( (MACCESS)hw );\
              hw++;	\
          }             \
        }

#define MBLOCK_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw = (U_INT32_OR_64) (((MACCESS)(ma))+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP16(readw( (MACCESS)hw ));\
              hw += 2;	\
          }             \
        }

#define MBLOCK_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP32(readl( (MACCESS)hw ));\
              hw += 4;	\
          }             \
        }

#define MBLOCK_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              writeb( *mem, (MACCESS)hw );\
              mem++;\
              hw++;	\
          }             \
        }

#define MBLOCK_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (U_INT32_OR_64) (((MACCESS)(ma))+(offs)); \
          while(sz--){ \
              writew( WSWAP16( *mem ), (MACCESS)hw );\
              mem++; \
              hw += 2;	\
          }             \
        }

#define MBLOCK_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              writel( WSWAP32( *mem ), (MACCESS)hw );\
              mem++; \
              hw += 4;	\
          }             \
        }

#define MFIFO_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = readb( (MACCESS)hw );\
          }             \
        }

#define MFIFO_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP16(readw( (MACCESS)hw ));\
          }             \
        }

#define MFIFO_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP32(readl( (MACCESS)hw ));\
          }             \
        }

#define MFIFO_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              writeb( *mem, (MACCESS)hw );\
              mem++;\
          }             \
        }

#define MFIFO_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (U_INT3_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              writew( WSWAP16( *mem ), (MACCESS)hw );\
              mem++; \
          }             \
        }

#define MFIFO_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              writel( WSWAP32( *mem ), (MACCESS)hw );\
              mem++; \
          }             \
        }


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

#else
/*---- I/O mapped hardware ----*/
typedef U_INT32_OR_64 MACCESS;         /* access pointer */

#define MACCESS_CLONE(ma_src,ma_dst,offs)	ma_dst=(ma_src)+((offs))

#define MREAD_D8(ma,offs)			inb((MACCESS)(ma)+(offs))
#define MREAD_D16(ma,offs)			RSWAP16(inw((MACCESS)(ma)+(offs)))
#define MREAD_D32(ma,offs)			RSWAP32(inl((MACCESS)(ma)+(offs)))

#define MWRITE_D8(ma,offs,val)		outb(val,(MACCESS)(ma)+(offs))
#define MWRITE_D16(ma,offs,val)		outw(WSWAP16(val),(MACCESS)(ma)+(offs))
#define MWRITE_D32(ma,offs,val)		outl(WSWAP32(val),(MACCESS)(ma)+(offs))

#define MSETMASK_D8(ma,offs,mask)	\
            outb( inb((MACCESS)(ma)+(offs)) | (mask) ,(MACCESS)(ma)+(offs))

#define MSETMASK_D16(ma,offs,mask)	\
            outw( inw((MACCESS)(ma)+(offs)) | (u_int16)WSWAP16(mask) , \
            (MACCESS)(ma)+(offs))

#define MSETMASK_D32(ma,offs,mask)	\
            outl( inl((MACCESS)(ma)+(offs)) | (u_int32)WSWAP32(mask) , \
            (MACCESS)(ma)+(offs))

#define MCLRMASK_D8(ma,offs,mask)	\
            outb( inb((MACCESS)(ma)+(offs)) & ~(mask),(MACCESS)(ma)+(offs))

#define MCLRMASK_D16(ma,offs,mask)	\
            outw( inw((MACCESS)(ma)+(offs)) & ~(u_int16)WSWAP16(mask) , \
            (MACCESS)(ma)+(offs))

#define MCLRMASK_D32(ma,offs,mask)	\
            outl( inl((MACCESS)(ma)+(offs)) & ~(u_int32)WSWAP32(mask) , \
            (MACCESS)(ma)+(offs))


#define MBLOCK_READ_D8(ma,offs,size,dst) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = inb( (MACCESS)hw );\
              hw++;	\
          }             \
        }

#define MBLOCK_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP16(inw( (MACCESS)hw ));\
              hw += 2;	\
          }             \
        }

#define MBLOCK_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP32(inl( (MACCESS)hw ));\
              hw += 4;	\
          }             \
        }

#define MBLOCK_WRITE_D8(ma,offs,size,src) \
        { int sz=size;           \
          u_int8 *mem=(u_int8 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              outb( *mem, (MACCESS)hw );\
              mem++;\
              hw++;	\
          }             \
        }

#define MBLOCK_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              outw( WSWAP16( *mem ), (MACCESS)hw );\
              mem++; \
              hw += 2;	\
          }             \
        }

#define MBLOCK_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              outl( WSWAP32( *mem ), (MACCESS)hw );\
              mem++; \
              hw += 4;	\
          }             \
        }

#define MFIFO_READ_D8(ma,offs,size,dst) \
        insb( (MACCESS)(ma)+(offs), (void *)(dst), (unsigned long)((size)>>0))
#define MFIFO_WRITE_D8(ma,offs,size,src) \
        outsb( (MACCESS)(ma)+(offs), (void *)(src), (unsigned long)((size)>>0))

#ifndef MAC_BYTESWAP
/*--- non byteswapped versions ---*/
#define MFIFO_READ_D16(ma,offs,size,dst) \
        insw( (MACCESS)(ma)+(offs), (void *)(dst), (unsigned long)((size)>>1))
#define MFIFO_READ_D32(ma,offs,size,dst) \
        insl( (MACCESS)(ma)+(offs), (void *)(dst), (unsigned long)((size)>>2))
#define MFIFO_WRITE_D16(ma,offs,size,src) \
        outsw( (MACCESS)(ma)+(offs), (void *)(src), (unsigned long)((size)>>1))
#define MFIFO_WRITE_D32(ma,offs,size,src) \
        outsl( (MACCESS)(ma)+(offs), (void *)(src), (unsigned long)((size)>>2))
#else
/*--- byteswapped versions ---*/
#define MFIFO_READ_D16(ma,offs,size,dst) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP16(inw( (MACCESS)hw ));\
          }             \
        }

#define MFIFO_READ_D32(ma,offs,size,dst) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)dst; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
			  *mem++ = RSWAP32(inl( (MACCESS)hw ));\
          }             \
        }

#define MFIFO_WRITE_D16(ma,offs,size,src) \
        { int sz=size>>1;           \
          u_int16 *mem=(u_int16 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              outw( WSWAP16( *mem ), (MACCESS)hw );\
              mem++; \
          }             \
        }

#define MFIFO_WRITE_D32(ma,offs,size,src) \
        { int sz=size>>2;           \
          u_int32 *mem=(u_int32 *)src; \
          unsigned long hw = (U_INT32_OR_64) ((MACCESS)(ma)+(offs)); \
          while(sz--){ \
              outl( WSWAP32( *mem ), (MACCESS)hw );\
              mem++; \
          }             \
        }
#endif /* MAC_BYTESWAP */

#define	MBLOCK_SET_D8(ma,offs,size,val)		\
		{ 	u_int32 i;					\
		for( i=0; i<(size); i++ )		\
			MWRITE_D8(ma,(offs)+i,val);		\
		}

#define MBLOCK_SET_D16(ma,offs,size,val)	\
		{ 	u_int32 i;	\
		for( i=0; i<(size); i+=2 )	\
			MWRITE_D16(ma,(offs)+i,val);		\
		}

#define MBLOCK_SET_D32(ma,offs,size,val)	\
		{ 	u_int32 i;		\
		for( i=0; i<(size); i+=4 )	\
			MWRITE_D32(ma,(offs)+i,val);		\
		}

#endif

#endif /* _MAC_LINUX_GENERIC_H */


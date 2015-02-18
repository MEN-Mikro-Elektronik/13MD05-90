/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  memdump.c
 *
 *      \author  see
 *        $Date: 2009/07/22 13:26:59 $
 *    $Revision: 1.11 $
 *
 *     \project  UTL library
 *  	 \brief  Print formatted hexdump
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: memdump.c,v $
 * Revision 1.11  2009/07/22 13:26:59  dpfeuffer
 * R: Generate doxygen documentation for MDIS5
 * M: file and function headers changed for doxygen
 *
 * Revision 1.10  2009/03/31 10:56:26  ufranke
 * cosmetics
 *
 * Revision 1.9  2008/08/18 15:12:58  CKauntz
 * Added MENTYPS_64bit switch for 64 bit support and correct address view
 *
 * Revision 1.8  2005/06/30 10:46:19  UFranke
 * cosmetics
 *
 * Revision 1.7  2003/11/12 13:48:45  dschmidt
 * __MAPILIB prefix added
 *
 * __MAPILIB prefix added
 * Revision 1.6  1999/07/20 14:46:11  Franke
 * removed bug at word/byte print
 *
 * Revision 1.5  1999/04/15 14:16:55  Franke
 * cosmetics
 *
 * Revision 1.4  1999/02/15 15:12:12  see
 * UTL_Memdump: recognize and indent with trailing blanks
 *
 * Revision 1.3  1998/08/11 16:16:54  Schmidt
 * unreferenced local variable 'c' removed
 *
 * Revision 1.2  1998/08/10 10:44:32  see
 * cosmetics
 *
 * Revision 1.1  1998/07/02 15:29:16  see
 * Added by mcvs
 *
 * cloned from S_UTILS lib
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1998 MEN Mikro Elektronik GmbH. All rights reserved.
 ****************************************************************************/

char UTL_memdump_RCSid[]="$Id: memdump.c,v 1.11 2009/07/22 13:26:59 dpfeuffer Exp $";

#include <MEN/men_typs.h>
#include <stdio.h>
#include <MEN/usr_utl.h>

/**********************************************************************/
/** Print formatted hexdump
 *
 *  Trailing blanks in the description string define the
 *  indention of all following debug lines.
 *  \n
 *  E.g. output for
 * \code
 * UTL_Memdump("WORD access", (char*)0x0000f000, 0x40, 2);
 * \endcode
 * is:
 * \code
 *	WORD access (64 bytes):
 *	0000f000+0000: 4afc 0001 0000 007c 0000 0009 0000 0074 J|.....|.......t
 *	0000f000+0010: 0777 0f00 8000 0006 0000 0000 0000 0000 .w..............
 *	0000f000+0020: 0000 0000 0000 0000 0000 0000 0000 3d72 ..............=r
 *	0000f000+0030: 00ff ee00 5c04 0023 0064 0068 0070 0000 ..n.\..#.d.h.p..
 * \endcode
 *
 *  \param info    \IN description string or NULL (first line)
 *  \param buf     \IN memory start address
 *  \param n       \IN memory length [bytes]
 *  \param fmt     \IN output format\n
 *                     1 = byte aligned\n
 *                     2 = word aligned\n
 *                     4 = long aligned\n
 *                     8 = 64-bit aligned
 */
void __MAPILIB UTL_Memdump(
    char *info,
    char *buf,
    u_int32 n,
    u_int32 fmt
)
{
    char *k, *k0, *kmax = buf+n;
	char *s = info;
    int32 i, b, blanks=0;

    if (info) {
		while(*s++ == ' ')			/* count trailing blanks */
			blanks++;

		printf("%s (%ld bytes)\n",info,n);	/* print info */
	}

    for (k=k0=buf; k0<kmax; k0+=16)
    {
		for (b=0; b<blanks; b++)	/* write trailing blanks */
			printf(" ");

#ifndef MENTYPS_64BIT
		printf("%08x+%04x: ",(unsigned int) buf, (unsigned int)(k-buf));
#else
		printf("%016x+%04x: ",(U_INT32_OR_64) buf, (unsigned int)(k-buf));
#endif /* U_INT32_OR_64 is u_int32 */

		switch(fmt)                                        /* dump hex: */
        {
           case 8 : for (k=k0,i=0; i<16; i+=8, k+=8)       /* 64-bit aligned */
                       if (k<kmax)  printf("%08lx%08lx ",*(u_int32*)k,*(u_int32*)(k+4));
                       else         printf("         ");
                    break;
           case 4 : for (k=k0,i=0; i<16; i+=4, k+=4)       /* long aligned */
                       if (k<kmax)  printf("%08lx ",*(u_int32*)k);
                       else         printf("         ");
                    break;
           case 2 : for (k=k0,i=0; i<16; i+=2, k+=2)       /* word aligned */
                       if (k<kmax)  printf("%04x ",*(u_int16*)k & 0xffff);
                       else         printf("     ");
                    break;
           default: for (k=k0,i=0; i<16; i++, k++)         /* byte aligned */
                       if (k<kmax)  printf("%02x ",*k & 0xff);
                       else         printf("   ");
        }

        for (k=k0,i=0; i<16 && k<kmax; i++, k++)                /* dump ascii */
            if ( *(u_int8*)k>=32 && *(u_int8*)k<=127 )
               printf("%c", *k);
            else
               printf(".");

        printf("\n");
    }
}

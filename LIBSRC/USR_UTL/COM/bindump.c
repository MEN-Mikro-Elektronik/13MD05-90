/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  bindump.c
 *
 *      \author  see
 *        $Date: 2009/07/22 13:26:57 $
 *    $Revision: 1.6 $
 *
 *     \project  UTL library
 *  	 \brief  Binary dump routine
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: bindump.c,v $
 * Revision 1.6  2009/07/22 13:26:57  dpfeuffer
 * R: Generate doxygen documentation for MDIS5
 * M: file and function headers changed for doxygen
 *
 * Revision 1.5  2009/03/31 10:55:45  ufranke
 * cosmetics
 *
 * Revision 1.4  2005/06/30 10:45:46  UFranke
 * cosmetics
 *
 * Revision 1.3  1998/08/11 16:16:50  Schmidt
 * UTL_Bindump() : unreferenced local variables 'n' and 'count' removed
 *
 * Revision 1.2  1998/08/10 10:44:29  see
 * UTL_Bindump: string buffer is now passed from application
 * UTL_Bindump: format changed to nr of bits
 *
 * Revision 1.1  1998/07/02 15:29:14  see
 * Added by mcvs
 *
 * cloned from S_UTILS lib
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1998 MEN Mikro Elektronik GmbH. All rights reserved.
 ****************************************************************************/

char UTL_bindump_RCSid[]="$Id: bindump.c,v 1.6 2009/07/22 13:26:57 dpfeuffer Exp $";

#include <MEN/men_typs.h>
#include <stdio.h>
#include <MEN/usr_utl.h>

/**********************************************************************/
/** Create binary dump string of long value
 *
 *  All nibbles are separated with blanks:\n
 *  bits=8:   'xxxx xxxx'\n
 *  bits=9: 'x xxxx xxxx'
 *
 *  \param  data    \IN value to dump
 *  \param  bits    \IN nr of bits (1..32)
 *  \param  buf     \OUT filled string buffer (size=40)
 *  \return			ptr to string buffer
 */
char *UTL_Bindump(u_int32 data, u_int32 bits, char *buf)
{
   u_int32 mask;
   char *sptr=buf;

   if (bits > 32)
	   bits = 32;

   mask = 1 << (bits-1);

   while(bits--) {
      if (sptr!=buf && bits && (bits+1)%4==0)   /* after 4 digits */
		  *sptr++ = ' ';             			/* insert blank */

      *sptr++ = ((data & mask) ? '1' : '0');
	  mask >>= 1;
   }

   *sptr = 0;                        /* terminator */
   return(buf);
}

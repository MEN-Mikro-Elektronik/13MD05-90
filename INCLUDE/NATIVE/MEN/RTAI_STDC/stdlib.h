/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  stdlib.h
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/06/06 09:36:31 $
 *    $Revision: 1.2 $
 *
 *  	 \brief  Header file to let RTAI MDIS example programs perform
 *				 a limted set of stdlib functions
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: stdlib.h,v $
 * Revision 1.2  2003/06/06 09:36:31  kp
 * cosmetics
 *
 * Revision 1.1  2003/04/11 16:16:54  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _RTAI_STD_STDLIB_H
#define _RTAI_STD_STDLIB_H

#define malloc 	rt_malloc 	/**< RTAI memory allocation */
#define free 	rt_free		/**< RTAI memory release */

extern int atoi(const char *s);
extern long int strtol(const char *nptr, char **endptr, int base);
extern unsigned long int strtoul(const char *nptr, char **endptr, int base);

/** C-lib routine isspace */
extern int inline isspace( int ch )
{
    return (unsigned int)(ch - 9) < 5u  ||  ch == ' ';
}

#endif /* _RTAI_STD_STDLIB_H */


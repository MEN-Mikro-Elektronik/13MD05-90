/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: panic.c
 *      Project: UTI library
 *
 *       Author: kp
 *        $Date: 2009/10/28 18:36:40 $
 *    $Revision: 1.6 $
 *
 *  Description: printout error message and leave the program
 *                      
 *                      
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: panic.c,v $
 * Revision 1.6  2009/10/28 18:36:40  rt
 * R: 1.) Not compilable with newer LINUX (TLS) glibc versions
 * M: 1.) Include errno.h.
 *
 * Revision 1.5  2008/09/15 12:49:13  GLeonhardt
 * Avoid warnings
 *
 * Revision 1.4  1999/01/20 14:54:27  kp
 * updated for LINUX
 *
 * Revision 1.3  1998/01/08 08:51:58  popp
 * Updated for Visual C 4.0
 *
 * Revision 1.2  1997/12/19 15:14:16  kp
 * HP/OS9 merge
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Header: /dd2/CVSR/COM/LIBSRC/UTI/panic.c,v 1.6 2009/10/28 18:36:40 rt Exp $";

#include <stdio.h>
#include <stdlib.h>
#ifdef LINUX
 #include <errno.h>
#endif /*LINUX*/

void panic(str1, str2)
char *str1, *str2;
{
#ifndef LINUX
	extern int errno;
#endif /*LINUX*/
#ifdef OS9
	extern char *_prgname();

	fprintf(stderr, "%s: %s %s\n", _prgname(), str1, str2);
#endif
#if defined(HPUX) || defined(HPRT) || defined(WIN32) || defined(LINUX)
	fprintf(stderr, "%s %s\n", str1, str2);
	perror("");
#endif

	exit(errno);
}

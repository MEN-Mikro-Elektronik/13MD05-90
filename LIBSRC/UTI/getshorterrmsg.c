/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: getshorterrmsg.c
 *      Project: 
 *
 *       Author: 
 *        $Date: 2009/10/28 18:38:44 $
 *    $Revision: 1.5 $
 *
 *  Description: fill buffer with error message
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
 * $Log: getshorterrmsg.c,v $
 * Revision 1.5  2009/10/28 18:38:44  rt
 * R:1 Not compilable under Linux.
 * M:1. Use strerror() instead of ERRMSG().
 *
 * Revision 1.4  1999/01/20 14:54:21  kp
 * updated for LINUX
 *
 * Revision 1.3  1998/01/08 08:51:49  popp
 * Updated for Visual C 4.0
 *
 * Revision 1.2  1997/12/19 15:05:32  kp
 * HP/OS9 merge
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Header: /dd2/CVSR/COM/LIBSRC/UTI/getshorterrmsg.c,v 1.5 2009/10/28 18:38:44 rt Exp $";

#include <stdio.h>
#include <errno.h>

char *getshorterrmsg( error, buffer )
int error;			/* error number to search for							*/
char *buffer;		/* the buffer that gets the message						*/
					/* buffer must be at least 81 chars						*/
{

#if defined(HPRT) || defined(HPUX)
	char *s;
	s = ERRMSG(error);
	sprintf(buffer, "%d - %s",error,(s ? s : "(unknown)"));
#endif

#ifdef OS9
	extern char *index();
	char *p;
	int fd;			/* file descriptor for error message file 				*/
	int cmp_lo, cmp_hi;

	if( (fd = open("/dd/SYS/errmsg.short", 1)) == -1)
		return NULL;	/* cannot open	*/		
		
	while( readln( fd, buffer, 80 ) > 0 )
		if( sscanf( buffer, "%3d:%3d", &cmp_hi, &cmp_lo ) == 2 )
			if( (cmp_hi == error >> 8) && (cmp_lo == error & 0xff)){
				if( p = index( buffer, '\n'))
					*p = '\0';
				close(fd);
				return buffer;
			}		
	/* error number was not found: simply print error number */
	sprintf( buffer, "%03d:%03d", error >> 8, error & 0xff );
#endif

#if defined(WIN32) || defined(LINUX)
    char *result;

    result = strerror(error);
	strcpy(buffer,result);
#endif

#if !defined(HPRT) && !defined(HPUX) && !defined(OS9) && !defined(WIN32) && !defined(LINUX)
	sprintf(buffer, "%d", error);
#endif
	return buffer;
}

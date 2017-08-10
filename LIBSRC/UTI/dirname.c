/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: dirname
 *      Project: uti
 *
 *      $Author: GLeonhardt $
 *        $Date: 2008/09/15 12:56:56 $
 *    $Revision: 1.4 $
 *
 *  Description: gets the directory from a whole path
 *                      
 *               buf is needed to hold the directory name, dirname always 
 *				 returns <buf>
 *
 *     Required:  
 *     Switches:  
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: dirname.c,v $
 * Revision 1.4  2008/09/15 12:56:56  GLeonhardt
 * Avoid warnings
 *
 * Revision 1.3  2008/03/07 09:50:37  aw
 * + include string.h
 *
 * Revision 1.2  1998/01/08 08:51:32  popp
 * Updated for Visual C 4.0
 *
 * Revision 1.1  1996/12/19 15:24:37  kp
 * Initial revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1996 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static char *RCSid="$Header: /dd2/CVSR/COM/LIBSRC/UTI/dirname.c,v 1.4 2008/09/15 12:56:56 GLeonhardt Exp $";

#include <string.h>

char *dirname(path,buf)
char *path, *buf;
{
	char *p;
    /* include ident string, suppress warning */
    char a = a = RCSid[0];

	strcpy( buf, path );
	if( (p = strrchr(buf,'\\')) != NULL ){
		/*--- path includes a directory name ---*/
		*p = '\0';
	}
	else if( (p = strrchr(buf,'/')) != NULL ){
		/*--- path includes a directory name ---*/
		*p = '\0';
	}
	else {
		/*--- no directory included, return "." ---*/
		strcpy( buf, ".");
	}
	
	return buf;
}			


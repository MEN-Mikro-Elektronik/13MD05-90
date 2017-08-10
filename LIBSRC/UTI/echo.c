/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: echo.c
 *      Project: uti lib
 *
 *      $Author: aw $
 *        $Date: 2008/03/07 09:50:39 $
 *    $Revision: 1.5 $
 *
 *  Description: switch on/off echo
 *                      
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: echo.c,v $
 * Revision 1.5  2008/03/07 09:50:39  aw
 * + define OS9000
 *
 * Revision 1.4  1999/01/20 14:54:14  kp
 * updated for LINUX
 *
 * Revision 1.3  1998/01/08 08:51:39  popp
 * Updated for Visual C 4.0
 *
 * Revision 1.2  1997/12/19 14:51:27  kp
 * HP/OS9 merge
 *
 * Revision 1.1  1996/12/19 15:24:38  kp
 * Initial revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1996 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static char *RCSid="$Header: /dd2/CVSR/COM/LIBSRC/UTI/echo.c,v 1.5 2008/03/07 09:50:39 aw Exp $";

#include <stdio.h>

#if defined(HPRT) || defined(HPUX) || defined(LINUX)

#include <ioctl.h>

echo(path,on)
int	path;
int	on;
{
	p_echo(path,on);
}

p_echo(path, on)
int	path;
int	on;
{
	static struct old_sgttyb opts;
	int old;

    ioctl(path, OTIOCGETP, &opts);		/* get term params */

	old = opts.sg_flags & ECHO;

	if (on)		
		opts.sg_flags |= ECHO;		/* echo ON */
	else
		opts.sg_flags &= ~ECHO;		/* echo OFF */

    ioctl(path, OTIOCSETP, &opts);		/* set term params */
	return old;

}
#endif /* HP */

#if defined(OS9) || defined(OS9000)
#include <sgstat.h>
static struct sgbuf termpd;

echo(path, on)
int	path;
int	on;
{
	p_echo(path,on);
}

p_echo(path, on)
int	path;
int	on;
{
	int	rval;

	_gs_opt(path,&termpd);	rval=termpd.sg_echo;	termpd.sg_echo = on;
	_ss_opt(path,&termpd);

	return(rval);
}
#endif

#ifdef WIN32

echo(path, on)
int	path;
int	on;
{
	/* no function */
}

p_echo(path, on)
int	path;
int	on;
{
	return 0;
}
#endif

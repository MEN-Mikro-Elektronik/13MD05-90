/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: echo.c
 *      Project: uti lib
 *
 *      Author: aw 
 *
 *  Description: switch on/off echo
 *                      
 *---------------------------------------------------------------------------
 * Copyright (c) 1996-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
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

/*****************************************************************************

Project...........: io.lib
Filename..........: pause.c
Author............: K.P.
Creation Date.....: 30.05.90

Description.......: nopause(): switch terminal to nopause mode (tmode nopause)
					restpause(): restore previous settings
					
********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 | 30.05.90 | First Installation                           		| K.P.   *
******************************************************************************/

#include <sgstat.h>
static struct sgbuf termpd,oldtermpd;

nopause()
{
	_gs_opt(1,&termpd);
	oldtermpd = termpd;
	termpd.sg_pause = 0;
	_ss_opt(1,&termpd);
} 
restpause()
{
	_ss_opt(1,&oldtermpd);
}



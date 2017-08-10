/*****************************************************************************

Project...........: uti.lib - utility library
Filename..........: clearmem.c
Author............: k.p.
Creation Date.....: 26.09.90

Description.......: clear a number of bytes

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 | 26.09.90 | First Installation                           		| K.P.   *
******************************************************************************/

/* 
 * clearmem(char *mem, int count)
 * clear a number of bytes;
 */
clearmem(mem, count)
char 		*mem;
int			count;
{
	while(count--) *mem++ = 0;
}


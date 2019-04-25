/*****************************************************************************
*
* Project...........: uti.lib - utility library
* Filename..........: clearmem.c
* Author............: k.p.
* Creation Date.....: 26.09.90
*
* Description.......: clear a number of bytes
*
*---------------------------------------------------------------------------
* Copyright (c) 2019, MEN Mikro Elektronik GmbH
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


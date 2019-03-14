/*****************************************************************************
*
* Project...........: ulib
* Filename..........: cntarg.c
* Author............: H.S.
* Creation Date.....: 4.4.90
*
* Description.......: Counts the 'real' options in argc,argv 
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

int cntarg(int argc,char *argv[])
/* counts the 'real' arguments in argc,argv */
/* (program call and options are not counted */
{
	int i,n;
	char *index();

	n = 0;
	for(i=1; i<argc; i++){
		if(*argv[i] != '-') n++;
	}
	return(n);
}


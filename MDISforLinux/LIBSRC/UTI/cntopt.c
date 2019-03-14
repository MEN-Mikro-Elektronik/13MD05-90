/*****************************************************************************
*
* Project...........: ulib
* Filename..........: cntopt.c
* Author............: H.S.
* Creation Date.....: 4.4.90
* Description.......: Count the options in argc,argv
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

int cntopt(int argc,char *argv[])
/* counts the options in argc,argv */
{
	int i,j,n;
	char *index();

	n = 0;
	for(i=1; i<argc; i++){
		if(*argv[i] == '-'){
			for(j=1; argv[i][j] != 0 && argv[i][j] != '=';j++) n++;
		}
	}
	return(n);
}


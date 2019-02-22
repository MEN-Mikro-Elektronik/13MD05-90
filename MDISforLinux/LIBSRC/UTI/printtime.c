/*****************************************************************************
* 
* Project...........: uti.l
* Filename..........: printtime.c
* Author............: kp
* Creation Date.....: 15.10.90
* 
* Description.......: 
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

#include <time.h>
#include <stdio.h>

void printtime()
{
	extern struct tm *localtime();
/*	extern long time();*/
	struct tm *tt;
	long t;
	
	static char *days[] = {
		"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
	};
	
	t = time(NULL);
	tt = localtime(&t);
	t /= 86400;
	
	printf(" %-9s",days[tt->tm_wday]);
	printf(" %02d.%02d.%02d",tt->tm_mday,tt->tm_mon+1,tt->tm_year);
	printf(" %02d:%02d",tt->tm_hour,tt->tm_min);
}


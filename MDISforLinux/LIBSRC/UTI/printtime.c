/*****************************************************************************

Project...........: uti.l
Filename..........: printtime.c
Author............: kp
Creation Date.....: 15.10.90

Description.......: 

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |          | First Installation                           		|        *
******************************************************************************/
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


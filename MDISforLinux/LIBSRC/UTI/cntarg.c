/*****************************************************************************

Project...........: ulib
Filename..........: cntarg.c
Author............: H.S.
Creation Date.....: 4.4.90

Description.......: Counts the 'real' options in argc,argv 

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |          | First Installation                           		|        *
******************************************************************************/

int cntarg(argc,argv)
/* counts the 'real' arguments in argc,argv */
/* (program call and options are not counted */
char **argv;
{
	int i,n;
	char *index();

	n = 0;
	for(i=1; i<argc; i++){
		if(*argv[i] != '-') n++;
	}
	return(n);
}


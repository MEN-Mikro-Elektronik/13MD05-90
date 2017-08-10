/*****************************************************************************

Project...........: ulib
Filename..........: cntopt.c
Author............: H.S.
Creation Date.....: 4.4.90
Description.......: Count the options in argc,argv

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************

******************************************************************************
* ED | DATE     | CHANGES                                      		| EDITOR *
*----------------------------------------------------------------------------*
*              <<<--- OS-9/68000 V2.3 Release --->>>                         *
*----------------------------------------------------------------------------*
* 01 |          | First Installation                           		|        *
******************************************************************************/

int cntopt(argc,argv)
/* counts the options in argc,argv */
char **argv;
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


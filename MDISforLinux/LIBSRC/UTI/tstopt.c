/*****************************************************************************
* 
* Project...........: ulib 
* Filename..........: tstopt.c
* Author............: H.S. / K.P.
* Creation Date.....: 4.4.90
* 
* Description.......: Test command line options
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//char *option;	/* option character to test, followed by */
				/* nothing : no argument */
				/*     ':' : optional argument, */ 
				/*			 deflt returned if no argument given */ 
				/* 	   '=' : argument required */
				/* 	   '!' : toggle argument (like -x/-nx) */
//char *deflt;	
char *tstopt(int argc,char *argv[],char *option,char *deflt)
{
	int i;
	char *p,*p1,*p2;
	
	for(i=1; i<argc; i++){
		if(*argv[i] != '-')	continue;
		if(!(p2 = strchr(argv[i],'='))) p2 = (char *)-1;
		if((p = strchr(argv[i],option[0])) && (p < p2)){
			if(*(p+1) != '=') p1 = NULL; else p1 = (char *)-1;
			if( strlen(option) == 1 ){		/* option without argument */
					return (char *)-1;
			}else{
				switch(option[1]){
				case ':':		/* option with optional argument */
					if(p1 == NULL)  return deflt;
					else		   return p+2;
				case '=':		/* option which requires an argument */
					if(p1 == NULL) {
						fprintf(stderr,
						 "*** Option '%c' requires an argument\n",option[0]);
						exit(1);
					}
					return p+2;
				case '!':		/* option like  -x  or -nx */
					if(*(p-1) == 'n') 	return (char *) -2;
					else				return (char *) -1; 
				}
			}
		}	
	}
	return NULL;	/* option not found */
}

void illiopt(int argc,char *argv[],char *opts)
{
	int i;
	char *p,*p1;

	for(i=1; i<argc; i++){
		if(*argv[i] != '-')	continue;
		for(p = (argv[i]+1); *p && *p != '='; p++){
			if(strchr(opts,*p) == NULL){
				if( *p == 'n' ){
					p++;
					p1 = opts;
					while( strchr(p1,'!')!=NULL && *(strchr(p1,'!')-1)!=*p ){
						p1 = strchr(p1,'!')+1;
					}
					if( strchr(p1,'!') == NULL ){
						fprintf(stderr,"*** Unrecognized option '-n%c'\n",*p);
						exit(0);
					}
				}else{
					fprintf(stderr,"*** Unrecognized option '-%c'\n",*p);
					exit(0);
				}
			}
		}
	}
}

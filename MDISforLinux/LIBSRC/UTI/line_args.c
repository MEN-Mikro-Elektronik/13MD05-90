/*****************************************************************************
*
* Project...........: uti.l
* Filename..........: line_args.c
* Author............: kp
* Creation Date.....: 30.11.90
*
* Description.......: convert line into arguments
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

#include <stdlib.h>

#if !defined(NULL)
#define NULL 0L
#endif

/*
 * line_args()
 * returns the number of words found or -1 if error
 * last argument will be a NULL pointer!
 * attention: separators will be replaced with a '\0' character 
 */

int line_args(lp, argbuf, maxargs, separators, termchars)
char *lp;		/* input line */
char **argbuf;		/* ptr array for results (maxarg+1 entries !)*/
int	 maxargs;	/* size of argbuf */
char *separators;	/* argument separator chars */
char *termchars;	/* termination characters */
{
	char 	*p, **u, *strchr(), *strsave();
	int		end = 0, term = 0;
	int 	argc = 0, flg = 0, quote=0, sep=0;

	/* scan input line */

	for(u = argbuf, p = lp;  argc < maxargs; p++){ 	

                if (strchr(termchars, *p))      /* terminator */
                  term=1;
                if(strchr(separators, *p))      /* separator */
                  sep=1;
                if(p)
		  end  = (*p == '\0');          /* end of string */
		if(*p == '"')
                  quote = !quote;

		if(end && quote)                /* string end but opened '"' */
			return -1;		/* error */

		if(quote == 0 || flg == 0){	/* not in '"' */
			if(end || term || sep){
				if(flg == 1){
			  		*p = 0;                         /* found string end */
			  		if((*u = strsave(*u)) == 0)     /* save string */
						return -1;
					u++;
			  	}	
			  	flg = 0;
			  	if(end || term) break;
			}	
			else {
			  	if(flg == 0) {
					*u = p;				/* save string begin */
					argc++;
				}
			  	flg = 1;
			}
		}				
	}
	*u = NULL;							/* flag end of args */
	return argc;
}

/*
 * free_args()
 */
 
void free_args(argbuf)
char **argbuf;
{
	while(*argbuf){
		free(*argbuf);
		argbuf++;
	}
}

/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: macro_ex.c
 *      Project: 
 *
 *       Author: 
 *
 *  Description: macro definition and expansion
 *                      
 *                      
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *---------------------------------------------------------------------------
 * Copyright (c) 1997-2019, MEN Mikro Elektronik GmbH
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


#define MNAMELEN	20	/* max chars for macro name */
#if !defined(NULL)
#define NULL		0L  /* ptr to nowhere */
#endif
/* 
 * variable structure 
 */
typedef struct macro {
	struct macro *next;	/* ptr to next variable */
	char *name,			/* name of variable */
		 *subs;			/* substitution string */
} *Macro;

extern char *strsave();

/*
 * macro_def()	define a macro
 * returns: 0=ok, 1=macro exists (overwrite = 0), -1= allocation error 
 */

int macro_def(mhead, name, sub, overwrite)
Macro *mhead;	/* the macro list header */
char *name,		/* the macro name */
	 *sub; 		/* the substitution string */
int	 overwrite; /* overwrite macro if macro already defined */
{
	Macro newmac, macro_search();

	if( (newmac = macro_search(mhead, name)) != NULL ){		/* macro exists */
		if(overwrite)
			free(newmac->subs); 				/* free old subst. string */
		else
			return 1;
	}	
	else {
		/* create new macro */
		if((newmac = (Macro)malloc(sizeof(*newmac))) == NULL)
			return -1;

		/* save macro name string */
		if((name = strsave(name)) == NULL){
			free(newmac);
			return -1;
		}
		/* insert new macro at head of list */
		newmac->next = *mhead;
		newmac->name = name;
		*mhead 	 	 = newmac;
	}

	/* save subst. string */
	if((sub = strsave(sub)) == NULL)
		return -1;

	newmac->subs = sub;
	return 0;
}		

/*
 * macro_search()
 * search for a macro
 * returns: pointer to macro struct if found or NULL if not found
 */
Macro macro_search(mhead, name)
Macro *mhead;	/* macro list header */	
char *name; 	/* macro name to look for */
{
	register Macro m;

	for(m = *mhead; m; m = m->next){
		if(!strcmp(name, m->name))
			return m;
	}
	return NULL; /* not found */
}

/*
 * macro_expand()
 * expand a string containing macro(s)
 */
char *macro_expand(mhead, word, buf, maxchars)
Macro *mhead;	/* macro list header */	
char *word, 	/* macro name to expand */
	 *buf;		/* buffer for expanded macro */
int  maxchars;	/* size of buf */
{
	int i;
	char *p, *oldbuf;
	char tmpbuf[MNAMELEN+1];
	Macro mac;
	
	oldbuf = buf;

    if (word)
	  while(*word && maxchars){	/* scan string */
		if(*word == '$'){		
			word++;
			if(*word == '('){
				word++;

				for (i = 0, p = tmpbuf; 
				 *word != ')' && *word && i<MNAMELEN; i++)
					*p++ = *word++;
				
				if(*word == '\0' || i == MNAMELEN){
					return NULL; /* unterminated macro */
				}
				*p = '\0';
			}
			else if(*word != '\0'){
				tmpbuf[0] = *word;
				tmpbuf[1] = 0;
			}
			else {
				return NULL;
			}
			word++;

			if( (mac = macro_search(mhead, tmpbuf)) != NULL ){
				/* recursive call to resolve nested macros */
				if(macro_expand( mhead, mac->subs, buf, maxchars) == NULL)
					return NULL;

				maxchars -= strlen(buf);
				buf += strlen(buf);
			}
			else {
				return NULL; /* macro not found */
			}									
		}
		else {
			*buf++ = *word++;
			maxchars--;
		}
	}
	*buf = 0;
	return oldbuf;
}

/*
 * macro_clonelist()
 * Clone macro list
 * returns: 0=ok
 */
int macro_clonelist(mhead, new_mhead )
Macro *mhead;	/* the macro list header to clone */
Macro *new_mhead;
{
	register Macro m;
	int rc;

	for( m = *mhead; m; m = m->next){
		if( (rc = macro_def( new_mhead, m->name, m->subs, 0 )) != 0 )
			return rc;
	}
	return 0;
}

/*
 * macro_freelist
 * return memory of a macro list
 */
void macro_freelist( mhead )
Macro *mhead;	/* the macro list header */
{
	register Macro m,n;

	for(m = *mhead; m; m = n){
		n = m->next;
		free(m);
	}
	mhead=NULL;
}

/*
 * macro_dump - dump macro list
 */
void macro_dump( mhead )
Macro *mhead;	/* the macro list header */
{
	register Macro m;

	for(m = *mhead; m; m = m->next)
		printf( "macro <%s> = <%s>\n", m->name, m->subs );
}

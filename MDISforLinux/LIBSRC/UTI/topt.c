/*****************************************************************************
* 
* Project...........: test option
* Filename..........: topt.c
* Author............: W.Witt
* Creation Date.....: 06.01.91
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

#include	<stdio.h>
#include	<errno.h>
#include	<ctype.h>

#define bit8		unsigned char		/* Byte		*/
#define bit16		unsigned short		/* word16	*/
#define bit32		unsigned long		/* word32	*/
/*****************************************************************************
** option character to test, followed by
** nothing : default is used
** 	   '=' : argument is decimal
**	   '$' : argument is hexadecimal
**     ':' : argument is a string
** 	   '!' : toggle argument (like -x/-nx)
**
** note: [ option '-n' is not allowed ]
******************************************************************************
*/
topt(argc,argv,opts,optv)

int argc;
char *argv[];		/* **argv; 											*/
char *opts;			/* option character to test, followed by 			*/
					/* nothing : no argument 							*/
					/*     ':' : optional argument, 					*/ 
					/*			 deflt returned if no argument given 	*/ 
					/* 	   '=' : argument required 						*/
					/* 	   '!' : toggle argument (like -x/-nx) 			*/
					/*	   '$' : argument is hexadecimal				*/	
struct option *optv;/* structure for options value						*/
{
int 	i,j,n,flag;
int		*ptr= (int *)optv;
char 	*tp,*op;
char	*srq(),*cpy(),*skip();
/*1---------------------------------------------------------------------------*/
	flag = 0;
	for (i=1; i<argc; i++)
	{	if (*argv[i] != '-')
			continue;
/*2---------------------------------------------------------------------------*/
		for (tp=argv[i]+1; *tp != '\0'; tp++)
		{	flag = (int)tp;
			for (op=opts,j=0; *op != '\0'; op++,j++)
			{	if (*tp == *op)
				{	flag = 0;
					switch( *(op+1) )
					{	case '!':	if (*(tp-1) == 'n')
										*(ptr+j)=0;
									else
										(*(ptr+j))++;
									break;

						case ':':	tp = srq(tp+1);
								/*	if	(isalpha(*(tp-1)))
										break; 
								*/	tp = cpy(*(ptr+j),tp);
									break;
	
						case '$':	tp = srq(tp+1);	
									if (sscanf(tp,"%x",&n) == 1)
										*(ptr+j)=n;
									tp = skip(tp);
									break;

						case '=':	tp = srq(tp+1);	
									if (sscanf(tp,"%d",&n) == 1)
										*(ptr+j)=n;
									tp = skip(tp);
									break;

						default:	flag = (int)tp;
									break;						
					} /* end switch	*/
				} /* end if */
/*----------------------------------------------------------------------------*/
				if( flag == 0 )
					break;
					
				while (*op != ',' && *op != ' ')
					if (*op == '\0')
						break;
					else
						op++;
			} /* end for op	*/
/*----------------------------------------------------------------------------*/
			if( flag )
				return flag;
			if( *tp == '\0' )
				break;
				
		} /* end for tp	*/
	} /* end for argc	*/
	return 0;
}

/*page*/
/*****************************************************************************
** copy string
*/
static char *cpy(s,t)
char *s;
char *t;
{
/*
	int i;
	
	for(i=0; i<strlen(s); i++) {
		if( *t == ',' || *t == '-' || *t == '\0' )
			break;
		*s++ = *t++;	
		*s = '\0';
	}
	return (t);
*/
	while (*t != ',' && *t != '-' && *t != '\0')
	{	*s++ = *t++;
		*s   = '\0';
	}
	return (t);
}
/*****************************************************************************
** skip decimal or hex
*/
static char *skip(t)
char *t;
{
	while (*t != ',' && *t != '-' && *t != '\0' &&
	 	   *t>='0' && *t<='9' || *t>='a' && *t<='f' || *t>='A' && *t<='F')
		t++;
	return (t-1);
}
/*****************************************************************************
** if '=' then pointer +1
*/
static char *srq(t)
char *t;
{
	while (*t == '=' || *t == '$' && *t != '\0')
		t++;
	return (t);
}
/*----------------------------------------------------------------------------*/

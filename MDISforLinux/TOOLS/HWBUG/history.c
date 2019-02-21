/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: history.c
 *      Project: HwBug
 *
 *      $Author: rt $
 *        $Date: 2010/09/24 16:58:01 $
 *    $Revision: 1.3 $
 *
 *  Description: history functions
 *                      
 *                      
 *     Required: - 
 *     Switches: - 
 *
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *---------------------------------------------------------------------------
 * Copyright (c) 1993-2019, MEN Mikro Elektronik GmbH
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
 
static char RCSid[]="$Header: /dd2/CVSR/LINUX/TOOLS/HWBUG/history.c,v 1.3 2010/09/24 16:58:01 rt Exp $";
#include "hwbug.h"

#define HISTORY_SIZE	30		/* number of commands kept */

static char *histbuf[HISTORY_SIZE];
static char **current;
static char **setnext;

static char **_prev( p )
char **p;
{
    if( --p < histbuf )
    	p = &histbuf[HISTORY_SIZE-1];

    return p;
}        

static char **_next( p )
char **p;
{
	if( ++p > &histbuf[HISTORY_SIZE-1])
		p = histbuf;

	return p;
}


/*----------------------------------------------------------------------
 * hist_init - init history buffer
 */
void hist_init(void)
{
    /* include ident string, suppress warning */
    char a = a = RCSid[0];
	setnext = current = histbuf;
}

/*----------------------------------------------------------------------
 * hist_prev - get previous command
 * returns: previous command or NULL if none
 */
char *hist_prev(void)
{
	char **p;

	p = _prev( current );

	if( *p != 0L && p != setnext){
		current = p;
		return *current;
	}
	return 0L;
}

/*----------------------------------------------------------------------
 * hist_next - get next command
 * returns: next command or NULL if none
 */
char *hist_next(void)
{
	char **p = current;

	if( p == setnext ) return 0L;

	p = _next( p );
	
	current = p;
	return *current;
}

/*----------------------------------------------------------------------
 * hist_save - save a command in history buffer
 */
void hist_save( char *command )
{
	char **p = _prev(setnext);

	if( !*command || (*p && !strcmp( *p, command ))){
		current = setnext;
		return;					/* don't save identical commands */
	}
	if( *setnext )
		free( *setnext );		/* release buffer for old command */
	
	if( (*setnext = malloc(strlen(command)+1)) != NULL ) {
		strcpy(*setnext, command);
	}
	
	current = setnext = _next(setnext);
}	

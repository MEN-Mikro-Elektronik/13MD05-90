/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: dl_list.c
 *      Project: utility library
 *
 *      Author: popp 
 *
 *  Description: functions for double linked lists
 *               
 *     Required:  
 *     Switches: 
 *
 *---------------------------[ Public Functions ]----------------------------
 *  list *dl_newlist( l )
 *  node *dl_remove( n )
 *  node *dl_remhead( l )
 *  node *dl_addtail( l, n )
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

#include <MEN/dl_list.h>

/****************************************************************************
 *
 *  Description: Initialize list header (empty list)
 *                         
 *---------------------------------------------------------------------------
 *  Input......:  l - ptr to list structure 
 *  Output.....:  l - same as input
 *  Globals....:  ---
 ****************************************************************************/
list *dl_newlist(l)
register list *l;
{
	l->head = (node *)&l->tail;
	l->tailpred = (node *)&l->head;
	l->tail = (node *)0L;
	return l;
}

/****************************************************************************
 *
 *  Description:  remove a node from a list
 *                         
 *---------------------------------------------------------------------------
 *  Input......:  n - node to remove
 *  Output.....:  n - same as input
 *  Globals....:  ---
 ****************************************************************************/
node *dl_remove(n)
register node *n;
{
	n->next->prev = n->prev;
	n->prev->next = n->next;
	return n;
}

/****************************************************************************
 *
 *  Description: remove a node from the head of the list 
 *                         
 *---------------------------------------------------------------------------
 *  Input......:  l - ptr to list header
 *  Output.....:  n - removed node ( NULL if list was empty )
 *  Globals....:  ---
 ****************************************************************************/
node *dl_remhead(l)
register list *l;
{
	register node *n;
	if( l->head->next == 0L ) return 0L; /* list empty */

	n = l->head;
	l->head = n->next;
	n->next->prev = (node *)&(l->head);

	return n;
}

/****************************************************************************
 *
 *  Description:  add a node at the tail to the list
 *---------------------------------------------------------------------------
 *  Input......:  l - ptr to list functions
 *				  n - node to add
 *  Output.....:  n - added node
 *  Globals....:  ---
 ****************************************************************************/
node *dl_addtail(l,n)
register list *l;
register node *n;
{
	n->prev = l->tailpred;
	l->tailpred = n;
	n->next = n->prev->next;
	n->prev->next = n;
	return n;
}
	

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_dl_list.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  double linked list routines
 *
 *    \switches  -
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2000-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
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

#include "oss_intern.h"

/**********************************************************************/
/** Initialize list header
 * \copydoc oss_specification.c::OSS_DL_NewList()
 */
OSS_DL_LIST *OSS_DL_NewList( OSS_DL_LIST *l )
{
	l->head = (OSS_DL_NODE *)&l->tail;
	l->tailpred = (OSS_DL_NODE *)&l->head;
	l->tail = (OSS_DL_NODE *)NULL;
	return l;
}

/**********************************************************************/
/** Remove a node from a list
 * \copydoc oss_specification.c::OSS_DL_Remove()
 */
OSS_DL_NODE *OSS_DL_Remove( OSS_DL_NODE *n )
{
	n->next->prev = n->prev;
	n->prev->next = n->next;
	return n;
}

/**********************************************************************/
/** Remove a node from the head of the list
 * \copydoc oss_specification.c::OSS_DL_RemHead()
 */
OSS_DL_NODE *OSS_DL_RemHead( OSS_DL_LIST *l )
{
	OSS_DL_NODE *n;
	if( l->head->next == NULL ) return NULL; /* list empty */

	n = l->head;
	l->head = n->next;
	n->next->prev = (OSS_DL_NODE *)&(l->head);

	return n;
}

/**********************************************************************/
/** Add a node at tail of list
 * \copydoc oss_specification.c::OSS_DL_AddTail()
 */
OSS_DL_NODE *OSS_DL_AddTail( OSS_DL_LIST *l, OSS_DL_NODE *n )
{
	n->prev = l->tailpred;
	l->tailpred = n;
	n->next = n->prev->next;
	n->prev->next = n;
	return n;
}
	


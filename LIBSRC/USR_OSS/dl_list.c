/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  dl_list.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/06/06 09:28:29 $
 *    $Revision: 1.1 $
 *
 *  	 \brief  Double linked list routines
 *
 *	   \project  MDIS4Linux
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: dl_list.c,v $
 * Revision 1.1  2003/06/06 09:28:29  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "uos_int.h"

/**********************************************************************/
/** Initialize list header
 * \copydoc oss_specification.c::UOS_DL_NewList()
 */
UOS_DL_LIST *UOS_DL_NewList( UOS_DL_LIST *l )
{
	l->head = (UOS_DL_NODE *)&l->tail;
	l->tailpred = (UOS_DL_NODE *)&l->head;
	l->tail = (UOS_DL_NODE *)NULL;
	return l;
}

/**********************************************************************/
/** Remove a node from a list
 * \copydoc oss_specification.c::UOS_DL_Remove()
 */
UOS_DL_NODE *UOS_DL_Remove( UOS_DL_NODE *n )
{
	n->next->prev = n->prev;
	n->prev->next = n->next;
	return n;
}

/**********************************************************************/
/** Remove a node from the head of the list
 * \copydoc oss_specification.c::UOS_DL_RemHead()
 */
UOS_DL_NODE *UOS_DL_RemHead( UOS_DL_LIST *l )
{
	UOS_DL_NODE *n;
	if( l->head->next == NULL ) return NULL; /* list empty */

	n = l->head;
	l->head = n->next;
	n->next->prev = (UOS_DL_NODE *)&(l->head);

	return n;
}

/**********************************************************************/
/** Add a node at tail of list
 * \copydoc oss_specification.c::UOS_DL_AddTail()
 */
UOS_DL_NODE *UOS_DL_AddTail( UOS_DL_LIST *l, UOS_DL_NODE *n )
{
	n->prev = l->tailpred;
	l->tailpred = n;
	n->next = n->prev->next;
	n->prev->next = n;
	return n;
}
	

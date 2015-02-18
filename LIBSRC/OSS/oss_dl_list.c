/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_dl_list.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2005/07/07 17:17:12 $
 *    $Revision: 1.4 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  double linked list routines
 *
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_dl_list.c,v $
 * Revision 1.4  2005/07/07 17:17:12  cs
 * Copyright line changed
 *
 * Revision 1.3  2003/10/07 11:47:40  kp
 * fixed typo
 *
 * Revision 1.2  2003/04/11 16:13:17  kp
 * Comments changed for Doxygen
 *
 * Revision 1.1  2001/01/19 14:39:07  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

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
	


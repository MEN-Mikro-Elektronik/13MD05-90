/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  men_rtai_shirq.h
 *
 *      \author  thomas.schnuerer@men.de
 *        $Date: 2005/01/18 17:43:24 $
 *    $Revision: 2.2 $
 *
 *  	 \brief  definitions for the RTAI interrupt sharing module.
 * 	 			 The passed handler prototype is declared mainly to let
 * 				 MDIS_IrqHandler fit in it (in general normal Linux ISRs).
 * 				 For usage without MDIS a NULL may be passed for pt_regs.
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: men_rtai_shirq.h,v $
 * Revision 2.2  2005/01/18 17:43:24  ts
 * Added Header, removed one Function
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _MEN_RTAI_SHIRQ_H_
#define _MEN_RTAI_SHIRQ_H_

/* acquire an interrupt */
int SHIRQ_RequestSharedIrq( unsigned int irq,
							char *name,	 /* opt.name of handler/device*/
							void(*handler)( int,void *, struct pt_regs*),
							void *data);	

int SHIRQ_RemoveSharedIrq( 	unsigned int irq,
							void(*handler)(int, void *, struct pt_regs *),
							void *data);

#endif /*_MEN_RTAI_SHIRQ_H_*/


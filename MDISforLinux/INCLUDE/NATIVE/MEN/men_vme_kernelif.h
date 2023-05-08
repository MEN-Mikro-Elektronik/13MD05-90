/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: men_vme_kernelif.h
 *
 *       Author: kp
 *
 *  Description: header file to use MEN VME kernel interface
 *
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * Copyright 2000-2019, MEN Mikro Elektronik GmbH
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

#ifndef _MEN_VME_KERNELIF_H
#define _MEN_VME_KERNELIF_H

#ifdef __cplusplus
	extern "C" {
#endif
/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/* VME space codes */
#define VME_A16_SPACE	1
#define VME_A24_SPACE	2
#define VME_A32_SPACE	3
#define VME_CSR_SPACE	4

#define VME_D16_ACCESS	0x100
#define VME_D32_ACCESS  0x200

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

// rename a few functions due to conflict (see coht/vmebridge/include/vmebus.h)
#ifdef A25_WRAPPER
#define VME_REQUEST_IRQ		men_vme_request_irq
#define VME_FREE_IRQ		men_vme_free_irq
#else
#define VME_REQUEST_IRQ		vme_request_irq
#define VME_FREE_IRQ		vme_free_irq
#endif /* A25_WRAPPER */

extern int vme_bus_to_phys( int space, u32 vmeadrs, void **physadrs_p );
extern int VME_REQUEST_IRQ(unsigned int vme_irq,
			   void (*handler)(int, void * ),
			   unsigned long flags,
			   const char *device,
			   void *dev_id);

extern void VME_FREE_IRQ(unsigned int vme_irq, void *dev_id);
extern int vme_ilevel_control( int level, int enable );


#if 1 /*def CONFIG_MEN_VME_RTAI_KERNELIF */
int vme_rt_request_irq( unsigned int vme_irq,
						void (*handler)( int, void *, struct pt_regs *),
						const char *device,
						void *dev_id);

int vme_rt_free_irq(unsigned int vme_irq, void *dev_id );
#endif /* CONFIG_MEN_VME_RTAI_KERNELIF */


#ifdef __cplusplus
	}
#endif

#endif	/* _MEN_VME_KERNELIF_H */

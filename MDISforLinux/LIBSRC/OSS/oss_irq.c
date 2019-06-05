/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_irq.c
 *
 *      \author  klaus.popp@men.de
 *
 *	   \project  MDISforLinux
 *  	 \brief  Interrupt related routines
 *
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

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/
static int G_irqDisabled = 0;	/* nesting counter */
static unsigned long G_savedFlags;

#ifdef CONFIG_SMP
static int G_irqLockProcessorId; /* which CPU holds OSS_irqLock */
#endif

#if defined(DEFINE_SPINLOCK)
DEFINE_SPINLOCK(OSS_irqLock);
#else
spinlock_t OSS_irqLock = SPIN_LOCK_UNLOCKED;
#endif


/**********************************************************************/
/** Mask device interrupts.
 * \copydoc oss_specification.c::OSS_IrqMaskR()
 *
 * \linux Masks \b local processor interrupts. To be SMP safe,
 *        interrupt routine must also call OSS_IrqMaskR.
 *        \a irqHandle is not used and may be passed as NULL.
 *
 *
 * \sa OSS_IrqRestore()
 */
OSS_IRQ_STATE OSS_IrqMaskR( OSS_HANDLE *oss, OSS_IRQ_HANDLE* irqHandle )
{
	OSS_IRQ_STATE flags;

	DBGWRT_1((DBH,"OSS_IrqMaskR (Lin) irqDisable=%d spin_is_locked=%d\n",
			  G_irqDisabled, spin_is_locked(&OSS_irqLock)));

	/*
	 * 2.4.18 uniprocessor systems with preemption patch
	 * always return 1 for spin_trylock, even if it is already locked...
	 */
#ifdef CONFIG_SMP
	/*
	 * This special handling is needed to allow nested calls to
	 * OSS_IrqMaskR from the same CPU. 	
	 */
	local_irq_save( flags ); 	/* disable this CPU's interrupts */
	
	if( !spin_trylock( &OSS_irqLock ) ){
		/* check for nesting calls to OSS_IrqMaskR */
		if( smp_processor_id() != G_irqLockProcessorId ){
			/* lock is held by another processor; wait until he releases it */
			spin_lock( &OSS_irqLock );
			/* got the lock */
			G_irqLockProcessorId = smp_processor_id();
		}
		else {
			DBGWRT_3((DBH,"  OSS_IrqMaskR (Lin): Nested call from CPU %d\n",
					  smp_processor_id()));
			/* the same CPU had already the lock, should never happen
			   because of preemption disabling in spin_lock() */
		}
	} else {
		/* got the lock */
		G_irqLockProcessorId = smp_processor_id();
	}

#else
	/* not SMP */
	
	local_irq_save( flags );
	
	/* lock if possible;
	 * if not possible: our CPU already has the lock
	 * should never happen because spin_lock disables preemption
	 */
	spin_trylock( &OSS_irqLock );
	
#endif

	G_irqDisabled++;

	return flags;
}

/**********************************************************************/
/** Unmask device interrupts.
 * \copydoc oss_specification.c::OSS_IrqRestore()
 *
 * \sa OSS_IrqMaskR()
 */
void OSS_IrqRestore(
	OSS_HANDLE *oss,
	OSS_IRQ_HANDLE* irqHandle,
	OSS_IRQ_STATE oldState )
{
	DBGWRT_1((DBH,"OSS_IrqRestore (Lin)\n"));

	if( G_irqDisabled == 0 ){
		/* programmer's error */
		DBGWRT_ERR((DBH,"*** OSS_IrqRestore CPU %d: cannot restore Irqs. \n", smp_processor_id()));
		DBGWRT_ERR((DBH,"System is in illegal state.\n"));
		DBGWRT_ERR((DBH,"Did you call OSS_IrqRestore correspondent OSS_IrqMaskR? ***\n"));
		
		/* If this function is called with G_irqDisabled == 0, we
		 * have to pull emergency brake.
		 * Either the system/IRQ threads have overtaken each other,
		 * or the programmer has called the IrqMaskR and IrqRestore
		 * methods in a wrong or unsymmetrical way.
		 * Both cases can lead to unpredictable system results.
		 */
		
#ifdef CONFIG_X86
		// go in "safe state": endless loop (to enable debugger to plug in here)
		while ( 1 ) {
			// write pattern to port80 for debugging purpose
			outw( 0x55aa , 0x80 );		
		}
#else
		// go in "safe state": endless loop (to enable debugger to plug in here)
		while ( 1 ) ;
#endif		
		return;
	} // end error handling for G_irqDisabled counter check
	
	G_irqDisabled--;
	
#ifdef CONFIG_SMP
	if( G_irqDisabled == 0 ){
		if (spin_is_locked(&OSS_irqLock) ) {
			DBGWRT_3((DBH,"   OSS_IrqRestore (Lin) actually restore\n"));
			G_irqLockProcessorId = -1;
			spin_unlock_irqrestore( &OSS_irqLock, oldState );
		} else {
			DBGWRT_ERR((DBH,"*** OSS_IrqRestore CPU %d: spinlock on OSS_irqLock not set. \n", smp_processor_id()));
			DBGWRT_ERR((DBH,"System is in illegal state.\n"));
			
			/* spin not locked although the counter was > 0; that should never happen!
			 * Probably this will lead to unpredictable results.
			 * Therefore, we stop here with endless loop.
			 */

#ifdef CONFIG_X86	
		// go in "safe state": endless loop (to enable debugger to plug in here)
		while ( 1 ) {
			// write pattern to port80 for debugging purpose
			outw( 0x77aa , 0x80 );	
		}
#else
		// go in "safe state": endless loop (to enable debugger to plug in here)
		while (1) ;
#endif		 			 			
		} // end error handling for CONIFIG_SMP
		
	} // end (G_irqDisabled == 0)
	
#else
	// not SMP
	spin_unlock_irqrestore( &OSS_irqLock, oldState );
#endif

}

/**********************************************************************/
/** Mask device interrupts (old implementation).
 * \copydoc oss_specification.c::OSS_IrqMask()
 *
 * \linux Masks \b all processor interrupts
 *
 * \sa OSS_IrqUnMask()
 */
void OSS_IrqMask( OSS_HANDLE *oss, OSS_IRQ_HANDLE* irqHandle )
{
	u_int32 flags;

	DBGWRT_1((DBH,"OSS_IrqMask (Lin)\n"));

	flags = OSS_IrqMaskR( oss, irqHandle );

	if( G_irqDisabled == 1 )
		G_savedFlags = flags;

}/*OSS_IrqMask*/

/**********************************************************************/
/** Unmask device interrupts (old implementation).
 * \copydoc oss_specification.c::OSS_IrqUnMask()
 *
 * \sa OSS_IrqMask()
 */
void OSS_IrqUnMask( OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle )
{
	DBGWRT_1((DBH,"OSS_IrqUnMask (Lin)\n"));

	if( G_irqDisabled == 1 )
		OSS_IrqRestore( oss, irqHandle, G_savedFlags );
}/*OSS_IrqUnMask*/

/**********************************************************************/
/** Compute interrupt vector from interrupt number and bus type.
 * \copydoc oss_specification.c::OSS_IrqLevelToVector()
 */
int32 OSS_IrqLevelToVector(
	OSS_HANDLE *oss,
	int32      busType,
	int32      irqNbr,
	int32      *vectorP
)
{
    DBGCMD( static const char functionName[] = "OSS_IrqLevelToVector"; )

	DBGWRT_1((DBH,"%s\n", functionName));

	switch( busType ){
	case OSS_BUSTYPE_PCI:
	case OSS_BUSTYPE_ISA:
	case OSS_BUSTYPE_MSI:
	case OSS_BUSTYPE_ISAPNP:
		*vectorP = irqNbr;		/* ts: under Linux, level = vector. Its a numeric Value */
		break;
	default:
        DBGWRT_ERR(( DBH, "*** %s: busType %ld not supported\n",
                      functionName, busType ));
		return( ERR_OSS_ILL_PARAM );
	}/*if*/

	DBGWRT_2((DBH," vector=%ld\n", *vectorP ));
	return( 0 );
}/*OSS_IrqLevelToVector*/







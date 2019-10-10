/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_specification.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2011/02/24 15:50:08 $
 *    $Revision: 1.18 $
 * 
 *  	 \brief  MEN OSS (Operating System Services) module specification.
 *
 * This file contains the common, platform independent specification for
 * the OSS module.
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_specification.c,v $
 * Revision 1.18  2011/02/24 15:50:08  dpfeuffer
 * R: OSS_SpinLockAcquire/Release must not be called with IRQ priority
 * M: function specification changed
 *
 * Revision 1.17  2010/11/29 16:10:16  dpfeuffer
 * R: OSS_SpinLockXxx function introduction
 * M: OSS_SpinLockXxx description added
 *
 * Revision 1.16  2008/12/18 14:19:44  CKauntz
 * R: Page description is empty
 * M: + Added page description and removed section to refer to
 *      page instead of section
 *    + Changed ossdllusagesect to ossdllusage
 *    + Changed ossalarmusagesect to ossalarmusage
 *    + Changed osssemusagesect to osssemusage
 *    + Changed osssigusagesect to osssigusage
 *
 * Revision 1.15  2008/10/16 14:36:59  ufranke
 * OSS_MaskR()/OSS_Mask() restrictions improved
 *
 * Revision 1.14  2008/10/07 14:20:11  dpfeuffer
 * cosmetics
 *
 * Revision 1.13  2008/08/29 08:11:17  dpfeuffer
 * cosmetics
 *
 * Revision 1.12  2008/05/29 14:47:54  CKauntz
 * Added OSS_Swap64 and OSS_SWAP64
 * Fixed function parameters names for doxygen documentation
 *
 * Revision 1.11  2008/05/27 13:34:04  dpfeuffer
 * OSS_MapVmeAddr(), OSS_UnMapVmeAddr(): function description fixed
 *
 * Revision 1.10  2008/05/20 10:25:08  dpfeuffer
 * OSS_SigRemove(), OSS_GetSmbHdl: function description fixed
 *
 * Revision 1.9  2006/12/05 11:22:13  ufranke
 * added SMP support
 *  + note to OSS_IrqMaskR()/OSS_IrqRestore()
 *    multiprocessor support - spin lock
 *
 * Revision 1.8  2005/11/25 10:18:06  dpfeuffer
 * added OSS_GetSmbHdl(), OSS_SetSmbHdl()
 *
 * Revision 1.7  2005/07/08 11:32:48  cs
 * cosmetics
 *
 * Revision 1.6  2004/06/18 13:56:00  dpfeuffer
 * OSS_MapVmeAddr(), OSS_UnMapVmeAddr added
 *
 * Revision 1.5  2004/06/09 09:24:40  kp
 * added note to OSS_IrqMaskR that it must support nested calls
 *
 * Revision 1.4  2004/05/25 14:08:40  ub
 * Added info about usage in IRQ/Alarm routines.
 *
 * Revision 1.3  2003/08/04 12:42:02  kp
 * added missing OSS_Ident()
 *
 * Revision 1.2  2003/05/09 12:24:39  kp
 * added OSS_AlarmMask/OSS_AlarmRestore
 *
 * Revision 1.1  2003/04/11 16:13:02  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

/*! \page osscommonspec  Common OSS specificiation "MDIS4/2003-0"

 The MEN OSS module (Operating System Services) is an abstraction of
 the services provided by the operating system. 

 OSS is mainly used in MENs Driver Interface System (MDIS) but can be used
 also in other contexts. In general, OSS is usable only in kernel mode
 of the OS (if the OS has a kernel mode). 
 
 Most OSS implementations provide a routine to initialize and
 terminate the OSS library: OSS_Init() and OSS_Exit(). OSS_Init() will
 return the \em OSS \em handle that must be passed to all other OSS
 functions. If not OSS_Init() exists for your OS, pass \c NULL as the
 OSS handle.

 The common OSS specification provides the following functional groups. 

 - \b Memory allocation: OSS_MemGet(), OSS_MemFree()
 - \b Interrupt related: OSS_IrqMaskR(), OSS_IrqRestore()
 - \b SpinLock related: OSS_SpinLockCreate(), OSS_SpinLockRemove(), OSS_SpinLockAcquire(), OSS_SpinLockRelease()
 - \b Time related: OSS_Delay(),  OSS_MikroDelay(), 
   OSS_TickRateGet(), OSS_TickGet()
 - \ref osssemusage "Semaphores": OSS_SemCreate(), OSS_SemRemove(), 
   OSS_SemWait(), OSS_SemSignal()
 - \ref osssigusage "Signals": OSS_SigCreate(), OSS_SigRemove(), 
   OSS_SigSend(), OSS_SigInfo()
 - \ref ossalarmusage "Alarms": OSS_AlarmCreate(), OSS_AlarmRemove(), 
   OSS_AlarmSet(), OSS_AlarmClear(), OSS_AlarmMask(), OSS_AlarmRestore()
 - \b C-library substitutes: OSS_Sprintf(), OSS_Vsprintf(), OSS_StrLen(),
   OSS_StrCpy(), OSS_StrCmp(), OSS_StrNcmp(), OSS_StrTok(), OSS_MemCopy(),
   OSS_MemFill()
 - \b PCI access: OSS_PciGetConfig(), OSS_PciSetConfig()
 - \ref ossdllusage "Lists": OSS_DL_NewList(), OSS_DL_Remove(), 
   OSS_DL_AddTail(), OSS_DL_RemHead()
 - \b Swapping: OSS_Swap16(), OSS_Swap32(), OSS_Swap64(), OSS_SWAP16(), 
   OSS_SWAP32(), OSS_SWAP64()
 - \b Debugging: OSS_DbgLevelSet(), OSS_DbgLevelGet()
 - \b Version info: OSS_Ident()
 
 The following functions should not be used by MDIS low level drivers, but 
 can be used by the MDIS/BBIS kernel, BBIS drivers and other drivers:

 - \b Interrupt translation: OSS_IrqLevelToVector()
 - \b Time related: OSS_MikroDelayInit()
 - \b Bus translation and mapping: OSS_BusToPhysAddr(), 
      OSS_MapPhysToVirtAddr(), OSS_AssignResources(), OSS_UnAssignResources(),
	  OSS_UnMapVirtAddr()
 - \b PCI specific:  OSS_PciSlotToPciDevice()
 - \b VMEbus specific:  OSS_MapVmeAddr(), OSS_UnMapVmeAddr
 - \b ISA bus specific: OSS_IsaGetConfig()

*/
/**********************************************************************/
/** Returns the identification string (Version info) of the OSS module
 */
char* OSS_Ident( void ){}

/**********************************************************************/
/** Set debug level of OSS instance.
 * 
 * Sets the debug level of this OSS instance. Has no effect if the
 * module is not compiled with \c DBG switch set.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param newLevel	\IN new debug level mask, a combination of the 
 *						\c DBG_xxx constants from \em MEN/dbg.h, e.g.
 *						\c DBG_ALL, DBG_LEV1...
 */
void OSS_DbgLevelSet( OSS_HANDLE *oss, u_int32 newLevel ){}

/**********************************************************************/
/** Get debug level of OSS instance.
 * 
 * gets the debug level of this OSS instance.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \return current debug level mask
 */
u_int32 OSS_DbgLevelGet( OSS_HANDLE *oss ){}




/**********************************************************************/
/** Allocates general memory block.	
 *
 * OSS_MemGet() will allocate a memory block with at least the
 * requested \a size. Depending on the operating system the actual
 * block may be larger; this size is returned in \a gotsizeP. When the
 * block is returned to free system memory you must return the actual
 * size, not the requested size. The return value of OSS_MemGet() is
 * a pointer to the first byte of the memory block or \c NULL if
 * failed. 
 *
 * The memory block allocated has no special attributes, especially it
 * is not guaranteed to be physically contignous (if the system uses
 * virtual addresses) and it is also not safe to use it for DMA.
 * However each OSS implementation guarantees that the allocated
 * memory block is 4 byte aligned.
 *
 * Typical usage of the routines:
 *
 * \code
 *   void *block;
 *  u_int32 gotsize;
 *
 *  // Allocate 256 bytes of memory
 *  block = OSS_MemGet( llHdl->osHdl, 256, &gotsize );
 *
 *  if( block == NULL ) 
 *      return ERR_OSS_MEM_ALLOC;
 *  // Here the memory can be used
 *   ....
 *  // free the memory
 *  OSS_MemFree( llHdl->osHdl, block, gotsize );
 * \endcode
 *
 * \remark OSS_MemGet() should not be used in IRQ or Alarm routines.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param size 		\IN number of bytes to allocate
 * \param gotsizeP  \OUT actual number of bytes allocated
 * \return pointer to memory block or \c NULL if memory could not be allocated
 *
 */
extern void *OSS_MemGet(
    OSS_HANDLE *oss,
    u_int32    size,
    u_int32    *gotsizeP){}

/**********************************************************************/
/** Return memory block.	
 *
 * Returns a memory block previously allocated by OSS_MemGet().
 * 
 * \remark The result is undefined if an attempt is made to free a block
 * not previously allocated or with a different size.
 *
 * \remark OSS_MemFree() should not be used in IRQ or Alarm routines.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param addr		\IN address of memory block
 * \param size		\IN number of bytes to free
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 */
int32 OSS_MemFree(
    OSS_HANDLE *oss,
    void       *addr,
	u_int32    size){}


/**********************************************************************/
/** Check if memory block accessible by caller
 *
 * This function checks if the memory block starting at \a addr can
 * be accessed by calling routine/process with the attributes specified
 * with \a mode.
 *
 * This function is typically used to verify that an address in user
 * land is really owned by the calling process to avoid that the kernel
 * mode driver crashes the system due to incorrectly passed addresses.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param addr		\IN address of memory block to check
 * \param size		\IN number of bytes to check starting at \a addr
 * \param mode		\IN combination of \c OSS_READ ORed with \c OSS_WRITE
 * \return 0 on success or \c ERR_OSS_NO_PERM if no permission
 */
int32 OSS_MemChk(
    OSS_HANDLE *oss,
    void       *addr,
    u_int32    size,
	int32      mode){}



/*! \page ossalarmusage Gernal Usage of OSS Alarms

  Alarm routines are kernel functions which are triggered cyclically
  or once with a defined period.  Alarm routines are typically
  executed in interrupt context or from an high priority task.

  An alarm routine is installed from the driver where an argument for
  the routine can be defined.

  The alarm routine is passed the installed application argument.

  Alarm routines are identified via a alarm handle, which is
  created by the OSS_AlaramCreate() function. All subsequent calls refer
  to this handle.

  The number of installable alarm routines is not limited.

  Typical usage:

  \code
      // Driver INIT:
         OSS_AlarmCreate();
      // Driver EXIT:
         OSS_AlarmRemove();
      //Driver GETSTAT/SETSTAT/WRITE/READ:
         OSS_AlarmSet();
         ...
         OSS_AlarmClear();

      // Handler:	  
	  AlarmHandler()
	  {
	  }
  \endcode
  
  \subsection Preventing Race conditions

  From the drivers main routines, alarms can be temporarily disabled by
  calling OSS_AlarmMask(), to prevent concurrent access to shared data
  used both by the main routines and alarm handler. After finishing
  the critical code, call OSS_AlarmRestore().
  
  The time for which alarms are masked shall be kept as short as
  possible (since in most implementations all interrupts are
  masked). Furthermore, while alarms are masked, only call (OSS)
  functions that are allowed to be called from interrupt routines.

  If an alarm would be triggered while alarms are masked, execution 
  of the alarm handler is delayed until OSS_AlarmRestore() is called.

  In some implementations, device interrupts may still occurr while alarms
  are masked.
*/



/**********************************************************************/
/** Create an alarm.
 * 
 * Creates the alarm handle, but does not trigger the alarm. To start
 * the cyclic or one-shot alarm, use OSS_AlarmSet().
 *
 * Function \a funct is installed as alarm routine, which can be
 * activated by a subsequent OSS_AlarmSet() call:
 *
 * \code
 * void funct(void *arg)
 * \endcode
 *
 * Argument \a arg is passed to the alarm routine.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param funct			\IN function to be called when alarm is fired
 * \param arg			\IN argument to pass to \a func
 * \param alarmP		\OUT will receive created alarm handle. Must be passed
 *							 to further OSS_AlarmXXX() routines.
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 */
int32 OSS_AlarmCreate(
    OSS_HANDLE       *oss,
    void             (*funct)(void *arg),
	void             *arg,
	OSS_ALARM_HANDLE **alarmP)
{}


/**********************************************************************/
/** Destroys alarm handle.
 * 
 * Frees all resources allocated by OSS_AlarmCreate(). The alarm 
 * is automatically deactivated if it was pending.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param alarmP		\IN pointer to alarm handle to remove
 *						\OUT *alarmP set to NULL
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 */
int32 OSS_AlarmRemove(
    OSS_HANDLE       *oss,
	OSS_ALARM_HANDLE **alarmP){}


/**********************************************************************/
/** Activate an installed alarm routine
 *
 * This routine activates an alarm created with OSS_AlarmCreate(). 
 *
 * The installed alarm routine is is called depending on the \a cyclic
 * parameter:
 *  - \a cyclic==1: cyclically with a period of \em msec
 *  - \a cyclic==0: once after \em msec.
 * 
 * Alarm time may be rounded up to the systems minimum ticker
 * resolution. This (rounded) time is retured in \a *realMsecP.
 *
 * \remark The result is undefined if \a msec is passed as 0.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param alarmP		\IN alarm handle, returned by OSS_AlarmCreate()
 * \param msec			\IN alarm time in milliseconds
 * \param cyclic		\IN alarm mode: 0=single, 1=cyclic
 * \param realMsecP		\OUT rounded alarm time [msec]
 * 
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_AlarmSet(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE *alarmP,
    u_int32          msec,
    u_int32          cyclic,
    u_int32          *realMsecP){}

/**********************************************************************/
/** Deactivate an installed alarm routine
 *
 * This routine deactivates an alarm previously activated with
 * OSS_AlarmSet(). Can be used to shut down a cyclic alarm or an alarm
 * in single mode if not yet fired.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param alarmP		\IN alarm handle, returned by OSS_AlarmCreate()
 * 
 * \return 0 on success or \c ERR_OSS_xxx error code on error:
 *  - \c ERR_OSS_ALARM_CLR - if alarm not active
 */
int32 OSS_AlarmClear(
    OSS_HANDLE       *oss,
	OSS_ALARM_HANDLE *alarmP){}

/**********************************************************************/
/** Mask alarms
 *
 * This routine temporarily masks \b all alarms to occur until
 * OSS_AlarmRestore() is called. On most platforms, this routine
 * does essentially the same as OSS_IrqMaskR().
 *
 * The return value of OSS_AlarmMask() is the system's (alarm) state
 * before OSS_AlarmMask() has been called. \c OSS_ALARM_STATE is a
 * system specific type that reflects if alarms are masked (or not).
 * The returned value has to be passed to OSS_AlarmRestore().
 *
 * Example alarm masking:
 * \code
 *  OSS_ALARM_STATE almState;
 *
 *  almState = OSS_AlarmMask( osh );
 *
 *  // critical code here
 *
 *  OSS_AlarmRestore( osh, almState );  
 * \endcode
 *
 * \remark This function has been added in MDIS4/2003
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \return reflects alarm enabled state before alarm is masked.
 */
OSS_ALARM_STATE OSS_AlarmMask( OSS_HANDLE *oss ){}

/**********************************************************************/
/** Unmask alarms.
 *
 * Must be called to restore the alarm enable state to the state
 * before OSS_AlarmMask() has been called.
 *
 * \remark This function has been added in MDIS4/2003
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param oldState		\IN value returned by OSS_AlarmMask().
 */
void OSS_AlarmRestore( OSS_HANDLE *oss, OSS_ALARM_STATE oldState ){}

/*---- Double linked lists ----*/

/*! \page ossdllusage Using OSS Double Linked Lists

   OSS provides a limited set of routines to maintain double linked
  lists. Functionality has been cloned from AMIGA ROM kernel.

  Generally you need to declare and initialize a list header:
  \code
    OSS_DL_LIST mylist;
	
	OSS_DL_NewList( &mylist );
  \endcode

  Then you will have typically dynamically allocated objects that
  you want to maintain:

  \code
    typedef struct {
	   OSS_DL_NODE n;
	   int a, b, c; 
	} MYOBJ;

	MYOBJ objA = (MYOBJ *)OSS_MemGet( oss, sizeof(MYOBJ), &gotSize );
	MYOBJ objB = (MYOBJ *)OSS_MemGet( oss, sizeof(MYOBJ), &gotSize );
	MYOBJ objC = (MYOBJ *)OSS_MemGet( oss, sizeof(MYOBJ), &gotSize );
  \endcode

  Then, you can add your objects to the end of the list:
  \code
    OSS_DL_AddTail( &mylist, &objA.n );
    OSS_DL_AddTail( &mylist, &objB.n );
    OSS_DL_AddTail( &mylist, &objC.n );
  \endcode

  To scan the list, use that code:
  \code
	MYOBJ *obj;
	for( obj=(MYOBJ *)mylist.head; 
		 obj->n.next;
		 obj = (MYOBJ *)obj->n.next ){
		 // do something
    }
  \endcode

  To remove a given object from the list:
  \code
    OSS_DL_Remove( &obj.n );
  \endcode

  To remove an object from the head of list (to implement a FIFO):
  \code
    obj = (MYOBJ *)OSS_DL_RemHead( &mylist );
  \endcode
*/

/**********************************************************************/
/** Initialize list header
 *
 * Mark list as empty
 *
 * See \ref ossdllusage for more info.
 *
 * \param l 			 \IN pointer to list structure 
 * \return l
 * \sa OSS_DL_Remove, OSS_DL_RemHead, OSS_DL_AddTail
 */
OSS_DL_LIST *OSS_DL_NewList( OSS_DL_LIST *l ){}

/**********************************************************************/
/** Remove a node from a list
 *
 * See \ref ossdllusage for more info.
 *
 * \param n 			 \IN node to remove
 * \return n
 * \sa OSS_DL_NewList, OSS_DL_RemHead, OSS_DL_AddTail
 */
OSS_DL_NODE *OSS_DL_Remove( OSS_DL_NODE *n ){}

/**********************************************************************/
/** Remove a node from the head of the list 
 *
 * See \ref ossdllusage for more info.
 *
 * \param l 			 \IN pointer to list header
 * \return removed node or NULL if list was empty
 * \sa OSS_DL_NewList, OSS_DL_Remove, OSS_DL_AddTail
 */
OSS_DL_NODE *OSS_DL_RemHead( OSS_DL_LIST *l ){}

/**********************************************************************/
/** Add a node at tail of list
 *
 * See \ref ossdllusage for more info.
 *
 * \param l 			 \IN pointer to list header
 * \param n 			 \IN node to add
 * \return n
 * \sa OSS_DL_NewList, OSS_DL_Remove, OSS_DL_RemHead
 */
OSS_DL_NODE *OSS_DL_AddTail( OSS_DL_LIST *l, OSS_DL_NODE *n ){}

/*---- IRQ routines ----*/


/**********************************************************************/
/** Mask device interrupts.
 *
 * This is the revised version of OSS_IrqMask().
 *
 * This routine masks the processor interrupts. Depending on the OS
 * and platform, it masks either all interrupts or only the interrupt
 * level at which the device would interrupt. For example, under
 * OS-9/68k, when the device interrupts at IRQ level 4,
 * OSS_IrqMask() would mask IRQ levels 1..4, but level 5..7
 * interrupts could still occur. 
 *
 * Other implementations mask all processor interrupts.
 *
 * To indicate which interrupt level to mask, this routine
 * uses the \a irqHandle structure, which is filled typically
 * by an \em OSS_IrqInit() routine. This routine does not exist
 * on all implementations. If it does not exist, you can pass \c NULL
 * to \a irqHandle.
 *
 * To re-enable processor interrupts, call OSS_IrqRestore().
 *
 * The return value of OSS_IrqMaskR() is the system's IRQ state before
 * OSS_IrqMaskR() has been called. \c OSS_IRQ_STATE is a system specific
 * type that reflects the interrupt state of the system. The returned
 * value has to be passed to OSS_IrqRestore().
 *
 * Implementations must make sure that OSS_IrqMaskR() can be called from
 * any context, include from within interrupt and alarm contexts. Nesting 
 * calls to OSS_IrqMaskR()/OSS_IrqRestore() must be also supported
 *
 * For multiprocessor environments spin lock will be taken by OSS_IrqMaskR().
 *
 * With locked interrupts/ spinlocks no system/OSS calls are allowed with the 
 * exception of:
 *    OSS_MikroDelay()
 *    OSS_MemCopy()
 *    OSS_MemFill()
 *
 * In general critical sections protected by OSS_IrqMaskR() must be as short
 * as possible.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param irqHandle		\IN IRQ handle, returned by OSS_IrqInit() or
 *							\c NULL if no OSS_IrqInit() exists.
 * \return reflects IRQ state before interrupt is masked.
 */
OSS_IRQ_STATE OSS_IrqMaskR( OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle ){}

/**********************************************************************/
/** Unmask device interrupts.
 *
 * Revised version of OSS_IrqUnMask().
 *
 * Must be called to restore the interrupt flag/level to the state
 * before OSS_IrqMaskR() has been called.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param irqHandle		\IN IRQ handle, returned by OSS_IrqInit() or
 *							\c NULL if no OSS_IrqInit() exists.
 * \param oldState		\IN value returned by OSS_IrqMaskR().
 */
void OSS_IrqRestore( 
	OSS_HANDLE *oss, 
	OSS_IRQ_HANDLE *irqHandle, 
	OSS_IRQ_STATE oldState ){}

/**********************************************************************/
/** Mask device interrupts (old implementation).
 *
 * This routine masks the processor interrupts. Depending on the OS
 * and platform, it masks either all interrupts or only the interrupt
 * level at which the device would interrupt. For example, under
 * OS-9/68k, when the device interrupts at IRQ level 4,
 * OSS_IrqMask() would mask IRQ levels 1..4, but level 5..7
 * interrupts could still occur. 
 *
 * Other implementations mask all processor interrupts.
 *
 * To indicate which interrupt level to mask, this routine
 * uses the \a irqHandle structure, which is filled typically
 * by an \em OSS_IrqInit() routine. This routine does not exist
 * on all implementations. If it does not exist, you can pass \c NULL
 * to \a irqHandle.
 *
 * To re-enable processor interrupts, call OSS_IrqUnMask().
 *
 * This routine should keep track of the nesting level. If
 * OSS_IrqMask() has been called already without calling
 * OSS_IrqUnMask() in between, it shall ignore the second call.
 * 
 * For multiprocessor environments spin lock will be released by OSS_IrqMask().
 * 
 * With locked interrupts/ spinlocks no system/OSS calls are allowed with the 
 * exception of:
 *    OSS_MikroDelay()
 *    OSS_MemCopy()
 *    OSS_MemFill()
 *
 * In general critical sections protected by OSS_IrqMask() must be as short
 * as possible.
 *
 * \deprecated Due to the nesting problems, this routine shall no longer
 * be used. Use OSS_IrqMaskR() / OSS_IrqRestore() instead.
 * 
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param irqHandle		\IN IRQ handle, returned by OSS_IrqInit() or
 *							\c NULL if no OSS_IrqInit() exists.
 */
void OSS_IrqMask( OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle ){}

/**********************************************************************/
/** Unmask device interrupts (old implementation).
 *
 * Must be called to restore the interrupt flag/level to the state
 * before OSS_IrqMask() has been called.
 *
 * \deprecated Due to the nesting problems, this routine shall no longer
 * be used. Use OSS_IrqMaskR() / OSS_IrqRestore() instead.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param irqHandle		\IN IRQ handle, returned by OSS_IrqInit() or
 *							\c NULL if no OSS_IrqInit() exists.
 *
 */
void OSS_IrqUnMask( OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle ){}


/**********************************************************************/
/** Compute interrupt vector from interrupt number and bus type.
 *
 * Used by BBIS handler to translate an interrupt level or interrupt
 * number into an interrupt vector. 
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busType		\IN \c OSS_BUSTYPE_xxx
 * \param irqNbr		\IN interrupt number/level to translate
 * \param vectorP		\OUT will contain translated vector
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_IrqLevelToVector(
	OSS_HANDLE *oss,
	int32      busType,
	int32      irqNbr,
	int32      *vectorP	){}

/**********************************************************************/
/** Create a spin lock.
 *
 * This routine must be called before any other OSS_SpinLockXXX()
 * routines.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param spinlP	\OUT will receive created spin lock handle. Must be 
 *					     passed to further OSS_SpinLockXXX() routines.
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SpinLockCreate( OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP ){}

/**********************************************************************/
/** Destroy spin lock handle.
 *
 * Frees all resources allocated by OSS_SpinLockCreate(). 
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param spinlP	\IN pointer to spin lock handle to remove
 *					\OUT *spinlP set to NULL
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SpinLockRemove( OSS_HANDLE *oss, OSS_SPINL_HANDLE **spinlP ){}

/**********************************************************************/
/** Acquire spin lock.
 *
 * The method spins until the specified spin lock has been acquired.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param spinl		\IN spin lock handle, returned by OSS_SpinLockCreate()
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SpinLockAcquire( OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl ){}

/**********************************************************************/
/** Release spin lock.
 *
 * The method releases the spin lock gotten by previously calling
 * OSS_SpinLockAcquire.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 		\IN OSS handle, returned by OSS_Init()
 * \param spinl		\IN spin lock handle, returned by OSS_SpinLockCreate()
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SpinLockRelease( OSS_HANDLE *oss, OSS_SPINL_HANDLE *spinl ){}

/*---- Semaphore routines ----*/

/*! \page osssemusage Using OSS Semaphores

  Semaphores are used for process synchronisation and/or to lock
  unsharable resources.
   
  There are two types of semaphores available: binary and counting
  semaphores.

  Both types of semaphore have the following characteristics:
   - OSS_SemCreate sets the semaphore value to it's inital value
   - OSS_SemWait waits until value>0 and decrements value
   - OSS_SemSignal increments value (and wakes waiting process)
   - A semaphore is locked when value=0
   - A semaphore is released when value>0
   - A semaphore value never becomes negative

   A \b binary semaphore (\c OSS_SEM_BIN) can have values 0 or 1:
   - The semaphore remembers if it was signalled

   A \b counting semaphore (\c OSS_SEM_COUNT) can have values 0..0x7FFFFFFF:
   - The semaphore remembers how many times it was signalled
                     
   Examples:
   -# Using binary semaphore for process synchronisation:
      - The semaphore is created with initial value=0
      - A process calls SemWait to wait for an event
      - The ISR calls SemSignal when the event has been occured
   -# Using binary semaphore to lock unsharable resources:
      - The semaphore is created with initial value=1
      - A process calls SemWait to lock a resource
      - The process uses the resource
      - The process calls SewSignal to release the resource
                     
   Semaphores are identified via a semaphore handle, which is created
   by OSS_SemCreate(). All subsequent calls refer to this handle.

   The number of installable semaphores is not limited.

   Typical usage:

  \code
      // Driver INIT:
          OSS_SemCreate();
      // Driver EXIT:
          OSS_SemRemove();
      // Driver (any call):
          OSS_SemWait();
      // Driver IRQ:
          OSS_SemSignal();
  \endcode
*/


/**********************************************************************/
/** Create a semaphore.
 * 
 * Creates a binary or counting semaphore. 
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param semType		\IN semaphore type 
 *							- \c OSS_SEM_BIN:    binary semaphore 
 *							- \c OSS_SEM_COUNT:  counting semaphore
 * \param initVal  		\IN initial semaphore value
 *                        - 0..1   (for \c OSS_SEM_BIN)
 *                        - 0..+n  (for \c OSS_SEM_COUNT)
 * \param semP			\OUT will receive created semaphore handle. Must be 
 *							 passed to further OSS_SemXXX() routines.
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SemCreate(
    OSS_HANDLE 	   *oss,
    int32          semType,
    int32          initVal,
	OSS_SEM_HANDLE **semP){}

/**********************************************************************/
/** Destroy semaphore handle.
 * 
 * Frees all resources allocated by OSS_SemCreate(). 
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param semP			\IN pointer to semaphore handle to remove
 *						\OUT *semP set to NULL
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 */
int32 OSS_SemRemove(
    OSS_HANDLE *oss,
	OSS_SEM_HANDLE **semP){}

/**********************************************************************/
/** Wait for semaphore.
 * 
 * The function waits with a given timeout until the semaphore value
 * is larger than zero and decrements the event value.
 *
 * Error \c ERR_OSS_TIMEOUT is returned if the semaphore was not released
 * within the timeout period.
 *
 * Error \c ERR_OSS_TIMEOUT is returned if \c OSS_SEM_NOWAIT specified and
 * semaphore was not available.
 *
 * Error \c ERR_OSS_SIG_OCCURED is returned if a deadly signal occured
 * while waiting. 
 *
 * \remark Only some OSes may abort a semaphore wait with a signal, 
 * currently OS-9 and Linux.
 *
 * Given timeout \a msec will be rounded up to system ticks
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param sem			\IN semaphore handle, returned by OSS_SemCreate()
 * \param msec			\IN wait timeout:
 *							- 0  no wait, just take sem (\c OSS_SEM_NOWAIT)
 *							- -1  endless timeout  (\c OSS_SEM_WAITFOREVER)
 *							- >0  timeout [msec]
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SemWait(
    OSS_HANDLE      *oss,
    OSS_SEM_HANDLE  *sem,
	int32           msec){}

/**********************************************************************/
/** Signal semaphore.
 *
 * The function increments the semaphores value by \c 1 and wakes up
 * the next queued process waiting for the event.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param sem			\IN semaphore handle, returned by OSS_SemCreate()
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SemSignal(
    OSS_HANDLE *oss,
	OSS_SEM_HANDLE *sem){}


/*---- Signal routines ----*/

/*! \page osssigusage Using OSS Signals

  Signals are used to notify processes/tasks in user land about
  asynchronous event inside (MDIS) drivers. This releases an
  application from periodically polling for that event.

  Before a signal can be sent by the driver to the application, the
  signal condition must have been activated by the application. This
  is normally done using a \em M_setstat call. Each driver can
  define any number of signal conditions appropriate for that device.

  The driver's interrupt or alarm routine will check the installed
  conditions and -- if the condition is satisfied -- send the signal
  to the process that installed it. When the application process
  receives the signal, it invokes a signal handler routine,
  asynchronous to the processes current execution thread. Therefore,
  from the application process point of view, a signal handler can be
  compared with an interrupt routine, because it interrupts the
  program's normal execution flow.

  Not all OSes that run MDIS natively support signals, only the UNIX
  like operating systems know the signal concept. On the other systems
  (RTAI, Windows and Ardence RTX), signals are emulated with some
  other interprocess communication method and a dedicated thread that
  invokes the signal handler.

  To allow application processes to be written OS independently, MDIS
  provides common API routines in the \em USR_OSS library that hides
  the implementation details from the application. 

  Typical usage:

  \code
      // Driver (any call):
          OSS_SigCreate();
      // Driver IRQ:
          OSS_SigSend();
      // Driver (any call):
          OSS_SigRemove();
  \endcode
*/  

/**********************************************************************/
/** Create signal handle.
 * 
 * This creates an OS specific handle to store an identifier for the 
 * calling task. To send the signal, use OSS_SigSend().
 *
 * This routine must be called in the context of the process/task 
 * that wishes to receive the signal.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param signal		\IN signal code to install
 * \param sigP			\OUT will receive created signal handle. Must be 
 *							 passed to further OSS_SigXXX() routines.
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SigCreate(
    OSS_HANDLE       *oss,
    int32            signal,
	OSS_SIG_HANDLE   **sigP){}


/**********************************************************************/
/** Destroy signal handle.
 * 
 * Frees all resources allocated by OSS_SigCreate(). The function
 * fails if the calling process id is not equal to the installing
 * process.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param sigP			\IN pointer to signal handle to remove
 *						\OUT *sigP set to NULL
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 */
int32 OSS_SigRemove(
    OSS_HANDLE     *oss,
	OSS_SIG_HANDLE **sigP){}

/**********************************************************************/
/** Send signal to a process
 * 
 * Sends the signal previously created by OSS_SigCreate() to the 
 * process that called OSS_SigCreate().
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param sig			\IN signal handle, returned by OSS_SigCreate()
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_SigSend(
    OSS_HANDLE *oss,
	OSS_SIG_HANDLE* sig){}

/**********************************************************************/
/** Get info about signal
 *
 * \deprecated Don't use it. Not all OSes have a process ID.
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param sig			\IN signal handle, returned by OSS_SigCreate()
 * \param signalP		\OUT signal code
 * \param pidP			\OUT process id
 */
int32 OSS_SigInfo(
    OSS_HANDLE     *oss,
    OSS_SIG_HANDLE *sig,
    int32          *signalP,
	int32          *pidP){}
 
/*--- Time routines ---*/

/**********************************************************************/
/** Let process sleep for specified time.
 * 
 * Suspends the calling process/task for at least the specified time.
 * Time may be rounded up according to ticker resolution.
 *
 * A \a msec value of <= 0 shall delay the process for the smallest possible
 * time. 
 *
 * \remark All signals during sleep will be ignored. It does not return
 * before the specified time has elapsed.
 *
 * \remark Don't use in IRQ or Alarm routines.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param msec			\IN number of milliseconds to delay
 * \return actual time slept in milliseconds
 */
int32 OSS_Delay( OSS_HANDLE *oss, int32 msec ){}

/**********************************************************************/
/** Initialisation routine for OSS_MikroDelay().
 * 
 * This routine performs any initialisation required to execute
 * OSS_MikroDelay(). For example, it might calibrate a delay loop counter.
 *
 * \remark MDIS LL drivers don't need to call this routine. It is 
 * 		   done by the MDIS kernel.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_MikroDelayInit( OSS_HANDLE *oss ){}
 
/**********************************************************************/
/** Delay execution of a process by using a busy-loop.
 * 
 * Delays the calling process for \em at \em least \a usec by wasting
 * CPU time in a busy loop. It can be called from interrupt or alarm
 * context.
 *
 * \remark \a usec shall be in range 0..1000 microseconds.
 * \remark OSS_MikroDelayInit() must be called before this routine can be used.
 * 
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param microSec		\IN number of microseconds to delay
 * 
 */
int32 OSS_MikroDelay( OSS_HANDLE *oss, u_int32 microSec ){}

/**********************************************************************/
/** Get the current system tick.
 * 
 * The OSS tick is a 32 bit value that is incremented at a rate
 * determined by OSS_TickRateGet(). The counter always uses the full
 * 32 bits and wraps from 0xffffffff to 0x00000000.
 *
 * \remark If interrupts are masked, the tick may no longer be incremented.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \return current system tick value.
 */
u_int32 OSS_TickGet(OSS_HANDLE *oss){}

/**********************************************************************/
/** Get the tick rate.
 * 
 * Get the rate at which the OSS tick is incremented in ticks per
 * seconds.
 *
 * \remark this function shall return a value between 50 and 1000000;
 * i.e. 20ms to 1us.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \return tick rate in Hz
 * 
 */
int32 OSS_TickRateGet( OSS_HANDLE *oss ){}

/**********************************************************************/
/** Map physical address space to virtual address space
 * 
 * Maps a given physical address \a physAddr space to the callers virtual
 * address space and returns the mapped address in \a *virtAddrP.
 *
 * The additional parameters \a addrSpace, \a busType and \a busNbr
 * are required for some implementations to use the right mapping
 * routine for the specified physical address, although \a busType
 * and \a busNbr are ignored in most implementations.
 *
 * On OSes without MMU or with a 1:1 mapping between physical and virtual
 * addresses, this routine just passes back the supplied \a physAddr.
 *
 * Caller should return the mapped address by means of OSS_UnMapVirtAddr()
 * when the mapping is no longer needed.
 *
 * \remark This routine is intended to be used to map hardware registers.
 * Therefore, the mapped page(s) will be setup to have the cache disabled
 * when it is accessed.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param physAddr  	\IN physical address to map
 * \param size			\IN number of bytes to map
 * \param addrSpace		\IN either \c OSS_ADDRSPACE_MEM or \c OSS_ADDRSPACE_IO
 * \param busType		\IN one of \c OSS_BUSTYPEE_NONE/VME/PCI/ISA
 * \param busNbr		\IN bus type specific parameter (e.g. pci bus nbr)
 * \param virtAddrP		\OUT mapped virtual address
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error:
 *   - \c ERR_OSS_MAP_FAILED: mapping not possible
 *
 */
int32 OSS_MapPhysToVirtAddr(
    OSS_HANDLE *oss,
    void       *physAddr,
    u_int32    size,
	int32	   addrSpace,
    int32      busType,
    int32      busNbr,
    void       **virtAddrP
	){}


/**********************************************************************/
/** Unmap virtual address space
 * 
 * Unmaps virtual address space previously mapped by OSS_MapPhysToVirtAddr().
 *
 * \remark The result is undefined if an attempt is made to unmap a virtual
 * address not previously mapped or with a different size.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param virtAddrP  	\IN *virtAddrP = virtual address to unmap. \n
 *						\OUT *virtAddrP set to NULL.
 * \param size			\IN number of bytes to unmap
 * \param addrSpace		\IN either \c OSS_ADDRSPACE_MEM or \c OSS_ADDRSPACE_IO
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 */
int32 OSS_UnMapVirtAddr(
    OSS_HANDLE *oss,
    void       **virtAddrP,
    u_int32    size,
	int32	   addrSpace
	){}

/**********************************************************************/
/** Convert bus address to CPU physical address.
 *
 * This routine converts the specified parameters to a CPU local
 * address.  It generally does not map the bus address into callers
 * address space or reserves the bus address for caller's use. See
 * OSS_MapPhysToVirtAddr() and OSS_AssignResources() for this purpose.
 * 
 * OSS_BusToPhysAddr() takes a variable number of arguments, depending
 * on \a busType:
 *
 * - \c OSS_BUSTYPE_NONE:
 *   - \a arg3 = void *localAddr (it's usually returned unchanged)
 *
 * - \c OSS_BUSTYPE_VME
 *   - \a arg3 = void *vmeBusAddr: VME bus address relative to VME space
 *	 - \a arg4 = u_int32 vmeSpace: VMEbus space and data width, specified as
 *         \c OSS_VME_A16, \c OSS_VME_A24, \c OSS_VME_A32 ORed with
 *	       \c OSS_VME_D16, \c OSS_VME_D32.
 *	 - \a arg5 = u_int32 size: requested size (sometimes ignored)
 *		\deprecated OSS_MapVmeAddr() (and subsequent OSS_UnMapVmeAddr())
 *		should be used instead of OSS_BusToPhysAddr(..,OSS_BUSTYPE_VME,..).
 *
 * 
 * - \c OSS_BUSTYPE_PCI
 *   - \a arg3 = int32 busNbr (0..255)
 *   - \a arg4 = int32 pciDevNbr (0..31)
 *   - \a arg5 = int32 pciFunction (0..7)
 *   - \a arg6 = int32 addrNbr (0..5) PCI BAR to read
 *   
 * - \c OSS_BUSTYPE_ISA
 *	 - ???
 * 
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busType		\IN bus type (see above)
 * \param physicalAddrP \OUT CPU physical address
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 */
int32 OSS_BusToPhysAddr(
    OSS_HANDLE *oss,
    int32       busType,
    void       **physicalAddrP,
	...){}

/**********************************************************************/
/** Map VMEbus address space into CPU physical address space.
 *
 * The function maps the VMEbus address space starting at address
 * \em vmeBusAddr with the size of \em size bytes into CPU local address
 * space and stores the corresponding local address into \em locAddrP.
 *
 * The used VMEbus address space (VMEbus address modifier) must be
 * specified using the \em addrMode parameter.\n
 * The maximum possible data access size on the VMEbus that should be used
 * for the pointer access with the returned local address must be set to
 * VME_DM_16 or VME_DM_32 through the \em maxDataMode parameter.\n
 *
 * The VMEbus address remains mapped until OSS_UnMapVmeAddr() is called
 * with the \em addrWinHdlP gotten from OSS_MapVmeAddr().
 *
 * If none VMEbus address windows is available for the specified parameters
 * the function returns with error code \c ERR_OSS_NO_ADDR_WINDOW.
 *
 * The function's behavior can be altered using the \em flags parameter
 * (e.g. to specify a HW Swapping Mode or to enable Posted Write Access).
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param vmeBusAddr	\IN VME bus address relative to VME space.
 *                          Must be aligned to maxDataMode.
 * \param addrMode		\IN address mode. See \link _VME_DMDA VME_AM_X address modes
 *                      \endlink
 * \param maxDataMode	\IN maximum VMEbus data access size (VME_DM_16 or VME_DM_32)
 * \param size			\IN number of bytes to map. Should be aligned to maxDataMode.
 * \param flags			\IN bitwise OR of any of the following: \n
 *                        <b>Hardware Swapping Modes</b>\n
 *                          \c VME_F_HWSWAP_MODE1 to use HW swapping mode #1\n
 *                        <b>Posted Write Access</b>\n
 *                          \c VME_F_POSTWR use posted write instead of delayed write\n
 * \param locAddrP		\IN pointer to variable that receives the local address
 * \param *locAddrP		\OUT local address to access the mapped VMEbus address space
 * \param addrWinHdlP	\IN pointer to variable that receives the address-window handle
 * \param *addrWinHdlP	\OUT address-window handle
 *
 * \return 0=ok, or error code
 */
int32 OSS_MapVmeAddr(
	OSS_HANDLE	*oss,
	u_int64 	vmeBusAddr,
	u_int32		addrMode,
	u_int32		maxDataMode,
	u_int32		size,
	u_int32		flags,
	void		**locAddrP,
	void		**addrWinHdlP){}

/**********************************************************************/
/** Unmap VMEbus address space
 *  
 * The function unmaps an OSS_MapVmeAddr() mapped VMEbus address space,
 * specified by the corresponding \em addrWinHdl .
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param addrWinHdl	\IN Address-window handle
 *
 * \return 0=ok, or error code
 */
int32 OSS_UnMapVmeAddr(
	OSS_HANDLE	*oss,
	void		*addrWinHdl ){}

/**********************************************************************/
/** Read a register from PCI configuration space
 *
 * This function can be used in two forms:
 * - Read one of the standard configuration registers. In this case \a which
 *   must be one of
 *   - \c OSS_PCI_VENDOR_ID          
 *   - \c OSS_PCI_DEVICE_ID          
 *   - \c OSS_PCI_COMMAND            
 *   - \c OSS_PCI_STATUS             
 *   - \c OSS_PCI_REVISION_ID        
 *   - \c OSS_PCI_CLASS              
 *   - \c OSS_PCI_SUB_CLASS          
 *   - \c OSS_PCI_PROG_IF            
 *   - \c OSS_PCI_CACHE_LINE_SIZE    
 *   - \c OSS_PCI_PCI_LATENCY_TIMER  
 *   - \c OSS_PCI_HEADER_TYPE        
 *   - \c OSS_PCI_BIST               
 *   - \c OSS_PCI_ADDR_0             
 *   - \c OSS_PCI_ADDR_1             
 *   - \c OSS_PCI_ADDR_2             
 *   - \c OSS_PCI_ADDR_3             
 *   - \c OSS_PCI_ADDR_4             
 *   - \c OSS_PCI_ADDR_5             
 *   - \c OSS_PCI_CIS                
 *   - \c OSS_PCI_SUBSYS_VENDOR_ID   
 *   - \c OSS_PCI_SUBSYS_ID          
 *   - \c OSS_PCI_EXPROM_ADDR        
 *   - \c OSS_PCI_INTERRUPT_PIN      
 *   - \c OSS_PCI_INTERRUPT_LINE     
 *
 *   Example: 
 *   \code
 *     error = OSS_PciGetConfig( oss, bus, dev, func, OSS_PCI_CLASS, &value );
 *   \endcode
 *
 * - Read an arbitrary configuration register. In this case, \a which is 
 *   composed of an access width specifier ORed with the register offset. 
 *   Possible access widths: 
 *   - \c OSS_PCI_ACCESS_8		   
 *   - \c OSS_PCI_ACCESS_16		   
 *   - \c OSS_PCI_ACCESS_32		   
 *
 *   Example: 
 *   \code
 *     error = OSS_PciGetConfig( oss, bus, dev, func, 
 *								 OSS_PCI_ACCESS_32 | 0x40, &value );
 *   \endcode
 * 
 * \remark If an attempt is made to access a non existant PCI device, no
 * error is returned, but \a *valueP will contain \c 0xFFFFFFFF.
 * 
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busNbr		\IN PCI bus number (0..255)
 * \param pciDevNbr		\IN PCI device number (0..31)
 * \param pciFunction	\IN PCI function number (0..7)
 * \param which			\IN see text above
 * \param valueP		\OUT value read from config space. Must be a pointer
 *							 a 32 bit value in any case. 
 * \return 0 on success or \c ERR_OSS_PCI_xxx error code on error
 *
 */
int32 OSS_PciGetConfig(
    OSS_HANDLE *oss,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       *valueP
	){}

/**********************************************************************/
/** Write to a register in PCI configuration space
 *
 * This function can be used in two forms:
 * - Write one of the standard configuration registers. In this case \a which
 *   must be one of
 *   - \c OSS_PCI_VENDOR_ID          
 *   - \c OSS_PCI_DEVICE_ID          
 *   - \c OSS_PCI_COMMAND            
 *   - \c OSS_PCI_STATUS             
 *   - \c OSS_PCI_REVISION_ID        
 *   - \c OSS_PCI_CLASS              
 *   - \c OSS_PCI_SUB_CLASS          
 *   - \c OSS_PCI_PROG_IF            
 *   - \c OSS_PCI_CACHE_LINE_SIZE    
 *   - \c OSS_PCI_PCI_LATENCY_TIMER  
 *   - \c OSS_PCI_HEADER_TYPE        
 *   - \c OSS_PCI_BIST               
 *   - \c OSS_PCI_ADDR_0             
 *   - \c OSS_PCI_ADDR_1             
 *   - \c OSS_PCI_ADDR_2             
 *   - \c OSS_PCI_ADDR_3             
 *   - \c OSS_PCI_ADDR_4             
 *   - \c OSS_PCI_ADDR_5             
 *   - \c OSS_PCI_CIS                
 *   - \c OSS_PCI_SUBSYS_VENDOR_ID   
 *   - \c OSS_PCI_SUBSYS_ID          
 *   - \c OSS_PCI_EXPROM_ADDR        
 *   - \c OSS_PCI_INTERRUPT_PIN      
 *   - \c OSS_PCI_INTERRUPT_LINE     
 *
 *   Example: 
 *   \code
 *     error = OSS_PciSetConfig( oss, bus, dev, func, OSS_PCI_CLASS, value );
 *   \endcode
 *
 * - Write an arbitrary configuration register. In this case, \a which is 
 *   composed of an access width specifier ORed with the register offset. 
 *   Possible access widths: 
 *   - \c OSS_PCI_ACCESS_8		   
 *   - \c OSS_PCI_ACCESS_16		   
 *   - \c OSS_PCI_ACCESS_32		   
 *
 *   Example: 
 *   \code
 *     error = OSS_PciSetConfig( oss, bus, dev, func, 
 *								 OSS_PCI_ACCESS_32 | 0x40, value );
 *   \endcode
 * 
 * \remark If an attempt is made to access a non existant PCI device, no
 * error is returned, the value is written into nirwanha.
 * 
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busNbr		\IN PCI bus number (0..255)
 * \param pciDevNbr		\IN PCI device number (0..31)
 * \param pciFunction	\IN PCI function number (0..7)
 * \param which			\IN see text above
 * \param value			\IN value to write to config space.
 *
 * \return 0 on success or \c ERR_OSS_PCI_xxx error code on error
 *
 */
int32 OSS_PciSetConfig(
    OSS_HANDLE *oss,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       value
	){}


/**********************************************************************/
/** Convert mechanical slot number to PCI device number
 *
 * The main purpose of this function is to determine the PCI device
 * number of a PCI card in slot \a mechSlot.
 * 
 * Typically used by BBIS drivers, such as the driver for D201 board 
 * family. 
 *
 * Mechanical slots are numbered from 1..17. On CompactPCI, 1 is
 * the CPU itsself, 2 the slot next to the CPU.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busNbr		\IN the PCI bus number in question
 * \param mechSlot		\IN mechanical slot to convert (1..17)
 * \param pciDevNbrP	\OUT PCI device number (0..31)
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_PciSlotToPciDevice(
    OSS_HANDLE *oss,
    int32      busNbr,
    int32      mechSlot,
	int32      *pciDevNbrP){}



/**********************************************************************/
/** Get ISAPNP configuration data.
 *
 *  \todo add full documentation.
 *
 *  \param oss 			\IN	OSS handle, returned by OSS_Init()
 *  \param cardVendId	\IN card vendor id (3 characters)
 *  \param cardProdId   \IN card product number (4 hex digits) 
 *  \param devVendId    \IN device vendor id (3 characters)
 *  \param devProdId    \IN device product number (4 hex digit)
 *  \param devNbr		\IN device number (decimal 0..9)
 *  \param resType      \IN resource type (OSS_ISA_...)
 *  \param resNbr		\IN resource number (decimal 0..9)
 *  \param valueP		\OUT output value
 *  
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_IsaGetConfig(
		OSS_HANDLE	*oss,
		int8		*cardVendId,
		int16		cardProdId,
		int8		*devVendId,
		int16		devProdId,
		int32       devNbr,
		int32       resType,
		int32       resNbr,
		u_int32		*valueP){}

/**********************************************************************/
/** Assign memory, I/O, interrupt resources to a driver
 *
 * This function is necessary for operating systems like Windows
 * which register all the resources that a driver need. This function
 * should be call before OSS_MapPhysToVirtAddr().
 *
 * Currently, memory, I/O regions and interrupts are
 * supported as resources.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busType		\IN \c OSS_BUSTYPE_xxx: Bustype where device is 
 *							located.
 * \param busNbr  		\IN busType specific, e.g. PICI bus number 5
 * \param resNbr		\IN number of resources in \a res
 * \param res			\IN array of resources to assign
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_AssignResources(
    OSS_HANDLE      *oss,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
	OSS_RESOURCES   res[]){}

/**********************************************************************/
/** Release resources assigned by OSS_AssignResources()
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busType		\IN \c OSS_BUSTYPE_xxx: Bustype where device is 
 *							located.
 * \param busNbr  		\IN busType specific, e.g. PICI bus number 5
 * \param resNbr		\IN number of resources in \a res
 * \param res			\IN array of resources to release
 * 
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 */
int32 OSS_UnAssignResources(
    OSS_HANDLE      *oss,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
	OSS_RESOURCES   res[]){}

/*---- Swapping routines ----*/

/**********************************************************************/
/** Swap bytes in 16-bit word.
 *
 * \param word			\IN word to swap
 * \return swapped word
 * \sa OSS_SWAP16, OSS_Swap32, OSS_Swap64
 */
u_int16 OSS_Swap16( u_int16 word ){}

/**********************************************************************/
/** Swap bytes in 32-bit word.
 *
 * \param dword			\IN double word to swap
 * \return swapped double word
 * \sa OSS_SWAP32, OSS_Swap16, OSS_Swap64
 */
u_int32 OSS_Swap32( u_int32 dword ){}

/**********************************************************************/
/** Swap bytes in 64-bit word.
 *
 * \param qword			\IN quad word to swap
 * \return swapped quad word
 * \sa OSS_SWAP64, OSS_Swap16, OSS_Swap32
 */
u_int32 OSS_Swap64( u_int64 qword ){}

/**********************************************************************/
/** Macro to swap bytes in 16-bit word.
 * 
 * \remark Parameter \a word is accessed multiple times by this macro.
 * Use OSS_Swap16() if this is a problem.
 *
 * \param word			\IN word to swap
 * \return swapped word
 * \sa OSS_Swap16, OSS_SWAP32, OSS_SWAP64
 */
#define OSS_SWAP16(word)

/**********************************************************************/
/** Macro to swap bytes in 32-bit word.
 * 
 * \remark Parameter \a dword is accessed multiple times by this macro.
 * Use OSS_Swap32() if this is a problem.
 *
 * \param dword			\IN double word to swap
 * \return swapped double word
 * \sa OSS_Swap32, OSS_SWAP16, OSS_SWAP64
 */
#define OSS_SWAP32(dword)

/**********************************************************************/
/** Macro to swap bytes in 64-bit word.
 * 
 * \remark Parameter \a qword is accessed multiple times by this macro.
 * Use OSS_Swap64() if this is a problem.
 *
 * \param qword			\IN quad word to swap
 * \return swapped quad word
 * \sa OSS_SWAP32, OSS_SWAP16, OSS_Swap32, OSS_Swap16
 */
#define OSS_SWAP64(qword)

/*---- Task routines ----*/

/**********************************************************************/
/** Get current task id.
 *
 * \remark Task ID is invalid if called at interrupt level.
 *
 * \deprecated Don't use it. Not all OSes have a task id.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \return task ID of calling process
 */
u_int32 OSS_GetPid(OSS_HANDLE *oss){}

/*---- CLIB substitutes ----*/

/**********************************************************************/
/** Copy memory block.
 *
 * Subsitute for C standard library call \em memcpy().
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param size	  		\IN number of bytes to copy
 * \param src	 		\IN source address
 * \param dest    		\IN destination address
 */
void OSS_MemCopy(
   OSS_HANDLE *oss,
   u_int32    size,
   char       *src,
   char       *dest){}

/**********************************************************************/
/** Fill data block with byte value.
 *
 * Subsitute for C standard library call \em memset().
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param size	  		\IN number of bytes to fill
 * \param adr	 		\IN start address
 * \param value    		\IN fill value
 */
void OSS_MemFill(
    OSS_HANDLE *oss,
    u_int32    size,
    char       *adr,
	int8       value){}

/**********************************************************************/
/** Copy string.
 *
 * Subsitute for C standard library call \em strcpy().
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param from			\IN source string
 * \param to	  		\IN destination string
 */
void OSS_StrCpy(
    OSS_HANDLE  *oss,
    char        *from,
	char        *to){}


/**********************************************************************/
/** Get the length of a string.
 *
 * Subsitute for C standard library call \em strlen().
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param string  		\IN start address of string
 */
u_int32 OSS_StrLen(
    OSS_HANDLE  *oss,
	char        *string){}

/**********************************************************************/
/** Compare two strings.
 *
 * Subsitute for C standard library call \em strcmp().
 *
 * Compares str1 and str2 lexicographicaly.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param str1  		\IN pointer to 1st string
 * \param str2  		\IN pointer to 2nd string
 * \return \n
 *		   - 0 if equal
 *		   - >0 if str1[x] > str2[x] ( first different char )
 *		   - <0 if str1[x] < str2[x] ( first different char )
 */
int32 OSS_StrCmp(
    OSS_HANDLE  *oss,
    char        *str1,
	char        *str2){}

/**********************************************************************/
/** Compare a limited number of characters of two strings.
 *
 * Subsitute for C standard library call \em strncmp().
 *
 * Compares at most \a nbrOfBytes characters of str1 and str2
 * lexicographicaly.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param str1  		\IN pointer to 1st string
 * \param str2  		\IN pointer to 2nd string
 * \param nbrOfBytes	\IN max. number of bytes to compare
 * \return \n
 *		   - 0 if equal
 *		   - >0 if str1[x] > str2[x] ( first different char )
 *		   - <0 if str1[x] < str2[x] ( first different char )
 */
int32 OSS_StrNcmp(
    OSS_HANDLE  *oss,
    char        *str1,
    char        *str2,
	u_int32     nbrOfBytes){}

/**********************************************************************/
/** Break string into tokens.
 *
 * Subsitute for C standard library call \em strtok(), but
 * OSS_StrTok() is reentrant.
 *
 * For the first call, pass \a *lastP as NULL. In this case, OSS_StrTok()
 * searches from the beginning of \a string for the separator string 
 * \a separator. The separator string must be found completely in 
 * \a string. 
 *
 * If a separator was found, the first char of the separator occurrance
 * within \a string is replaced by a '\\0'. The variable pointed to 
 * by \a *lastP is used to save the search start point within \a string
 * for the next call.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param string		\IN string to break into tokens. Can be NULL for
 *							for second and subsequent calls.
 * \param separator  	\IN separator string
 * \param lastP      	\INOUT start of next separator search. Should be 
 *							passed as NULL for the first call.
 * \return pointer to token or NULL if separator not found
 */
char* OSS_StrTok(
    OSS_HANDLE  *oss,
    char        *string,
    char        *separator,
	char        **lastP){}


/**********************************************************************/
/** Print to string using variable number of arguments 
 *
 * Subsitute for C standard library call \em sprintf().
 *
 * \remark Implementations don't need to support floating point formats.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param str       	\IN destination buffer, will contain formatted string.
 * \param fmt        	\IN printf()-like format string
 * \param ...			\IN arguments
 *
 * \return number of characters written to string (excluding \\0)
 */
int32 OSS_Sprintf(
    OSS_HANDLE  *oss,
    char *str,
    const char *fmt,
	...){}

/**********************************************************************/
/** Print to string using \em va_list
 *
 * Subsitute for C standard library call \em vsprintf().
 *
 * \remark Implementations don't need to support floating point formats.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param str       	\IN destination buffer, will contain formatted string.
 * \param fmt        	\IN printf()-like format string
 * \param arg			\IN arguments
 *
 * \return number of characters written to string (excluding \\0)
 */
int32 OSS_Vsprintf(
    OSS_HANDLE  *oss,
    char *str,
    const char *fmt,
	va_list arg){}

/**********************************************************************/
/** Get SMB handle
 *
 * The function requests the smbHdl for the specified SMBus.
 *
 * The smbHdl is a pointer to a SMB_HANDLE structure (as defined in
 * sysmanagbus2.h) that provides the functions to access the specified
 * SMBus.
 * The smbHdl can be the initialized handle returned from the
 * Smb_<HWName>_Init() function of the common SMB2 library or it can be
 * initialized from a native SW module.
 * The OS specific implementation of the SMBus support is responsible
 * to initialize the smbHdl prior to OSS_GetSmbHdl() calls.
 *
 * With the gotten smbHdl, the caller can use the provided SMB functions.
 * Example:
 *   SMB_HANDLE	*smbH = ((SMB_HANDLE*)*smbHdlP);
 *   error = smbH->WriteByte( smbH, addr, data );
 *
 * The function returns \c ERR_OSS_ILL_HANDLE if the requested smbHdl
 * is not available.
 *
 * \remark The returned smbHdl is no longer valid after the SMB libraries
 * exit function was called. The caller of OSS_GetSmbHdl must not call
 * the SMB libraries exit function. The SMB library is responsible for the
 * synchronisation of the provided SMB library functions. The suggested
 * assignment for the SMBus identifikation is: busNbr=0 : onboard SMBus,
 * busNbr=1 : additional onboard SMBus or plugged SMBus.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busNbr       	\IN SMBus identifikation (0,1,2,...)
 * \param smbHdlP		\OUT smbHdl
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 * \sa OSS_SetSmbHdl
 */
int32 OSS_GetSmbHdl(
    OSS_HANDLE  *oss,
    u_int32		busNbr,
	void		**smbHdlP){}

/**********************************************************************/
/** Set SMB handle
 *
 * The function sets the smbHdl for the specified SMBus.
 *
 * The smbHdl is a pointer to a initialized SMB_HANDLE structure (as
 * defined in sysmanagbus2.h) that provides the functions to access the
 * specified SMBus.
 * The smbHdl can be the initialized handle returned from the
 * Smb_<HWName>_Init() function of the common SMB2 library or it can be
 * initialized from a native SW module.
 *
 * \remark The returned smbHdl is no longer valid after the SMB libraries
 * exit function was called.
 *
 * \param oss 			\IN OSS handle, returned by OSS_Init()
 * \param busNbr       	\IN SMBus identifikation (0,1,2,...)
 * \param smbHdl		\OUT smbHdl
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 * \sa OSS_GetSmbHdl
 */
int32 OSS_SetSmbHdl(
    OSS_HANDLE  *oss,
    u_int32		busNbr,
	void		*smbHdl){}





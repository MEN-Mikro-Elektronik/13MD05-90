/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  usr_oss_specification.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/07/22 14:36:50 $
 *    $Revision: 1.5 $
 *
 *  	 \brief  MEN USR_OSS (Operating System Services) module specification.
 *
 * This file contains the common, platform independent specification for
 * the USR_OSS module.
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: usr_oss_specification.c,v $
 * Revision 1.5  2009/07/22 14:36:50  dpfeuffer
 * (AUTOCI) Checkin due to new revision 1.5 of fileset WINDOWS/LIBSRC/usr_oss
 *
 * Revision 1.4  2008/12/03 11:27:57  CKauntz
 * Added Page titles and removed following section names
 * Adapted references instead of section name to page name
 *
 * Revision 1.3  2003/10/07 12:29:41  kp
 * \menimages again removed. Shall be in mainpage.
 * Added page title
 *
 * \menimages again removed. Shall be in mainpage.
 * Added page title
 *
 * Revision 1.2  2003/05/20 09:09:03  dschmidt
 * \menimages added to include images
 *
 * Revision 1.1  2003/05/09 12:25:25  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*! \page usrosscommonspec USR_OSS specificiation MDIS5

 The MEN USR_OSS module (User space Operating System Services) is an
 abstraction of the services provided by the operating system in user space.

 USR_OSS is mainly used to keep MDIS example programs and tools OS
 independent, but can be used also in other contexts.

 \remark Unless otherwise noted, USR_OSS functions return an error code
 \c ERR_UOS_xxx \b and set the global error number \em errno, that can
 be retrieved by UOS_ErrnoGet(). This \em errno is process/task
 specific.


 The common USR_OSS specification provides the following functional groups:

 - \b Time related: UOS_Delay(), UOS_MikroDelay(), UOS_MikroDelayInit()
   UOS_MsecTimerGet(), UOS_MsecTimerResolution()
 - \ref uossigusage "Signal Handling": UOS_SigInit(), UOS_SigExit(),
   UOS_SigInstall(), UOS_SigRemove(), UOS_SigMask(), UOS_SigUnMask(),
   UOS_SigWait()
 - \b User Input: UOS_KeyPressed(), UOS_KeyWait()
 - \b Random number generation: UOS_Random(), UOS_RandomMap()
 - \ref uosdllusage "Lists": UOS_DL_NewList(), UOS_DL_Remove(),
   UOS_DL_AddTail(), UOS_DL_RemHead()
 - \b Errno: UOS_ErrnoGet(), UOS_ErrnoSet(), UOS_ErrStringTs(),
   UOS_ErrString()
 - \b Version info: UOS_Ident()
*/

/**********************************************************************/
/** Get global error code (task/process specific errno)
 *
 * The function returns the last occured system error code for the calling
 * task/process
 *
 * \return value of \em errno
 */
u_int32 UOS_ErrnoGet( void ){}

/**********************************************************************/
/** Set global error code (task/process specific errno)
 *
 * Can be used for example, by MDIS API libraries to set library
 * specific error codes.
 *
 * \remark This function has been added in MDIS4/2003
 *
 * \return value of \a errStr
 */
u_int32 __MAPILIB UOS_ErrnoSet( u_int32 error ){}

/**********************************************************************/
/** Convert UOS error number to static string
 *
 * \deprecated Use of this function in multithreaded applications
 * or under OSes with global namespace is dangerous, since this
 * function uses and returns a static buffer to print its error
 * messages. Better use thread-safe version UOS_ErrStringTs().
 *
 * Format of returned string:
 * \verbatim
     ERROR (UOS) 0x%04x:  <error description>
   \endverbatim
 *
 * If the \a errCode is not an UOS error code, prints "unknown error"
 * as error description.
 *
 * \param errCode 		\IN error code \c ERR_UOS_xxx
 * \return pointer to error message (static buffer!)
 */
char * __MAPILIB UOS_ErrString(int32 errCode){}

/**********************************************************************/
/** Convert UOS error number to string
 *
 * Format of returned string:
 * \verbatim
     ERROR (UOS) 0x%04x:  <error description>
   \endverbatim
 *
 * If the \a errCode is not an UOS error code, prints "unknown error"
 * as error description.
 *
 * \remark This function has been added in MDIS4/2003
 *
 * \param errCode 		\IN error code \c ERR_UOS_xxx
 * \param strBuf		\OUT filled with error message (should have space
 *							 for 512 characters, including '\\0')
 * \return \a strBuf
 */
char * __MAPILIB UOS_ErrStringTs(int32 errCode, char *strBuf ){}

/**********************************************************************/
/** Let a process sleep for a specified time
 *
 * Suspends the calling process/task for at least the specified time.
 * Time may be rounded up according to ticker resolution.
 *
 * A \a msec value of <= 0 shall delay the process for the smallest possible
 * time.
 *
 * \remark All signals during sleep will be ignored. It does not return
 * before the specified time has elapsed. However signal handlers will
 * be executed if signals are unmasked.
 *
 * \param msec			\IN number of milliseconds to delay
 * \return actual time slept in milliseconds
 */
int32 __MAPILIB UOS_Delay(u_int32 msec){}

/**********************************************************************/
/** Initialisation routine for UOS_MikroDelay().
 *
 * This routine performs any initialisation required to execute
 * UOS_MikroDelay(). For example, it might calibrate a delay loop counter.
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_MikroDelayInit( void ){}

/**********************************************************************/
/** Delay execution of a process by using a busy-loop.
 *
 * Delays the calling process for \em at \em least \a usec by wasting
 * CPU time in a busy loop. It can be called from signal handler
 * context.
 *
 * \remark \a usec shall be in range 0..1000000 microseconds.
 * \remark UOS_MikroDelayInit() must be called before this routine can be
 * used.
 *
 * \param usec			\IN number of microseconds to delay
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_MikroDelay( u_int32 usec ){}

/**********************************************************************/
/** Read the current timer value in milliseconds
 *
 * \remark The UOS specification is weak. It does not specify what is
 * the time reference of the returned value. It may be, for example,
 * relative to January 1st 1970 or relative to system start.
 *
 * Implementations must handle the situation correctly when the UOS
 * timer wraps over from 0xffffffff to 0x0; i.e. the difference
 * between two calls to UOS_MsecTimerGet() must be correct even when
 * the UOS timer has wrapped over.
 *
 * \return current time in ms
 */
u_int32 __MAPILIB UOS_MsecTimerGet(void){}

/**********************************************************************/
/** Get timer resolution of UOS_MsecTimerGet()
 *
 * \return timer resolution in milliseconds.
 */
u_int32 __MAPILIB UOS_MsecTimerResolution(void){}


/*---- Signal routines ----*/

/*! \page uossigusage Using USR_OSS Signals

  The UOS_SigXXX() routines provide an abstraction of the signal handling
  of the underlying operating system.

  Please refer to the MDIS developer guide for more information on signals.

  Since not all OSes are aware of the signal concept, these functions
  can be used to emulate signals on those OSes; for example by starting
  a seperate thread that executes the signal handler.

  Before signals can be used, UOS_SigInit() has to be called once by
  the application. UOS_SigInit() can be passed a user signal handler
  that is called for every received signal. Alternatively, one can
  pass \c NULL instead of a user signal handler. In this case, only
  UOS_SigWait() can be used to handle signals.

  Furthermore, the application has to call UOS_SigInstall() for every
  signal \em number it wishes to handle.

  After this initialisation has been done, the application is ready to
  receive signals. It will then instruct the (MDIS) device drivers to
  send signals on specific events by using driver specific SetStat
  codes.

  If a signal is received, the user's signal handler is called
  (provided a user signal handler has been passed to UOS_SigInit() )
  with the signal number as its first argument:

  \code
  void __MAPILIB SigHandler( u_int32 sigCode )
  {
      switch( sigCode ){
	  case UOS_SIG_USR1:
	       // handle this signal
		   break;
	  }
  }
  \endcode

  If no signal handler was passed to UOS_SigInit(), the application
  can only wait for signals synchronously by calling UOS_SigWait().

  To protect global data used by the main thread and the signal
  handler from being accessed concurrently, you can use UOS_SigMask()
  and UOS_SigUnMask() from the main thread.

  \subsection signums Signal numbers

  For full portability across operating systems, an application shall
  only use signal number \c UOS_SIG_USR1 and \c UOS_SIG_USR2. However,
  if portability is not an issue, most OSes allow much more different
  signals to be used.

*/

/**********************************************************************/
/** Init signal handling
 *
 * This function has to be called exactly once by the application
 * to prepare for signal handling.
 *
 * You can pass a user signal handler \a sigHandler to this routine.
 * If you only want to use UOS_SigWait() to receive signals, you can
 * pass NULL.
 *
 * See \ref uossigusage for a general description of USR_OSS
 * signal handling.
 *
 * \param sigHandler signal handler routine or NULL for no handler
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_SigInit(void (*sigHandler)(u_int32 sigCode)){}

/**********************************************************************/
/** Terminate signal handling
 *
 * This function should be called at the end of the application to
 * free all resources used for signal handling.
 *
 * See \ref uossigusage for a general description of USR_OSS
 * signal handling.
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_SigExit(void){}

/**********************************************************************/
/** Install a signal to be received
 *
 * This function assigns the signal \a sigCode to the users signal
 * handler. If it is not called, it isn't garanteed that the signal
 * will be handled by the users signal handler
 *
 * If specified signal is already installed, the function returns an
 * \c ERR_UOS_BUSY error.
 *
 * If specified signal is not allowed, the function returns an
 * \c ERR_UOS_ILL_SIG error.
 *
 * If signal handling not initialized, the function returns an
 * \c ERR_UOS_NOT_INIZED error.
 *
 * See \ref uossigusage for a general description of USR_OSS
 * signal handling.
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_SigInstall(u_int32 sigCode){}

/**********************************************************************/
/** Remove signal to be received
 *
 * Removes the signal code \a sigCode from the list of signals handled
 * by the users signal handler
 *
 * If signal is not installed, the function returns an
 * \c ERR_UOS_NOT_INSTALLED error.
 *
 * If signal handling not initialized, the function returns an
 * \c ERR_UOS_NOT_INIZED error.
 *
 * \param sigCode	signal code
 * See \ref uossigusage for a general description of USR_OSS
 * signal handling.
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_SigRemove(u_int32 sigCode){}

/**********************************************************************/
/** Mask all signals
 *
 * Blocks all signals (at least those installed by UOS_SigInstall()).
 * All incomming signals are queued.
 *
 * \remark UOS_SigMask and UOS_SigMask() do not need to support
 * nesting. I.e. if you call UOS_SigUnMask() signals will be unmasked
 * regardless how often you called UOS_SigMask() before.
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_SigMask(void){}

/**********************************************************************/
/** Unmask all signals
 *
 * Unmasks all signals (at least those installed by UOS_SigInstall())
 * If signals are already pending (queued), they are executed when
 * this call is issued.
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_SigUnMask(void){}

/**********************************************************************/
/** Wait until signal received
 *
 * This function waits until a signal is received or a timeout occurs.
 * If a signal has been received, its value is store in \a sigCodeP and
 * in addition, the users signal handler is called (if installed).
 *
 * The function checks if any signal is already queued:
 * - Signal is already queued: The function reads the signal from the
 *   queue and calls the user signal handler (if installed).
 * - Queue is empty: The function unmasks all signals and puts the
 *   process into sleep state until a new signal received.
 *
 * If no signal was received within the timeout period, the function
 * returns with error \c ERR_UOS_TIMEOUT.
 *
 * If signal handling not initialized, the function returns an
 * \c ERR_UOS_NOT_INIZED error.
 *
 *
 * On some OSes, if the process received a deadly signal (i.e. was
 * killed), the function returns with error \c ERR_UOS_ABORTED and a
 * valid signal code in \a sigCodeP.
 *
 * Notes:
 * - Signals must be masked before calling this function.
 * - Signals will be masked when this function returns.
 * - Given timeout will be rounded up to system ticks
 *
 * \param msec 		 wait timeout (0=endless) [msec]
 * \param sigCodeP   pointer to var where received signal will be stored
 *
 * \return 0 on success or \c ERR_UOS_xxx error code on error.
 *         On failure, UOS global errno is set to the error code
 */
int32 __MAPILIB UOS_SigWait(u_int32 msec, u_int32 *sigCodeP){}


/**********************************************************************/
/** Check if any key pressed on console
 *
 * This function checks if any character is buffered in the stdin
 * path. If so, the char this read and returned as integer value. If
 * not, -1 is returned.
 *
 * \remark The pressed character shall be echoed to stdout
 *
 * \return -1 if no key pressed or ASCII code of key (0..255)
 */
int32 __MAPILIB UOS_KeyPressed( void ){}


/**********************************************************************/
/** Wait until any key pressed on console
 *
 * This function waits until any character could be read from the
 * stdin path.
 *
 * \remark The pressed character shall be echoed to stdout
 *
 * \return -1 if read error or ASCII code of key (0..255)
 */
int32 __MAPILIB UOS_KeyWait( void ){}


/**********************************************************************/
/** Create a new pseudo random integer value
 *
 * \param old	 initial or last returned random value
 * \return random integer value (0..0xffffffff)
 */
u_int32 __MAPILIB UOS_Random(u_int32 old){}

/**********************************************************************/
/** Map created integer value into specified range
 *
 * \param val     integer value
 * \param ra	  min value
 * \param re	  max value
 * \return mapped integer value [ra..re]
 */
u_int32 __MAPILIB UOS_RandomMap(u_int32 val, u_int32 ra, u_int32 re){}

/**********************************************************************/
/** Return ident string of USR_OSS module
 *
 * \return a static constant string containing the USR_OSS module's
 * revision
 */
char* __MAPILIB UOS_Ident( void ){}

/*! \page uosdllusage Using USR_OSS Double Linked Lists

  USR_OSS provides the same set of
  \ref ossdllusage "double linked list routines as the OSS module".
  Just replace the \c OSS_ prefix with \c UOS_.

*/

/**********************************************************************/
/** Initialize list header
 *
 * Mark list as empty
 *
 * See \ref uosdllusage for more info.
 *
 * \param l 			 \IN pointer to list structure
 * \return l
 * \sa UOS_DL_Remove, UOS_DL_RemHead, UOS_DL_AddTail
 */
UOS_DL_LIST * __MAPILIB UOS_DL_NewList( UOS_DL_LIST *l ){}

/**********************************************************************/
/** Remove a node from a list
 *
 * See \ref uosdllusage for more info.
 *
 * \param n 			 \IN node to remove
 * \return n
 * \sa UOS_DL_NewList, UOS_DL_RemHead, UOS_DL_AddTail
 */
UOS_DL_NODE * __MAPILIB UOS_DL_Remove( UOS_DL_NODE *n ){}

/**********************************************************************/
/** Remove a node from the head of the list
 *
 * See \ref uosdllusage for more info.
 *
 * \param l 			 \IN pointer to list header
 * \return removed node or NULL if list was empty
 * \sa UOS_DL_NewList, UOS_DL_Remove, UOS_DL_AddTail
 */
UOS_DL_NODE * __MAPILIB UOS_DL_RemHead( UOS_DL_LIST *l ){}

/**********************************************************************/
/** Add a node at tail of list
 *
 * See \ref uosdllusage for more info.
 *
 * \param l 			 \IN pointer to list header
 * \param n 			 \IN node to add
 * \return n
 * \sa UOS_DL_NewList, UOS_DL_Remove, UOS_DL_RemHead
 */
UOS_DL_NODE * __MAPILIB UOS_DL_AddTail( UOS_DL_LIST *l, UOS_DL_NODE *n ){}


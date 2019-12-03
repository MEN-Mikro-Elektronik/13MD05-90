/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  mdis_api_specification.c
 *
 *      \author  klaus.popp@men.de
 * 
 *       \brief  MDIS_API specification.
 *
 * This file contains the common, platform independent specification for
 * the MDIS API.
 *
 */
/*---------------------------------------------------------------------------
 * Copyright 1997-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/

/*! \page mdisapicommonspec Common MDIS_API Specification "MDIS5"

  The MDIS API provides a standard driver interface to MDIS device drivers,
  e.g. for M-Modules, PC-MIP or on-board devices.  

  All MDIS drivers have the following features: 
  - Several paths can be opened for each device and each path can 
    have different attributes.  
  - I/O can be done channel-by-channel (channel I/O) or blockwise (block I/O).
  - Channel I/O operations can be done with automatic channel
    incrementation.  
  - Block I/O operations can be done for each channel and in various 
    buffer modes.
  - Signal conditions can be installed, which allows a programmable 
    signal to be sent to the application when a specific condition 
    becomes true.
  - Status calls are provided for starting specific actions and
    setting or querying parameters, making it possible to access
    functions specific to a device driver as well as the standardized
    MDIS functions.

  MDIS API provides the following routines:
  - Open and close path to device: M_open(), M_close()
  - Exchange 32 bit value with driver: M_read(), M_write()
  - Exchange larger data blocks with driver: M_getblock(), M_setblock()
  - Status calls: M_getstat(), M_setstat()
  - Get formatted error string: M_errstringTs(), M_errstring()

  All MDIS functions return a negative value when an error occurs. The
  appropriate error code is stored in a global system variable. Since
  this variable (mostly \em errno) depends on the operating system, function
  UOS_ErrnoGet() (located in the UOS library) should be used to get the
  error code.  An error message string can easily be generated using
  M_errstringTs() (see example programs of the individual MDIS API calls).

  For signal handling and further functions, see the 
  \ref usrosscommonspec "USR_OSS library specification".
*/
  
/*! \page mdis_api_dummy
 \menimages
*/

/**********************************************************************/
/** Open path to device
 *
 * M_open() opens a new path to a device. The device name is passed as
 * a string. The function returns the path number (handle) by which the
 * device is to be referred to from then on.  
 *
 * The device name may be OS-specific, but is typically something
 * like \c "/m22_1".
 *
 * Typically, if the first path is opened to the device, the device is
 * initialized.  Subsequent references to the device are by the
 * returned path number path. If the same device is opened several
 * times, there are different paths to it.
 *
 * Each of these paths have the same (global) device parameters but
 * have their own (local) path parameters:
 * - current channel number
 * - channel I/O mode 
 *
 * \param device        \IN name of the device e.g. "/m22_1" 
 *
 * \return the device handle (>= 0)  on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 *
 * \b Example: 
 * \code
 *   #include <MEN/men_typs.h>
 *   #include <MEN/mdis_api.h>
 *   ...
 *
 *   MDIS_PATH path;
 *
 *   path = M_open( "/m22_1" );
 *   if( path < 0 ){
 *       // open failed
 *       return -1;
 *   }
 * \endcode
 */
MDIS_PATH __MAPILIB M_open(const char *device){}

/**********************************************************************/
/** Close path to device
 *  
 * M_close() closes the specified path to a device.  Typically, if the
 * last path to the device is closed, the device is deinitialized.
 *
 * \param path          \IN device file handle, returned by M_open()
 * \return \c 0 on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 */
int32 __MAPILIB M_close(MDIS_PATH path){}


/**********************************************************************/
/** Read 32-bit integer value from device
 *  
 * M_read() reads a 32-bit integer value from the current channel of
 * the specified path into the variable where value points to, which
 * is passed as a pointer. Depending on the selected I/O mode, the
 * current channel may be incremented.
 *
 * \param path          \IN device file handle, returned by M_open()
 * \param valueP        \OUT pointer to variable where read value will  
 *                           be stored.
 *
 * \return \c 0 on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 *
 * \b Example: 
 * \code
 *
 * #include <stdio.h>
 * #include <MEN/men_typs.h>
 * #include <MEN/mdis_api.h>
 * #include <MEN/usr_oss.h>
 *
 * int main( int argc, char **argv)
 * {
 *  int32 readData;
 *  char strBuf[128];
 *  // .. open path to device ..
 *
 *  //------------------------
 *  // read from device
 *
 *  if (M_read(path, &readData) < 0) {
 *      // failed: create error message
 *      printf("*** can’t read: %s\n", M_errstringTs(UOS_ErrnoGet(),strBuf));
 *  }
 *  else
 *      printf("value=0x%lx read\n", readData);
 *
 *  // ...
 * }
 * \endcode
 */
int32 __MAPILIB M_read(MDIS_PATH path, int32 *valueP){}

/**********************************************************************/
/** Write 32-bit integer value to device
 *  
 * M_write() outputs a 32-bit integer value to the current channel of
 * the specified path.  Depending on the selected I/O mode, the
 * current channel may be incremented.
 *
 * \param path      \IN device file handle, returned by M_open()
 * \param value     \IN value to write
 *
 * \return \c 0 on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 *
 * \b Example: 
 * \code
 * #include <stdio.h>
 * #include <MEN/men_typs.h>
 * #include <MEN/mdis_api.h>
 * #include <MEN/usr_oss.h>
 *
 * int main( int argc, char **argv)
 * {
 *  int32 writeData = 0x12345678;
 *  char strBuf[128];
 *
 *  // .. open path to device ..
 *
 *  //------------------------
 *  // write to device
 *
 *  if (M_write(path, writeData) < 0) {
 *      // failed: create error message 
 *      printf("*** can’t write: %s\n", M_errstringTs(UOS_ErrnoGet(),strBuf));
 *  }
 * }
 * \endcode
 */
int32 __MAPILIB M_write(MDIS_PATH path, int32 value){}

/**********************************************************************/
/** Read data block from device
 *  
 * M_getblock() reads a data block of (maximum) \a length bytes from the
 * specified path into buffer \a buf provided by the application and
 * returns the number of bytes read.  Depending on the device driver
 * implementation, you can read data from the hardware or from an
 * input buffer.  
 *
 * If an input buffer is supported, the buffer handling method depends
 * on the block I/O mode used. The minimum size of the application
 * buffer, one buffer entry, can be queried using the \c M_BUF_RD_WIDTH
 * GetStat call. If the requested block size is smaller than one
 * buffer entry, the function returns an error.
 *
 * \param path      \IN device file handle, returned by M_open()
 * \param buffer    \OUT will contain read data
 * \param length    \IN max. number of bytes to read
 *
 * \return number of bytes read on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 *
 * \b Example: 
 * \code
 * #include <stdio.h>
 * #include <MEN/men_typs.h>
 * #include <MEN/mdis_api.h>
 * #include <MEN/usr_oss.h>
 *
 * int main( int argc, char **argv)
 * {
 *  u_int8 readBuf[32];
 *  char strBuf[128];
 *  int32 readSize;
 *
 *  // .. open path to device ..
 *
 *  //------------------------
 *  // read block from device (32 bytes)
 *  if ((readSize = M_getblock(path, readBuf, 32)) < 0) {
 *      // failed: create error message 
 *      printf("*** getblock: %s\n", M_errstringTs(UOS_ErrnoGet(),strBuf));
 *  }
 *  else
 *      printf("%d bytes read\n", readSize);
 * }
 * \endcode
 */
int32 __MAPILIB M_getblock(MDIS_PATH path, u_int8 *buffer, int32 length){}
   

/**********************************************************************/
/** Write data block to device
 *  
 * M_setblock() writes a data block buf of (maximum) \a length bytes to the
 * specified path and returns the number of bytes written.  Depending
 * on the device driver implementation, you can write data to the
 * hardware or to an output buffer.  
 *
 * If an output buffer is supported, the buffer handling method
 * depends on the block I/O mode used. The minimum size of the
 * application buffer, one buffer entry, can be queried using the
 * \c M_BUF_WR_WIDTH GetStat call. If the requested block size is smaller
 * than one buffer entry, the function returns an error.
 *
 * \param path      \IN device file handle, returned by M_open()
 * \param buffer    \IN data block to write
 * \param length    \IN max. number of bytes to write
 *
 * \return number of bytes written on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 *
 * \b Example: 
 * \code
 * #include <stdio.h>
 * #include <MEN/men_typs.h>
 * #include <MEN/mdis_api.h>
 * #include <MEN/usr_oss.h>
 *
 * int main( int argc, char **argv)
 * {
 *  u_int8 writeBuf[32];
 *  char strBuf[128];
 *  int32 writeSize;
 *
 *  // .. open path to device ..
 *
 *  //------------------------
 *  // write block to device (32 bytes)
 *  if ((writeSize = M_getblock(path, writeBuf, 32)) < 0) {
 *      // failed: create error message 
 *      printf("*** setblock: %s\n", M_errstringTs(UOS_ErrnoGet(),strBuf));
 *  }
 *  else
 *      printf("%d bytes written\n", writeSize);
 * }
 * \endcode
 */
int32 __MAPILIB M_setblock(MDIS_PATH path, const u_int8 *buffer, int32 length){}

/**********************************************************************/
/** Get status from device
 *  
 * M_getstat() is used to obtain the parameters for path.  The
 * parameter \a code specifies which parameter is to be obtained. The
 * value obtained is written to the variable \a data depending on the
 * status code type:
 *
 * - For normal status codes, \a dataP points to a 32-bit
 *   integer value. 
 * - For block status codes, \a dataP it is interpreted as a
 *   pointer to structure #M_SG_BLOCK, which contains the size and
 *   location of the data buffer. Note that block getstats can
 * 	 be used to exchange data with the driver in both directions
 *   (to and from the driver).
 *
 * All standard status codes are defined in \em MEN/mdis_api.h and in
 * the MDIS5 user guide. In addition, most device drivers also support
 * device-specific status codes. These are defined in the driver’s
 * header file and described in the corresponding device driver user
 * manual.
 *
 * 
 * \param path      \IN device file handle, returned by M_open()
 * \param code      \IN status code (defined in \em mdis_api.h or \em xxx_drv.h
 * \param dataP     \INOUT depends on type of code:
 *     - standard codes: *dataP will contain the obtained paramter
 *     - block codes: dataP must point to #M_SG_BLOCK structure
 *
 * \return \c 0 on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 *
 * \b Example: 
 * \code
 *  char llHandler[M_MAX_NAME];
 *  int32 chNumber;
 *  M_SG_BLOCK blk;
 *
 *  // standard getstat: (here: number of channels)
 *  if( M_getstat(path, M_LL_CH_NUMBER, &chNumber) < 0 ) 
 *      printf("*** getstat: %s\n", M_errstringTs(UOS_ErrnoGet(),strBuf));    
 *  else
 *      printf("%d channels\n", chNumber);
 *
 *  // block getstat (here: device driver name)
 *  blk.size = sizeof(llHandler);
 *  blk.data = (void*)llHandler;
 *
 *  if( M_getstat(path, M_MK_BLK_LL_HANDLER, (int32*)&blk) < 0)
 *      printf("*** getstat: %s\n",M_errstringTs(UOS_ErrnoGet(),strBuf));
 *  else
 *      printf("device driver: %s\n", llHandler);
 * }
 * \endcode
 */
int32 __MAPILIB M_getstat(MDIS_PATH path, int32 code, int32 *dataP){}

/**********************************************************************/
/** Set status of device
 *  
 * M_setstat() is used to modify the parameters for path. The status
 * code \a code specifies which parameter is to be set to the new value
 * data.  
 * - For normal status codes, \a data contains a 32-bit integer
 *   value. 
 * - For block status codes, \a data it is interpreted as a pointer to
 *   structure #M_SG_BLOCK, which contains the size and location of the
 *   data buffer. Note that block setstats can
 * 	 be used only to send data from the application to the driver (not the
 *   opposite direction).
 *
 * All standard status codes are defined in \em MEN/mdis_api.h and in
 * the MDIS5 user guide. In addition, most device drivers also support
 * device-specific status codes. These are defined in the driver’s
 * header file and described in the corresponding device driver user
 * manual.
 *
 * 
 * \param path      \IN device file handle, returned by M_open()
 * \param code      \IN status code (defined in \em mdis_api.h or \em xxx_drv.h
 * \param data      \IN depends on type of code:
 *     - standard codes: value to write
 *     - block codes: data must point to #M_SG_BLOCK structure
 *
 * \return \c 0 on success or \c -1 on error. 
 *         On failure, global \em errno is set to the error code
 *
 * \b Example: 
 * \code
 *  int32 chCurrent;
 *  u_int8 blkData[32];
 *  M_SG_BLOCK blk;
 *
 *  // standard setstat: (here: current channel)
 *  if( M_setstat(path, M_MK_CH_CURRENT, chCurrent) < 0 ) 
 *      printf("*** setstat: %s\n", M_errstringTs(UOS_ErrnoGet(),strBuf));    
 *
 *  // block setstat (here: device device specific)
 *  blk.size = sizeof(blkData);
 *  blk.data = (void*)blkData;
 *
 *  if( M_setstat(path, M12_BLK_PARAMS, (int32*)&blk) < 0)
 *      printf("*** setstat: %s\n",M_errstringTs(UOS_ErrnoGet(),strBuf));
 * \endcode
 */
int32 __MAPILIB M_setstat(MDIS_PATH path, int32 code, INT32_OR_64 data){}


/**********************************************************************/
/** Convert MDIS error code to static string
 *  
 * \deprecated Use of this function in multithreaded applications
 * or under OSes with global namespace is dangerous, since this
 * function uses and returns a static buffer to print its error 
 * messages. Better use thread-safe version M_errstringTs().
 *
 * M_errstring() creates an error message for error \a errCode and
 * returns a pointer to the generated string with the following
 * format: 
 * 
 * \verbatim
 * ERROR <errtype> <errcode>: <errdescr> 
 * \endverbatim
 *
 * where \em errtype describes if the error code comes from the system
 * or the driver, \em errcode describes the error code in hexadecimal
 * format or in the operating system’s native format. \em errdescr is the
 * corresponding error message string.  
 *
 * \b Examples: 
 * \verbatim
   ERROR (MDIS) 0x1103: MK: illegal parameter
   ERROR (OS9) #000:221: module not found
   \endverbatim
 *
 * \param errCode       \IN error code determined by UOS_ErrnoGet()
 * \return pointer to error message (static buffer!)
 *
 */
char* __MAPILIB M_errstring(int32 errCode){}

/**********************************************************************/
/** Convert MDIS error code to string
 *  
 * M_errstringTs() creates an error message for error \a errCode and
 * returns a pointer to the generated string with the following
 * format: 
 * 
 * \verbatim
 * ERROR <errtype> <errcode>: <errdescr> 
 * \endverbatim
 *
 * where \em errtype describes if the error code comes from the system
 * or the driver, \em errcode describes the error code in hexadecimal
 * format or in the operating system’s native format. \em errdescr is the
 * corresponding error message string.  
 *
 * \b Examples: 
 * \verbatim
    ERROR (MDIS) 0x1103: MK: illegal parameter
    ERROR (OS9) #000:221: module not found
   \endverbatim
 *
 * \remark This function has been added in MDIS4/2003
 *
 * \param errCode       \IN error code determined by UOS_ErrnoGet()
 * \param strBuf        \OUT filled with error message (should have space
 *                           for 512 characters, including '\\0')
 * \return \a strBuf
 *
 */
char* __MAPILIB M_errstringTs(int32 errCode, char *strBuf){}

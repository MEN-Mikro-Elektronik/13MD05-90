/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  mdis_api.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2014/07/17 12:29:39 $
 *    $Revision: 1.8 $
 *
 *  	 \brief  MDIS user interface for Linux
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mdis_api.c,v $
 * Revision 1.8  2014/07/17 12:29:39  ts
 * R: compiler warning about different prototypes of function tolower()
 * M: renaming tolower() in tolower_mdis() prevents collision with libc tolower
 *
 * Revision 1.7  2009/08/04 17:06:04  CRuff
 * R: make 64bit compatible
 * M: change type of device path to MDIS_PATH for all path occurrences
 *
 * Revision 1.6  2004/06/09 11:12:49  kp
 * M_open/M_setblock arguments changed (const)
 *
 * Revision 1.5  2003/10/07 12:54:54  kp
 * added \menimages for Doxygen
 *
 * Revision 1.4  2003/06/06 09:14:53  kp
 * changed everything for doxygen docu
 *
 * Revision 1.3  2003/02/21 11:34:05  kp
 * added MDIS_CreateDevice, MDIS_OpenBoard and their related Remove functions.
 * Used for RTAI and non-MDIS devices
 *
 * Revision 1.2  2001/08/16 10:15:57  kp
 * added error number decompression for PPC
 *
 * Revision 1.1  2001/01/19 14:59:37  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/LINUX/LIBSRC/MDIS_API/mdis_api.c,v 1.8 2014/07/17 12:29:39 ts Exp $";

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <MEN/men_typs.h>   /* MEN type definitions      */
#include <MEN/mdis_api.h>   /* MDIS api                  */
#include <MEN/mdis_err.h>   /* MDIS error codes          */
#include <MEN/mdis_mk.h>

/*! \mainpage

 This is the documentation of the MDIS API for Linux user space.

 Refer to the \ref mdisapicommonspec "MDIS_API Common Specification" for the
 plain common specification.

 In addition to the standard API calls, Linux implementation provides
 the following calls, which are primarily used by the \b
 mdis_createdev tool:

 - MDIS_CreateDevice(), MDIS_RemoveDevice(), MDIS_OpenBoard(),
   MDIS_RemoveBoard()

*/
/*! \page dummy
 \menimages
*/

static int32 ReadDesc( const char *name, int32 *lenP, char **dataP );

/*
 * DECOMPRESS_ERRNO macro invokes a special routine in case we're running
 * on PowerPC. On this machine, error numbers returned by system calls
 * cannot be higher than 515. So the MDIS kernel compresses MDIS error
 * numbers so that they fit into this range.
 */
#if defined(PPC)
# define DECOMPRESS_ERRNO(x) DecompressErrno(x)
  static int DecompressErrno( int mdisErr );
#else
# define DECOMPRESS_ERRNO(x) (x)
#endif

static int32 DoOpen(
	const char *device,
	int ioctlCode,
	MDIS_OPEN_DEVICE_DATA *moData );

static char *tolower_mdis(char *s)
{
	char *t = s;

	while(*s){
		if( *s>='A' && *s<='Z') *s+='a'-'A';
		s++;
	}
	return t;
}

/**********************************************************************/
/** Open path to device
 *
 * \copydoc mdis_api_specification.c::M_open()
 *
 * \linux Opens the global MDIS device (usually /dev/mdis).
 *	Then reads in a file "/etc/mdis/device.bin" for the descriptor.
 *  Descriptor data passed via ioctl to MDIS driver.
 *
 *  The MDIS kernel will then parse the descriptor for the board name and
 *  returns. M_open will then read the board descriptor from
 *	/etc/mdis/boarddevice.bin and calls again the MDIS ioctl. This second
 *  call will invoke the actual open procedure within the MDIS kernel
 *
 * \sa M_close
 */
MDIS_PATH  M_open(const char *device)
{
	MDIS_OPEN_DEVICE_DATA moData;

	return DoOpen( device, MDIS_OPEN_DEVICE, &moData );
}

/**********************************************************************/
/** Creates an MDIS device
 *
 *  Linux special call. Does almost the same as M_open, but in addition allows
 *  to
 * 	- if \a persist == TRUE, creates the device but does not remove
 *	  the device when the last path is closed
 *	- if \a rtMode == TRUE tells the MDIS kernel to create the device
 *	  for use with RTAI
 *
 * \param device    \IN name of the device e.g. "m22_1"
 * \param rtMode	\IN flags if device is to be used for RTAI
 * \param persist	\IN flags if device shall be removed on last close
 * \return the device handle (>= 0)  on success or \c -1 on error.
 *         On failure, global \em errno is set to the error code
 * \sa M_open, MDIS_RemoveDevice, MDIS_OpenBoard
 */
int32 MDIS_CreateDevice( char *device, int rtMode, int persist )
{
	MDIS_CREATE_DEVICE_DATA crData;

	crData.rtMode = rtMode;
	crData.persist = persist;

	return DoOpen( device, MDIS_CREATE_DEVICE, &crData.d );
}

/**********************************************************************/
/** Removes a perisistant MDIS device
 *
 *  Linux special call.
 * \param device    \IN name of the device e.g. "/m22_1"
 * \return \c 0 on success or \c -1 on error.
 *         On failure, global \em errno is set to the error code
 */
int32 MDIS_RemoveDevice( char *device )
{
	MDIS_OPEN_DEVICE_DATA moData;
	int32 rv;
	int fd;

	/* open global MDIS device */
	if( (fd = open(MDIS_DEV_NAME,O_RDWR)) < 0 )
		return -1;

	if( *device == '/' ) device++; /* skip leading / */

	strncpy( moData.devName, device, sizeof(moData.devName)-1 );
	moData.devName[sizeof(moData.devName)-1] = '\0';

	if( (rv = ioctl( fd, MDIS_REMOVE_DEVICE, &moData )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);

	close(fd);

	return rv;		
}

/**********************************************************************/
/** Creates an BBIS device
 *
 *  Linux special call. Used to initialize a BBIS driver only, without
 *  a MDIS device on it, for example to use non-MDIS drivers.
 *
 * \param device    \IN name of the board device e.g. "d201_1"
 * \return \c 0 on success or \c -1 on error.
 *         On failure, global \em errno is set to the error code
 * \sa M_open, MDIS_CreateDevice, MDIS_RemoveBoard
 */
int32 MDIS_OpenBoard( char *device )
{
	int fd, rv=-1;
	char *brdData = NULL;
	MDIS_OPEN_DEVICE_DATA moData;

	/* open global MDIS device */
	if( (fd = open(MDIS_DEV_NAME,O_RDWR)) < 0 )
		return -1;

	/* read device descriptor */
	if( ReadDesc( device, &moData.brdDescLen, &brdData ) < 0 )
		goto error;

	/*--- perform ioctl to pass board descriptor to MDIS ---*/
	moData.brdData 		= (void *)brdData;
	strcpy( moData.brdName, device );
	moData.devName[0] 	= '\0';
	moData.devDescLen	= 0;
	moData.devData		= NULL;

	rv = ioctl( fd, MDIS_OPEN_BOARD, &moData );
	if( rv < 0 )
		errno = DECOMPRESS_ERRNO(errno);
 error:
	close(fd);

	return rv;
}

/**********************************************************************/
/** Deinit BBIS device if its use count is 0
 *
 *  Linux special call.
 *
 * \param device    \IN name of the board device e.g. "d201_1"
 * \return \c 0 on success or \c -1 on error.
 *         On failure, global \em errno is set to the error code
 * \sa M_open, MDIS_CreateDevice, MDIS_OpenBoard
 */
int32 MDIS_RemoveBoard( char *device )
{
	MDIS_OPEN_DEVICE_DATA moData;
	int32 rv;
	int fd;

	/* open global MDIS device */
	if( (fd = open(MDIS_DEV_NAME,O_RDWR)) < 0 )
		return -1;

	if( *device == '/' ) device++; /* skip leading / */

	strncpy( moData.brdName, device, sizeof(moData.brdName)-1 );
	moData.brdName[sizeof(moData.brdName)-1] = '\0';

	if( (rv = ioctl( fd, MDIS_REMOVE_BOARD, &moData )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);

	close(fd);

	return rv;		
}

static int32 ReadDesc( const char *name, int32 *lenP, char **dataP )
{
	char descFileName[128];
	FILE *descFp = NULL;
	size_t len;
	int rv = -1;

	/*--- read the descriptor from file ---*/	
	if( strlen(name) >
		(sizeof(descFileName) - strlen(MDIS_DESCRIPTOR_DIR) - 6 )){

		errno = E2BIG;
		goto error;
	}
			
	/*
	 * check for slashes in device name. only a leading slash is allowed
	 */
	if( *name == '/' ) name++; /* skip leading / */
	{
		const char *p = name;
		while( *p )
			if( *p++ == '/' ){
				errno = EISDIR;
				goto error;
			}
	}

	/*--- read the descriptor from file ---*/
	strcpy( descFileName, MDIS_DESCRIPTOR_DIR );
	strcat( descFileName, "/" );
	strcat( descFileName, name );
	strcat( descFileName, ".bin" );
	tolower_mdis( descFileName );

	if( (descFp = fopen( descFileName, "rb" )) == NULL )
		goto error;

	/* determine length of file */
	fseek( descFp, 0, SEEK_END );
	len = ftell( descFp );
	fseek( descFp, 0, SEEK_SET );

	/* alloc buffer for data */
	if( (*dataP = malloc( len )) == NULL ){
		errno = ENOMEM;
		goto error;
	}

	if( fread( *dataP, 1, len, descFp ) != len )
		goto error;

	*lenP = len;

	if( descFp ) 
	  fclose( descFp );

	return 0;

 error:
	if( *dataP && rv < 0) {
		free( *dataP );
		*dataP = NULL;
	}

        if( descFp )
          fclose( descFp );

	return rv;
}

static int32 DoOpen(
	const char *device,
	int ioctlCode,
	MDIS_OPEN_DEVICE_DATA *moData )
{
	int fd, rv=-1;
	char *devData = NULL;
	char *brdData = NULL;

	if( *device == '/' ) device++; /* skip leading / */

	/* open global MDIS device */
	if( (fd = open(MDIS_DEV_NAME,O_RDWR)) < 0 )
		return -1;

	/* read device descriptor */
	if( ReadDesc( device, &moData->devDescLen, &devData ) < 0 )
		goto error;

	moData->devData 		= (void *)devData;

	/*--- perform ioctl to pass device descriptor to MDIS ---*/
	strcpy( moData->devName, device );
	moData->brdName[0] 	= '\0';
	moData->brdDescLen	= 0;
	moData->brdData		= NULL;

	if( ioctl( fd, ioctlCode, moData ) < 0)
		goto error;

	/*--- ok, MDIS kernel returned name of board device, read board desc ---*/

	/* read device descriptor */
	if( ReadDesc( moData->brdName, &moData->brdDescLen, &brdData ) < 0 )
		goto error;

	moData->brdData 	= (void *)brdData;

	if( ioctl( fd, ioctlCode, moData ) < 0){
		goto error;
	}
	rv = fd;
error:	
	if( brdData ) free(brdData);
	if( devData ) free(devData);
	if( rv < 0 ){
		errno = DECOMPRESS_ERRNO(errno);
		close(fd);
	}
	return rv;
}

/**********************************************************************/
/** Close path to device
 *
 * \copydoc mdis_api_specification.c::M_close()
 * \sa M_open
 */
int32  M_close(MDIS_PATH path)
{
	int32 rv;
	if( (rv = close(path)) < 0 )
		errno = DECOMPRESS_ERRNO(errno);
	
	return rv;
}

/**********************************************************************/
/** Read 32-bit integer value from device
 *
 * \copydoc mdis_api_specification.c::M_read()
 * \sa M_getblock, M_write
 */
int32 M_read(MDIS_PATH path, int32 *valueP)
{
	int32 rv;
	if( (rv = ioctl( path, MDIS_READ, valueP )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);
	
	return rv;
}

/**********************************************************************/
/** Write 32-bit integer value to device
 *
 * \copydoc mdis_api_specification.c::M_write()
 * \sa M_setblock, M_read
 */	
int32 M_write(MDIS_PATH path, int32 value)
{
	int32 rv;
	if( (rv = ioctl( path, MDIS_WRITE, value )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);
	
	return rv;		
}

/**********************************************************************/
/** Get status from device
 *
 * \copydoc mdis_api_specification.c::M_getstat()
 * \sa M_setstat
 */	
int32 M_getstat(MDIS_PATH path, int32 code, int32 *dataP)
{
	MDIS_LINUX_SGSTAT sg;
	int32 rv;

	sg.code = code;
	sg.p.data = (void *)dataP;
	if( (rv = ioctl( path, MDIS_GETSTAT, &sg )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);
	
	return rv;		
}


/**********************************************************************/
/** Set status of device
 *
 * \copydoc mdis_api_specification.c::M_setstat()
 * \sa M_getstat
 */	
int32 M_setstat(MDIS_PATH path, int32 code, INT32_OR_64 data)
{
	MDIS_LINUX_SGSTAT sg;
	int32 rv;

	sg.code = code;
	sg.p.value = data;
	if( (rv = ioctl( path, MDIS_SETSTAT, &sg )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);
	
	return rv;
}

/**********************************************************************/
/** Read data block from device
 *	
 * \copydoc mdis_api_specification.c::M_getblock()
 * \sa M_read, M_setblock
 */
int32 M_getblock(MDIS_PATH path, u_int8 *buffer, int32 length)
{
	int32 rv;
	if( (rv = read( path, buffer, length )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);
	
	return rv;		
}

/**********************************************************************/
/** Write data block to device
 *	
 * \copydoc mdis_api_specification.c::M_setblock()
 * \sa M_write, M_getblock
 */
int32 M_setblock(MDIS_PATH path, const u_int8 *buffer, int32 length)
{
	int32 rv;
	if( (rv = write( path, buffer, length )) < 0 )
		errno = DECOMPRESS_ERRNO(errno);
	
	return rv;		
}

#if defined(PPC)
/****************************** DecompressErrno *******************************
 *
 *  Description:  PowerPC special routine to decompress MDIS error numbers
 *
 * On this machine, error numbers returned by system calls
 * cannot be higher than 515. So MK compressed MDIS error numbers so
 * they fit into this range.
 * This routine converts them to the original values
 *---------------------------------------------------------------------------
 *  Input......:  compErr		compressed MDIS error code
 *  Output.....:  returns		decompressed error code
 *  Globals....:  -
 ****************************************************************************/
static int DecompressErrno( int compErr )
{	
	MK_ERRNO_COMPRESSION_TABLE;
	const MDIS_ERRNO_COMPRESSION_TABLE *p = mdisErrnoCompressionTable;
	int rv;

	if( compErr < 0xa0 ) return compErr; /* non MDIS error */

	for( ; p->orgStart>0; p++ ){
		if( compErr >= p->compStart && compErr <= p->compEnd ){
			rv = compErr - p->compStart + p->orgStart;
			return rv;
		}
	}
	return EINVAL;				/* not found in compression table */
}

#endif /* PPC */

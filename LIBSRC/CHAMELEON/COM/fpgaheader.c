/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  fpgaheader.c
 *
 *      \author  klaus.popp@men.de/rene.lange@men.de
 *        $Date: 2005/06/23 16:12:09 $
 *    $Revision: 1.5 $
 *
 *	   \project  -
 *  	 \brief  Routines to parse and interpret MEN (chameleon) FPGA
 *				 header/data.
 *
 *    \switches   _LITTLE_ENDIAN_ / _BIG_ENDIAN_, MENMON
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: fpgaheader.c,v $
 * Revision 1.5  2005/06/23 16:12:09  kp
 * Copyright line changed (sbo)
 *
 * Revision 1.4  2005/05/04 08:59:46  kp
 * The filenames of new-style FGPAs (e.g. EM04n01IC002A1) are no longer checked
 * against the CPU name/model
 *
 * Revision 1.3  2005/02/08 12:55:16  kp
 * + support for FPGA long header
 *
 * Revision 1.2  2005/01/14 10:01:02  kp
 * cosmetic
 *
 * Revision 1.1  2004/12/20 08:46:34  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#ifdef MENMON
# include <mmglob.h>			/* for strncmp */
#else
# include <string.h>
# include <MEN/men_typs.h>
#endif

#include <MEN/fpga_header.h>
#include <MEN/chameleon.h>


/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
#ifdef _BIG_ENDIAN_
# define _SWAP_BE16(w) (w)
# define _SWAP_BE32(l) (l)
#elif defined(_LITTLE_ENDIAN_)
# define _SWAP_BE16(w) ((((w)&0xff)<<8) + (((w)&0xff00)>>8))
# define _SWAP_BE32(l) ((((l)&0xff)<<24) + (((l)&0xff00)<<8) + \
					 (((l)&0xff0000)>>8) + (((l)&0xff000000)>>24))
#else
# error "Must define _BIG_ENDIAN_ or _LITTLE_ENDIAN_"
#endif

#define _ISDIGIT(c) ((c)>='0' && (c)<='9')

#define FPGA_HEADER_SHORT_SIZE 0x030
#define FPGA_HEADER_LONG_SIZE  0x100


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

static void SafeStrnCpy( char *dst, const char *src, int n )
{
	strncpy( dst, src, n-1);
	dst[n] = '\0';
}

/**********************************************************************/
/** Scan decimal positive number until non digit found
 *
 *  \param s 	\IN string to scan
 *  \param numP	\IN receives converted value
 *  \return pointer to first non digit found
 */
static const char *ScanNumber( const char *s, u_int32 *numP )
{
	u_int32 num = 0;
	
	while( _ISDIGIT( *s )) {
		num = num * 10 + (*s - '0');
		
		s++;
	}
	*numP = num;

	return s;
}


/**********************************************************************/
/** Compute info fields from filename
 *
 *  \return 0=ok, or error FPGA_HEADER_CODE_ABSENT
 */
static int32 FilenameScan( const char *fileName, FPGA_HEADER_INFO *info )
{
	int32 rv = FPGA_HEADER_CODE_ABSENT;
	u_int32 valU32;
	const char *s;

	if( (strlen( fileName ) > 10 )
		&& fileName[7] == 'I' && fileName[8] == 'C' ){
		/* format according to men ims-07-29 part 2) */

		SafeStrnCpy( info->boardType, &fileName[0], 8 );	
		
		/* parse IC number */
		s = ScanNumber( &fileName[ 9 ], &valU32 );
		if( s != &fileName[12] )
			return rv;			/* IC number must be followed by variant */

		info->icNo	  = valU32;
		
	}
	else {
		/* old EM04 style filename */
		SafeStrnCpy( info->boardType, &fileName[0], 6 );	
		SafeStrnCpy( info->cBoardType, &fileName[5], 6 );	
	
		/* remove trailing '-' at board name */
		if( info->cBoardType[4] == '-' )
			info->cBoardType[4] = '\0';

		/* parse IC number */
		s = ScanNumber( &fileName[ 10 ], &valU32 );
		if( !(*s >= 'A' && *s <= 'Z') )
			return rv;			/* IC number must be followed by variant */

		info->icNo	  = valU32;
	}

	info->variant = *s++;

	/* parse revision */
	s = ScanNumber( s, &valU32 );
	if( *s )
		return rv;				/* revision number must be at EOS */
	info->revision	  = valU32;

	return 0;
}




/**********************************************************************/
/** Identify short FPGA header
 *
 *  Also compute IC number, variant code and revision code from FPGA
 *  code name
 *
 *  See parameters of ChameleonFpgaHeaderIdent
 *
 */
static int32 ChameleonFpgaHeader1Ident(
	const void *_hdr,
	u_int32 maxSize,
	FPGA_HEADER_INFO *info)
{
	FPGA_HEADER *hdr = (FPGA_HEADER *)_hdr;
	u_int32 size;
	int32 rv = FPGA_HEADER_CODE_ABSENT;

	/* check if header contains magic */
	if( _SWAP_BE32(hdr->magic) != FPGA_HEADER_MAGIC )
		return rv;

	/* check if size is within limits */
	size = _SWAP_BE32(hdr->size);
	info->size 		= size;
	info->totSize 	= size + FPGA_HEADER_SHORT_SIZE;
	info->xorSum	= hdr->chksum;
	info->data		= (u_int8 *)hdr + FPGA_HEADER_SHORT_SIZE;

	if( info->totSize > maxSize )
		return rv;

	SafeStrnCpy( info->fileName, hdr->fileName, sizeof(info->fileName) );
	SafeStrnCpy( info->fpgaType, hdr->fpgaType, sizeof(info->fpgaType) );

	return FilenameScan( info->fileName, info );
}

/**********************************************************************/
/** Identify long FPGA header
 *
 *  Also compute IC number, variant code and revision code from FPGA
 *  code name
 *
 *  See parameters of ChameleonFpgaHeaderIdent
 *
 */
static int32 ChameleonFpgaHeader2Ident(
	const void *_hdr,
	u_int32 maxSize,
	FPGA_HEADER_INFO *info)
{
	FPGA_LONGHEADER *hdr = (FPGA_LONGHEADER *)_hdr;
	u_int32 size;
	int32 rv = FPGA_HEADER_CODE_ABSENT;

	/* check if header contains magic */
	if( _SWAP_BE32(hdr->magic) != FPGA_LONGHEADER_MAGIC )
		return rv;

	/* check if size is within limits */
	size = _SWAP_BE32(hdr->size);
	info->size 		= size;
	info->totSize 	= size + FPGA_HEADER_LONG_SIZE;
	info->xorSum	= hdr->chksum;
	info->data		= (u_int8 *)hdr + FPGA_HEADER_LONG_SIZE;

	if( info->totSize > maxSize )
		return rv;

	SafeStrnCpy( info->fileName, hdr->fileName, sizeof(info->fileName) );
	SafeStrnCpy( info->fpgaType, hdr->fpgaType, sizeof(info->fpgaType) );

	return FilenameScan( info->fileName, info );
}

/**********************************************************************/
/** identify FPGA header in memory, check everything but code XOR
 *
 *		Valid for formats:
 *
 *		- FPGA_HEADER and FPGA_LOGHEADER
 *		- header fileName interpretation according to
 *			- old format, as used on EM04
 *			- men ims-07-29, item 2)
 *
 *
 *  \param _hdr		\IN	pointer to FPGA header (either format)
 *	\param maxSize	\IN max size for header + FPGA netto data
 *  \param info		\OUT filled with info
 *
 *  \return 0=ok, or error FPGA_HEADER_CODE_ABSENT
 */
int32 ChameleonFpgaHeaderIdent(
	const void *_hdr,
	u_int32 maxSize,
	FPGA_HEADER_INFO *info)
{
	int32 rv;

	memset( info, 0, sizeof(*info));
	info->variant = -1;
	info->revision = -1;
	info->icNo = -1;

	if( (rv = ChameleonFpgaHeader1Ident( _hdr, maxSize, info )) == 0 )
		return rv;
		
	if( (rv = ChameleonFpgaHeader2Ident( _hdr, maxSize, info )) == 0 )
		return rv;

	return rv;
}


/**********************************************************************/
/** Compute 32 Bit XOR over a data block
 *
 *  \param p		\IN start address
 *	\param nBytes	\IN number of bytes to check (aligned to 4)
 *  \param xorP		\OUT receives computed checksum
 *
 *  \return 0=ok, or error FPGA_HEADER_BAD_PARAM
 */
int32 ChameleonFpgaDataXor( const u_int32 *p, u_int32 nBytes, u_int32 *xorP )
{
	u_int32 xor, i;

	if( nBytes & 3 )
		return FPGA_HEADER_BAD_PARAM;

	nBytes /= 4;

	for( xor=0,i=0; i<nBytes; i++ )
		xor ^= *p++;

	*xorP = xor;
	return 0;
}

/**********************************************************************/
/** Check XOR of FPGA data
 *  \return 0=ok, or error FPGA_DATA_CHKSUM_ERR
 */
int32 ChameleonFpgaDataCheckXor( FPGA_HEADER_INFO *info)
{
	u_int32 xor;
	int32 rv;

	if( (rv = ChameleonFpgaDataXor( (u_int32 *)info->data, info->size, &xor ))
		!= 0 )
		return rv;

	if( info->xorSum != xor )
		return FPGA_DATA_CHKSUM_ERR;

	return 0;
}

static char UCase( char c )
{
	if( c >= 'a' && c <= 'z' )
		c += 'A' - 'a';
	return c;
}

/**********************************************************************/
/** Compare a "X" masked string \a s1 with string \a s2
 * \return 0 if all chars matched ok, >0 if not
 */	
static int StrnCmpXMask( const char *s1, const char *s2, int n )
{
	char c1, c2;

	while (1) {
		if( n==0 )
			return 0;
		c1 = UCase(*s1);
		c2 = UCase(*s2);

		if ((c1 != 'X') && (c1 != c2))
			return(c1 - c2);
		if (c1 == 0 )
			return(0);

		n--;
		s1++;
		s2++;
	}
}

/**********************************************************************/
/** Check if FPGA header filename is ok for board
 *
 * uses info->boardType and info->cBoardType for checking	
 *
 * \param info				\IN info from ChameleonFpgaHeaderIdent()
 * \param mainProductName 	\IN name of product to check (chars 3..9 of
 *								MEN product name)
 *								(NULL or "" if unknown)
 * \param carrierName 		\IN name of carrier to check (chars 3..7) of
 *								MEN product name)
 *								(NULL or "" if unknown). Will be checked
 *								for old header filename format.
 *	
 *
 *  \return 0=ok, or error FPGA_
 */
int32 ChameleonFpgaHeaderOkForBoard(
	const FPGA_HEADER_INFO *info,
	const char *mainProductName,
	const char *carrierName)
{

	if( (mainProductName == NULL) || (mainProductName[0] == '\0'))
		return 0;			/* running on unknown board, assume ok */


	if( info->cBoardType[0] == '\0' ){
		/* new filename format */
		if( strlen( mainProductName ) != 7 )
			return FPGA_HEADER_BAD_PARAM;
		
#if 0
		/* we do no longer check the names for new filename format! */
		if( StrnCmpXMask( info->boardType, mainProductName, 7 ) != 0 )
			return FPGA_CODE_MISMATCH;		
#endif
	}
	else {
		/* old filename format */

		if( strlen( mainProductName ) < 5 )
			return FPGA_HEADER_BAD_PARAM;
		
		if( StrnCmpXMask( info->boardType, mainProductName, 5 ) != 0 )
			return FPGA_CODE_MISMATCH;		

		if( (carrierName == NULL) || (carrierName[0] == '\0'))
			return 0;/* running on unknown carrier board, assume ok */

		if( strncmp( info->cBoardType, "XXXX", 4 ) != 0 ){

			if( StrnCmpXMask( info->cBoardType, carrierName, 5 ) != 0 )
				return FPGA_CODE_MISMATCH;		   			
		}
	}	

	return 0;
}

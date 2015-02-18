/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: desc.c
 *      Project: mdis 4.0 / ll-driver / bbis
 *
 *      $Author: UFranke $
 *        $Date: 2010/09/02 15:15:08 $
 *    $Revision: 1.16 $
 *
 *  Description: Descriptor decoder functions
 *               (for c-struct and binary descriptors)
 *
 *     Required: oss.l
 *     Switches: DBG		enable debugging
 *               INCLUDE_MIPIOS_VX
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *  DESC_Ident             Gets the pointer to ident string.
 *  DESC_Init              Get handle to access descriptor
 *  DESC_Exit              Terminate use of descriptor handle
 *  DESC_GetUInt32         Get descriptor entry of type "U_INT32"
 *  DESC_GetBinary         Get descriptor entry of type "BINARY"
 *  DESC_GetString         Get descriptor entry of type "STRING"
 *  DESC_DbgLevelSet       Sets the debug level of this module.
 *  DESC_DbgLevelGet       Gets the debug level of this module.
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: desc.c,v $
 * Revision 1.16  2010/09/02 15:15:08  UFranke
 * cosmetics
 *
 * Revision 1.15  2010/04/21 15:56:25  CRuff
 * R:1. Porting to MDIS5 (according to porting guide rev. 0.9)
 * M:1. convert format of pointer / MACCESS debug print to 0x08p
 *
 * Revision 1.14  2009/08/04 17:13:27  CRuff
 * R: pointer casts invalid on 64bit systems
 * M: in function GetTag(): cast pointer to U_INT32_OR_64 instead of u_int32
 *    before pointer arithmetics
 *
 * Revision 1.13  2009/03/31 11:00:28  ufranke
 * changed
 *  - EXCLUDE_MIPIOS_VX to INCLUDE_MIPIOS_VX
 *
 * Revision 1.12  2008/12/09 09:20:06  ufranke
 * R: MIPIOS needs to know the size of the low level descriptor
 * M: added function DESC_GetDescSize()
 *
 * Revision 1.11  2008/05/07 11:27:01  ts
 * fixed compiler warning "line 180: descIntHdl might be used uninitialized in
 * this function" by initializing the pointer with NULL
 *
 * Revision 1.10  2004/03/19 11:47:37  ub
 * replaced u_char by u_int8
 *
 * Revision 1.9  2001/12/10 16:45:10  Franke
 * Bugfix
 *    1. if a key was searched in the root, not found there
 *       but found in a subdirectory, this value was given back
 *    2. DESC_GetBinary the actual number of bytes was copied
 *       was wrong at default
 *
 * Revision 1.8  1999/04/27 14:16:13  kp
 * mark static functions with nodoc
 *
 * Revision 1.7  1998/06/23 17:32:32  Franke
 * removed bug in DESC_GetUInt32() at DBGWRT_2() default value dump
 *
 * Revision 1.6  1998/06/15 17:33:36  Franke
 * removed bug in DESC_Init() first desc byte check big/little endian
 *
 * Revision 1.5  1998/06/15 10:43:40  Franke
 * changed DBG_ALL => OSS_DBG_DEFAULT
 *
 * Revision 1.4  1998/05/29 16:49:00  see
 * DESC_Init: check first byte to detect corrupted descriptor
 * DESC_GetXXX: debug messages added: print result
 * MakeKeyString: pass desc handle instead oss handle to allow debug
 * use new macros for error messages
 *
 * Revision 1.3  1998/04/17 15:34:34  see
 * DESC_GetTag: incrementing tagP: casting changed since OS-9 compiler error
 * DESC_GetTag: renamed to GetTag since it is static
 * DESC_MakeKeyString: renamed to MakeKeyString since it is static
 * MakeKeyString: using OSS_Vsprintf instead sprintf
 * MakeKeyString: local prototype added
 * MakeKeyString: no debug output possible since no desc handle passed
 * DBG macros updated
 * global dbgLev is now stored in descIntHdl
 * include dbg.h
 * DESC_Init: no debug output possible if MemGet failed
 * DESC_Exit: no debug output possible if MemFree failed
 * header: public functions added
 * max key length MAX_LEN is now DESC_MAX_KEYLEN in desc.h
 * DESC_DbgLevelSet/Get: desc handle as first arg added
 * global IdentString removed, is now returned directly in DESC_Ident
 * DESC_GetXXX: print keyFmt at DBGWRT
 *
 * Revision 1.2  1998/03/25 13:59:28  Franke
 * static DebLev disabled for OS9 no debug driver
 *
 * Revision 1.1  1998/03/10 12:21:32  franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997..2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: desc.c,v 1.16 2010/09/02 15:15:08 UFranke Exp $";

#include <MEN/men_typs.h>         /* men type definitions */
#include <MEN/mdis_err.h>
#include <MEN/oss.h>

#include <MEN/desctyps.h>
#include <MEN/desc.h>

/* pass debug definitions to dbg.h */
#define DBG_MYLEVEL		descIntHdl->dbgLev
#include <MEN/dbg.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/* internal descriptor handle */
typedef struct {
   u_int32     OwnMemSize;
   OSS_HANDLE  *osHdl;				/* oss handle */
   DBG_HANDLE  *dbgHdl;				/* debug handle */
   u_int32     dbgLev;				/* debug level */
   void        *descStructP;		/* ptr to descr data */
} DESC_INT_HDL;

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/* debug handle */
#define DBH		descIntHdl->dbgHdl

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int32 MakeKeyString(
    DESC_INT_HDL  *descIntHdl,
    char        *keyFmt,
    char        **keyStringP,
    u_int32     *allocatedSizeP,
    va_list     argptr);

static int32 GetTag
(
    DESC_INT_HDL  *descIntHdl,
    u_int16     tagKind,
    u_int16     **tagPP,
    char        *keyString,
    u_int32		*descSizeP
);

/*****************************  DESC_Ident  *********************************
 *
 *  Description:  Gets the pointer to ident string.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  return  pointer to ident string
 *
 *  Globals....:  -
 ****************************************************************************/
char* DESC_Ident( void )
{
	return("DESC - Descriptor Decoder: $Id: desc.c,v 1.16 2010/09/02 15:15:08 UFranke Exp $");
}/*DESC_Ident*/


/********************************* DESC_Init ********************************
 *
 *  Description: Get handle to access descriptor
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: descSpec       descriptor specifier (here: ptr to descriptor)
 *               osHdl          OS specific data
 *               descHandleP    ptr to variable where handle will be stored
 *
 *  Output.....: Return:        error code
 *               *descHandleP   handle for descriptor access
 *  Globals....: -
 ****************************************************************************/
int32 DESC_Init(
DESC_SPEC   *descSpec,
OSS_HANDLE  *osHdl,
DESC_HANDLE **descHandleP
)
{
    u_int32      gotsize;
    DESC_INT_HDL *descIntHdl = NULL;

	/* first byte of word must be 0x6d */
	if( (*(u_int16*)descSpec & 0xff00) != 0x6d00 )
       return( ERR_DESC_CORRUPTED );
	

	/* allocate handle */
    descIntHdl = (DESC_INT_HDL*) OSS_MemGet( osHdl,
											 sizeof(DESC_INT_HDL),
                                             &gotsize );
    *descHandleP = (DESC_HANDLE*) descIntHdl;

    if( descIntHdl == NULL )
        return( ERR_OSS_MEM_ALLOC );

    /* fill turkey with 0 */
    OSS_MemFill( osHdl, gotsize, (char*) descIntHdl, 0 );

    /* fill up the turkey */
    descIntHdl->OwnMemSize  = gotsize;
    descIntHdl->osHdl       = osHdl;
    descIntHdl->descStructP = descSpec;

	/* prepare debugging */
	DBG_MYLEVEL = OSS_DBG_DEFAULT;		/* set OS specific debug level */
	DBGINIT((NULL,&DBH));
    DBGWRT_1((DBH,"DESC - DESC_Init: descSpec=0x%p\n", descSpec));

    return( 0 );
}/*DESC_Init*/


/********************************* DESC_Exit ********************************
 *
 *  Description: Terminate use of descriptor handle
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: descHandleP    Descriptor handle
 *
 *  Output.....: descHandleP    is set to NULL
 *               Return:        error code
 *  Globals....: -
 ****************************************************************************/
int32 DESC_Exit( DESC_HANDLE **descHandleP )
{
    DESC_INT_HDL  *descIntHdl = (DESC_INT_HDL*) *descHandleP;

    DBGWRT_1((DBH,"DESC - DESC_Exit\n"));
	DBGEXIT((&DBH));

    if( OSS_MemFree( descIntHdl->osHdl, (int8*) descIntHdl,
					descIntHdl->OwnMemSize ))
        return( ERR_OSS_MEM_FREE );

   *descHandleP = 0;
   return( 0 );
}/*DESC_Exit*/

#ifdef INCLUDE_MIPIOS_VX
	/********************************* DESC_GetDescSize *************************
	 *
	 *  Description: Gets the size of the descriptor.
	 *
	 *
	 *---------------------------------------------------------------------------
	 *  Input......:  descHandle    handle for descriptor access
	 *
	 *  Output.....: descSizeP      size of the descriptor
	 *               Return:        0 | error code
	 *  Globals....: -
	 ****************************************************************************/
	int32 DESC_GetDescSize( DESC_HANDLE *descHandle, u_int32 *descSizeP )
	{
	    int32   error = ERR_DESC_CORRUPTED;
		u_int32 size = 0;
	    DESC_INT_HDL  *descIntHdl = (DESC_INT_HDL*) descHandle;
	    u_int16 *tagP = NULL;
	
		if(	descHandle == NULL || descSizeP == NULL )
		{
			goto CLEANUP;
		}
	
	    error = GetTag( descIntHdl, DESC_U_INT32, &tagP, "we_dont_want_to_find_this_key_to_go_until_end_of_descriptor__", &size );
	    if( error == ERR_DESC_KEY_NOTFOUND )
	    {
	    	error = 0;
		}
	
	CLEANUP:
		if( error )
		{
	    	DBGWRT_ERR((DBH,"*** DESC - %s: size %d error %x ***\n", __FUNCTION__, size, error ));
	    }
		*descSizeP = size;
		return( error );
	}
#endif /*INCLUDE_MIPIOS_VX*/

/********************************** DESC_GetUInt32 ************************
 *
 *  Description:  Get descriptor entry of type "U_INT32"
 *
 *  Searches the descriptor for the specified key and return its value. If
 *  any error occurs or if the key is not found, the default value is
 *  returned.
 *
 *  The <keyFmt> has the same format as the printf format argument. The
 *  variable argument list is then used to expand the <keyFmt> argument.
 *
 *  Example:
 *      "CHANNEL_%d/GAIN", 2
 *  expands to
 *      "CHANNEL_2/GAIN"
 *
 *  Subdirectories in the descriptor must be separated with a '/' character
 *
 *  NOTE: ERR_DESC_KEY_NOTFOUND is returned when default value used
 *
 *---------------------------------------------------------------------------
 *  Input......:  descHandle    handle for descriptor access
 *                defVal        default value if key not found
 *                valueP        ptr to variable where value will be stored
 *                keyfmt        format string for key
 *                ...           arguments for <keyfmt>
 *
 *  Output.....:  Return:       error code
 *                *valueP       value of key or default value if key not found
 *  Globals....:  ---
 ****************************************************************************/
int32 DESC_GetUInt32(
DESC_HANDLE *descHandle,
u_int32     defVal,
u_int32     *valueP,
char        *keyFmt,
...
)
{
    u_int16 *tagP = NULL;
    u_int16 *lenP = NULL;
    u_int32 *valP = NULL;
    int32   retVal;
    char    *keyString = NULL;
    u_int32 gotSize;
    va_list argptr;
    DESC_INT_HDL  *descIntHdl = (DESC_INT_HDL*) descHandle;
	u_int32 size = 0;

    DBGWRT_1((DBH,"DESC - DESC_GetUInt32:\n"));

    tagP = NULL;

    va_start( argptr, keyFmt );
    retVal = MakeKeyString( descIntHdl, keyFmt, &keyString, &gotSize, argptr );
    va_end( argptr );
    if( retVal == 0 )
    {
        retVal = GetTag( descIntHdl, DESC_U_INT32, &tagP, keyString, &size );
        if( retVal == 0 )
        {
            lenP = ( tagP + 1 );
            valP = (u_int32*) ((u_int8*)tagP + *lenP); /* go to the last byte of entry + 1 */
            *valueP = *valP;
			DBGWRT_2((DBH," 0x%08x (found)\n",*valP));
        }
        else
        {
            *valueP = defVal;
			DBGWRT_2((DBH," 0x%08x (default)\n",*valueP));
        }/*if*/
        OSS_MemFree( descIntHdl->osHdl, keyString, gotSize );
    }

    return( retVal );
}/*DESC_GetUInt32*/


/********************************** DESC_GetBinary *************************
 *
 *  Description:  Get descriptor entry of type "BINARY"
 *
 *  Searches the descriptor for the specified key and copy the binary array
 *  from the descriptor into the caller's buffer. If any error occurs or if
 *  the key is not found, the default array is copied to the caller's buffer
 *
 *  See further notes on DESC_GetUInt32
 *
 *  NOTE: ERR_DESC_KEY_NOTFOUND is returned when default value used
 *
 *---------------------------------------------------------------------------
 *  Input......:  descHandle    handle for descriptor access
 *                defVal        array with default data
 *                defValLen     number of bytes in <defVal> array
 *                buf           callers buffer where data will be stored
 *                lenP          ptr to buffer length
 *                              IN: *lenp = size of <buf> array
 *                keyFmt        format string for key
 *                ...           arguments for <keyFmt>
 *
 *  Output.....:  Return:       error code
 *                buf           array entries of key or default data
 *                *lenP         actual number of bytes copied
 *  Globals....:  ---
 ****************************************************************************/
int32 DESC_GetBinary
(
    DESC_HANDLE *descHandle,
    u_int8      *defVal,
    u_int32     defValLen,
    u_int8      *bufP,
    u_int32     *lenP,
    char        *keyFmt,
    ...
)
{
    u_int16 *tagP;
    u_int16 *tagLenP;
    u_int32 arrLen;
    u_int8  *srcBuf;
    int32   retVal;
    char    *name;
    int     nameLen;
    u_int8  pad;
    char    *keyString;
    u_int32 gotSize;
    va_list argptr;
    DESC_INT_HDL  *descIntHdl = (DESC_INT_HDL*) descHandle;
	u_int32 size = 0;


    DBGWRT_1((DBH,"DESC - DESC_GetBinary: \n"));

    tagP = NULL;

    va_start( argptr, keyFmt );
    retVal = MakeKeyString( descIntHdl, keyFmt, &keyString, &gotSize, argptr );
    va_end( argptr );
    if( retVal == 0 )
    {
        retVal = GetTag( descIntHdl, DESC_BINARY, &tagP, keyString, &size );
        if( retVal == 0 )
        {
            tagLenP = ( tagP + 1 );
            name  = (char*) tagP;
            name += 4;
            nameLen = OSS_StrLen( descIntHdl->osHdl, name );
            pad = *((u_int8*)(name + nameLen + 1));
            arrLen  = *tagLenP - ( 2 + nameLen + pad );

            if( *lenP < arrLen )
            {
                OSS_MemFree( descIntHdl->osHdl, keyString, gotSize );
                return( ERR_DESC_BUF_TOOSMALL );
            }/*if*/

            srcBuf = (u_int8*)(name + OSS_StrLen( descIntHdl->osHdl, name ) + 2);
            *lenP = arrLen;
            OSS_MemCopy( descIntHdl->osHdl, arrLen, (char*) srcBuf, (char*) bufP );
			DBGDMP_2((DBH," (found)",bufP,arrLen,1));
        }
        else
        {
            /* default array */
            *lenP = defValLen;
            OSS_MemCopy( descIntHdl->osHdl, defValLen, (char*) defVal, (char*) bufP );
			DBGDMP_2((DBH," (default)",bufP,defValLen,1));
        }/*if*/

        OSS_MemFree( descIntHdl->osHdl, keyString, gotSize );
    }

    return( retVal );
} /*DESC_GetBinary*/

/********************************** DESC_GetString **************************
 *
 *  Description:  Get descriptor entry of type "STRING"
 *
 *  Searches the descriptor for the specified key and copy the string
 *  from the descriptor into the caller's buffer. If any error occurs or if
 *  the key is not found, the default string is copied to the caller's buffer
 *
 *  See further notes on DESC_GetUInt32
 *
 *  NOTE: ERR_DESC_KEY_NOTFOUND is returned when default value used
 *
 *---------------------------------------------------------------------------
 *  Input......:  descHandle    handle for descriptor access
 *                defVal        default string
 *                bufP          callers buffer where string will be stored
 *                lenP          ptr to buffer length
 *                              IN: *lenp = size of <buf>
 *                keyFmt        format string for key
 *                ...           arguments for <keyfmt>
 *
 *  Output.....:  Return:       error code
 *                bufP          string of key or default string
 *                *lenP         actual number of characters copied (incl.
 *                              terminating '\0')
 *  Globals....:  ---
 ****************************************************************************/
int32 DESC_GetString(
DESC_HANDLE *descHandle,
char    *defVal,
char    *bufP,
u_int32 *lenP,
char    *keyFmt,
...
)
{
    u_int16 *tagP;
    u_int16 *tagLenP;
    u_int32 arrLen;
    u_int8  *srcBuf;
    int32   retVal;
    char    *name;
    char    *keyString;
    u_int32 gotSize;
    va_list argptr;
    DESC_INT_HDL  *descIntHdl = (DESC_INT_HDL*) descHandle;
	u_int32 size = 0;

    DBGWRT_1((DBH,"DESC - DESC_GetString:\n"));

    tagP = NULL;

    va_start( argptr, keyFmt );
    retVal = MakeKeyString( descIntHdl, keyFmt, &keyString, &gotSize, argptr );
    va_end( argptr );

    if( retVal == 0 )
    {
        retVal = GetTag( descIntHdl, DESC_STRING, &tagP, keyString, &size );
        if( retVal == 0 )
        {
            tagLenP = ( tagP + 1 );
            name  = (char*) tagP;
            name += 4;
            arrLen  = *tagLenP - ( 1 + OSS_StrLen( descIntHdl->osHdl, name ));

            if( *lenP < arrLen )
            {
                OSS_MemFree( descIntHdl->osHdl, keyString, gotSize );
                return( ERR_DESC_BUF_TOOSMALL );
            }/*if*/

            srcBuf = (u_int8*)(name + OSS_StrLen( descIntHdl->osHdl, name ) + 1);
            *lenP = arrLen;
            OSS_MemCopy( descIntHdl->osHdl, arrLen, (char*) srcBuf, (char*) bufP );
			DBGWRT_2((DBH," '%s' (found)\n",bufP));
        }
        else
        {
            /* default array */
            OSS_MemCopy( descIntHdl->osHdl, *lenP, (char*) defVal, (char*) bufP );
			DBGWRT_2((DBH," '%s' (default)\n",bufP));
        }/*if*/
        OSS_MemFree( descIntHdl->osHdl, keyString, gotSize );
    }

    return( retVal );
}/*DESC_GetString*/


/**************************** DESC_DbgLevelSet ******************************
 *
 *  Description:  Sets the debug level of this module.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  descHandle    handle for descriptor access
 *                dbgLevel   	new dbg level
 *
 *  Output.....:  ---
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
void DESC_DbgLevelSet
(
    DESC_HANDLE *descHandle,
    u_int32 dbgLevel
)
{
    DESC_INT_HDL  *descIntHdl = (DESC_INT_HDL*) descHandle;
    descIntHdl->dbgLev = dbgLevel;
}/*DESC_DbgLevelSet*/

/**************************** DESC_DbgLevelGet ******************************
 *
 *  Description:  Gets the debug level of this module.
 *
 *---------------------------------------------------------------------------
 *  Input......:  descHandle    handle for descriptor access
 *
 *  Output.....:  dbgLevelP 	current dbg level
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
void DESC_DbgLevelGet
(
    DESC_HANDLE *descHandle,
    u_int32 *dbgLevelP
)
{
    DESC_INT_HDL  *descIntHdl = (DESC_INT_HDL*) descHandle;
    *dbgLevelP = descIntHdl->dbgLev;
}/*DESC_DbgLevelGet*/

/****************************** MakeKeyString *******************************
 *
 *  Description:  Create the key string.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  descIntHdl      internal descriptor handle
 *                keyFmt          format string
 *                argptr          pointer to the variable argument list
 *
 *  Output.....:  keyStringP      pointer to key string
 *                allocatedSizeP  size of allocated memory
 *                return          0 | error code
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int32 MakeKeyString		/* nodoc */
(
    DESC_INT_HDL  *descIntHdl,
    char        *keyFmt,
    char        **keyStringP,
    u_int32     *allocatedSizeP,
    va_list     argptr
)
{
    int32   error = 0;
    char    *dest;

    *allocatedSizeP = 0;

    /* build keyString */
    *keyStringP = (char*)OSS_MemGet( descIntHdl->osHdl, DESC_MAX_KEYLEN, allocatedSizeP );
    if( *keyStringP != NULL )
    {

        dest = *keyStringP;
        OSS_Vsprintf( descIntHdl->osHdl, dest, keyFmt, argptr );
    }
    else
    {
        DBGWRT_ERR((DBH," *** MakeKeyString: can't alloc mem\n"));
        error = ERR_OSS_MEM_ALLOC;
    }/*if*/

    return( error );
}/*MakeKeyString*/

/********************************** GetTag **********************************
 *
 *  Description: Search the keyString and gets the tag.
 *
 *               NOTE: keyString will destroyed.
 *
 *---------------------------------------------------------------------------
 *  Input......:  descIntHdl  internal descriptor handle
 *                tagKind     kind of tag
 *                keyString   search string
 *				  descSizeP   pointer where size will be stored
 *
 *  Output.....:  tagPP       found tag
 *                *descSizeP  0 | desc size
 *                return      0 | error code
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int32 GetTag				/* nodoc */
(
    DESC_INT_HDL  *descIntHdl,
    u_int16     tagKind,
    u_int16     **tagPP,
    char        *keyString,
    u_int32		*descSizeP
)
{
    u_int16 *tagP;
    u_int16 *lenP;
    char    *name;
    char    *helpP;
    u_int32 notFound;
    int32   dirLevel;
    int32   nbrOfDirsMustMatch;
    int32   nbrOfDirsMatching;
    char    *lastP;
    char    *tokP;

	*descSizeP = 0;
    notFound = 1;
    nbrOfDirsMatching = 0;
    dirLevel = 0;
    tagP = (u_int16*) descIntHdl->descStructP;

    /* get strLen and number of directory entrys of the keystring */
    nbrOfDirsMustMatch = 0;
    helpP  = keyString;
    while( *helpP != 0 )
    {
        if( *helpP == '/' )
            nbrOfDirsMustMatch++;
        helpP++;
    }/*while*/

    DBGWRT_2((DBH,"\n keyString='%s' nbrOfDirsMustMatch %d\n",keyString, nbrOfDirsMustMatch));

    lastP = NULL;
    tokP = OSS_StrTok( descIntHdl->osHdl, keyString, "/", &lastP );

    while( *tagP != DESC_END || dirLevel >= 0 ) /* search until last DESC_END */
    {
		/* check if padding OK */
        if(    *tagP != (u_int16) DESC_DIR
            && *tagP != (u_int16) DESC_STRING
            && *tagP != (u_int16) DESC_U_INT32
            && *tagP != (u_int16) DESC_BINARY
            && *tagP != (u_int16) DESC_END
          )
        {
           return( ERR_DESC_CORRUPTED );
        }/*if*/

		DBGWRT_3((DBH,"  tag=%04x %s dirLevel=%d nbrOfDirsMatching=%d\n",*tagP,
		                 (*tagP == (u_int16) DESC_END) ? "":(char*)(tagP+2),
		                 dirLevel, nbrOfDirsMatching));

        if( *tagP == (u_int16) DESC_DIR )
        {
            if( nbrOfDirsMustMatch > dirLevel )
            {
            	/* look into sub dirs, if necessary only */
                name  = (char*) tagP;
                name += 4;
				DBGWRT_2((DBH,"DIR name %s tok %s\n", name, tokP ));
                /* check the dir entry */
                if( !OSS_StrCmp( descIntHdl->osHdl, name, tokP ))
                {
                    tokP = OSS_StrTok( descIntHdl->osHdl, keyString, "/", &lastP );
                    nbrOfDirsMatching++;
                }/*if*/
            }/*if*/
            dirLevel++;
        }/*if*/

        if( *tagP == (u_int16) DESC_END )
        {
            dirLevel--;
            if( dirLevel < nbrOfDirsMatching )
            	nbrOfDirsMatching = dirLevel;
        }/*if*/

        if( dirLevel >= 0
            && *tagP != (u_int16) DESC_END )
        {
            lenP = ( tagP + 1 );
            /* minimal version - looks not for directory entrys matching */
            if( *tagP == tagKind
                && nbrOfDirsMatching == nbrOfDirsMustMatch
                && dirLevel == nbrOfDirsMustMatch
              )
            {
                name  = (char*) tagP;
                name += 4;
                tokP = lastP; /* no more tokens */
                notFound = OSS_StrCmp( descIntHdl->osHdl, name, tokP );
                if( !notFound )
                {
				   DBGWRT_2((DBH,"ENTRY name %s tok %s nbrOfDirsMatching %d\n", name, tokP, nbrOfDirsMatching ));
                   break;
                }/*if*/
            }/*if*/

            tagP = (u_int16*)((U_INT32_OR_64)tagP + *lenP + 4);
        }
        else
        {
           if( dirLevel >= 0 && *tagP == (u_int16) DESC_END )
               tagP += 2; /* sizeof tag + sizeof length = 4 byte */
        }/*if*/
    }/*while*/

    if( notFound )
    {
    	if( dirLevel == -1 )
    	{
	    	u_int32 size;
			/*
				The end tag of a descriptor looks like:
	
				...
		    	struct {
	    	    	u_int16 __typ, __len;
	    		} __end_m22_1;
				...
	    		{ DESC_END, 0 }
				};
			*/
	    	size = (U_INT32_OR_64)tagP - (U_INT32_OR_64)descIntHdl->descStructP + sizeof(*lenP);
	    	/*printf(" dirLevel %d tagP/desc %08x/%08x  size %d\n", dirLevel, tagP, descIntHdl->descStructP, size );*/

	    	*descSizeP = size;
    	}
    	
        return( ERR_DESC_KEY_NOTFOUND );
    }
    else
    {
        *tagPP = tagP;
        return( 0 );
    }/**/

}/*GetTag*/





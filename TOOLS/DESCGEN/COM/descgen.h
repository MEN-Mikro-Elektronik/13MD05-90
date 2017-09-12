/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: descgen.h
 *
 *       Author:
 *        $Date: 2009/09/23 18:34:22 $
 *    $Revision: 1.9 $
 *
 *  Description: Main include file for descriptor generator
 *
 *     Switches: GLOBAL
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: descgen.h,v $
 * Revision 1.9  2009/09/23 18:34:22  CRuff
 * R: compiler warnings on linux 64bit system
 * M: explicitly declare constants as unsigned long long in macro
 *    _TWISTLONGLONG()
 *
 * Revision 1.8  2009/09/23 13:02:41  CRuff
 * R: not 64bit compatible
 * M: added type conversions to make 64bit compatible on linux system
 *
 * Revision 1.7  2000/07/04 09:21:21  Schmidt
 * OutRegFile() prototype added
 *
 * Revision 1.6  1999/12/02 12:09:28  Schmidt
 * enum OUTPUT_MODE: binary replaced with bin_big and bin_lit
 *
 * Revision 1.5  1999/04/28 16:27:09  kp
 * prepared for OS9K
 *
 * Revision 1.4  1999/03/09 10:05:16  see
 * TWISTWORD/LONG macros were wrong for LITTLE_ENDIAN
 *
 * Revision 1.3  1998/07/17 09:26:10  kp
 * changed G_version to array due to problem with HP-UX gcc sscanf
 *
 * Revision 1.2  1998/03/27 17:11:11  see
 * various defs, globals and prototypes added
 *
 * Revision 1.1  1998/01/16 14:49:13  popp
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#ifndef _DESCGEN_H
#define _DESCGEN_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define MAX_LINE_LENGTH		2048 		/* max length of input lines */
#define MAX_FNAME_LENGTH	256 		/* max length of abs file name */
#define MAX_TNAME_LENGTH	128	 		/* max length of tag name */

#define VERBOSE(x)			if(G_verbose) printf x
#define DEBUG(x)			if(G_debug) printf x

/* alignment */
#define LONGALIGN(p) ((p)%4 ? (p+(4-(p)%4)) : (p))

/* byte-ordering */
#define _TWISTWORD(w) ((((w)&0xff)<<8) + (((w)&0xff00)>>8))
#define _TWISTLONG(l) ((((l)&0xff)<<24) + (((l)&0xff00)<<8) + (((l)&0xff0000)>>8) + (((l)&0xff000000)>>24))
#define _TWISTLONGLONG(ll) ( (((ll)&0xff)<<56) + (((ll)&0xff00)<<40) + (((ll)&0xff0000)<<24) + (((ll)&0xff000000)<<8) + (((ll)&0xff00000000ULL)>>8) + (((ll)&0xff0000000000ULL)>>24) + (((ll)&0xff000000000000ULL)>>40) + (((ll)&0xff00000000000000ULL)>>56) )

#define TWISTLONGLONG(v) TwistLongLong(v)
#define TWISTLONG(v) TwistLong(v)
#define TWISTWORD(v) TwistWord(v)

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/*--- output modes ---*/
typedef enum {
	csource, winnt, os9, os9000, bin_big, bin_lit
} OUTPUT_MODE;

/*--- descriptor tag structure ---*/
typedef struct _DESCR_TAG {
	struct _DESCR_TAG	*next;			/* ptr to next tag of same level */
	struct _DESCR_TAG	*parent; 		/* ptr to parent directory */
	struct _DESCR_TAG	*children;		/* ptr to first entry in case
										   of directory tag */
	char				*name;			/* name of tag */
	u_int16				type;			/* type of tag */
	union {								/* union to hold tag value */
		char 			*string;		
		u_int32			uInt32;
		struct {
			u_int8		*arr;		
			int32		entries;
		} uInt8;
	} val;

	u_int16				align1;			/* aligment bytes before value */
	u_int16				align2;			/* aligment bytes after value */
	u_int16				len;			/* length of tag */
} DESCR_TAG;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
#ifndef GLOBAL
# define GLOBAL extern
#endif

GLOBAL int 			G_verbose;			/* be verbose */
GLOBAL int 			G_overwrite;		/* overwrite files */
GLOBAL int 			G_debug;			/* debug output on */
GLOBAL DESCR_TAG	G_objRoot;			/* root of all descriptor objects */
GLOBAL OUTPUT_MODE 	G_outputMode;		/* output mode */
GLOBAL char		 	*G_outputDir;		/* output directory */
GLOBAL char			G_version[];		/* program version */
GLOBAL int			G_targetBigEnd;  	/* byteordering of target */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
/*--- descgen.c ---*/
u_int16 TagTypeVal( char *name );
char *TagTypeName( u_int16 val );
char *StrSave(char *str);
int32 FileExist(char *fname);
char *StrToLower(char *dest, char *source);

/*--- parse.c ---*/
void ParseInit( char *fname, DESCR_TAG *rootObj );
int32 ParseLine( char *lineBuf, int32 lineCount );

/*--- csource.c ---*/
int32 OutCSource( char *fname, DESCR_TAG *topTag );

/*--- binary.c ---*/
int32 OutBinary(DESCR_TAG *topTag );
u_int32  BuildBinaryData(void *buf, DESCR_TAG *topTag, int32 level, int32 dowrite);
u_int16 TwistWord( u_int16 val );
u_int32 TwistLong( u_int32 val );
u_int64 TwistLongLong( u_int64 val );


/*--- os9.c ---*/
int32 OutOS9(char *drvname, char *fmgrname, U_INT32_OR_64 portaddr, DESCR_TAG *topTag);

/*--- os9k.c ---*/
int32 OutOS9000(
	char *drvname,
	char *fmgrname,
	char *procname,
	u_int32 portaddr,
	DESCR_TAG *topTag);

/*--- regfile.c ---*/
int32 OutRegFile( char *fname, DESCR_TAG *topTag );


#ifdef __cplusplus
	}
#endif

#endif	/* _DESCGEN_H */





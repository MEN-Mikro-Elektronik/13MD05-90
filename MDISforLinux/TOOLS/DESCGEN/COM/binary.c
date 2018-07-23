/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: binary.c
 *      Project: descgen
 *
 *       Author: see
 *        $Date: 2012/05/14 21:26:28 $
 *    $Revision: 1.9 $
 *
 *  Description: Output descriptor as binary
 *
 *
 *     Required: -
 *     Switches: LITTLE_ENDIAN  Host system has Intel byte-ordering
 *               BIG_ENDIAN  	Host system has Motorola byte-ordering
 *
 *				 LINUX			If defined, byteordering is determined by
 *								endian.h
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: binary.c,v $
 * Revision 1.9  2012/05/14 21:26:28  ts
 * R: compiler warning about unused variable res
 * M: removed variable
 *
 * Revision 1.8  2009/09/23 18:33:01  CRuff
 * R: compiler warning on linux systems
 * M: fix compiler warnings
 *
 * Revision 1.7  2009/09/23 13:02:39  CRuff
 * R: not 64bit compatible
 * M: added type conversions to make 64bit compatible on linux system
 *
 * Revision 1.6  2001/01/19 14:37:11  kp
 * added LINUX switch
 *
 * Revision 1.5  1999/04/28 16:27:04  kp
 * support Big/Little Endian targets
 *
 * Revision 1.4  1999/03/09 10:05:11  see
 * byte swapping-problems fixed (BINARY, STRING)
 *
 * Revision 1.3  1998/07/17 08:47:42  kp
 * cosmetics
 *
 * Revision 1.2  1998/04/17 15:37:42  see
 * BuildBinaryData: bug fixed: subkey name was not written
 * BuildBinaryData: bug fixed: final end dir was not written
 *
 * Revision 1.1  1998/03/27 17:11:10  see
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuremberg, Germany
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



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <MEN/men_typs.h>
#include <MEN/desctyps.h>
#include "descgen.h"

#ifdef LINUX
# include <endian.h>
# undef BIG_ENDIAN
# undef LITTLE_ENDIAN

# if __BYTE_ORDER == __BIG_ENDIAN
#  define BIG_ENDIAN
# endif

# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define LITTLE_ENDIAN
# endif
#endif /* LINUX */

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static const u_int16 dir_end_type = DESC_END;
static const u_int16 dir_end_size = 0x0000;
static const char pad_char = 0x00;			/* fill byte */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void mwrite(void **dstP,void *src,u_int32 size,u_int32 dowrite);

/********************************* OutBinary *******************************
 *
 *  Description: Create binary file for each toplevel tag
 *
 *---------------------------------------------------------------------------
 *  Input......: fname			source file name
 *				 topTag			root directory of objects
 *  Output.....: error code
 *  Globals....: -
 ****************************************************************************/
int32 OutBinary(DESCR_TAG *topTag)
{
	FILE *fp;
	DESCR_TAG *tag;
	char binfile[MAX_FNAME_LENGTH];
	char tagname[MAX_TNAME_LENGTH];
	void *buf = NULL;
	u_int32 bufsize;

	/*-----------------------------+
    |  for all top level tags      |
    +-----------------------------*/
	for (tag=topTag->children; tag; tag=tag->next) {
		/*printf("--- %s: typ=%04x, len=%04x\n",tag->name,tag->type,tag->len);*/

		/*-------------------+
		|  create file name  |
		+-------------------*/
		StrToLower(tagname,tag->name);
		sprintf(binfile,"%s%c%s.bin",G_outputDir,MEN_PATHSEP,tagname);

		/*-------------------+
		|  check if output   |
		|  file already exist|
		+-------------------*/
		if (FileExist(binfile))
			if (!G_overwrite) {
				fprintf(stderr,"*** output file %s already exists\n",
						binfile);
				continue;
			}
			else
				printf("overwriting %s\n",binfile);
		else
			printf("creating %s\n",binfile);

		/*-------------------+
		|  open output file  |
		+-------------------*/
		if ((fp = fopen(binfile, "wb")) == NULL) {
			fprintf(stderr, "*** error %d opening output file %s\n",errno,binfile);
			return errno;
		}

		/*-------------------+
		|  output binary data|
		+-------------------*/
		/* calculate required size */
		bufsize = BuildBinaryData(buf, tag, 1, 0);

		/* alloc mem */
		if ((buf = malloc(bufsize)) == NULL) {
			fprintf(stderr, "*** can't alloc %d bytes\n",bufsize);
			return(errno);
		}

		/* build data in mem */
		BuildBinaryData(buf, tag, 1, 1);

		/* write to file */
		fwrite(buf,1,bufsize,fp);
		VERBOSE(("(%d bytes written)\n\n",bufsize));

		free(buf);
		fclose(fp);
	}
	return 0;
}

/******************************* BuildBinaryData ****************************
 *
 *  Description: Create binary data and/or return data size
 *
 *---------------------------------------------------------------------------
 *  Input......: buf		destination buffer
 *				 topTag		tag to generate
 *				 level		recursion level
 *               dowrite	1=write or 0 for counting only
 *  Output.....: return		nr of written bytes
 *  Globals....: -
 ****************************************************************************/
u_int32 BuildBinaryData(void *buf, DESCR_TAG *topTag, int32 level, int32 dowrite)
{
	DESCR_TAG *tag;
	u_int32 i, len;
	U_INT32_OR_64 start;
	char pads;

	start = (U_INT32_OR_64)buf;

	/*--------------------------------+
    |  output binary data             |
    +--------------------------------*/
	for (tag=topTag->children; tag; tag=tag->next) {
		/*printf("[%d] %s: typ=%04x, len=%04x\n",
		  level,tag->name,tag->type,tag->len);*/

		switch(tag->type) {
		case DESC_DIR:
			/* start dir: type + len */
			mwrite(&buf,(void*)&tag->type,2,dowrite);
			mwrite(&buf,(void*)&tag->len,2,dowrite);
			mwrite(&buf,(void*)tag->name,strlen(tag->name)+1,dowrite);

			/* pads */
			for (i=0; i<tag->align1; i++)
				mwrite(&buf,(void*)&pad_char,1,dowrite);

			/* process subdir .. */
			buf = (char*)buf + BuildBinaryData(buf,tag,level+1,dowrite);

			/* end dir: type + len */
			mwrite(&buf,(void*)&dir_end_type,2,dowrite);
			mwrite(&buf,(void*)&dir_end_size,2,dowrite);
			break;
		case DESC_U_INT32:
		case DESC_BINARY:
		case DESC_STRING:
			/* type, len, name */
			mwrite(&buf,(void*)&tag->type,2,dowrite);
			mwrite(&buf,(void*)&tag->len,2,dowrite);
			mwrite(&buf,(void*)tag->name,strlen(tag->name)+1,dowrite);

			switch(tag->type) {
			case DESC_U_INT32:
				/* pads */
				for (i=0; i<tag->align1; i++)
					mwrite(&buf,(void*)&pad_char,1,dowrite);

				/* value */
				mwrite(&buf,(void*)&tag->val.uInt32,4,dowrite);
				break;

			case DESC_BINARY:
				/* nr of pads */
				pads = tag->align2 & 0xff;
				mwrite(&buf,(void*)&pads,1,dowrite);

				/* byte array */
				for (i=0; i<tag->val.uInt8.entries; i++)
					mwrite(&buf,(void*)&tag->val.uInt8.arr[i],1,dowrite);

				/* pads */
				for (i=0; i<tag->align2; i++)
					mwrite(&buf,(void*)&pad_char,1,dowrite);

				break;

			case DESC_STRING:
				len = strlen(tag->val.string)+1;

				/* char array */
				for (i=0; i<len; i++)
					mwrite(&buf,(void*)&tag->val.string[i],1,dowrite);

				/* pads */
				for (i=0; i<tag->align2; i++)
					mwrite(&buf,(void*)&pad_char,1,dowrite);

				break;
			}

			break;
		}
	}

	/*-------------------------------+
    |  final end dir: type + len     |
    +-------------------------------*/
	if (level == 1) {
		mwrite(&buf,(void*)&dir_end_type,2,dowrite);
		mwrite(&buf,(void*)&dir_end_size,2,dowrite);
	}

	return( (u_int32)((U_INT32_OR_64)buf - start) );	/* return written bytes */
}


/********************************* mwrite ***********************************
 *
 *  Description: Write byte/short/long value to buffer
 *
 *---------------------------------------------------------------------------
 *  Input......: dstP		address of destination ptr
 *               src		ptr to source
 *               size   	value size (1,2 or 4) [bytes]
 *               dowrite	1=write or 0 for updating ptr only
 *  Output.....: dstP		incremented buffer ptr
 *  Globals....: -
 ****************************************************************************/

static void mwrite(
    void **dstP,
	void *src,
    u_int32 size,
	u_int32 dowrite
)
{
	register void *dst = *dstP;

	if (dowrite) {
		switch(size) {
			case 1: *(u_int8* )dst = *(u_int8* )src;	 		break;
			case 2: *(u_int16*)dst = TWISTWORD(*(u_int16*)src);	 break;
			case 4: *(u_int32*)dst = TWISTLONG(*(u_int32*)src);	 break;
			default: strncpy((char*)dst,(char*)src,size);
		}
	}

	*dstP = (void*)((U_INT32_OR_64)*dstP + (U_INT32_OR_64)size);	/* update buf ptr */
}


u_int16 TwistWord( u_int16 val )
{
#ifdef BIG_ENDIAN
	if( !G_targetBigEnd )
#else
	if( G_targetBigEnd )
#endif
		return _TWISTWORD(val);
	else
		return val;
}

u_int32 TwistLong( u_int32 val )
{
#ifdef BIG_ENDIAN
	if( !G_targetBigEnd )
#else
	if( G_targetBigEnd )
#endif
		return _TWISTLONG(val);
	else
		return val;
}

u_int64 TwistLongLong( u_int64 val )
{
#ifdef BIG_ENDIAN
	if( !G_targetBigEnd )
#else
	if( G_targetBigEnd )
#endif
		return _TWISTLONGLONG(val);
	else
		return val;
}

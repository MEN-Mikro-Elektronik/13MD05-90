/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: os9k.c
 *      Project: descgen
 *
 *       Author: kp
 *        $Date: 2001/01/19 14:37:19 $
 *    $Revision: 1.3 $
 *
 *  Description: Output descriptor as OS-9000 descriptor module
 *
 *
 *     Required: -
 *     Switches: LITTLE_ENDIAN  Host system has Intel byte-ordering
 *               BIG_ENDIAN  	Host system has Motorola byte-ordering
 *
 *
 *   Note: prepared for OS-9000 on I386, but CRC calculation doesn't work
 *---------------------------[ Public Functions ]----------------------------
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: os9k.c,v $
 * Revision 1.3  2001/01/19 14:37:19  kp
 * cosmetics
 *
 * Revision 1.2  1999/06/09 16:02:54  kp
 * changed totally for MDISFM/BBISFM
 *
 * Revision 1.1  1999/04/28 16:27:13  kp
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuremberg, Germany
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
#include <stddef.h>
#include <errno.h>
#include <time.h>

#include <MEN/men_typs.h>
#include <MEN/desctyps.h>
#include "descgen.h"
#include "os9k.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--- size of uninit data of logical unit storage ---*/
#define LU_STAT_SIZE 1024

typedef enum {
	_pppc_, _i386_
} PROC_TYPE;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void BuildMhCom(
	PROC_TYPE proctype,
    mh_com *head,
    u_int32 msize,
    u_int32 mname_offs,
	u_int32 mdisfmdesc_offs,
	u_int32 idata_offs
);
static void BuildDdCom(
    dd_com *p,
	u_int32 mport,
	u_int32 mpdev_offs,
	u_int32 mfmgr_offs
);

static u_int16 CreateEdition();
static u_int16 CalcHeaderParity(u_int16 *p);
static u_int32 CalcCRC(void *p, u_int32 size);
static void CRC(u_int16 *p, u_int32 size, u_int32 *pa);

/********************************* OutOS9000 *********************************
 *
 *  Description: Open new file for each tag and create OS-9000
 *		         module descriptor
 *
 *---------------------------------------------------------------------------
 *  Input......: drvname		driver name
 *               fmgrname		file manager name
 *				 portaddr		device port address
 *				 procname		target processor type (pppc or 386)
 *				 topTag			root directory of objects
 *  Output.....: error code
 *  Globals....: -
 ****************************************************************************/

int32 OutOS9000(
	char *drvname,
	char *fmgrname,
	char *procname,
	u_int32 portaddr,
	DESCR_TAG *topTag)
{
	FILE *fp;
	DESCR_TAG *tag, *tag2;
	char os9file[MAX_FNAME_LENGTH];		/* file names */
	char tagname[MAX_TNAME_LENGTH];
	char drvnamebuf[MAX_FNAME_LENGTH];
	u_int32 msize, mdisDescSize;			/* size's */
	u_int32 mname_offs, mfmgr_offs;		/* offsets */
	u_int32 mpdev_offs, mdisDesc_offs;
	u_int32 idata_offs, lustat_offs;
	u_int32 mdisfmdesc_offs;
	u_int32 crc_offs, desc_type;
	char *mname_str, *mfmgr_str, *mpdev_str;	/* names */
	char *processor, *hw_type;
	char *buf=NULL;
	u_int32 mport=0, crc;
	PROC_TYPE proctype;

	processor = (procname ? procname : "pppc" ); 	/* init processor name */

	if( strcmp( processor, "pppc") == 0 )
		proctype = _pppc_;
	else if( strcmp( processor, "386") == 0 )
		proctype = _i386_;
	else {
		fprintf(stderr,"*** Unsupported processor type %s\n", processor );
		return 1;
	}

	if( proctype == _pppc_ )
		G_targetBigEnd = TRUE;
	else
		G_targetBigEnd = FALSE;

	/*-----------------------------+
    |  for all top level tags      |
    +-----------------------------*/
	for (tag=topTag->children; tag; tag=tag->next) {
		/*printf("--- %s: typ=%04x, len=%04x\n",tag->name,tag->type,tag->len);*/
		StrToLower(tagname,tag->name);

		/*----------------------------------------+
		|  Look for DESC_TYPE (Device or board?)  |
		|  Look for HW_TYPE  					  |
		+----------------------------------------*/
		desc_type = 0;
		hw_type = NULL;

		for (tag2=tag->children; tag2; tag2=tag2->next) {
			if( strcmp(tag2->name, "DESC_TYPE") == 0 )
				desc_type = tag2->val.uInt32;
			if( strcmp(tag2->name, "HW_TYPE") == 0 )
				hw_type = tag2->val.string;

		}

		if( desc_type != 1 && desc_type != 2 ){
			fprintf(stderr,"*** Missing or bad DESC_TYPE keyword\n" );
			return 1;
		}
		if( hw_type == NULL ){
			fprintf(stderr,"*** Missing or bad HW_TYPE keyword\n" );
			return 1;
		}


		/*-----------------------+
		|  Build ll driver name  |
		+-----------------------*/
		if( drvname )
			mpdev_str = drvname;
		else {
			mpdev_str = drvnamebuf;

			if( desc_type == 1 ){

				/*
				 * Since some old descriptors for M-modules <M100 have leading
				 * zeroes in hw name (e.g. M008), but ll drivers have the name
				 * ll_m8, remove the leading zeroes in that case.
				 */
				int i,j;

				strcpy( drvnamebuf, "ll_" );
				i=3; j=0;

				drvnamebuf[i++] = hw_type[j++];

				if( hw_type[0] == 'm' || hw_type[0] == 'M'){
					while( hw_type[j] == '0' ) /* skip leading zeroes */
						j++;
				}
				/*--- copy rest of string ---*/
				strcpy( &drvnamebuf[i], &hw_type[j] );
			}
			else {
				sprintf( drvnamebuf, "bb_%s", hw_type );
			}
		}
		mfmgr_str = (desc_type==1 ? "mdisfm" : "bbisfm");/* init fmgr name */
		if( fmgrname ) mfmgr_str = fmgrname;

		/*---------------------------------------------------------+
		|  prepare parameters                                      |
		+---------------------------------------------------------*/
		/* calculate required MEN descriptor data size */
		mdisDescSize = BuildBinaryData(buf, tag, 1, 0);

		/* name strings */
		mname_str 	= tagname;		/* module name */

		/* offsets */
		mdisfmdesc_offs= sizeof(mh_com);
		mpdev_offs	= mdisfmdesc_offs + sizeof(mdisfm_desc);
		mfmgr_offs	= mpdev_offs + strlen(mpdev_str)+1;
		mname_offs	= mfmgr_offs + strlen(mfmgr_str)+1;
		idata_offs	= mname_offs + strlen(mname_str)+1;

		idata_offs	= LONGALIGN(idata_offs);
		lustat_offs	= idata_offs+8;

		mdisDesc_offs = lustat_offs + 8 /*why???*/+ sizeof(mdisfm_lu_stat);
		mdisDesc_offs = LONGALIGN(mdisDesc_offs);

		crc_offs	= mdisDesc_offs + mdisDescSize;
		crc_offs	= LONGALIGN(crc_offs);

		/* total size */
		msize 		= crc_offs + 4;

		/* create port addr */
		if (portaddr == 0) {
			if (strlen(mname_str) > 4)
				strncpy((char*)&mport,mname_str+strlen(mname_str)-4,4);
			else
				strncpy((char*)&mport,mname_str,strlen(mname_str));

			mport = TWISTLONG(mport);	/* (is twisted twice) */
		}
		else
			mport = portaddr;

		VERBOSE(("Creating descriptor module '%s'\n",mname_str));
		VERBOSE(("Port address    : 0x%08x\n",TWISTLONG(mport)));
		VERBOSE(("Filemanager name: '%s'\n",mfmgr_str));
		VERBOSE(("Driver name     : '%s'\n",mpdev_str));
		VERBOSE(("Target Processor: '%s'\n",processor));

		/*---------------------------------------------------------+
		|  create os9 module (in-memory)                           |
		+---------------------------------------------------------*/
		/* alloc mem for descriptor */
		if ((buf = (char*)calloc(1,msize)) == NULL) {
			fprintf(stderr, "*** can't alloc %d bytes\n",msize);
			return(errno);
		}
		/* build the descriptor in memory */
		BuildMhCom( proctype, (mh_com *)buf,
					msize, mname_offs, mdisfmdesc_offs,
					idata_offs);


		BuildDdCom( (dd_com *)(buf + mdisfmdesc_offs), mport, mpdev_offs,
					mfmgr_offs );

		/*--- copy strings ---*/
		strcpy(buf + mpdev_offs, mpdev_str);
		strcpy(buf + mfmgr_offs, mfmgr_str);
		strcpy(buf + mname_offs, mname_str);

		/*--- fill init data header ---*/
		{
			u_int32 *p = (u_int32 *)(buf + idata_offs);

			*p++ = TWISTLONG(0x0);
			*p++ = TWISTLONG(sizeof(mdisfm_lu_stat));
		}

		/*---- MDISFM lu stat ---*/
		{
			mdisfm_lu_stat *p = (mdisfm_lu_stat *)(buf + lustat_offs);

			p->mdisDescOffs = TWISTLONG(mdisDesc_offs);
		}

		/*--- fill in our descriptor data ---*/
		BuildBinaryData(buf + mdisDesc_offs, tag, 1, 1);

		/* create CRC */
		crc = CalcCRC(buf,crc_offs);
		*(u_int32*)((char*)buf + crc_offs) = TWISTLONG(crc);

		DEBUG(("crc=0x%06x\n",crc));

		/*---------------------------------------------------------+
		|  write os9 module (output file)                         |
		+---------------------------------------------------------*/
		sprintf(os9file,"%s%c%s",G_outputDir,MEN_PATHSEP,tagname);

		/* check if output file already exist */
		if (FileExist(os9file))
			if (!G_overwrite) {
				fprintf(stderr,"*** output file %s already exists\n",os9file);
				return 0;
			}
			else
				printf("overwriting %s\n",os9file);
		else
			printf("creating %s\n",os9file);

		/* open output file */
		if ((fp = fopen(os9file, "wb")) == NULL) {
			fprintf(stderr, "*** error %d opening output file %s\n",errno,os9file);
			return errno;
		}

		/* write all data */
		if (fwrite(buf,1,msize,fp) != msize) {
			fprintf(stderr, "*** error %d writing output file %s\n",errno,os9file);
			return errno;
		}

		VERBOSE(("(%d bytes written)\n\n",msize));
		fclose(fp);

		/* free mem */
		free(buf);
	}

	return(0);
}

/********************************* BuildMhCom ******************************
 *
 *  Description: Fill common module header
 *
 *---------------------------------------------------------------------------
 *  Input......: head	ptr to descriptor header
 *               xxxx   header parameters
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/

static void BuildMhCom(
	PROC_TYPE proctype,
    mh_com *head,
    u_int32 msize,
    u_int32 mname_offs,
	u_int32 mdisfmdesc_offs,
	u_int32 idata_offs
)
{
	u_int16 medit = CreateEdition();

	/*-------------------------+
    | common module header     |
    +-------------------------*/
	switch( proctype ){
	case _pppc_:
		head->m_sync = TWISTWORD( MODSYNC_PPC );
		break;
	case _i386_:
		head->m_sync = TWISTWORD( MODSYNC_X86 );
		break;
	}

	head->m_sysrev 	= TWISTWORD( 2 );				/* (always 2) */
	head->m_size 	= TWISTLONG( msize );
	head->m_owner 	= TWISTLONG( 0 );				/* superuser */
	head->m_name 	= TWISTLONG( mname_offs );
	head->m_access 	= TWISTWORD( 0x555 );
	head->m_tylan 	= TWISTWORD( 0x0f01 );
	head->m_attrev 	= TWISTWORD( 0x8000 );
	head->m_edit 	= TWISTWORD( medit );

	head->m_exec	= TWISTLONG( mdisfmdesc_offs );
	head->m_data	= TWISTLONG( sizeof(mdisfm_lu_stat) + LU_STAT_SIZE);
	head->m_idata	= TWISTLONG( idata_offs );
	head->m_idref	= 0;

	head->m_parity 	= CalcHeaderParity((u_int16*)head);		/* create parity */

	/* other fields are zero */

	DEBUG(("edition=0x%04x\n",medit));
}

/********************************* BuildDdCom ******************************
 *
 *  Description: Fill device descriptor header
 *
 *---------------------------------------------------------------------------
 *  Input......: p		ptr to dd_com header
 *               xxxx   header parameters
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void BuildDdCom(
    dd_com *p,
	u_int32 mport,
	u_int32 mpdev_offs,
	u_int32 mfmgr_offs
)
{
	p->dd_port 		= TWISTLONG(mport);
	p->dd_pd_size	= TWISTWORD(sizeof(mdisfm_path_desc));
	p->dd_mode		= TWISTWORD(0x2003);
	p->dd_fmgr		= TWISTLONG(mfmgr_offs);
	p->dd_drvr		= TWISTLONG(mpdev_offs);
	p->dd_class		= TWISTWORD(0x1);
}


/********************************* CreateEdition ****************************
 *
 *  Description: Create edition from global DESCGEN version string
 *
 *---------------------------------------------------------------------------
 *  Input......: -
 *  Output.....: return    edition
 *  Globals....: G_version
 ****************************************************************************/

static u_int16 CreateEdition()
{
	int major=1, minor=0;

	sscanf(G_version,"V%d.%d",&major,&minor);

	return((major<<8) | (minor & 0xff));
}

/***************************** CalcHeaderParity ******************************
 *  Description: Calculate module header parity
 *
 *---------------------------------------------------------------------------
 *  Input......: p		 ptr to module header
 *  Output.....: return  parity
 *  Globals....: -
 ****************************************************************************/

static u_int16 CalcHeaderParity(u_int16 *p)
{
	u_int16 parity=0xffff;
	int16 wordcount;

	for (wordcount = (sizeof(mh_com)-2)/2; --wordcount >= 0;)
		parity ^= *p++;

	DEBUG(("parity=0x%04x\n", parity));
	return parity;
}


/***************************** CalcCRC **************************************
 *
 *  Description: Calculate module CRC
 *
 *---------------------------------------------------------------------------
 *  Input......: buf	 ptr to CRC location
 *               size    module size
 *  Output.....: return  crc
 *  Globals....: -
 ****************************************************************************/

static u_int32 CalcCRC( buf, size )
void *buf;
u_int32 size;
{
	u_int32 a = 0xffffffff;

	CRC( (u_int16 *)buf, size, &a );
	CRC( 0L, 0, &a );
	a = ~a & 0xffffff;

	return a;
}

/******************************** CRC ***************************************
 *
 *  Description: CRC calculation low level algorithm
 *				 (don't try to understand)
 *
 *---------------------------------------------------------------------------
 *  Input......: p		 ptr to module start
 *               size    module size
 *               pa		 ptr to accum
 *  Output.....: return  parity
 *  Globals....: -
 ****************************************************************************/

static void CRC(
    u_int16 *p,
    u_int32 size,
    u_int32 *pa
)
{
	u_int32 a = *pa & 0x00ffffff, b, d;
	u_int16 c;

	if( !size ){
		d = b = (a & 0xffff0000)>>16;
		a <<= 8;
		b <<= 1;
		a ^= b;
		d ^= b;
		b <<= 5;
		a ^= b;
		c = (u_int16)d; c <<= 2; d ^= c;
		c = (u_int16)d; d <<= 4; c ^= d;
		if( c & 0x80 )
			a ^= 0x00800021;
	}
	else {
		size /= 2;

		while( size-- ){
			b = TWISTWORD(*p) << 8;
			p++;
			b = (b ^ a) & 0x00ffff00;
			a <<= 16;
			b >>= 2;
			a = a ^ b;
			b >>= 5;
			a = a ^ b;

			/* determine number of bits set in b */
			c = (u_int16)b;
			b >>= 1; c ^= b; b = c;
			b <<= 2; c ^= b; b = c;
			b <<= 4; c ^= b; b = c;
			b <<= 8; c ^= b;

			if( c & 0x8000 )
				a ^= 0x00800021;
		}
	}

	*pa = a & 0x00ffffff;
}



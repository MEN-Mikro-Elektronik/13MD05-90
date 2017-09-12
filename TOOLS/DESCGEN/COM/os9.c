/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: os9.c
 *      Project: descgen
 *
 *       Author: see
 *        $Date: 2009/09/23 13:02:43 $
 *    $Revision: 1.9 $
 *
 *  Description: Output descriptor as OS-9 descriptor module
 *
 *
 *     Required: -
 *     Switches: LITTLE_ENDIAN  Host system has Intel byte-ordering
 *               BIG_ENDIAN  	Host system has Motorola byte-ordering
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: os9.c,v $
 * Revision 1.9  2009/09/23 13:02:43  CRuff
 * R: not 64bit compatible
 * M: added type conversions to make 64bit compatible on linux system
 *
 * Revision 1.8  2001/01/19 14:37:17  kp
 * cosmetics
 *
 * Revision 1.7  1999/06/09 16:02:52  kp
 * cosmetics
 *
 * Revision 1.6  1999/04/28 16:27:11  kp
 * Bug fix: open descriptor with "wb"
 *
 * Revision 1.5  1999/03/09 10:05:19  see
 * byte swapping-problems fixed (mport, mparity, CRC)
 * CRC: unused vars removed
 *
 * Revision 1.4  1998/07/17 09:26:06  kp
 * - changed old OS-9 types (u_char) to MEN std types
 * - fixed bug: mopt_offs computation wrong
 * - fixed problem with HPUX-gcc structure alignment
 *
 * Revision 1.3  1998/07/17 08:47:53  kp
 * cosmetics
 *
 * Revision 1.2  1998/04/17 15:37:49  see
 * bug fixed: head->_mmode: SHARE flag must be cleared
 *
 * Revision 1.1  1998/03/27 17:11:08  see
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

static const char *RCSid="$Id: os9.c,v 1.9 2009/09/23 13:02:43 CRuff Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <MEN/men_typs.h>
#include <MEN/desctyps.h>
#include "descgen.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define VHPCNT  (sizeof(struct modhcom)-2)  /* sizeof common header */
#define MODSYNC 0x4afc      /* module header sync code */

/* permission */
#define MP_OWNER_READ	0x0001
#define MP_OWNER_WRITE	0x0002

/* access mode */
#define S_IREAD			0x01	 	/* owner read */
#define S_IWRITE		0x02		/* owner write */

/* type, language */
#define MT_DEVDESC	15
#define ML_ANY		0

/* module attribute*/
#define MA_REENT	0x80

/* device type */
#define DT_SCF      0

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* std module header */
struct modhcom {
    short       _msync,     /* sync bytes ($4afc) */
				_msysrev;	/* system revision check value */
    long        _msize,     /* module size */
                _mowner,    /* owner id */
				_mname;		/* offset to module name */
	short		_maccess,   /* access permission */
                _mtylan,    /* type/lang */
                _mattrev,   /* rev/attr */
                _medit;     /* edition */
	long		_musage,	/* comment string offset */
				_msymbol;	/* symbol table offset */
	short		_mident;	/* ident code */
	char		_mspare[6];	/* reserved bytes */
	long		_mhdext;	/* module header extension offset */
	short		_mhdextsz;	/* module header extension size */
	short		_mparity;   /* header parity */
};

/* descriptor header */
typedef struct {
    struct modhcom	_mh;     /* common header info */
	char *			_mport;		/* device port address */
	unsigned char	_mvector;	/* trap vector number */
	unsigned char	_mirqlvl;	/* irq interrupt level */
	unsigned char	_mpriority;	/* irq polling priority */
	unsigned char	_mmode;		/* device mode capabilities */
	short			_mfmgr;		/* file manager name offset */
	short			_mpdev;		/* device driver name offset */
	short			_mdevcon;	/* device configuration offset */
	unsigned short	_mdscres[1];/* (reserved) */
	unsigned long	_mdevflags;	/* reserved for future usage */
	unsigned short	_mdscres2[1];	/* reserved */
	unsigned short	_mopt;		/* option table size */
	unsigned char	_mdtype;	/* device type code */
	unsigned char 	_pad[3];
} mod_dev;

/* SCF path options */
struct scf_opt {
	u_int8		pd_dtp;			/* device type */
	u_int8		pd_upc;			/* case (0=both, 1=upper only) */
	u_int8		pd_bso;			/* backsp (0=bse, 1=bse,sp,bse) */
	u_int8		pd_dlo;			/* delete (0=bse over line, 1=crlf) */
	u_int8		pd_eko;			/* echo (0=no echo) */
	u_int8		pd_alf;			/* autolf (0=no auto lf) */
	u_int8		pd_nul;			/* end of line null count */
	u_int8		pd_pau;			/* pause (0=no end of page pause) */
	u_int8		pd_pag;			/* lines per page */
	u_int8		pd_bsp;			/* backspace character */
	u_int8		pd_del;			/* delete line character */
	u_int8		pd_eor;			/* end of record char (read only) */
	u_int8		pd_eof;			/* end of file char */
	u_int8		pd_rpr;			/* reprint line char */
	u_int8		pd_dup;			/* dup last line char */
	u_int8		pd_psc;			/* pause char */
	u_int8		pd_int;			/* kbd intr char (ctl c) */
	u_int8		pd_qut;			/* kbd quit char (ctl q) */
	u_int8		pd_bse;			/* backspace echo character */
	u_int8		pd_ovf;			/* line overflow char (bell) */
	u_int8		pd_par;			/* parity code */
	u_int8		pd_bau;			/* acia baud rate (color computer) */
	short		pd_d2p;			/* dev2 output name offset */
	u_int8		pd_xon;			/* x-on char */
	u_int8		pd_xoff;		/* x-off char */
	u_int8		pd_Tab;			/* Tab character (0=none) */
	u_int8		pd_Tabs;		/* Tab field size */
	u_int32		pd_tbl;			/* Device table addr (copy) */
	u_int16		pd_Col;			/* current column number */
	u_int8		pd_err;			/* most recent I/O error status */
	u_int8		_pad;
};

/* SPF path options */
struct spf_opts {
     struct scf_opt scf_opts;      /* SCF options */
     u_int8 pds_Chan,    /* channel number (base 0) */
            pds_Comm;    /* communications protocol (0 = async) */
     u_int16 pds_ChanID, /* hardware node/channel ID */
             pds_RTime,  /* timeout on read in ticks (0 = indefinite) */
             pds_WTime,  /* timeout on write in ticks (0 = indefinite) */
             pds_RBuf,   /* required input buffer size */
             pds_WBuf,   /* required output buffer size */
             pds_Control, /* device control flags */
             dummy1[23]; /* reserved */
     u_int32 pds_DatCnt;   /* actual transfer count of last read/write */
};

/* OS-9000 compatible type definitions */
typedef struct modhcom mh_com;
typedef mod_dev mh_dev;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void BuildHeader(mod_dev *head, U_INT32_OR_64 mport, u_int32 msize,
						u_int32 mname_offs, u_int32 mfmgr_offs,
						u_int32 mpdev_offs, u_int32 devcon_offs);
static void BuildOpt(struct spf_opts *opt);
static u_int16 CreateEdition();
static u_int16 CalcHeaderParity(u_int16 *p);
static u_int32 CalcCRC(void *p, u_int32 size);
static void CRC(u_int16 *p, u_int32 size, u_int32 *pa);

/********************************* OutOS9 ***********************************
 *
 *  Description: Open new file for each tag and create OS-9 module descriptor
 *			
 *---------------------------------------------------------------------------
 *  Input......: mpdev_str		driver name
 *               mport          dummy port address
 *				 topTag			root directory of objects
 *  Output.....: error code
 *  Globals....: -
 ****************************************************************************/

int32 OutOS9(char *drvname, char *fmgrname, U_INT32_OR_64 portaddr, DESCR_TAG *topTag)
{
	FILE *fp;
	DESCR_TAG *tag;
	char os9file[MAX_FNAME_LENGTH];		/* file names */
	char tagname[MAX_TNAME_LENGTH];
	u_int32 msize, mdevcon;				/* size's */
	u_int32 mname_offs, mfmgr_offs;		/* offsets */
	u_int32 mpdev_offs, mdevcon_offs;
	u_int32 mopt_offs, crc_offs;
	char *mname_str, *mfmgr_str, *mpdev_str;	/* names */
	char *buf=NULL;
	u_int32 crc;
	U_INT32_OR_64 mport=0;

	mpdev_str = (drvname ? drvname : "spMDIS");		/* init driver name */
	mfmgr_str = (fmgrname ? fmgrname : "spf");		/* init fmgr name */

	/*-----------------------------+
    |  for all top level tags      |
    +-----------------------------*/
	for (tag=topTag->children; tag; tag=tag->next) {
		/*printf("--- %s: typ=%04x, len=%04x\n",tag->name,tag->type,tag->len);*/
		StrToLower(tagname,tag->name);

		/*---------------------------------------------------------+
		|  prepare parameters                                      |
		+---------------------------------------------------------*/
		/* calculate required devcon size */
		mdevcon 	= BuildBinaryData(buf, tag, 1, 0);

		/* name strings */
		mname_str 	= tagname;		/* module name */

		/* offsets */
		mopt_offs	= sizeof(mod_dev) - 4;
		mdevcon_offs= LONGALIGN(mopt_offs + sizeof(struct spf_opts));
		mfmgr_offs 	= mdevcon_offs + LONGALIGN(mdevcon);
		mpdev_offs	= mfmgr_offs   + LONGALIGN(strlen(mfmgr_str)+1);
		mname_offs	= mpdev_offs   + LONGALIGN(strlen(mpdev_str)+1);
		crc_offs    = mname_offs   + LONGALIGN(strlen(mname_str)+1);

		/* total size */
		msize 		= crc_offs + 4;

		/* create port addr */
		if (portaddr == 0) {
			if (strlen(mname_str) > 4)
				strncpy((char*)&mport,mname_str+strlen(mname_str)-4,4);
			else
				strncpy((char*)&mport,mname_str,strlen(mname_str));
#ifdef _LIN64
			mport = TWISTLONGLONG(mport);	/* (is twisted twice) */
#else
			mport = TWISTLONG(mport);	/* (is twisted twice) */
#endif
		}
		else
			mport = portaddr;

		VERBOSE(("Creating descriptor module '%s'\n",mname_str));
#ifdef _LIN64
		VERBOSE(("Port address    : 0x%08x\n",TWISTLONGLONG(mport)));
#else
		VERBOSE(("Port address    : 0x%08x\n",TWISTLONG(mport)));
#endif
		VERBOSE(("Filemanager name: '%s'\n",mfmgr_str));
		VERBOSE(("Driver name     : '%s'\n",mpdev_str));

		/*---------------------------------------------------------+
		|  create os9 module (in-memory)                           |
		+---------------------------------------------------------*/
		/* alloc mem for descriptor */
		if ((buf = (char*)calloc(1,msize)) == NULL) {
			fprintf(stderr, "*** can't alloc %d bytes\n",msize);
			return(errno);
		}
		/* build the descriptor in memory */
		BuildHeader((mod_dev*)buf,mport,msize,mname_offs,	/* module head */
					mfmgr_offs,mpdev_offs,mdevcon_offs);
		BuildOpt((struct spf_opts*)(buf + mopt_offs));		/* path opts */
		BuildBinaryData(buf + mdevcon_offs, tag, 1, 1);		/* devcon */
		strcpy(buf + mfmgr_offs, mfmgr_str);				/* name strings */
		strcpy(buf + mpdev_offs, mpdev_str);
		strcpy(buf + mname_offs, mname_str);

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

/********************************* BuildHeader ******************************
 *
 *  Description: Build device descriptor header
 *			
 *---------------------------------------------------------------------------
 *  Input......: head	ptr to descriptor header
 *               mxxxx  header parameters
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/

static void BuildHeader(
    mod_dev *head,
    U_INT32_OR_64 mport,
    u_int32 msize,
    u_int32 mname_offs,
    u_int32 mfmgr_offs,
    u_int32 mpdev_offs,
    u_int32 mdevcon_offs
)
{
	u_int16 medit = CreateEdition();

	/*-------------------------+
    | common module header     |
    +-------------------------*/
	head->_mh._msync 	= TWISTWORD( MODSYNC );			/* sync */
	head->_mh._msysrev 	= TWISTWORD( 1 );				/* (always 1) */
	head->_mh._msize 	= TWISTLONG( msize );
	head->_mh._mowner 	= TWISTLONG( 0 );				/* superuser */
	head->_mh._mname 	= TWISTLONG( mname_offs );
	head->_mh._maccess 	= TWISTWORD( MP_OWNER_READ | MP_OWNER_WRITE );
	head->_mh._mtylan 	= TWISTWORD( MT_DEVDESC<<8 | ML_ANY );
	head->_mh._mattrev 	= TWISTWORD( MA_REENT<<8 );	
	head->_mh._medit 	= TWISTWORD( medit );
	/* other fields are zero */

	head->_mh._mparity 	= CalcHeaderParity((u_int16*)head);		/* create parity */

	/*-------------------------+
    | descriptor module header |
    +-------------------------*/
#ifdef _LIN64
	head->_mport		= (char *)(TWISTLONGLONG( mport ));
#else
	head->_mport		= (char *)(TWISTLONG( mport ));
#endif
	head->_mmode		= S_IREAD | S_IWRITE;
	head->_mfmgr		= TWISTWORD( (short)mfmgr_offs );
	head->_mpdev		= TWISTWORD( (short)mpdev_offs );
	head->_mdevcon		= TWISTWORD( (short)mdevcon_offs );
	head->_mopt			= TWISTWORD( sizeof(struct spf_opts) );
	head->_mdtype		= DT_SCF;
	/* other fields are zero */

	DEBUG(("edition=0x%04x\n",medit));
}

/********************************* BuildOpt *********************************
 *
 *  Description: Build path options
 *			
 *---------------------------------------------------------------------------
 *  Input......:
 *               mname	module name
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/

static void BuildOpt(struct spf_opts *opt)
{
	/*-------------------------+
    | spf path options         |
    +-------------------------*/
	opt->scf_opts.pd_dtp 	= DT_SCF;
	opt->pds_Chan			= 0;		/* channel 0 */
	opt->pds_Control		= 0;		/* no interlock, permit line edit */
	/* other fields are zero */
}

/********************************* CreateEdition **********************************
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

/***************************** CalcHeaderParity ********************************
 *
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

	for (wordcount = (VHPCNT)/2; --wordcount >= 0;)
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



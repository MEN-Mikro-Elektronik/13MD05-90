/****************************************************************************
 ************                                                    ************
 ************               D E S C G E N                        ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: kp
 *        $Date: 2007/04/03 11:20:03 $
 *    $Revision: 1.17 $
 *
 *  Description: Descriptor Generator for MDIS4
 *
 *               Scans all given input files into a local data base and
 *               creates output file(s) of specified format:
 *
 *               csource     single output file is created <firstfile>.c
 *                           (C-structures for all given toplevel tags)
 *
 *               binary      multiple output files are created <tagname>.bin
 *                           (binary file for each given toplevel tag)
 *
 *               os9         multiple output files are created <tagname>
 *                           (os9 descriptor module for each given toplevel tag)
 *
 *               NOTE: This code contains no OS-specific calls !!!
 *
 *     Required: -
 *     Switches: WINNT - if defined include -winnt option
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: descgen.c,v $
 * Revision 1.17  2007/04/03 11:20:03  DPfeuffer
 * fixed: _WIZ tags will be ignored (for mdis config wizard)
 *
 * Revision 1.16  2003/02/21 13:21:31  kp
 * Version 1.7:
 * _WIZ tags will be ignored (for mdis config wizard)
 *
 * Revision 1.15  2001/01/19 14:37:15  kp
 * added WINNT switch. Include -regfile option only when WINNT defined
 *
 * Revision 1.14  2000/07/04 09:59:36  Schmidt
 * new revision: === Rev 1.6 ===
 *
 * Revision 1.13  2000/07/04 09:21:16  Schmidt
 * winnt option implemented
 *
 * Revision 1.12  1999/12/02 12:09:19  Schmidt
 * option binary replaced with options bin_big and bin_lit
 *
 * Revision 1.11  1999/08/12 09:02:32  kp
 * allow zero length binary fields
 * === Rev 1.5 ===
 *
 * Revision 1.10  1999/06/09 16:02:50  kp
 * fixed help strings for OS9000
 * adapted to OS9000's MDIS/BBISFM
 * === REV 1.4 ===
 *
 * Revision 1.9  1999/04/28 16:27:07  kp
 * support -z option
 * prepared for OS9K
 * === V1.3 ===
 *
 * Revision 1.8  1999/03/09 10:05:14  see
 * ported to Win95, byte-swapping problems fixed
 * V1.2
 *
 * Revision 1.7  1999/03/08 14:44:12  see
 * usage: cosmetics
 *
 * Revision 1.6  1998/07/20 13:10:49  see
 * G_version updated
 * V1.1
 *
 * Revision 1.5  1998/07/17 09:26:14  kp
 * changed G_version to array due to problem with HP-UX gcc sscanf
 *
 * Revision 1.4  1998/07/17 08:47:49  kp
 * cosmetics
 *
 * Revision 1.3  1998/03/27 17:11:16  see
 * csource mode modified: file name is now created in OutCSource
 * all error messages trailed with "***"
 * binary mode implemented
 * os9 mode implemented
 * option -r added
 * option -d added
 * option -o=dir added
 * FileExist added
 * StrToLower added
 * comments added
 * program description added
 * V1.0
 *
 * Revision 1.2  1998/01/19 10:50:53  kp
 * align all tags on 4 byte boundaries
 * V0.5
 *
 * Revision 1.1  1998/01/16 14:48:52  popp
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

static const char RCSid[]="$Id: descgen.c,v 1.17 2007/04/03 11:20:03 DPfeuffer Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define GLOBAL
#include <MEN/men_typs.h>
#include <MEN/desctyps.h>
#include "descgen.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/*--- structure to convert tag type names to its value and vice versa ---*/
typedef struct {
	char 		*name;					/* tag type name */
	u_int16 	val;					/* and its value */
} TAG_TYPE;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
char G_version[] = "V1.8";
char *G_outputDir = ".";

const TAG_TYPE G_tagType[] = {
	{ "BINARY", 	DESC_BINARY },
	{ "U_INT32", 	DESC_U_INT32 },
	{ "STRING", 	DESC_STRING },
	{ "DIR", 		DESC_DIR },
	{ "END", 		DESC_END },
	{ NULL, 0 }
};

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int32 ProcessInFile( char *fname );
static int GetLine( FILE *fp, char *lineBuf, int32 *lineCountP );
static void DumpDB( DESCR_TAG *dir, int32 level );
void Align( DESCR_TAG *topTag );


/********************************* usage *************************************
 *
 *  Description: Show platform dependent usage string and exit
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: excode		program's exit code
 *  Output.....: does not return
 *  Globals....: -
 ****************************************************************************/
void usage( int excode )
{
	fprintf(stderr,"Usage:    descgen [<opts>] <files> [<opts>]\n");
	fprintf(stderr,"Function: MDIS4 descriptor generator\n");
	fprintf(stderr,"Options:\n");
	fprintf(stderr,"    files        meta-descriptor files (*.dsc)\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"    output formats and specific options\n");
	fprintf(stderr,"    -csource     generate C-source code file [default]\n");
	fprintf(stderr,"    -bin_big     generate raw binary file (big endian)\n");
	fprintf(stderr,"    -bin_lit     generate raw binary file (little endian)\n");	
	fprintf(stderr,"    -os9         generate OS-9 descriptor module\n");
	fprintf(stderr,"                 -port=<portaddr>  port address (hex)\n");
	fprintf(stderr,"                 -drv=<drvname>    driver name\n");
	fprintf(stderr,"                 -fmgr=<fmgrname>  file manager name\n");
	fprintf(stderr,"    -os9000      generate OS-9000 descriptor module\n");
	fprintf(stderr,"                 -port=<portaddr>  port address (hex)\n");
/*	fprintf(stderr,"                 -drv=<drvname>    driver name\n");
	fprintf(stderr,"                 -fmgr=<fmgrname>  file manager name\n");*/
	fprintf(stderr,"                 -tp=<proc>        processor type\n");
	fprintf(stderr,"                                   pppc = power PC\n");
/*	fprintf(stderr,"                                   386  = Intel\n");*/
#ifdef WINNT
	fprintf(stderr,"    -winnt       generate .reg-file for Windows NT\n");
#endif
	fprintf(stderr,"\n");
	fprintf(stderr,"    general options\n");
	fprintf(stderr,"    -o=<dir>     output directory\n");
	fprintf(stderr,"    -r           overwrite existing files\n");
	fprintf(stderr,"    -d           debug mode\n");
	fprintf(stderr,"    -v           verbose mode\n");
	fprintf(stderr,"    -z=<file>    get command line options from <file>\n");
	fprintf(stderr,"\n");
	fprintf(stderr, "(c) 1997-2000 by MEN, %s\n", G_version );
	exit(excode);
}


/********************************* main **************************************
 *
 *  Description: Hard to describe
 *			
 *---------------------------------------------------------------------------
 *  Input......:
 *  Output.....:
 *  Globals....: -
 ****************************************************************************/
int main( int argc, char *argv[] )
{
	int i, optsFromFile=FALSE, pass;
	int32 error=0;
	char *firstFile=NULL, *drvname=NULL, *fmgrname=NULL, *os9proc=NULL;
	u_int32 portaddr=0;
	FILE *optFp=NULL;
	char optBuf[200], *opt=NULL;	

	G_outputMode = csource;

	/*--- fill the root object ---*/
	G_objRoot.name = "_root_";
	G_objRoot.type = DESC_DIR;
	
	/*-----------------------------+
    |  Parse command line options  |
    +-----------------------------*/

	/*
     * Do parsing twice: First pass all options, then all files
	 */

	for( pass=0; pass<2; pass++ ){
		for( i=1; i<argc; i++ ){
		
			if( optsFromFile ){
				/* read next line from command line options file */
				if( fgets( optBuf, sizeof(optBuf), optFp ) == NULL ){
					fclose( optFp );
					optsFromFile=FALSE;
				}
				else {
					if( optBuf[strlen(optBuf)-1] == '\n' )
						optBuf[strlen(optBuf)-1] = '\0';
					opt = optBuf;
					i--;
				}
			}
			else opt = argv[i];

			if( !strncmp(opt, "-z=",3)){
				if( (optFp = fopen( opt+3, "r" )) == NULL ){
					fprintf(stderr,"*** Can't open command line opts file %s\n",
							opt+3 );	
				}
				optsFromFile=TRUE;
				i--;
			}
			else if( pass==0 && *opt == '-' ){

				if( !strcmp(opt, "-csource"))
					G_outputMode = csource;

				else if( !strcmp(opt, "-winnt"))
					G_outputMode = winnt;

				else if( !strcmp(opt, "-os9"))
					G_outputMode = os9;

				else if( !strcmp(opt, "-os9000"))
					G_outputMode = os9000;

				else if( !strcmp(opt, "-bin_big"))
					G_outputMode = bin_big;
					
				else if( !strcmp(opt, "-bin_lit"))
					G_outputMode = bin_lit;					

				else if( !strncmp(opt, "-o=",3))
					G_outputDir = StrSave(opt+3);

				else if( !strncmp(opt, "-port=",6))
					sscanf(opt+6,"%x",&portaddr);

				else if( !strncmp(opt, "-drv=",5))
					drvname = StrSave(opt+5);

				else if( !strncmp(opt, "-fmgr=",6))
					fmgrname = StrSave(opt+6);

				else if( !strncmp(opt, "-tp=",4))
					os9proc = StrSave(opt+4);

				else if(strlen(opt)==2){
					/*
					 * one-character options
					 */
					char copt = *(opt+1);

					switch(copt){
					case 'v':
						G_verbose++; break;
					case 'd':
						G_debug++; break;
					case 'r':
						G_overwrite++; break;
					case '?':
						usage(0); break;

					default:
						fprintf(stderr,"*** Unknown option %c\n", opt );
						exit(1);
					}
				}
				else {
					fprintf(stderr,"*** Unknown option %s\n", opt );
					exit(1);
				}

			}
			else if( pass==1 && *opt != '-' ){
				/*----------------------+
				|  Process input files  |
				+----------------------*/
				if( firstFile == NULL )
					firstFile = opt;

				if( (error = ProcessInFile( opt )))
					break;
			}
		}
	}
	/*DEBUG(("mode=%d\n", G_outputMode ));*/

	if( firstFile==NULL ) usage(1);

	if( error ) exit(error);
	
	/*----------------------------+
    |  Generate alignment bytes   |
    +----------------------------*/
	Align( &G_objRoot );

	/*--------------------------+
    |  Dump Internal data base  |
    +--------------------------*/
	if( G_verbose ){
		printf("\n[ INTERNAL DATA BASE ]\n\n");
		DumpDB( &G_objRoot, 0);
	}

	/*------------------------------------+
    |  Output in specified output format  |
    +------------------------------------*/
	switch( G_outputMode ){
	case csource:
		error = OutCSource( firstFile , &G_objRoot );
		break;
	case bin_big:
		G_targetBigEnd = TRUE;
		error = OutBinary( &G_objRoot );
		break;
	case bin_lit:
		G_targetBigEnd = FALSE;
		error = OutBinary( &G_objRoot );
		break;
	case os9:
		G_targetBigEnd = TRUE;
		error = OutOS9(drvname, fmgrname, portaddr, &G_objRoot);
		break;
	case os9000:
		error = OutOS9000(drvname, fmgrname, os9proc, portaddr, &G_objRoot);
		break;
#ifdef WINNT
	case winnt:
		error = OutRegFile( firstFile , &G_objRoot );
		break;
#endif
	default:
		fprintf(stderr, "*** output format currently not supported\n");
		error = 1;
	}
	return error;
}
				
/********************************* DumpDB **********************************
 *
 *  Description: dump internal data base
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: dir		Start Directory
 *				 level		Start Level
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void DumpDB( DESCR_TAG *dir, int32 level )
{
	DESCR_TAG *tag = dir->children;
	int32 i;

	while( tag ){
		for(i=0; i<level*2; i++) putchar(' ');

		printf("%-20s %04x ", tag->name, tag->type);
		
		switch( tag->type ){
		case DESC_DIR:
			printf("\n");
			DumpDB( tag, level+1 );
			break;
		case DESC_U_INT32:
			printf("0x%08x", tag->val.uInt32 );
			break;
		case DESC_BINARY:
			for(i=0; i<tag->val.uInt8.entries; i++ )
				printf("0x%02x ", tag->val.uInt8.arr[i] );
			break;
		case DESC_STRING:
			printf("%s", tag->val.string );
			break;
		}

		printf("\n");
		tag = tag->next;
	}
}
/********************************* ProcessInFile ****************************
 *
 *  Description: Process input file, build internal data base
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: fname		Filename
 *  Output.....: Return: 	error code
 *  Globals....: -
 ****************************************************************************/
static int32 ProcessInFile( char *fname )
{
	FILE *fp=NULL;
	char *lineBuf=NULL;
	int32 error=0;
	int eofFlag;
	int32 lineCount=0;

	/*--- open file ---*/
	if( (fp = fopen(fname, "rb")) == NULL ){
		fprintf( stderr, "*** can't open input file %s (error=%d)\n",
				fname, errno );
		return errno;
	}

	/*--- alloc line buffer ---*/
	if( (lineBuf = malloc(MAX_LINE_LENGTH)) == NULL ){
		fprintf( stderr, "*** can't alloc line buffer (error=%d)\n", errno );
		error = errno;
		goto cleanup;
	}
				

	printf("processing %s\n", fname );

	ParseInit( fname, &G_objRoot );

	/*---------------------+
    |  Read lines in file  |
    +---------------------*/
	do {
		if( (eofFlag = GetLine( fp, lineBuf, &lineCount)) < 0 ){
			error = 1;
			break;
		}
		if( *lineBuf != '\0' ){	/* ignore empty lines */
			DEBUG(("line %d=<%s>\n", lineCount, lineBuf ));

			if( ParseLine( lineBuf, lineCount )){
				error = 1;
				eofFlag = 1;
			}
		}
	} while( eofFlag==0 );

cleanup:
	if( lineBuf )
		free(lineBuf);
	if( fp )
		fclose(fp);
	return error;
}


/********************************* GetLine **********************************
 *
 *  Description: Get line from input file
 *			
 *	Get a single or multiple lines (when continuation sign '\' present) from
 *  the input file.
 *
 *	Remove any comment at end of line.
 *  Remove any trailing white space
 *  End of line can be anything: CR, LF or CR-LF.
 *  Return '\0' terminated line
 *			
 *---------------------------------------------------------------------------
 *  Input......: fp			input file stream
 *				 lineBuf	buffer where input line will be stored
 *				 lineCountP pointer to linecounter
 *  Output.....: Return:	0=ok 1=EOF -1=error
 *				 *lineCountP updated
 *  Globals....: -
 ****************************************************************************/
static int GetLine( FILE *fp, char *lineBuf, int32 *lineCountP )
{
	char *p = lineBuf;
	int c=-1, c1;
	int endOfLine=0;
	int comment=0;
	int32 count=0;

	while( endOfLine==0 && (c = getc(fp)) != EOF ){
		switch(c){
		case 0xa:				/* end-of line chars */
		case 0xd:
			c1 = getc(fp);		/* check for two-char end-of line */
			if( (c==0xa && c1 != 0xd) ||
			    (c==0xd && c1 != 0xa))

				ungetc(c1, fp);	/* put char back to stream */

			(*lineCountP)++;

			if( count==0 || p[-1] != '\\' )	/* check for continuation symbol */
				endOfLine=1;
			else {
				p--;			/* remove continuation symbol */
				count--;
			}
			break;
		case '#':				/* comment start */
			comment=2;
			break;			

		case '/':
			comment++;
			if( comment == 2 ){
				p--;			/* remove first '/' */
				count--;
				break;
			}
				
		default:
			if( comment < 2 ){
				if( ++count >= MAX_LINE_LENGTH-1 ){
					fprintf(stderr, "*** line %d too long\n",*lineCountP);
					return -1;
				}
				*p++ = c;		/* put char into line bufer */
			}
		}
	}

	/*--- remove trailing white space ---*/
	while( p != lineBuf ){
		if( p[-1]==' ' || p[-1]=='\t' )
			p--;
		else
			break;
	}
	*p = '\0';
	return c==EOF ? 1:0;
}

/********************************* Align ***********************************
 *
 *  Description: Generate alignment bytes for each tag
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: topTag		root directory of objects
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
void Align( DESCR_TAG *topTag )
{
	DESCR_TAG *tag = topTag->children;
	static int32 offs;
	int32 len;
	u_int16 pad;

	while( tag ){		
		pad = 0;
		/*printf("[%s] ",tag->parent == &G_objRoot ? "G_objRoot":"parent");*/

		if( tag->parent == &G_objRoot ){
			offs = 0;
			len = 0;
		}
		else
			len = 4 + strlen(tag->name)+1;

		switch( tag->type ){
		case DESC_DIR:
			/*--- for directory tag ---*/
			if( (len+offs) & 0x3 ){
				tag->align1 = 4 - ((len+offs) & 0x3 );
				len += tag->align1;
			}

			DEBUG(("\nsubdir %-20s typ=0x%04x offs=0x%04x len=%d pad1=%d\n",
				  tag->name, tag->type, offs, len, tag->align1));

			offs += len;

			Align( tag );

			/*--- for end tag --*/
			offs += 4;
			goto next;

			break;
		case DESC_U_INT32:
			pad = 4 - ((offs+len) % 4);
			if( pad==4 ) pad=0;
			len += 4;
			tag->align1 = pad;
			break;
		case DESC_BINARY:
			len += tag->val.uInt8.entries+1; /* add 1 for pad flag */
			if( (len+offs) & 1 ) pad=1;
			tag->align2 = pad;
			break;
		case DESC_STRING:
			len += strlen(tag->val.string)+1;
			if( (len+offs) & 1 ) pad=1;
			tag->align2 = pad;
			break;
		}

		len += pad;
		
		/*--- alway align next tag on 4 byte boundary ---*/
		if( (len+offs) & 2) {
			tag->align2 += 2;
			len += 2;
		}

		DEBUG(("   tag %-20s typ=0x%04x offs=0x%04x len=%d pad1=%d pad2=%d\n",
			  tag->name, tag->type, offs, len, tag->align1, tag->align2));

		offs += len;

next:
		tag->len = (u_int16)len-4;

		tag = tag->next;
	}
}

/********************************* TagTypeName ******************************
 *
 *  Description: Convert tag type value to name
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: val		tag type value
 *  Output.....: Return:	name of type or NULL if not found
 *  Globals....: G_tagType
 ****************************************************************************/
char *TagTypeName( u_int16 val )
{
	const TAG_TYPE *t = G_tagType;

	while( t->name != NULL ){
		if( t->val == val )
			return t->name;
		t++;
	}
	return NULL;
}

/********************************* TagTypeVal ********************************
 *
 *  Description: Convert tag type name to value
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: name		name of type
 *  Output.....: Return:	value of type or 0 if not found
 *  Globals....: G_tagType
 ****************************************************************************/
u_int16 TagTypeVal( char *name )
{
	const TAG_TYPE *t = G_tagType;

	while( t->name != NULL ){
		if( !strcmp( t->name, name ) )
			return t->val;
		t++;
	}
	return 0;
}

/********************************* StrSave **********************************
 *
 *  Description: allocate memory for string and copy string
 *			
 *			
 *---------------------------------------------------------------------------
 *  Input......: str			string to be saved
 *  Output.....: str			new memory for string
 *  Globals....: -
 ****************************************************************************/
char *StrSave(char *str)
{
	char *dest;
	
	if((dest = malloc(strlen(str)+1)))
		strcpy(dest,str);

	if( !dest ){
		fprintf(stderr, "*** StrSave: Out of memory\n");
		exit(1);
	}
	return dest;
}			

/********************************* FileExist ******************************
 *
 *  Description: Check if file already exists
 *			
 *---------------------------------------------------------------------------
 *  Input......: fname		file name
 *  Output.....: return		0 = file does not exist
 *							1 = file does exist
 *  Globals....: -
 ****************************************************************************/

int32 FileExist(char *fname)
{
	FILE *fp;

	if ((fp = fopen(fname, "r")) != NULL) {
		fclose(fp);
		return(1);
	}

	return(0);
}

/********************************* StrToLower *****************************
 *
 *  Description: Convert a string to lower case
 *			     dest and source may be the same
 *
 *---------------------------------------------------------------------------
 *  Input......: source		source string
 *  Output.....: dest 		destination string
 *  Globals....: -
 ****************************************************************************/

char *StrToLower(char *dest, char *source)
{
	register char *dp = dest;

	while(*source)
		*dp++ = tolower(*source++);

	*dp = '\0';	/* terminate string */		

	return dest;
}









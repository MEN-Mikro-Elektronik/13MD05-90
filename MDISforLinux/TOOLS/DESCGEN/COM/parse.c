/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: parse.c
 *      Project: descgen
 *
 *       Author:
 *
 *  Description: Input file parser. Builds internal database
 *
 *
 *     Required:
 *     Switches:
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1997-2019, MEN Mikro Elektronik GmbH
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
#include <ctype.h>

#include <MEN/men_typs.h>
#include <MEN/desctyps.h>
#include "descgen.h"

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
static DESCR_TAG	*G_curTagDir;
static char			*G_fname;

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static char *SkipWhite( char *p );
static char *GetSymbol( char *p, char *buf, int32 len );
static int IsValidCSymbol( char *p );
static int32 ParseInt(
char *p,
void *arr,
int32 maxEntries,
int32 bytesPerEntry,
int32 lineCount );
char *GetInt( int base, char *p, u_int32 *valP);

/********************************* ParseInit ********************************
 *
 *  Description: Init parser for new input file
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: fname			File name of file (just for error messages)
 *				 rootObj		Root object
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
void ParseInit( char *fname, DESCR_TAG *rootObj )
{
	G_fname = fname;
	G_curTagDir = rootObj;
}

/********************************* ParseLine ********************************
 *
 *  Description: Parse input line
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: lineBuf		current line, null terminated
 *				 lineCount		current physical line number in file
 *  Output.....: Return			error code
 *  Globals....: G_outputMode
 ****************************************************************************/
int32 ParseLine( char *lineBuf, int32 lineCount )
{
	char *p = lineBuf;
	char symbolBuf[100];
	int toplevel = G_curTagDir->parent == NULL;
	DESCR_TAG *tag, *tag1;

	/*---------------------------------+
	|  Search for object name or '}'   |
	+---------------------------------*/
	p = SkipWhite(p);

	if( *p == '}' ){
		/*-------------------+
		|  End of directory  |
		+-------------------*/
		if( G_curTagDir->parent == NULL ){
			fprintf( stderr, "*** %s: line %d: unexpected '}'\n",
					G_fname, lineCount );
			return 1;
		}
		G_curTagDir = G_curTagDir->parent; /* up one directory */

		p++;
		if( SkipWhite(p) != p ){
			fprintf( stderr, "*** %s: line %d: Warning: ignoring \"%s\"\n",
					G_fname, lineCount, p );
		}
		return 0;
	}
	else {
		/*---------------------------------------------------+
		|  Must be the name of a tag.                        |
		|  In csource mode, check if this is a valid C-name  |
		+---------------------------------------------------*/
		p = GetSymbol( p, symbolBuf, sizeof(symbolBuf) );

		/*--- check for MDIS wizard internal tags; ignore these ---*/
		if( strncmp( symbolBuf, "_WIZ", 4 ) == 0 ){
			/*printf("ignoring %s\n", symbolBuf );*/
			return 0;
		}

		if( (toplevel && G_outputMode==csource) || toplevel==0 ){
			/*--- check if valid C-symbol ---*/
			if( ! IsValidCSymbol( symbolBuf )){
				fprintf( stderr, "*** %s: line %d: invalid symbol name %s\n",
						G_fname, lineCount, symbolBuf );
				return 1;
			}
		}
	}

	/*-----------------------------+
	|  Create a new tag structure  |
	+-----------------------------*/
	if( (tag = malloc(sizeof(DESCR_TAG))) == NULL ){
		fprintf( stderr, "*** %s: line %d: can't allocate new tag\n",
				G_fname, lineCount );
		return 1;
	}

	tag->next 		= NULL;
	tag->parent 	= G_curTagDir;
	tag->children	= NULL;
	tag->name		= StrSave(symbolBuf);
	tag->type		= 0;
	tag->align1		= 0;
	tag->align2		= 0;

	/*--- append new tag to end of current directory ---*/

	if( G_curTagDir->children == NULL )
		G_curTagDir->children = tag;
	else {
		for( tag1=G_curTagDir->children; tag1->next; tag1=tag1->next )
			;
		tag1->next = tag;
	}

	/*------------------------+
	|  Search for '=' or '{'  |
	+------------------------*/
	p = SkipWhite(p);

	if( *p=='{' ){
		/*----------------+
		|  Directory tag  |
		+----------------*/
		tag->type = DESC_DIR;

		/*printf("New dir %s\n", tag->name );*/
		G_curTagDir = tag;

		p++;
		if( SkipWhite(p) != p ){
			fprintf( stderr, "*** %s: line %d: Warning: ignoring \"%s\"\n",
					G_fname, lineCount, p );
		}

	}
	else if( *p=='=' ){
		/*printf("New tag %s\n", tag->name );*/

		/*--------------+
		|  Normal tag   |
		+--------------*/
		p++;					/* skip '=' */
		p = SkipWhite(p);

		/*--- get tag type ---*/
		p = GetSymbol( p, symbolBuf, sizeof(symbolBuf) );

		if( (tag->type = TagTypeVal( symbolBuf )) == 0 ){
			fprintf( stderr, "*** %s: line %d: invalid tag type %s\n",
				G_fname, lineCount, symbolBuf );
			return 1;
		}

		/*--- get symbol value ---*/
		p = SkipWhite(p);

		switch( tag->type ){
		case DESC_BINARY:
			{
				u_int8 *valBuf;
				int32 entries;

				if( (valBuf = malloc( sizeof(u_int8)*MAX_LINE_LENGTH)) ==
				   NULL){
					fprintf(stderr, "*** %s: line %d: Out of memory\n",
							G_fname, lineCount);
					return -1;
				}

				if( (entries = ParseInt( p, (void *)valBuf, MAX_LINE_LENGTH,
										1, lineCount )) < 0 )
					return 1;

				if( entries > 0 ) {
					if( (tag->val.uInt8.arr = malloc( entries )) == NULL ){
						fprintf(stderr, "*** %s: line %d: Out of memory\n",
								G_fname, lineCount);
						return -1;
					}

					memcpy( tag->val.uInt8.arr, valBuf, entries );
				}
				else
					tag->val.uInt8.arr = NULL;

				free( valBuf );
				tag->val.uInt8.entries = entries;
			}
			break;
		case DESC_U_INT32:
			{
				int32 entries;
				if( (entries = ParseInt( p, (void *)&tag->val.uInt32, 1, 4,
										lineCount )) < 0 )
					return 1;
				if( entries != 1 ){
					fprintf(stderr, "*** %s: line %d: Expected exactly one value "
									"behind U_INT32, but found %d\n",
									G_fname, lineCount, entries);
					return 1;
				}
			}
			break;
		case DESC_STRING:
			tag->val.string = StrSave(p);
			break;
		default:
			fprintf( stderr, "*** %s: line %d: tag type %s not allowed\n",
				G_fname, symbolBuf );
			return 1;
		}

	}
	else {
		fprintf( stderr, "*** %s: line %d: expected '=' or '{'\n",
				G_fname, lineCount );
		return 1;
	}
	return 0;
}

/********************************* SkipWhite ********************************
 *
 *  Description: Skip any white space
 *
 *  Do not advance behind '\0'
 *---------------------------------------------------------------------------
 *  Input......: p			ptr to start in line
 *  Output.....: Return		ptr to next non-white space
 *  Globals....: -
 ****************************************************************************/
static char *SkipWhite( char *p )
{
	while( *p ){
		if( *p!=' ' && *p!='\t' )
			break;
		p++;
	}
	return p;
}

/********************************* GetSymbol ********************************
 *
 *  Description: Copy the symbol name from the line buffer
 *
 *	copying takes place until a invalid character is detected
 *  Valid characters are a-z, A-Z, 0-9, _, /,  \
 *---------------------------------------------------------------------------
 *  Input......: p			ptr in line
 *				 buf		buffer where symbol will be copied to
 *				 len		length of buf
 *  Output.....: Return		ptr in input line behind symbol
 *				 buf		symbol name
 *  Globals....: -
 ****************************************************************************/
static char *GetSymbol( char *p, char *buf, int32 len )
{
	while( *p && len>1){
		if( isalnum(*p) || *p=='_' || *p=='\\' || *p=='/' ){
			*buf++ = *p++;
			len--;
		}
		else
			break;
	}
	*buf = '\0';
	return p;
}

/********************************* IsValidCSymbol ****************************
 *
 *  Description: Checks if the symbol is a valid C-syntax
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: p			ptr to symbol name
 *  Output.....: Return		0=not a vaild symbol 1=it is
 *  Globals....: -
 ****************************************************************************/
static int IsValidCSymbol( char *p )
{
	if( !isalpha(*p) && (*p != '_') ) return 0;
	p++;

	while( *p ){
		if( isalnum(*p) || *p=='_' )
			p++;
		else
			return 0;
	}
	return 1;
}

/********************************* ParseInt **********************************
 *
 *  Description: Get an array of integer values
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: p			ptr to input line
 *				 arr		place to save results
 *				 maxEntries	max size of arr
 *				 bytesPerEntry number of bytes per entry
 *				 lineCount	for error messages
 *  Output.....: >0 = number of entries  <0 = error
 *  Globals....: -
 ****************************************************************************/
static int32 ParseInt(
char *p,
void *arr,
int32 maxEntries,
int32 bytesPerEntry,
int32 lineCount )
{
	u_int32 value;
	int entries=0;

	while( *p ){
		if( p[0]=='0' && p[1]=='x' ){
			p = GetInt( 16, p+2, &value );
		}
		else if( p[0]=='%' ){
			p = GetInt( 2, p+1, &value );
		}
		else if( isdigit( p[0] )){
			p = GetInt( 10, p, &value );
		}
		else {
			fprintf(stderr, "*** %s: line %d: illegal integer %s\n",
					G_fname, lineCount, p );
			return -1;
		}

		if( ++entries > maxEntries ){
			fprintf(stderr, "*** %s: line %d: too many elements\n",
					G_fname, lineCount);
			return -1;
		}

		if( bytesPerEntry<4 && value >= (u_int32)(1L<<(8*bytesPerEntry))){
			fprintf(stderr, "*** %s: line %d: value 0x%04x out of range\n",
					G_fname, lineCount, value);
			return -1;
		}

		switch( bytesPerEntry ){
		case 1:
			*(u_int8 *)arr = (u_int8)value;
			break;
		case 2:
			*(u_int16 *)arr = (u_int16)value;
			break;
		case 4:
			*(u_int32 *)arr = value;
			break;
		}

		arr = (void *)((u_int8 *)arr + bytesPerEntry );

		p = SkipWhite(p);
		if( *p != ',' && *p != '\0'){
			fprintf(stderr, "*** %s: line %d: comma or EOL expected at %s\n",
					G_fname, lineCount, p);
			return -1;
		}
		if( *p == ',' ) p++;

		p = SkipWhite(p);


	}

	return entries;
}
/********************************* GetInt ***********************************
 *
 *  Description: convert ASCII to integer
 *
 *
 *---------------------------------------------------------------------------
 *  Input......: base		radix 2, 10, 16
 *				 p			ptr to input line
 *				 valP		pointer to variable where value will be stored
 *  Output.....: Return		ptr to next character behind numeric value
 *				 *valP		converted value
 *  Globals....: -
 ****************************************************************************/
char *GetInt( int base, char *p, u_int32 *valP)
{
	u_int32 tmp=0, new;

	while( *p ){
		if( *p >= '0' && *p <= '9' )
			new = *p - '0';
		else if( *p >= 'a' && *p <= 'f' )
			new = *p - 'a' + 10;
		else if( *p >= 'A' && *p <= 'F' )
			new = *p - 'A' + 10;
		else
			goto ex;

		if( (int)new >= base )
			goto ex;

		tmp = (tmp*base) + new;
		p++;
	}
ex:
	/*printf("GetInt: %04x\n", tmp );*/
	*valP = tmp;
	return p;
}


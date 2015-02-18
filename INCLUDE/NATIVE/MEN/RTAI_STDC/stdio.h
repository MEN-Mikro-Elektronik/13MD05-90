/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  stdio.h
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2004/06/09 10:30:29 $
 *    $Revision: 1.2 $
 *
 *  	 \brief  Header file to let RTAI MDIS example programs perform
 *				 a limted set of stdio functions
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: stdio.h,v $
 * Revision 1.2  2004/06/09 10:30:29  kp
 * removed BIG/LITTLE endian switches (now taken from build system)
 *
 * Revision 1.1  2003/04/11 16:16:53  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _RTAI_STD_STDIO_H
#define _RTAI_STD_STDIO_H

#include <linux/types.h>
#include <linux/kernel.h>

#include <rtai.h>
#include <rtai_fifos.h>
#include <rtai_sched.h>

#include <MEN/RTAI_STDC/stdlib.h>
#include <asm/byteorder.h>

#define UOS_FOR_RTAI /* set conditional switch for USR_OSS functions */
#define UTL_FOR_RTAI /* set conditional switch for USR_UTL functions */


#define main		men_rtai_program_entry

#define printf 		men_rtai_printf
#define vfprintf 	men_rtai_vfprintf
#define vprintf 	men_rtai_vprintf
#define fprintf 	men_rtai_fprintf
#define fflush		men_rtai_fflush
#define fgets		men_rtai_fgets
#define getchar		men_rtai_getchar
#define getc		men_rtai_getc
#define fopen		men_rtai_fopen
#define fclose		men_rtai_fclose
#define fread		men_rtai_fread
#define fwrite		men_rtai_fwrite
#define feof		men_rtai_feof
#define ferror		men_rtai_ferror
#define clearerr	men_rtai_clearerr
#define fseek		men_rtai_fseek
#define ftell		men_rtai_ftell
#define sprintf		men_rtai_sprintf
#define snprintf	men_rtai_snprintf
#define vsprintf	men_rtai_vsprintf
#define vsnprintf	men_rtai_vsnprintf

typedef void FILE;
typedef long fpos_t;

#define stdin  (FILE*)1
#define stdout (FILE*)2
#define stderr (FILE*)3

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define EOF -1

int printf( const char *fmt, ... );

int fprintf( FILE *fp, const char *fmt, ... );

int vfprintf( FILE *fp, const char *fmt, va_list args );
int vprintf( const char *fmt, va_list args );
int snprintf(char *str,size_t size,const char *format,...);
int vsnprintf(char* str, size_t size, const char *format, va_list arg_ptr);
int sprintf(char *dest,const char *format,...);
int vsprintf(char *dest,const char *format, va_list arg_ptr);

char *fgets(char *s, int size, FILE *fp);
int getc(FILE *fp);

extern FILE *fopen (const char *path, const char *mode);
extern int fclose( FILE *fp );
extern size_t fread( void *ptr, size_t size, size_t nmemb, FILE *fp );
extern size_t fwrite( const void *ptr, size_t size, size_t nmemb, FILE *fp );
extern int __inline fflush (FILE *fp) { return 0; }
extern int __inline getchar(void) { return getc(stdin); }
extern int feof( FILE *fp );
extern void clearerr( FILE *fp);
extern int fseek( FILE *fp, long offset, int whence);
extern long ftell( FILE *stream);

extern int __inline fgetpos( FILE *fp, fpos_t *pos)
{
	long p = ftell(fp);
	if( p < 0 )
		return -1;
	*pos = p;
	return 0;
}

extern int __inline fsetpos( FILE *fp, fpos_t *pos)
{
	return fseek(fp, *pos, SEEK_SET);
}

extern void __inline rewind( FILE *fp )
{
	fseek( fp, 0, SEEK_SET );
}

#endif /* _RTAI_STD_STDIO_H */

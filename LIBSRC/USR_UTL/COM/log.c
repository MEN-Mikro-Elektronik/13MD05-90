/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: log.c
 *      Project: usr_utl
 *
 *       Author: ww
 *        $Date: 2014/10/29 12:02:33 $
 *    $Revision: 1.3 $
 *
 *  Description: debugger logfile
 *
 *
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: log.c,v $
 * Revision 1.3  2014/10/29 12:02:33  channoyer
 * R: Files for OS9 cant be compiled under QNX due to different errors
 * M: enclose whole code with #ifndef __QNX__
 *
 * Revision 1.2  2013/10/29 15:40:07  ts
 * R: Files for OS9 cant be compiled under VxWorks due to different errors
 * M: enclose whole code with #ifndef VXWORKS
 *
 * Revision 1.1  2012/12/05 10:53:48  ww
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2007 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
#if !defined(VXWORKS) && !defined(__QNX__) && !defined(LINUX) 
#ifdef	RCSID
static const char RCSid[]="$Header: /dd2/CVSR/COM/LIBSRC/USR_UTL/COM/log.c,v 1.3 2014/10/29 12:02:33 channoyer Exp $";
#endif

#include	<stdio.h>
#include	<stdarg.h>
#include    <stdlib.h>
#include	<string.h>
#include	<time.h>
#include	<errno.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define	BUILD		0
#define	VERSION		1
#define	EDITION		VERSION
#define	YEAR		"2012"

#define DEBUGP(_x_)
#define DEBUGE(_x_)
#define DEBUG1(_x_)
#define DEBUG2(_x_)

#define	E_LOG_OPEN  0x200
#define	E_LOG_CLOSE 0x201

#define S_FILEPOINTER
/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
extern int	op_log;
/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
#ifndef S_FILEPOINTER
FILE	*G_fp;
int		G_logfile_flag;
#endif

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
void    UTL_Getdate( char *bp );
void    UTL_Gettime( char *bp );

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
#if 0
error_code			g_logfile_open( FILE **fp, char *filename );
error_code			g_logfile_close( FILE *fp );
void				g_logfile_printf( FILE *fp, char *frmt, ... );
void				g_logfile_printe( FILE *fp, int error );

/* version.c */
u_int32				g_version();
u_int32				g_build();
u_int32				g_year();
#endif

u_int32	g_version()	{	return((u_int32)EDITION);	}
u_int32	g_build()	{	return((u_int32)BUILD);		}
u_int32	g_year()	{	return((u_int32)YEAR);		}

/********************************** g_revision ******************************
 *
 *  Description:  revision ans date
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  pointer
 *  Globals....:  ---
 ****************************************************************************/
char *
g_revision()
{
    static char *revision   = "$Revision: 1.3 $";

	return &revision[11];
}
char *
g_date()
{
    static char *date       = "$Date: 2014/10/29 12:02:33 $";

	return &date[7];
}
char *
g_copyright()
{
	static char *copyright 	= "(c) Copyright 2012 by MEN Mikro Elektronik GmbH, Nuremberg, Germany";

	return &copyright[0];
}

#ifndef S_FILEPOINTER
/**********************************************************************/
/** Create Logfile
 *
 *  \param  lp      \IN     pointer to logfile name
 *  \return	error   \OUT    error code
 */
error_code
UTL_Logfile_Open(char *lp)
{
	char		fname[40];
	error_code	error;

	DEBUGP(("->logfile_open filename='%s'\n",lp));

	error = 0;

	if(op_log)
	{
		strcpy(fname,lp);
		strcat(fname,".log");

		DEBUGP(("  filename='%s'\n",fname));

		if ((G_fp = fopen(fname,"wb")) == NULL)
		{
			DEBUGP(("*** can't open file '%s', error=%d\n",fname,errno));
			G_logfile_flag =0;
			error = E_LOG_OPEN;
		}
		else {
			fputs("\nopen logfile\n", G_fp);
			fprintf( G_fp, "V%3.3s, Build %d, (c) Copyright by men GmbH %4.4s\n", g_revision(), g_build(), g_date());
			fflush(G_fp);
			G_logfile_flag = 1;
		}
	}
	DEBUGP(("<-logfile_open filename (%d)\n",error));
	return error;
}

error_code
UTL_Logfile_Close()
{
	if (G_fp) {
		fputs("\nclose logfile\n", G_fp);
		fflush(G_fp);
		return ( fclose(G_fp) );
	}
	else
		return 0;
}

void
UTL_Logfile_Write( char c )
{
	int	static cnt = 0;

	if (G_logfile_flag)
	{
		fprintf( G_fp, "0x%02x ",(u_int8)c);

		if (++cnt >= 50)
		{
			cnt =0;
			fprintf( G_fp, "\n");
		}
/*		fputc( c, G_fp);
*/	}
}

void
UTL_Logfile_Printf( char *frmt, ... )
{
    va_list args;
    va_start(args, frmt);

	if (G_logfile_flag)
   		vfprintf(G_fp, frmt, args);

    va_end(args);
}

#if 0
void print_debug( DBG_HANDLE *dbg, char *frmt, ... )
{
    va_list args;
    va_start(args, frmt);

   	vprintf(frmt, args);

    va_end(args);
}
#endif

#else

/**********************************************************************/
/** Open Logfile
 *
 *  \param  fpP
 *  \param  filename
 *  \return error       0=ok, or negative error number
 */

error_code
UTL_Logfile_Open( FILE **fpP, char *filename )
{
	FILE 		*fp;
	error_code	error;

	DEBUG1(("->l_logfile_open filename='%s'\n",filename));

	error = 0;

	if ((fp = fopen(filename,"w")) == NULL)
	{
		DEBUGE(("*** can't open file '%s', error=%d\n",filename,errno));
		error = E_LOG_OPEN;
	}
	else {
		//fputs("->open\n", fp);
        //fprintf( fp, "V%3.3s, Build %d, (c) Copyright by men GmbH %4.4s\n",
        //								g_revision(), g_build(), g_date());
        //fprintf( fp, "V%d.%d, Build %d,\n(c) Copyright %s by MEN Mikro Elektronik GmbH, Nuremberg, Germany\n",
        //								g_version()/100, g_version()%100, g_build(), g_year());
		//fprintf( fp, "V%d.%d, Build %d\n",	g_version()/100, g_version()%100, g_build());
		//fflush(fp);
	}
	*fpP = fp;

	DEBUG2(("  fp=%x\n",(u_int32)fp));
	DEBUG1(("<-l_logfile_open filename (%d)\n",error));
	return error;
}
error_code
UTL_Logfile_Close( FILE *fp )
{
	if (fp)
	{
//		fputs("\n<-close\n", fp);
		fflush(fp);
		return ( fclose(fp) );
	}
	else
		return 0;
}
#if 1
void
UTL_Logfile_Printf( FILE *fp, char *frmt, ... )
{
	if (fp)
	{
    	va_list args;
    	va_start(args, frmt);

		vfprintf( fp, frmt, args);

    	va_end(args);
		fflush(fp);
	}
}
void
UTL_Logfile_Printf_Stamp( FILE *fp, char *frmt, ... )
{
	char	buf[32];

	if (fp)
	{
    	va_list args;
    	va_start(args, frmt);

		UTL_Getdate( buf );		fprintf( fp, "%s - ",buf);
		UTL_Gettime( buf );		fprintf( fp, "%s:  ",buf);

		vfprintf( fp, frmt, args);

    	va_end(args);
		fflush(fp);
	}
}
#else
void
UTL_Logfile_Printe( FILE *fp, int error )
{
	static	u_int32 cnt = 0;
	char	buf[32];
	int		ln;

	DEBUG1(("->l_logfile_printe: error=%d\n",error));

	if (fp)
	{
		fprintf( fp, "%5d: ", cnt++ );

		UTL_Getdate( buf );	fprintf( fp, "%s - ",buf );
		UTL_Gettime( buf );	fprintf( fp, "%s - ",buf );

		fprintf( fp, "%3d - ", error );

		if ( (ln = g_error_string(error,buf)) )
			fprintf( fp, "%s\n", buf );
		else
			fprintf( fp, "???\n" );

		fflush(fp);
	}
	DEBUG1(("<-l_logfile_printe\n"));
}
#endif
#endif

/**********************************************************************/
/** convert date to string "22.07.2005"
 *
 *  \param  bp			buffer where the date should be stored
 *  \return ---
 */
void
UTL_Getdate( char *bp )
{
	struct tm	*tmp;
	time_t		tm;

	time(&tm);

	tmp = localtime(&tm);

	strftime(bp, 32, "%d.%m.%Y", tmp);
}

/**********************************************************************/
/** convert time to string "10:12:24"
 *
 *  \param  bp			buffer where the date should be stored
 *  \return ---
 */
void
UTL_Gettime( char *bp )
{
	struct tm	*tmp;
	time_t		tm;

	time(&tm);

	tmp = localtime(&tm);

	strftime(bp, 32, "%H:%M:%S", tmp);
}
#endif /* VXWORKS && __QNX__ */

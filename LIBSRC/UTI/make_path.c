/*****************************************************************************

Project...........: uti.l
Filename..........: make_path.c
Author............: K.P.
Creation Date.....: 03.12.90

Description.......: concatenate filename to pathname

********************* ALL RIGHTS RESERVED BY MEN GMBH ************************/
#include <string.h>

#if !defined NULL
#define NULL 0L
#endif

#if defined (WIN32)
# define PSEP '\\'
#endif

#if defined (HPRT) || defined (HPUX) || defined (OS9) || defined(LINUX) || defined(OS9000)
# define PSEP '/'
#endif

char *make_path(buf, path, file, size)
char *buf;	/* result buffer */
char *path;	/* the pathname */
char *file;	/* the filename */
int  size;	/* sizeof buf 	*/
{
	if((strlen(path) + strlen(file) + 1) >= size)
	    return NULL;

	if (buf != path)             			/* buf not equal ? */
		strcpy(buf,path);                       /* copy path */

	if(*(buf+strlen(buf)-1) == '.')		/* remove '.' */
		*(buf+strlen(buf)-1) = 0;

	if(*(buf+strlen(buf)-1) != PSEP) {      /* add '\' */
		*(buf+strlen(buf)+1) = 0;
		*(buf+strlen(buf))   = PSEP;
	}

	strcat(buf,file);                       /* add file */
	return(buf);
}



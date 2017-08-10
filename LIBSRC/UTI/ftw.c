/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: ftw.c.c
 *      Project: UTI library
 *
 *       Author: 
 *        $Date: 1999/03/24 14:18:00 $
 *    $Revision: 1.8 $
 *
 *  Description: walk down a file tree
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
 * $Log: ftw.c,v $
 * Revision 1.8  1999/03/24 14:18:00  see
 * Updated for Visual C 5.0
 *
 * Revision 1.7  1999/01/20 14:54:17  kp
 * updated for LINUX
 *
 * Revision 1.6  1998/01/08 08:51:45  popp
 * Updated for Visual C 4.0
 *
 * Revision 1.5  1997/12/19 14:50:52  kp
 * HP/OS9 merge
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: ftw.c,v 1.8 1999/03/24 14:18:00 see Exp $";

#include <stdio.h>
#include <errno.h>

#ifdef OS9
# include <direct.h>
# include <modes.h>
# include <dir.h>
#endif

#if defined (HPUX) || defined (HPRT) || defined(LINUX)
# include <types.h>
# include <dir.h>
# include <stat.h>
# define POSIX
#endif

#if defined(WIN32)
# define DBG(cmd)	;
# include <stdlib.h>
# include <direct.h>
# include <string.h>
# include <io.h>
#endif

#include <MEN/uti.h>

#if defined (HPUX) || defined (HPRT) || defined OS9 || defined(LINUX)
# define DBG(cmd)	;

#define MAXPATH 128

int ftw(startdir, func, depth, arg1, arg2, arg3)
char	*startdir;				/* path of start directory 					*/
int		(*func)();				/* the function to be called for each file 	*/
int		depth;					/* directory depth (0 = current dir only)	*/
long	arg1, arg2, arg3;		/* additional args for func()				*/
{	
	static struct fildes statb;	/* buffer for filedescriptor				*/
	struct direct *entry;
	char   	*fname, *make_path();
	char 	buf[MAXPATH];
	DIR 	*fd;
	int		ret = 0;

	DBG(printf("opendir=%s\n",startdir));

	if(fd = opendir(startdir)){
		while(entry = readdir(fd)){
		    fname = &(entry->d_name[0]);
			DBG(printf("readdir=%s\n",fname));

			if((!strcmp(".", fname)) || (!strcmp("..", fname)))
				continue; 
			
			/* create whole pathname */
			if(make_path(buf, startdir, fname, MAXPATH) == 0L){
				ret = -1;
				break;
			}

#ifdef OS9
			/* stat the file */
			if (os9stat(buf, &statb) == -1) {
				DBG(printf("*** can't os9stat: errno=%d\n",errno));

				if (errno == E_BPNAM)	/* ignore bad pathname error */
					continue;
				else {
					ret = -1;
					break;
				}
            }
			DBG(printf("os9stat: fd_att=0x%x errno=%d\n",statb.fd_att,errno));
#endif
#ifdef POSIX
			/* stat the file */
			if(stat(buf, &statb) == -1){
				ret = -1;
				break;
			}
#endif

			if((ret = (*func)(startdir, fname, &statb, arg1, arg2, arg3)) < 0)
				break;
							
#ifdef OS9
			if(statb.fd_att & S_IFDIR && depth > 0 && ret == 0){
				/* rekursive call to nested directories */
				if((ret = ftw(buf, func, depth-1, arg1, arg2, arg3)) != 0) {
#endif
#ifdef POSIX
			if((statb.st_mode & S_IFMT) == S_IFDIR && depth > 0 && ret == 0){
				if((ret = newftw(buf, func, depth-1, arg1, arg2, arg3)) != 0) {
#endif
					printf("*** ftw startdir=%s failed\n",buf);
					break;
				}
			}
		}	
				
		DBG(printf("closedir=%s\n",startdir));
		closedir(fd);
	}
	else {
	   ret = -1;  /* can't opendir */
	   DBG(printf("*** can't opendir=%s\n",startdir));
   }

	if(ret > 0) ret = 0;
	return ret;
}
#endif /* HPUX etc */


#if defined (WIN32)

static int FTWDEPTH = 0;        /* recursion depth */
static char curdir[_MAX_PATH];	/* saved current dir */

int ftw(
	char	*startdir,		/* absolute path of start directory */
							/* the function to be called for each file 	*/
	int		(*func)(char*, char*, struct _finddata_t*, long, long, long),
	int		depth,			/* directory depth (0 = current dir only)	*/
	long	arg1,			/* additional args1..3 for func()				*/
	long	arg2,
	long	arg3
)
{
   struct _finddata_t fd;
   long hdl, ret;
   char	buf[_MAX_PATH];

   /* init: save current dir */
   if (FTWDEPTH++ == 0) {
	  if (_getcwd(curdir,_MAX_PATH) == NULL) {			/* save curr dir */
         ret = -1;
	     goto abort;
	  }
   }      

   /* cd startdir */
   if (_chdir(startdir) == -1) {
      ret = -1;
	  goto abort;
   }

   DBG(printf("[%02d] cd -> %s\n", FTWDEPTH, startdir));

   /* process .. */
   if ((hdl = _findfirst("*.*",&fd)) == -1) {   
      ret = -1;
	  goto abort;
   }

   do {
	  /*DBG(printf("[%02d]       %s, %s\n", FTWDEPTH, startdir, fd.name));*/

      /* skip if "." or ".." */
      if (strcmp(fd.name,".") && strcmp(fd.name,"..")) {
	     /* exec callback .. */
		 if ((ret = (*func)(startdir,fd.name,&fd,arg1,arg2,arg3)) < 0) {
            ret = -1;
			goto abort;
		 }

		 /* subdir ? */
         if ((fd.attrib & _A_SUBDIR)  &&  FTWDEPTH < depth) {
            /* create whole pathname */
		    if (make_path(buf,startdir,fd.name,_MAX_PATH) == NULL) {
               ret = -1;
               goto abort;
			}

            /* recursive call to nested dirs */
			if ((ret = ftw(buf,func,depth,arg1,arg2,arg3)) < 0) {
                ret = -1;
				goto abort;
			}
		 }
	  }
   } while (_findnext(hdl, &fd) == 0);	/* while files found */

   abort:

   /* cleanup */
   _findclose(hdl);

   /* restore startdir */
   if (_chdir(startdir) == -1) {
      ret = -1;
	  goto abort;
   }

   DBG(printf("[%02d] cd <- %s\n", FTWDEPTH, startdir));

   /* term: restore current dir */
   if (--FTWDEPTH == 0) {
      if (_chdir(curdir) == -1)
         return(-1);  

	  DBG(printf("[%02d] cd <- %s\n", FTWDEPTH, curdir));
   }

   return(ret);
}

#endif /* WIN32 */


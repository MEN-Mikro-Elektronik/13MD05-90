
#include <MEN/typdefs.h>

#ifndef _HWBUG_H_
#define _HWBUG_H_

#if OS9
#undef NULL
#include <setjmp.h>
#include <setsys.h>
#include <stdio.h>
#include <ctype.h>
#define FLUSH fflush(stdout)
#endif


#if defined (LYNX) || defined (HPRT) || defined (LINUX)

#undef NULL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define FLUSH fflush(stdout)
#endif

#ifdef LINUX

extern void os_exit(void);
extern int os_get_char(void);
extern void hist_init(void);
extern char *hist_prev(void);
extern char *hist_next(void);
extern void hist_save( char *command );
extern int change_data( int argc, char **argv);
extern int fill_data( int argc, char **argv);
extern int display_data( int argc, char **argv);
extern  void os_usage(int argc, char **argv);
extern int32 os_init_io_mapped(void);
extern void os_init(void);
extern u_int32 os_access_address( );
#endif

int make_hex( );

extern u_int8 G_iomapped;  /**< flag for performing io-mapped access */

#endif /* _HWBUG_H_ */

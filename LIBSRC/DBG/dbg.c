/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: dbg.c
 *      Project: DBG library
 *
 *       Author: kp
 *        $Date: 2006/09/27 18:51:21 $
 *    $Revision: 1.6 $
 *
 *  Description: Debug functions for Linux
 *				 Main task is done by the macros in dbg_os.h. This
 *				 file exports only the DBG_MemDump routine and Init/Exit
 *
 *				 If MDIS_RTAI_SUPPORT was set in kernel config, the dbg module
 *				 uses rt_printk instead of printk
 *     Required: -
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: dbg.c,v $
 * Revision 1.6  2006/09/27 18:51:21  ts
 * use correct printk Variant in Realtime Linux
 *
 * Revision 1.5  2005/07/07 14:54:06  cs
 * Copyright line changed
 *
 * Revision 1.4  2004/06/09 09:15:51  kp
 * include sysdep.h
 *
 * Revision 1.3  2003/04/11 16:08:03  kp
 * changed CONFIG_RTHAL switch against MDIS_RTAI_SUPPORT
 *
 * Revision 1.2  2003/02/21 11:15:46  kp
 * use rt_printk if CONFIG_RTHAL is defined
 *
 * Revision 1.1  2001/01/19 14:38:27  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2001 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

const char DBG_RCSid[]="$Id: dbg.c,v 1.6 2006/09/27 18:51:21 ts Exp $";


#define __NO_VERSION__
#define EXPORT_SYMTAB
#include <linux/kernel.h>
#include <linux/module.h>

#include <MEN/men_typs.h>
/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#ifdef MDIS_RTAI_SUPPORT
# include <MEN/men_mdis_rt.h>
#endif

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef void DBG_HANDLE;

/* dummy definition for dbg.h */
#define DBG_MYLEVEL     null
/* say: don't declare DBG_HANDLE */
#define _NO_DBG_HANDLE
/* DBG_HANDLE needs to be declared BEFORE including dbg.h */
#include <MEN/dbg.h>

/*--- symbols exported by DBG module ---*/
EXPORT_SYMBOL(DBG_Init);
EXPORT_SYMBOL(DBG_Exit);
EXPORT_SYMBOL(DBG_Memdump);              

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
/* none */


/****************************** DBG_Init ************************************
 *
 *  Description:  Initializes the debug output, allocates and returns debug
 *                handle.
 *				  Does nothing on Linux	
 *---------------------------------------------------------------------------
 *  Input......:  name    name of debug output data module or NULL
 *				  dbgP    pointer to variable where debug handle will stored
 *
 *  Output.....:  *dbgP   debug handle
 *                return  success (0) or error code
 *  Globals....:  -
 ****************************************************************************/
int32 DBG_Init( char *name, DBG_HANDLE **dbgP )
{
    *dbgP = (DBG_HANDLE *)1;
    return(0);
}/*DBG_Init*/

/****************************** DBG_Exit ************************************
 *
 *  Description:  Terminate the debug output
 *
 *---------------------------------------------------------------------------
 *  Input......:  dbgP    pointer to variable where debug handle is stored
 *  Output.....:  return  0
 *  Globals....:  -
 ****************************************************************************/
int32  DBG_Exit( DBG_HANDLE **dbgP )
{
    *dbgP = NULL;
    return( 0 );
}/*DBG_Exit*/


/*************************** DBG_Memdump ************************************
 *
 *  Description:  Print a formatted hex dump
 *
 *                Trailing blanks in the description string define the
 *                indention of all following debug lines.
 *
 *---------------------------------------------------------------------------
 *  Input......:  txt     description string or NULL (first line)
 *                buf     memory to dump
 *                len     nr of bytes
 *                fmt     dump format [bytes]
 *                        1=byte, 2=word, 4=long, 8=64bit
 *  Output.....:  return  0
 *  Globals....:  -
 ****************************************************************************/
int32  DBG_Memdump
(
   DBG_HANDLE *dbg,
   char    *txt,
   void    *buf,
   u_int32 len,
   u_int32 fmt
)
{
    long i,line=1, blanks=0, n;
    char *k=txt, *k0, *kmax=(char*)buf+len;

    if (txt) {
        while(*k++ == ' ')          /* count trailing blanks */
            blanks++;

        printk( __LDL "%s (%ld bytes)\n",txt,len);
    }

    for (k=k0=buf; k0<kmax; k0+=16,line++) {
        for (n=0; n<blanks; n++)    /* write trailing blanks */
            printk(" ");
   
        printk(__LDL "%08lx+%04x: ",(long)buf, (short)(k-(char*)buf));

        switch(fmt) {                                      /* dump hex: */
           case 8 : for (k=k0,i=0; i<16; i+=8, k+=8)       /* 64-bit aligned */
                       if (k<kmax)  printk("%08lx%08lx ",
                                              *(long*)k,*(long*)k+4);
                       else         printk("                 ");
                    break;
           case 4 : for (k=k0,i=0; i<16; i+=4, k+=4)       /* long aligned */
                       if (k<kmax)  printk("%08lx ",*(long*)k);
                       else         printk("         ");
                    break;
           case 2 : for (k=k0,i=0; i<16; i+=2, k+=2)       /* word aligned */
                       if (k<kmax)  printk("%04x ",*(short*)k & 0xffff);
                       else         printk("     ");
                    break;
           default: for (k=k0,i=0; i<16; i++, k++)         /* byte aligned */
                       if (k<kmax)  printk("%02x ",*k & 0xff);
                       else         printk("   ");
        }

        for (k=k0,i=0; i<16 && k<kmax; i++, k++)           /* dump ascii's: */
            if ( ((u_int8)*k)>=32 && ((u_int8)*k)<=127 )
               printk("%c", *k);
            else
               printk(".");

        printk("\n");
    }

    return(0);
}/*DBG_Memdump*/



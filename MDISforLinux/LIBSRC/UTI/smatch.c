/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: smatch.c
 *      Project: uti.l
 *
 *      $Author: GLeonhardt $
 *        $Date: 2008/09/15 12:49:18 $
 *    $Revision: 1.2 $
 *
 *  Description: compare strings including wildcards
 *				 s = string to compare
 *               t = string with wildcards       
 *               ( Derived from the minix source code, find.c )
 *       
 *     Required: - 
 *     Switches: - 
 * 
 *---------------------------[ Public Functions ]----------------------------
 *  
 *  
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: smatch.c,v $
 * Revision 1.2  2008/09/15 12:49:18  GLeonhardt
 * Avoid warnings
 *
 * Revision 1.1  1994/11/21 08:48:40  kp
 * Initial revision
 *
 * Revision 1.1  94/04/18  15:51:30  kp
 * Initial revision
 * 
 *---------------------------------------------------------------------------
 * (c) Copyright 1992 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static char RCSid[]="$Header: /dd2/CVSR/COM/LIBSRC/UTI/smatch.c,v 1.2 2008/09/15 12:49:18 GLeonhardt Exp $";

/*################### SMATCH #########################*/
/* Don't try to understand the following one... */
int smatch(s, t)    /* shell-like matching */
        char *s, *t;
{
        /* include ident string, suppress warning */
        char a = a = RCSid[0];
        register int n;

        if (*t == '\0')
                return *s == '\0';
        if (*t == '*') {
                ++t;
                do
                        if (smatch(s,t))
                                return 1;
                while (*s++ != '\0');
                return 0;
        }
        if (*s == '\0') 
                return 0;
        if (*t == '\\')
                return (*s == *++t) ? smatch(++s, ++t) : 0;
        if (*t == '?')
                return smatch(++s, ++t);
        if (*t == '[') {
                while (*++t != ']') {
                        if (*t == '\\')
                                ++t;
                        if (*(t+1) != '-')
                                if (*t == *s) {
                                        while (*++t != ']')
                                                if (*t == '\\')
                                                        ++t;
                                        return smatch(++s, ++t);
                                }
                                else
                                        continue;
                        if (*(t+2) == ']')
                                return (*s == *t || *s == '-');
                        n =  (*(t+2) == '\\') ? 3 : 2;
                        if (*s >= *t && *s <= *(t+n)) {
                                while (*++t != ']')
                                        if (*t == '\\')
                                                ++t;
                                return smatch(++s, ++t);
                        }
                        t += n;
                }
                return 0;
        }
        return  (*s == *t) ? smatch(++s, ++t) : 0;
}



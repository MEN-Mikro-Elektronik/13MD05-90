/*********************  P r o g r a m  -  M o d u l e ***********************
 *  
 *         Name: smatch.c
 *      Project: uti.l
 *
 *      Author: GLeonhardt 
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
 *---------------------------------------------------------------------------
 * Copyright (c) 1992-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
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

/*################### SMATCH #########################*/
/* Don't try to understand the following one... */
int smatch(s, t)    /* shell-like matching */
        char *s, *t;
{
        /* include ident string, suppress warning */
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
                        if (*(t+1) != '-') {
                                if (*t == *s) {
                                        while (*++t != ']')
                                                if (*t == '\\')
                                                        ++t;
                                        return smatch(++s, ++t);
                                }
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



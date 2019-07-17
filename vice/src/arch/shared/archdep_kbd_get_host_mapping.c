/** \file   archdep_kbd_get_host_mapping.c
 * \brief   guess the hosts keyboard layout
 * \author  groepaz <groepaz@gmx.net>
 */

/*
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"
#include <stdio.h>
#include <stdlib.h>

#include "keyboard.h"

#include "archdep_kbd_get_host_mapping.h"

#if defined(WIN32_COMPILE)
/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a black (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages, constants are defined in winnt.h

   https://msdn.microsoft.com/en-us/library/windows/desktop/dd318693%28v=vs.85%29.aspx
*/
int archdep_kbd_get_host_mapping(void)
{
    uintptr_t lang;
    int n;
    int maps[KBD_MAPPING_NUM] = {
        KBD_MAPPING_US, KBD_MAPPING_UK, KBD_MAPPING_DE, KBD_MAPPING_DA,
        KBD_MAPPING_NO, KBD_MAPPING_FI, KBD_MAPPING_IT, KBD_MAPPING_NL };
    int langids[KBD_MAPPING_NUM] = {
        MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_US),
        MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_UK),
        MAKELANGID(LANG_GERMAN,     SUBLANG_GERMAN),
        MAKELANGID(LANG_DANISH,     SUBLANG_DANISH_DENMARK),
        MAKELANGID(LANG_NORWEGIAN,  SUBLANG_NORWEGIAN_BOKMAL),
        MAKELANGID(LANG_FINNISH,    SUBLANG_FINNISH_FINLAND),
        MAKELANGID(LANG_ITALIAN,    SUBLANG_ITALIAN),
        MAKELANGID(LANG_DUTCH,      SUBLANG_DUTCH)
    };

    /* GetKeyboardLayout returns a pointer, but the first 16 bits of it return
     * a 'language identfier', whatever that is. This is a bit weird. */
    lang = (uintptr_t)(void *)GetKeyboardLayout(0);

    /* try full match first */
    lang &= 0xffff; /* lower 16 bit contain the language id */
    for (n = 0; n < KBD_MAPPING_NUM; n++) {
        if (lang == langids[n]) {
            return maps[n];
        }
    }
    /* try only primary language */
    lang &= 0x3ff; /* lower 10 bit contain the primary language id */
    for (n = 0; n < KBD_MAPPING_NUM; n++) {
        if (lang == (langids[n] & 0x3ff)) {
            return maps[n];
        }
    }
    return KBD_MAPPING_US;
}
#else

/* Amiga, Beos, OS2 */
#if defined(AMIGA_SUPPORT) || defined(BEOS_COMPILE) || defined(__OS2__)

/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a black (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages/actual detection
*/
int archdep_kbd_get_host_mapping(void)
{
    return KBD_MAPPING_US;
}

#else

#include <locale.h>
#include <string.h>

/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a blank (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages
*/

int archdep_kbd_get_host_mapping(void)
{
    int n;
    char *l;
    int maps[KBD_MAPPING_NUM] = {
        KBD_MAPPING_US, KBD_MAPPING_UK, KBD_MAPPING_DE, KBD_MAPPING_DA,
        KBD_MAPPING_NO, KBD_MAPPING_FI, KBD_MAPPING_IT, KBD_MAPPING_NL };
    char *s[KBD_MAPPING_NUM] = {
        "en_US", "en_UK", "de", "da", "no", "fi", "it", "nl" };
    /* setup the locale */
    setlocale(LC_ALL, "");
    l = setlocale(LC_ALL, NULL);
    if (l && (strlen(l) > 1)) {
        for (n = 1; n < KBD_MAPPING_NUM; n++) {
            if (strncmp(l, s[n], strlen(s[n])) == 0) {
                return maps[n];
            }
        }
    }
    return KBD_MAPPING_US;
}
#endif
#endif

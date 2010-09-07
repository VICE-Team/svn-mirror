/*
 * mon_fallback.c - The VICE built-in monitor.
 *
 * Written by
 *  Daniel Kahlin <daniel@kahlin.net>
 *
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

#include <ctype.h>

#include "mon_disassemble.h"
#include "mon_memory.h"
#include "monitor.h"
#include "montypes.h"


/* undefine to remove the experimental fallback parser. */
#define HAVE_FALLBACK_PARSER 1


static char *skip_ws(char *str)
{
    /* skip white space */
    while( *str && isspace(*str) ) {
        str++;
    }
    return str;
}


static char *get_hex(char *str, int n, int *val)
{
    int i;
    int v = 0;

    for (i = 0; i < n; i++) {
        char c;
        int t = 0;
        str = skip_ws(str);
        if (*str == 0) {
            break;
        }

        c = tolower(*str);
        if (c >= '0' && c <= '9') {
            t = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            t = c - 'a' + 10;
        }
        v <<= 4;
        v |= t;
        str++;
    }
    *val = v;
    return str;
}

static char *may_get_addr(char *str, MON_ADDR *a)
{
    int v;
    str = get_hex(str, 4, &v);
    *a = new_addr(e_default_space, v);
    return str;
}

static char *may_get_range(char *str, MON_ADDR *a1, MON_ADDR *a2)
{
    int v;

    *a1 = new_addr(e_invalid_space, 0);
    *a2 = new_addr(e_invalid_space, 0);

    str = get_hex(str, 4, &v);
    *a1 = new_addr(e_default_space, v);
    str = skip_ws(str);
    if (*str != 0) {
        str = get_hex(str, 4, &v);
        *a2 = new_addr(e_default_space, v);
    }
    return str;
}


#ifndef HAVE_FALLBACK_PARSER
int fallback_parse(char *str)
{
    return 1;
}
#endif

#ifdef HAVE_FALLBACK_PARSER
int fallback_parse(char *str)
{
    MON_ADDR a1,a2;
    str = skip_ws(str);
    if (*str == 0) {
        return 1;
    }

    switch ( tolower(*str++) ) {
    case 'd':
        str = may_get_range(str, &a1, &a2);
        mon_disassemble_lines(a1, a2);
        return 0;
    case 'g':
        str = may_get_addr(str, &a1);
        mon_jump(a1);
        return 0;
    case 'i':
        str = may_get_range(str, &a1, &a2);
        mon_memory_display(0, a1, a2, DF_PETSCII);
        return 0;
    case 'm':
        str = may_get_range(str, &a1, &a2);
        mon_memory_display(default_radix, a1, a2, DF_PETSCII);
        return 0;
    default:
        break;
    }

    return 1;
}
#endif


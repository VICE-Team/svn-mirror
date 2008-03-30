/*
 * lib.c - Library functions.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void *lib_malloc(size_t size)
{
    void *p = malloc(size);

#ifndef __OS2__
    if (p == NULL && size > 0)
        exit(-1);
#endif

    return p;
}

/* Like calloc, but abort if not enough memory is available.  */
void *lib_calloc(size_t nmemb, size_t size)
{
    void *p = calloc(nmemb, size);

#ifndef __OS2__
    if (p == NULL && (size * nmemb) > 0)
        exit(-1);
#endif

    return p;
}

/* Like realloc, but abort if not enough memory is available.  */
void *lib_realloc(void *p, size_t size)
{
    void *new_p = realloc(p, size);

#ifndef __OS2__
    if (new_p == NULL)
        exit(-1);
#endif

    return new_p;
}

/* Malloc enough space for `str', copy `str' into it and return its
   address.  */
char *lib_stralloc(const char *str)
{
    size_t length;
    char *p;

    if (str == NULL)
        exit(-1);

    length = strlen(str) + 1;
    p = (char *)lib_malloc(length);

    memcpy(p, str, length);
    return p;
}


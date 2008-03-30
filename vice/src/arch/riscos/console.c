/*
 * console.c - Console access interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "console.h"
#include "utils.h"


static FILE *mon_input, *mon_output;


int console_init(void)
{
    return 0;
}

console_t *console_open(const char *id)
{
    console_t *console;

    mon_input = stdin; mon_output = stdout;

    console = xmalloc(sizeof(console_t));

    console->console_xres = 80;
    console->console_yres = 25;
    console->console_can_stay_open = 0;

    return console;
}

int console_close(console_t *log)
{
    free(log);

    return 0;
}

int console_out(console_t *log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stdout, format, ap);

    return 0;
}

char *console_in(console_t *log)
{
    char *p = (char*)xmalloc(1024);

    fflush(mon_output);
    fgets(p, 1024, mon_input);

    /* Remove trailing newlines.  */
    {
        int len;

        for (len = strlen(p);
             len > 0 && (p[len - 1] == '\r'
                         || p[len - 1] == '\n');
             len--)
            p[len - 1] = '\0';
    }

    return p;
}


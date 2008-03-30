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

#include <conio.h>
#include <stdarg.h>
#include <stdio.h>

#include "console.h"
#include "utils.h"
#include "video.h"

static int old_input_mode, old_output_mode;
static FILE *mon_output, *mon_input;

console_t console_open(const char *id)
{
    enable_text();
    clrscr();
    _set_screen_lines(43);
    _setcursortype(_SOLIDCURSOR);

    old_input_mode = setmode(STDIN_FILENO, O_TEXT);
    old_output_mode = setmode(STDOUT_FILENO, O_TEXT);

    mon_output = fopen("CON", "wt");
    mon_input = fopen("CON", "rt");
    setbuf(mon_output, NULL); /* No buffering.  */

    return 0;
}

int console_close(console_t log)
{
    setmode(STDIN_FILENO, old_input_mode);
    setmode(STDIN_FILENO, old_output_mode);

    disable_text();

    fclose(mon_input);
    fclose(mon_output);

    return 0;
}

int console_out(console_t log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(mon_output, format, ap);

    return 0;
}

char *console_in(console_t log)
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


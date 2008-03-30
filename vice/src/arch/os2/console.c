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

#define INCL_DOS_PROCESS
#include "vice.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

#include "utils.h"
#include "dialogs.h"
#include "console.h"

HWND hwndMonitor=NULLHANDLE;

console_t *console_open(const char *id)
{
    console_t *console;

    console = xmalloc(sizeof(console_t));

    log_debug("console_open");

    console->console_xres = 80;
    console->console_yres = 25;
    console->console_can_stay_open = 0;

    return console;
}

int console_close(console_t *log)
{
    if (!hwndMonitor) return 0;
    WinSendMsg(hwndMonitor, WM_CLOSE, 0, 0);
    hwndMonitor=NULLHANDLE;
    log_debug("console_close");

    free(log);

    return 0;
}

int console_out(console_t *log, const char *format, ...)
{
    char text[1024];
    char *c;
    static char out[1024];
    char *d;

    va_list ap;

    if (!hwndMonitor) return 0;

    va_start(ap, format);
    vsprintf(text, format, ap);

    if (strlen(text)+strlen(out)>1023)
    {
        out[0]='\0';
        return 0;
    }

    strcat(out, text);

    if (out[strlen(out)-1]=='\n')
    {
        out[strlen(out)-1]='\0';
        WinSendMsg(hwndMonitor, WM_INSERT, out, NULL);
        log_debug(out);
        out[0]='\0';
    }

    return 0;
}

char *console_in(console_t *log)
{
    char *c = xmalloc(1024);
    int wait_for_input = TRUE;

    *c = '\0';

    console_out(log, "\n");
    WinSendMsg(hwndMonitor, WM_INPUT, c, &wait_for_input);

    while (wait_for_input) DosSleep(1);

    log_debug("console_in %s", c);

    if (!strcmp(c,"quit")) strcpy(c,"exit");

    return c;
}


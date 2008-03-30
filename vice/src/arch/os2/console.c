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

#define INCL_DOSPROCESS // DosSleep
#include <os2.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

#include "utils.h"
#include "dialogs.h"
#include "console.h"

extern HWND hwndMonitor;

int console_init(void)
{
    return 0;
}

console_t *console_open(const char *id)
{
    console_t *console = xmalloc(sizeof(console_t));
    console->console_xres = 80;
    console->console_yres = 25;
    console->console_can_stay_open = 0;
    WinShowDlg(HWND_DESKTOP, DLG_MONITOR, 1);
    // WinEnableControl(hwndMonitor, EF_MONIN, 1);
    // WinShowDlg(hwndMonitor, EF_MONIN, 1);
    return console;
}

int console_close(console_t *log)
{
    WinShowDlg(HWND_DESKTOP, DLG_MONITOR, 0);
    // WinEnableControl(hwndMonitor, EF_MONIN, 0);
    // WinShowDlg(hwndMonitor, EF_MONIN, 0);
    free(log);
    return 0;
}

int console_out(console_t *log, const char *format, ...)
{
    static char out[1024];
    char in[1024];  // FIXME: what happens if strlen>1024?
    char *txt, *mid;
    va_list ap;

    if (!hwndMonitor)
        return 0;

    va_start(ap, format);
    vsprintf(in, format, ap);

    /*
     if (strlen(in)+strlen(out)>1023)
     {
     out[0]='\0';
     return 0;
     }
     */

    txt=in;
    while (strrchr(txt,'\n') && strchr(txt,'\n') != txt+strlen(txt))
    {
        mid=strchr(txt,'\n')+1;
        *strchr(txt,'\n')='\0';

        strcat(out, txt);

        WinSendMsg(hwndMonitor, WM_INSERT, out, NULL);
        out[0]='\0';

        while (*mid=='\n') mid++;
        txt = mid;
    }

    if (mid != txt)
        strcat(out, txt);

    return 0;
}

extern int trigger_shutdown;
extern int trigger_console_exit;

char *console_in(console_t *log)
{
    char *c=NULL;
    int wait_for_input = TRUE;

    console_out(log, "\n");
    WinSendMsg(hwndMonitor, WM_INPUT, &c, &wait_for_input);

    while (wait_for_input && !trigger_shutdown && !trigger_console_exit)
        DosSleep(1);

    if (trigger_shutdown || trigger_console_exit)
        c=stralloc("exit");

    WinSendMsg(hwndMonitor, WM_PROMPT, c, NULL);

    return c;
}

int console_close_all(void)
{
    WinSendMsg(hwndMonitor, WM_CLOSE, 0, 0);
    hwndMonitor=NULLHANDLE;
    return 0;
}


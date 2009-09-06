/*
 * uimon.c - Monitor access interface.
 *
 * Written by
 *  Spiro Trikaliotis <Spiro.Trikaliotis@gmx.de>
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

#define INCL_DOSPROCESS     // DosSleep

#include <os2.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dialogs.h"
#include "dlg-monitor.h"
#include "lib.h"
#include "uimon.h"
#include "util.h"
#include "console.h"

#include "log.h"

extern int trigger_shutdown;
extern int trigger_console_exit;

//
// int console_init() --> dlg-monitor.c
//

console_t *console;

void uimon_window_close()
{
    WinSendMsg(hwndMonitor, WM_CONSOLE, kCLOSE, 0);

    //
    // Here the FTE Problem comes from!
    // WinSetDlgFocus(HWND_DESKTOP, IDM_VICE2);
    //

    lib_free(console);
    console = NULL;
}

void uimon_notify_change()
{
    WinSendMsg(hwndMonitor, WM_CONSOLE, kOPEN, 0);
    WinSendMsg(hwndMonitor, WM_UPDATE, 0, 0);
}

console_t *uimon_window_open()
{
    console = lib_malloc(sizeof(console_t));

    //
    // FIXME: THIS VALUE ISN'T UPDATED YET
    //
    console->console_xres = 60;
    console->console_yres = 20;
    console->console_can_stay_open = 1;

    return console;
}

void uimon_window_suspend()
{
    WinEnableControl(hwndMonitor, EF_MONIN, 0);
}

console_t *uimon_window_resume()
{
    WinEnableControl(hwndMonitor, EF_MONIN, 1);
    return console;
}

int uimon_out(const char *buffer)
{
    //
    // Split output into single lines. If a line doesn't end with
    // a EOL we have to store it until it is flushed (endl, flush)
    //
    static char *out = NULL;
    int flag = FALSE;
    char *txt, *tmp, *eol;

    if (!hwndMonitor || !console) {
        return 0;
    }

    if (!out) {
        out = lib_calloc(1, 1);
    }

    txt = (char *)buffer;

    eol = strchr(txt, '\n');
    while (strrchr(txt,'\n') && eol != txt + strlen(txt)) {
        *eol = '\0';

        tmp = util_concat(out, txt, NULL);
        WinSendMsg(hwndMonitor, WM_INSERT, tmp, NULL);
        lib_free(tmp);

        out[0] = '\0';

        eol++;
        while (*eol == '\n') {
            eol++;
        }
        txt = eol;

        eol = strchr(txt, '\n');

        flag = TRUE;
    }

    if (!flag) {
        char *line = util_concat(out, txt, NULL);
        lib_free(out);
        out = line;
    }

    return 0;
}

char *uimon_in(const char *prompt)
{
    char *c = NULL;
    int wait_for_input = TRUE;

    uimon_out(prompt);
    uimon_out("\n");
    WinSendMsg(hwndMonitor, WM_INPUT, &c, &wait_for_input);

    while (wait_for_input && !trigger_shutdown && !trigger_console_exit) {
        DosSleep(1);
    }

    if (trigger_shutdown || trigger_console_exit) {
        c = lib_stralloc("exit");
    }

    trigger_console_exit = FALSE;

    WinSendMsg(hwndMonitor, WM_PROMPT, c, NULL);

    return c;
}

void uimon_set_interface(struct monitor_interface_s **monitor_interface_init, int count)
{
    log_debug("Interfaces: %d", count);
}

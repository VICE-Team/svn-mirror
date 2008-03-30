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
#include "mon.h"
#include "uimon.h"
#include "utils.h"
//#include "snippets\\pmwin2.h"   // WinShowDlg

#include "log.h"

extern int trigger_shutdown;
extern int trigger_console_exit;

//
// int console_init() --> dlg-monitor.c
//

void uimon_window_close()
{
    WinSendMsg(hwndMonitor, WM_CONSOLE, kCLOSE, 0);

    //
    // Here the FTE Problem comes from!
    // WinSetDlgFocus(HWND_DESKTOP, IDM_VICE2);
    //

    free(console_log);
    console_log = NULL;
}

void uimon_notify_change()
{
    WinSendMsg(hwndMonitor, WM_CONSOLE, kOPEN, 0);
    WinSendMsg(hwndMonitor, WM_UPDATE, 0, 0);
}

console_t *uimon_window_open()
{
    console_t *console_log = xmalloc(sizeof(console_t));

    //
    // FIXME: THIS VALUE ISN'T UPDATED YET
    //
    console_log->console_xres = 60;
    console_log->console_yres = 20;
    console_log->console_can_stay_open = 1;

    return console_log;
}

void uimon_window_suspend()
{
    WinEnableControl(hwndMonitor, EF_MONIN, 0);
}

console_t *uimon_window_resume()
{
    WinEnableControl(hwndMonitor, EF_MONIN, 1);
    return console_log;
}

int uimon_out(const char *format, ...)
{
    //
    // Split output into single lines. If a line doesn't end with
    // a EOL we have to store it until it is flushed (endl, flush)
    //
    static char *out = NULL;

    int flag = FALSE;

    char *in;
    char *txt, *tmp, *eol;

    va_list ap;

    if (!hwndMonitor || !console_log)
        return 0;

    if (!out) out = xcalloc(1,1);

    va_start(ap, format);
    in=xmvsprintf(format, ap);

    txt=in;

    eol = strchr(txt, '\n');
    while (strrchr(txt,'\n') && eol!= txt+strlen(txt))
    {
        *eol='\0';

        tmp = concat(out, txt, NULL);
        WinSendMsg(hwndMonitor, WM_INSERT, tmp, NULL);
        free(tmp);

        out[0]='\0';

        eol++;
        while (*eol=='\n') eol++;
        txt = eol;

        eol = strchr(txt, '\n');

        flag = TRUE;
    }

    if (!flag)
    {
        char *line = concat(out, txt, NULL);
        free(out);
        out = line;
    }

    free(in);

    return 0;
}

char *uimon_in()
{
    char *c=NULL;
    int wait_for_input = TRUE;

    uimon_out("\n");
    WinSendMsg(hwndMonitor, WM_INPUT, &c, &wait_for_input);

    while (wait_for_input && !trigger_shutdown && !trigger_console_exit)
        DosSleep(1);

    if (trigger_shutdown || trigger_console_exit)
        c=stralloc("exit");

    trigger_console_exit = FALSE;

    WinSendMsg(hwndMonitor, WM_PROMPT, c, NULL);

    return c;
}

void uimon_set_interface(monitor_interface_t **monitor_interface_init,
                         int count)
{
    log_debug("Interfaces: %d", count);
}


/*
 * console.c - Console access interface.
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textwin.h"

#include "console.h"
#include "ui.h"
#include "uimsgwin.h"
#include "utils.h"
#include "videoarch.h"


static FILE *mon_input, *mon_output;

static int WimpCmdBlock[64];

static int EscapePending;



int console_init(void)
{
    return 0;
}

console_t *console_open(const char *id)
{
    console_t *console;

    EscapePending = 0;

    mon_input = stdin; mon_output = stdout;

    console = xmalloc(sizeof(console_t));

    console->console_xres = 80;
    console->console_yres = 25;
    console->console_can_stay_open = 0;

    if (FullScreenMode == 1)
    {
      Wimp_CommandWindow((int)"Vice Monitor");
    }
    else
    {
      ui_message_window_open(msg_win_monitor, "Vice Monitor", "WIMPLIB LINE EDITOR", console->console_xres, console->console_yres);
      ui_message_window_busy(msg_win_monitor, 1);
    }

    return console;
}

int console_close(console_t *log)
{
    if (!ui_message_window_is_open(msg_win_monitor))
    {
      Wimp_CommandWindow(-1);
    }
    else
    {
      ui_message_window_busy(msg_win_monitor, 0);
      ui_message_window_close(msg_win_monitor);
    }

    free(log);

    return 0;
}

int console_out(console_t *log, const char *format, ...)
{
    va_list ap;
    int status = 0;

    if (EscapePending != 0)
    {
      EscapePending = 0;
      status = -1;
    }

    va_start(ap, format);
    if (!ui_message_window_is_open(msg_win_monitor))
    {
      vfprintf(stdout, format, ap);
    }
    else
    {
      text_window_t *tw = ui_message_get_text_window(msg_win_monitor);

      if (tw != NULL)
      {
        static char buffer[1024];

        vsprintf(buffer, format, ap);
        if (textwin_add_text(tw, buffer) > 0)
        {
          textwin_caret_to_end(tw);
          while (ui_poll_core(WimpCmdBlock) != 0) ;
        }
      }
    }
    return status;
}

char *console_in(console_t *log, const char *prompt)
{
    char *p;

    EscapePending = 0;

    console_out(log, "%s", prompt);

    if (!ui_message_window_is_open(msg_win_monitor))
    {
      int len;

      p = (char*)xmalloc(1024);

      fflush(mon_output);
      fgets(p, 1024, mon_input);

      /* Remove trailing newlines.  */
      for (len = strlen(p);
           len > 0 && (p[len - 1] == '\r' || p[len - 1] == '\n');
           len--)
          p[len - 1] = '\0';
    }
    else
    {
      const char *cmd;

      ui_message_window_busy(msg_win_monitor, 0);
      do
      {
        ui_poll_core(WimpCmdBlock);
        cmd = ui_message_window_get_last_command(msg_win_monitor);
      }
      while (cmd == NULL);
      p = (char*)xmalloc(wimp_strlen(cmd) + 1);
      wimp_strcpy(p, cmd);
      ui_message_window_busy(msg_win_monitor, 1);
    }

    return p;
}

void console_raise_escape(void)
{
    EscapePending = 1;
}

int console_close_all(void)
{
    return 0;
}


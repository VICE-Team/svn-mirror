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
#include <string.h>

#include "textwin.h"

#include "console.h"
#include "ui.h"
#include "utils.h"
#include "video.h"


static FILE *mon_input, *mon_output;

static int TestStringWidth = -1;

static int WimpCmdBlock[64];


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

    if (FullScreenMode == 1)
    {
      Wimp_CommandWindow((int)"Vice Monitor");
    }
    else
    {
      int textwidth;

      if (TestStringWidth < 0)
      {
        char strdummy[101];

        memset(strdummy, '0', 100);
        strdummy[100] = '\0';
        TestStringWidth = textwin_font_string_width(MonWinDescPtr, strdummy);
      }
      textwidth = (console->console_xres * TestStringWidth) / 100;
      MonWinDescPtr->MaxWidth = 3 * MonWinDescPtr->WindowBorder + textwidth;
      textwin_init(MonWinDescPtr, MonitorWindow, "WIMPLIB LINE EDITOR\n", NULL);
      textwin_open_centered(MonWinDescPtr, MonWinDescPtr->MaxWidth, 1024, ScreenMode.resx, ScreenMode.resy);
      textwin_set_caret(MonWinDescPtr, 0, 0);
      MonitorWindowOpen = 1;
    }

    return console;
}

int console_close(console_t *log)
{
    if (MonitorWindowOpen == 0)
    {
      Wimp_CommandWindow(-1);
    }
    else
    {
      textwin_free(MonWinDescPtr);
      MonitorWindowOpen = 0;
    }

    free(log);

    return 0;
}

int console_out(console_t *log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    if (MonitorWindowOpen == 0)
    {
      vfprintf(stdout, format, ap);
    }
    else
    {
      static char buffer[1024];

      vsprintf(buffer, format, ap);
      if (textwin_add_text(MonWinDescPtr, buffer) > 0)
      {
        textwin_caret_to_end(MonWinDescPtr);
        while (ui_poll_core(WimpCmdBlock) != 0) ;
      }
    }
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

int console_close_all(void)
{
    return 0;
}


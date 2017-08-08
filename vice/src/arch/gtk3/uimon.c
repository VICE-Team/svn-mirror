/*
 * uimon.c - Native GTK3 UI monitor stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "not_implemented.h"

#include "console.h"
#include "monitor.h"
#include "kbd.h"
#include "ui.h"
#include "uimon.h"
#include "videoarch.h"

#ifdef HAVE_VTE

char *uimon_get_in(char **ppchCommandLine, const char *prompt)
{
    NOT_IMPLEMENTED();
    return NULL;
}

void uimon_notify_change(void)
{
    NOT_IMPLEMENTED();
}

int uimon_out(const char *buffer)
{
    NOT_IMPLEMENTED();
    return 0;
}

void uimon_set_interface(monitor_interface_t **monitor_interface_init, int count)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void uimon_window_close(void)
{
    NOT_IMPLEMENTED();
}

static void window_destroy_cb(void)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

console_t *uimon_window_open(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

console_t *uimon_window_resume(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

#endif

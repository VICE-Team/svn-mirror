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

#include "uimon.h"

#include <stdio.h>
#include <stdarg.h>

#include "utils.h"
#include "console.h"

static console_t *console_log = NULL;

void arch_mon_window_close()
{
    console_close(console_log);
    console_log = NULL;
}


console_t *arch_mon_window_open()
{
    console_log = console_open("Monitor");
    return console_log;
}

void arch_mon_window_suspend()
{
    arch_mon_window_close();
}

console_t *arch_mon_window_resume()
{
    return arch_mon_window_open();
}

int arch_mon_out(const char *format, ...)
{
    va_list ap;
    char *buffer;

    if (!console_log)
        return 0;

    va_start(ap, format);
    return console_out(console_log, xmvsprintf(format, ap));
}

char *arch_mon_in()
{
    return console_in(console_log);
}

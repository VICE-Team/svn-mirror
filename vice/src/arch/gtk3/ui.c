/*
 * ui.c - Native GTK3 UI stuff.
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

#include "uiapi.h"

int ui_cmdline_options_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void ui_error(const char *format, ...)
{
    NOT_IMPLEMENTED();
}

char *ui_get_file(const char *format, ...)
{
    NOT_IMPLEMENTED();
    return NULL;
}

int ui_init(int *argc, char **argv)
{
    NOT_IMPLEMENTED();
    return 0;
}

int ui_init_finalize(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

int ui_init_finish(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    NOT_IMPLEMENTED();
    return UI_JAM_NONE;
}

int ui_resources_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void ui_resources_shutdown(void)
{
    NOT_IMPLEMENTED();
}

void ui_shutdown(void)
{
    NOT_IMPLEMENTED();
}

void ui_update_menus(void)
{
    NOT_IMPLEMENTED();
}

void ui_dispatch_events(void)
{
    NOT_IMPLEMENTED();
}


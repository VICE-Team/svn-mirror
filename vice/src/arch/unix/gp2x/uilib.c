/*
 * uilib.c
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

#include <stdio.h>
#include <string.h>

#include "lib.h"
#include "resources.h"
#include "uilib.h"
#include "uimenu.h"
#include "vsync.h"


void uilib_select_string(const char *resname, const char *title,
                         const char *name)
{
    ui_button_t button;
    char *value;
    char *new_value;
    int len;

    vsync_suspend_speed_eval();

    resources_get_value(resname, (void *)&value);

    if (value == NULL)
        value = "";

    len = strlen(value) * 2;
    if (len < 255)
        len = 255;

    new_value = lib_calloc(1, len + 1);
    strncpy(new_value, value, len);

    button = ui_input_string(title, name, new_value, len);

    if (button == UI_BUTTON_OK)
        resources_set_value(resname, (resource_value_t)new_value);

    lib_free(new_value);
}


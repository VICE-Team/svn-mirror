/*
 * mouse.c - Mouse handling for BeOS
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "vice.h"

#include <stdio.h>

#include "cmdline.h"
#include "keyboard.h"
#include "log.h"
#include "mouse.h"
#include "resources.h"
#include "types.h"
#include "ui.h"

int                 _mouse_enabled;
int                 _mouse_x, _mouse_y;
static int          mouse_acquired=0;

typedef struct mouse_data_t {
    DWORD   X;
    DWORD   Y;
    BYTE    LeftButton;
    BYTE    RightButton;
    BYTE    padding[2];
} mouse_data;

/* ------------------------------------------------------------------------- */

static int set_mouse_enabled(resource_value_t v, void *param)
{
    _mouse_enabled = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "Mouse", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &_mouse_enabled, set_mouse_enabled, NULL },
    { NULL }
};

int mouse_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-mouse", SET_RESOURCE, 1, NULL, NULL,
      "Mouse", NULL, NULL, "Enable emulation of the 1351 proportional mouse" },
    { "+mouse", SET_RESOURCE, 0, NULL, NULL,
      "Mouse", NULL, NULL, "Disable emulation of the 1351 proportional mouse" },
    { NULL }
};

int mouse_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void mouse_set_format(void)
{
}

void mouse_update_mouse(void)
{
}

void mouse_init(void)
{
}


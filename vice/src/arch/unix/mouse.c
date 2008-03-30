/*
 * mouse.c - Mouse handling for Unix-Systems.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
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

/* This is a first rough implementation of mouse emulation for MS-DOS.
   A smarter and less buggy emulation is of course possible. */

#include "vice.h"

#include <stdio.h>

#include "cmdline.h"
#include "keyboard.h"
#include "mouse.h"
#include "log.h"
#include "resources.h"
#include "ui.h"

int _mouse_enabled = 0;
int mouse_x, mouse_y;
int mouse_accelx, mouse_accely;

/* ------------------------------------------------------------------------- */

static int set_mouse_enabled(resource_value_t v, void *param)
{
    _mouse_enabled = (int) v;
    ui_check_mouse_cursor();
    return 0;
}

static resource_t resources[] = {
    { "Mouse", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &_mouse_enabled, set_mouse_enabled, NULL },
    { NULL }
};

int mouse_resources_init(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] = {
    { "-mouse", SET_RESOURCE, 1, NULL, NULL,
      "Mouse", NULL, NULL, N_("Enable emulation of the 1351 proportional mouse") },
    { "+mouse", SET_RESOURCE, 0, NULL, NULL,
      "Mouse", NULL, NULL, N_("Disable emulation of the 1351 proportional mouse") },
    { NULL }
};

int mouse_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int mouse_init(void)
{
    /* FIXME: Always successful?  */
    return 0;
}

/* ------------------------------------------------------------------------- */

void mouse_button(int bnumber, int state)
{
    if (bnumber == 1 && state)
        joystick_value[1] |= 16;
    else if (bnumber == 1)
        joystick_value[1] &= ~16;
    /*    if (bnumber == 2 && state)
        joystick_value[1] |= 1;
    else if (bnumber == 2)
    joystick_value[1] &= ~1;*/
}


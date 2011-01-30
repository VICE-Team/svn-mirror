/*
 * uirenderer.c
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "resources.h"
#include "ui.h"
#include "uiapi.h"
#include "uilib.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"

void ui_select_renderer(ui_window_t w, int check, int type, char *chip)
{
    char r1[0x20], r2[0x20];
    int renderer = 0, crt, have_scale2x, scale2x, doublesize;

    strcpy(r1, chip);
    strcpy(r2, chip);
    strcat(r1, "Scale2x");
    strcat(r2, "DoubleSize");

    resources_get_int("PalEmulation", &crt);

    if (resources_query_type(r1) == RES_INTEGER) {
        resources_get_int(r1, &scale2x);
        have_scale2x = 1;
    } else {
        scale2x = 0;
        have_scale2x = 0;
    }

    resources_get_int(r2, &doublesize);

    if (crt) {
        renderer = 1;
    } else if ((scale2x > 0) && (doublesize > 0)) {
        renderer = 2;
    }

    if (!check) {
        switch (type) {
            case 0: /* unfiltered */
                crt = 0;
                scale2x = 0;
                break;
            case 1: /* CRT */
                crt = 1;
                scale2x = 0;
                break;
            case 2: /* scale 2x */
                crt = 0;
                scale2x = 1;
                doublesize = 1;
                break;
        }

        resources_set_int("PalEmulation", crt);
        if (have_scale2x) {
            resources_set_int(r1, scale2x);
        }
        resources_set_int(r2, doublesize);
        ui_update_menus();
    } else {
        if (renderer == type) {
            ui_menu_set_tick(w, 1);
        } else {
            ui_menu_set_tick(w, 0);
        }
    }

}

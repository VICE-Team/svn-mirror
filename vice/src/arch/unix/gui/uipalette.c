/*
 * uipalette.c
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
#include <stdlib.h>

#include "lib.h"
#include "resources.h"
#include "ui.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"


UI_CALLBACK(ui_load_palette)
{
    char *filename, *title;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();
    title = lib_stralloc(_("Load custom palette"));
    filename = ui_select_file(title, NULL, 0, False, last_dir, "*.vpl",
                              &button, False, NULL);

    lib_free(title);
    switch (button) {
      case UI_BUTTON_OK:
        if (resources_set_value(UI_MENU_CB_PARAM,
                (resource_value_t)filename) < 0)
            ui_error(_("Could not load palette file\n'%s'"), filename);
        if (last_dir)
            lib_free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    ui_update_menus();
    if (filename != NULL)
        lib_free(filename);
}


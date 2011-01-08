/*
 * uipetdww.c - PET DWW UI interface for MS-DOS.
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

#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uipetdww.h"

TUI_MENU_DEFINE_TOGGLE(PETDWW)

static TUI_MENU_CALLBACK(petdww_image_file_callback)
{
    char s[256];
    const char *v;

    if (been_activated) {

        *s = '\0';

        if (tui_input_string("Change PET DWW image name", "New image name:", s, 255) == -1) {
            return NULL;
        }

        if (*s == '\0') {
            return NULL;
        }

        resources_set_string("PETDWWfilename", s);
    }

    resources_get_string("PETDWWfilename", &v);
    return v;
}

static tui_menu_item_def_t petdww_menu_items[] = {
    { "_Enable PET DWW:", "Emulate PET DWW",
      toggle_PETDWW_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "PET DWW _image file:", "Select the PET DWW image file",
      petdww_image_file_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uipetdww_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_petdww_submenu;

    ui_petdww_submenu = tui_menu_create("PET DWW settings", 1);

    tui_menu_add(ui_petdww_submenu, petdww_menu_items);

    tui_menu_add_submenu(parent_submenu, "PET _DWW settings...",
                         "PET DWW settings",
                         ui_petdww_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

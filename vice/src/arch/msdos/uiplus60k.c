/*
 * uiplus60k.c - +60K EXPANSION HACK UI interface for MS-DOS.
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
#include "uiplus60k.h"

TUI_MENU_DEFINE_TOGGLE(PLUS60K)

static TUI_MENU_CALLBACK(plus60k_image_file_callback)
{
    char s[256], *v;

    if (been_activated) {

        *s = '\0';

        if (tui_input_string("Change +60K image name",
                             "New image name:", s, 255) == -1)
            return NULL;

        if (*s == '\0')
            return NULL;

        resources_set_value("PLUS60Kfilename", (resource_value_t)s);
    }

    resources_get_value("PLUS60Kfilename", (void *)&v);
    return v;
}

static tui_menu_item_def_t plus60k_menu_items[] = {
    { "_Enable +60K:", "Emulate +60K RAM Expansion Hack",
      toggle_PLUS60K_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "+60K _image file:", "Select the +60K image file",
      plus60k_image_file_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uiplus60k_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_plus60k_submenu;

    ui_plus60k_submenu = tui_menu_create("+60K settings", 1);

    tui_menu_add(ui_plus60k_submenu, plus60k_menu_items);

    tui_menu_add_submenu(parent_submenu, "_60K settings...",
                         "+60K settings",
                         ui_plus60k_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

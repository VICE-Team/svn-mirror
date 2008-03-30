/*
 * uiplus256k.c - +256K EXPANSION UI interface for MS-DOS.
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
#include "uiplus256k.h"

TUI_MENU_DEFINE_TOGGLE(PLUS256K)

static TUI_MENU_CALLBACK(plus256k_image_file_callback)
{
    char s[256], *v;

    if (been_activated) {

        *s = '\0';

        if (tui_input_string("Change PLUS256K image name",
                             "New image name:", s, 255) == -1)
            return NULL;

        if (*s == '\0')
            return NULL;

        resources_set_value("PLUS256Kfilename", (resource_value_t)s);
    }

    resources_get_value("PLUS256Kfilename", (void *)&v);
    return v;
}

static tui_menu_item_def_t plus256k_menu_items[] = {
    { "_Enable PLUS256K:", "Emulate PLUS256K RAM Expansion",
      toggle_PLUS256K_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "PLUS256K _image file:", "Select the PLUS256K image file",
      plus256k_image_file_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uiplus256k_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_plus256k_submenu;

    ui_plus256k_submenu = tui_menu_create("PLUS256K settings", 1);

    tui_menu_add(ui_plus256k_submenu, plus256k_menu_items);

    tui_menu_add_submenu(parent_submenu, "PLUS_256K settings...",
                         "PLUS256K settings",
                         ui_plus256k_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

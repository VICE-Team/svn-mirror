/*
 * uic64_256k.c - 256K EXPANSION HACK UI interface for MS-DOS.
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
#include "uic64_256k.h"

TUI_MENU_DEFINE_TOGGLE(C64_256K)
TUI_MENU_DEFINE_RADIO(C64_256Kbase)

static TUI_MENU_CALLBACK(c64_256k_base_submenu_callback)
{
    int value;
    static char s[100];

    resources_get_value("C64_256Kbase", (void *)&value);
    sprintf(s, "%X-%X",value,value+0x7f);
    return s;
}

static tui_menu_item_def_t c64_256k_base_submenu[] = {
    { "$_DE00-$DE7F", NULL, radio_C64_256Kbase_callback,
      (void *)0xde00, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D_E80-$DEFF", NULL, radio_C64_256Kbase_callback,
      (void *)0xde80, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF_00-$DF7F", NULL, radio_C64_256Kbase_callback,
      (void *)0xdf00, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$D_F80-$DFFF", NULL, radio_C64_256Kbase_callback,
      (void *)0xdf80, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(c64_256k_image_file_callback)
{
    char s[256], *v;

    if (been_activated) {

        *s = '\0';

        if (tui_input_string("Change 256K image name",
                             "New image name:", s, 255) == -1)
            return NULL;

        if (*s == '\0')
            return NULL;

        resources_set_value("C64_256Kfilename", (resource_value_t)s);
    }

    resources_get_value("C64_256Kfilename", (void *)&v);
    return v;
}

static tui_menu_item_def_t c64_256k_menu_items[] = {
    { "_Enable 256K:", "Emulate 256K RAM Expansion",
      toggle_C64_256K_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "256K _base:", "Select the base of the 256K RAM Expansion",
      c64_256k_base_submenu_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, c64_256k_base_submenu,
      "256K base" },
    { "256K _image file:", "Select the 256K image file",
      c64_256k_image_file_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uic64_256k_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_c64_256k_submenu;

    ui_c64_256k_submenu = tui_menu_create("256K settings", 1);

    tui_menu_add(ui_c64_256k_submenu, c64_256k_menu_items);

    tui_menu_add_submenu(parent_submenu, "_256K settings...",
                         "256K settings",
                         ui_c64_256k_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

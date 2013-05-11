/*
 * uilightpen.c - Lightpen UI interface for MS-DOS.
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

#include "lightpen.h"
#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uilightpen.h"

TUI_MENU_DEFINE_TOGGLE(Lightpen)
TUI_MENU_DEFINE_RADIO(LightpenType)

static TUI_MENU_CALLBACK(lightpen_type_submenu_callback)
{
    int value;
    char *s;

    resources_get_int("LightpenType", &value);
    switch (value) {
        case LIGHTPEN_TYPE_PEN_U:
        default:
            s = "Pen with button Up";
            break;
        case LIGHTPEN_TYPE_PEN_L:
            s = "Pen with button Left";
            break;
        case LIGHTPEN_TYPE_PEN_DATEL:
            s = "Datel Pen";
            break;
        case LIGHTPEN_TYPE_GUN_Y:
            s = "Magnum Light Phaser";
            break;
        case LIGHTPEN_TYPE_GUN_L:
            s = "Stack Light Rifle";
            break;
        case LIGHTPEN_TYPE_INKWELL:
            s = "Inkwell Pen";
    }
    return s;
}

static tui_menu_item_def_t lightpen_type_submenu[] = {
    { "Pen with button Up", NULL, radio_LightpenType_callback,
      (void *)LIGHTPEN_TYPE_PEN_U, 20, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Pen with button Left", NULL, radio_LightpenType_callback,
      (void *)LIGHTPEN_TYPE_PEN_L, 20, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Datel Pen", NULL, radio_LightpenType_callback,
      (void *)LIGHTPEN_TYPE_PEN_DATEL, 20, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Magnum Light Phaser", NULL, radio_LightpenType_callback,
      (void *)LIGHTPEN_TYPE_GUN_Y, 20, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Stack Light Rifle", NULL, radio_LightpenType_callback,
      (void *)LIGHTPEN_TYPE_GUN_L, 20, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Inkwell Pen", NULL, radio_LightpenType_callback,
      (void *)LIGHTPEN_TYPE_INKWELL, 20, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t lightpen_menu_items[] = {
    { "_Enable Lightpen:", "Emulate Lightpen",
      toggle_Lightpen_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Lightpen _type:", "Select the lightpen type",
      lightpen_type_submenu_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, lightpen_type_submenu,
      "Lightpen type" },
    { NULL }
};

void uilightpen_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_lightpen_submenu;

    ui_lightpen_submenu = tui_menu_create("Lightpen settings", 1);

    tui_menu_add(ui_lightpen_submenu, lightpen_menu_items);

    tui_menu_add_submenu(parent_submenu, "_Lightpen settings...",
                         "Lightpen settings",
                         ui_lightpen_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

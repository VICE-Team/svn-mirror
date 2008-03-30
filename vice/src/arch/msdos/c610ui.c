/*
 * c610ui.c - Definition of the C610-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "c610ui.h"

#include "menudefs.h"
#include "sidui.h"
#include "tui.h"
#include "ui.h"

static struct {
    char *name;
    char *brief_description;
    const char *menu_item;
    char *long_description;
} palette_items[] = {
    { "default", "Default (green)", "_Default",
      "Default VICE C610 palette (Green)" },
    { "amber", "Amber", "_Amber",
      "Amber palette" },
    { "white", "White", "_White",
      "White palette" },
    { NULL }
};

static TUI_MENU_CALLBACK(palette_callback)
{
    if (been_activated) {
	if (resources_set_value("PaletteFile", (resource_value_t) param) < 0)
	   tui_error("Invalid palette file");
	ui_update_menus();
    }

    return NULL;
}

static TUI_MENU_CALLBACK(palette_menu_callback)
{
    char *s;
    int i;

    resources_get_value("PaletteFile", (resource_value_t *) &s);
    for (i = 0; palette_items[i].name != NULL; i++) {
	if (strcmp(s, palette_items[i].name) == 0)
	   return palette_items[i].brief_description;
    }

    return "Custom";
}

static void add_palette_submenu(tui_menu_t parent)
{
    int i;
    tui_menu_t palette_menu = tui_menu_create("Color Set", 1);

    for (i = 0; palette_items[i].name != NULL; i++)
	tui_menu_add_item(palette_menu,
			  palette_items[i].menu_item,
			  palette_items[i].long_description,
			  palette_callback,
			  (void *) palette_items[i].name, 0,
			  TUI_MENU_BEH_CLOSE);

    tui_menu_add_submenu(parent, "Color _Palette:",
			 "Choose color palette",
			 palette_menu,
			 palette_menu_callback,
			 NULL,
			 15);
}

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(DoubleScan)

static tui_menu_item_def_t video_menu_items[] = {
    { "--" },
    { "_Double Scan:",
      "Display double lines when running in 80-column mode",
      toggle_DoubleScan_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

int c610_ui_init(void)
{
    ui_create_main_menu(0, 0, 0, 0);

    tui_menu_add_separator(ui_video_submenu);
    tui_menu_add(ui_sound_submenu, sid_ui_menu_items);
    add_palette_submenu(ui_video_submenu);
    tui_menu_add(ui_video_submenu, video_menu_items);

    return 0;
}

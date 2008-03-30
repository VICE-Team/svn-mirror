/*
 * petui.c - Definition of the PET-specific part of the UI.
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

#include "petui.h"

#include "ui.h"
#include "tui.h"
#include "tuimenu.h"
#include "menudefs.h"

static struct {
    char *name;
    char *brief_description;
    char *long_description;
} palette_items[] = {
    { "default", "_Default",
      "Default VICE PET palette (Green)" },
    { "amber", "_Amber",
      "Amber palette" },
    { "white", "_White",
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
			  palette_items[i].brief_description,
			  palette_items[i].long_description,
			  palette_callback,
			  (void *) palette_items[i].name, 0,
			  TUI_MENU_BEH_CLOSE);

    tui_menu_add_submenu(parent, "Color _Palette:",
			 "Choose color palette",
			 palette_menu,
			 palette_menu_callback,
			 NULL,
			 10);
}

int pet_ui_init(void)
{
    ui_create_main_menu(0, 0, 0, 0);

    tui_menu_add_separator(ui_video_submenu);
    add_palette_submenu(ui_video_submenu);

    return 0;
}

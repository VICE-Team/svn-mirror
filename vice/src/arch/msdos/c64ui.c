/*
 * c64ui.c - Definition of the C64-specific part of the UI.
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

#include "c64ui.h"

#include "ui.h"
#include "tui.h"
#include "tuimenu.h"
#include "menudefs.h"

/* ------------------------------------------------------------------------- */

TUI_MENU_DEFINE_TOGGLE(CheckSsColl)
TUI_MENU_DEFINE_TOGGLE(CheckSbColl)

static tui_menu_item_def_t vic_ii_menu_items[] = {
    { "--" },
    { "Sprite-_Background Collisions:",
      "Emulate sprite-background collision register",
      toggle_CheckSbColl_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Sprite-_Sprite Collisions:",
      "Emulate sprite-sprite collision register",
      toggle_CheckSsColl_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

TUI_MENU_CALLBACK(toggle_REU_callback)
{
    int value;

    if (been_activated) {
        resources_toggle("REU", (resource_value_t *) &value);
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time.  */
	if (value)
            resources_set_value("IEEE488", (resource_value_t) 0);
	ui_update_menus();
    } else {
        resources_get_value("REU", (resource_value_t *) &value);
    }

    return value ? "On" : "Off";
}

TUI_MENU_DEFINE_TOGGLE(EmuID)

TUI_MENU_CALLBACK(toggle_IEEE488_callback)
{
    int value;

    if (been_activated) {
        resources_toggle("IEEE488", (resource_value_t *) &value);
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time.  */
	if (value)
            resources_set_value("REU", (resource_value_t) 0);
    } else {
        resources_get_value("IEEE488", (resource_value_t *) &value);
    }

    return value ? "On" : "Off";
}

static tui_menu_item_def_t special_menu_items[] = {
    { "--" },
    { "512K _RAM Expansion Unit (C1750):",
      "Emulate auxiliary 512K RAM Expansion Unit",
      toggle_REU_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_IEEE488 Interface Emulation:",
      "Emulate external IEEE488 interface",
      toggle_IEEE488_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Emulator Identification:",
      "Allow programs to identify the emulator they are running on",
      toggle_EmuID_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#if 0
    /* Some day...  */
    { "_Mouse emulation:",
      "Emulate a Commodore 1351 proportional mouse",
      toggle_mouse_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#endif
    { NULL }
};

/* ------------------------------------------------------------------------- */

TUI_MENU_CALLBACK(toggle_SidModel_callback)
{
    int value;

    if (been_activated) {
        resources_toggle("SidModel", (resource_value_t *) &value);
    } else {
        resources_get_value("SidModel", (resource_value_t *) &value);
    }

    return value ? "8580 (New)" : "6581 (Old)";
}

TUI_MENU_DEFINE_TOGGLE(SidFilters)

static tui_menu_item_def_t sid_menu_items[] = {
    { "--" },
    { "SID _Model:",
      "Select the SID model to emulate",
      toggle_SidModel_callback, NULL, 10,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "SID _Filters:",
      "Enable/disable emulation of the SID built-in programmable filters",
      toggle_SidFilters_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

/* ------------------------------------------------------------------------- */

static struct {
    const char *name;
    const char *brief_description;
    const char *menu_item;
    const char *long_description;
} palette_items[] = {
    { "default", "Default", "_Default",
      "Default VICE C64 palette" },
    { "ccs64", "CCS64", "_CCS64",
      "Palette from the CCS64 emulator by Per Hakan Sundell" },
    { "frodo", "Frodo", "_Frodo",
      "Palette from the Frodo emulator by Christian Bauer" },
    { "godot", "GoDot", "_GoDot",
      "Palette as suggested by the authors of the GoDot C64 graphics package" },
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
			 10);
}

/* ------------------------------------------------------------------------- */

int c64_ui_init(void)
{
    ui_create_main_menu(1, 1, 1);

    tui_menu_add_separator(ui_video_submenu);
    add_palette_submenu(ui_video_submenu);
    tui_menu_add(ui_video_submenu, vic_ii_menu_items);
    tui_menu_add(ui_sound_submenu, sid_menu_items);
    tui_menu_add(ui_special_submenu, special_menu_items);
    return 0;
}

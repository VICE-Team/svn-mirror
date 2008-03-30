/*
 * uidigimax.c - DIGIMAX cartridge UI interface for MS-DOS.
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
#include "uidigimax.h"

TUI_MENU_DEFINE_TOGGLE(DIGIMAX)
TUI_MENU_DEFINE_RADIO(DIGIMAXbase)

static TUI_MENU_CALLBACK(digimax_base_submenu_callback)
{
    int value;
    static char s[100];

    resources_get_int("DIGIMAXbase", &value);
    if (value==0xdd00)
      sprintf(s, "Userport");
    else
      sprintf(s, "$%X", value);
    return s;
}

static tui_menu_item_def_t digimax_base_submenu[] = {
    { "Userport", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdd00, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE00", NULL, radio_DIGIMAXbase_callback,
      (void *)0xde00, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE20", NULL, radio_DIGIMAXbase_callback,
      (void *)0xde20, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE40", NULL, radio_DIGIMAXbase_callback,
      (void *)0xde40, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE60", NULL, radio_DIGIMAXbase_callback,
      (void *)0xde60, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE80", NULL, radio_DIGIMAXbase_callback,
      (void *)0xde80, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEA0", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdea0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEC0", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdec0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DEE0", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdee0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF00", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdf00, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF20", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdf20, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF40", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdf40, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF60", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdf60, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DF80", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdf80, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFA0", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdfa0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFC0", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdfc0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DFE0", NULL, radio_DIGIMAXbase_callback,
      (void *)0xdfe0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t digimax_menu_items[] = {
    { "_Enable Digimax:", "Emulate Digimax Cartridge",
      toggle_DIGIMAX_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Digimax _base:", "Select the base of the Digimax Cartridge",
      digimax_base_submenu_callback, NULL, 11,
      TUI_MENU_BEH_CONTINUE, digimax_base_submenu,
      "Digimax base" },
    { NULL }
};

void uidigimax_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_digimax_submenu;

    ui_digimax_submenu = tui_menu_create("Digimax settings", 1);

    tui_menu_add(ui_digimax_submenu, digimax_menu_items);

    tui_menu_add_submenu(parent_submenu, "_Digimax settings...",
                         "Digimax settings",
                         ui_digimax_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

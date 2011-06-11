/*
 * uitfe.c - TFE and RRNET UI interface for MS-DOS.
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

#ifdef HAVE_TFE

#include <stdio.h>

#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uitfe.h"

TUI_MENU_DEFINE_TOGGLE(ETHERNET_ACTIVE)
TUI_MENU_DEFINE_TOGGLE(TFEIOSwap)
TUI_MENU_DEFINE_RADIO(ETHERNET_AS_RR)

static TUI_MENU_CALLBACK(ethernet_as_rr_submenu_callback)
{
    int value;
    char *s;

    resources_get_int("ETHERNET_AS_RR", &value);
    s = (value) ? "RRNET" : "TFE";
    return s;
}

static tui_menu_item_def_t ethernet_as_rr_submenu[] = {
    { "_TFE", NULL, radio_ETHERNET_AS_RR_callback,
      (void *)0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_RRNET", NULL, radio_ETHERNET_AS_RR_callback,
      (void *)1, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t tfe_c64_menu_items[] = {
    { "_Enable Ethernet:", "Emulate Ethernet Cartridge",
      toggle_ETHERNET_ACTIVE_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_TFE or RRNET:", "Select which type to use",
      ethernet_as_rr_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, ethernet_as_rr_submenu,
      "Ethernet type" },
    { NULL }
};

static tui_menu_item_def_t tfe_vic20_menu_items[] = {
    { "_Enable Ethernet:", "Emulate Ethernet Cartridge",
      toggle_ETHERNET_ACTIVE_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Enable MasC=uerade I/O swap:", "Map TFE I/O to vic20 I/O-3",
      toggle_TFEIOSwap_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uitfe_c64_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_tfe_submenu;

    ui_tfe_submenu = tui_menu_create("TFE settings", 1);

    tui_menu_add(ui_tfe_submenu, tfe_c64_menu_items);

    tui_menu_add_submenu(parent_submenu, "_TFE settings...",
                         "TFE settings",
                         ui_tfe_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

void uitfe_vic20_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_tfe_submenu;

    ui_tfe_submenu = tui_menu_create("TFE settings (MasC=uerade)", 1);

    tui_menu_add(ui_tfe_submenu, tfe_vic20_menu_items);

    tui_menu_add_submenu(parent_submenu, "_TFE settings (MasC=uerade)...",
                         "TFE settings",
                         ui_tfe_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

#endif

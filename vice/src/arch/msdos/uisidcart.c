/*
 * uisidcart.c - SID Cartridge UI interface for MS-DOS.
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
#include "sid.h"
#include "tui.h"
#include "tuimenu.h"
#include "uisidcart.h"

static char *sidcart_primary_address;
static char *sidcart_secondary_address;
static char *sidcart_clock;

TUI_MENU_DEFINE_TOGGLE(SidCart)
TUI_MENU_DEFINE_TOGGLE(SidFilters)
TUI_MENU_DEFINE_RADIO(SidAddress)
TUI_MENU_DEFINE_RADIO(SidClock)


static TUI_MENU_CALLBACK(sid_engine_model_submenu_callback)
{
    char *s;
    int temp;
    int value;

    resources_get_int("SidModel", &temp);
    resources_get_int("SidEngine", &value);
    value <<= 8;
    value |= temp;
    switch (value) {
        case SID_FASTSID_6581:
            s = "6581 (Fast SID)";
            break;
        case SID_FASTSID_8580:
            s = "8580 (Fast SID)";
            break;
#ifdef HAVE_PARSID
        case SID_PARSID_PORT1:
            s = "ParSID in Port 1";
            break;
        case SID_PARSID_PORT2:
            s = "ParSID in Port 2";
            break;
        case SID_PARSID_PORT3:
            s = "ParSID in Port 3";
            break;
#endif
    }
    return s;
}

static TUI_MENU_CALLBACK(sid_radio_engine_model_callback)
{
    int engine;
    int model;

    if (been_activated) {
        engine = (int)param;
        engine >>= 8;
        model = (int)param;
        model &= 0xff;
        sid_set_engine_model(engine, model);
        *become_default = 1;
    } else {
        resource_value_t v;
        resources_get_int("SidEngine", &engine);
        resources_get_int("SidModel", &model);
        engine <<= 8;
        engine |= model;
        if (engine == (int)param) {
            *become_default = 1;
        }
    }
    return NULL;
}

static tui_menu_item_def_t sid_engine_model_submenu[] = {
    { "_6581 (Fast SID)",
      "Fast SID 6581 emulation",
      sid_radio_engine_model_callback, (void *)SID_FASTSID_6581, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8580 (Fast SID)",
      "Fast SID 8580 emulation",
      sid_radio_engine_model_callback, (void *)SID_FASTSID_8580, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#ifdef HAVE_PARSID
    { "ParSID in Port 1",
      "ParSID emulation",
      sid_radio_engine_model_callback, (void *)SID_PARSID_PORT1, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "ParSID in Port 2",
      "ParSID emulation",
      sid_radio_engine_model_callback, (void *)SID_PARSID_PORT2, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "ParSID in Port 3",
      "ParSID emulation",
      sid_radio_engine_model_callback, (void *)SID_PARSID_PORT3, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
#endif
    { NULL }
};

static TUI_MENU_CALLBACK(sid_address_submenu_callback)
{
    static char s[256];
    int value;

    resources_get_int("SidAddress", &value);
    sprintf(s, (value) ? sidcart_secondary_address : sidcart_primary_address);

    return s;
}

static tui_menu_item_def_t sid_address_submenu[] = {
    { NULL, /* primary sidcart address */
      "Primary SID address",
      radio_SidAddress_callback, (void *)0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL, /* secondary sidcart address */
      "Secondary SID address",
      radio_SidAddress_callback, (void *)1, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(sid_clock_submenu_callback)
{
    char *s;
    int value;

    resources_get_int("SidClock", &value);
    s = (value) ? sidcart_clock : "C64";

    return s;
}

static tui_menu_item_def_t sid_clock_submenu[] = {
    { "C64",
      "C64 Clock",
      radio_SidClock_callback, (void *)0, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL, /* native clock */
      "Native Clock",
      radio_SidClock_callback, (void *)1, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

tui_menu_item_def_t sidcart_ui_menu_items[] = {
    { "_Enable SID cart:",
      "Enable/disable emulation of the SID cartridge",
      toggle_SidCart_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "SID _Engine/Model:",
      "Select the SID engine and model",
      sid_engine_model_submenu_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, sid_engine_model_submenu, "SID engine/model" },
    { "SID _Filters:",
      "Enable/disable emulation of the SID built-in programmable filters",
      toggle_SidFilters_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "SID _Address:",
      "Select the address of the SID",
      sid_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_address_submenu, "SID address" },
    { "SID _Clock:",
      "Select the clock used for the SID",
      sid_clock_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, sid_clock_submenu, "SID clock" },
    { NULL }
};

TUI_MENU_DEFINE_TOGGLE(DIGIBLASTER)

tui_menu_item_def_t sidcart_plus4_ui_menu_items[] = {
    { "_Enable SID cart:",
      "Enable/disable emulation of the SID cartridge",
      toggle_SidCart_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "SID _Engine/Model:",
      "Select the SID engine and model",
      sid_engine_model_submenu_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, sid_engine_model_submenu, "SID engine/model" },
    { "SID _Filters:",
      "Enable/disable emulation of the SID built-in programmable filters",
      toggle_SidFilters_callback, NULL, 4,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "SID _Address:",
      "Select the address of the SID",
      sid_address_submenu_callback, NULL, 5,
      TUI_MENU_BEH_CONTINUE, sid_address_submenu, "SID address" },
    { "SID _Clock:",
      "Select the clock used for the SID",
      sid_clock_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, sid_clock_submenu, "SID clock" },
    { "Enable digiblaster add-on:",
      "Enable/disable emulation of the digiblaster add-on",
      toggle_DIGIBLASTER_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uisidcart_init(struct tui_menu *parent_submenu, char *addr1, char *addr2, char *clock)
{
    tui_menu_t ui_sidcart_submenu;

    sidcart_primary_address = addr1;
    sid_address_submenu[0].label = addr1;
    sidcart_secondary_address = addr2;
    sid_address_submenu[1].label = addr2;
    sidcart_clock = clock;
    sid_clock_submenu[1].label = clock;

    ui_sidcart_submenu = tui_menu_create("SID cart settings", 1);

    tui_menu_add(ui_sidcart_submenu, sidcart_ui_menu_items);

    tui_menu_add_submenu(parent_submenu, "_SID cart settings...",
                         "SID cart settings",
                         ui_sidcart_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

void uisidcart_plus4_init(struct tui_menu *parent_submenu, char *addr1, char *addr2, char *clock)
{
    tui_menu_t ui_sidcart_plus4_submenu;

    sidcart_primary_address = addr1;
    sid_address_submenu[0].label = addr1;
    sidcart_secondary_address = addr2;
    sid_address_submenu[1].label = addr2;
    sidcart_clock = clock;
    sid_clock_submenu[1].label = clock;

    ui_sidcart_plus4_submenu = tui_menu_create("SID cart settings", 1);

    tui_menu_add(ui_sidcart_plus4_submenu, sidcart_plus4_ui_menu_items);

    tui_menu_add_submenu(parent_submenu, "_SID cart settings...",
                         "SID cart settings",
                         ui_sidcart_plus4_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}

/*
 * uic64cart.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>

#include "cartridge.h"
#include "keyboard.h"
#include "lib.h"
#include "menudefs.h"
#include "tui.h"
#include "tuifs.h"
#include "tuimenu.h"
#include "ui.h"
#include "uic64cart.h"
#include "util.h"

TUI_MENU_DEFINE_TOGGLE(CartridgeReset)

static TUI_MENU_CALLBACK(attach_cartridge_callback)
{
    if (been_activated) {
        char *default_item, *directory;
        char *name;
        const char *s, *filter;
        int type = (int)param;

        s = cartridge_get_file_name((WORD)0);
        util_fname_split(s, &directory, &default_item);

        filter = (type == CARTRIDGE_CRT) ? "*.crt" : "*";

        name = tui_file_selector("Attach cartridge image", directory, filter, default_item, NULL, NULL, NULL);
        if (name != NULL && (s == NULL || strcasecmp(name, s) != 0) && cartridge_attach_image(type, name) < 0) {
            tui_error("Invalid cartridge image.");
        }
        ui_update_menus();
        lib_free(name);
    }

    return NULL;
}

static TUI_MENU_CALLBACK(cartridge_set_default_callback)
{
    if (been_activated) {
        cartridge_set_default();
    }

    return NULL;
}


static TUI_MENU_CALLBACK(cartridge_callback)
{
    const char *s = cartridge_get_file_name((WORD)0);

    if (s == NULL || *s == '\0') {
        return "(none)";
    } else {
        return s;
    }
}

static tui_menu_item_def_t attach_cartridge_submenu_items[] = {
    { "Attach _CRT image...",
      "Attach a CRT image, autodetecting its type",
      attach_cartridge_callback, (void *)CARTRIDGE_CRT, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach generic _8KB image...",
      "Attach a generic 8KB cartridge dump",
      attach_cartridge_callback, (void *)CARTRIDGE_GENERIC_8KB, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach Generic _16KB image...",
      "Attach a generic 16KB cartridge dump",
      attach_cartridge_callback, (void *)CARTRIDGE_GENERIC_16KB, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach _Action Replay image...",
      "Attach an Action Replay cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_ACTION_REPLAY, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach _Action Replay MK3 image...",
      "Attach an Action Replay MK3 cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_ACTION_REPLAY3, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach _Action Replay MK4 image...",
      "Attach an Action Replay MK4 cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_ACTION_REPLAY4, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach _Stardos image...",
      "Attach a Stardos cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_STARDOS, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach Atomic _Power image...",
      "Attach an Atomic Power cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_ATOMIC_POWER, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach _Epyx FastLoad image...",
      "Attach an Epyx FastLoad cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_EPYX_FASTLOAD, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach _IEEE-488 Interface image...",
      "Attach an IEEE-488 interface cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_IEEE488, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach _Retro Replay image...",
      "Attach a Retro Replay cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_RETRO_REPLAY, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach IDE_64 interface image...",
      "Attach an IDE64 interface cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_IDE64, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach Super Snapshot V_4 image...",
      "Attach an Super Snapshot V4 cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_SUPER_SNAPSHOT, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Attach Super Snapshot V_5 image...",
      "Attach an Super Snapshot V5 cartridge image",
      attach_cartridge_callback, (void *)CARTRIDGE_SUPER_SNAPSHOT_V5, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "Set cartridge as _default",
      "Save the current cartridge to the settings",
      cartridge_set_default_callback, NULL, 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Reset on cart change",
      "Reset the machine when a cart is inserted or detached",
      toggle_CartridgeReset_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t attach_cartridge_menu_items[] = {
    { "--" },
    { "_Cartridge:",
      "Attach a cartridge image",
      cartridge_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, attach_cartridge_submenu_items,
      "Attach cartridge" },
    { NULL }
};

static TUI_MENU_CALLBACK(detach_cartridge_callback)
{
    const char *s;

    if (been_activated) {
        cartridge_detach_image(-1);
    }

    s = cartridge_get_file_name((WORD)0);

    if (s == NULL || *s == '\0') {
        return "(none)";
    } else {
        return s;
    }
}

static tui_menu_item_def_t detach_cartridge_menu_items[] = {
    { "--" },
    { "_Cartridge:",
      "Detach attached cartridge image",
      detach_cartridge_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(freeze_cartridge_callback)
{
    if (been_activated) {
        keyboard_clear_keymatrix();
        cartridge_trigger_freeze();
    }
    /* This way, the "Not Really!" item is always the default one.  */
    *become_default = 0;

    return NULL;
}

static tui_menu_item_def_t freeze_cartridge_menu_items[] = {
    { "Cartridge _Freeze",
      "Activates the cartridge's freeze button",
      freeze_cartridge_callback, NULL, 0,
      TUI_MENU_BEH_RESUME, NULL, NULL },
    { NULL }
};

void uic64cart_init(struct tui_menu *parent_submenu)
{
    tui_menu_add(ui_attach_submenu, attach_cartridge_menu_items);
    tui_menu_add(ui_detach_submenu, detach_cartridge_menu_items);
    tui_menu_add(ui_reset_submenu, freeze_cartridge_menu_items);
}

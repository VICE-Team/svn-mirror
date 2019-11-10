/*
 * menu_ram.c - RAM pattern menu for SDL UI.
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

#include "types.h"

#include "menu_common.h"
#include "menu_ram.h"
#include "resources.h"
#include "uimenu.h"

static const char *sdl_ui_menu_ram_slider_helper(int activated, ui_callback_data_t param, const char *resource_name, const int min, const int max)
{
    static char buf[20];
    int previous, new_value;

    if (resources_get_int(resource_name, &previous)) {
        return sdl_menu_text_unknown;
    }

    sprintf(buf, "%3.3f%%", (float)previous / (4096.0f / 100.0f));

    if (activated) {
        new_value = sdl_ui_slider_input_dialog((const char *)param, previous, min, max);
        if (new_value != previous) {
            resources_set_int(resource_name, new_value);
        }
    } else {
        return buf;
    }

    return NULL;
}

#define UI_MENU_DEFINE_SLIDER_RAM(resource, min, max)                              \
    static UI_MENU_CALLBACK(slider_##resource##_callback)                          \
    {                                                                              \
        return sdl_ui_menu_ram_slider_helper(activated, param, #resource, min, max);   \
    }

UI_MENU_DEFINE_RADIO(RAMInitStartValue)
UI_MENU_DEFINE_RADIO(RAMInitValueInvert)

UI_MENU_DEFINE_RADIO(RAMInitPatternInvert)
UI_MENU_DEFINE_RADIO(RAMInitPatternInvertValue)

UI_MENU_DEFINE_RADIO(RAMInitStartRandom)
UI_MENU_DEFINE_RADIO(RAMInitRepeatRandom)
UI_MENU_DEFINE_SLIDER_RAM(RAMInitRandomChance, 0, 0x1000)

static const ui_menu_entry_t RAMInitValueInvert_menu[] = {
    { "no",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)0 },
    { "1 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)1 },
    { "2 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)2 },
    { "4 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)4 },
    { "8 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)8 },
    { "16 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)16 },
    { "32 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)32 },
    { "64 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)64 },
    { "128 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)128 },
    { "256 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)256 },
    { "512 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)512 },
    { "1024 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)1024 },
    { "2048 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)2048 },
    { "4096 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)4096 },
    { "8192 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)8192 },
    { "16384 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)16384 },
    { "32768 bytes.",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitValueInvert_callback,
      (ui_callback_data_t)32768 },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t RAMInitPatternInvert_menu[] = {
    { "0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)0 },
    { "1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)1 },
    { "2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)2 },
    { "4",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)4 },
    { "8",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)8 },
    { "16",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)16 },
    { "32",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)32 },
    { "64",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)64 },
    { "128",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)128 },
    { "256",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)256 },
    { "512",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)512 },
    { "1024",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)1024 },
    { "2048",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)2048 },
    { "4096",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)4096 },
    { "8192",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)8192 },
    { "16384",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)16384 },
    { "32768",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvert_callback,
      (ui_callback_data_t)32768 },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t RAMInitStartRandom_menu[] = {
    { "none",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)0 },
    { "1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)1 },
    { "2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)2 },
    { "4",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)4 },
    { "8",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)8 },
    { "16",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)16 },
    { "32",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)32 },
    { "64",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)64 },
    { "128",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)128 },
    { "256",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartRandom_callback,
      (ui_callback_data_t)256 },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t RAMInitRepeatRandom_menu[] = {
    { "none",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)0 },
    { "1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)1 },
    { "2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)2 },
    { "4",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)4 },
    { "8",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)8 },
    { "16",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)16 },
    { "32",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)32 },
    { "64",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)64 },
    { "128",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)128 },
    { "256",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitRepeatRandom_callback,
      (ui_callback_data_t)256 },
    SDL_MENU_LIST_END
};

const ui_menu_entry_t ram_menu[] = {
    SDL_MENU_ITEM_TITLE("Value of first byte"),
    { "$0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartValue_callback,
      (ui_callback_data_t)0 },
    { "$ff",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitStartValue_callback,
      (ui_callback_data_t)255 },
    SDL_MENU_ITEM_SEPARATOR,
    { "Invert first byte every",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)RAMInitValueInvert_menu },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Value of second byte"),
    { "$00",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvertValue_callback,
      (ui_callback_data_t)0 },
    { "$66",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvertValue_callback,
      (ui_callback_data_t)0x66 },
    { "$99",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvertValue_callback,
      (ui_callback_data_t)0x99 },
    { "$ff",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RAMInitPatternInvertValue_callback,
      (ui_callback_data_t)255 },
    SDL_MENU_ITEM_SEPARATOR,
    { "Invert with second byte every",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)RAMInitPatternInvert_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Length of random pattern",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)RAMInitStartRandom_menu },
    { "Repeat random pattern each",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)RAMInitRepeatRandom_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Global random chance:",
      MENU_ENTRY_RESOURCE_INT,
      slider_RAMInitRandomChance_callback,
      (ui_callback_data_t)"Set chance (0-4096)" },
    SDL_MENU_LIST_END
};

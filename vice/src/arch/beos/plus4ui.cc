/*
 * plus4ui.c - C64-specific user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <FilePanel.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Window.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__BEOS__) && defined(WORDS_BIGENDIAN)
#include <string.h>
#endif

#include "ui_file.h"

extern "C" {
#include "constants.h" 
#include "plus4model.h"
#include "plus4ui.h"
#include "resources.h"
#include "ui.h"
#include "ui_sidcart.h"
#include "ui_ted.h"
#include "video.h"
}

ui_menu_toggle  plus4_ui_menu_toggles[] = {
    { "TEDDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "TEDDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "TEDVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "Acia1Enable", MENU_TOGGLE_ACIA },
    { "SIDCartJoy", MENU_TOGGLE_SIDCART_JOY },
    { "SpeechEnabled", MENU_TOGGLE_V364SPEECH },
    { NULL, 0 }
};

ui_res_possible_values plus4AciaDevice[] = {
    { 1, MENU_ACIA_RS323_DEVICE_1 },
    { 2, MENU_ACIA_RS323_DEVICE_2 },
    { 3, MENU_ACIA_RS323_DEVICE_3 },
    { 4, MENU_ACIA_RS323_DEVICE_4 },
    { -1, 0 }
};

ui_res_possible_values plus4AciaInt[] = {
    { 0, MENU_ACIA_INT_NONE },
    { 1, MENU_ACIA_INT_IRQ },
    { 2, MENU_ACIA_INT_NMI },
    { -1, 0 }
};

ui_res_possible_values plus4_RenderFilters[] = {
    { VIDEO_FILTER_NONE, MENU_RENDER_FILTER_NONE },
    { VIDEO_FILTER_CRT, MENU_RENDER_FILTER_CRT_EMULATION },
    { VIDEO_FILTER_SCALE2X, MENU_RENDER_FILTER_SCALE2X },
    { -1, 0 }
};

ui_res_value_list plus4_ui_res_values[] = {
    { "Acia1Dev", plus4AciaDevice },
    { "Acia1Irq", plus4AciaInt },
    { "TEDFilter", plus4_RenderFilters },
    { NULL, NULL }
};

static const char *plus4sidcartaddresspair[] = { "$FD40", "$FE80" };
static const char *plus4sidcartclockpair[] = { "C64", "PLUS4" };

static void plus4_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
        case MENU_TED_SETTINGS:
            ui_ted();
            break;
        case MENU_SIDCART_SETTINGS:
            ui_sidcart(plus4sidcartaddresspair, plus4sidcartclockpair);
            ui_update_menus();
            break;
        case MENU_V364SPEECH_FILE:
            ui_select_file(B_SAVE_PANEL, V364SPEECH_FILE, (void*)0);
            break;
        case MENU_PLUS4_MODEL_C16_PAL:
            plus4model_set(PLUS4MODEL_C16_PAL);
            break;
        case MENU_PLUS4_MODEL_C16_NTSC:
            plus4model_set(PLUS4MODEL_C16_NTSC);
            break;
        case MENU_PLUS4_MODEL_PLUS4_PAL:
            plus4model_set(PLUS4MODEL_PLUS4_PAL);
            break;
        case MENU_PLUS4_MODEL_PLUS4_NTSC:
            plus4model_set(PLUS4MODEL_PLUS4_NTSC);
            break;
        case MENU_PLUS4_MODEL_V364_NTSC:
            plus4model_set(PLUS4MODEL_V364_NTSC);
            break;
        case MENU_PLUS4_MODEL_C232_NTSC:
            plus4model_set(PLUS4MODEL_232_NTSC);
            break;
        default:
            break;
    }
}

int plus4ui_init(void)
{
    ui_register_machine_specific(plus4_ui_specific);
    ui_register_menu_toggles(plus4_ui_menu_toggles);
    ui_register_res_values(plus4_ui_res_values);
    ui_update_menus();
    return 0;
}

void plus4ui_shutdown(void)
{
}

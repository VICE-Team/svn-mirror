/*
 * petui.cc - PET-specific user interface.
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

extern "C" {
#include "archdep.h"
#include "constants.h"
#include "petui.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "ui_file.h"
#include "ui_pet.h"
#include "ui_sidcart.h"
#include "util.h"
#include "video.h"
}

ui_menu_toggle  pet_ui_menu_toggles[] = {
    { "CRTCDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "CRTCStretchVertical", MENU_TOGGLE_STRETCHVERTICAL },
    { "CRTCDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "CRTCVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "PETREU", MENU_TOGGLE_PETREU },
    { "PETDWW", MENU_TOGGLE_PETDWW },
    { "UserportDAC", MENU_TOGGLE_PET_USERPORT_DAC },
    { NULL, 0 }
};

ui_res_possible_values PETREUSize[] = {
    { 128, MENU_PETREU_SIZE_128 },
    { 512, MENU_PETREU_SIZE_512 },
    { 1024, MENU_PETREU_SIZE_1024 },
    { 2048, MENU_PETREU_SIZE_2048 },
    { -1, 0 }
};

ui_res_possible_values pet_RenderFilters[] = {
    { VIDEO_FILTER_NONE, MENU_RENDER_FILTER_NONE },
    { VIDEO_FILTER_CRT, MENU_RENDER_FILTER_CRT_EMULATION },
    { -1, 0 }
};

ui_res_value_list pet_ui_res_values[] = {
    {"PETREUsize", PETREUSize},
    {"CRTCFilter", pet_RenderFilters },
    {NULL, NULL}
};

static char *petsidcartaddresspair[] = { "$8F00", "$E900" };
static char *petsidcartclockpair[] = { "C64", "PET" };

void pet_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
        case MENU_PET_SETTINGS:
            ui_pet();
            break;
        case MENU_SIDCART_SETTINGS:
            ui_sidcart(petsidcartaddresspair, petsidcartclockpair);
            break;
        case MENU_PETREU_FILE:
            ui_select_file(B_SAVE_PANEL, PETREU_FILE, (void*)0);
            break;
        case MENU_PETDWW_FILE:
            ui_select_file(B_SAVE_PANEL, PETDWW_FILE, (void*)0);
            break;
        default:
            break;
    }
}

extern "C" {
int petui_init(void)
{
    ui_register_machine_specific(pet_ui_specific);
    ui_register_menu_toggles(pet_ui_menu_toggles);
    ui_register_res_values(pet_ui_res_values);
    ui_update_menus();
    return 0;
}

void petui_shutdown(void)
{
}

}

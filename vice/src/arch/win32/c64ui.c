/*
 * c64ui.c - C64-specific user interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include <windows.h>
#include <windowsx.h>

#include "ui.h"

#include "c64ui.h"
#include "cartridge.h"
#include "kbd.h"
#include "keyboard.h"
#include "res.h"
#include "uilib.h"
#include "uivicii.h"
#include "winmain.h"

ui_menu_toggle  c64_ui_menu_toggles[]={
    { "DoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "DoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "REU", IDM_TOGGLE_REU },
    { "Mouse", IDM_MOUSE },
    { "SidFilters", IDM_TOGGLE_SIDFILTERS },
#ifdef HAVE_RESID
    { "SidUseResid", IDM_TOGGLE_SOUND_RESID },
#endif
    { NULL, 0 }
};

static ui_res_possible_values SidType[] = {
    {0, IDM_SIDTYPE_6581},
    {1, IDM_SIDTYPE_8580},
    {-1,0}
};

ui_res_value_list c64_ui_res_values[] = {
    {"SidModel", SidType},
    {NULL,NULL}
};

void c64_ui_specific(WPARAM wparam, HWND hwnd)
{
char    *s;
int     type;

    switch (wparam) {
        case IDM_CART_ATTACH_CRT:
            type = CARTRIDGE_CRT;
            if ((s = ui_select_file("Attach CRT cartridge image",
                "CRT cartridge image files (*.crt)\0*.crt\0"
                "All files (*.*)\0*.*\0", NULL, hwnd)) != NULL) {
                if (cartridge_attach_image(type, s) < 0)
                    ui_error("Invalid cartridge image");
                free(s);
            }
            break;
        case IDM_CART_ATTACH_8KB:
            type = CARTRIDGE_GENERIC_8KB;
            if ((s = ui_select_file("Attach raw 8KB cartridge image",
                "Raw 8KB cartridge image files (*.bin)\0*.bin\0"
                "All files (*.*)\0*.*\0", NULL, hwnd)) != NULL) {
                if (cartridge_attach_image(type, s) < 0)
                    ui_error("Invalid cartridge image");
                free(s);
            }
            break;
        case IDM_CART_ATTACH_16KB:
            type = CARTRIDGE_GENERIC_16KB;
            if ((s = ui_select_file("Attach raw 16KB cartridge image",
                "Raw 16KB cartrdige image files (*.bin)\0*.bin\0"
                "All files (*.*)\0*.*\0", NULL, hwnd)) != NULL) {
                if (cartridge_attach_image(type, s) < 0)
                    ui_error("Invalid cartridge image");
                free(s);
            }
            break;
        case IDM_CART_ATTACH_AR:
            type = CARTRIDGE_ACTION_REPLAY;
            if ((s = ui_select_file("Attach Action Replay cartridge image",
                "Raw AR cartridge image files (*.bin)\0*.bin\0"
                "All files (*.*)\0*.*\0", NULL, hwnd)) != NULL) {
                if (cartridge_attach_image(type, s) < 0)
                    ui_error("Invalid cartridge image");
                free(s);
            }
            break;
        case IDM_CART_ATTACH_AT:
            type = CARTRIDGE_ATOMIC_POWER;
            if ((s = ui_select_file("Attach Atomic Power cartridge image",
                "Raw AT cartridge image files (*.bin)\0*.bin\0"
                "All files (*.*)\0*.*\0", NULL, hwnd)) != NULL) {
                if (cartridge_attach_image(type, s) < 0)
                    ui_error("Invalid cartridge image");
                free(s);
            }
            break;
        case IDM_CART_ATTACH_IEEE488:
            type = CARTRIDGE_IEEE488;
            if ((s = ui_select_file("Attach IEEE interface cartridge image",
                "Raw IEEE488 interface cartridge image files (*.bin)\0*.bin\0"
                "All files (*.*)\0*.*\0", NULL, hwnd)) != NULL) {
                if (cartridge_attach_image(type, s) < 0)
                    ui_error("Invalid cartridge image");
                free(s);
            }
            break;
        case IDM_CART_ATTACH_SS4:
            type = CARTRIDGE_SUPER_SNAPSHOT;
            if ((s = ui_select_file("Attach Super Snapshot 4 cartridge image",
                "Raw SS4 cartridge image files (*.bin)\0*.bin\0"
                "All files (*.*)\0*.*\0", NULL, hwnd)) != NULL) {
                if (cartridge_attach_image(type, s) < 0)
                    ui_error("Invalid cartridge image");
                free(s);
            }
            break;
        case IDM_CART_SET_DEFAULT:
            cartridge_set_default();
            break;
        case IDM_CART_DETACH:
            cartridge_detach_image();
            break;
        case IDM_CART_FREEZE|0x00010000:
        case IDM_CART_FREEZE:
            keyboard_clear_keymatrix();
            cartridge_trigger_freeze();
            break;
        case IDM_VICII_SETTINGS:
            ui_vicii_settings_dialog(hwnd);
    }
}

int c64_ui_init(void)
{
    ui_register_machine_specific(c64_ui_specific);
    ui_register_menu_toggles(c64_ui_menu_toggles);
    ui_register_res_values(c64_ui_res_values);
    return 0;
}


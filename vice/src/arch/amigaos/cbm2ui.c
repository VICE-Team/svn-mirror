/*
 * cbm2ui.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#define UI_CBM2
#define UI_MENU_NAME cbm2_ui_translation_menu
#define UI_TRANSLATED_MENU_NAME cbm2_ui_menu

#include "private.h"
#include "cbm2ui.h"
#include "cbm2uires.h"
#include "machine.h"
#include "translate.h"

#include "mui/uiacia.h"
#include "mui/uicbm2settings.h"
#include "mui/uidrivepetcbm2.h"
#include "mui/uijoystick.h"
#include "mui/uijoystickll.h"
#include "mui/uiprinter.h"
#include "mui/uiromcbm2settings.h"
#include "mui/uisid.h"
#include "mui/uivideo.h"

static const ui_menu_toggle_t cbm2_ui_menu_toggles[] = {
    { "CrtcDoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "CrtcDoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "CrtcVideoCache", IDM_TOGGLE_VIDEOCACHE },
    { NULL, 0 }
};

static int cbm2_ui_specific(video_canvas_t *canvas, int idm)
{
    switch (idm) {
        case IDM_CBM2_SETTINGS:
            ui_cbm2_settings_dialog();
            break;
        case IDM_SID_SETTINGS:
            ui_sid_settings_dialog();
            break;
        case IDM_COMPUTER_ROM_SETTINGS:
            ui_cbm2_computer_rom_settings_dialog(canvas);
            break;
        case IDM_DRIVE_ROM_SETTINGS:
            ui_cbm2_drive_rom_settings_dialog(canvas);
            break;
        case IDM_VIDEO_SETTINGS:
            if (machine_class == VICE_MACHINE_CBM5x0) {
                ui_video_settings_dialog(canvas,
                                         "VICIIExternalPalette", "VICIIPaletteFile",
                                         translate_text(IDS_VICII_EXTERNAL_PALETTE), translate_text(IDS_VICII_PALETTE_NAME),
                                         NULL, NULL,
                                         NULL, NULL,
                                         "VICIIScale2x");
            } else {
                ui_video_settings_dialog(canvas,
                                         "CRTCExternalPalette", "CRTCPaletteFile",
                                         translate_text(IDS_CRTC_EXTERNAL_PALETTE), translate_text(IDS_CRTC_PALETTE_NAME),
                                         NULL, NULL,
                                         NULL, NULL,
                                         NULL);
            }
            break;
        case IDM_DRIVE_SETTINGS:
            uidrivepetcbm2_settings_dialog();
            break;
        case IDM_PRINTER_SETTINGS:
            ui_printer_settings_dialog(canvas, 1, 1);
            break;
        case IDM_ACIA_SETTINGS:
            ui_acia_settings_dialog();
            break;
#ifdef AMIGA_OS4
        case IDM_JOY_SETTINGS:
            if (machine_class == VICE_MACHINE_CBM5x0) {
                ui_joystick_settings_cbm5x0_dialog();
            } else {
                ui_joystick_settings_pet_dialog();
            }
            break;
#else
        case IDM_JOY_DEVICE_SELECTION:
            if (machine_class == VICE_MACHINE_CBM5x0) {
                ui_joystick_device_cbm5x0_dialog();
            } else {
                ui_joystick_device_pet_dialog();
            }
            break;
        case IDM_JOY_FIRE_SELECTION:
            if (machine_class == VICE_MACHINE_CBM5x0) {
                ui_joystick_fire_cbm5x0_dialog();
            } else {
                ui_joystick_fire_pet_dialog();
            }
            break;
#endif
        case IDM_KEYBOARD_SETTINGS:
//          uikeyboard_settings_dialog(hwnd, &uikeyboard_config);
            break;
    }

    return 0;
}

int cbm2ui_init(void)
{
    ui_register_menu_translation_layout(cbm2_ui_translation_menu);
    ui_register_menu_layout(cbm2_ui_menu);
    ui_register_machine_specific(cbm2_ui_specific);
    ui_register_menu_toggles(cbm2_ui_menu_toggles);

    return 0;
}

void cbm2ui_shutdown(void)
{
}

/*
 * uijoyport.c
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
#ifdef AMIGA_M68K
#define _INLINE_MUIMASTER_H
#endif
#include "mui.h"

#include "joyport.h"
#include "uijoyport.h"
#include "intl.h"
#include "machine.h"
#include "translate.h"

static char *ui_joyport_1[JOYPORT_MAX_DEVICES + 1];
static const int ui_joyport_1_values[JOYPORT_MAX_DEVICES + 1];

static char *ui_joyport_2[JOYPORT_MAX_DEVICES + 1];
static const int ui_joyport_2_values[JOYPORT_MAX_DEVICES + 1];

static ui_to_from_t ui_to_from1[] = {
    { NULL, MUI_TYPE_CYCLE, "JoyPort1Device", ui_joyport_1, ui_joyport_1_values, NULL },
    UI_END /* mandatory */
};

static ui_to_from_t ui_to_from2[] = {
    { NULL, MUI_TYPE_CYCLE, "JoyPort1Device", ui_joyport_1, ui_joyport_1_values, NULL },
    { NULL, MUI_TYPE_CYCLE, "JoyPort2Device", ui_joyport_2, ui_joyport_2_values, NULL },
    UI_END /* mandatory */
};

static APTR build_gui1(void)
{
    APTR app, ui, ok, cancel;

    app = mui_get_app();

    ui = GroupObject,
           CYCLE(ui_to_from1[0].object, translate_text(IDS_JOYPORT_1_DEVICE), ui_joyport_1)
           OK_CANCEL_BUTTON
         End;

    if (ui != NULL) {
        DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, BTN_OK);
    }

    return ui;
}

static APTR build_gui2(void)
{
    APTR app, ui, ok, cancel;

    app = mui_get_app();

    ui = GroupObject,
           CYCLE(ui_to_from2[0].object, translate_text(IDS_JOYPORT_1_DEVICE), ui_joyport_1)
           CYCLE(ui_to_from2[1].object, translate_text(IDS_JOYPORT_2_DEVICE), ui_joyport_2)
           OK_CANCEL_BUTTON
         End;

    if (ui != NULL) {
        DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, BTN_OK);
    }

    return ui;
}

void ui_joyport_settings_dialog(int ports)
{
    APTR window;
    int i;
    joyport_desc_t *devices_port_1 = joyport_get_valid_devices(JOYPORT_1);
    joyport_desc_t *devices_port_2 = joyport_get_valid_devices(JOYPORT_2);

    for (i = 0; devices_port_1[i].name; ++i) {
        ui_joyport_1[i] = translate_text(devices_port_1[i].trans_name);
        ui_joyport_1_values[i] = devices_port_1[i].id;
    }
    ui_joyport_1[i] = NULL;
    ui_joyport_1_values[i] = -1;

    lib_free(devices_port_1);

    if (ports == 2) {
        for (i = 0; devices_port_2[i].name; ++i) {
            ui_joyport_2[i] = translate_text(devices_port_2[i].trans_name);
            ui_joyport_2_values[i] = devices_port_2[i].id;
        }
        ui_joyport_2[i] = NULL;
        ui_joyport_2_values[i] = -1;

        lib_free(devices_port_2);

        window = mui_make_simple_window(build_gui2(), translate_text(IDS_JOYPORT_SETTINGS));

        if (window != NULL) {
            mui_add_window(window);
            ui_get_to(ui_to_from2);
            set(window, MUIA_Window_Open, TRUE);
            if (mui_run() == BTN_OK) {
                ui_get_from(ui_to_from2);
            }
            set(window, MUIA_Window_Open, FALSE);
            mui_rem_window(window);
            MUI_DisposeObject(window);
        }
    } else {
        window = mui_make_simple_window(build_gui1(), translate_text(IDS_JOYPORT_SETTINGS));

        if (window != NULL) {
            mui_add_window(window);
            ui_get_to(ui_to_from1);
            set(window, MUIA_Window_Open, TRUE);
            if (mui_run() == BTN_OK) {
                ui_get_from(ui_to_from1);
            }
            set(window, MUIA_Window_Open, FALSE);
            mui_rem_window(window);
            MUI_DisposeObject(window);
        }
    }
}

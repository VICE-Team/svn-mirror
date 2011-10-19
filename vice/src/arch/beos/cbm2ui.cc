/*
 * cbm2ui.cc - CBM2-specific user interface.
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

#include <Message.h>
#include <stdio.h>

extern "C" {
#include "cbm2ui.h"
#include "constants.h"
#include "ui.h"
#include "ui_cbm2.h"
#include "ui_sid.h"
#include "video.h"
}

ui_menu_toggle  cbm2_ui_menu_toggles[] = {
    { "CRTCDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "CRTCStretchVertical", MENU_TOGGLE_STRETCHVERTICAL },
    { "CRTCDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "CRTCVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { NULL, 0 }
};

ui_res_possible_values cbm2RenderFilters[] = {
    { VIDEO_FILTER_NONE, MENU_RENDER_FILTER_NONE },
    { VIDEO_FILTER_CRT, MENU_RENDER_FILTER_CRT_EMULATION },
    { -1, 0 }
};

ui_res_value_list cbm2_ui_res_values[] = {
    { "CRTCFilter", cbm2RenderFilters },
    { NULL, NULL }
};

void cbm2_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
        case MENU_CBM2_SETTINGS:
            ui_cbm2();
            break;
        case MENU_SID_SETTINGS:
            ui_sid(NULL);
            break;
        default: ;
    }
}

int cbm2ui_init(void)
{
    ui_register_machine_specific(cbm2_ui_specific);
    ui_register_menu_toggles(cbm2_ui_menu_toggles);
    ui_register_res_values(cbm2_ui_res_values);
    ui_update_menus();
    return 0;
}

void cbm2ui_shutdown(void)
{
}

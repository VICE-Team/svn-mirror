/*
 * uipalemu.c
 *
 * Written by
 *  pottendo <pottendo@gmx.net>
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

#include "uipalemu.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uimenu.h"
#include "uipalemu.h"
#include "util.h"


static UI_CALLBACK(toggle_DelayLoopEmulation)
{
    int delayloopemulation;

    resources_get_int("PALEmulation", &delayloopemulation);

    if (!CHECK_MENUS) {
        resources_set_int("PALEmulation", !delayloopemulation);
        ui_update_menus();
    } else {
        int video_standard;

        resources_get_int("MachineVideoStandard", &video_standard);

        ui_menu_set_tick(w, delayloopemulation);

        if (video_standard == MACHINE_SYNC_PAL)
            ui_menu_set_sensitive(w, 1);
        else
            ui_menu_set_sensitive(w, 0);
    }
}

#ifndef USE_GNOMEUI
static UI_CALLBACK(PAL_control_cb)
{
    char buf[50];
    ui_button_t button;
    long res;
    int current;
    int what = vice_ptr_to_int(UI_MENU_CB_PARAM);
    char*resource;

    switch (what)
    {
      case 0:
        resource = "PALScanLineShade";
        resources_get_int(resource, &current);
        current /= 10;
        sprintf(buf, "%d", current);
        button = ui_input_string(_("PAL Scanline shade"), _("Scanline Shade in percent"), buf, 50);
        break;
      case 2:
        resource = "ColorTint";
        resources_get_int(resource, &current);
        current /= 10;
        sprintf(buf, "%d", current);
        button = ui_input_string(_("New PAL Tint"), _("Tint in percent"), buf, 50);
        break;
      case 3:
        resource = "PALOddLinePhase";
        resources_get_int(resource, &current);
        current /= 10;
        sprintf(buf, "%d", current);
        button = ui_input_string(_("New PAL Odd Line Phase"), _("Phase in percent"), buf, 50);
        break;
      case 4:
        resource = "PALOddLineOffset";
        resources_get_int(resource, &current);
        current /= 10;
        sprintf(buf, "%d", current);
        button = ui_input_string(_("New PAL Odd Line Offset"), _("Offset in percent"), buf, 50);
        break;
      case 1:
      default:
        resource = "PALBlur";
        resources_get_int(resource, &current);
        current /= 10;
        sprintf(buf, "%d", current);
        button = ui_input_string(_("PAL Blurredness"), _("Blurredness in percent"), buf, 50);
        break;
    }
    
    switch (button) {
      case UI_BUTTON_OK:
        if (util_string_to_long(buf, NULL, 10, &res) != 0) {
             ui_error(_("Invalid value: %s"), buf);
             return;
        }
        break;
      default:
        break;
    }

    if ((current != res) &&
        (res <= 100) &&
        (res >= 0))
        resources_set_int(resource, (int)(res * 10));
}
#endif

ui_menu_entry_t PALMode_submenu[] = {
    { N_("*Activate PAL emulation"),
      (ui_callback_t)toggle_DelayLoopEmulation, NULL, NULL },
#ifndef USE_GNOMEUI
    { "--" },
    { N_("PAL Scanline Shade"),
      (ui_callback_t)PAL_control_cb, (ui_callback_data_t) 0, NULL },
    { N_("PAL Blurredness"),
      (ui_callback_t)PAL_control_cb, (ui_callback_data_t) 1, NULL },
    { N_("New PAL Tint"),
      (ui_callback_t)PAL_control_cb, (ui_callback_data_t) 2, NULL },
    { N_("New PAL Odd Lines Phase"),
      (ui_callback_t)PAL_control_cb, (ui_callback_data_t) 3, NULL },
    { N_("New PAL Odd Lines Offset"),
      (ui_callback_t)PAL_control_cb, (ui_callback_data_t) 4, NULL },
#endif
    { NULL }
};

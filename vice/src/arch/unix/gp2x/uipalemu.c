/*
 * uipalemu.c
 *
 * Written by
 *  Martin Pottendorfer (pottend@utanet.at)
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
#include "uimenu.h"
#include "uipalemu.h"
#include "util.h"


static UI_CALLBACK(toggle_DelayLoopEmulation)
{
    int delayloopemulation;

    resources_get_value("PALEmulation", (void *)&delayloopemulation);

    if (!CHECK_MENUS) {
        resources_set_value("PALEmulation",
                            (resource_value_t)!delayloopemulation);
        ui_update_menus();
    } else {
        int video_standard;

        resources_get_value("MachineVideoStandard", (void *)&video_standard);

        ui_menu_set_tick(w, delayloopemulation);

        if (video_standard == MACHINE_SYNC_PAL)
            ui_menu_set_sensitive(w, True);
        else
            ui_menu_set_sensitive(w, False);
    }
}

UI_MENU_DEFINE_RADIO(PALMode)

#ifndef USE_GNOMEUI
static UI_CALLBACK(PAL_control_cb)
{
    char buf[50];
    ui_button_t button;
    long res;
    int current;
    int what = (int) UI_MENU_CB_PARAM;
    char*resource;

    if (what == 0)
    {
	resource = "PALScanLineShade";
	resources_get_value(resource, (void *)&current);
	current /= 10;
	sprintf(buf, "%d", current);
	button = ui_input_string(_("PAL Scanline shade"),
				 _("Scanline Shade in percent"),
				 buf, 50);
	
    }
    else
    {
	resource = "PALBlur";
	resources_get_value(resource, (void *)&current);
	current /= 10;
	sprintf(buf, "%d", current);
	button = ui_input_string(_("PAL Blurredness"),
				 _("Blurredness in percent"),
				 buf, 50);
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
        resources_set_value(resource, (resource_value_t)(res * 10));
}
#endif

ui_menu_entry_t PALMode_submenu[] = {
    { N_("*Activate PAL emulation"),
      (ui_callback_t)toggle_DelayLoopEmulation, NULL, NULL },
    { "--" },
    { N_("*Fast PAL Emulation"),
      (ui_callback_t)radio_PALMode, (ui_callback_data_t)0, NULL },
    { N_("*Exact PAL Emulation"),
      (ui_callback_t)radio_PALMode, (ui_callback_data_t)1, NULL },
#ifndef USE_GNOMEUI
    { "--" },
    { N_("PAL Scanline Shade"),
      (ui_callback_t)PAL_control_cb, (ui_callback_data_t) 0, NULL },
    { N_("PAL Blurredness"),
      (ui_callback_t)PAL_control_cb, (ui_callback_data_t) 1, NULL },
#endif
    { NULL }
};


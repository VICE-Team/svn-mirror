/*
 * uivicii.c
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "c64ui.h"
#include "drive.h"
#include "resources.h"
#include "uipalette.h"
#include "uimenu.h"
#include "uivicii.h"
#include "utils.h"


UI_MENU_DEFINE_STRING_RADIO(PaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { N_("*Default"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"default", NULL },
    { "*C64S",
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"c64s", NULL },
    { N_("*CCS64"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"ccs64", NULL },
    { N_("*Frodo"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"frodo", NULL },
    { N_("*GoDot"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"godot", NULL },
    { "*PC64",
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"pc64", NULL },
    { "--" },
    { N_("Load custom"),
      (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"PaletteFile", NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(CheckSsColl)
UI_MENU_DEFINE_TOGGLE(CheckSbColl)
UI_MENU_DEFINE_TOGGLE(ExternalPalette)

static UI_CALLBACK(toggle_DelayLoopEmulation)
{
    int delayloopemulation;

    resources_get_value("DelayLoopEmulation",
                        (resource_value_t *)&delayloopemulation);
    if (!CHECK_MENUS) {
        resources_set_value("DelayLoopEmulation",
                            (resource_value_t)!delayloopemulation);
        ui_update_menus();
    } else {
        int video_standard;

        resources_get_value("VideoStandard",
                            (resource_value_t *)&video_standard);
        ui_menu_set_tick(w, delayloopemulation);

        if (video_standard == DRIVE_SYNC_PAL)
            ui_menu_set_sensitive(w, True);
        else
            ui_menu_set_sensitive(w, False);
    }
}

UI_MENU_DEFINE_RADIO(PALMode)

static UI_CALLBACK(PAL_scanline_shade_cb)
{
    char buf[50];
    ui_button_t button;
    long res;
    int current;
    
    resources_get_value("PALScanLineShade", (resource_value_t) &current);
    current /= 10;
    sprintf(buf, "%d", current);
    button = ui_input_string(_("PAL Scanline shade"), _("Scanline Shade in percent"),
			     buf, 50);
    switch (button)
    {
    case UI_BUTTON_OK:
	if (util_string_to_long(buf, NULL, 10, &res) != 0)
	{
	     ui_error(_("Invalid value: %s"), buf);
	     return;
	}
	resources_set_value("PALScanLineShade", (resource_value_t) res);
	break;
    default:
	break;
    }

    if ((current != res) &&
	(res <= 100) &&
	(res >= 0) )
	resources_set_value("PALScanLineShade", (resource_value_t) (res * 10));
}

ui_menu_entry_t PALMode_submenu[] = {
    { N_("*Fake PAL Emulation"),
      (ui_callback_t)radio_PALMode, (ui_callback_data_t)0, NULL },
    { N_("*Sharp PAL Emulation (Y/C Input)"),
      (ui_callback_t)radio_PALMode, (ui_callback_data_t)1, NULL },
    { N_("*Blurry PAL Emulation (Composite Input)"),
      (ui_callback_t)radio_PALMode, (ui_callback_data_t)2, NULL },
    { N_("PAL Scanline Shade"),
      (ui_callback_t)PAL_scanline_shade_cb, NULL, NULL },
    { NULL }
};

ui_menu_entry_t vic_submenu[] = {
    { N_("Video standard"),
      NULL, NULL, set_video_standard_submenu },
    { "--" },
    { N_("*Sprite-sprite collisions"),
      (ui_callback_t)toggle_CheckSsColl, NULL, NULL },
    { N_("*Sprite-background collisions"),
      (ui_callback_t)toggle_CheckSbColl, NULL, NULL },
    { "--" },
    { N_("*External color set"),
      (ui_callback_t)toggle_ExternalPalette, NULL, NULL },
    { N_("Color set"),
      NULL, NULL, palette_submenu },
    { "--" },
    { N_("*PAL emulation"),
      (ui_callback_t)toggle_DelayLoopEmulation, NULL, NULL },
    { N_("PAL Emulation Settings"),
      NULL, NULL, PALMode_submenu },
    { NULL }
};


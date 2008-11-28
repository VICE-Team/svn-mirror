/*
 * uic64dtv.c
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

#include "c64dtv-resources.h"
#include "lib.h"
#include "resources.h"
#include "uilib.h"
#include "uimenu.h"
#include "uic64dtv.h"
#include "uips2mouse.h"
#include "util.h"

UI_MENU_DEFINE_RADIO(DtvRevision)
UI_MENU_DEFINE_TOGGLE(c64dtvromrw)
UI_MENU_DEFINE_RADIO(HummerUserportJoyPort)
UI_MENU_DEFINE_RADIO(HummerUserportDevice)

UI_CALLBACK(set_c64dtv_rom_name)
{
    char *filename;
    ui_button_t button;
    static char *last_dir;

    filename = ui_select_file(_("C64DTV ROM image name"),
                              NULL, 0, 0, last_dir,
                    	      "*.[bB][iI][nN]",
                    	      &button, 0, NULL, UI_FC_LOAD);

    switch (button) {
        case UI_BUTTON_OK:
            resources_set_string("c64dtvromfilename", filename);
            if (last_dir) {
                lib_free(last_dir);
            }
            util_fname_split(filename, &last_dir, NULL);
            break;
        default:
            /* Do nothing special. */
        break;
    }
    if (filename != NULL) {
        lib_free(filename);
    }
}

static ui_menu_entry_t c64dtv_revision_submenu[] = {
    { "*DTV2", (ui_callback_t)radio_DtvRevision,
      (ui_callback_data_t)2, NULL },
    { "*DTV3", (ui_callback_t)radio_DtvRevision,
      (ui_callback_data_t)3, NULL },
    { NULL }
};

static ui_menu_entry_t c64dtv_hummer_joy_submenu[] = {
    { N_("*Joy1"), (ui_callback_t)radio_HummerUserportJoyPort,
      (ui_callback_data_t)1, NULL },
    { N_("*Joy2"), (ui_callback_t)radio_HummerUserportJoyPort,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

static ui_menu_entry_t c64dtv_hummer_userport_device_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_HummerUserportDevice,
      (ui_callback_data_t)HUMMER_USERPORT_NONE, NULL },
    { "*ADC", (ui_callback_t)radio_HummerUserportDevice,
      (ui_callback_data_t)HUMMER_USERPORT_ADC, NULL },
    { N_("*Joystick"), (ui_callback_t)radio_HummerUserportDevice,
      (ui_callback_data_t)HUMMER_USERPORT_JOY, NULL },
    { NULL }
};

ui_menu_entry_t c64dtv_submenu[] = {
    { N_("C64DTV ROM image name..."),
      (ui_callback_t)set_c64dtv_rom_name,
      (ui_callback_data_t)"c64dtvromfilename", NULL },
    { N_("*Enable writes to C64DTV ROM image"),
      (ui_callback_t)toggle_c64dtvromrw, NULL, NULL },
    { "--" },
    { N_("C64DTV blitter revision"),
      NULL, NULL, c64dtv_revision_submenu },
    { "--" },
    { N_("*Hummer Userport Device"),
      NULL, NULL, c64dtv_hummer_userport_device_submenu },
    { N_("Hummer joystick port mapped to Userport"),
      NULL, NULL, c64dtv_hummer_joy_submenu },
    { N_("PS/2 mouse on Userport"),
      NULL, NULL, ps2_mouse_submenu },
    { NULL }
};

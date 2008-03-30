/*
 * uisid.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <stdlib.h>

#include "lib.h"
#include "resources.h"
#include "sid.h"
#include "uimenu.h"
#include "vsync.h"


UI_MENU_DEFINE_RADIO(SidEngine)

ui_menu_entry_t sid_engine_submenu[] = {
    { N_("*Fast SID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_FASTSID, NULL },
#ifdef HAVE_RESID
    { N_("*ReSID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_RESID, NULL },
#endif
#ifdef HAVE_CATWEASELMKIII
    { N_("*Catweasel MKIII"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_CATWEASELMKIII, NULL },
#endif
#ifdef HAVE_HARDSID
    { N_("*HardSID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_HARDSID, NULL },
#endif
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidModel)

ui_menu_entry_t sid_model_submenu[] = {
    { N_("*6581 (old)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)0, NULL },
    { N_("*8580 (new)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)1, NULL },
    { N_("*8580 + digi boost"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)2, NULL },
    { NULL }
};

#ifdef HAVE_RESID
UI_MENU_DEFINE_RADIO(SidResidSampling)

ui_menu_entry_t sid_resid_sampling_submenu[] = {
    { N_("*Fast"),
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)0, NULL },
    { N_("*Interpolating"),
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)1, NULL },
    { N_("*Resampling"),
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)2, NULL },
    { N_("*Fast Resampling"),
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)3, NULL },
    { NULL }
};

UI_CALLBACK(set_sid_resid_passband)
{
    static char input_string[32];
    char *msg_string;
    ui_button_t button;
    int i;

    vsync_suspend_speed_eval();

    resources_get_value("SidResidPassband", (void *)&i);

    sprintf(input_string, "%d", i);
    msg_string = lib_stralloc(_("Enter passband in percentage of total bandwidth\n(0 - 90, lower is faster, higher is better)"));
    button = ui_input_string(_("Passband percentage"),
                             msg_string, input_string, 32);
    lib_free(msg_string);
    if (button == UI_BUTTON_OK) {
        i = atoi(input_string);
        if (i < 0) {
            i = 0;
        } else {
            if (i > 90) {
                i = 90;
            }
        }
        resources_set_value("SidResidPassband", (resource_value_t)i);
        ui_update_menus();
    }
}
#endif


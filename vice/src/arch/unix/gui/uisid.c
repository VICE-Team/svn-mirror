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
#include "uiapi.h"
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
#ifdef HAVE_RESID_FP
    { N_("*ReSID-FP"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_RESID_FP, NULL },
#endif
#ifdef HAVE_CATWEASELMKIII
    { N_("*Catweasel MKIII"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_CATWEASELMKIII, NULL },
#endif
#ifdef HAVE_HARDSID
    { N_("*HardSID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_HARDSID, NULL },
#endif
#ifdef HAVE_PARSID
    { N_("*ParSID Port 1"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT1, NULL },
    { N_("*ParSID Port 2"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT2, NULL },
    { N_("*ParSID Port 3"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT3, NULL },
#endif
    { NULL }
};

ui_menu_entry_t sid_dtv_engine_submenu[] = {
    { N_("*Fast SID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_FASTSID, NULL },
#ifdef HAVE_RESID
    { N_("*ReSID-DTV"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_RESID, NULL },
#endif
#ifdef HAVE_RESID_FP
    { N_("*ReSID-FP"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_RESID_FP, NULL },
#endif
#ifdef HAVE_CATWEASELMKIII
    { N_("*Catweasel MKIII"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_CATWEASELMKIII, NULL },
#endif
#ifdef HAVE_HARDSID
    { N_("*HardSID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_HARDSID, NULL },
#endif
#ifdef HAVE_PARSID
    { N_("*ParSID Port 1"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT1, NULL },
    { N_("*ParSID Port 2"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT2, NULL },
    { N_("*ParSID Port 3"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT3, NULL },
#endif
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidModel)

ui_menu_entry_t sid_model_submenu[] = {
    { N_("*6581 (old)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581, NULL },
    { N_("*8580 (new)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580, NULL },
    { N_("*8580 + digi boost"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580D, NULL },
/*
    { N_("*6581R4 (not yet implemented)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R4, NULL },
*/
#ifdef HAVE_RESID_FP
    { "--" },
    { N_("*6581R3 4885 (very light)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_4885, NULL },
    { N_("*6581R3 0486S (light)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_0486S, NULL },
    { N_("*6581R3 3984 (light avg)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_3984, NULL },
    { N_("*6581R4AR 3789 (avg)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R4AR_3789, NULL },
    { N_("*6581R3 4485 (dark)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_4485, NULL },
    { N_("*6581R4 1986S (very dark)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R4_1986S, NULL },
    { N_("*8580 3691 (light)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_3691, NULL },
    { N_("*8580 3691 + digi boost"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_3691D, NULL },
    { N_("*8580 1489 (dark)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_1489, NULL },
    { N_("*8580 1489 + digi boost"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_1489D, NULL },
#endif
    { NULL }
};

ui_menu_entry_t sid_dtv_model_submenu[] = {
    { N_("*DTVSID"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_DTVSID, NULL },
    { "--" },
    { N_("*6581 (old)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581, NULL },
    { N_("*8580 (new)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580, NULL },
#ifdef HAVE_RESID_FP
    { "--" },
    { N_("*6581R3 4885 (very light)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_4885, NULL },
    { N_("*6581R3 0486S (light)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_0486S, NULL },
    { N_("*6581R3 3984 (light avg)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_3984, NULL },
    { N_("*6581R4AR 3789 (avg)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R4AR_3789, NULL },
    { N_("*6581R3 4485 (dark)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R3_4485, NULL },
    { N_("*6581R4 1986S (very dark)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_6581R4_1986S, NULL },
    { N_("*8580 3691 (light)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_3691, NULL },
    { N_("*8580 3691 + digi boost"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_3691D, NULL },
    { N_("*8580 1489 (dark)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_1489, NULL },
    { N_("*8580 1489 + digi boost"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_MODEL_8580R5_1489D, NULL },
#endif
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

    resources_get_int("SidResidPassband", &i);

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
        resources_set_int("SidResidPassband", i);
        ui_update_menus();
    }
}
#endif

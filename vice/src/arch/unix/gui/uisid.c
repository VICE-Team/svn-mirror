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
#include "uilib.h"
#include "uimenu.h"
#include "vsync.h"

static UI_CALLBACK(radio_SidModel)
{
    int engine, model, selected;
    selected = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (!CHECK_MENUS) {
        engine = selected >> 8;
        model = selected & 0xff;
        sid_set_engine_model(engine, model);
        ui_update_menus();
    } else {
        resources_get_int("SidEngine", &engine);
        resources_get_int("SidModel", &model);

        if (selected == ((engine << 8) | model)) {
            ui_menu_set_tick(w, 1);
        } else {
            ui_menu_set_tick(w, 0);
        }
    }
}

ui_menu_entry_t sid_model_submenu[] = {
    { "6581 (FastSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_FASTSID_6581, NULL },
    { "8580 (FastSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_FASTSID_8580, NULL },
#ifdef HAVE_RESID
    { "--", UI_MENU_TYPE_SEPARATOR },
    { "6581 (ReSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_RESID_6581, NULL },
    { "8580 (ReSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_RESID_8580, NULL },
    { N_("8580 + digi boost (ReSID)"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_RESID_8580D, NULL },
#endif
#if defined(HAVE_CATWEASELMKIII) || defined(HAVE_HARDSID) || defined(HAVE_PARSID)
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef HAVE_CATWEASELMKIII
    { "Catweasel MKIII", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_CATWEASELMKIII, NULL },
#endif
#ifdef HAVE_HARDSID
    { "HardSID", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_HARDSID, NULL },
#endif
#ifdef HAVE_PARSID
    { N_("ParSID Port 1"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT1, NULL },
    { N_("ParSID Port 2"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT2, NULL },
    { N_("ParSID Port 3"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT3, NULL },
#endif
#endif
    { NULL }
};

ui_menu_entry_t sid_dtv_model_submenu[] = {
#ifdef HAVE_RESID
    { "DTVSID", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_RESID_DTVSID, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
#endif
    { "6581 (FastSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_FASTSID_6581, NULL },
    { "8580 (FastSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_FASTSID_8580, NULL },
#if defined(HAVE_CATWEASELMKIII) || defined(HAVE_HARDSID) || defined(HAVE_PARSID)
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef HAVE_CATWEASELMKIII
    { "Catweasel MKIII", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_CATWEASELMKIII, NULL },
#endif
#ifdef HAVE_HARDSID
    { "HardSID", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_HARDSID, NULL },
#endif
#ifdef HAVE_PARSID
    { N_("ParSID Port 1"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT1, NULL },
    { N_("ParSID Port 2"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT2, NULL },
    { N_("ParSID Port 3"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT3, NULL },
#endif
#endif
    { NULL }
};

#if 0
ui_menu_entry_t sid_model_noresid_submenu[] = {
    { "6581 (FastSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_FASTSID_6581, NULL },
    { "8580 (FastSID)", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)SID_FASTSID_8580, NULL },
#if defined(HAVE_CATWEASELMKIII) || defined(HAVE_HARDSID) || defined(HAVE_PARSID)
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef HAVE_CATWEASELMKIII
    { "Catweasel MKIII", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_CATWEASELMKIII, NULL },
#endif
#ifdef HAVE_HARDSID
    { "HardSID", UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_HARDSID, NULL },
#endif
#ifdef HAVE_PARSID
    { N_("ParSID Port 1"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT1, NULL },
    { N_("ParSID Port 2"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT2, NULL },
    { N_("ParSID Port 3"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidModel,
      (ui_callback_data_t)SID_PARSID_PORT3, NULL },
#endif
#endif
    { NULL }
};
#endif

#ifdef HAVE_RESID
UI_MENU_DEFINE_RADIO(SidResidSampling)

ui_menu_entry_t sid_resid_sampling_submenu[] = {
    { N_("Fast"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)0, NULL },
    { N_("Interpolating"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)1, NULL },
    { N_("Resampling"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)2, NULL },
    { N_("Fast resampling"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_SidResidSampling, (ui_callback_data_t)3, NULL },
    { NULL }
};

#if !defined(USE_GNOMEUI)
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
    button = ui_input_string(_("Passband percentage"), msg_string, input_string, 32);
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

#endif

UI_MENU_DEFINE_RADIO(SidStereo)

ui_menu_entry_t sid_extra_sids_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereo, (ui_callback_data_t)0, NULL },
    { N_("One"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereo, (ui_callback_data_t)1, NULL },
    { N_("Two"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_SidStereo, (ui_callback_data_t)2, NULL },
    { NULL }
};

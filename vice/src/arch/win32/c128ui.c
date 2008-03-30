/*
 * c128ui.c - C128-specific user interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@axelero.hu>
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

#include "c128ui.h"
#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uireu.h"
#include "uivicii.h"
#include "vsync.h"
#include "uisnapshot.h"
#include "uilib.h"
#include "fullscreen.h"

ui_menu_toggle  c128_ui_menu_toggles[] = {
    { "DoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "DoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "IEEE488", IDM_IEEE488 },
    { "Mouse", IDM_MOUSE },
    { "Mouse", IDM_MOUSE|0x00010000 },
    { "SidFilters", IDM_TOGGLE_SIDFILTERS },
#ifdef HAVE_RESID
    { "SidUseResid", IDM_TOGGLE_SOUND_RESID },
#endif
    { "VDC_DoubleSize", IDM_TOGGLE_VDC_DOUBLESIZE },
    { "VDC_DoubleScan", IDM_TOGGLE_VDC_DOUBLESCAN },
    { "VDC_64KB", IDM_TOGGLE_VDC_64KB },
    { "InternalFunctionROM", IDM_TOGGLE_IFUNCTIONROM },
    { "ExternalFunctionROM", IDM_TOGGLE_EFUNCTIONROM },
    { NULL, 0 }
};

static ui_res_possible_values SidType[] = {
    {0, IDM_SIDTYPE_6581},
    {1, IDM_SIDTYPE_8580},
    {-1,0}
};

#ifdef HAVE_RESID
static ui_res_possible_values SidResidSampling[] = {
    {0, IDM_RESID_SAMPLE_FAST},
    {1, IDM_RESID_SAMPLE_INTERPOLATE},
    {2, IDM_RESID_SAMPLE_RESAMPLE},
    {-1,0}
};
#endif

ui_res_value_list c128_ui_res_values[] = {
    {"SidModel", SidType},
#ifdef HAVE_RESID
    {"SidResidSampling", SidResidSampling},
#endif
    {NULL,NULL}
};

void c128_ui_specific(WPARAM wparam, HWND hwnd)
{
char    *s;

    switch (wparam) {
      case IDM_RESID_SAMPLE_FAST:
        resources_set_value("SidResidSampling", (resource_value_t) 0);
        vsync_suspend_speed_eval();
        break;
      case IDM_RESID_SAMPLE_INTERPOLATE:
        resources_set_value("SidResidSampling", (resource_value_t) 1);
        vsync_suspend_speed_eval();
        break;
      case IDM_RESID_SAMPLE_RESAMPLE:
        resources_set_value("SidResidSampling", (resource_value_t) 2);
        vsync_suspend_speed_eval();
         break;
      case IDM_VICII_SETTINGS:
        ui_vicii_settings_dialog(hwnd);
        break;
      case IDM_REU_SETTINGS:
        ui_reu_settings_dialog(hwnd);
        break;
        case IDM_IFUNCTIONROM_NAME:
            SuspendFullscreenMode(hwnd);
            s = ui_select_file(hwnd,"Function ROM image",UI_LIB_FILTER_ALL,FILE_SELECTOR_DEFAULT_STYLE,NULL);
            if (s != NULL) {
                if (resources_set_value("InternalFunctionName", (resource_value_t) s) <0) {
                    ui_error("Could not load function ROM image\n'%s'", s);
                }
                free(s);
            }
            ResumeFullscreenMode(hwnd);
            break;
        case IDM_EFUNCTIONROM_NAME:
            SuspendFullscreenMode(hwnd);
            s = ui_select_file(hwnd,"Function ROM image",UI_LIB_FILTER_ALL,FILE_SELECTOR_DEFAULT_STYLE,NULL);
            if (s != NULL) {
                if (resources_set_value("ExternalFunctionName", (resource_value_t) s) <0) {
                    ui_error("Could not load function ROM image\n'%s'", s);
                }
                free(s);
            }
            ResumeFullscreenMode(hwnd);
            break;
    }
}

int c128_ui_init(void)
{
    ui_register_machine_specific(c128_ui_specific);
    ui_register_menu_toggles(c128_ui_menu_toggles);
    ui_register_res_values(c128_ui_res_values);
    return 0;
}


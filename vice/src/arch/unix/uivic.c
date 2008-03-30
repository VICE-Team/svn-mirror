/*
 * uivic.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "fullscreenarch.h"
#include "machine.h"
#include "resources.h"
#include "uimenu.h"
#include "uipalemu.h"
#include "uipalette.h"


UI_MENU_DEFINE_RADIO(MachineVideoStandard)

static ui_menu_entry_t set_video_standard_submenu[] = {
    { N_("*PAL-G"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_PAL, NULL },
    { N_("*NTSC-M"), (ui_callback_t)radio_MachineVideoStandard,
      (ui_callback_data_t)MACHINE_SYNC_NTSC, NULL },
    { NULL }
};

UI_MENU_DEFINE_STRING_RADIO(VICPaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { N_("*Default"), (ui_callback_t)radio_VICPaletteFile,
      (ui_callback_data_t)"default", NULL },
    { N_("Load custom"), (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"VICPaletteFile", NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VICDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICVideoCache)
UI_MENU_DEFINE_TOGGLE(VICExternalPalette)
#ifdef HAVE_XVIDEO
UI_MENU_DEFINE_TOGGLE(VICHwScale)
#endif
UI_MENU_DEFINE_TOGGLE(VICScale2x)
#ifdef USE_XF86_EXTENSIONS
UI_MENU_DEFINE_TOGGLE(VICFullscreen)
UI_MENU_DEFINE_STRING_RADIO(VICFullscreenDevice)
UI_MENU_DEFINE_TOGGLE(VICFullscreenDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICFullscreenDoubleScan)
#ifdef USE_XF86_VIDMODE_EXT
UI_MENU_DEFINE_RADIO(VICVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
UI_MENU_DEFINE_RADIO(VICDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
UI_MENU_DEFINE_RADIO(VICDGA2FullscreenMode);
#endif
#endif
UI_MENU_DEFINE_TOGGLE(UseXSync)

#ifdef USE_XF86_EXTENSIONS
static ui_menu_entry_t set_fullscreen_device_submenu[] = {
#ifdef USE_XF86_VIDMODE_EXT
    { "*Vidmode", (ui_callback_t)radio_VICFullscreenDevice,
      (ui_callback_data_t)"Vidmode", NULL },
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    { "*DGA2", (ui_callback_t)radio_VICFullscreenDevice,
      (ui_callback_data_t)"DGA1", NULL },
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    { "*DGA2", (ui_callback_t)radio_VICFullscreenDevice,
      (ui_callback_data_t)"DGA2", NULL },
#endif
    { NULL }
};
#endif

static UI_CALLBACK(color_set)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        int val;

        resources_get_value("VICExternalPalette", (void *)&val);

        if (val)
            ui_menu_set_sensitive(w, True);
        else
            ui_menu_set_sensitive(w, False);
    }
}

ui_menu_entry_t vic_submenu[] = {
    { N_("*Double size"),
      (ui_callback_t)toggle_VICDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_VICDoubleScan, NULL, NULL },
    { N_("*Video cache"),
      (ui_callback_t)toggle_VICVideoCache, NULL, NULL },
    { "--" },
    { N_("*External color set"),
      (ui_callback_t)toggle_VICExternalPalette, NULL, NULL },
    { N_("*Color set"),
      (ui_callback_t)color_set, NULL, palette_submenu },
    { "--" },
#ifdef HAVE_XVIDEO
    { N_("*Hardware scaling"),
      (ui_callback_t)toggle_VICHwScale, NULL, NULL },
#endif
    { N_("PAL Emulation Settings"),
      NULL, NULL, PALMode_submenu },
    { "--" },
    { N_("*Scale 2x render"),
      (ui_callback_t)toggle_VICScale2x, NULL, NULL },
    { "--" },
#ifdef USE_XF86_EXTENSIONS
    { N_("*Enable fullscreen"),
      (ui_callback_t)toggle_VICFullscreen, NULL, NULL, XK_d, UI_HOTMOD_META },
    { N_("*Double size"),
      (ui_callback_t)toggle_VICFullscreenDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_VICFullscreenDoubleScan, NULL, NULL },
    { N_("Fullscreen device"),
      NULL, NULL, set_fullscreen_device_submenu },
    /* Translators: 'VidMode', 'DGA1' and 'DGA2' must remain in the beginning
       of the translation e.g. German: "VidMode Auflösungen" */
#ifdef USE_XF86_VIDMODE_EXT
    { N_("VidMode Resolutions"),
      (ui_callback_t) NULL, NULL, NULL },
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    { N_("DGA1 Resolutions"),
      (ui_callback_t)NULL, NULL, NULL },
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    { N_("DGA2 Resolutions"),
      (ui_callback_t)NULL, NULL, NULL },
#endif
    { "--" },
#endif
    { N_("Video standard"),
      NULL, NULL, set_video_standard_submenu },
    { "--" },
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
    { NULL }
};

void uivic_menu_create(void)
{
#ifdef USE_XF86_EXTENSIONS
#ifdef USE_XF86_VIDMODE_EXT
    fullscreen_mode_callback("Vidmode",
                             (void *)radio_VICVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    fullscreen_mode_callback("DGA1",
                             (void *)radio_VICDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_callback("DGA2",
                             (void *)radio_VICDGA2FullscreenMode);
#endif
    fullscreen_menu_create(vic_submenu);
#endif
}

void uivic_menu_shutdown(void)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_menu_shutdown(vic_submenu);
#endif
}


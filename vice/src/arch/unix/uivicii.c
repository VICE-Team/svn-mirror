/*
 * uivicii.c
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

#include "c64ui.h"
#include "fullscreenarch.h"
#include "resources.h"
#include "uipalette.h"
#include "uimenu.h"
#include "uivicii.h"
#include "uipalemu.h"


extern struct ui_menu_entry_s set_video_standard_submenu[];


UI_MENU_DEFINE_STRING_RADIO(VICIIPaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { N_("*Default"), (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"default", NULL },
    { "*C64S", (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"c64s", NULL },
    { N_("*CCS64"), (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"ccs64", NULL },
    { N_("*Frodo"), (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"frodo", NULL },
    { N_("*GoDot"), (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"godot", NULL },
    { "*PC64", (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"pc64", NULL },
    { "*C64HQ", (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"c64hq", NULL },
    { "--" },
    { N_("Load custom"), (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"VICIIPaletteFile", NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VICIIDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICIIDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICIIVideoCache)
UI_MENU_DEFINE_TOGGLE(VICIINewLuminances)
UI_MENU_DEFINE_TOGGLE(VICIIExternalPalette)
#ifdef HAVE_XVIDEO
UI_MENU_DEFINE_TOGGLE(VICIIHwScale)
#endif
UI_MENU_DEFINE_TOGGLE(VICIIScale2x)
#ifdef USE_XF86_EXTENSIONS
UI_MENU_DEFINE_TOGGLE(VICIIFullscreen)
UI_MENU_DEFINE_STRING_RADIO(VICIIFullscreenDevice)
UI_MENU_DEFINE_TOGGLE(VICIIFullscreenDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICIIFullscreenDoubleScan)
#ifdef USE_XF86_VIDMODE_EXT
UI_MENU_DEFINE_RADIO(VICIIVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
UI_MENU_DEFINE_RADIO(VICIIDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
UI_MENU_DEFINE_RADIO(VICIIDGA2FullscreenMode);
#endif
#endif
UI_MENU_DEFINE_TOGGLE(VICIICheckSsColl)
UI_MENU_DEFINE_TOGGLE(VICIICheckSbColl)
UI_MENU_DEFINE_TOGGLE(UseXSync)

#ifdef USE_XF86_EXTENSIONS
static ui_menu_entry_t set_fullscreen_device_submenu[] = {
#ifdef USE_XF86_VIDMODE_EXT
    { "*Vidmode", (ui_callback_t)radio_VICIIFullscreenDevice,
      (ui_callback_data_t)"Vidmode", NULL },
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    { "*DGA1", (ui_callback_t)radio_VICIIFullscreenDevice,
      (ui_callback_data_t)"DGA1", NULL },
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    { "*DGA2", (ui_callback_t)radio_VICIIFullscreenDevice,
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

        resources_get_value("VICIIExternalPalette", (void *)&val);

        if (val)
            ui_menu_set_sensitive(w, True);
        else
            ui_menu_set_sensitive(w, False);
    }
}

ui_menu_entry_t vicii_submenu[] = {
    { N_("*Double size"),
      (ui_callback_t)toggle_VICIIDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_VICIIDoubleScan, NULL, NULL },
    { N_("*Video cache"),
      (ui_callback_t)toggle_VICIIVideoCache, NULL, NULL },
    { "--" },
    { N_("*New Luminances"),
      (ui_callback_t)toggle_VICIINewLuminances, NULL, NULL },
    { N_("*External color set"),
      (ui_callback_t)toggle_VICIIExternalPalette, NULL, NULL },
    { N_("*Color set"),
      (ui_callback_t)color_set, NULL, palette_submenu },
    { "--" },
#ifdef HAVE_XVIDEO
    { N_("*Hardware scaling"),
      (ui_callback_t)toggle_VICIIHwScale, NULL, NULL },
#endif
    { N_("PAL Emulation"),
      NULL, NULL, PALMode_submenu },
    { "--" },
    { N_("*Scale 2x render"),
      (ui_callback_t)toggle_VICIIScale2x, NULL, NULL },
    { "--" },
#ifdef USE_XF86_EXTENSIONS
    { N_("*Enable fullscreen"),
      (ui_callback_t)toggle_VICIIFullscreen, NULL, NULL, XK_d, UI_HOTMOD_META },
    { N_("*Double size"),
      (ui_callback_t)toggle_VICIIFullscreenDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_VICIIFullscreenDoubleScan, NULL, NULL },
    { N_("Fullscreen device"),
      NULL, NULL, set_fullscreen_device_submenu },
    /* Translators: 'VidMode', 'DGA1' and 'DGA2' must remain in the beginning
       of the translation e.g. German: "VidMode Auflösungen" */
#ifdef USE_XF86_VIDMODE_EXT
    { N_("VidMode Resolutions"),
      (ui_callback_t)NULL, NULL, NULL },
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
    { N_("*Sprite-sprite collisions"),
      (ui_callback_t)toggle_VICIICheckSsColl, NULL, NULL },
    { N_("*Sprite-background collisions"),
      (ui_callback_t)toggle_VICIICheckSbColl, NULL, NULL },
    { "--" },
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
    { NULL }
};

void uivicii_menu_create(void)
{
#ifdef USE_XF86_EXTENSIONS
#ifdef USE_XF86_VIDMODE_EXT
    fullscreen_mode_callback("Vidmode",
                             (void *)radio_VICIIVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    fullscreen_mode_callback("DGA1",
                             (void *)radio_VICIIDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_callback("DGA2",
                             (void *)radio_VICIIDGA2FullscreenMode);
#endif
    fullscreen_menu_create(vicii_submenu);
#endif
}

void uivicii_menu_shutdown(void)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_menu_shutdown(vicii_submenu);
#endif
}


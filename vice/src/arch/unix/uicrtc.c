/*
 * uicrtc.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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
#include "uicrtc.h"
#include "uimenu.h"
#include "uipalette.h"


UI_MENU_DEFINE_STRING_RADIO(CrtcPaletteFile)

static ui_menu_entry_t crtc_palette_submenu[] = {
    { N_("*Default (Green)"), (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"green", NULL },
    { N_("*Amber"), (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"amber", NULL },
    { N_("*White"), (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"white", NULL },
    { "--" },
    { N_("Load custom"), (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"CrtcPaletteFile", NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(CrtcDoubleSize)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleScan)
UI_MENU_DEFINE_TOGGLE(CrtcVideoCache)
UI_MENU_DEFINE_TOGGLE(CrtcScale2x)
#ifdef USE_XF86_EXTENSIONS
UI_MENU_DEFINE_TOGGLE(CrtcFullscreen)
UI_MENU_DEFINE_STRING_RADIO(CrtcFullscreenDevice)
UI_MENU_DEFINE_TOGGLE(CrtcFullscreenDoubleSize)
UI_MENU_DEFINE_TOGGLE(CrtcFullscreenDoubleScan)
#ifdef USE_XF86_VIDMODE_EXT
UI_MENU_DEFINE_RADIO(CrtcVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
UI_MENU_DEFINE_RADIO(CrtcDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
UI_MENU_DEFINE_RADIO(CrtcDGA2FullscreenMode);
#endif
#endif
UI_MENU_DEFINE_TOGGLE(UseXSync)

#ifdef USE_XF86_EXTENSIONS
static ui_menu_entry_t set_fullscreen_device_submenu[] = {
#ifdef USE_XF86_VIDMODE_EXT
    { "*Vidmode", (ui_callback_t)radio_CrtcFullscreenDevice,
      (ui_callback_data_t)"Vidmode", NULL },
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    { "*DGA1", (ui_callback_t)radio_CrtcFullscreenDevice,
      (ui_callback_data_t)"DGA1", NULL },
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    { "*DGA2", (ui_callback_t)radio_CrtcFullscreenDevice,
      (ui_callback_data_t)"DGA2", NULL },
#endif
    { NULL }
};
#endif

ui_menu_entry_t crtc_submenu[] = {
    { N_("*Double size"),
      (ui_callback_t)toggle_CrtcDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_CrtcDoubleScan, NULL, NULL },
    { N_("*Video cache"),
      (ui_callback_t)toggle_CrtcVideoCache, NULL, NULL },
    { "--" },
    { N_("*Scale 2x render"),
      (ui_callback_t)toggle_CrtcScale2x, NULL, NULL },
#ifdef USE_XF86_EXTENSIONS
    { "--" },
    { N_("*Enable fullscreen"),
      (ui_callback_t)toggle_CrtcFullscreen, NULL, NULL, XK_f, UI_HOTMOD_META },
    { N_("*Double size"),
      (ui_callback_t)toggle_CrtcFullscreenDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_CrtcFullscreenDoubleScan, NULL, NULL },
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
#endif
    { "--" },
    { N_("*CRTC Screen color"),
      NULL, NULL, crtc_palette_submenu },
    { "--" },
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
    { NULL }
};

void uicrtc_menu_create(void)
{
#ifdef USE_XF86_EXTENSIONS
#ifdef USE_XF86_VIDMODE_EXT
    fullscreen_mode_callback("Vidmode",
                             (void *)radio_CrtcVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    fullscreen_mode_callback("DGA1",
                             (void *)radio_CrtcDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_callback("DGA2",
                             (void *)radio_CrtcDGA2FullscreenMode);
#endif
    fullscreen_menu_create(crtc_submenu);
#endif
}

void uicrtc_menu_shutdown(void)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_menu_shutdown(crtc_submenu);
#endif
}


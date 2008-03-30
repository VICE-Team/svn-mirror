/*
 * uivdc.c
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
#include "uimenu.h"
#include "uipalette.h"
#include "uivdc.h"


UI_MENU_DEFINE_STRING_RADIO(VDCPaletteFile)

static ui_menu_entry_t vdc_palette_submenu[] = {
    { N_("*Default"), (ui_callback_t)radio_VDCPaletteFile,
      (ui_callback_data_t)"vdc_deft", NULL },
    { "--" },
    { N_("Load custom"), (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"VDCPaletteFile", NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(VDCRevision)

static ui_menu_entry_t set_vdc_revison_submenu[] = {
    { N_("*Revison 0"), (ui_callback_t)radio_VDCRevision,
      (ui_callback_data_t)0, NULL },
    { N_("*Revison 1"), (ui_callback_t)radio_VDCRevision,
      (ui_callback_data_t)1, NULL },
    { N_("*Revison 2"), (ui_callback_t)radio_VDCRevision,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VDCDoubleSize)
UI_MENU_DEFINE_TOGGLE(VDCDoubleScan)
UI_MENU_DEFINE_TOGGLE(VDCVideoCache)
UI_MENU_DEFINE_TOGGLE(UseXSync)
UI_MENU_DEFINE_TOGGLE(VDC64KB)
#ifdef USE_XF86_EXTENSIONS
UI_MENU_DEFINE_TOGGLE(VDCFullscreen)
UI_MENU_DEFINE_STRING_RADIO(VDCFullscreenDevice)
UI_MENU_DEFINE_TOGGLE(VDCFullscreenDoubleSize)
UI_MENU_DEFINE_TOGGLE(VDCFullscreenDoubleScan)
#ifdef USE_XF86_VIDMODE_EXT
UI_MENU_DEFINE_RADIO(VDCVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
UI_MENU_DEFINE_RADIO(VDCDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
UI_MENU_DEFINE_RADIO(VDCDGA2FullscreenMode);
#endif

static ui_menu_entry_t set_fullscreen_device_submenu[] = {
#ifdef USE_XF86_VIDMODE_EXT
    { "*Vidmode", (ui_callback_t)radio_VDCFullscreenDevice,
      (ui_callback_data_t)"Vidmode", NULL },
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    { "*DGA1", (ui_callback_t)radio_VDCFullscreenDevice,
      (ui_callback_data_t)"DGA1", NULL },
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    { "*DGA2", (ui_callback_t)radio_VDCFullscreenDevice,
      (ui_callback_data_t)"DGA2", NULL },
#endif
    { NULL }
};
#endif

ui_menu_entry_t vdc_submenu[] = {
    { N_("*Double size"),
      (ui_callback_t)toggle_VDCDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_VDCDoubleScan, NULL, NULL },
    { N_("*Video cache"),
      (ui_callback_t)toggle_VDCVideoCache, NULL, NULL },
    { "--" },
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
    { "--" },
#ifdef USE_XF86_EXTENSIONS
    { N_("*Enable fullscreen"),
      (ui_callback_t)toggle_VDCFullscreen, NULL, NULL, XK_f, UI_HOTMOD_META },
    { N_("*Double size"),
      (ui_callback_t)toggle_VDCFullscreenDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_VDCFullscreenDoubleScan, NULL, NULL },
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
      (ui_callback_t) NULL, NULL, NULL },
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    { N_("DGA2 Resolutions"),
      (ui_callback_t) NULL, NULL, NULL },
#endif
    { "--" },
#endif
    { N_("*64KB display memory"),
      (ui_callback_t)toggle_VDC64KB, NULL, NULL },
    { N_("Revision"),
      NULL, NULL, set_vdc_revison_submenu },
    { "--" },
    { N_("Color set"),
      NULL, NULL, vdc_palette_submenu },
    { NULL }
};

void uivdc_menu_create(void)
{
#ifdef USE_XF86_EXTENSIONS
#ifdef USE_XF86_VIDMODE_EXT
    fullscreen_mode_callback("Vidmode",
                             (void *)radio_VDCVidmodeFullscreenMode);
#endif
#ifdef USE_XF86_DGA1_EXTENSIONS
    fullscreen_mode_callback("DGA1",
                             (void *)radio_VDCDGA1FullscreenMode);
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_callback("DGA2",
                             (void *)radio_VDCDGA2FullscreenMode);
#endif
    fullscreen_menu_create(vdc_submenu);
#endif
}

void uivdc_menu_shutdown(void)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_menu_shutdown(vdc_submenu);
#endif
}


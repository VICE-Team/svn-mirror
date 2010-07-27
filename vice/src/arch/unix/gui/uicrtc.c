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
#include "uiapi.h"
#include "uicrtc.h"
#include "uimenu.h"
#include "uipalemu.h"
#include "uipalette.h"
#include "uirenderer.h"
#include "resources.h"
#ifdef HAVE_OPENGL_SYNC
#include <stdlib.h>             /* strtol() */
#include "openGL_sync.h"
#include "lib.h"
#endif

#include "uifullscreen-menu.h"

UI_FULLSCREEN(CRTC, KEYSYM_d)

static UI_CALLBACK(radio_CrtcPaletteFile)
{
    ui_select_palette(w, CHECK_MENUS, UI_MENU_CB_PARAM, "Crtc");
}

static ui_menu_entry_t crtc_palette_submenu[] = {
    { N_("*internal"), (ui_callback_t)radio_CrtcPaletteFile,
      NULL, NULL },
    { "--" },
    { N_("*Default (Green)"), (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"green", NULL },
    { N_("*Amber"), (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"amber", NULL },
    { N_("*White"), (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"white", NULL },
    { "--" },
    { N_("Load custom"), (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"Crtc", NULL },
    { NULL }
};

static UI_CALLBACK(radio_renderer)
{
    ui_select_renderer(w, CHECK_MENUS, vice_ptr_to_int(UI_MENU_CB_PARAM), "Crtc");
}

static ui_menu_entry_t renderer_submenu[] = {
    { N_("*unfiltered"), (ui_callback_t)radio_renderer,
      (ui_callback_data_t)0, NULL },
    { N_("*CRT Emulation"), (ui_callback_t)radio_renderer,
      (ui_callback_data_t)1, NULL },
#if 0
    { N_("*Scale 2x"), (ui_callback_t)radio_renderer,
      (ui_callback_data_t)2, NULL },
#endif
    { NULL }
};

#define NOTHING(x) x

UI_MENU_DEFINE_TOGGLE(CrtcDoubleSize)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleScan)
UI_MENU_DEFINE_TOGGLE(CrtcVideoCache)

#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE_COND(CrtcHwScale, HwScalePossible, NOTHING)
#endif

#ifdef HAVE_OPENGL_SYNC
UI_MENU_DEFINE_TOGGLE_COND(openGL_sync, openGL_no_sync, openGL_available)

static UI_CALLBACK(openGL_set_desktoprefresh)
{
    if (!CHECK_MENUS) {
        float f;
        char *buf = lib_calloc(sizeof(char), 10);

        sprintf(buf, "%.0f", openGL_get_canvas_refreshrate());
        ui_input_string(_("Refreshrate: "), _("Enter Refreshrate (Hz): "), buf, 10);
        f = (float) strtol(buf, NULL, 10);
        openGL_set_canvas_refreshrate(f);
        lib_free(buf);
    } else {
        if (openGL_available(0) && openGL_sync_enabled()) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}
#endif

#ifndef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(UseXSync)
#endif

#ifdef HAVE_HWSCALE
#ifdef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(KeepAspectRatio)
UI_MENU_DEFINE_TOGGLE(TrueAspectRatio)
#endif
#endif

ui_menu_entry_t crtc_submenu[] = {
    { N_("*Double size"),
      (ui_callback_t)toggle_CrtcDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_CrtcDoubleScan, NULL, NULL },
    { N_("*Video cache"),
      (ui_callback_t)toggle_CrtcVideoCache, NULL, NULL },
    { "--" },
    { N_("Color set"),
      NULL, NULL, crtc_palette_submenu },
    { "--" },
    { N_("Renderer"),
      NULL, NULL, renderer_submenu },
#ifndef USE_GNOMEUI
    { N_("CRT Emulation Settings"),
      NULL, NULL, PALMode_submenu },
#endif
#ifdef HAVE_HWSCALE
    { "--" },
    { N_("*Hardware scaling"),
      (ui_callback_t)toggle_CrtcHwScale, NULL, NULL },
#ifdef USE_GNOMEUI
    { N_("*Keep aspect ratio"),
      (ui_callback_t)toggle_KeepAspectRatio, NULL, NULL },
    { N_("*True aspect ratio"),
      (ui_callback_t)toggle_TrueAspectRatio, NULL, NULL },
#endif
#endif
#ifdef HAVE_OPENGL_SYNC
    { "--" },
    { N_("*OpenGL Rastersynchronization"),
      (ui_callback_t)toggle_openGL_sync, NULL, NULL },
    { N_("Desktop Refreshrate..."),
      (ui_callback_t)openGL_set_desktoprefresh, NULL, NULL },
#endif
#ifdef HAVE_FULLSCREEN
    { "--" },
    { N_("*Fullscreen settings"), NULL, NULL, fullscreen_menuCRTC },
#endif    
#ifndef USE_GNOMEUI
    { "--" },
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
#endif
    { NULL }
};

void uicrtc_menu_create(void)
{
    UI_FULLSCREEN_MENU_CREATE(CRTC)
}

void uicrtc_menu_shutdown(void)
{
    UI_FULLSCREEN_MENU_SHUTDOWN(CRTC)
}

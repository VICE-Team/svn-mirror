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
    { N_("Internal"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CrtcPaletteFile,
      NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Default (Green)"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"green", NULL },
    { N_("Amber"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"amber", NULL },
    { N_("White"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_CrtcPaletteFile,
      (ui_callback_data_t)"white", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load custom"), UI_MENU_TYPE_NORMAL, (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"Crtc", NULL },
    { NULL }
};

static UI_CALLBACK(radio_renderer)
{
    ui_select_renderer(w, CHECK_MENUS, vice_ptr_to_int(UI_MENU_CB_PARAM), "Crtc");
}

static ui_menu_entry_t renderer_submenu[] = {
    { N_("Unfiltered"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)0, NULL },
    { N_("CRT emulation"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)1, NULL },
#if 0
    { N_("Scale2x"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
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
UI_MENU_DEFINE_TOGGLE(KeepAspectRatio)
UI_MENU_DEFINE_TOGGLE(TrueAspectRatio)
#ifndef USE_GNOMEUI
extern UI_CALLBACK(set_custom_aspect_ratio);
#endif /* USE_GNOMEUI */
#endif /* HAVE_HWSCALE */

ui_menu_entry_t crtc_submenu[] = {
    { N_("Double size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CrtcDoubleSize, NULL, NULL },
    { N_("Double scan"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CrtcDoubleScan, NULL, NULL },
    { N_("Video cache"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CrtcVideoCache, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Colors"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, crtc_palette_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Renderer"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, renderer_submenu },
#ifndef USE_GNOMEUI
    { N_("CRT emulation settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, PALMode_submenu },
#endif
#ifdef HAVE_HWSCALE
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Hardware scaling"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_CrtcHwScale, NULL, NULL },
    { N_("Keep aspect ratio"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeepAspectRatio, NULL, NULL },
    { N_("True aspect ratio"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TrueAspectRatio, NULL, NULL },
#ifndef USE_GNOMEUI
    { N_("Set custom aspect ratio"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_custom_aspect_ratio,
      (ui_callback_data_t)"AspectRatio", NULL },
#endif /* USE_GNOMEUI */
#endif /* HAVE_HWSCALE */
#ifdef HAVE_OPENGL_SYNC
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("OpenGL Rastersynchronization"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_openGL_sync, NULL, NULL },
    { N_("Desktop Refreshrate"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)openGL_set_desktoprefresh, NULL, NULL },
#endif
#ifdef HAVE_FULLSCREEN
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Fullscreen settings"), UI_MENU_TYPE_NORMAL, NULL, NULL, fullscreen_menuCRTC },
#endif
#ifndef USE_GNOMEUI
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Use XSync()"), UI_MENU_TYPE_TICK,
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

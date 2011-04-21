/*
 * uited.c
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
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uimenu.h"
#include "uipalemu.h"
#include "uipalette.h"
#include "uirenderer.h"
#include "uited.h"
#include "uifullscreen-menu.h"
#include "util.h"

#ifdef HAVE_OPENGL_SYNC
#include <stdlib.h>             /* strtol() */
#include "openGL_sync.h"
#endif

UI_FULLSCREEN(TED, KEYSYM_d)

static UI_CALLBACK(radio_TEDPaletteFile)
{
    ui_select_palette(w, CHECK_MENUS, UI_MENU_CB_PARAM, "TED");
}

static ui_menu_entry_t palette_submenu[] = {
    { N_("Internal"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_TEDPaletteFile,
      NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Default"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_TEDPaletteFile,
      (ui_callback_data_t)"default", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load custom"), UI_MENU_TYPE_NORMAL, (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"TED", NULL },
    { NULL }
};

static UI_CALLBACK(radio_renderer)
{
    ui_select_renderer(w, CHECK_MENUS, vice_ptr_to_int(UI_MENU_CB_PARAM), "TED");
}

static ui_menu_entry_t renderer_submenu[] = {
    { N_("Unfiltered"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)0, NULL },
    { N_("CRT emulation"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)1, NULL },
    { N_("Scale2x"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(TEDDoubleSize)
UI_MENU_DEFINE_TOGGLE(TEDDoubleScan)
UI_MENU_DEFINE_TOGGLE(TEDVideoCache)

#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE(TEDHwScale)
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
#endif

#ifdef HAVE_OPENGL_SYNC
UI_MENU_DEFINE_TOGGLE_COND(openGL_sync, openGL_no_sync, openGL_available)

static UI_CALLBACK(openGL_set_desktoprefresh)
{
    char *enter_refresh_rate = util_concat(_("Enter refresh rate"), " (Hz): ", NULL);
    char *refresh_rate = util_concat(_("Refresh rate"), ": ", NULL);

    if (!CHECK_MENUS) {
        float f;
        char *buf = lib_calloc(sizeof(char), 10);

        sprintf(buf, "%.0f", openGL_get_canvas_refreshrate());
        ui_input_string(refresh_rate, enter_refresh_rate, buf, 10);
        f = (float) strtol(buf, NULL, 10);
        openGL_set_canvas_refreshrate(f);
        lib_free(buf);
        lib_free(enter_refresh_rate);
        lib_free(refresh_rate);
    } else {
        if (openGL_available(0) && openGL_sync_enabled()) {
            ui_menu_set_sensitive(w, 1);
        } else {
            ui_menu_set_sensitive(w, 0);
        }
    }
}
#endif

ui_menu_entry_t ted_submenu[] = {
    { N_("Double size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TEDDoubleSize, NULL, NULL },
    { N_("Double scan"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TEDDoubleScan, NULL, NULL },
    { N_("Video cache"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TEDVideoCache, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Colors"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, palette_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Render filter"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, renderer_submenu },
#ifndef USE_GNOMEUI
    { N_("CRT emulation settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, PALMode_submenu },
#endif
#ifdef HAVE_HWSCALE
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Hardware scaling"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TEDHwScale, NULL, NULL },
    { N_("Keep aspect ratio"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeepAspectRatio, NULL, NULL },
    { N_("True aspect ratio"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TrueAspectRatio, NULL, NULL },
#ifndef USE_GNOMEUI
    { N_("Set custom aspect ratio"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_custom_aspect_ratio,
      (ui_callback_data_t)"AspectRatio", NULL },
#endif /* USE_GNOMEUI */
#endif
#ifdef HAVE_OPENGL_SYNC
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("OpenGL Rastersynchronization"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_openGL_sync, NULL, NULL },
    { N_("Desktop Refreshrate"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)openGL_set_desktoprefresh, NULL, NULL },
#endif
#ifdef HAVE_FULLSCREEN
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Fullscreen settings"), UI_MENU_TYPE_NORMAL, NULL, NULL, fullscreen_menuTED },
#endif
#ifndef USE_GNOMEUI
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Use XSync()"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
#endif
    { NULL }
};

void uited_menu_create(void)
{
    UI_FULLSCREEN_MENU_CREATE(TED)
}

void uited_menu_shutdown(void)
{
    UI_FULLSCREEN_MENU_SHUTDOWN(TED);
}

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
#include <string.h>

#include "c64ui.h"
#include "fullscreenarch.h"
#include "resources.h"
#include "uiapi.h"
#include "uipalette.h"
#include "uimenu.h"
#include "uivicii.h"
#include "uipalemu.h"
#include "uirenderer.h"
#include "vicii.h"

#ifdef HAVE_OPENGL_SYNC
#include <stdlib.h>		/* strtol() */
#include "openGL_sync.h"
#include "lib.h"
#endif

#include "uifullscreen-menu.h"
UI_FULLSCREEN(VICII, KEYSYM_d)

static UI_CALLBACK(radio_VICIIPaletteFile)
{
    ui_select_palette(w, CHECK_MENUS, UI_MENU_CB_PARAM, "VICII");
}

static ui_menu_entry_t palette_submenu[] = {
    { N_("Internal"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Default"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"default", NULL },
    { "VICE", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"vice", NULL },
    { "C64S", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"c64s", NULL },
    { "CCS64", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"ccs64", NULL },
    { "Frodo", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"frodo", NULL },
    { "GoDot", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"godot", NULL },
    { "PC64", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"pc64", NULL },
    { "C64HQ", UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIPaletteFile,
      (ui_callback_data_t)"c64hq", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load custom"), UI_MENU_TYPE_NORMAL, (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"VICII", NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(VICIIBorderMode)

static ui_menu_entry_t bordermode_submenu[] = {
    { N_("Normal"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIBorderMode,
      (ui_callback_data_t)VICII_NORMAL_BORDERS, NULL },
    { N_("Full"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIBorderMode,
      (ui_callback_data_t)VICII_FULL_BORDERS, NULL },
    { N_("Debug"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VICIIBorderMode,
      (ui_callback_data_t)VICII_DEBUG_BORDERS, NULL },
    { NULL }
};

static UI_CALLBACK(radio_renderer)
{
    ui_select_renderer(w, CHECK_MENUS, vice_ptr_to_int(UI_MENU_CB_PARAM), "VICII");
}

static ui_menu_entry_t renderer_submenu[] = {
    { N_("Unfiltered"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)0, NULL },
    { N_("CRT Emulation"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)1, NULL },
    { N_("Scale 2x"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

#define NOTHING(x) x

UI_MENU_DEFINE_TOGGLE(VICIIDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICIIDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICIIVideoCache)
UI_MENU_DEFINE_TOGGLE(VICIINewLuminances)

#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE_COND(VICIIHwScale, HwScalePossible, NOTHING)
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

UI_MENU_DEFINE_TOGGLE(VICIICheckSsColl)
UI_MENU_DEFINE_TOGGLE(VICIICheckSbColl)

#ifndef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(UseXSync)
#endif

#ifdef HAVE_HWSCALE
#ifdef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(KeepAspectRatio)
#endif
UI_MENU_DEFINE_TOGGLE(TrueAspectRatio)
#endif

ui_menu_entry_t vicii_submenu[] = {
    { N_("Double size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VICIIDoubleSize, NULL, NULL },
    { N_("Double scan"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VICIIDoubleScan, NULL, NULL },
    { N_("Video cache"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VICIIVideoCache, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("New Luminances"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VICIINewLuminances, NULL, NULL },
    { N_("Color set"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, palette_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Renderer"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, renderer_submenu },
#ifndef USE_GNOMEUI
    { N_("CRT Emulation Settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, PALMode_submenu },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Border mode"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, bordermode_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Sprite-sprite collisions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VICIICheckSsColl, NULL, NULL },
    { N_("Sprite-background collisions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VICIICheckSbColl, NULL, NULL },
#ifdef HAVE_HWSCALE
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Hardware scaling"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VICIIHwScale, NULL, NULL },
#ifdef USE_GNOMEUI
    { N_("Keep aspect ratio"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeepAspectRatio, NULL, NULL },
#endif
    { N_("True aspect ratio"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_TrueAspectRatio, NULL, NULL },
#endif
#ifdef HAVE_OPENGL_SYNC
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("OpenGL Rastersynchronization"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_openGL_sync, NULL, NULL },
    { N_("Desktop Refreshrate..."), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)openGL_set_desktoprefresh, NULL, NULL },
#endif
#ifdef HAVE_FULLSCREEN
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Fullscreen settings"), UI_MENU_TYPE_NORMAL, NULL, NULL, fullscreen_menuVICII },
#endif
#ifndef USE_GNOMEUI
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Use XSync()"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
#endif
    { NULL }
};

void uivicii_menu_create(void)
{
    UI_FULLSCREEN_MENU_CREATE(VICII)
}

void uivicii_menu_shutdown(void)
{
    UI_FULLSCREEN_MENU_SHUTDOWN(VICII)
}

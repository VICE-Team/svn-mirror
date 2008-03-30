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
#include "vicii.h"
#include "openGL_sync.h"

extern struct ui_menu_entry_s set_video_standard_submenu[];

#include "uifullscreen-menu.h"
UI_FULLSCREEN(VICII)

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

UI_MENU_DEFINE_RADIO(VICIIBorderMode)

static ui_menu_entry_t bordermode_submenu[] = {
    { N_("*Normal"), (ui_callback_t)radio_VICIIBorderMode,
      (ui_callback_data_t)VICII_NORMAL_BORDERS, NULL },
    { N_("*Full"), (ui_callback_t)radio_VICIIBorderMode,
      (ui_callback_data_t)VICII_FULL_BORDERS, NULL },
    { N_("*Debug"), (ui_callback_t)radio_VICIIBorderMode,
      (ui_callback_data_t)VICII_DEBUG_BORDERS, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VICIIDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICIIDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICIIVideoCache)
UI_MENU_DEFINE_TOGGLE(VICIINewLuminances)
UI_MENU_DEFINE_TOGGLE(VICIIExternalPalette)
#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE(VICIIHwScale)
#endif
UI_MENU_DEFINE_TOGGLE(VICIIScale2x)
#ifdef HAVE_OPENGL_SYNC
UI_MENU_DEFINE_TOGGLE_COND(openGL_sync, openGL_no_sync, openGL_available)
#endif
UI_MENU_DEFINE_TOGGLE(VICIICheckSsColl)
UI_MENU_DEFINE_TOGGLE(VICIICheckSbColl)
#ifndef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(UseXSync)
#endif

static UI_CALLBACK(color_set)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        int val;

        resources_get_int("VICIIExternalPalette", &val);

        if (val)
            ui_menu_set_sensitive(w, 1);
        else
            ui_menu_set_sensitive(w, 0);
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
    { N_("PAL Emulation"),
      NULL, NULL, PALMode_submenu },
    { N_("*Scale 2x render"),
      (ui_callback_t)toggle_VICIIScale2x, NULL, NULL },
    { "--" },
    { N_("Video standard"),
      NULL, NULL, set_video_standard_submenu },
    { "--" },
    { N_("*Border mode"),
      NULL, NULL, bordermode_submenu },
    { "--" },
    { N_("*Sprite-sprite collisions"),
      (ui_callback_t)toggle_VICIICheckSsColl, NULL, NULL },
    { N_("*Sprite-background collisions"),
      (ui_callback_t)toggle_VICIICheckSbColl, NULL, NULL },
    { "--" },
#ifdef HAVE_HWSCALE
    { N_("*Hardware scaling"),
      (ui_callback_t)toggle_VICIIHwScale, NULL, NULL },
#endif
#ifdef HAVE_OPENGL_SYNC
    { N_("*OpenGL Rastersynchronization"),
      (ui_callback_t)toggle_openGL_sync, NULL, NULL },
#endif
#ifdef HAVE_FULLSCREEN
    { N_("*Fullscreen settings"), NULL, NULL, fullscreen_menuVICII },
#endif
#ifndef USE_GNOMEUI
    { N_("*Use XSync()"),
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

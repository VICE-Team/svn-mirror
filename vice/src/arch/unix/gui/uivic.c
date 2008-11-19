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
#include "openGL_sync.h"

#include "uifullscreen-menu.h"
UI_FULLSCREEN(VIC, KEYSYM_d)

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
#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE(VICHwScale)
#endif
UI_MENU_DEFINE_TOGGLE(VICScale2x)
#ifdef HAVE_OPENGL_SYNC
UI_MENU_DEFINE_TOGGLE_COND(openGL_sync, openGL_no_sync, openGL_available)
#endif
#ifndef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(UseXSync)
#endif

static UI_CALLBACK(color_set)
{
    if (!CHECK_MENUS) {
        ui_update_menus();
    } else {
        int val;

        resources_get_int("VICExternalPalette", &val);

        if (val)
            ui_menu_set_sensitive(w, 1);
        else
            ui_menu_set_sensitive(w, 0);
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
    { N_("PAL Emulation Settings"),
      NULL, NULL, PALMode_submenu },
    { N_("*Scale 2x render"),
      (ui_callback_t)toggle_VICScale2x, NULL, NULL },
    { "--" },
    { N_("Video standard"),
      NULL, NULL, set_video_standard_submenu },
    { "--" },
#ifdef HAVE_HWSCALE
    { N_("*Hardware scaling"),
      (ui_callback_t)toggle_VICHwScale, NULL, NULL },
#endif
#ifdef HAVE_OPENGL_SYNC
    { N_("*OpenGL Rastersynchronization"),
      (ui_callback_t)toggle_openGL_sync, NULL, NULL },
#endif
#ifdef HAVE_FULLSCREEN
    { N_("*Fullscreen settings"), NULL, NULL, fullscreen_menuVIC },
#endif
#ifndef USE_GNOMEUI
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
#endif
    { NULL }
};

void uivic_menu_create(void)
{
    UI_FULLSCREEN_MENU_CREATE(VIC)
}

void uivic_menu_shutdown(void)
{
    UI_FULLSCREEN_MENU_SHUTDOWN(VIC);
}


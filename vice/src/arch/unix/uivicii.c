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
#include "resources.h"
#include "uipalette.h"
#include "uimenu.h"
#include "uivicii.h"
#include "utils.h"
#include "uipalemu.h"


UI_MENU_DEFINE_STRING_RADIO(PaletteFile)

static ui_menu_entry_t palette_submenu[] = {
    { N_("*Default"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"default", NULL },
    { "*C64S",
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"c64s", NULL },
    { N_("*CCS64"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"ccs64", NULL },
    { N_("*Frodo"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"frodo", NULL },
    { N_("*GoDot"),
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"godot", NULL },
    { "*PC64",
      (ui_callback_t)radio_PaletteFile, (ui_callback_data_t)"pc64", NULL },
    { "--" },
    { N_("Load custom"),
      (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"PaletteFile", NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VICIIDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICIIVideoCache)
UI_MENU_DEFINE_TOGGLE(VICIICheckSsColl)
UI_MENU_DEFINE_TOGGLE(VICIICheckSbColl)
UI_MENU_DEFINE_TOGGLE(VICIINewLuminances)
UI_MENU_DEFINE_TOGGLE(ExternalPalette)

ui_menu_entry_t vic_submenu[] = {
    { N_("*Double scan"),
      (ui_callback_t)toggle_VICIIDoubleScan, NULL, NULL },
    { N_("*Video cache"),
      (ui_callback_t)toggle_VICIIVideoCache, NULL, NULL },
    { "--" },
    { N_("Video standard"),
      NULL, NULL, set_video_standard_submenu },
    { "--" },
    { N_("*Sprite-sprite collisions"),
      (ui_callback_t)toggle_VICIICheckSsColl, NULL, NULL },
    { N_("*Sprite-background collisions"),
      (ui_callback_t)toggle_VICIICheckSbColl, NULL, NULL },
    { "--" },
    { N_("*New Luminances"),
      (ui_callback_t)toggle_VICIINewLuminances, NULL, NULL },
    { N_("*External color set"),
      (ui_callback_t)toggle_ExternalPalette, NULL, NULL },
    { N_("Color set"),
      NULL, NULL, palette_submenu },
    { "--" },
    { N_("PAL Emulation"),
      NULL, NULL, PALMode_submenu },
    { NULL }
};


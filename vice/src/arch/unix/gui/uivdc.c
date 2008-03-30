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

#include "uifullscreen-menu.h"
UI_FULLSCREEN(VDC)

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
#ifndef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(UseXSync)
#endif
UI_MENU_DEFINE_TOGGLE(VDC64KB)

ui_menu_entry_t vdc_submenu[] = {
    { N_("*Double size"),
      (ui_callback_t)toggle_VDCDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_VDCDoubleScan, NULL, NULL },
    { N_("*Video cache"),
      (ui_callback_t)toggle_VDCVideoCache, NULL, NULL },
    { "--" },
#ifdef HAVE_FULLSCREEN
    { N_("*Fullscreen settings"), NULL, NULL, fullscreen_menuVDC },
#endif
#ifndef USE_GNOMEUI
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
#endif
    { "--" },
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
    UI_FULLSCREEN_MENU_CREATE(VDC)
}

void uivdc_menu_shutdown(void)
{
    UI_FULLSCREEN_MENU_SHUTDOWN(VDC)
}

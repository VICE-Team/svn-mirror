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
#include "resources.h"
#include "uiapi.h"
#include "uimenu.h"
#include "uipalemu.h"
#include "uipalette.h"
#include "uirenderer.h"
#include "uivdc.h"
#include "uifullscreen-menu.h"
#ifdef HAVE_OPENGL_SYNC
#include <stdlib.h>             /* strtol() */
#include "openGL_sync.h"
#include "lib.h"
#endif

UI_FULLSCREEN(VDC, KEYSYM_f)

static UI_CALLBACK(radio_VDCPaletteFile)
{
    ui_select_palette(w, CHECK_MENUS, UI_MENU_CB_PARAM, "VDC");
}

static ui_menu_entry_t vdc_palette_submenu[] = {
    { N_("Internal"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VDCPaletteFile,
      NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Default"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VDCPaletteFile,
      (ui_callback_data_t)"vdc_deft", NULL },
    { N_("Composite"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VDCPaletteFile,
      (ui_callback_data_t)"vdc_comp", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Load custom"), UI_MENU_TYPE_NORMAL, (ui_callback_t)ui_load_palette,
      (ui_callback_data_t)"VDC", NULL },
    { NULL }
};

static UI_CALLBACK(radio_renderer)
{
    ui_select_renderer(w, CHECK_MENUS, vice_ptr_to_int(UI_MENU_CB_PARAM), "VDC");
}

static ui_menu_entry_t renderer_submenu[] = {
    { N_("Unfiltered"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)0, NULL },
    { N_("CRT Emulation"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)1, NULL },
#if 0
    { N_("Scale2x"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_renderer,
      (ui_callback_data_t)2, NULL },
#endif
    { NULL }
};

UI_MENU_DEFINE_RADIO(VDCRevision)

static ui_menu_entry_t set_vdc_revison_submenu[] = {
    { N_("Revison 0"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VDCRevision,
      (ui_callback_data_t)0, NULL },
    { N_("Revison 1"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VDCRevision,
      (ui_callback_data_t)1, NULL },
    { N_("Revison 2"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_VDCRevision,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

#define NOTHING(x) x

UI_MENU_DEFINE_TOGGLE(VDCDoubleSize)
UI_MENU_DEFINE_TOGGLE(VDCDoubleScan)
UI_MENU_DEFINE_TOGGLE(VDCVideoCache)

#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE_COND(VDCHwScale, HwScalePossible, NOTHING)
#endif

#ifndef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(UseXSync)
#endif

UI_MENU_DEFINE_TOGGLE(VDC64KB)

#ifdef HAVE_HWSCALE
#ifdef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(KeepAspectRatio)
#endif
UI_MENU_DEFINE_TOGGLE(TrueAspectRatio)
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

ui_menu_entry_t set_vdcmodel_submenu[] = {
    { N_("64kB display memory"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VDC64KB, NULL, NULL },
    { N_("Revision"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_vdc_revison_submenu },
    { NULL }
};

ui_menu_entry_t vdc_submenu[] = {
    { N_("Double size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VDCDoubleSize, NULL, NULL },
    { N_("Double scan"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VDCDoubleScan, NULL, NULL },
    { N_("Video cache"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VDCVideoCache, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Color set"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, vdc_palette_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Renderer"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, renderer_submenu },
#ifndef USE_GNOMEUI
    { N_("CRT Emulation Settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, PALMode_submenu },
#endif
#ifdef HAVE_HWSCALE
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Hardware scaling"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_VDCHwScale, NULL, NULL },
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
    { N_("Fullscreen settings"), UI_MENU_TYPE_NORMAL, NULL, NULL, fullscreen_menuVDC },
#endif
#ifndef USE_GNOMEUI
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Use XSync()"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
#endif
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

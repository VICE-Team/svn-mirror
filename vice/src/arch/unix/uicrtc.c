/*
 * uicrtc.c
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "uimenu.h"
#include "uisettings.h"

UI_MENU_DEFINE_TOGGLE(CrtcVideoCache)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleSize)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleScan)
UI_MENU_DEFINE_TOGGLE(UseXSync)

static ui_menu_entry_t crtc_video_settings_submenu[] = {
    { N_("*Video cache"),
      (ui_callback_t)toggle_CrtcVideoCache, NULL, NULL },
    { N_("*Double size"),
      (ui_callback_t)toggle_CrtcDoubleSize, NULL, NULL },
    { N_("*Double scan"),
      (ui_callback_t)toggle_CrtcDoubleScan, NULL, NULL },
    { N_("*Use XSync()"),
      (ui_callback_t)toggle_UseXSync, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_crtc_video_settings_menu[] = {
    { N_("Crtc Video settings"),
      NULL, NULL, crtc_video_settings_submenu },
    { NULL }
};


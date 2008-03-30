/*
 * uisidcart.c
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "sidcart.h"
#include "uilib.h"
#include "uimenu.h"
#include "uisidcart.h"

UI_MENU_DEFINE_RADIO(SidEngine)

ui_menu_entry_t sidcart_engine_submenu[] = {
    { N_("*Fast SID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_FASTSID, NULL },
#ifdef HAVE_CATWEASELMKIII
    { N_("*Catweasel MKIII"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_CATWEASELMKIII, NULL },
#endif
#ifdef HAVE_HARDSID
    { N_("*HardSID"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_HARDSID, NULL },
#endif
#ifdef HAVE_PARSID
    { N_("*ParSID Port 1"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT1, NULL },
    { N_("*ParSID Port 2"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT2, NULL },
    { N_("*ParSID Port 3"), (ui_callback_t)radio_SidEngine,
      (ui_callback_data_t)SID_ENGINE_PARSID_PORT3, NULL },
#endif
    { NULL }
};

UI_MENU_DEFINE_RADIO(SidModel)

ui_menu_entry_t sidcart_model_submenu[] = {
    { N_("*6581 (old)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)0, NULL },
    { N_("*8580 (new)"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)1, NULL },
    { N_("*8580 + digi boost"),
      (ui_callback_t)radio_SidModel, (ui_callback_data_t)2, NULL },
    { NULL }
};

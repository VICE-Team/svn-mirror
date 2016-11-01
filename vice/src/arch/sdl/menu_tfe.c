/*
 * menu_tfe.c - TFE menu for SDL UI.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#ifdef HAVE_PCAP

#include <stdio.h>

#include "types.h"

#include "lib.h"
#include "menu_common.h"
#include "menu_tfe.h"
#include "rawnet.h"
#include "resources.h"
#include "uimenu.h"

/* Common menus */

UI_MENU_DEFINE_TOGGLE(TFE_ACTIVE)
UI_MENU_DEFINE_TOGGLE(TFEIOSwap)

const ui_menu_entry_t tfe_menu[] = {
    { "TFE emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TFE_ACTIVE_callback,
      NULL },
    SDL_MENU_LIST_END
};

const ui_menu_entry_t tfe20_menu[] = {
    { "TFE emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TFE_ACTIVE_callback,
      NULL },
    { "Swap TFE I/O",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TFEIOSwap_callback,
      NULL },
    SDL_MENU_LIST_END
};

#endif /* HAVE_PCAP */

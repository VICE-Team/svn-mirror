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

#ifdef HAVE_TFE

#include <stdio.h>

#include "types.h"

#include "menu_common.h"
#include "menu_tfe.h"
#include "resources.h"
#include "uimenu.h"


/* *nix TFE settings */
#if defined(UNIX_COMPILE)
UI_MENU_DEFINE_STRING(ETHERNET_INTERFACE)

#define VICE_SDL_TFE_ARCHDEP_ITEMS \
    { "Interface", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_ETHERNET_INTERFACE_callback, \
      (ui_callback_data_t)"Ethernet interface" },

#endif /* defined(UNIX_COMPILE) */


/* win32 TFE settings */
#if defined(WIN32_COMPILE) && !defined(__XBOX__)
/* FIXME proper submenu with available devices */
UI_MENU_DEFINE_STRING(ETHERNET_INTERFACE)

#define VICE_SDL_TFE_ARCHDEP_ITEMS \
    { "Interface", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_ETHERNET_INTERFACE_callback, \
      (ui_callback_data_t)"Ethernet interface" },

#endif /* defined(WIN32_COMPILE) && !defined(__XBOX__) */


/* Common menus */

UI_MENU_DEFINE_TOGGLE(ETHERNET_ACTIVE)
UI_MENU_DEFINE_TOGGLE(ETHERNET_AS_RR)

static UI_MENU_CALLBACK(show_ETHERNET_DISABLED_callback)
{
    int value;

    resources_get_int("ETHERNET_DISABLED", &value);

    return value ? "(disabled)" : NULL;
}

const ui_menu_entry_t tfe_menu[] = {
    { "Ethernet support",
      MENU_ENTRY_TEXT,
      show_ETHERNET_DISABLED_callback,
      (ui_callback_data_t)1 },
    { "Ethernet emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_ETHERNET_ACTIVE_callback,
      NULL },
    { "RR-NET compatibility mode",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_ETHERNET_AS_RR_callback,
      NULL },
    VICE_SDL_TFE_ARCHDEP_ITEMS
    { NULL }
};

#endif /* HAVE_TFE */

/*
 * menu_midi.c - MIDI menu for SDL UI.
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

#ifdef HAVE_MIDI

#include <stdio.h>

#include "types.h"

#include "menu_common.h"
#include "menu_midi.h"
#include "uimenu.h"


/* *nix MIDI settings */
#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
UI_MENU_DEFINE_STRING(MIDIInDev)
UI_MENU_DEFINE_STRING(MIDIOutDev)
UI_MENU_DEFINE_RADIO(MIDIDriver)

static const ui_menu_entry_t midi_driver_menu[] = {
    { "OSS",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MIDIDriver_callback,
      (ui_callback_data_t)0 },
#ifdef USE_ALSA
    { "ALSA",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MIDIDriver_callback,
      (ui_callback_data_t)1 },
#endif
    { NULL }
};

#define VICE_SDL_MIDI_ARCHDEP_ITEMS \
    SDL_MENU_ITEM_SEPARATOR, \
    { "Driver", \
      MENU_ENTRY_SUBMENU, \
      submenu_radio_callback, \
      (ui_callback_data_t)midi_driver_menu }, \
    SDL_MENU_ITEM_SEPARATOR, \
    SDL_MENU_ITEM_TITLE("OSS driver devices"), \
    { "MIDI-In", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_MIDIInDev_callback, \
      (ui_callback_data_t)"MIDI-In device" }, \
    { "MIDI-Out", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_MIDIOutDev_callback, \
      (ui_callback_data_t)"MIDI-Out device" },

#endif /* defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT) */


/* OSX MIDI settings */
#if defined(MACOSX_SUPPORT)
UI_MENU_DEFINE_STRING(MIDIName)
UI_MENU_DEFINE_STRING(MIDIInName)
UI_MENU_DEFINE_STRING(MIDIOutName)

#define VICE_SDL_MIDI_ARCHDEP_ITEMS \
    SDL_MENU_ITEM_SEPARATOR, \
    { "Client name", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_MIDIName_callback, \
      (ui_callback_data_t)"Name of MIDI client" }, \
    { "MIDI-In", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_MIDIInName_callback, \
      (ui_callback_data_t)"Name of MIDI-In Port" }, \
    { "MIDI-Out", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_MIDIOutName_callback, \
      (ui_callback_data_t)"Name of MIDI-Out Port" },

#endif /* defined(MACOSX_SUPPORT) */


/* win32 MIDI settings */
#if defined(WIN32_COMPILE) && !defined(__XBOX__)
/* FIXME proper submenu with available devices */
UI_MENU_DEFINE_INT(MIDIInDev)
UI_MENU_DEFINE_INT(MIDIOutDev)

#define VICE_SDL_MIDI_ARCHDEP_ITEMS \
    SDL_MENU_ITEM_SEPARATOR, \
    { "MIDI-In", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_MIDIInDev_callback, \
      (ui_callback_data_t)"MIDI-In device" }, \
    { "MIDI-Out", \
      MENU_ENTRY_RESOURCE_STRING, \
      string_MIDIOutDev_callback, \
      (ui_callback_data_t)"MIDI-Out device" },

#endif /* defined(WIN32_COMPILE) && !defined(__XBOX__) */


/* Common menus */

UI_MENU_DEFINE_TOGGLE(MIDIEnable)
UI_MENU_DEFINE_RADIO(MIDIMode)

static const ui_menu_entry_t midi_type_menu[] = {
    { "Sequential",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MIDIMode_callback,
      (ui_callback_data_t)0 },
    { "Passport",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MIDIMode_callback,
      (ui_callback_data_t)1 },
    { "DATEL",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MIDIMode_callback,
      (ui_callback_data_t)2 },
    { "Namesoft",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MIDIMode_callback,
      (ui_callback_data_t)3 },
    { "Maplin",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MIDIMode_callback,
      (ui_callback_data_t)4 },
    { NULL }
};

const ui_menu_entry_t midi_c64_menu[] = {
    { "MIDI emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MIDIEnable_callback,
      NULL },
    { "MIDI cart type",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)midi_type_menu },
    VICE_SDL_MIDI_ARCHDEP_ITEMS
    { NULL }
};

const ui_menu_entry_t midi_vic20_menu[] = {
    { "MIDI emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_MIDIEnable_callback,
      NULL },
    VICE_SDL_MIDI_ARCHDEP_ITEMS
    { NULL }
};

#endif /* HAVE_MIDI */

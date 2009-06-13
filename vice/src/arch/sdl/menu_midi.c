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

#include <windows.h>
#include <mmsystem.h>

UI_MENU_DEFINE_RADIO(MIDIInDev)
UI_MENU_DEFINE_RADIO(MIDIOutDev)

static ui_menu_entry_t midi_in_dyn_menu[21];
static ui_menu_entry_t midi_out_dyn_menu[21];

static int midi_in_dyn_menu_init = 0;
static int midi_out_dyn_menu_init = 0;

UI_MENU_CALLBACK(MIDIInDev_dynmenu_callback)
{
    MMRESULT ret;
    MIDIINCAPS mic;
    int i;
    int num_in = midiInGetNumDevs();

    /* rebuild menu if it already exists. */
    if (midi_in_dyn_menu_init != 0) {
        for (i = 0; midi_in_dyn_menu[i].string != NULL; i++) {
            lib_free(midi_in_dyn_menu[i].string);
        }
    } else {
        midi_in_dyn_menu_init = 1;
    }

    if (num_in == 0) {
        midi_in_dyn_menu[0].string = (char *)lib_stralloc("No Devices Present");
        midi_in_dyn_menu[0].type = MENU_ENTRY_TEXT;
        midi_in_dyn_menu[0].callback = seperator_callback;
        midi_in_dyn_menu[0].data = NULL;
        midi_in_dyn_menu[1].string = NULL;
        midi_in_dyn_menu[i].type = 0;
        midi_in_dyn_menu[i].callback = NULL;
        midi_in_dyn_menu[i].data = NULL;
    } else {
        for (i = 0; (i < num_in) && (i < 20); i++) {
            ret = midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS));
            if (ret == MMSYSERR_NOERROR) {
                midi_in_dyn_menu[i].string = (char *)lib_stralloc(mic.szPname);
            } else {
                midi_in_dyn_menu[i].string = (char *)lib_stralloc("Error getting name");
            }
            midi_in_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
            midi_in_dyn_menu[i].callback = radio_MIDIInDev_callback;
            midi_in_dyn_menu[i].data = (ui_callback_data_t)i;
        }
        midi_in_dyn_menu[i].string = NULL;
        midi_in_dyn_menu[i].type = 0;
        midi_in_dyn_menu[i].callback = NULL;
        midi_in_dyn_menu[i].data = NULL;
    }
    return "->";
}

UI_MENU_CALLBACK(MIDIOutDev_dynmenu_callback)
{
    MMRESULT ret;
    MIDIOUTCAPS moc;
    int i;
    int num_out = midiOutGetNumDevs();

    /* rebuild menu if it already exists. */
    if (midi_out_dyn_menu_init != 0) {
        for (i = 0; midi_out_dyn_menu[i].string != NULL; i++) {
            lib_free(midi_out_dyn_menu[i].string);
        }
    } else {
        midi_out_dyn_menu_init = 1;
    }

    if (num_out == 0) {
        midi_out_dyn_menu[0].string = (char *)lib_stralloc("No Devices Present");
        midi_out_dyn_menu[0].type = MENU_ENTRY_TEXT;
        midi_out_dyn_menu[0].callback = seperator_callback;
        midi_out_dyn_menu[0].data = NULL;
        midi_out_dyn_menu[1].string = NULL;
        midi_out_dyn_menu[i].type = 0;
        midi_out_dyn_menu[i].callback = NULL;
        midi_out_dyn_menu[i].data = NULL;
    } else {
        for (i = 0; (i < num_out) && (i < 20); i++) {
            ret = midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS));
            if (ret == MMSYSERR_NOERROR) {
                midi_out_dyn_menu[i].string = (char *)lib_stralloc(moc.szPname);
            } else {
                midi_out_dyn_menu[i].string = (char *)lib_stralloc("Error getting name");
            }
            midi_out_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
            midi_out_dyn_menu[i].callback = radio_MIDIOutDev_callback;
            midi_out_dyn_menu[i].data = (ui_callback_data_t)i;
        }
        midi_out_dyn_menu[i].string = NULL;
        midi_out_dyn_menu[i].type = 0;
        midi_out_dyn_menu[i].callback = NULL;
        midi_out_dyn_menu[i].data = NULL;
    }
    return "->";
}

#define VICE_SDL_MIDI_ARCHDEP_ITEMS \
    SDL_MENU_ITEM_SEPARATOR, \
    { "MIDI-In", \
      MENU_ENTRY_SUBMENU, \
      MIDIInDev_dynmenu_callback, \
      (ui_callback_data_t)midi_in_dyn_menu }, \
    { "MIDI-Out", \
      MENU_ENTRY_SUBMENU, \
      MIDIOutDev_dynmenu_callback, \
      (ui_callback_data_t)midi_out_dyn_menu },

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

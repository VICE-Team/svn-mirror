/*
 * menu_c128hw.c - C128 HW menu for SDL UI.
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

#include "types.h"

#include "menu_c64_common_expansions.h"
#include "menu_common.h"
#include "menu_joystick.h"

#ifdef HAVE_MIDI
#include "menu_midi.h"
#endif

#ifdef HAVE_MOUSE
#include "menu_lightpen.h"
#include "menu_mouse.h"
#endif

#include "menu_ram.h"
#include "menu_rom.h"

#ifdef HAVE_RS232
#include "menu_rs232.h"
#endif

#include "menu_sid.h"

#ifdef HAVE_TFE
#include "menu_tfe.h"
#endif

#include "uimenu.h"

UI_MENU_DEFINE_RADIO(VDC64KB)
UI_MENU_DEFINE_RADIO(VDCRevision)

static const ui_menu_entry_t vdc_menu[] = {
    SDL_MENU_ITEM_TITLE("VDC revision"),
    { "Rev 0",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VDCRevision_callback,
      (ui_callback_data_t)0 },
    { "Rev 1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VDCRevision_callback,
      (ui_callback_data_t)1 },
    { "Rev 2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VDCRevision_callback,
      (ui_callback_data_t)2 },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("VDC memory size"),
    { "16kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VDC64KB_callback,
      (ui_callback_data_t)0 },
    { "64kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VDC64KB_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_LIST_END
};

UI_MENU_DEFINE_TOGGLE(EmuID)
UI_MENU_DEFINE_TOGGLE(IEEE488)
UI_MENU_DEFINE_TOGGLE(C128FullBanks)
UI_MENU_DEFINE_TOGGLE(SFXSoundSampler)

const ui_menu_entry_t c128_hardware_menu[] = {
    { "Joystick settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_c64_menu },
    { "SID settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_c128_menu },
    { "VDC settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vdc_menu },
#ifdef HAVE_MOUSE
    { "Mouse emulation",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)mouse_menu },
    { "Lightpen emulation",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)lightpen_menu },
#endif
    { "RAM pattern settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ram_menu },
    { "RAM banks 2 and 3",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_C128FullBanks_callback,
      NULL },
    { "ROM settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)c128_rom_menu },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Hardware expansions"),
#ifdef HAVE_RS232 
    { "RS232 settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)rs232_menu },
#endif
    { "Digimax settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)digimax_menu },
    { "IEEE488 interface",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_IEEE488_callback,
      NULL },
#ifdef HAVE_MIDI
    { "MIDI settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)midi_c64_menu },
#endif
#ifdef HAVE_TFE
    { "The Final Ethernet settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)tfe_menu },
#endif
    { "SFX Sound Expander settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)soundexpander_menu },
    { "SFX Sound Sampler",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SFXSoundSampler_callback,
      NULL },
    { "Emulator ID",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_EmuID_callback,
      NULL },
    SDL_MENU_LIST_END
};

/*
 * menu_scpu64hw.c - SCPU64 HW menu for SDL UI.
 *
 * Written by
 *  Marco van den Heuevel <blackystardust68@yahoo.com>
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

#include "c64fastiec.h"
#include "cartridge.h"
#include "menu_c64_common_expansions.h"
#include "menu_c64_expansions.h"
#include "menu_c64model.h"
#include "menu_common.h"
#include "menu_joyport.h"
#include "menu_joystick.h"
#include "menu_scpu64hw.h"

#ifdef HAVE_MIDI
#include "menu_midi.h"
#endif

#ifdef HAVE_MOUSE
#include "menu_mouse.h"
#endif

#include "menu_ram.h"
#include "menu_rom.h"
#include "menu_userport.h"

#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
#include "menu_rs232.h"
#endif

#include "menu_sid.h"

#ifdef HAVE_RAWNET
#include "menu_ethernet.h"
#include "menu_ethernetcart.h"
#endif

#include "uimenu.h"
#include "userport.h"
#include "util.h"

UI_MENU_DEFINE_RADIO(BurstMod)

const ui_menu_entry_t scpu64_burstmod_menu[] = {
    { "None",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_BurstMod_callback,
      (ui_callback_data_t)BURST_MOD_NONE },
    { "CIA1",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_BurstMod_callback,
      (ui_callback_data_t)BURST_MOD_CIA1 },
    { "CIA2",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_BurstMod_callback,
      (ui_callback_data_t)BURST_MOD_CIA2 },
    SDL_MENU_LIST_END
};

UI_MENU_DEFINE_RADIO(SIMMSize)

const ui_menu_entry_t scpu64_simmsize_menu[] = {
    { "0 MiB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SIMMSize_callback,
      (ui_callback_data_t)0 },
    { "1 MiB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SIMMSize_callback,
      (ui_callback_data_t)1 },
    { "4 MiB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SIMMSize_callback,
      (ui_callback_data_t)4 },
    { "8 MiB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SIMMSize_callback,
      (ui_callback_data_t)8 },
    { "16 MiB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SIMMSize_callback,
      (ui_callback_data_t)16 },
    SDL_MENU_LIST_END
};

UI_MENU_DEFINE_TOGGLE(JiffySwitch)
UI_MENU_DEFINE_TOGGLE(SpeedSwitch)

const ui_menu_entry_t scpu64_hardware_menu_template[] = {
    { "Model settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)scpu64_model_menu },
    { "SIMM size",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)scpu64_simmsize_menu },
    { "Jiffy switch enable",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JiffySwitch_callback,
      NULL },
    { "Speed switch enable",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SpeedSwitch_callback,
      NULL },
    { "Joyport settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joyport_menu },
    { "Joystick settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_c64_menu },
#ifdef HAVE_MOUSE
    { "Mouse emulation",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)mouse_menu },
#endif
    { "RAM pattern settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ram_menu },
    { "ROM settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)scpu64_rom_menu },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Hardware expansions"),
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
    { "RS232 settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)rs232_c64_menu },
#endif
    { CARTRIDGE_NAME_DIGIMAX " settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)digimax_menu },
    { CARTRIDGE_NAME_DS12C887RTC " settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ds12c887rtc_c64_menu },
#ifdef HAVE_MIDI
    { "MIDI settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)midi_c64_menu },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ethernet_menu },
    { CARTRIDGE_NAME_ETHERNETCART " settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ethernetcart_menu },
#endif
    { "Burst Mode Modification",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)scpu64_burstmod_menu },
    { "Userport settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)userport_menu },
    SDL_MENU_LIST_END
};

ui_menu_entry_t scpu64_hardware_menu[sizeof(scpu64_hardware_menu_template) / sizeof(ui_menu_entry_t)];

void scpu64_create_machine_menu(void)
{
    int has_userport = userport_get_active_state();
    int i;
    int j = 0;

    for (i = 0; scpu64_hardware_menu_template[i].string != NULL; i++) {
        if (!util_strcasecmp(scpu64_hardware_menu_template[i].string, "Userport settings")) {
            if (has_userport) {
                scpu64_hardware_menu[j].string = scpu64_hardware_menu_template[i].string;
                scpu64_hardware_menu[j].type = scpu64_hardware_menu_template[i].type;
                scpu64_hardware_menu[j].callback = scpu64_hardware_menu_template[i].callback;
                scpu64_hardware_menu[j].data = scpu64_hardware_menu_template[i].data;
                j++;
            }
        } else {
            scpu64_hardware_menu[j].string = scpu64_hardware_menu_template[i].string;
            scpu64_hardware_menu[j].type = scpu64_hardware_menu_template[i].type;
            scpu64_hardware_menu[j].callback = scpu64_hardware_menu_template[i].callback;
            scpu64_hardware_menu[j].data = scpu64_hardware_menu_template[i].data;
            j++;
        }
    }
    scpu64_hardware_menu[j].string = scpu64_hardware_menu_template[i].string;
    scpu64_hardware_menu[j].type = scpu64_hardware_menu_template[i].type;
    scpu64_hardware_menu[j].callback = scpu64_hardware_menu_template[i].callback;
    scpu64_hardware_menu[j].data = scpu64_hardware_menu_template[i].data;
}

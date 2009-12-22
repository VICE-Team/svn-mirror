/*
 * menu_plus4hw.c - PLUS4 HW menu for SDL UI.
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

#include "machine.h"
#include "menu_common.h"
#include "menu_joystick.h"
#include "menu_ram.h"
#include "menu_rom.h"

#ifdef HAVE_RS232
#include "menu_rs232.h"
#endif

#include "menu_sid.h"
#include "pets.h"
#include "uimenu.h"

/* PET VIDEO SETTINGS */

/* PET MEMORY SETTINGS */

UI_MENU_DEFINE_RADIO(RamSize)
UI_MENU_DEFINE_RADIO(IOSize)
UI_MENU_DEFINE_TOGGLE(SuperPET)
UI_MENU_DEFINE_TOGGLE(Ram9)
UI_MENU_DEFINE_TOGGLE(RamA)

static const ui_menu_entry_t pet_memory_menu[] = {
    SDL_MENU_ITEM_TITLE("Memory size"),
    { "4kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RamSize_callback,
      (ui_callback_data_t)4 },
    { "8kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RamSize_callback,
      (ui_callback_data_t)8 },
    { "16kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RamSize_callback,
      (ui_callback_data_t)16 },
    { "32kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RamSize_callback,
      (ui_callback_data_t)32 },
    { "96kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RamSize_callback,
      (ui_callback_data_t)96 },
    { "128kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_RamSize_callback,
      (ui_callback_data_t)128 },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("I/O size"),
    { "256 bytes",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_IOSize_callback,
      (ui_callback_data_t)0x100 },
    { "2kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_IOSize_callback,
      (ui_callback_data_t)0x800 },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("8296 memory blocks"),
    { "$9xxx as RAM",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_Ram9_callback,
      NULL },
    { "$Axxx as RAM",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_RamA_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SUPERPET I/O"),
    { "Enable SUPERPET I/O (disables 8x96)",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SuperPET_callback,
      NULL },
    { NULL }
};

/* PETREU */

UI_MENU_DEFINE_TOGGLE(PETREU)
UI_MENU_DEFINE_RADIO(PETREUsize)
UI_MENU_DEFINE_FILE_STRING(PETREUfilename)

static const ui_menu_entry_t petreu_menu[] = {
    { "Enable PET REU",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_PETREU_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Memory size"),
    { "128kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_PETREUsize_callback,
      (ui_callback_data_t)128 },
    { "512kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_PETREUsize_callback,
      (ui_callback_data_t)512 },
    { "1024kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_PETREUsize_callback,
      (ui_callback_data_t)1024 },
    { "2048kB",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_PETREUsize_callback,
      (ui_callback_data_t)2048 },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("RAM image"),
    { "PET REU image file",
      MENU_ENTRY_DIALOG,
      file_string_PETREUfilename_callback,
      (ui_callback_data_t)"Select PET REU image" },
    { NULL }
};

/* PETDWW */

UI_MENU_DEFINE_TOGGLE(PETDWW)
UI_MENU_DEFINE_FILE_STRING(PETDWWfilename)

static const ui_menu_entry_t petdww_menu[] = {
    { "Enable PET DWW",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_PETDWW_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("RAM image"),
    { "PET DWW image file",
      MENU_ENTRY_DIALOG,
      file_string_PETDWWfilename_callback,
      (ui_callback_data_t)"Select PET DWW image" },
    { NULL }
};

/* PET MODEL SELECTION */

enum {
    PET_MODEL_2001_8N,
    PET_MODEL_3008,
    PET_MODEL_3016,
    PET_MODEL_3032,
    PET_MODEL_3032B,
    PET_MODEL_4016,
    PET_MODEL_4032,
    PET_MODEL_4032B,
    PET_MODEL_8032,
    PET_MODEL_8096,
    PET_MODEL_8296,
    PET_MODEL_SUPERPET
};

static UI_MENU_CALLBACK(select_pet_model_callback)
{
    int model;

    model = vice_ptr_to_int(param);
    if (activated) {
        switch (model) {
            case PET_MODEL_2001_8N:
                pet_set_model("2001", NULL);
                break;
            case PET_MODEL_3008:
                pet_set_model("3008", NULL);
                break;
            case PET_MODEL_3016:
                pet_set_model("3016", NULL);
                break;
            case PET_MODEL_3032:
                pet_set_model("3032", NULL);
                break;
            case PET_MODEL_3032B:
                pet_set_model("3032B", NULL);
                break;
            case PET_MODEL_4016:
                pet_set_model("4016", NULL);
                break;
            case PET_MODEL_4032:
                pet_set_model("4032", NULL);
                break;
            case PET_MODEL_4032B:
                pet_set_model("4032B", NULL);
                break;
            case PET_MODEL_8032:
                pet_set_model("8032", NULL);
                break;
            case PET_MODEL_8096:
                pet_set_model("8096", NULL);
                break;
            case PET_MODEL_8296:
                pet_set_model("8296", NULL);
                break;
            default:
            case PET_MODEL_SUPERPET:
                pet_set_model("SuperPET", NULL);
                break;
        }
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        return sdl_menu_text_exit_ui;
    }
    return NULL;
}

static const ui_menu_entry_t pet_model_menu[] = {
    { "PET 2001-8N",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_2001_8N },
    { "PET 3008",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_3008 },
    { "PET 3016",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_3016 },
    { "PET 3032",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_3032 },
    { "PET 3032B",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_3032B },
    { "PET 4016",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_4016 },
    { "PET 4032",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_4032 },
    { "PET 4032B",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_4032B },
    { "PET 8032",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_8032 },
    { "PET 8096",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_8096 },
    { "PET 8296",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_8296 },
    { "SUPERPET",
      MENU_ENTRY_OTHER,
      select_pet_model_callback,
      (ui_callback_data_t)PET_MODEL_SUPERPET },
    { NULL }
};

UI_MENU_DEFINE_RADIO(KeymapIndex)

static const ui_menu_entry_t pet_keyboard_menu[] = {
    { "Graphics",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KeymapIndex_callback,
      (ui_callback_data_t)2 },
    { "Business (UK)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KeymapIndex_callback,
      (ui_callback_data_t)0 },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Crtc)
UI_MENU_DEFINE_TOGGLE(EmuID)

const ui_menu_entry_t pet_hardware_menu[] = {
    { "Select PET model",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pet_model_menu },
    { "Keyboard",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)pet_keyboard_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Joystick settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_userport_only_menu },
    { "SID cart settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sid_pet_menu },
    { "RAM pattern settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ram_menu },
    { "ROM settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pet_rom_menu },
    { "PET REU settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)petreu_menu },
    { "PET DWW settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)petdww_menu },
    { "Memory and I/O settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pet_memory_menu },
    { "CRTC chip enable",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_Crtc_callback,
      NULL },
#ifdef HAVE_RS232
    { "RS232 settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)rs232_nouser_menu },
#endif 
    { "Emulator ID",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_EmuID_callback,
      NULL },
    { NULL }
};

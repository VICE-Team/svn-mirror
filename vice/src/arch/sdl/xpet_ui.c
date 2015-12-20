/*
 * xpet_ui.c - Implementation of the PET-specific part of the UI.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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
#include <stdlib.h>

#include "debug.h"
#include "lib.h"

#if 1
#include "log.h"
#endif

#include "menu_common.h"
#include "menu_debug.h"
#include "menu_drive.h"
#include "menu_ffmpeg.h"
#include "menu_help.h"
#include "menu_jam.h"
#include "menu_joyport.h"
#include "menu_network.h"
#include "menu_petcart.h"
#include "menu_pethw.h"
#include "menu_printer.h"
#include "menu_reset.h"
#include "menu_screenshot.h"
#include "menu_settings.h"
#include "menu_snapshot.h"
#include "menu_sound.h"
#include "menu_speed.h"
#include "menu_tape.h"
#include "menu_video.h"
#include "petmem.h"
#include "pet-resources.h"
#include "resources.h"
#include "ui.h"
#include "uimenu.h"
#include "vkbd.h"

static const ui_menu_entry_t xpet_main_menu[] = {
    { "Autostart image",
      MENU_ENTRY_DIALOG,
      autostart_callback,
      NULL },
    { "Drive",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)drive_menu },
    { "Tape",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)tape_menu },
    { "Cartridge",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)petcart_menu },
    { "Printer",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)printer_ieee_menu },
    { "Machine settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pet_hardware_menu },
    { "Video settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pet_video_menu },
    { "Sound settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sound_output_menu },
    { "Snapshot",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)snapshot_menu },
    { "Screenshot",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)screenshot_menu },
    { "Speed settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)speed_menu },
    { "Reset",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)reset_menu },
    { "Action on CPU JAM",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)jam_menu },
#ifdef HAVE_NETWORK
    { "Network",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)network_menu },
#endif
    { "Pause",
      MENU_ENTRY_OTHER,
      pause_callback,
      NULL },
    { "Monitor",
      MENU_ENTRY_OTHER,
      monitor_callback,
      NULL },
    { "Virtual keyboard",
      MENU_ENTRY_OTHER,
      vkbd_callback,
      NULL },
    { "Statusbar",
      MENU_ENTRY_OTHER,
      statusbar_callback,
      NULL },
#ifdef DEBUG
    { "Debug",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)debug_menu },
#endif
    { "Help",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)help_menu },
    { "Settings management",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)settings_manager_menu },
    { "Quit emulator",
      MENU_ENTRY_OTHER,
      quit_callback,
      NULL },
    SDL_MENU_LIST_END
};

static BYTE *pet_font;

/* FIXME: support all PET keyboards (see pet-resources.h) */

void petui_set_menu_params(int index, menu_draw_t *menu_draw)
{
    static int old_keymap = -1;
    int cols = petmem_get_rom_columns();
    int keymap;

#if 1
    log_warning(LOG_DEFAULT, "cols : %d", cols);
#endif

    menu_draw->max_text_x = cols ? cols : 40;
    menu_draw->extra_x = 24;
    menu_draw->extra_y = (cols == 40) ? 32 : 20;

    menu_draw->max_text_y = (cols == 40) ? 26 : 28;

    resources_get_int("KeyboardType", &keymap);

    if (keymap != old_keymap) {
        if (keymap == KBD_TYPE_GRAPHICS_US) {
            sdl_vkbd_set_vkbd(&vkbd_pet_gr);
        } else {
            sdl_vkbd_set_vkbd(&vkbd_pet_uk);
        }
        old_keymap = keymap;
    }
}

int petui_init(void)
{
    int i, j;

#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif

    sdl_ui_set_menu_params = petui_set_menu_params;
    uijoyport_menu_create(0, 0, 1, 1);
    uidrive_menu_create();
    uikeyboard_menu_create();

    sdl_ui_set_main_menu(xpet_main_menu);

    pet_font = lib_malloc(8 * 256);
    for (i = 0; i < 128; i++) {
        for (j = 0; j < 8; j++) {
            pet_font[(i * 8) + j] = mem_chargen_rom[(i * 16) + (256 * 16) + j];
            pet_font[(i * 8) + (128 * 8) + j] = mem_chargen_rom[(i * 16) + j];
        }
    }
    sdl_ui_set_menu_font(pet_font, 8, 8);

#ifdef HAVE_FFMPEG
    sdl_menu_ffmpeg_init();
#endif

    return 0;
}

void petui_shutdown(void)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif

#ifdef HAVE_FFMPEG
    sdl_menu_ffmpeg_shutdown();
#endif

    lib_free(pet_font);
}

/*
 * xvic_ui.c - Implementation of the VIC20-specific part of the UI.
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
#include "machine.h"
#include "menu_common.h"
#include "menu_debug.h"
#include "menu_drive.h"
#include "menu_edit.h"
#include "menu_ethernet.h"
#include "menu_ffmpeg.h"
#include "menu_help.h"
#include "menu_jam.h"
#include "menu_joyport.h"
#include "menu_joystick.h"
#include "menu_media.h"
#include "menu_midi.h"
#include "menu_monitor.h"
#include "menu_network.h"
#include "menu_printer.h"
#include "menu_reset.h"
#include "menu_sampler.h"
#include "menu_screenshot.h"
#include "menu_settings.h"
#include "menu_sid.h"
#include "menu_snapshot.h"
#include "menu_sound.h"
#include "menu_speed.h"
#include "menu_tape.h"
#include "menu_userport.h"
#include "menu_vic20cart.h"
#include "menu_vic20hw.h"
#include "menu_video.h"
#include "resources.h"
#include "ui.h"
#include "uifonts.h"
#include "uimenu.h"
#include "vic.h"
#include "victypes.h"
#include "vic20rom.h"
#include "vic20ui.h"
#include "vic20memrom.h"
#include "videoarch.h"
#include "vkbd.h"

static UI_MENU_CALLBACK(pause_callback_wrapper);

static ui_menu_entry_t xvic_main_menu[] = {
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
      (ui_callback_data_t)vic20cart_menu },
    { "Printer",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)printer_iec_menu },
    { "Machine settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vic20_hardware_menu },
    { "Video settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vic20_video_menu },
    { "Sound settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sound_output_menu },
    { "Sampler settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)sampler_menu },
    { "Snapshot",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)snapshot_menu },
    { "Save media file",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)media_menu },
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
      MENU_ENTRY_OTHER_TOGGLE,
      pause_callback_wrapper,
      NULL },
    /* Caution: index is hardcoded below */
    { "Advance Frame",
      MENU_ENTRY_OTHER,
      advance_frame_callback,
      NULL },
    { "Monitor",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)monitor_menu },
    /* Caution: index is hardcoded below */
    { "Virtual keyboard",
      MENU_ENTRY_OTHER,
      vkbd_callback,
      NULL },
    { "Statusbar",
      MENU_ENTRY_OTHER_TOGGLE,
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
    { "Edit",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)edit_menu },
    { "Quit emulator",
      MENU_ENTRY_OTHER,
      quit_callback,
      NULL },
    SDL_MENU_LIST_END
};

#ifdef HAVE_NETWORK
# define MENU_ADVANCE_FRAME_IDX      16
# define MENU_VIRTUAL_KEYBOARD_IDX   18
#else
# define MENU_ADVANCE_FRAME_IDX      15
# define MENU_VIRTUAL_KEYBOARD_IDX   17
#endif
static UI_MENU_CALLBACK(pause_callback_wrapper)
{
    xvic_main_menu[MENU_ADVANCE_FRAME_IDX].status =
        sdl_pause_state || !sdl_menu_state ? MENU_STATUS_ACTIVE : MENU_STATUS_INACTIVE;
    xvic_main_menu[MENU_VIRTUAL_KEYBOARD_IDX].status =
        sdl_pause_state ? MENU_STATUS_INACTIVE : MENU_STATUS_ACTIVE;
    return pause_callback(activated, param);
}

static void vic20ui_set_menu_params(int index, menu_draw_t *menu_draw)
{
    int videostandard;

    resources_get_int("MachineVideoStandard", &videostandard);

    /* VIC */
#ifdef VIC_DUPLICATES_PIXELS
    menu_draw->max_text_x = 48;
#else
    /* FIXME some functions assume max_text_x to be at least 40 */
    menu_draw->max_text_x = (videostandard == MACHINE_SYNC_PAL) ? 28 : 25;
#endif
    menu_draw->max_text_y = 26;

    menu_draw->extra_x = sdl_active_canvas->viewport->first_x - sdl_active_canvas->geometry->gfx_position.x;
    menu_draw->extra_y = sdl_active_canvas->geometry->first_displayed_line - sdl_active_canvas->geometry->gfx_position.y;

#ifdef VIC_DUPLICATES_PIXELS
    menu_draw->extra_x += (videostandard == MACHINE_SYNC_PAL) ? 36 : 8;
#endif
    menu_draw->extra_y += (videostandard == MACHINE_SYNC_PAL) ? 40 : 24;

    menu_draw->color_front = menu_draw->color_default_front = 1;
    menu_draw->color_back = menu_draw->color_default_back = 0;
    menu_draw->color_cursor_back = 6;
    menu_draw->color_cursor_revers = 0;
    menu_draw->color_active_green = 13;
    menu_draw->color_inactive_red = 2;
    menu_draw->color_active_grey = 15;
    menu_draw->color_inactive_grey = 11;
}

/** \brief  Pre-initialize the UI before the canvas window gets created
 *
 * \return  0 on success, -1 on failure
 */
int vic20ui_init_early(void)
{
    return 0;
}

/** \brief  Initialize the UI
 *
 * \return  0 on success, -1 on failure
 */
int vic20ui_init(void)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif

    sdl_ui_set_menu_params = vic20ui_set_menu_params;
    uijoyport_menu_create(1, 0, 1, 1, 1);
    uijoystick_menu_create(1, 0, 1, 1, 1);
    uiuserport_menu_create(1);
    uisampler_menu_create();
    uidrive_menu_create();
    uikeyboard_menu_create();
    uipalette_menu_create("VIC", NULL);
    uisid_menu_create();
    uimedia_menu_create();

    sdl_ui_set_main_menu(xvic_main_menu);
    sdl_ui_font_init(VIC20_CHARGEN_NAME, 0, 0x800, 0);
    sdl_vkbd_set_vkbd(&vkbd_vic20);

#ifdef HAVE_FFMPEG
    sdl_menu_ffmpeg_init();
#endif

    return 0;
}

void vic20ui_shutdown(void)
{
    uisid_menu_shutdown();
    uikeyboard_menu_shutdown();
    uipalette_menu_shutdown();
    uijoyport_menu_shutdown();
    uijoystick_menu_shutdown();
    uiuserport_menu_shutdown();
    uitapeport_menu_shutdown();
    uimedia_menu_shutdown();
#ifdef HAVE_MIDI
    sdl_menu_midi_in_free();
    sdl_menu_midi_out_free();
#endif

#ifdef HAVE_RAWNET
    sdl_menu_ethernet_interface_free();
#endif

#ifdef HAVE_FFMPEG
    sdl_menu_ffmpeg_shutdown();
#endif

    sdl_ui_font_shutdown();
}

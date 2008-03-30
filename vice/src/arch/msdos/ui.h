/*
 * ui.h - A (very) simple user interface for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _UI_DOS_H
#define _UI_DOS_H

#include "types.h"
#include "tuimenu.h"

typedef enum {
    UI_JAM_RESET, UI_JAM_HARD_RESET, UI_JAM_MONITOR, UI_JAM_DEBUG
} ui_jam_action_t;

/* ------------------------------------------------------------------------- */

/* This is used by `ui_enable_drive_status()'.  */
typedef enum {
    UI_DRIVE_ENABLE_NONE = 0,
    UI_DRIVE_ENABLE_0 = 1 << 0,
    UI_DRIVE_ENABLE_1 = 1 << 1,
    UI_DRIVE_ENABLE_2 = 1 << 2,
    UI_DRIVE_ENABLE_3 = 1 << 3
} ui_drive_enable_t;

/* ------------------------------------------------------------------------- */

extern int ui_init_resources(void);
extern int ui_init_cmdline_options(void);

extern int ui_init(int *argc, char **argv);
extern int ui_init_finish(void);
extern void ui_error(const char *format, ...);
extern ui_jam_action_t ui_jam_dialog(const char *format, ...);
extern void ui_show_text(const char *title, const char *text);
extern void ui_update_menus(void);
extern void ui_main(char hotkey);
extern void ui_enable_drive_status(ui_drive_enable_t state,
                                   int *drive_led_color);
extern void ui_display_drive_track(int drive_number, double track_number);
extern void ui_display_drive_led(int drive_number, int status);
extern void ui_set_warp_status(int status);
extern void ui_dispatch_events(void);
extern int ui_extend_image_dialog(void);

#endif

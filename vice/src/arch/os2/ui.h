/*
 * ui.h - A user interface for OS/2.
 *
 * Written by
 *  Thomas Bretz (tbretz@gsi.de)
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

#ifndef _UI_H
#define _UI_H

#include "types.h"
#include "cmdline.h"

typedef enum {
    UI_JAM_RESET, UI_JAM_HARD_RESET, UI_JAM_MONITOR, UI_JAM_DEBUG
} ui_jam_action_t;

typedef enum {
    UI_DRIVE_ENABLE_NONE = 0,
    UI_DRIVE_ENABLE_0 = 1 << 0,
    UI_DRIVE_ENABLE_1 = 1 << 1,
    UI_DRIVE_ENABLE_2 = 1 << 2,
    UI_DRIVE_ENABLE_3 = 1 << 3
} ui_drive_enable_t;

/* ------------------------- VICE functions -------------------------------- */
extern int  ui_init_resources(void);
extern int  ui_init_cmdline_options(void);

extern int  ui_init(int *argc, char **argv);
extern int  ui_init_finish(void);
extern void ui_error(const char *format, ...);
extern ui_jam_action_t ui_jam_dialog(const char *format, ...);
extern void ui_show_text(char *title, char *text);
extern void ui_update_menus(void);
extern void ui_main(char hotkey);
extern int  ui_extend_image_dialog(void);
extern void ui_display_drive_led  (int drive_number, int status);
extern void ui_display_drive_track(int drive_number, int drive_base,
				   double track_number);
extern void ui_display_drive_current_image(int drive_number, const char *image);
extern void ui_enable_drive_status(int state, int *drive_led_color);

// ------------------------- OS/2 only -------------------------
extern void ui_OK_dialog          (char *title, char *msg);
extern int  ui_yesno_dialog       (char *title, char *msg);
extern int  ui_file_dialog        (char *title, char *drive,
                                   char *path,  char *button, char *result);

#endif

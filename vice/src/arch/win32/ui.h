/*
 * ui.h - Windows user interface.
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

#ifndef _UI_H
#define _UI_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "video.h"

typedef enum {
    UI_JAM_RESET, UI_JAM_HARD_RESET, UI_JAM_MONITOR, UI_JAM_DEBUG
} ui_jam_action_t;

extern int ui_init_resources(void);
extern int ui_init_cmdline_options(void);
extern int ui_init(int *argc, char **argv);
extern int ui_init_finish(void);
extern void ui_exit(void);
extern void ui_display_speed(float percent, float framerate, int warp_flag);
extern void ui_toggle_drive_status(int state);
extern void ui_display_drive_track(double track_number);
extern void ui_display_drive_led(int status);
extern void ui_display_paused(int flag);
extern void ui_dispatch_next_event(void);
extern void ui_dispatch_events(void);
extern void ui_update_menus(void);

extern void ui_error(const char *format,...);
extern int ui_extend_image_dialog();
extern ui_jam_action_t ui_jam_dialog(const char *format,...);

/* ------------------------------------------------------------------------ */

/* Windows-specific functions.  */

extern HWND ui_get_main_hwnd(void);
extern HWND ui_open_canvas_window(const char *title, unsigned int width,
                                  unsigned int height,
                                  canvas_redraw_t exposure_handler);
extern void ui_resize_canvas_window(HWND w, unsigned int width,
                                    unsigned int height);

#endif

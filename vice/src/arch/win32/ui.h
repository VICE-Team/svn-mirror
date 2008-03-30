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

#ifndef _UI_WIN32_H
#define _UI_WIN32_H

#include "uiapi.h"
#include "video.h"

typedef enum {
    UI_BUTTON_NONE, UI_BUTTON_CLOSE, UI_BUTTON_OK, UI_BUTTON_CANCEL,
    UI_BUTTON_YES, UI_BUTTON_NO, UI_BUTTON_RESET, UI_BUTTON_HARDRESET,
    UI_BUTTON_MON, UI_BUTTON_DEBUG, UI_BUTTON_CONTENTS, UI_BUTTON_AUTOSTART
} ui_button_t;

/* ------------------------------------------------------------------------- */

extern void ui_exit(void);
extern void ui_display_speed(float percent, float framerate, int warp_flag);
extern void ui_display_paused(int flag);
extern void ui_dispatch_next_event(void);
extern void ui_dispatch_events(void);

extern ui_button_t ui_ask_confirmation(const char *title, const char *text);

typedef void (*ui_machine_specific_t) (WPARAM wparam, HWND hwnd);
extern void ui_register_machine_specific(ui_machine_specific_t func);

/* ------------------------------------------------------------------------ */

/* Windows-specific functions.  */

extern HWND ui_get_main_hwnd(void);
extern HWND ui_open_canvas_window(const char *title, unsigned int width,
                                  unsigned int height,
                                  canvas_redraw_t exposure_handler);
extern void ui_resize_canvas_window(HWND w, unsigned int width,
                                    unsigned int height);

#endif


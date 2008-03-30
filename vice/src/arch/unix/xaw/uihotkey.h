/*
 * uihotkeys.h - Implementation of UI hotkeys.
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

#ifndef _UIHOTKEY_H
#define _UIHOTKEY_H

#include <X11/keysym.h>

#include "ui.h"

typedef enum {
    UI_HOTMOD_NONE = 0,
    UI_HOTMOD_CTRL = 1 << 0,
    UI_HOTMOD_ALT = 1 << 1,
    UI_HOTMOD_META = 1 << 2,
    UI_HOTMOD_SHIFT = 1 << 3
} ui_hotkey_modifier_t;

int ui_hotkey_init(void);
void ui_hotkey_event_handler(Widget w, XtPointer closure,
                             XEvent *xevent, Boolean *continue_to_dispatch);
void ui_hotkey_register(ui_hotkey_modifier_t modifier,
                        KeySym keysym, ui_callback_t callback,
                        ui_callback_data_t client_data);

#endif

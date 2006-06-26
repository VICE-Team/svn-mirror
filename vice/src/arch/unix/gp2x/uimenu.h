/*
 * uimenu.h
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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

#ifndef _UIMENU_H
#define _UIMENU_H

#include "ui.h"
#include "uiarch.h"

typedef struct ui_menu_entry_s {
    char *string;
    ui_callback_t callback;
    ui_callback_data_t callback_data;
    struct ui_menu_entry_s *sub_menu;
    ui_keysym_t hotkey_keysym;
    ui_hotkey_modifier_t hotkey_modifier;
} ui_menu_entry_t;

#define UI_MENU_DEFINE_RADIO(resource)                                  \
    static UI_CALLBACK(radio_##resource)                                \
    {                                                                   \
        _ui_menu_radio_helper();    \
    }
extern void _ui_menu_radio_helper(void);

#define UI_MENU_DEFINE_TOGGLE(resource)                                 \
    static UI_CALLBACK(toggle_##resource)                               \
    {                                                                   \
        _ui_menu_toggle_helper();   \
    }
extern void _ui_menu_toggle_helper(void);

#define UI_MENU_DEFINE_STRING_RADIO(resource)                               \
    static void radio_##resource() \
    {                                                                       \
        _ui_menu_string_radio_helper(); \
    }
extern void _ui_menu_string_radio_helper(void);

#endif

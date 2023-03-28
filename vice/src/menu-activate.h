/*
 * menu-activate.h - API to start the settings menu from a joystick action.
 *
 * Written by
 *  Fabrizio Gennari <fabrizio.ge@tiscali.it>
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

#ifndef VICE_MENU_ACTIVATE_H
#define VICE_MENU_ACTIVATE_H

#include "vice.h"

#include <stdio.h>

void arch_ui_activate(void);

#if (defined USE_SDLUI ||defined USE_SDL2UI)
struct ui_menu_entry_s;
void arch_ui_perform_action(struct ui_menu_entry_s* item);
void arch_hotkey_path_to_file(FILE* file, struct ui_menu_entry_s* item);
struct ui_menu_entry_s* arch_hotkey_action(char* path);
#else
void arch_ui_perform_action(int action);
void arch_hotkey_path_to_file(FILE* file, int action);
int arch_hotkey_action(char* path);
#endif

#endif

/*
 * menu-activate.c - API to start the settings menu from a joystick action.
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

#include "menu-activate.h"

#include "lib.h"
#include "uimenu.h"
#include "uihotkey.h"

void arch_ui_activate(void)
{
    sdl_ui_activate();
}

void arch_ui_perform_action(ui_menu_entry_t* item)
{
    sdl_ui_hotkey(item);
}

void arch_hotkey_path_to_file(FILE* fp, ui_menu_entry_t* item)
{
    char *hotkey_path = sdl_ui_hotkey_path(item);
    fprintf(fp, " %s", hotkey_path);
    lib_free(hotkey_path);
}

ui_menu_entry_t* arch_hotkey_action(char* path) {
    return sdl_ui_hotkey_action(path);
}


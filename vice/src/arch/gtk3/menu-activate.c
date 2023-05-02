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

#include <stdlib.h>
#include <stdio.h>

#include "menu-activate.h"
#include "uiactions.h"
#include "uisettings.h"

void arch_ui_activate(void)
{
    /* Use actions system to avoid popping up multiple dialogs on multiple
     * button presses: */
    ui_action_trigger(ACTION_SETTINGS_DIALOG);
}

void arch_ui_perform_action(int action)
{
    ui_action_trigger(action);
}

void arch_hotkey_path_to_file(FILE* file, int action)
{
    fprintf(file, " %d", action);
}

int arch_hotkey_action(char* path) {
    return atoi(path);
}


/** \file   hotkeys.h
 * \brief   Gtk3 custom hotkeys handling - header
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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
 */

#ifndef VICE_HOTKEYS_H
#define VICE_HOTKEYS_H

#include <stdbool.h>


/** \brief  Prefix of Gtk3 hotkeys files
 */
#define VKM_PREFIX  "gtk3_hotkeys"

/** \brief  Extension of Gtk3 hotkeys files
 *
 * Although the extension is the same as for the SDL UI, the format is slightly
 * different.
 */
#define VKM_EXT     ".vkm"

/** \brief  Filename of default Gtk3 hotkeys files
 */
#define VKM_DEFAULT_NAME    VKM_PREFIX##VKM_EXT


int     ui_hotkeys_resources_init(void);
int     ui_hotkeys_cmdline_options_init(void);

void    ui_hotkeys_init(void);
void    ui_hotkeys_shutdown(void);

bool    ui_hotkeys_parse(const char *path);

char *  ui_hotkeys_get_hotkey_string_for_action(const char *action);

bool    ui_hotkeys_export(const char *path);

#endif


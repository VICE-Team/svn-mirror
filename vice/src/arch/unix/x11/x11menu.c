/*
 * x11menu.c - Common X11 menu functions.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "lib.h"
#include "ui.h"
#include "util.h"
#include "x11menu.h"


char *make_menu_label(ui_menu_entry_t *e)
{
    const char *key_string, *tmp = "";
    char *retstr, *trans;

    /* Check wether NO_TRANS prefix is there, if yes don't translate it */
    if (strncmp(e->string, NO_TRANS, strlen(NO_TRANS)) == 0)
        trans = lib_stralloc(e->string + strlen(NO_TRANS));
    else
        trans = lib_stralloc(_(e->string));

    if (e->hotkey_keysym == (ui_keysym_t)0)
        return trans;

    if (e->hotkey_modifier & UI_HOTMOD_CONTROL)
        tmp = "C-";
    if (e->hotkey_modifier & UI_HOTMOD_META)
        tmp = "M-";
    if (e->hotkey_modifier & UI_HOTMOD_ALT)
        tmp = "A-";
    if (e->hotkey_modifier & UI_HOTMOD_SHIFT)
        tmp = "S-";

    key_string = strchr(XKeysymToString(e->hotkey_keysym), '_');
    if (key_string == NULL)
        key_string = XKeysymToString(e->hotkey_keysym);
    else
        key_string++;

    retstr = util_concat(trans, "    (", tmp, key_string, ")", NULL);

    lib_free(trans);

    return retstr;
}


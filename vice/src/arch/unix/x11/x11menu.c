/*
 * x11menu.c - Common X11 menu functions.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "x11menu.h"

#ifdef USE_GNOMEUI
#include "uihotkey.h"
#else
#include "xaw/uihotkey.h"
#endif

char *make_menu_label(ui_menu_entry_t *e)
{
    const char *key_string;
    char *tmp, *retstr, *trans;

    /* Check wether NO_TRANS prefix is there, if yes don't translate it */
    if (strncmp(e->string, NO_TRANS, strlen(NO_TRANS)) == 0)
	trans = stralloc(e->string + strlen(NO_TRANS));
    else
	trans = stralloc(_(e->string));
    
    if (e->hotkey_keysym == (KeySym) 0)
        return trans;

    tmp = xmalloc(1024);

    *tmp = '\0';
    if (e->hotkey_modifier & UI_HOTMOD_CONTROL)
        strcat(tmp, "C-");
    if (e->hotkey_modifier & UI_HOTMOD_META)
        strcat(tmp, "M-");
    if (e->hotkey_modifier & UI_HOTMOD_ALT)
        strcat(tmp, "A-");
    if (e->hotkey_modifier & UI_HOTMOD_SHIFT)
        strcat(tmp, "S-");

    key_string = strchr(XKeysymToString(e->hotkey_keysym), '_');
    if (key_string == NULL)
        key_string = XKeysymToString(e->hotkey_keysym);
    else
        key_string++;

    retstr = concat(trans, "    (", tmp, key_string, ")", NULL);

    free(tmp);
    return retstr;
}


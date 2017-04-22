/*
 * kbd.c - Native GTK3 UI keyboard stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "keyboard.h"
#include "kbd.h"

#include "not_implemented.h"

int kbd_arch_get_host_mapping(void)
{
    NOT_IMPLEMENTED_WARN_ONLY();
    return KBD_MAPPING_US;
}

void kbd_arch_init(void)
{
    NOT_IMPLEMENTED();
}

signed long kbd_arch_keyname_to_keynum(char *keyname)
{
    NOT_IMPLEMENTED();
    return 0;
}

const char *kbd_arch_keynum_to_keyname(signed long keynum)
{
    NOT_IMPLEMENTED();
    return NULL;
}

void kbd_initialize_numpad_joykeys(int *joykeys)
{
    NOT_IMPLEMENTED();
}


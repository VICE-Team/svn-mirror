/*
 * c64keyboard.c
 *
 * Written by
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

#include "c64keyboard.h"
#include "keyboard.h"
#include "maincpu.h"
#include "vicii.h"


static void c64keyboard_machine_func(int *keyarr)
{
    unsigned int lightpen, i;
    static unsigned int old_lightpen = 1;

    lightpen = 1;

    for (i = 0; i < 8; i++) {
        if (keyarr[i] & 0x10)
            lightpen = 0;
    }

    if (old_lightpen && !lightpen)
        vicii_trigger_light_pen(maincpu_clk);

    old_lightpen = lightpen;
}

void c64keyboard_init(void)
{
    keyboard_register_machine(c64keyboard_machine_func);
}



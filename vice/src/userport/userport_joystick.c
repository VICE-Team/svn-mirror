/*
 * userport_dac.c - Generic userport 8bit DAC emulation.
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
#include <stdlib.h>
#include <string.h>

#include "joystick.h"
#include "types.h"
#include "userport_joystick.h"

int userport_joystick_enable;
int userport_joystick_type;

static int userport_joystick_cga_select = 0;
static BYTE userport_joystick_hit_button_sp2 = 0xff;

/* ------------------------------------------------------------------------- */

/* FIXME: direction and unused bits are not yet taken into account */

void userport_joystick_store_pa2(BYTE value)
{
}

void userport_joystick_store_pbx(BYTE value)
{
    if (userport_joystick_enable && userport_joystick_type == USERPORT_JOYSTICK_CGA) {
        userport_joystick_cga_select = (value & 0x80) ? 0 : 1;
    }
}

void userport_joystick_store_sdr(BYTE value)
{
    if (userport_joystick_enable && userport_joystick_type == USERPORT_JOYSTICK_HIT) {
        userport_joystick_hit_button_sp2 = (get_joystick_value(4) & 0x10) ? 0 : 0xff;
    }
}

BYTE userport_joystick_read_pa2(BYTE orig)
{
    BYTE retval = orig;

    if (userport_joystick_enable && userport_joystick_type == USERPORT_JOYSTICK_HIT) {
        retval &= 0xfb;
        retval |= (BYTE)((get_joystick_value(3) & 0x10) ? 0 : 4);
    }
    return retval;
}

BYTE userport_joystick_read_pbx(BYTE orig)
{
    BYTE retval = orig;
    BYTE jv3;
    BYTE jv4;

    if (userport_joystick_enable) {
        jv3 = get_joystick_value(3);
        jv4 = get_joystick_value(4);
        switch (userport_joystick_type) {
            case USERPORT_JOYSTICK_HIT:
                retval = (BYTE)~((jv3 & 0xf) | ((jv4 & 0xf) << 4));
                break;
            case USERPORT_JOYSTICK_CGA:
                retval = (BYTE)~((jv3 & 0x10) | ((jv4 & 0x10) << 1)
                               | (get_joystick_value(userport_joystick_cga_select + 3) & 0xf));
                break;
            case USERPORT_JOYSTICK_PET:
                retval = ((jv3 & 0xf) | ((jv4 & 0xf) << 4));
                retval |= (jv3 & 0x10) ? 3 : 0;
                retval |= (jv4 & 0x10) ? 0x30 : 0;
                retval = (BYTE)~retval;
                break;
            case USERPORT_JOYSTICK_HUMMER:
                retval = (BYTE)~(jv3 & 0x1f);
                break;
            case USERPORT_JOYSTICK_OEM:
                retval = ((jv3 & 1) << 7);
                retval |= ((jv3 & 2) << 5);
                retval |= ((jv3 & 4) << 3);
                retval |= ((jv3 & 8) << 1);
                retval |= ((jv3 & 16) >> 1);
                retval = (BYTE)~!retval;
                break;
        }
    }
    return retval;
}

BYTE userport_joystick_read_sdr(BYTE orig)
{
    BYTE retval = orig;

    if (userport_joystick_enable && userport_joystick_type == USERPORT_JOYSTICK_HIT) {
        retval = userport_joystick_hit_button_sp2;
    }
    return retval;
}

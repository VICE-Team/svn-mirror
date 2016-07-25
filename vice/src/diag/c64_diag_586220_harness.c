/*
 * c64_diag_586220_harness.c - c64 diagnosis cartridge harness hub emulation.
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
#include <string.h>

#include "c64_diag_586220_harness.h"
#include "types.h"

static BYTE c64_diag_userport[C64_DIAG_USERPORT_PINS];
static BYTE c64_diag_tapeport[C64_DIAG_TAPEPORT_PINS];
static BYTE c64_diag_joyport0[C64_DIAG_JOYPORT_PINS];
static BYTE c64_diag_joyport1[C64_DIAG_JOYPORT_PINS];
static BYTE c64_diag_keyboard[C64_DIAG_KEYBOARD_PINS];
static BYTE c64_diag_serial[C64_DIAG_SERIAL_PINS];

void c64_diag_586220_init(void)
{
    memset(c64_diag_userport, 0, C64_DIAG_USERPORT_PINS);
    memset(c64_diag_tapeport, 0, C64_DIAG_TAPEPORT_PINS);
    memset(c64_diag_joyport0, 0, C64_DIAG_JOYPORT_PINS);
    memset(c64_diag_joyport1, 0, C64_DIAG_JOYPORT_PINS);
    memset(c64_diag_keyboard, 0, C64_DIAG_KEYBOARD_PINS);
    memset(c64_diag_serial, 0, C64_DIAG_SERIAL_PINS);
}

void c64_diag_586220_store_userport(BYTE pin, BYTE val)
{
    if (pin < C64_DIAG_USERPORT_PINS) {
        c64_diag_userport[pin] = val;
    }
}

void c64_diag_586220_store_tapeport(BYTE pin, BYTE val)
{
    if (pin < C64_DIAG_TAPEPORT_PINS) {
        c64_diag_tapeport[pin] = val;
    }
}

void c64_diag_586220_store_joyport(BYTE port, BYTE pin, BYTE val)
{
    if (port < 2 && pin < C64_DIAG_JOYPORT_PINS) {
        if (!port) {
            c64_diag_joyport0[pin] = val;
        } else {
            c64_diag_joyport1[pin] = val;
        }
    }
}

void c64_diag_586220_store_keyboard(BYTE pin, BYTE val)
{
    if (pin < C64_DIAG_KEYBOARD_PINS) {
        c64_diag_keyboard[pin] = val;
    }
}

void c64_diag_586220_store_serial(BYTE pin, BYTE val)
{
    if (pin < C64_DIAG_SERIAL_PINS) {
        c64_diag_serial[pin] = val;
    }
}

BYTE c64_diag_586220_read_userport(BYTE pin)
{
    switch (pin) {
        case C64_DIAG_USERPORT_CNT1:
            return c64_diag_userport[C64_DIAG_USERPORT_CNT2];
        case C64_DIAG_USERPORT_SP1:
            return c64_diag_userport[C64_DIAG_USERPORT_SP2];
        case C64_DIAG_USERPORT_CNT2:
            return c64_diag_userport[C64_DIAG_USERPORT_CNT1];
        case C64_DIAG_USERPORT_SP2:
            return c64_diag_userport[C64_DIAG_USERPORT_SP1];
        case C64_DIAG_USERPORT_PC2:
            return c64_diag_userport[C64_DIAG_USERPORT_FLAG2];
        case C64_DIAG_USERPORT_PA3:
            return c64_diag_userport[C64_DIAG_USERPORT_PA2];
        case C64_DIAG_USERPORT_FLAG2:
            return c64_diag_userport[C64_DIAG_USERPORT_PC2];
        case C64_DIAG_USERPORT_PB0:
            return c64_diag_userport[C64_DIAG_USERPORT_PB4];
        case C64_DIAG_USERPORT_PB1:
            return c64_diag_userport[C64_DIAG_USERPORT_PB5];
        case C64_DIAG_USERPORT_PB2:
            return c64_diag_userport[C64_DIAG_USERPORT_PB6];
        case C64_DIAG_USERPORT_PB3:
            return c64_diag_userport[C64_DIAG_USERPORT_PB7];
        case C64_DIAG_USERPORT_PB4:
            return c64_diag_userport[C64_DIAG_USERPORT_PB0];
        case C64_DIAG_USERPORT_PB5:
            return c64_diag_userport[C64_DIAG_USERPORT_PB1];
        case C64_DIAG_USERPORT_PB6:
            return c64_diag_userport[C64_DIAG_USERPORT_PB2];
        case C64_DIAG_USERPORT_PB7:
            return c64_diag_userport[C64_DIAG_USERPORT_PB3];
        case C64_DIAG_USERPORT_PA2:
            return c64_diag_userport[C64_DIAG_USERPORT_PA3];
    }
    return 0;
}

BYTE c64_diag_586220_read_tapeport(BYTE pin)
{
    switch (pin) {
        case C64_DIAG_TAPEPORT_MOTOR:
            return c64_diag_tapeport[C64_DIAG_TAPEPORT_MOTOR];
        case C64_DIAG_TAPEPORT_READ:
            return c64_diag_tapeport[C64_DIAG_TAPEPORT_SENSE];
        case C64_DIAG_TAPEPORT_WRITE:
            return c64_diag_tapeport[C64_DIAG_TAPEPORT_WRITE];
        case C64_DIAG_TAPEPORT_SENSE:
            return c64_diag_tapeport[C64_DIAG_TAPEPORT_READ];
    }
    return 0;
}

BYTE c64_diag_586220_read_joyport(BYTE port, BYTE pin)
{
    if (c64_diag_tapeport[C64_DIAG_TAPEPORT_MOTOR] && c64_diag_tapeport[C64_DIAG_TAPEPORT_WRITE]) {
        if (!port) {
            switch (pin) {
                case C64_DIAG_JOYPORT_UP:
                case C64_DIAG_JOYPORT_DOWN:
                case C64_DIAG_JOYPORT_LEFT:
                case C64_DIAG_JOYPORT_RIGHT:
                case C64_DIAG_JOYPORT_BUTTON:
                    return c64_diag_joyport1[pin];
            }
        } else {
            switch (pin) {
                case C64_DIAG_JOYPORT_UP:
                case C64_DIAG_JOYPORT_DOWN:
                case C64_DIAG_JOYPORT_LEFT:
                case C64_DIAG_JOYPORT_RIGHT:
                case C64_DIAG_JOYPORT_BUTTON:
                    return c64_diag_joyport0[pin];
            }
        }
    }

#if 0
    /* Still needs to be determined what comes through on the pins */
    switch (pin) {
        case C64_DIAG_JOYPORT_POTY:
        case C64_DIAG_JOYPORT_POTX:
    }
#endif

    return 1;
}

BYTE c64_diag_586220_read_keyboard(BYTE pin)
{
    switch (pin) {
        case C64_DIAG_KEYBOARD_PA0:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB0];
        case C64_DIAG_KEYBOARD_PA1:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB1];
        case C64_DIAG_KEYBOARD_PA2:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB2];
        case C64_DIAG_KEYBOARD_PA3:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB3];
        case C64_DIAG_KEYBOARD_PA4:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB4];
        case C64_DIAG_KEYBOARD_PA5:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB5];
        case C64_DIAG_KEYBOARD_PA6:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB6];
        case C64_DIAG_KEYBOARD_PA7:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PB7];
        case C64_DIAG_KEYBOARD_PB0:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA0];
        case C64_DIAG_KEYBOARD_PB1:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA1];
        case C64_DIAG_KEYBOARD_PB2:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA2];
        case C64_DIAG_KEYBOARD_PB3:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA3];
        case C64_DIAG_KEYBOARD_PB4:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA4];
        case C64_DIAG_KEYBOARD_PB5:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA5];
        case C64_DIAG_KEYBOARD_PB6:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA6];
        case C64_DIAG_KEYBOARD_PB7:
            return c64_diag_keyboard[C64_DIAG_KEYBOARD_PA7];
    }
    return 0;
}

BYTE c64_diag_586220_read_serial(BYTE pin)
{
    switch (pin) {
        case C64_DIAG_SERIAL_SRQ:
            return c64_diag_serial[C64_DIAG_SERIAL_DATA];
        case C64_DIAG_SERIAL_ATN:
            return c64_diag_serial[C64_DIAG_SERIAL_CLK];
        case C64_DIAG_SERIAL_CLK:
            return c64_diag_serial[C64_DIAG_SERIAL_ATN];
        case C64_DIAG_SERIAL_DATA:
            return c64_diag_serial[C64_DIAG_SERIAL_SRQ];
    }
    return 0;
}

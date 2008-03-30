/*
 * emuid.c -- Emulator ID for use in the different emulators.
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

#include "emuid.h"
#include "types.h"


/* Emulation identification string.  */
static BYTE emulator_id[] = {
    0x56, 0x49, 0x43, 0x45, 0x20, 0x31, 0x2e, 0x36,     /* VICE 1.6 */
    0x20, 0x20, 0x28, 0x43, 0x29, 0x20, 0x45, 0x2e,     /*   (c) E. */
    0x50, 0x45, 0x52, 0x41, 0x5a, 0x5a, 0x4f, 0x4c,     /* PERAZZOL */
    0x49, 0x2f, 0x41, 0x2e, 0x46, 0x41, 0x43, 0x48,     /* I/A.FACH */
    0x41, 0x54, 0x2f, 0x54, 0x2e, 0x52, 0x41, 0x4e,     /* AT/T.RAN */
    0x54, 0x41, 0x4e, 0x45, 0x4e, 0x2f, 0x44, 0x2e,     /* TANEN/D. */
    0x53, 0x4c, 0x41, 0x44, 0x49, 0x43, 0x2f, 0x41,     /* SLADIC/A */
    0x2e, 0x42, 0x4f, 0x4f, 0x53, 0x45, 0x2f, 0x4a,     /* .BOOSE/J */
    0x2e, 0x56, 0x41, 0x4c, 0x54, 0x41, 0x2f, 0x4a,     /* .VALTA/J */
    0x2e, 0x53, 0x4f, 0x4e, 0x4e, 0x49, 0x4e, 0x45,     /* .SONNINE */
    0x4e, 0x2f, 0x54, 0x2e, 0x42, 0x52, 0x45, 0x54,     /* N/T.BRET */
    0x5a, 0x00, 0x00, 0x00, 0x01, 0x01, 0x56, 0x55      /* Z.....VU */
};

BYTE REGPARM1 emuid_read(WORD addr)
{
    addr %= 0x60;

    if (addr == 0x5f)
        emulator_id[addr] ^= 0xff;

    return emulator_id[addr];
}


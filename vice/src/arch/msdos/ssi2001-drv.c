/*
 * ssi2001-drv.c - MSDOS specific SSI2001 (ISA SID card) driver.
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

#ifdef HAVE_SSI2001
#include <stdio.h>
#include <string.h>

#include "types.h"

#define SSI2008_BASE 0x280

static BYTE read_sid(BYTE reg)
{
    return inportb(SSI2008_BASE + (reg & 0x1f));
}

static void write_sid(BYTE reg, BYTE data)
{
    outportb(SSI2008_BASE + (reg & 0x1f), data);
}

int ssi2001_drv_open(void)
{
    int i;

    /* mute all sids */
    for (i = 0; i < 32; i++) {
        write_sid(i, 0);
    }
    return 0;
}

int ssi2001_drv_close(void)
{
    int i;

    /* mute all sids */
    for (i = 0; i < 32; i++) {
        write_sid(i, 0);
    }
    return 0;
}

/* read value from SIDs */
int ssi2001_drv_read(WORD addr, int chipno)
{
    /* if addr is from read-only register, perform a real read */
    if (addr >= 0x19 && addr <= 0x1C) {
        return read_sid(addr);
    }

    return 0;
}

/* write value into SID */
void ssi2001_drv_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (addr <= 0x18) {
        write_sid(addr, val);
    }
}
#endif

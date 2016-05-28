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
#include <dos.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

#define SSI2008_BASE 0x280

#define MAXSID 1

static int sids_found = -1;

static int is_windows_nt(void)
{
    unsigned short real_version;
    int version_major = -1;
    int version_minor = -1;

    real_version = _get_dos_version(1);
    version_major = real_version >> 8;
    version_minor = real_version & 0xff;

    if (version_major == 5 && version_minor == 50) {
#ifdef SSI2001_DEBUG
        printf("Working on windows NT, no ISA direct access possible\n");
#endif
        return 1;
    }
    return 0;
}

static BYTE read_sid(BYTE reg)
{
    return inportb(SSI2008_BASE + (reg & 0x1f));
}

static void write_sid(BYTE reg, BYTE data)
{
    outportb(SSI2008_BASE + (reg & 0x1f), data);
}

static int detect_sid(void)
{
    int i;

    if (is_windows_nt()) {
        return 0;
    }

    for (i = 0x18; i >= 0; --i) {
        write_sid((BYTE)i, 0);
    }

    write_sid(0x12, 0xff);

    for (i = 0; i < 100; ++i) {
        if (read_sid(0x1b)) {
            return 0;
        }
    }

    write_sid(0x0e, 0xff);
    write_sid(0x0f, 0xff);
    write_sid(0x12, 0x20);

    for (i = 0; i < 100; ++i) {
        if (read_sid(0x1b)) {
            return 1;
        }
    }
    return 0;
}

int ssi2001_drv_open(void)
{
    int i;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = -1;

    if (!detect_sid()) {
#ifdef SSI2001_DEBUG
        printf("NO SSI2001 found\n");
#endif
        return -1;
    }

    /* mute all sids */
    for (i = 0; i < 32; i++) {
        write_sid(i, 0);
    }
#ifdef SSI2001_DEBUG
    printf("SSI2001 detected at $280 using ISA direct access method\n");
#endif

    sids_found = 1;

    return 0;
}

int ssi2001_drv_close(void)
{
    int i;

    /* mute all sids */
    for (i = 0; i < 32; i++) {
        write_sid(i, 0);
    }

    sids_found = -1;

    return 0;
}

/* read value from SIDs */
int ssi2001_drv_read(WORD addr, int chipno)
{
    /* if addr is from read-only register, perform a real read */
    if (chipno < MAXSID && addr < 0x20) {
        return read_sid(addr);
    }

    return 0;
}

/* write value into SID */
void ssi2001_drv_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && addr < 0x20) {
        write_sid(addr, val);
    }
}

int ssi2001_drv_available(void)
{
    return sids_found;
}
#endif

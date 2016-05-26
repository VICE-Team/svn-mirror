/*
 * hs-isa.c - Unix specific ISA hardsid driver.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, modified from the sidplay2 sources.  It is
 * a one for all driver with real timing support via real time kernel
 * extensions or through the hardware buffering.  It supports the hardsid
 * isa/pci single/quattro and also the catweasel MK3/4.
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

#if defined(HAVE_HARDSID) && defined(HAVE_HARDSID_ISA)

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "hardsid.h"
#include "io-access.h"
#include "types.h"

#define HARDSID_BASE 0x300

/* static int hardsid_open_status = 0; */
static int hs_available = 0;

void hs_isa_store(WORD addr, BYTE value, int chipno)
{
    io_access_store(HARDSID_BASE, value);
    io_access_store(HARDSID_BASE + 1, addr & 0x1f);
}

BYTE hs_isa_read(WORD addr, int chipno)
{
    io_access_store(HARDSID_BASE + 1, (addr & 0x1f) | 0x20);
    usleep(2);
    return io_access_read(HARDSID_BASE);
}

static int detect_sid(void)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        hs_isa_store((WORD)i, 0, 0);
    }

    hs_isa_store(0x12, 0xff, 0);

    for (i = 0; i < 100; ++i) {
        if (hs_isa_read(0x1b, 0)) {
            return 0;
        }
    }

    hs_isa_store(0x0e, 0xff, 0);
    hs_isa_store(0x0f, 0xff, 0);
    hs_isa_store(0x12, 0x20, 0);

    for (i = 0; i < 100; ++i) {
        if (hs_isa_read(0x1b, 0)) {
            return 1;
        }
    }
    return 0;
}

int hs_isa_open(void)
{
    if (io_access_map(HARDSID_BASE, 2) < 0) {
        return -1;
    }

    if (detect_sid()) {
        return 0;
    }
    return -1;
}

int hs_isa_close(void)
{
    io_access_unmap(HARDSID_BASE, 2);

    return 0;
}

int hs_isa_available(void)
{
    if (hs_available) {
        return 1;
    }

    if (hs_isa_open() < 0) {
        return 0;
    }
    hs_isa_close();
    hs_available = 1;
    return 1;
}

/* ---------------------------------------------------------------------*/

void hs_isa_state_read(int chipno, struct sid_hs_snapshot_state_s *sid_state)
{
    sid_state->hsid_main_clk = 0;
    sid_state->hsid_alarm_clk = 0;
    sid_state->lastaccess_clk = 0;
    sid_state->lastaccess_ms = 0;
    sid_state->lastaccess_chipno = 0;
    sid_state->chipused = 0;
    sid_state->device_map[0] = 0;
    sid_state->device_map[1] = 0;
}

void hs_isa_state_write(int chipno, struct sid_hs_snapshot_state_s *sid_state)
{
}
#endif

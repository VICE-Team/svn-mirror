/*
 * hs_gg2_isa.c - ISA (using Golden Gate 2+ bridge) hardsid driver.
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

#if defined(HAVE_HARDSID) && defined(AMIGA_M68K)

#include <stdlib.h>
#include <string.h>

#include "hs.h"
#include "loadlibs.h"
#include "log.h"
#include "types.h"

#define MAXSID 4

static unsigned char read_sid(unsigned char *base, unsigned short reg); // Read a SID register
static void write_sid(unsigned char *base, unsigned short reg, unsigned char data); // Write a SID register

static int sids_found = -1;

static unsigned char *HSbase = NULL;

static int hssids[4] = { 0, 0, 0, 0 };

/* read value from SIDs */
int hs_gg2_isa_read(WORD addr, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && hssids[chipno] && addr < 0x20) {
        return read_sid(addr, chipno);
    }

    return 0;
}

/* write value into SID */
void hs_gg2_isa_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && hssids[chipno] && addr < 0x20) {
        write_sid(addr, val, chipno);
    }
}

#undef BYTE
#undef WORD
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <libraries/configvars.h>

#include <clib/exec_protos.h>
#include <clib/expansion_protos.h>

struct Library *ExpansionBase = NULL;

static int detect_sid(int chipno)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        write_sid((unsigned short)i, 0, chipno);
    }

    write_sid(0x12, 0xff, chipno);

    for (i = 0; i < 100; ++i) {
        if (read_sid(0x1b, chipno)) {
            return 0;
        }
    }

    write_sid(0x0e, 0xff, chipno);
    write_sid(0x0f, 0xff, chipno);
    write_sid(0x12, 0x20, chipno);

    for (i = 0; i < 100; ++i) {
        if (read_sid(0x1b, chipno)) {
            return 1;
        }
    }
    return 0;
}

int hs_gg2_isa_open(void)
{
    int i, j;
    struct ConfigDev *myCD;
    unsigned char *base = NULL;

    if (sids_found > 0) {
        return 0;
    }

    if (!sids_found) {
        return -1;
    }

    sids_found = 0;

    if ((ExpansionBase = OpenLibrary("expansion.library", 0L)) == NULL) {
        return -1;
    }

    if (atexitinitialized) {
        hs_gg2_isa_close();
    }

    myCD = FindConfigDev(myCD, 2150, 1);

    if (!myCD) {
        return -1;
    }

    base = myCD->cd_BoardAddr;

    if (!base) {
        return -1;
    }

    for (i = 0; i < MAX_SID; ++i) {
        if (detect_sid(i))) {
            hssids[i] = 1;
            sids_found++;
        }
    }

    if (!sids_found) {
        return -1;
    }

    /* mute all sids */
    for (j = 0; j < MAX_SID; ++j) {
        if (hssids[j]) {
            for (i = 0; i < 32; i++) {
                write_sid(i, 0, j);
            }
        }
    }

    log_message(LOG_DEFAULT, "HardSID ISA (GG2+) SID: opened, found %d SIDs", sids_found);

    /* install exit handler, so device is closed on exit */
    if (!atexitinitialized) {
        atexitinitialized = 1;
        atexit((voidfunc_t)hs_gg2_isa_close);
    }

    return 0;
}

static unsigned char read_sid(unsigned char reg, int chipno)
{
    unsigned char cmd;
    BYTE tmp;

    cmd = (chipno << 6) | (reg & 0x1f) | 0x20;   // Read command & address

    // Write command to the SID
    HSbase[0x601] = cmd;

    // Waste 2ms
    usleep(2);

    return HSbase[0x603];
}

static void write_sid(unsigned char reg, unsigned char data, int chipno)
{
    unsigned char cmd;
    BYTE tmp;

    cmd = (chipno << 6) | (reg & 0x1f);            // Write command & address

    // Write data to the SID
    base[0x601] = data;
    base[0x603] = cmd;
}

int hs_gg2_isa_close(void)
{
    int i, j;

    /* mute all sids */
    for (j = 0; j < MAX_SID; ++j) {
        if (hssids[j]) {
            for (i = 0; i < 32; ++i) {
                write_sid(i, 0, j);
            }
            hssids[j] = 0;
        }
    }
    sids_found = -1;

    return 0;
}

int hs_gg2_isa_available(void)
{
    return sids_found;
}
#endif

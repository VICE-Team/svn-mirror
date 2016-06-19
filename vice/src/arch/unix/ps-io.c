/*
 * ps-io.c - I/O based PARallel port SID support for UNIX.
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

#ifdef HAVE_PARSID
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "alarm.h"
#include "io-access.h"
#include "log.h"
#include "parsid.h"
#include "ps.h"
#include "sid-resources.h"
#include "types.h"

#define MAXSID 3

static unsigned int ports[3] = {-1, -1, -1};
static int pssids[3] = {-1, -1, -1};
static int sids_found = -1;

void ps_io_out_ctr(BYTE parsid_ctrport, int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        io_access_store(pssids[chipno] + 2, parsid_ctrport);
    }
}

BYTE ps_io_in_ctr(int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        return io_access_read(pssids[chipno] + 2);
    }
    return 0;
}

static void parsid_get_ports(void)
{
    ports[0] = 0x3bc;
    ports[1] = 0x378;
    ports[2] = 0x278;
}

BYTE ps_io_in_data(int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        return io_access_read(pssids[chipno]);
    }
    return 0;
}

void ps_io_out_data(BYTE outval, int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        io_access_store(pssids[chipno], outval);
    }
}

static BYTE detect_sid_read(WORD addr, WORD base)
{
    BYTE value = 0;
    BYTE ctl = io_access_read(base + 2);

    io_access_store(base, addr & 0x1f);

    ctl &= ~parsid_AUTOFEED;
    io_access_store(base + 2, ctl);

    ctl |= parsid_AUTOFEED;
    io_access_store(base + 2, ctl);

    ctl |= parsid_PCD;
    io_access_store(base + 2, ctl);

    ctl |= parsid_nINIT;
    io_access_store(base + 2, ctl);

    ctl |= parsid_STROBE;
    io_access_store(base + 2, ctl);

    value = io_access_read(base);

    ctl &= ~parsid_STROBE;
    io_access_store(base + 2, ctl);

    return value;
}

static void detect_sid_store(WORD addr, BYTE outval, WORD base)
{
    BYTE ctl = io_access_read(base + 2);

    io_access_store(base, (addr & 0x1f));

    ctl &= ~parsid_AUTOFEED;
    io_access_store(base + 2, ctl);

    ctl |= parsid_AUTOFEED;
    io_access_store(base + 2, ctl);

    io_access_store(base, outval);

    ctl |= parsid_STROBE;
    io_access_store(base + 2, ctl);

    ctl &= ~parsid_STROBE;
    io_access_store(base + 2, ctl);
}

static int detect_sid(WORD addr)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        detect_sid_store(i, 0, addr);
    }

    detect_sid_store(0x12, 0xff, addr);

    for (i = 0; i < 100; ++i) {
        if (detect_sid_read(0x1b, addr)) {
            return 0;
        }
    }

    detect_sid_store(0x0e, 0xff, addr);
    detect_sid_store(0x0f, 0xff, addr);
    detect_sid_store(0x12, 0x20, addr);

    for (i = 0; i < 100; ++i) {
        if (detect_sid_read(0x1b, addr)) {
            return 1;
        }
    }
    return 0;
}

int ps_io_open(void)
{
    int i = 0;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    log_message(LOG_DEFAULT, "Detecting direct I/O ParSIDs.");

    parsid_get_ports();

    for (i = 0; i < MAXSID; ++i) {
        if (!io_access_map(ports[i], 3)) {
           if (detect_sid(ports[i])) {
                pssids[sids_found] = ports[i];
                sids_found++;
                log_message(LOG_DEFAULT, "ParSID found at %X.", ports[i]);
            } else {
                log_message(LOG_DEFAULT, "No ParSID at %X.", ports[i]);
            }
        } else {
            log_message(LOG_DEFAULT, "Could not get permission to access %X.", ports[i]);
        }
    }

    if (!sids_found) {
        return -1;
    }

    log_message(LOG_DEFAULT, "Direct I/O ParSID: opened, found %d SIDs.", sids_found);

    return 0;
}

int ps_io_close(void)
{
    int i;

    for (i = 0; i < MAXSID; ++i) {
        if (pssids[i] != -1) {
            io_access_unmap(pssids[i], 3);
            pssids[i] = -1;
        }
        ports[i] = -1;
    }

    sids_found = -1;

    log_message(LOG_DEFAULT, "Direct I/O ParSID: closed.");

    return 0;
}

int ps_io_available(void)
{
    return sids_found;
}
#endif

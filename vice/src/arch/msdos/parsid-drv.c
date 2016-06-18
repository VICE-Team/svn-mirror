/*
 * parsid-drv.c - PARallel port SID support for MSDOS.
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
#include <dos.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <pc.h>
#include <dpmi.h>
#include <libc/farptrgs.h>
#include <go32.h>

#include "alarm.h"
#include "parsid.h"
#include "log.h"
#include "sid-resources.h"
#include "types.h"

#define MAXSID 3

static unsigned int ports[3] = {-1, -1, -1};
static int pssids[3] = {-1, -1, -1};
static int sids_found = -1;

/* input/output functions */
static BYTE parsid_inb(WORD addr)
{
    return inportb(addr);
}

static void parsid_outb(WORD addr, BYTE value)
{
    outportb(addr, value);
}

void parsid_drv_out_ctr(BYTE parsid_ctrport, int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        parsid_outb(pssids[chipno] + 2, parsid_ctrport);
    }
}

BYTE parsid_drv_in_ctr(int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        return parsid_inb(pssids[chipno] + 2);
    }
    return 0;
}

static int is_windows_nt(void)
{
    unsigned short real_version;
    int version_major = -1;
    int version_minor = -1;

    real_version = _get_dos_version(1);
    version_major = real_version >> 8;
    version_minor = real_version & 0xff;

    if (version_major == 5 && version_minor == 50) {
        return 1;
    }
    return 0;
}

static void parsid_get_ports(void)
{
    int j;
    unsigned long ptraddr = 0x0408;		/* Base Address: segment is zero */
    unsigned int address;				/* Address of Port */

    if (is_windows_nt()) {
        return;
    }

    for (j = 0; j < 3; j++) {
        address = _farpeekw(_dos_ds, ptraddr + (j * 4));
        if (address != 0) {
            ports[j] = address;
            log_message(LOG_DEFAULT, "Parallel port found at %X.", address);
        }
    }
}

static BYTE detect_sid_read(WORD addr, WORD base)
{
    BYTE value = 0;
    BYTE ctl = parsid_inb(base + 2);

    parsid_outb(base, addr & 0x1f);
    
    ctl &= ~parsid_AUTOFEED;
    parsid_outb(base + 2, ctl);

    ctl |= parsid_AUTOFEED;
    parsid_outb(base + 2, ctl);

    ctl |= parsid_PCD;
    parsid_outb(base + 2, ctl);

    ctl |= parsid_nINIT;
    parsid_outb(base + 2, ctl);

    ctl |= parsid_STROBE;
    parsid_outb(base + 2, ctl);

    value = parsid_inb(base);

    ctl &= ~parsid_STROBE;
    parsid_outb(base + 2, ctl);

    return (int)value;
}

static void detect_sid_store(WORD addr, BYTE outval, WORD base)
{
    BYTE ctl = parsid_inb(base + 2);

    parsid_outb(base, (addr & 0x1f));

    ctl &= ~parsid_AUTOFEED;
    parsid_outb(base + 2, ctl);

    ctl |= parsid_AUTOFEED;
    parsid_outb(base + 2, ctl);

    parsid_outb(base, outval);

    ctl |= parsid_STROBE;
    parsid_outb(base + 2, ctl);

    ctl &= ~parsid_STROBE;
    parsid_outb(base + 2, ctl);
}

static int detect_sid(WORD addr)
{
    int i;

    if (is_windows_nt()) {
        log_message(LOG_DEFAULT, "Running on Windows NT, cannot use direct memory access.");
        return 0;
    }

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

int parsid_drv_open(void)
{
    int i = 0;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    log_message(LOG_DEFAULT, "Detecting ParSIDs.");

    parsid_get_ports();

    for (i = 0; i < MAXSID; ++i) {
        if (ports[i] != -1) {
            if (detect_sid(ports[i])) {
                pssids[sids_found] = ports[i];
                sids_found++;
                log_message(LOG_DEFAULT, "ParSID found on port at address $%X.", ports[i]);
            }
        }
    }

    if (!sids_found) {
        log_message(LOG_DEFAULT, "No ParSIDs found.");
        return -1;
    }

    log_message(LOG_DEFAULT, "ParSID: opened, found %d SIDs.", sids_found);

    return 0;
}

int parsid_drv_close(void)
{
    int i;

    for (i = 0; i < MAXSID; ++i) {
        pssids[i] = -1;
        ports[i] = -1;
    }

    sids_found = -1;

    log_message(LOG_DEFAULT, "ParSID: closed.");

    return 0;
}

BYTE parsid_drv_in_data(int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        return parsid_inb(pssids[chipno]);
    }
    return 0;
}

void parsid_drv_out_data(BYTE outval, int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != -1) {
        parsid_outb(pssids[chipno], outval);
    }
}

void parsid_drv_sleep(int amount)
{
    sleep(amount);
}

int parsid_drv_available(void)
{
    return sids_found;
}
#endif

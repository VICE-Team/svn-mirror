/*
 * hd-pci.c - MSDOS specific PCI hardsid driver.
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

#ifdef HAVE_HARDSID

#include <stdio.h>
#include <dpmi.h>
#include <string.h>

#include "hs.h"
#include "log.h"
#include "pci-drv.h"
#include "sid-snapshot.h"
#include "types.h"

#define MAXSID 4

typedef unsigned short uint16;
typedef unsigned long uint32;

static unsigned int base;

static int sids_found = -1;
static int hssids[MAXSID] = {-1, -1, -1, -1};

static int pci_find_hardsid(int index)
{
    int i = 0, j = 0, res;
    int bus, device, func;
    uint32 baseAddr;

    if (pci_install_check() != 0) {
        return -1;
    }

    while (i <= index) {

        /* Find the HardSID card */
        res = pci_find(0x6581, 0x8580, j++, &bus, &device, &func);
        if (res != 0) {
            return -1;
        }

        i++;
    }

    for (i = 0x10; i <= 0x24; i += 4) {

        /* Read a base address */
        res = pci_read_config_dword(bus, device, func, i, &baseAddr);
        if (res != 0) {
            return -1;
        }

        /* Check for I/O space */
        if (baseAddr & 1) {
            return baseAddr & ~3;
        }
    }

    return -1;
}

static BYTE read_sid(BYTE reg, int chipno)
{
    BYTE ret;

    outportb(base + 4, ((chipno << 6) | (reg & 0x1f) | 0x20));
    usleep(2);
    outportb(base, 0x20);
    ret = inportb(base);
    outportb(base, 0x80);

    return ret;
}

static void write_sid(BYTE reg, BYTE data, int chipno)
{
    outportw(base + 3, ((chipno << 14) | (reg & 0x1f) << 8) | data);
    usleep(2);
}

static int detect_sid(int chipno)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        write_sid((BYTE)i, 0, chipno);
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

int hs_pci_open(void)
{
    int i, j;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    base = pci_find_hardsid(0);

    if (base == -1) {
        return -1;
    }

    /* Reset the hardsid PCI interface (as per hardsid linux driver) */
    outportb(base + 0x00, 0xff);
    outportb(base + 0x02, 0x00);
    usleep(100);
    outportb(base + 0x02, 0x24);

    for (j = 0; j < MAXSID; ++j) {
        if (detect_sid(j)) {
            hssids[sids_found] = j;
            sids_found++;
        }
    }

    /* mute all sids */
    for (j = 0; j < MAXSID; ++j) {
        if (hssids[j] != -1) {
            for (i = 0; i < 32; i++) {
                write_sid((BYTE)i, 0, hssids[j]);
            }
        }
    }

    log_message(LOG_DEFAULT, "HardSID: opened");

    return 0;
}

int hs_pci_close(void)
{
    int i, j;

    /* mute all sids */
    for (j = 0; j < MAXSID; ++j) {
        if (hssids[j] != -1) {
            for (i = 0; i < 32; i++) {
                write_sid((BYTE)i, 0, hssids[j]);
            }
        }
        hssids[j] = -1;
    }

    log_message(LOG_DEFAULT, "HardSID: closed");

    sids_found = -1;

    return 0;
}

/* read value from SIDs */
int hs_pci_read(WORD addr, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && hssids[chipno] != -1 && addr < 0x20) {
        return read_sid(addr, hssids[chipno]);
    }
    return 0;
}

/* write value into SID */
void hs_pci_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && hssids[chipno] != -1 && addr < 0x20) {
        write_sid(addr, val, hssids[chipno]);
    }
}

int hs_pci_available(void)
{
    return sids_found;
}
#endif

/*
 * hs_openpci.c - Amiga specific OpenPCI hardsid driver.
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

#if defined(HAVE_PROTO_OPENPCI_H) && defined(HAVE_HARDSID)

#include <stdlib.h>
#include <string.h>

#include "hardsid.h"
#include "loadlibs.h"
#include "log.h"
#include "types.h"

#define MAXSID 1

static int hs_found = 0;

static unsigned char read_sid(unsigned char reg); // Read a SID register
static void write_sid(unsigned char reg, unsigned char data); // Write a SID register

static int sidfh = 0;

typedef void (*voidfunc_t)(void);

static unsigned long HSbase;

/* read value from SIDs */
int hs_openpci_read(WORD addr, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && addr < 0x20) {
        /* if addr is from read-only register, perform a real read */
        if (addr >= 0x19 && addr <= 0x1C && sidfh >= 0) {
            return read_sid(addr);
        }
    }

    return 0;
}

/* write value into SID */
void hs_openpci_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && addr <= 0x18) {
        /* if the device is opened, write to device */
        if (sidfh >= 0) {
            write_sid(addr, val);
        }
    }
}

#undef BYTE
#undef WORD
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/openpci.h>
#include <libraries/openpci.h>

#if defined(pci_obtain_card) && defined(pci_release_card)
static int HSLock = FALSE;
#endif

static struct pci_dev *dev = NULL;

int hs_openpci_open(void)
{
    static int atexitinitialized = 0;
    unsigned int i;
    unsigned char bus = 0;

    if (hs_found != 0) {
        return (hs_found == 1) ? 0 : -1;
    }

    if (!pci_lib_loaded) {
        hs_found = -1;
        return -1;
    }

    if (atexitinitialized) {
        hardsid_drv_close();
    }

    bus = pci_bus();

    if (!bus) {
        log_message(LOG_DEFAULT, "No PCI bus found\n");
        hs_found = -1;
        return -1;
    }

    dev = pci_find_device(0x6581, 0x8580, NULL);

    if (dev == NULL) {
        log_message( LOG_DEFAULT, "Unable to find a HardSID PCI card\n" );
        hs_found = -1;
        return -1;
    }

#if defined(pci_obtain_card) && defined(pci_release_card)
    // Lock the device, since we're a driver
    HSLock = pci_obtain_card(dev);
    if (!HSLock) {
        log_message(LOG_DEFAULT, "Unable to lock the hardsid. Another driver may have an exclusive lock\n" );
        hs_found = -1;
        return -1;
    }
#endif

    HSbase = dev->base_address[0];

    // Reset the hardsid PCI interface (as per hardsid linux driver)
    pci_outb(0xff, HSbase + 0x00);
    pci_outb(0x00, HSbase + 0x02);
    usleep(100);
    pci_outb(0x24, HSbase + 0x02);

    /* mute all sids */
    for (i = 0; i < 32; i++) {
        write_sid(i, 0);
    }

    log_message(LOG_DEFAULT, "HardSID PCI: opened");

    /* install exit handler, so device is closed on exit */
    if (!atexitinitialized) {
        atexitinitialized = 1;
        atexit((voidfunc_t)hardsid_drv_close);
    }

    sidfh = 1; /* ok */

    hs_found = 1;

    return 0;
}

static unsigned char read_sid(unsigned char reg)
{
    unsigned char ret;

    pci_outb((reg & 0x1f) | 0x20, HSbase + 4);
    usleep(2);
    pci_outb(0x20, HSbase);
    ret = pci_inb(HSbase);
    pci_outb(0x80, HSbase);
    return ret;
}

static void write_sid(unsigned char reg, unsigned char data)
{
    pci_outw(((reg & 0x1f) << 8) | data, HSbase + 3);
}

int hs_openpci_close(void)
{
    unsigned int i;

    if (hs_found == 1) {
        /* mute all sids */
        for (i = 0; i < sizeof(sidbuf); i++) {
            write_sid(i, 0);
        }
    }

#if defined(pci_obtain_card) && defined(pci_release_card)
    if (HSLock) {
        pci_release_card(dev);
    }
#endif

    log_message(LOG_DEFAULT, "HardSID PCI: closed");

    hs_found = 0;

    return 0;
}
#endif


int hs_openpci_available(void)
{
    if (!hs_found) {
        hs_openpci_open();
    }
    return (hs_found == 1) ? 1 : 0;
}
#endif

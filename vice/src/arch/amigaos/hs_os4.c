/*
 * hs_os4.c - AmigaOS 4.x specific PCI hardsid driver.
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

#if defined(HAVE_HARDSID) && defined(AMIGA_OS4)

#include <stdlib.h>
#include <string.h>

#include "hardsid.h"
#include "loadlibs.h"
#include "log.h"
#include "types.h"

static int hs_found = 0;

static unsigned char read_sid(unsigned char reg); // Read a SID register
static void write_sid(unsigned char reg, unsigned char data); // Write a SID register

typedef void (*voidfunc_t)(void);

#define MAXSID 1

static int sidfh = 0;

/* read value from SIDs */
int hs_os4_read(WORD addr, int chipno)
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
void hs_os4_store(WORD addr, BYTE val, int chipno)
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
#include <proto/expansion.h>
#include <proto/exec.h>

static struct PCIIFace *IPCI = NULL;

static struct PCIDevice *HSDevPCI = NULL;
static struct PCIResourceRange *HSDevBAR = NULL;
int HSLock = FALSE;

int hs_os4_open(void)
{
    static int atexitinitialized = 0;
    unsigned int i;

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

    IPCI = (struct PCIIFace *)IExec->GetInterface(ExpansionBase, "pci", 1, NULL);
    if (!IPCI) {
        log_message(LOG_DEFAULT, "Unable to obtain PCI expansion interface\n");
        hs_found = -1;
        return -1;
    }

    // Try and find a HS on the PCI bus
    HSDevPCI = IPCI->FindDeviceTags(FDT_VendorID, 0x6581,
                                    FDT_DeviceID, 0x8580,
                                    FDT_Index,    0,
                                    TAG_DONE);
    if (!HSDevPCI) {
        log_message(LOG_DEFAULT, "Unable to find a HardSID PCI card\n");
        hs_found = -1;
        return -1;
    }

    // Lock the device, since we're a driver
    HSLock = HSDevPCI->Lock(PCI_LOCK_SHARED);
    if (!HSLock) {
        log_message(LOG_DEFAULT, "Unable to lock the hardsid. Another driver may have an exclusive lock\n");
        hs_found = -1;
        return -1;
    }

    // Get the resource range
    HSDevBAR = HSDevPCI->GetResourceRange(0);
    if (!HSDevBAR) {
        log_message(LOG_DEFAULT, "Unable to get resource range 0\n");
        hs_found = -1;
        return -1;
    }

    // Reset the hardsid PCI interface (as per hardsid linux driver)
    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 0x00, 0xff);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 0x02, 0xff);
    usleep(100);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 0x02, 0x24);

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

int hs_os4_close(void)
{
    unsigned int i;

    if (hs_found == 1) {
        /* mute all sids */
        for (i = 0; i < 32; i++) {
            write_sid(i, 0);
        }
    }

    if (HSDevBAR) {
        HSDevPCI->FreeResourceRange(HSDevBAR);
    }
    if (HSLock) {
        HSDevPCI->Unlock();
    }
    if (IPCI) {
        IExec->DropInterface((struct Interface *)IPCI);
    }

    log_message(LOG_DEFAULT, "HardSID PCI: closed");

    hs_found = 0;

    return 0;
}

static unsigned char read_sid(unsigned char reg)
{
    unsigned char ret;

    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 4, ((reg & 0x1f) | 0x20));
    usleep(2);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress, 0x20);
    ret = HSDevPCI->InByte(HSDevBAR->BaseAddress);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress, 0x80);

    return ret;
}

static void write_sid(unsigned char reg, unsigned char data)
{
    HSDevPCI->OutWord(HSDevBAR->BaseAddress + 3, ((reg & 0x1f) << 8) | data);
}

int hs_os4_available(void)
{
    if (!hs_found) {
        hs_os4_open();
    }
    return (hs_found == 1) ? 1 : 0;
}
#endif

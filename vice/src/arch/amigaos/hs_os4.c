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


static unsigned char read_sid(unsigned char reg, int chipno); // Read a SID register
static void write_sid(unsigned char reg, unsigned char data, int chipno); // Write a SID register

#define MAXSID 4

static int sids_found = -1;

static int hssids[MAXSID] = {-1, -1, -1, -1};

/* read value from SIDs */
int hs_os4_read(WORD addr, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && hssids[chipno] != -1 && addr < 0x20) {
        return read_sid(addr, hssids[chipno]);
    }
    return 0;
}

/* write value into SID */
void hs_os4_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && hssids[chipno] != -1 && addr < 0x20) {
        write_sid(addr, val, hssids[chipno]);
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

int hs_os4_open(void)
{
    unsigned int i, j;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    log_message(LOG_DEFAULT, "Detecting PCI HardSID boards.");

    if (!pci_lib_loaded) {
        log_message(LOG_DEFAULT, "Expansion library not available.");
        return -1;
    }

    IPCI = (struct PCIIFace *)IExec->GetInterface(ExpansionBase, "pci", 1, NULL);
    if (!IPCI) {
        log_message(LOG_DEFAULT, "Unable to obtain PCI expansion interface.");
        return -1;
    }

    // Try and find a HS on the PCI bus
    HSDevPCI = IPCI->FindDeviceTags(FDT_VendorID, 0x6581,
                                    FDT_DeviceID, 0x8580,
                                    FDT_Index,    0,
                                    TAG_DONE);
    if (!HSDevPCI) {
        log_message(LOG_DEFAULT, "Unable to find a PCI HardSID board.");
        return -1;
    }

    // Lock the device, since we're a driver
    HSLock = HSDevPCI->Lock(PCI_LOCK_SHARED);
    if (!HSLock) {
        log_message(LOG_DEFAULT, "Unable to lock the PCI HardSID. Another driver may have an exclusive lock.");
        return -1;
    }

    // Get the resource range
    HSDevBAR = HSDevPCI->GetResourceRange(0);
    if (!HSDevBAR) {
        log_message(LOG_DEFAULT, "Unable to get PCI HardSID resource range 0.");
        return -1;
    }

    // Reset the hardsid PCI interface (as per hardsid linux driver)
    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 0x00, 0xff);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 0x02, 0xff);
    usleep(100);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 0x02, 0x24);

    for (j = 0; j < MAXSID; ++j) {
        if (detect_sid(j)) {
            hssids[sids_found] = j;
            sids_found++;
        }
    }

    if (!sids_found) {
        log_message(LOG_DEFAULT, "No PCI HardSID boards found.");
        return -1;
    }

    /* mute all sids */
    for (j = 0; j < MAXSID; ++j) {
        if (hssids[j] != -1) {
            for (i = 0; i < 32; i++) {
                write_sid(i, 0, hssids[j]);
            }
        }
    }

    log_message(LOG_DEFAULT, "PCI HardSID: opened at $%X with %d SIDs.", HSDevBAR->BaseAddress, sids_found);

    return 0;
}

int hs_os4_close(void)
{
    unsigned int i, j;

    /* mute all sids */
    for (j = 0; j < MAXSID; ++j) {
        if (hssids[j] != -1) {
            for (i = 0; i < 32; i++) {
                write_sid(i, 0, hssids[j]);
            }
        }
        hssids[j] = -1;
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

    log_message(LOG_DEFAULT, "PCI HardSID: closed.");

    sids_found = -1;

    return 0;
}

static unsigned char read_sid(unsigned char reg, int chipno)
{
    unsigned char ret;

    HSDevPCI->OutByte(HSDevBAR->BaseAddress + 4, ((chipno << 6) | (reg & 0x1f) | 0x20));
    usleep(2);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress, 0x20);
    ret = HSDevPCI->InByte(HSDevBAR->BaseAddress);
    HSDevPCI->OutByte(HSDevBAR->BaseAddress, 0x80);

    return ret;
}

static void write_sid(unsigned char reg, unsigned char data, int chipno)
{
    HSDevPCI->OutWord(HSDevBAR->BaseAddress + 3, ((chipno << 14) | (reg & 0x1f) << 8) | data);
}

int hs_os4_available(void)
{
    return sids_found;
}
#endif

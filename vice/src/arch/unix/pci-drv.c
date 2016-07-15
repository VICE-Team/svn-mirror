/*
 * pci-drv.c - unix specific PCI driver code.
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

#include <stdio.h>
#include <string.h>

#include "io-access.h"
#include "log.h"
#include "types.h"

#ifdef __linux
static int pci_get_linux_proc_base(int vendorID, int deviceID, DWORD *base1, DWORD *base2)
{
    FILE *f;
    char buf[512];
    DWORD cnt, dfn, vend, irq, b1, b2;

    f = fopen("/proc/bus/pci/devices", "r");

    if (!f) {
        return -1;
    }

    while (fgets(buf, sizeof(buf) - 1, f)) {
        cnt = sscanf(buf, "%x %x %x %x %x", &dfn, &vend, &irq, &b1, &b2);
        if (cnt == 5) {
            if (vend == (deviceID | (vendorID << 16))) {
                *base1 = b1;
                *base2 = b2;
                fclose(f);
                return 0;
            }
        }
    }

    fclose(f);

    return -1;
}
#endif

int pci_get_base(int vendorID, int deviceID, DWORD *base1, DWORD *base2);
{
    int retval = -1;

#ifdef __linux
    retval = pci_get_linux_proc_base(vendorID, deviceID, base1, base2);
#endif

    return retval;
}

/*
 * catweaselmkiii-drv.c - Unix specific cw3 driver.
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

/* Tested and confirmed working on:

 - Linux 2.6 (driver)
 - Linux 2.6 (/dev/port PCI I/O access)
 - Linux 2.6 (permission based PCI I/O access)
 */

#include "vice.h"

#ifdef HAVE_CATWEASELMKIII


#include "catweaselmkiii.h"
#include "cw.h"
#include "types.h"

static int use_cw_device = 0;
static int use_cw_pci = 0;

int catweaselmkiii_drv_open(void)
{
    if (!cw_device_open()) {
        use_cw_device = 1;
        return 0;
    }

    if (!cw_pci_open()) {
        use_cw_pci = 1;
        return 0;
    }
    return -1;
}

int catweaselmkiii_drv_available(void)
{
    if (use_cw_device) {
        return cw_device_available();
    }

    if (use_cw_pci) {
        return cw_pci_available();
    }

    return 0;
}

int catweaselmkiii_drv_close(void)
{
    if (use_cw_device) {
        cw_device_close();
        use_cw_device = 0;
    }

    if (use_cw_pci) {
        cw_pci_close();
        use_cw_pci = 0;
    }

    return 0;
}

int catweaselmkiii_drv_read(WORD addr, int chipno)
{
    if (use_cw_device) {
        return cw_device_read(addr, chipno);
    }

    if (use_cw_pci) {
        return cw_pci_read(addr, chipno);
    }

    return 0;
}

void catweaselmkiii_drv_store(WORD addr, BYTE val, int chipno)
{
    if (use_cw_device) {
        cw_device_store(addr, val, chipno);
    }

    if (use_cw_pci) {
        cw_pci_store(addr, val, chipno);
    }
}

void catweaselmkiii_drv_set_machine_parameter(long cycles_per_sec)
{
    if (use_cw_device) {
        cw_device_set_machine_parameter(cycles_per_sec);
    }

    if (use_cw_pci) {
        cw_pci_set_machine_parameter(cycles_per_sec);
    }
}
#endif

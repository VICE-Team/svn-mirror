/*
 * hardsid-drv.c - MSDOS specific PCI hardsid driver.
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

#include "log.h"
#include "sid-snapshot.h"
#include "types.h"

typedef unsigned short uint16;
typedef unsigned long uint32;

static int hs_available = 0;

static int base;

static int sidfh = -1;

static int pci_install_check(void)
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(r));

    r.x.ax = 0xb101;
    r.d.edi = 0x0;

    if (__dpmi_int(0x1a, &r) != 0) {
        return -1;
    }
	
    if (r.h.ah != 0 || r.d.edx != 0x20494350) {
        return -1;
    }

    return 0;
}

static int pci_find(int vendorID, int deviceID, int index, int *bus, int *device, int *func)
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(r));

    r.x.ax = 0xb102;
    r.x.cx = deviceID;
    r.x.dx = vendorID;
    r.x.si = index;

    if (__dpmi_int(0x1a, &r) != 0) {
        return -1;
    }

    if (r.h.ah == 0) {
        *bus = r.h.bh;
        *device = (r.h.bl >> 3) & 0x1f;
        *func = r.h.bl & 0x03;
    }

    return r.h.ah;
}

static int pci_read_config_dword(int bus, int device, int func, int reg, uint32 *value)
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(r));

    r.x.ax = 0xb10a;
    r.h.bh = bus;
    r.h.bl = (device << 3) + func;
    r.x.di = reg;

    if (__dpmi_int(0x1a, &r) != 0) {
        return -1;
    }
	
    if (r.h.ah == 0) {
        *value = r.d.ecx;
    }

    return r.h.ah;
}

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

static unsigned char read_sid(unsigned char reg)
{
    unsigned char ret;

    outportb(base + 4, ((reg & 0x1f) | 0x20));
    usleep(2);
    outportb(base, 0x20);
    ret = inportb(base);
    outportb(base, 0x80);

    return ret;
}

static void write_sid( unsigned char reg, unsigned char data )
{
    outportb(base + 3, ((reg & 0x1f) << 8) | data);
}

int hardsid_drv_open(void)
{
    unsigned int i;

    base = pci_find_hardsid(0);

    if (base == -1) {
        log_message( LOG_DEFAULT, "Unable to find a HardSID PCI card\n" );
        return -1;
    }

    // Reset the hardsid PCI interface (as per hardsid linux driver)
    outportb(base + 0x00, 0xff);
    outportb(base + 0x02, 0x00);
    usleep(100);
    outportb(base + 0x02, 0x24);

    /* mute all sids */
    for (i = 0; i < 32; i++) {
        write_sid(i, 0);
    }

    log_message(LOG_DEFAULT, "HardSID PCI: opened");

    sidfh = 1; /* ok */

    return 0;
}

int hardsid_drv_close(void)
{
    unsigned int i;

    /* mute all sids */
    for (i = 0; i < 32; i++) {
        write_sid(i, 0);
    }

    log_message(LOG_DEFAULT, "HardSID PCI: closed");

    return 0;
}

/* read value from SIDs */
int hardsid_drv_read(WORD addr, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < 1 && addr < 0x20) {
        /* if addr is from read-only register, perform a real read */
        if (addr >= 0x19 && addr <= 0x1C && sidfh >= 0) {
            return read_sid(addr);
        }
    }
    return 0;
}

/* write value into SID */
void hardsid_drv_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < 1 && addr <= 0x18) {
        /* if the device is opened, write to device */
        if (sidfh >= 0) {
            write_sid(addr, val);
        }
    }
}

void hardsid_drv_set_device(unsigned int chipno, unsigned int device)
{
}

int hardsid_drv_available(void)
{
    if (hs_available) {
        return 1;
    }

    if (hardsid_drv_open() < 0) {
        return 0;
    }
    hardsid_drv_close();
    hs_available = 1;
    return 1;
}

void hardsid_drv_reset(void)
{
}

/* ---------------------------------------------------------------------*/

void hardsid_drv_state_read(int chipno, struct sid_hs_snapshot_state_s *sid_state)
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

void hardsid_drv_state_write(int chipno, struct sid_hs_snapshot_state_s *sid_state)
{
}
#endif

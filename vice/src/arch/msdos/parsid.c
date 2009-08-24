/*
 * parsid.c - PARallel port SID support for MSDOS.
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

/* control register bits */
#define parsid_STROBE   0x01
#define parsid_AUTOFEED 0x02
#define parsid_nINIT    0x04
#define parsid_SELECTIN 0x08
#define parsid_PCD      0x20

static unsigned char parsid_ctrport;
static int parsid_port_address[4];

/* input/output functions */
BYTE parsid_inb(WORD addr)
{
    return inportb(addr);
}

void parsid_outb(WORD addr, BYTE value)
{
    outportb(addr,value);
}

/* pin functions */
static void parsid_chip_select(void)
{
    parsid_ctrport != parsid_STROBE;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_chip_deselect(void)
{
    parsid_ctrport &= ~parsid_STROBE;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_reset_start(void)
{
    parsid_ctrport |= parsid_SELECTIN;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_reset_end(void)
{
    parsid_ctrport &= ~parsid_SELECTIN;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_latch_open(void)
{
    parsid_ctrport &= ~parsid_AUTOFEED;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_latch_lock(void)
{
    parsid_ctrport |= parsid_AUTOFEED;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_RW_write(void)
{
    parsid_ctrport &= ~parsid_nINIT;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_RW_read(void)
{
    parsid_ctrport |= parsid_nINIT;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

/* parallel port direction control */
static void parsid_port_write(void)
{
    parsid_ctrport &= ~parsid_PCD;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_port_read(void)
{
    parsid_ctrport |= parsid_PCD;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_sidwait(void)
{
}

int parsid_check_port(int port)
{
    if (port < 1 || port > 3) {
        return -1;
    }

    if (parsid_port_address[port - 1] == 0) {
        return -1;
    }

    parsid_ctrport = parsid_inb(parsid_port_address[parsid_port] + 2);
    return 0;
}

static int parsid_init(void)
{
    int j;
    unsigned long ptraddr = 0x0408;		/* Base Address: segment is zero*/
    unsigned int address;			/* Address of Port */
    int ports = 0;

    for (j = 0; j < 3; j++) {
        parsid_port_address[j] = _farpeekw(_dos_ds, ptraddr);
        if (parsid_port_address[j] != 0) {
            ports++;
        }
        ptraddr += 2;
    }

    if (ports == 0) {
        return -1;
    } else {
        return 0;
    }
}

void parsid_reset(void)
{
    parsid_RW_write();
    parsid_port_write();
    parsid_chip_select();
    parsid_latch_open();
    parsid_outb(parsid_port_address[parsid_port], 0);
    parsid_reset_start();
    sleep(1);
    parsid_reset_end();
    parsid_latch_lock();
    parsid_chip_deselect();
}

int parsid_open(int port)
{
    if (parsid_init() < 0) {
        return -1;
    }

    if (parsid_check_port(port) < 0) {
        return -1;
    }

    parsid_reset();
    return 0;
}

int parsid_close(void)
{
    if (parsid_port != 0) {
        parsid_reset();
    }
    return 0;
}

int parsid_read(WORD addr, int chipno)
{
    int value;

    parsid_outb(parsid_port_address[parsid_port], addr);
    parsid_latch_open();
    parsid_sidwait();
    parsid_latch_lock();
    parsid_port_read();
    parsid_RW_read();
    parsid_chip_select();
    parsid_sidwait();
    value = parsid_inb(parsid_port_address[parsid_port]);
    parsid_chip_deselect();
    return value;
}

void parsid_store(WORD addr, BYTE outval, int chipno)
{
    parsid_outb(parsid_port_address[parsid_port], addr);
    parsid_latch_open();
    parsid_sidwait();
    parsid_latch_lock();
    parsid_outb(parsid_port_address[parsid_port], outval);
    parsid_chip_select();
    parsid_sidwait();
    parsid_chip_deselect();
}
#endif

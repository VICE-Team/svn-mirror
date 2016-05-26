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

static int parsid_port = 0;
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

void parsid_drv_out_ctr(WORD parsid_ctrport)
{
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

BYTE parsid_drv_in_ctr(void)
{
    return parsid_inb(parsid_port_address[parsid_port] + 2);
}

int parsid_drv_check_port(int port)
{
    if (port < 1 || port > 3) {
        return -1;
    }

    if (parsid_port_address[port - 1] == 0) {
        return -1;
    }

    parsid_port = port - 1;

    return 0;
}

int parsid_drv_init(void)
{
    int j;
    unsigned long ptraddr = 0x0408;		/* Base Address: segment is zero*/
    unsigned int address;				/* Address of Port */
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
    }
    return 0;
}

int parsid_drv_close(void)
{
    return 0;
}

BYTE parsid_drv_in_data(void)
{
    return parsid_inb(parsid_port_address[parsid_port]);
}

void parsid_drv_out_data(BYTE outval)
{
    parsid_outb(parsid_port_address[parsid_port], outval);
}

void parsid_drv_sleep(int amount)
{
    sleep(amount);
}
#endif

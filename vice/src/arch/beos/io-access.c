/*
 * io-access.c - BeOS specific I/O code.
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

#include "log.h"
#include "types.h"

#ifdef __HAIKU__
#include <Drivers.h>
#include <ISA.h>
#include <PCI.h>

#define POKE_DEVICE_FULLNAME "/dev/misc/poke"
#define POKE_SIGNATURE       'wltp'
#else
extern int read_isa_io(int dummy, void *addr, int size);
extern int write_isa_io(int dummy, void *addr, int size, DWORD val);
#endif

#ifdef __HAIKU__
enum {
    POKE_PORT_READ = B_DEVICE_OP_CODES_END + 1,
    POKE_PORT_WRITE,
    POKE_PORT_INDEXED_READ
};

typedef struct {
    uint32 signature;
    uint16 port;
    uint8 size;
    uint32 value;
} port_io_args;

static int poke_driver_fd;
#endif

int io_access_init(void)
{
#ifdef __HAIKU__
    poke_driver_fd = open(POKE_DEVICE_FULLNAME, O_RDWR);
    return (poke_driver_fd < 0) ? -1 : 0;
#else
    return 0;
#endif
}

void io_access_shutdown(void)
{
#ifdef __HAIKU__
    close(poke_driver_fd);
#endif
}

void io_access_store_byte(WORD addr, BYTE value)
{
#ifdef __HAIKU__
    port_io_args args = { POKE_SIGNATURE, addr, 1, value };

    ioctl(poke_driver_fd, POKE_PORT_WRITE, &args, sizeof(args));
#else
    write_isa_io(0, (void *)(DWORD)addr, 1, (DWORD)value);
#endif
}

BYTE io_access_read_byte(WORD addr)
{
#ifdef __HAIKU__
    port_io_args args = { POKE_SIGNATURE, addr, 1, 0 };

    if (ioctl(poke_driver_fd, POKE_PORT_READ, &args, sizeof(args)) < 0) {
        return 0;
    }
    return (BYTE)args.value;
#else
    return (BYTE)read_isa_io(0, (void *)(DWORD)addr, 1);
#endif
}

void io_access_store_long(WORD addr, DWORD value)
{
#ifdef __HAIKU__
    port_io_args args = { POKE_SIGNATURE, addr, 4, value };

    ioctl(poke_driver_fd, POKE_PORT_WRITE, &args, sizeof(args));
#else
    write_isa_io(0, (void *)(DWORD)addr, 4, (DWORD)value);
#endif
}

DWORD io_access_read_long(WORD addr)
{
#ifdef __HAIKU__
    port_io_args args = { POKE_SIGNATURE, addr, 4, 0 };

    if (ioctl(poke_driver_fd, POKE_PORT_READ, &args, sizeof(args)) < 0) {
        return 0;
    }
    return (DWORD)args.value;
#else
    return (DWORD)read_isa_io(0, (void *)(DWORD)addr, 4);
#endif
}
